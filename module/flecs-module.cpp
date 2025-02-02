module;

// Include flecs, but do not include c++ api, while still including things which need to be available for the c++ api to compile
#define FLECS_CPP20_MODULE_HEADER
#include "flecs.h"
#undef FLECS_CPP20_MODULE_HEADER

// Manually include needed std headers
#include <type_traits>
#include <stdio.h>
#include <limits>
#include <string.h>
#include <stdlib.h>
#include <utility>
#include <ctype.h>

export module flecs;

// Disable std includes
#define FLECS_NO_STD_INCLUDE

// Include all symbols tagged with FLECS_API
#undef FLECS_API
#define FLECS_API export

// Namespaces must be marked with export to be included in the module
#define FLECS_API_NAMESPACE export
// Definitions that need to be visible, but aren't actually part of the API
#define FLECS_API_DEPENDENCY export
// Disable static declarations in some places
#define FLECS_STATIC_IN_HEADER

#include "flecs/addons/cpp/flecs.hpp"
