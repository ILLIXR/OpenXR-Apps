// Copyright 2019, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Data frame header.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup xrt_iface
 */

#pragma once

#include "xrt/xrt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * Basic frame data structure - holds a pointer to buffer.
 *
 * @ingroup xrt_iface
 */
struct xrt_frame
{
	struct xrt_reference reference;
	void (*destroy)(struct xrt_frame *);
	void *owner;

	uint32_t width;
	uint32_t height;
	size_t stride;
	size_t size;
	uint8_t *data;

	enum xrt_format format;
	enum xrt_stereo_format stereo_format;

	uint64_t timestamp;
	uint64_t source_timestamp;
	uint64_t source_sequence; //!< sequence id
	uint64_t source_id; //!< Which @ref xrt_fs this frame originated from.
};


/*!
 * A object that is sent frames.
 *
 * @ingroup xrt_iface
 */
struct xrt_frame_sink
{
	/*!
	 * Push a frame into the sink.
	 */
	void (*push_frame)(struct xrt_frame_sink *sink,
	                   struct xrt_frame *frame);
};

/*!
 * A interface object used for destroying a frame graph.
 *
 * @see container_of
 * @ingroup xrt_iface
 */
struct xrt_frame_node
{
	struct xrt_frame_node *next;

	/*!
	 * Called first in when the graph is being destroyed, remove any
	 * references frames and other objects and stop threads.
	 */
	void (*break_apart)(struct xrt_frame_node *node);

	/*!
	 * Do the actual freeing of the objects.
	 */
	void (*destroy)(struct xrt_frame_node *node);
};

/*!
 * Object used to track all sinks and frame producers in a graph.
 *
 * @ingroup xrt_iface
 */
struct xrt_frame_context
{
	struct xrt_frame_node *nodes;
};


/*
 *
 * Inline functions.
 *
 */

/*!
 * Update the reference count on a frame.
 *
 * @ingroup xrt_iface
 */
XRT_MAYBE_UNUSED static void
xrt_frame_reference(struct xrt_frame **dst, struct xrt_frame *src)
{
	struct xrt_frame *old_dst = *dst;

	if (old_dst == src) {
		return;
	}

	if (src) {
		xrt_reference_inc(&src->reference);
	}

	*dst = src;

	if (old_dst) {
		if (xrt_reference_dec(&old_dst->reference)) {
			old_dst->destroy(old_dst);
		}
	}
}

/*!
 * Add a node to a context.
 *
 * @ingroup xrt_iface
 */
XRT_MAYBE_UNUSED static void
xrt_frame_context_add(struct xrt_frame_context *xfctx,
                      struct xrt_frame_node *node)
{
	node->next = xfctx->nodes;
	xfctx->nodes = node;
}

/*!
 * Destroy all child nodes, but free the context itself.
 *
 * @ingroup xrt_iface
 */
XRT_MAYBE_UNUSED static void
xrt_frame_context_destroy_nodes(struct xrt_frame_context *xfctx)
{
	struct xrt_frame_node *next = NULL;
	struct xrt_frame_node *node = xfctx->nodes;

	while (node != NULL) {
		next = node->next;
		node->break_apart(node);
		node = next;
	}

	node = xfctx->nodes;
	while (node != NULL) {
		next = node->next;
		node->destroy(node);
		node = next;
	}

	xfctx->nodes = NULL;
}


#ifdef __cplusplus
}
#endif
