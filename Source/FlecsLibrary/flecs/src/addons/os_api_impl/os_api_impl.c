/**
 * @file addons/os_api_impl/os_api_impl.c
 * @brief Builtin implementation for OS API.
 */

#include "../../private_api.h"

#ifdef FLECS_OS_API_IMPL
#ifdef ECS_TARGET_WINDOWS
#include "windows_impl.inl"
#else
#include "posix_impl.inl"
#endif
#endif
