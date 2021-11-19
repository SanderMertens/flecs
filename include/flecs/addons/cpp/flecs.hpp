/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * Modern, type safe C++11 API
 */

#pragma once

// C++ utilities
#include <type_traits>
#include "utils/util.hpp"

// Forward declarations
namespace flecs 
{

class world;
class world_async_stage;
class snapshot;
class iter;
class filter_iterator;
class world_filter;
class snapshot_filter;

namespace _ 
{
template <typename T, typename U = int>
class cpp_type;

template <typename Func, typename ... Components>
class each_invoker;
} // namespace _
} // namespace flecs

// Types imported from C API
#include "c_types.hpp"

// Mixin forward declarations
#include "mixins/id/decl.hpp"
#include "mixins/entity/decl.hpp"
#include "mixins/component/decl.hpp"
#include "mixins/type/decl.hpp"
#include "mixins/module/decl.hpp"
#include "mixins/term/decl.hpp"
#include "mixins/filter/decl.hpp"
#include "mixins/query/decl.hpp"
#include "mixins/system/decl.hpp"
#include "mixins/pipeline/decl.hpp"
#include "mixins/timer/decl.hpp"
#include "mixins/trigger/decl.hpp"
#include "mixins/observer/decl.hpp"

// Mixins (remove from list to disable)
namespace flecs {
using Mixins = mixin_list<
    id_m,
    entity_m,
    component_m,
    type_m,
    module_m,
    term_m,
    filter_m,
    query_m,
    system_m, 
    pipeline_m,
    timer_m,
    trigger_m,
    observer_m
>;
}

#include "log.hpp"
#include "pair.hpp"
#include "lifecycle_traits.hpp"
#include "world.hpp"

#include "iter.hpp"
#include "invoker.hpp"

#include "ref.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "type.hpp"
#include "snapshot.hpp"
#include "filter_iterator.hpp"

// Mixin implementations
#include "mixins/id/impl.hpp"
#include "mixins/entity/impl.hpp"
#include "mixins/component/impl.hpp"
#include "mixins/type/impl.hpp"
#include "mixins/module/impl.hpp"
#include "mixins/term/impl.hpp"
#include "mixins/filter/impl.hpp"
#include "mixins/query/impl.hpp"
#include "mixins/system/impl.hpp"
#include "mixins/pipeline/impl.hpp"
#include "mixins/timer/impl.hpp"
#include "mixins/trigger/impl.hpp"
#include "mixins/observer/impl.hpp"

#include "impl.hpp"
