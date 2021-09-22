// Copyright 2019, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Macros for generating extension-related tables and code and
 * inspecting Monado's extension support.
 *
 * MOSTLY GENERATED CODE - see below!
 *
 * To add support for a new extension, edit and run generate_oxr_ext_support.py,
 * then run clang-format on this file. Two entire chunks of this file get
 * replaced by that script: comments indicate where they are.
 *
 * @author Ryan Pavlik <ryan.pavlik@collabora.com>
 */

#pragma once

// beginning of GENERATED defines - do not modify - used by scripts

/*
 * XR_EXT_debug_utils
 */
#if defined(XR_EXT_debug_utils)
#define OXR_HAVE_EXT_debug_utils
#define OXR_EXTENSION_SUPPORT_EXT_debug_utils(_)                               \
	_(EXT_debug_utils, EXT_DEBUG_UTILS)
#else
#define OXR_EXTENSION_SUPPORT_EXT_debug_utils(_)
#endif


/*
 * XR_KHR_convert_timespec_time
 */
#if defined(XR_KHR_convert_timespec_time) && defined(XR_USE_TIMESPEC)
#define OXR_HAVE_KHR_convert_timespec_time
#define OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_)                     \
	_(KHR_convert_timespec_time, KHR_CONVERT_TIMESPEC_TIME)
#else
#define OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_)
#endif


/*
 * XR_KHR_opengl_enable
 */
#if defined(XR_KHR_opengl_enable) && defined(XR_USE_GRAPHICS_API_OPENGL)
#define OXR_HAVE_KHR_opengl_enable
#define OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_)                             \
	_(KHR_opengl_enable, KHR_OPENGL_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_)
#endif


/*
 * XR_KHR_opengl_es_enable
 */
#if defined(XR_KHR_opengl_es_enable) && defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#define OXR_HAVE_KHR_opengl_es_enable
#define OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_)                          \
	_(KHR_opengl_es_enable, KHR_OPENGL_ES_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_)
#endif


/*
 * XR_KHR_vulkan_enable
 */
#if defined(XR_KHR_vulkan_enable) && defined(XR_USE_GRAPHICS_API_VULKAN)
#define OXR_HAVE_KHR_vulkan_enable
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_)                             \
	_(KHR_vulkan_enable, KHR_VULKAN_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_)
#endif


/*
 * XR_MND_egl_enable
 */
#if defined(XR_MND_egl_enable) && defined(XR_USE_PLATFORM_EGL) &&              \
    defined(XR_USE_GRAPHICS_API_OPENGL)
#define OXR_HAVE_MND_egl_enable
#define OXR_EXTENSION_SUPPORT_MND_egl_enable(_)                                \
	_(MND_egl_enable, MND_EGL_ENABLE)
#else
#define OXR_EXTENSION_SUPPORT_MND_egl_enable(_)
#endif


/*
 * XR_MND_headless
 */
#if defined(XR_MND_headless)
#define OXR_HAVE_MND_headless
#define OXR_EXTENSION_SUPPORT_MND_headless(_) _(MND_headless, MND_HEADLESS)
#else
#define OXR_EXTENSION_SUPPORT_MND_headless(_)
#endif

// end of GENERATED per-extension defines - do not modify - used by scripts

/*!
 * Call this, passing a macro taking two parameters, to
 * generate tables, code, etc. related to OpenXR extensions.
 * Upon including invoking OXR_EXTENSION_SUPPORT_GENERATE() with some
 * MY_HANDLE_EXTENSION(mixed_case, all_caps), MY_HANDLE_EXTENSION will be called
 * for each extension implemented in Monado and supported in this build:
 *
 * - The first parameter is the name of the extension without the leading XR_
 *   prefix: e.g. `KHR_opengl_enable`
 * - The second parameter is the same text as the first, but in all uppercase,
 *   since this transform cannot be done in the C preprocessor, and some
 *   extension-related entities use this instead of the exact extension name.
 *
 * Implementation note: This macro calls another macro for each extension: that
 * macro is either defined to call your provided macro, or defined to nothing,
 * depending on if the extension is supported in this build.
 *
 * @note Do not edit anything between `clang-format off` and `clang-format on` -
 * it will be replaced next time this file is generated!
 */
// clang-format off
#define OXR_EXTENSION_SUPPORT_GENERATE(_) \
    OXR_EXTENSION_SUPPORT_EXT_debug_utils(_) \
    OXR_EXTENSION_SUPPORT_KHR_convert_timespec_time(_) \
    OXR_EXTENSION_SUPPORT_KHR_opengl_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_opengl_es_enable(_) \
    OXR_EXTENSION_SUPPORT_KHR_vulkan_enable(_) \
    OXR_EXTENSION_SUPPORT_MND_egl_enable(_) \
    OXR_EXTENSION_SUPPORT_MND_headless(_)
// clang-format on
