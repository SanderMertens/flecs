module;

// TODO: find out why
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 0

#define FLECS_CPP20_MODULE_HEADER
#include "flecs.h"
#undef FLECS_CPP20_MODULE_HEADER

#include <type_traits>
#include <stdio.h>
#include <limits>
#include <string.h>
#include <stdlib.h>
#include <utility>
#include <ctype.h>

export module flecs;

#define FLECS_CUSTOM_STD_INCLUDE
#define FLECS_CPP20_MODULE_BODY
#undef FLECS_API
#define FLECS_API export
#define FLECS_API_STRUCT export
#define FLECS_API_GLOBAL export

#include "flecs/addons/cpp/flecs.hpp"
