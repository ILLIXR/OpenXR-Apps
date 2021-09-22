#!/bin/bash

. /etc/os-release
sudo apt install -y software-properties-common curl
curl https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository -u -y "deb https://apt.kitware.com/ubuntu/ ${UBUNTU_CODENAME} main"
sudo add-apt-repository -u -y ppa:graphics-drivers/ppa
sudo add-apt-repository -u -y ppa:deadsnakes/ppa
sudo apt-get install -y \
	 git clang-10 make cmake libc++-dev libc++abi-dev \
	 libsqlite3-dev libeigen3-dev libboost-all-dev libatlas-base-dev libsuitesparse-dev libblas-dev \
	 glslang-tools libsdl2-dev libglu1-mesa-dev mesa-common-dev freeglut3-dev libglew-dev glew-utils libglfw3-dev \
	 libusb-dev libusb-1.0 libudev-dev libv4l-dev libhidapi-dev \
	 build-essential libx11-xcb-dev libxcb-glx0-dev libxcb-randr0-dev libxrandr-dev libxkbcommon-dev libwayland-dev \
	 libgtest-dev pkg-config libgtk2.0-dev wget

