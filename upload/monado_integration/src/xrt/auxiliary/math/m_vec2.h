// Copyright 2019, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  C vec2 math library.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 *
 * @see xrt_vec2
 * @ingroup aux_math
 */

#pragma once

#include "xrt/xrt_defines.h"

#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif


static inline struct xrt_vec2
m_vec2_mul(struct xrt_vec2 l, struct xrt_vec2 r)
{
	struct xrt_vec2 ret = {l.x * r.x, l.y * r.y};
	return ret;
}

static inline struct xrt_vec2
m_vec2_mul_scalar(struct xrt_vec2 l, float r)
{
	struct xrt_vec2 ret = {l.x * r, l.y * r};
	return ret;
}

static inline struct xrt_vec2
m_vec2_add(struct xrt_vec2 l, struct xrt_vec2 r)
{
	struct xrt_vec2 ret = {l.x + r.x, l.y + r.y};
	return ret;
}

static inline struct xrt_vec2
m_vec2_sub(struct xrt_vec2 l, struct xrt_vec2 r)
{
	struct xrt_vec2 ret = {l.x - r.x, l.y - r.y};
	return ret;
}

static inline struct xrt_vec2
m_vec2_div(struct xrt_vec2 l, struct xrt_vec2 r)
{
	struct xrt_vec2 ret = {l.x / r.x, l.y / r.y};
	return ret;
}

static inline struct xrt_vec2
m_vec2_div_scalar(struct xrt_vec2 l, float r)
{
	struct xrt_vec2 ret = {l.x / r, l.y / r};
	return ret;
}

static inline float
m_vec2_len(struct xrt_vec2 l)
{
	return sqrtf(l.x * l.x + l.y * l.y);
}


#ifdef __cplusplus
}
#endif
