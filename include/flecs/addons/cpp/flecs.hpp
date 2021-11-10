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
class query_base;

template<typename ... Components>
class filter;

template<typename ... Components>
class query;

template<typename ... Components>
class trigger;

template<typename ... Components>
class observer;

template <typename ... Components>
class filter_builder;

template <typename ... Components>
class query_builder;

template <typename ... Components>
class trigger_builder;

template <typename ... Components>
class observer_builder;

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
#include "addons/system/decl.hpp"
#include "addons/pipeline/decl.hpp"

// Mixins
namespace flecs {
using Mixins = mixin_list<
    system_m, 
    pipeline_m
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
#include "module.hpp"
#include "term.hpp"
#include "filter.hpp"
#include "snapshot.hpp"
#include "filter_iterator.hpp"
#include "query.hpp"
#include "trigger.hpp"
#include "observer.hpp"
#include "impl.hpp"

// Addon implementations
#include "addons/system/impl.hpp"
#include "addons/pipeline/impl.hpp"
