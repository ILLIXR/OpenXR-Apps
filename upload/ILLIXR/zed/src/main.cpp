// ZED includes
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <zed_opencv.hpp>

//ILLIXR includes
#include "common/threadloop.hpp"
#include "common/switchboard.hpp"
#include "common/data_format.hpp"

using namespace ILLIXR;

cv::Mat slMat2cvMat(Mat& input);

const record_header __imu_cam_record {"imu_cam", {
    {"iteration_no", typeid(std::size_t)},
    {"has_camera", typeid(bool)},
}};

typedef struct {
    cv::Mat* img0;
    cv::Mat* img1;
    std::size_t serial_no;
} cam_type;

std::shared_ptr<Camera> start_camera() {
    std::shared_ptr<Camera> zedm = std::make_shared<Camera>();

    // Cam setup
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION::VGA;
    init_params.coordinate_units = UNIT::METER;
    init_params.coordinate_system = COORDINATE_SYSTEM::RIGHT_HANDED_Z_UP_X_FWD; // Coordinate system used in ROS
    init_params.camera_fps = 15;
    init_params.depth_mode = DEPTH_MODE::NONE;
    // Open the camera
    ERROR_CODE err = zedm->open(init_params);
    if (err != ERROR_CODE::SUCCESS) {
        printf("%s\n", toString(err).c_str());
        zedm->close();
        abort();
    }

    // This is 4% of camera frame time, not 4 ms
    zedm->setCameraSettings(VIDEO_SETTINGS::EXPOSURE, 4);

    return zedm;
}

class zed_camera_thread : public threadloop {
public:
    zed_camera_thread(std::string name_, phonebook* pb_, std::shared_ptr<Camera> zedm_)
    : threadloop{name_, pb_}
    , sb{pb->lookup_impl<switchboard>()}
    , _m_cam_type{sb->publish<cam_type>("cam_type")}
    , zedm{zedm_}
    , image_size{zedm->getCameraInformation().camera_configuration.resolution}
    {
        // Image setup
        imageL_zed.alloc(image_size.width, image_size.height, MAT_TYPE::U8_C4, MEM::CPU);
        imageR_zed.alloc(image_size.width, image_size.height, MAT_TYPE::U8_C4, MEM::CPU);

        imageL_ocv = slMat2cvMat(imageL_zed);
        imageR_ocv = slMat2cvMat(imageR_zed);
    }

private:
    const std::shared_ptr<switchboard> sb;
    std::unique_ptr<writer<cam_type>> _m_cam_type;
    std::shared_ptr<Camera> zedm;
    Resolution image_size;
    RuntimeParameters runtime_parameters;
    std::size_t serial_no {0};

    Mat imageL_zed;
    Mat imageR_zed;

    cv::Mat imageL_ocv;
    cv::Mat imageR_ocv;

protected:
    virtual skip_option _p_should_skip() override {
        if (zedm->grab(runtime_parameters) == ERROR_CODE::SUCCESS) {
            return skip_option::run;
        } else {
            return skip_option::skip_and_spin;
        }
    }

    virtual void _p_one_iteration() override {
        // Retrieve images
        zedm->retrieveImage(imageL_zed, VIEW::LEFT, MEM::CPU, image_size);
        zedm->retrieveImage(imageR_zed, VIEW::RIGHT, MEM::CPU, image_size);

        auto start_cpu_time  = thread_cpu_time();
        auto start_wall_time = std::chrono::high_resolution_clock::now();

        _m_cam_type->put(new cam_type{
            // Make a copy, so that we don't have race
            new cv::Mat{imageL_ocv},
            new cv::Mat{imageR_ocv},
            iteration_no,
        });
    }
};

class zed_imu_thread : public threadloop {
public:

    virtual void stop() override {
        camera_thread_.stop();
        threadloop::stop();
    }

    zed_imu_thread(std::string name_, phonebook* pb_)
        : threadloop{name_, pb_}
        , sb{pb->lookup_impl<switchboard>()}
        , _m_imu_cam{sb->publish<imu_cam_type>("imu_cam")}
        , zedm{start_camera()}
        , camera_thread_{"zed_camera_thread", pb_, zedm}
        , _m_cam_type{sb->subscribe_latest<cam_type>("cam_type")}
        , _m_imu_integrator{sb->publish<imu_integrator_seq>("imu_integrator_seq")}
        , it_log{record_logger_}
    {
        camera_thread_.start();
    }

    // destructor
    virtual ~zed_imu_thread() override {
        zedm->close();
    }

protected:
    virtual skip_option _p_should_skip() override {
        std::this_thread::sleep_for(std::chrono::milliseconds{2});
        zedm->getSensorsData(sensors_data, TIME_REFERENCE::CURRENT);
        if (sensors_data.imu.timestamp > last_imu_ts) {
            return skip_option::run;
        } else {
            return skip_option::skip_and_yield;
        }
    }

    virtual void _p_one_iteration() override {
        // std::cout << "IMU Rate: " << sensors_data.imu.effective_rate << "\n" << std::endl;

        // Time as ullong (nanoseconds)
        imu_time = static_cast<ullong>(sensors_data.imu.timestamp.getNanoseconds());

        // Time as time_point
        using time_point = std::chrono::system_clock::time_point;
        time_type imu_time_point{std::chrono::duration_cast<time_point::duration>(std::chrono::nanoseconds(sensors_data.imu.timestamp.getNanoseconds()))};

        // Linear Acceleration and Angular Velocity (av converted from deg/s to rad/s)
        la = {sensors_data.imu.linear_acceleration_uncalibrated.x , sensors_data.imu.linear_acceleration_uncalibrated.y, sensors_data.imu.linear_acceleration_uncalibrated.z };
        av = {sensors_data.imu.angular_velocity_uncalibrated.x  * (M_PI/180), sensors_data.imu.angular_velocity_uncalibrated.y * (M_PI/180), sensors_data.imu.angular_velocity_uncalibrated.z * (M_PI/180)};

        std::optional<cv::Mat*> img0 = std::nullopt;
        std::optional<cv::Mat*> img1 = std::nullopt;

        const cam_type* c = _m_cam_type->get_latest_ro();
        if (c && c->serial_no != last_serial_no) {
            last_serial_no = c->serial_no;
            img0 = c->img0;
            img1 = c->img1;
        }

        it_log.log(record{__imu_cam_record, {
            {iteration_no},
            {bool(img0)},
        }});

        _m_imu_cam->put(new imu_cam_type {
            imu_time_point,
            av,
            la,
            img0,
            img1,
            imu_time,
        });

        _m_imu_integrator->put(new imu_integrator_seq {
            .seq = static_cast<int>(++_imu_integrator_seq)
        });

        last_imu_ts = sensors_data.imu.timestamp;
    }

private:
    std::shared_ptr<Camera> zedm;
    zed_camera_thread camera_thread_;

    const std::shared_ptr<switchboard> sb;
    std::unique_ptr<writer<imu_cam_type>> _m_imu_cam;
    std::unique_ptr<reader_latest<cam_type>> _m_cam_type;
    std::unique_ptr<writer<imu_integrator_seq>> _m_imu_integrator;

    // IMU
    SensorsData sensors_data;
    Timestamp last_imu_ts = 0;
    Eigen::Vector3f la;
    Eigen::Vector3f av;

    // Timestamps
    time_type t;
    ullong imu_time;

    std::size_t last_serial_no {0};
    long long _imu_integrator_seq{0};

    // Logger
    record_coalescer it_log;
};

// This line makes the plugin importable by Spindle
PLUGIN_MAIN(zed_imu_thread);

int main(int argc, char **argv) { return 0; }
