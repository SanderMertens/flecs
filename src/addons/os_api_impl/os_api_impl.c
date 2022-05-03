#include "../../private_api.h"

#ifdef FLECS_OS_API_IMPL
#ifdef ECS_TARGET_WINDOWS
#include "windows_impl.inl"
#else
#include "posix_impl.inl"
#endif
#endif
