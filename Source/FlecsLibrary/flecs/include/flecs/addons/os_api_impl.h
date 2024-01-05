/**
 * @file addons/os_api_impl.h
 * @brief Default OS API implementation.
 */

#ifdef FLECS_OS_API_IMPL

/**
 * @defgroup c_addons_os_api_impl OS API Implementation
 * @ingroup c_addons
 * Default implementation for OS API interface.
 *
 * @{
 */

#ifndef FLECS_OS_API_IMPL_H
#define FLECS_OS_API_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
void ecs_set_os_api_impl(void);

#ifdef __cplusplus
}
#endif

#endif // FLECS_OS_API_IMPL_H

/** @} */

#endif // FLECS_OS_API_IMPL
