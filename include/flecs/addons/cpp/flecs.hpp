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
class id;
class entity;
class entity_view;
class type;
class iter;
class term;
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

// Addon forward declarations
#include "mixins/module/decl.hpp"
#include "mixins/filter/decl.hpp"
#include "mixins/query/decl.hpp"
#include "mixins/system/decl.hpp"
#include "mixins/pipeline/decl.hpp"
#include "mixins/timer/decl.hpp"
#include "mixins/trigger/decl.hpp"
#include "mixins/observer/decl.hpp"

// Mixins
namespace flecs {
using Mixins = mixin_list<
    module_m,
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
#include "iter.hpp"
#include "world.hpp"
#include "id.hpp"
#include "entity_view.hpp"
#include "ref.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "invoker.hpp"
#include "type.hpp"
#include "term.hpp"
#include "snapshot.hpp"
#include "filter_iterator.hpp"
#include "impl.hpp"

// Addon implementations
#include "mixins/module/impl.hpp"
#include "mixins/filter/impl.hpp"
#include "mixins/query/impl.hpp"
#include "mixins/system/impl.hpp"
#include "mixins/pipeline/impl.hpp"
#include "mixins/timer/impl.hpp"
#include "mixins/trigger/impl.hpp"
#include "mixins/observer/impl.hpp"
