// Copyright 2020, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Interface to RealSense devices.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup drv_rs
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * @defgroup drv_rs North Star Driver
 * @ingroup drv
 *
 * @brief Driver for the North Star HMD.
 */

/*!
 * Create a RelaseSense 6DOF tracker device.
 *
 * @ingroup drv_rs
 */
struct xrt_device *
rs_6dof_create(void);

/*!
 * @dir drivers/realsense
 *
 * @brief @ref drv_rs files.
 */


#ifdef __cplusplus
}
#endif
