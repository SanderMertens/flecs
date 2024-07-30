/**
 * @file addons/cpp/flecs.hpp
 * @brief Flecs C++11 API.
 */

#pragma once

// STL includes
#include <type_traits>

/**
 * @defgroup cpp C++ API
 * @{
 */

namespace flecs
{

struct world;
struct world_async_stage;
struct iter;
struct entity_view;
struct entity;
struct type;
struct table;
struct table_range;
struct untyped_component;

template <typename T>
struct component;

template <typename T>
struct ref;

namespace _
{
template <typename T, typename U = int>
struct type;

template <typename Func, typename ... Components>
struct each_delegate;

} // namespace _
} // namespace flecs

// Types imported from C API
#include "c_types.hpp"

// C++ utilities
#include "utils/utils.hpp"

// Mixin forward declarations
#include "mixins/id/decl.hpp"
#include "mixins/term/decl.hpp"
#include "mixins/query/decl.hpp"
#include "mixins/event/decl.hpp"
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
#ifdef FLECS_DOC
#include "mixins/doc/decl.hpp"
#endif
#ifdef FLECS_REST
#include "mixins/rest/decl.hpp"
#endif
#ifdef FLECS_META
#include "mixins/meta/decl.hpp"
#endif
#ifdef FLECS_UNITS
#include "mixins/units/decl.hpp"
#endif
#ifdef FLECS_STATS
#include "mixins/stats/decl.hpp"
#endif
#ifdef FLECS_METRICS
#include "mixins/metrics/decl.hpp"
#endif
#ifdef FLECS_ALERTS
#include "mixins/alerts/decl.hpp"
#endif
#ifdef FLECS_JSON
#include "mixins/json/decl.hpp"
#endif
#ifdef FLECS_APP
#include "mixins/app/decl.hpp"
#endif
#ifdef FLECS_SCRIPT
#include "mixins/script/decl.hpp"
#endif

#include "log.hpp"
#include "pair.hpp"
#include "lifecycle_traits.hpp"
#include "world.hpp"
#include "field.hpp"
#include "iter.hpp"
#include "entity.hpp"
#include "delegate.hpp"
#include "component.hpp"
#include "ref.hpp"
#include "type.hpp"
#include "table.hpp"
#include "utils/iterable.hpp"

// Mixin implementations
#include "mixins/id/impl.hpp"
#include "mixins/entity/impl.hpp"
#include "mixins/component/impl.hpp"
#include "mixins/term/impl.hpp"
#include "mixins/query/impl.hpp"
#include "mixins/observer/impl.hpp"
#include "mixins/event/impl.hpp"
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
#ifdef FLECS_DOC
#include "mixins/doc/impl.hpp"
#endif
#ifdef FLECS_DOC
#include "mixins/doc/impl.hpp"
#endif
#ifdef FLECS_REST
#include "mixins/rest/impl.hpp"
#endif
#ifdef FLECS_META
#include "mixins/meta/impl.hpp"
#endif
#ifdef FLECS_UNITS
#include "mixins/units/impl.hpp"
#endif
#ifdef FLECS_STATS
#include "mixins/stats/impl.hpp"
#endif
#ifdef FLECS_METRICS
#include "mixins/metrics/impl.hpp"
#endif
#ifdef FLECS_ALERTS
#include "mixins/alerts/impl.hpp"
#endif
#ifdef FLECS_SCRIPT
#include "mixins/script/impl.hpp"
#endif

#include "impl/field.hpp"
#include "impl/iter.hpp"
#include "impl/world.hpp"

/**
 * @defgroup cpp_core Core
 * Core ECS functionality (entities, storage, queries)
 *
 * @{
 * @}
 */

/**
 * @defgroup cpp_addons Addons
 * C++ APIs for addons.
 *
 * @{
 * @}
 */

/** @} */
