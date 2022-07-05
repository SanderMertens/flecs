/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * Modern C++11 API
 */

#pragma once

// STL includes
#include <type_traits>

// Forward declarations
namespace flecs 
{

struct world;
struct world_async_stage;
struct iter;
struct entity_view;
struct entity;
struct type;
struct table;
struct untyped_component;

template <typename T>
struct component;

namespace _ 
{
template <typename T, typename U = int>
struct cpp_type;

template <typename Func, typename ... Components>
struct each_invoker;

} // namespace _
} // namespace flecs

// Types imported from C API
#include "c_types.hpp"

// C++ utilities
#include "utils/utils.hpp"

// Mixin forward declarations
#include "mixins/id/decl.hpp"
#include "mixins/term/decl.hpp"
#include "mixins/filter/decl.hpp"
#include "mixins/event/decl.hpp"
#include "mixins/query/decl.hpp"
#include "mixins/observer/decl.hpp"
#ifdef FLECS_SYSTEM
#include "mixins/system/decl.hpp"
#endif
#ifdef FLECS_PIPELINE
#include "mixins/pipeline/decl.hpp"
#endif
#ifdef FLECS_TIMER
#include "mixins/timer/decl.hpp"
#endif
#ifdef FLECS_SNAPSHOT
#include "mixins/snapshot/decl.hpp"
#endif
#ifdef FLECS_DOC
#include "mixins/doc/decl.hpp"
#endif
#ifdef FLECS_REST
#include "mixins/rest/decl.hpp"
#endif
#ifdef FLECS_RULES
#include "mixins/rule/decl.hpp"
#endif
#ifdef FLECS_META
#include "mixins/meta/decl.hpp"
#endif
#ifdef FLECS_UNITS
#include "mixins/units/decl.hpp"
#endif
#ifdef FLECS_MONITOR
#include "mixins/monitor/decl.hpp"
#endif
#ifdef FLECS_JSON
#include "mixins/json/decl.hpp"
#endif
#ifdef FLECS_APP
#include "mixins/app/decl.hpp"
#endif

#include "log.hpp"
#include "pair.hpp"
#include "lifecycle_traits.hpp"
#include "world.hpp"
#include "iter.hpp"
#include "ref.hpp"
#include "entity.hpp"
#include "invoker.hpp"
#include "utils/iterable.hpp"
#include "component.hpp"
#include "type.hpp"
#include "table.hpp"

// Mixin implementations
#include "mixins/id/impl.hpp"
#include "mixins/entity/impl.hpp"
#include "mixins/component/impl.hpp"
#include "mixins/term/impl.hpp"
#include "mixins/filter/impl.hpp"
#include "mixins/event/impl.hpp"
#include "mixins/query/impl.hpp"
#include "mixins/observer/impl.hpp"
#include "mixins/enum/impl.hpp"
#ifdef FLECS_MODULE
#include "mixins/module/impl.hpp"
#endif
#ifdef FLECS_SYSTEM
#include "mixins/system/impl.hpp"
#endif
#ifdef FLECS_PIPELINE
#include "mixins/pipeline/impl.hpp"
#endif
#ifdef FLECS_TIMER
#include "mixins/timer/impl.hpp"
#endif
#ifdef FLECS_SNAPSHOT
#include "mixins/snapshot/impl.hpp"
#endif
#ifdef FLECS_DOC
#include "mixins/doc/impl.hpp"
#endif
#ifdef FLECS_DOC
#include "mixins/doc/impl.hpp"
#endif
#ifdef FLECS_REST
#include "mixins/rest/impl.hpp"
#endif
#ifdef FLECS_RULES
#include "mixins/rule/impl.hpp"
#endif
#ifdef FLECS_META
#include "mixins/meta/impl.hpp"
#endif
#ifdef FLECS_UNITS
#include "mixins/units/impl.hpp"
#endif
#ifdef FLECS_MONITOR
#include "mixins/monitor/impl.hpp"
#endif

#include "impl.hpp"
