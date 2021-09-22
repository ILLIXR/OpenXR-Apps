// Copyright 2018-2019, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Logging functions.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup oxr_main
 */

#include <stdio.h>
#include <stdarg.h>

#include "xrt/xrt_compiler.h"
#include "util/u_misc.h"
#include "util/u_debug.h"

#include "oxr_objects.h"
#include "oxr_logger.h"

#include "openxr_includes/openxr_reflection.h"


DEBUG_GET_ONCE_BOOL_OPTION(entrypoints, "OXR_DEBUG_ENTRYPOINTS", false)
DEBUG_GET_ONCE_BOOL_OPTION(break_on_error, "OXR_BREAK_ON_ERROR", false)

static const char *
oxr_result_to_string(XrResult result);

void
oxr_log_init(struct oxr_logger *logger, const char *api_func_name)
{
	if (debug_get_bool_option_entrypoints()) {
		fprintf(stderr, "%s\n", api_func_name);
	}

	logger->inst = NULL;
	logger->api_func_name = api_func_name;
}

void
oxr_log_set_instance(struct oxr_logger *logger, struct oxr_instance *inst)
{
	logger->inst = inst;
}

void
oxr_log(struct oxr_logger *logger, const char *fmt, ...)
{
	if (logger->api_func_name != NULL) {
		fprintf(stderr, " in %s", logger->api_func_name);
	}

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
}

void
oxr_warn(struct oxr_logger *logger, const char *fmt, ...)
{
	if (logger->api_func_name != NULL) {
		fprintf(stderr, "%s WARNING: ", logger->api_func_name);
	} else {
		fprintf(stderr, "WARNING: ");
	}

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
}

XrResult
oxr_error(struct oxr_logger *logger, XrResult result, const char *fmt, ...)
{
	if (debug_get_bool_option_entrypoints()) {
		fprintf(stderr, "\t");
	}

	fprintf(stderr, "%s", oxr_result_to_string(result));

	if (logger->api_func_name != NULL) {
		fprintf(stderr, " in %s", logger->api_func_name);
	}

	fprintf(stderr, ": ");
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
	if (debug_get_bool_option_break_on_error() &&
	    result != XR_ERROR_FUNCTION_UNSUPPORTED) {
		/// Trigger a debugger breakpoint.
		XRT_DEBUGBREAK();
	}

	return result;
}

static const char *
oxr_result_to_string(XrResult result)
{
	// clang-format off
	switch (result) {

#define ENTRY(NAME, VALUE) \
	case VALUE: return #NAME;
	XR_LIST_ENUM_XrResult(ENTRY)
#undef ENTRY

	default: return "<UNKNOWN>";
	}
	// clang-format on
}


/*
 *
 * Sink logger.
 *
 */

static void
oxr_slog_ensure(struct oxr_sink_logger *slog, size_t extra)
{
	while (slog->store_size < extra + slog->length) {
		slog->store_size += 1024;
	}

	U_ARRAY_REALLOC_OR_FREE(slog->store, char, slog->store_size);
}

static void
slog_free_store(struct oxr_sink_logger *slog)
{
	free(slog->store);
	slog->length = 0;
	slog->store_size = 0;
}

void
oxr_slog(struct oxr_sink_logger *slog, const char *fmt, ...)
{
	va_list args;

	int ret = 0;

	va_start(args, fmt);
	ret = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	if (ret <= 0) {
		return;
	}

	oxr_slog_ensure(slog, ret + 1);

	va_start(args, fmt);
	ret = vsnprintf(slog->store + slog->length,
	                slog->store_size - slog->length, fmt, args);
	va_end(args);

	slog->length += ret;
}

void
oxr_slog_abort(struct oxr_sink_logger *slog)
{
	slog_free_store(slog);
}

void
oxr_log_slog(struct oxr_logger *log, struct oxr_sink_logger *slog)
{
	oxr_log(log, "%s", slog->store);
	slog_free_store(slog);
}

void
oxr_warn_slog(struct oxr_logger *log, struct oxr_sink_logger *slog)
{
	oxr_warn(log, "%s", slog->store);
	slog_free_store(slog);
}

XrResult
oxr_error_slog(struct oxr_logger *log,
               XrResult res,
               struct oxr_sink_logger *slog)
{
	oxr_error(log, res, "%s", slog->store);
	slog_free_store(slog);
	return res;
}
