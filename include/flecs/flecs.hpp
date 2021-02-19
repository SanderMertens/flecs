/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * This is a C++11 wrapper around the Flecs C API.
 */

#pragma once

#include <string>
#include <sstream>
#include <array>
#include <functional>
#include <iostream>

// Macros so that C++ new calls can allocate using ecs_os_api memory allocation functions
// Rationale:
//  - Using macros here instead of a templated function bc clients might override ecs_os_malloc
//    to contain extra debug info like source tracking location. Using a template function
//    in that scenario would collapse all source location into said function vs. the
//    actual call site
//  - FLECS_PLACEMENT_NEW(): exists to remove any naked new calls/make it easy to identify any regressions
//    by grepping for new/delete
#define FLECS_PLACEMENT_NEW(_ptr, _type)  ::new(flecs::_::placement_new_tag, _ptr) _type
#define FLECS_NEW(_type)                  FLECS_PLACEMENT_NEW(ecs_os_malloc(sizeof(_type)), _type)
#define FLECS_DELETE(_ptr)          \
  do {                              \
    if (_ptr) {                     \
      flecs::_::destruct_obj(_ptr); \
      ecs_os_free(_ptr);            \
    }                               \
  } while (false)

namespace flecs {
namespace _
{
// Dummy Placement new tag to disambiguate from any other operator new overrides
struct placement_new_tag_t{};
constexpr placement_new_tag_t placement_new_tag{};
template<class Ty> inline void destruct_obj(Ty* _ptr) { _ptr->~Ty(); }
}
}

inline void* operator new(size_t,   flecs::_::placement_new_tag_t, void* _ptr) noexcept { return _ptr; }
inline void  operator delete(void*, flecs::_::placement_new_tag_t, void*)      noexcept {              }

namespace flecs {

////////////////////////////////////////////////////////////////////////////////
//// Forward declarations and types
////////////////////////////////////////////////////////////////////////////////

using world_t = ecs_world_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using snapshot_t = ecs_snapshot_t;
using filter_t = ecs_filter_t;
using query_t = ecs_query_t;

class world;
class snapshot;
class entity;
class type;
class iter;
class filter;
class filter_iterator;
class child_iterator;
class world_filter;
class snapshot_filter;

template<typename ... Components>
class query_iterator;

template<typename ... Components>
class query;

template<typename ... Components>
class system;

enum match_kind {
    MatchAll = EcsMatchAll,
    MatchAny = EcsMatchAny,
    MatchExact = EcsMatchExact
};

namespace _
{
template <typename T>
class component_info;

template <typename ...Components>
bool pack_args_to_string(
    world_t *world, 
    std::stringstream& str, 
    bool is_each = false);
}

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using ComponentLifecycle = EcsComponentLifecycle;
using Trigger = EcsTrigger;
using Type = EcsType;
using Name = EcsName;
using Timer = EcsTimer;
using RateFilter = EcsRateFilter;
using TickSource = EcsTickSource;
using SignatureExpr = EcsSignatureExpr;
using Signature = EcsSignature;
using Query = EcsQuery;
using ViewAction = EcsIterAction;
using Context = EcsContext;

/* Builtin tag ids */
static const ecs_entity_t Module = EcsModule;
static const ecs_entity_t Prefab = EcsPrefab;
static const ecs_entity_t Hidden = EcsHidden;
static const ecs_entity_t Disabled = EcsDisabled;
static const ecs_entity_t DisabledIntern = EcsDisabledIntern;
static const ecs_entity_t Inactive = EcsInactive;
static const ecs_entity_t OnDemand = EcsOnDemand;
static const ecs_entity_t Monitor = EcsMonitor;
static const ecs_entity_t Pipeline = EcsPipeline;

/* Trigger tags */
static const ecs_entity_t OnAdd = EcsOnAdd;
static const ecs_entity_t OnRemove = EcsOnRemove;
static const ecs_entity_t OnSet = EcsOnSet;

/* Builtin pipeline tags */
static const ecs_entity_t PreFrame = EcsPreFrame;
static const ecs_entity_t OnLoad = EcsOnLoad;
static const ecs_entity_t PostLoad = EcsPostLoad;
static const ecs_entity_t PreUpdate = EcsPreUpdate;
static const ecs_entity_t OnUpdate = EcsOnUpdate;
static const ecs_entity_t OnValidate = EcsOnValidate;
static const ecs_entity_t PostUpdate = EcsPostUpdate;
static const ecs_entity_t PreStore = EcsPreStore;
static const ecs_entity_t OnStore = EcsOnStore;
static const ecs_entity_t PostFrame = EcsPostFrame;

/** Builtin entity ids */
static const ecs_entity_t World = EcsWorld;
static const ecs_entity_t Singleton = EcsSingleton;

/** Builtin roles */
static const ecs_entity_t Childof = ECS_CHILDOF;
static const ecs_entity_t Instanceof = ECS_INSTANCEOF;
static const ecs_entity_t Trait = ECS_TRAIT;
static const ecs_entity_t Switch = ECS_SWITCH;
static const ecs_entity_t Case = ECS_CASE;
static const ecs_entity_t Owned = ECS_OWNED;

////////////////////////////////////////////////////////////////////////////////

/** Unsafe wrapper class around a column.
 * This class can be used when a system does not know the type of a column at
 * compile time.
 */
class unsafe_column {
public:
    unsafe_column(void* array, size_t size, size_t count, bool is_shared = false)
        : m_array(array)
        , m_size(size)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    void* operator[](size_t index) {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!m_is_shared, ECS_INVALID_PARAMETER, NULL);
        return ECS_OFFSET(m_array, m_size * index);
    }

    /** Return whether component is set.
     * If the column is optional, this method may return false.
     * 
     * @return True if component is set, false if component is not set.
     */
    bool is_set() const {
        return m_array != nullptr;
    }

    /** Return whether component is shared.
     * If the column is shared, this method returns true.
     * 
     * @return True if component is shared, false if component is owned.
     */
    bool is_shared() const {
        return m_is_shared;
    }

protected:
    void* m_array;
    size_t m_size;
    size_t m_count;
    bool m_is_shared;        
};

/** Wrapper class around a column.
 * 
 * @tparam T component type of the column.
 */
template <typename T>
class column {
public:
    /** Create column from component array.
     *
     * @param array Pointer to the component array.
     * @param count Number of elements in component array.
     * @param is_shared Is the component shared or not.
     */
    column(T* array, size_t count, bool is_shared = false)
        : m_array(array)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Create column from iterator.
     *
     * @param iter Iterator object.
     * @param column Index of the signature of the query being iterated over.
     */
    column(iter &iter, int column);

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    T& operator[](size_t index) {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!index || !m_is_shared, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array[index];
    }

    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     * 
     * @return Reference to the first element.
     */
    T* operator->() {
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array;
    }

    /** Return whether component is set.
     * If the column is optional, this method may return false.
     * 
     * @return True if component is set, false if component is not set.
     */
    bool is_set() const {
        return m_array != nullptr;
    }

    /** Return whether component is shared.
     * If the column is shared, this method returns true.
     * 
     * @return True if component is shared, false if component is owned.
     */
    bool is_shared() const {
        return m_is_shared;
    }

protected:
    T* m_array;
    size_t m_count;
    bool m_is_shared;
};


////////////////////////////////////////////////////////////////////////////////

namespace _ {

/** Similar to flecs::column, but abstracts away from shared / owned columns.
 * 
 * @tparam T component type of the column.
 */
template <typename T, typename = void>
class any_column { };

template <typename T>
class any_column<T, typename std::enable_if<std::is_pointer<T>::value == true>::type > final : public column<typename std::remove_pointer<T>::type> {
public:
    any_column(T array, std::size_t count, bool is_shared = false)
        : column<typename std::remove_pointer<T>::type>(array, count, is_shared) { }

    T operator[](size_t index) {
        if (!this->m_is_shared) {
            ecs_assert(index < this->m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
            if (this->m_array) {
                return &this->m_array[index];
            } else {
                return nullptr;
            }
        } else {
            return &this->m_array[0];
        }
    }   
};

template <typename T>
class any_column<T, typename std::enable_if<std::is_pointer<T>::value == false>::type> final : public column<T> {
public:
    any_column(T* array, std::size_t count, bool is_shared = false)
        : column<T>(array, count, is_shared) { }

    T& operator[](size_t index) {
        if (!this->m_is_shared) {
            ecs_assert(index < this->m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
            return this->m_array[index];
        } else {
            return this->m_array[0];
        }
    }   
};

////////////////////////////////////////////////////////////////////////////////

/** Iterate over an integer range (used to iterate over entity range).
 *
 * @tparam Type of the iterator
 */
template <typename T>
class range_iterator
{
public:
    explicit range_iterator(T value)
        : m_value(value){}

    bool operator!=(range_iterator const& other) const
    {
        return m_value != other.m_value;
    }

    T const& operator*() const
    {
        return m_value;
    }

    range_iterator& operator++()
    {
        ++m_value;
        return *this;
    }

private:
    T m_value;
};

} // namespace _


////////////////////////////////////////////////////////////////////////////////

/** Class that enables iterating over table columns.
 */
class iter final {
    using row_iterator = _::range_iterator<size_t>;
public:
    /** Construct iterator from C iterator object.
     * This operation is typically not invoked directly by the user.
     *
     * @param it Pointer to C iterator.
     */
    iter(const ecs_iter_t *it) : m_iter(it) { 
        m_begin = 0;
        m_end = static_cast<std::size_t>(it->count);
    }

    row_iterator begin() const {
        return row_iterator(m_begin);
    }

    row_iterator end() const {
        return row_iterator(m_end);
    }

    /** Obtain handle to current system. 
     */
    flecs::entity system() const;

    /** Obtain current world. 
     */
    flecs::world world() const;

    /** Number of entities to iterate over. 
     */
    size_t count() const {
        return static_cast<size_t>(m_iter->count);
    }

    /** Number of columns in iteator.
     */
    int32_t column_count() const {
        return m_iter->column_count;
    }

    /** Size of column data type.
     *
     * @param col The column id.
     */
    size_t column_size(int32_t col) const {
        return ecs_column_size(m_iter, col);
    }    

    /** Return delta_time of current frame. 
     */
    FLECS_FLOAT delta_time() const {
        return m_iter->delta_time;
    }

    /** Return time elapsed since last time system was invoked.
     */
    FLECS_FLOAT delta_system_time() const {
        return m_iter->delta_system_time;
    }

    /** Return total time passed in simulation.
     */
    FLECS_FLOAT world_time() const {
        return m_iter->world_time;
    }

    /** Returns whether column is shared.
     * 
     * @param col The column id.
     */
    bool is_shared(int32_t col) const {
        return !ecs_is_owned(m_iter, col);
    }

    /** Returns whether column is owned.
     * 
     * @param col The column id.
     */
    bool is_owned(int32_t col) const {
        return ecs_is_owned(m_iter, col);
    }    

    /** Returns whether column is set.
     * 
     * @param col The column id.
     */
    bool is_set(int32_t col) const {
        return ecs_column_w_size(m_iter, 0, col) != NULL;
    }
    
    /** Access param field. 
     * The param field contains the value assigned to flecs::Context, or the
     * value passed to the `param` argument when invoking system::run.
     */
    void* param() {
        return m_iter->param;
    }

    /** Returns whether column is readonly.
     *
     * @param col The column id.
     */
    bool is_readonly(int32_t col) const {
        return ecs_is_readonly(m_iter, col);
    }

    /** Obtain column source (0 if self)
     *
     * @param col The column id.
     */    
    flecs::entity column_source(int32_t col) const;

    /** Obtain component/tag entity of column.
     *
     * @param col The column id.
     */
    flecs::entity column_entity(int32_t col) const;

    /** Obtain type of column 
     *
     * @param col The column id.
     */
    flecs::type column_type(int32_t col) const;

    /** Obtain entity being iterated over for row.
     *
     * @param row Row being iterated over.
     */
    flecs::entity entity(size_t row) const;

    /** Obtain type of table being iterated over.
     */
    type table_type() const;

    /** Obtain the total number of tables the iterator will iterate over.
     */
    int32_t table_count() const {
        return m_iter->table_count;
    }

    /** Obtain the total number of inactive tables the query is matched with.
     */
    int32_t inactive_table_count() const {
        return m_iter->inactive_table_count;
    }

    /** Obtain untyped pointer to table column.
     *
     * @param table_column Id of table column (corresponds with location in table type).
     * @return Pointer to table column.
     */
    void* table_column(int32_t col) const {
        return ecs_table_column(m_iter, col);
    }

    /** Obtain typed pointer to table column.
     * If the table does not contain a column with the specified type, the
     * function will assert.
     *
     * @tparam T Type of the table column.
     */
    template <typename T>
    flecs::column<T> table_column() const {
        auto type = ecs_iter_type(m_iter);
        auto col = ecs_type_index_of(type, _::component_info<T>::id());
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);
        return flecs::column<T>(static_cast<T*>(ecs_table_column(m_iter, col)), 
            static_cast<std::size_t>(m_iter->count), false);
    }

    /** Obtain column with const type.
     * If the specified column id does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    flecs::column<T> column(int32_t col) const {
        return get_column<T>(col);
    }

    /** Obtain column with non-const type.
     * If the specified column id does not match with the provided type or if
     * the column is readonly, the function will assert.
     *
     * @tparam T Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    flecs::column<T> column(int32_t col) const {
        ecs_assert(!ecs_is_readonly(m_iter, col), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_column<T>(col);
    }  

    /** Obtain unsafe column.
     * Unsafe columns are required when a system does not know at compile time
     * which component will be passed to it. 
     *
     * @param col The column id. 
     */
    flecs::unsafe_column column(int32_t col) const {
        return get_unsafe_column(col);
    }

    /** Obtain owned column.
     * Same as iter::column, but ensures that column is owned.
     *
     * @tparam Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T>
    flecs::column<T> owned(int32_t col) const {
        ecs_assert(!!ecs_is_owned(m_iter, col), ECS_COLUMN_IS_SHARED, NULL);
        return this->column<T>(col);
    }

    /** Obtain shared column.
     * Same as iter::column, but ensures that column is shared.
     *
     * @tparam Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T>
    const T& shared(int32_t col) const {
        ecs_assert(ecs_column_entity(m_iter, col) == _::component_info<T>::id(m_iter->world), ECS_COLUMN_TYPE_MISMATCH, NULL);
        ecs_assert(!ecs_is_owned(m_iter, col), ECS_COLUMN_IS_NOT_SHARED, NULL);
        return *static_cast<T*>(ecs_column_w_size(m_iter, sizeof(T), col));
    }

    /** Obtain single const element of owned or shared column.
     * If the specified column id does not match with the provided type the
     * function will assert.    
     *
     * @tparam Type of the element.
     * @param col The column id.
     * @param row The current row.
     * @return The component element.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>    
    T& element(int32_t col, int32_t row) const {
        return get_element<T>(col, row);
    }

    /** Obtain single const element of owned or shared column.
     * If the specified column id does not match with the provided type or if
     * the column is readonly, the function will assert.
     *
     * @tparam Type of the element.
     * @param col The column id.
     * @param row The current row.
     * @return The component element.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    T& element(int32_t col, int32_t row) const {
        ecs_assert(!ecs_is_readonly(m_iter, col), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_element<T>(col, row);
    }

private:
    /* Get column, check if correct type is used */
    template <typename T>
    flecs::column<T> get_column(int32_t column_id) const {
#ifndef NDEBUG
        ecs_entity_t column_entity = ecs_column_entity(m_iter, column_id);
        ecs_assert(column_entity & ECS_TRAIT || column_entity & ECS_SWITCH || 
            column_entity & ECS_CASE ||
            column_entity == _::component_info<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_is_owned(m_iter, column_id);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }
        
        return flecs::column<T>(
            static_cast<T*>(ecs_column_w_size(m_iter, sizeof(T), column_id)), 
            count, is_shared);
    } 

    flecs::unsafe_column get_unsafe_column(int32_t column_id) const {
        size_t count;
        size_t size = ecs_column_size(m_iter, column_id);
        bool is_shared = !ecs_is_owned(m_iter, column_id);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }

        return flecs::unsafe_column(ecs_column_w_size(m_iter, 0, column_id), size, count, is_shared);
    }       

    /* Get single field, check if correct type is used */
    template <typename T>
    T& get_element(int32_t col, int32_t row) const {
        ecs_assert(ecs_column_entity(m_iter, col) == _::component_info<T>::id(m_iter->world), ECS_COLUMN_TYPE_MISMATCH, NULL);
        return *static_cast<T*>(ecs_element_w_size(m_iter, sizeof(T), col, row));
    }       

    const ecs_iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

template <typename T>
inline column<T>::column(iter &iter, int32_t col) {
    *this = iter.column<T>(col);
}


////////////////////////////////////////////////////////////////////////////////

/** The world.
 * The world is the container of all ECS data and systems. If the world is
 * deleted, all data in the world will be deleted as well.
 */
class world final {
public:
    /** Create world.
     */
    world() 
        : m_world( ecs_init() )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world with command line arguments.
     * Currently command line arguments are not interpreted, but they may be
     * used in the future to configure Flecs parameters.
     */
    world(int argc, char *argv[])
        : m_world( ecs_init_w_args(argc, argv) )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world from C world.
     */
    explicit world(world_t *w) 
        : m_world( w ) 
        , m_owned( false ) { }

    /** Not allowed to copy a world. May only take a reference. 
     */
    world(const world& obj) = delete;

    world(world&& obj) {
        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
    }

    /** Not allowed to copy a world. May only take a reference.
     */
    world& operator=(const world& obj) = delete;

    world& operator=(world&& obj) {
        this->~world();

        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
        return *this;
    }
    
    ~world() { 
        if (m_owned && m_world) {
            ecs_fini(m_world); 
        }
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return m_world;
    }

    /** Enable tracing.
     *
     * @param level The tracing level.
     */
    static void enable_tracing(int level) {
        ecs_tracing_enable(level);
    }

    /** Progress world, run all systems.
     *
     * @param delta_time Custom delta_time. If 0 is provided, Flecs will automatically measure delta_tiem.
     */
    bool progress(FLECS_FLOAT delta_time = 0.0) const {
        return ecs_progress(m_world, delta_time);
    }

    /** Defer operations until end of frame. 
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin and defer_end operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     */
    bool defer_begin() {
        return ecs_defer_begin(m_world);
    }

    /** End block of operations to defer. 
     * See defer_begin.
     *
     * This operation is thread safe.
     */
    bool defer_end() {
        return ecs_defer_end(m_world);
    }

    /** Set number of threads.
     * This will distribute the load evenly across the configured number of 
     * threads for each system.
     *
     * @param threads Number of threads.
     */
    void set_threads(std::int32_t threads) const {
        ecs_set_threads(m_world, threads);
    }

    /** Get number of threads.
     *
     * @return Number of configured threads.
     */
    std::int32_t get_threads() const {
        return ecs_get_threads(m_world);
    }

    /** Get index of current thread.
     *
     * @return Unique index for current thread.
     */
    std::int32_t get_thread_index() const {
        return ecs_get_thread_index(m_world);
    }

    /** Set target FPS
     * This will ensure that the main loop (world::progress) does not run faster
     * than the specified frames per second.
     *
     * @param target_fps Target frames per second.
     */
    void set_target_fps(FLECS_FLOAT target_fps) const {
        ecs_set_target_fps(m_world, target_fps);
    }

    /** Get target FPS
     *
     * @return Configured frames per second.
     */
    FLECS_FLOAT get_target_fps() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->target_fps;
    }

    /** Get tick
     *
     * @return Monotonically increasing frame count.
     */
    std::int32_t get_tick() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->frame_count_total;
    }

    /** Set timescale
     *
     * @return Monotonically increasing frame count.
     */
    void set_time_scale(FLECS_FLOAT mul) const {
        ecs_set_time_scale(m_world, mul);
    }  

    /** Get timescale
     *
     * @return Monotonically increasing frame count.
     */
    FLECS_FLOAT get_time_scale() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->time_scale;
    }        

    /** Set world context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx The world context.
     */
    void set_context(void* ctx) const {
        ecs_set_context(m_world, ctx);
    }

    /** Get world context.
     *
     * @return The configured world context.
     */
    void* get_context() const {
        return ecs_get_context(m_world);
    }

    /** Preallocate memory for number of entities.
     * This function preallocates memory for the entity index.
     *
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim(std::int32_t entity_count) const {
        ecs_dim(m_world, entity_count);
    }

    /** Preallocate memory for type
     * This function preallocates memory for the component arrays of the
     * specified type.
     *
     * @param type Type to preallocate memory for.
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim_type(type_t t, std::int32_t entity_count) const {
        ecs_dim_type(m_world, t, entity_count);
    }

    /** Set entity range.
     * This function limits the range of issued entity ids between min and max.
     *
     * @param min Minimum entity id issued.
     * @param max Maximum entity id issued.
     */
    void set_entity_range(entity_t min, entity_t max) const {
        ecs_set_entity_range(m_world, min, max);
    }

    /** Enforce that operations cannot modify entities outside of range.
     * This function ensures that only entities within the specified range can
     * be modified. Use this function if specific parts of the code only are
     * allowed to modify a certain set of entities, as could be the case for
     * networked applications.
     *
     * @param enabled True if range check should be enabled, false if not.
     */
    void enable_range_check(bool enabled) const {
        ecs_enable_range_check(m_world, enabled);
    }

    /** Disables inactive systems.
     *
     * This removes systems that are not matched with any entities from the main
     * loop. Systems are only added to the main loop after they first match with
     * entities, but are not removed automatically.
     *
     * This function allows an application to manually disable inactive systems
     * which removes them from the main loop. Doing so will cause Flecs to
     * rebuild the pipeline in the next iteration.
     *
     * @param level The tracing level.
     */
    void deactivate_systems() {
        ecs_deactivate_systems(m_world);
    }    

    /** Lookup entity by name.
     * 
     * @param name Entity name.
     */
    flecs::entity lookup(const char *name) const;

    /** Lookup entity by name.
     *
     * @overload
     */    
    flecs::entity lookup(std::string& name) const;

    /** Set singleton component.
     */
    template <typename T>
    void set(T value) const;

    /** Get mut singleton component.
     */
    template <typename T>
    T* get_mut() const;

    /** Mark singleton component as modified.
     */
    template <typename T>
    void modified() const;

    /** Patch singleton component.
     */
    template <typename T>
    void patch(std::function<void(T&)> func) const;

    /** Get singleton component.
     */
    template <typename T>
    const T* get() const;

    /** Test if world has singleton component.
     */
    template <typename T>
    bool has() const;

    /** Remove singleton component.
     */
    template <typename T>
    void remove() const;

    /** Get id for type.
     */
    template <typename T>
    entity_t type_id() {
        return _::component_info<T>::id(m_world);
    }

    /** Get singleton entity for type.
     */
    template <typename T>
    flecs::entity singleton();

    /** Create alias for component.
     *
     * @tparam Component to create an alias for.
     * @param alias Alias for the component.
     */
    template <typename T>
    flecs::entity use(const char *alias = nullptr);

    /** Create alias for entity.
     *
     * @param name Name of the entity.
     * @param alias Alias for the entity.
     */
    flecs::entity use(const char *name, const char *alias = nullptr);    

    /** Create alias for entity.
     *
     * @param entity Entity for which to create the alias.
     * @param alias Alias for the entity.
     */
    void use(flecs::entity entity, const char *alias = nullptr);   

    /** Delete all entities matching a filter.
     *
     * @param filter The filter to use for matching.
     */
    void delete_entities(flecs::filter filter) const;

    /** Add component to all entities matching a filter.
     *
     * @tparam T The component to add.
     * @param filter The filter to use for matching.
     */
    template <typename T>
    void add(flecs::filter filter) const;

    /** Add type to all entities.
     *
     * @param type The type to add.
     */
    void add(flecs::type type) const;

    /** Add type to all entities matching a filter.
     *
     * @param type The type to add.
     * @param filter The filter to use for matching.
     */    
    void add(flecs::type type, flecs::filter filter) const;

    /** Add entity to all entities.
     *
     * @param entity The entity to add.
     */
    void add(flecs::entity entity) const;

    /** Add entity to all entities matching a filter.
     *
     * @param entity The entity to add.
     * @param filter The filter to use for matching.
     */    
    void add(flecs::entity entity, flecs::filter filter) const;

    /** Remove component from all entities matching a filter.
     *
     * @tparam T The component to remove.
     * @param filter The filter to use for matching.
     */    
    template <typename T>
    void remove(flecs::filter filter) const;

    /** Remove type from all entities.
     *
     * @param type The component to remove.
     */ 
    void remove(flecs::type type) const;

    /** Remove type from all entities matching a filter.
     *
     * @tparam T The component to remove.
     * @param filter The filter to use for matching.
     */     
    void remove(flecs::type type, flecs::filter filter) const;

    /** Remove entity from all entities.
     *
     * @param entity The entity to remove.
     */ 
    void remove(flecs::entity entity) const;

    /** Remove entity from all entities matching a filter.
     *
     * @param entity The entity to remove.
     * @param filter The filter to use for matching.
     */     
    void remove(flecs::entity entity, flecs::filter filter) const;

    /** Create iterable filter for entities in world.
     *
     * @param filter The filter to create.
     */ 
    world_filter filter(const flecs::filter& filter) const;

    flecs::filter_iterator begin() const;
    flecs::filter_iterator end() const;

    /** Count entities matching a component.
     *
     * @tparam T The component to use for matching.
     */
    template <typename T>
    int count() const {
        return ecs_count_type(
            m_world, _::component_info<T>::type(m_world));
    }

    /** Count entities matching a filter.
     *
     * @param filter The filter to use for matching.
     */
    int count(flecs::filter filter) const;

    /** Enable locking.
     * 
     * @param enabled True if locking should be enabled, false if not.
     */
    bool enable_locking(bool enabled) {
        return ecs_enable_locking(m_world, enabled);
    }

    /** Lock world.
     */
    void lock() {
        ecs_lock(m_world);
    }

    /** Unlock world.
     */
    void unlock() {
        ecs_unlock(m_world);
    }

    /** Create a prefab.
     */
    template <typename... Args>
    flecs::entity entity(Args &&... args) const;

    /** Create an entity.
     */
    template <typename... Args>
    flecs::entity prefab(Args &&... args) const;

    /** Create a type.
     */
    template <typename... Args>
    flecs::type type(Args &&... args) const;

    /** Create a module.
     */
    template <typename Module, typename... Args>
    flecs::entity module(Args &&... args) const;

    /** Import a module.
     */
    template <typename Module>
    flecs::entity import(); // Cannot be const because modules accept a non-const world

    /** Create an system.
     */
    template <typename... Comps, typename... Args>
    flecs::system<Comps...> system(Args &&... args) const;

    /** Create a query.
     */
    template <typename... Comps, typename... Args>
    flecs::query<Comps...> query(Args &&... args) const;

    /** Register a component.
     */
    template <typename T, typename... Args>
    flecs::entity component(Args &&... args) const;

    /** Register a POD component.
     */
    template <typename T, typename... Args>
    flecs::entity pod_component(Args &&... args) const;

    /** Register a relocatable component.
     */
    template <typename T, typename... Args>
    flecs::entity relocatable_component(Args &&... args) const;

    /** Create a snapshot.
     */
    template <typename... Args>
    flecs::snapshot snapshot(Args &&... args) const;
    
private:
    void init_builtin_components();

    world_t *m_world;
    bool m_owned;
};


////////////////////////////////////////////////////////////////////////////////

/** Fluent API for chaining entity operations
 * This class contains entity operations that can be chained. For example, by
 * using this class, an entity can be created like this:
 *
 * flecs::entity e = flecs::entity(world)
 *   .add<Position>()
 *   .add<Velocity>();
 */
template <typename Base>
class entity_builder {
    using base_type = const Base;
public:

    /** Add an entity to an entity by id.
     * This adds a raw entity id (64 bit integer) to the type of the current
     * entity.
     * 
     * @param entity The entity id to add.
     */
    base_type& add(entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity](world_t *world, entity_t id) {
            ecs_add_entity(world, id, entity);
        });
        return *static_cast<base_type*>(this);         
    }

    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    base_type& add() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_add_entity(world, id, _::component_info<T>::id(world));
        });
        return *static_cast<base_type*>(this);
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param entity The entity to add.
     */
    base_type& add(const entity& entity) const;

    /** Add a type to an entity by its C pointer.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     *
     * @param type The C type to add.
     */
    base_type& add(type_t type) const {
        static_cast<base_type*>(this)->invoke(
        [type](world_t *world, entity_t id) {
            ecs_add_type(world, id, type);
        });
        return *static_cast<base_type*>(this); 
    }

    /** Add a type to an entity.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     * 
     * @param type The type to add.
     */
    base_type& add(type type) const;

    /** Add a trait.
     * This operation adds a trait for an entity by entity id. If the trait
     * is a component, a value of the trait type will be associated with the
     * entity. If the trait is not a component, a value of the component type
     * will be associated with the entity. If both the trait and component ids
     * are regular entities, no values will be associated with the entity.
     *
     * @param trait The trait id.
     * @param entity The entity identifier.
     */
    base_type& add_trait(entity_t trait, entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity, trait](world_t *world, entity_t id) {
            ecs_add_entity(world, id, 
                ecs_trait(entity, trait));
        });
        return *static_cast<base_type*>(this); 
    }

    /** Add a trait.
     * This operation adds a trait for a component. A value of the trait type
     * will be associated with the entity.
     *
     * @tparam T the trait type.
     * @tparam C the component type.
     */
    template<typename T, typename C>
    base_type& add_trait() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {       
            ecs_add_entity(world, id, 
                ecs_trait(_::component_info<C>::id(world), 
                          _::component_info<T>::id(world)));
        });
        return *static_cast<base_type*>(this); 
    }

    /** Add a trait.
     * This operation adds a trait for a component. A value of the trait 
     * type will be associated with the entity. 
     *
     * @tparam T The trait to add.
     * @param component The component for which to add the trait.
     */
    template<typename T>
    base_type& add_trait(flecs::entity component) const;

    /** Add a trait tag.
     * This operation adds a trait tag for a component. A value of the component 
     * type will be associated with the entity. Note that the trait tag passed 
     * into this function should not be a component.
     *
     * @tparam C The component type.
     * @param trait The trait identifier.
     */
    template<typename C>
    base_type& add_trait_tag(flecs::entity trait) const;

    /** Add a trait.
     * This operation adds a trait for an entity by entity id. If the trait
     * is a component, a value of the trait type will be associated with the
     * entity. If the trait is not a component, a value of the component type
     * will be associated with the entity. If both the trait and component ids
     * are regular entities, no values will be associated with the entity.
     *
     * @param trait The trait to add.
     * @param entity The tag for which to add the trait.
     */
    base_type& add_trait(flecs::entity trait, flecs::entity entity) const;

    /** Remove an entity from an entity by id.
     *
     * @param entity The entity id to remove.
     */
    base_type& remove(entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, entity);
        });
        return *static_cast<base_type*>(this);
    }    

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T>
    base_type& remove() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, _::component_info<T>::id(world));
        });
        return *static_cast<base_type*>(this);
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    base_type& remove(const entity& entity) const;

    /** Remove a type from an entity by its C pointer.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     *
     * @param type the pointer to the type to remove.
     */
    base_type& remove(type_t type) const {
        static_cast<base_type*>(this)->invoke(
        [type](world_t *world, entity_t id) {
            ecs_remove_type(world, id, type);
        });
        return *static_cast<base_type*>(this);         
    }

    /** Remove a type from an entity.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     *
     * @param type the type to remove.
     */
    base_type& remove(type type) const;

    /** Remove a trait.
     * This operation removes a trait for an entity by entity id.
     *
     * @param trait The trait to remove.
     * @param entity The entity for which to remove the trait.
     */
    base_type& remove_trait(entity_t trait, entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity, trait](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, 
                ecs_trait(entity, trait));
        });
        return *static_cast<base_type*>(this);         
    }

    /** Remove a trait.
     * This operation removes a trait for a component.
     *
     * @tparam T The trait to remove.
     * @tparam C The component for which to remove the trait.
     */
    template<typename T, typename C>
    base_type& remove_trait() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {   
            ecs_remove_entity(world, id,
                ecs_trait(_::component_info<C>::id(world), 
                          _::component_info<T>::id(world)));
        });
        return *static_cast<base_type*>(this);
    }

    /** Remove a trait.
     * This operation removes a trait tag for a component. The trait should not
     * be a component.
     *
     * @tparam T The trait to remove.
     * @param component The component for which to remove the trait.
     */
    template<typename T>
    base_type& remove_trait(flecs::entity component) const;

    /** Remove a trait tag.
     * This operation removes a trait tag for a component. The trait should not
     * be a component.
     *
     * @tparam C The component for which to remove the trait.
     * @param trait The trait to remove.
     */
    template<typename C>
    base_type& remove_trait_tag(flecs::entity trait) const;

    /** Remove a trait.
     * This operation removes a trait for an entity.
     *
     * @param trait The trait to remove.
     * @param entity The entity for which to remove the trait.
     */
    base_type& remove_trait(flecs::entity trait, flecs::entity entity) const;

    /** Add a parent entity to an entity by id.
     *
     * @param parent The id of the parent to add.
     */    
    base_type& add_childof(entity_t parent) const {
        static_cast<base_type*>(this)->invoke(
        [parent](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_CHILDOF | parent);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Add a parent entity to an entity.
     * 
     * @param parent The parent to add.
     */
    base_type& add_childof(const entity& parent) const;

    /** Remove a parent entity from an entity by id.
     *
     * @param parent The id of the parent to remove.
     */
    base_type& remove_childof(entity_t parent) const {
        static_cast<base_type*>(this)->invoke(
        [parent](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, ECS_CHILDOF | parent);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Remove a parent entity from an entity.
     *
     * @param parent The parent to remove.
     */
    base_type& remove_childof(const entity& parent) const;

    /** Add a base entity to an entity by id.
     *
     * @param base The base id to add.
     */    
    base_type& add_instanceof(entity_t base) const {
        static_cast<base_type*>(this)->invoke(
        [base](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_INSTANCEOF | base);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Add a base entity to an entity.
     *
     * @param base The base to add.
     */
    base_type& add_instanceof(const entity& base) const;  

    /** Remove a base entity from an entity by id.
     *
     * @param base The base id to remove.
     */
    base_type& remove_instanceof(entity_t base) const {
        static_cast<base_type*>(this)->invoke(
        [base](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, ECS_INSTANCEOF | base);
        });
        return *static_cast<base_type*>(this);
    }

    /** Remove a base entity from an entity.
     *
     * @param base The base to remove.
     */
    base_type& remove_instanceof(const entity& base) const;

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OWNED flag
     */    
    base_type& add_owned(entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_OWNED | entity);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OWNED flag
     */    
    template <typename T>
    base_type& add_owned() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_OWNED | _::component_info<T>::id(world));
        });
        return *static_cast<base_type*>(this);  
    }

    /** Add owned flag for type entity.
     * This will ensure that all components in the type are owned for instances
     * of this entity.
     *
     * @param type The type for which to add the OWNED flag
     */    
    base_type& add_owned(flecs::type type) const;

    /** Add a switch to an entity by id.
     * The switch entity must be a type, that is it must have the EcsType
     * component. Entities created with flecs::type are valid here.
     *
     * @param sw The switch entity id to add.
     */    
    base_type& add_switch(entity_t sw) const {
        static_cast<base_type*>(this)->invoke(
        [sw](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_SWITCH | sw);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Add a switch to an entity.
     * The switch entity must be a type, that is it must have the EcsType
     * component.
     *
     * @param sw The switch entity to add.
     */ 
    base_type& add_switch(const entity& sw) const;

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    base_type& add_switch(const type& sw) const;

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch entity id to remove.
     */    
    base_type& remove_switch(entity_t sw) const {
        static_cast<base_type*>(this)->invoke(
        [sw](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, ECS_SWITCH | sw);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Remove a switch from an entity.
     *
     * @param sw The switch entity to remove.
     */ 
    base_type& remove_switch(const entity& sw) const;
    
    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    base_type& remove_switch(const type& sw) const;

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    base_type& add_case(entity_t sw_case) const {
        static_cast<base_type*>(this)->invoke(
        [sw_case](world_t *world, entity_t id) {
            ecs_add_entity(world, id, ECS_CASE | sw_case);
        });
        return *static_cast<base_type*>(this);
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    base_type& add_case() const {
        return this->add_case(_::component_info<T>::id());
    }

    /** Add a case to an entity.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity to add.
     */ 
    base_type& add_case(const entity& sw_case) const;

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    base_type& remove_case(entity_t sw_case) const {
        static_cast<base_type*>(this)->invoke(
        [sw_case](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, ECS_CASE | sw_case);
        });
        return *static_cast<base_type*>(this);  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    base_type& remove_case() const {
        return this->remove_case(_::component_info<T>::id());
    }    

    /** Remove a case from an entity.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */ 
    base_type& remove_case(const entity& sw_case) const;

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    base_type& enable() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_enable(world, id, true);
        });
        return *static_cast<base_type*>(this);
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    base_type& disable() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_enable(world, id, false);
        });
        return *static_cast<base_type*>(this);
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    base_type& enable() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_enable_component_w_entity(world, id, _::component_info<T>::id(), true);
        });
        return *static_cast<base_type*>(this);
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    base_type& disable() const {
        static_cast<base_type*>(this)->invoke(
        [](world_t *world, entity_t id) {
            ecs_enable_component_w_entity(world, id, _::component_info<T>::id(), false);
        });
        return *static_cast<base_type*>(this);
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param id The component to enable.
     */   
    base_type& enable(flecs::entity_t id) const {
        static_cast<base_type*>(this)->invoke(
        [id](world_t *world, entity_t e) {
            ecs_enable_component_w_entity(world, e, id, true);
        }); 
        return *static_cast<base_type*>(this);       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param id The component to disable.
     */   
    base_type& disable(flecs::entity_t id) const {
        static_cast<base_type*>(this)->invoke(
        [id](world_t *world, entity_t e) {
            ecs_enable_component_w_entity(world, e, id, false);
        }); 
        return *static_cast<base_type*>(this);       
    }

    /** Enable a component.
     * See enable<T>.
     *
     * @param entity The component to enable.
     */   
    base_type& enable(const flecs::entity& entity) const;

    /** Disable a component.
     * See disable<T>.
     *
     * @param entity The component to disable.
     */   
    base_type& disable(const flecs::entity& entity) const;

    /** Set a component for an entity.
     * This operation overwrites the component value. If the entity did not yet
     * have the component, this operation will add it.
     *
     * @tparam T The component to set.
     * @param value The value to assign to the component.
     */
    template <typename T>
    const base_type& set(T&& value) const {
        static_cast<base_type*>(this)->invoke(
        [&value](world_t *world, entity_t id) {
            auto comp_id = _::component_info<T>::id(world);

            ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

            ecs_set_ptr_w_entity(
                world, id, comp_id, sizeof(T), &value);
        });
        return *static_cast<base_type*>(this);
    }

    /** Set a component for an entity.
     * This operation overwrites the component value. If the entity did not yet
     * have the component, this operation will add it.
     *
     * @tparam T The component to set.
     * @param value The value to assign to the component.
     */
    template <typename T>
    const base_type& set(const T& value) const {
        static_cast<base_type*>(this)->invoke(
        [&value](world_t *world, entity_t id) {
            auto comp_id = _::component_info<T>::id(world);

            ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

            ecs_set_ptr_w_entity(
                world, id, comp_id, sizeof(T), &value);
        });
        return *static_cast<base_type*>(this);
    }

    /** Set a trait for an entity.
     * This operation overwrites the trait value. If the entity did not yet
     * have the trait, this operation will add it.
     *
     * @tparam T The trait to set.
     * @tparam C The component for which to set the trait.
     * @param value The value to assign to the trait.     
     */
    template <typename T, typename C>
    const base_type& set_trait(const T& value) const {
        static_cast<base_type*>(this)->invoke(
        [&value](world_t *world, entity_t id) {
            auto t_id = _::component_info<T>::id(world);

            ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

            ecs_set_ptr_w_entity(world, id, 
                ecs_trait(_::component_info<C>::id(world), t_id),
                        sizeof(T), &value);
        });
        return *static_cast<base_type*>(this);
    } 

    /** Set a trait tag for a component.
     * This operation overwrites the trait value. If the entity did not yet
     * have the trait, this operation will add it.
     *
     * This operation should be used for traits that are not components. If a
     * trait is not a component, it will assume the type of the component it is
     * assigned to.
     *
     * @tparam C The component for which to set the trait.
     * @param value The value to assign to the trait.      
     */
    template <typename C>
    const base_type& set_trait_tag(flecs::entity trait, const C& value) const;

    /** Set a trait for an entity.
     * This operation overwrites the trait value. If the entity did not yet
     * have the trait, this operation will add it.
     *
     * @tparam T The trait to set.
     * @param value The value to assign to the trait. 
     * @param entity The entity for which to set the trait.
     */
    template <typename T>
    const base_type& set_trait(const T& value, flecs::entity entity) const;

    /** Patch a component value.
     * This operation allows an application to partially overwrite a component 
     * value. The operation invokes a function with a reference to the value to
     * write, and a boolean indicating if the component already existed.
     *
     * @tparam T The component to patch.
     * @param func The function invoked by this operation.
     */
    template <typename T>
    const base_type& patch(std::function<void(T&, bool)> func) const {
        static_cast<base_type*>(this)->invoke(
        [&func](world_t *world, entity_t id) {
            auto comp_id = _::component_info<T>::id(world);

            ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

            bool is_added;
            T *ptr = static_cast<T*>(ecs_get_mut_w_entity(
                world, id, comp_id, &is_added));
            if (ptr) {
                func(*ptr, !is_added);
                ecs_modified_w_entity(world, id, comp_id);
            }
        });
        return *static_cast<base_type*>(this);
    }      

    /** Patch a component value.
     * This operation allows an application to partially overwrite a component 
     * value. The operation invokes a function with a reference to the value to
     * write.
     *
     * @tparam T The component to patch.
     * @param func The function invoked by this operation.
     */
    template <typename T>
    const base_type& patch(std::function<void(T&)> func) const {
        static_cast<base_type*>(this)->invoke(
        [&func](world_t *world, entity_t id) {
            auto comp_id = _::component_info<T>::id(world);

            ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

            bool is_added;
            T *ptr = static_cast<T*>(ecs_get_mut_w_entity(
                world, id, comp_id, &is_added));
            if (ptr) {
                func(*ptr);
                ecs_modified_w_entity(world, id, comp_id);
            }
        });
        return *static_cast<base_type*>(this);
    }            
};

////////////////////////////////////////////////////////////////////////////////
//// Quick and safe access to a component pointer
////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ref {
public:
    ref()
        : m_world( nullptr )
        , m_entity( 0 )
        , m_ref() 
    {       
    }

    ref(world_t *world, entity_t entity) 
        : m_world( world )
        , m_entity( entity )
        , m_ref() 
    {
        auto comp_id = _::component_info<T>::id(world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        ecs_get_ref_w_entity(
            m_world, &m_ref, m_entity, comp_id);
    }

    const T* operator->() {
        const T* result = static_cast<const T*>(ecs_get_ref_w_entity(
            m_world, &m_ref, m_entity, _::component_info<T>::id(m_world)));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    const T* get() {
        if (m_entity) {
            ecs_get_ref_w_entity(
                m_world, &m_ref, m_entity, _::component_info<T>::id(m_world));    
        }

        return static_cast<T*>(m_ref.ptr);
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    entity_t m_entity;
    ecs_ref_t m_ref;
};


////////////////////////////////////////////////////////////////////////////////

/** Entity class
 * This class provides access to entity operations.
 */
class entity : public entity_builder<entity> {
public:
    /** Default constructor.
     */
    explicit entity()
        : m_world( nullptr )
        , m_id( 0 ) { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(const world& world) 
        : m_world( world.c_ptr() )
        , m_id( ecs_new_w_type(m_world, 0) ) { }

    /** Create entity.
     *
     * @param world Pointer to the world in which to create the entity.
     */
    explicit entity(world_t *world) 
        : m_world( world )
        , m_id( world ? ecs_new_w_type(world, 0) : 0 ) { }

    /** Create a named entity.
     * Named entities can be looked up with the lookup functions. Entity names
     * may be scoped, where each element in the name is separated by "::".
     * For example: "Foo::Bar". If parts of the hierarchy in the scoped name do
     * not yet exist, they will be automatically created.
     *
     * @param world The world in which to create the entity.
     * @param name The entity name.
     * @param is_component If true, the entity will be created from the pool of component ids (default = false).
     */
    entity(const world& world, const char *name, bool is_component = false) 
        : m_world( world.c_ptr() )
        , m_id( ecs_lookup_path_w_sep(m_world, 0, name, "::", "::") ) 
        { 
            if (!m_id) {
                if (is_component) {
                    m_id = ecs_new_component_id(m_world);
                }

                m_id = ecs_add_path_w_sep(
                    m_world, m_id, 0, name, "::", "::");
            }
        }

    /** Create a named entity.
     * Named entities can be looked up with the lookup functions. Entity names
     * may be scoped, where each element in the name is separated by "::".
     * For example: "Foo::Bar". If parts of the hierarchy in the scoped name do
     * not yet exist, they will be automatically created.
     *
     * @param world The world in which to create the entity.
     * @param name The entity name.
     * @param is_component If true, the entity will be created from the pool of component ids (default = false).
     */
    entity(const world& world, std::string name, bool is_component = false) 
        : m_world( world.c_ptr() )
        , m_id( ecs_lookup_path_w_sep(m_world, 0, name.c_str(), "::", "::") ) 
        { 
            if (!m_id) {
                if (is_component) {
                    m_id = ecs_new_component_id(m_world);
                }

                m_id = ecs_add_path_w_sep(
                    m_world, m_id, 0, name.c_str(), "::", "::");
            }
        }         

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    entity(const world& world, entity_t id) 
        : m_world( world.c_ptr() )
        , m_id(id) { }

    /** Wrap an existing entity id.
     *
     * @param world Pointer to the world in which the entity is created.
     * @param id The entity id.
     */
    entity(world_t *world, entity_t id) 
        : m_world( world )
        , m_id(id) { }

    /** Equality operator. */
    bool operator==(const entity& e) {
        return this->id() == e.id();
    }  

    /** Inequality operator. */
    bool operator!=(const entity& e) {
        return this->id() != e.id();
    }            

    explicit operator bool() {
        return m_id != 0;
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity object that
     * belongs to a world, but the entity id is 0.
     *
     * @param world The world.
     */
    static
    flecs::entity null(const world& world) {
        return flecs::entity(world.c_ptr(), (ecs_entity_t)0);
    }

    static
    flecs::entity null() {
        return flecs::entity(nullptr, (ecs_entity_t)0);
    }    

    /** Get entity id.
     * @return The integer entity id.
     */
    entity_t id() const {
        return m_id;
    }

    /** Get lo entity id.
     * @return A new entity containing the lower 32 bits of the entity id.
     */
    flecs::entity lo() const {
        return flecs::entity(m_world, ecs_entity_t_lo(m_id));
    }

    /** Get hi entity id.
     * @return A new entity containing the higher 32 bits of the entity id.
     */
    flecs::entity hi() const {
        return flecs::entity(m_world, ecs_entity_t_hi(m_id));
    }

    /** Combine two entity ids.
     * @return A new entity that combines the provided entity ids in the lower
     *         and higher 32 bits of the entity id.
     */
    static 
    flecs::entity comb(flecs::entity lo, flecs::entity hi) {
        return flecs::entity(lo.world(), 
            ecs_entity_t_comb(lo.id(), hi.id()));
    }

    /** Add role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *
     * @return A new entity with the specified role set.
     */
    flecs::entity add_role(entity_t role) const {
        return flecs::entity(m_world, m_id | role);
    }

    /** Remove role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *    
     * @return A new entity with any roles removed.
     */
    flecs::entity remove_role() const {
        return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
    }

    /** Check if entity has specified role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *    
     * @return True if the entity has the role, false otherwise.
     */
    bool has_role(entity_t role) const {        
        return ((m_id & ECS_ROLE_MASK) == role);
    }

    /** Check is entity is alive.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_alive() {
        return ecs_is_alive(m_world, m_id);
    }

    /** Return the entity name.
     *
     * @return The entity name, or an empty string if the entity has no name.
     */
    std::string name() const {
        const EcsName *name = static_cast<const EcsName*>(
            ecs_get_w_entity(m_world, m_id, ecs_entity(EcsName)));
        if (name && name->value) {
            return std::string(name->value);
        } else {
            return std::string();
        }
    }

    /** Return the entity path.
     *
     * @return The hierarchical entity path, or an empty string if the entity 
     *         has no name.
     */
    std::string path(const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, 0, m_id, 0, sep, init_sep);
        if (path) {
            std::string result = std::string(path);
            ecs_os_free(path);
            return result;
        } else {
            return std::string();
        }
    }   

    bool enabled() {
        return !ecs_has_entity(m_world, m_id, flecs::Disabled);
    }

    /** Return the world.
     *
     * @return The world the entity is stored in.
     */
    flecs::world world() const {
        return flecs::world(m_world);
    }

    /** Return the type.
     *
     * @return Returns the entity type.
     */
    flecs::type type() const;

    /** Return type containing the entity.
     *
     * @return A type that contains only this entity.
     */
    flecs::type to_type() const;

    /** Get component value.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T>
    const T* get() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(
            ecs_get_w_entity(m_world, m_id, comp_id));
    }

    /** Get component value (untyped).
     * 
     * @param component The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_w_entity(m_world, m_id, component.id());
    }

    /** Get component value (untyped).
     * 
     * @param component The id of the component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(entity_t component_id) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_w_entity(m_world, m_id, component_id);
    } 

    /** Get mutable component value.
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @tparam T The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    template <typename T>
    T* get_mut(bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(m_world, m_id, comp_id, is_added));
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param component The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    void* get_mut(flecs::entity component, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_mut_w_entity(m_world, m_id, component.id(), is_added);
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param component The id of the component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t component_id, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_mut_w_entity(m_world, m_id, component_id, is_added);
    }

    /** Get trait value.
     * 
     * @tparam T The trait to get.
     * @tparam C The component for which to get the trait.
     * @return Pointer to the trait value, nullptr if the entity does not
     *         have the trait.
     */
    template<typename T, typename C>
    const T* get_trait() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        
        auto t_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_w_entity(m_world, m_id, ecs_trait(
            _::component_info<C>::id(m_world), t_id)));
    }   

    /** Get trait value.
     * 
     * @tparam T The trait to get.
     * @param component The component for which to get the trait.
     * @return Pointer to the trait value, nullptr if the entity does not
     *         have the trait.
     */
    template<typename T>
    const T* get_trait(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_w_entity(m_world, m_id, ecs_trait(
            component.id(), comp_id)));
    }

    /** Get trait tag value.
     * The trait passed to this function should not be a component. If a trait
     * is not a component, the trait assumes the type of the component it is
     * assigned to.
     * 
     * @tparam C The component for which to get the trait
     * @param trait The trait to get.
     * @return Pointer to the trait value, nullptr if the entity does not
     *         have the trait.
     */
    template<typename C>
    const C* get_trait_tag(flecs::entity trait) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<C>::id(m_world);

        ecs_assert(_::component_info<C>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<const C*>(ecs_get_w_entity(m_world, m_id, ecs_trait(
            comp_id, trait.id())));
    }

    /** Get trait tag value (untyped).
     * If a trait is not a component, the trait assumes the type of the 
     * component it is assigned to.
     * 
     * @param trait The trait to get.
     * @param component The component for which to get the trait.
     * @return Pointer to the trait value, nullptr if the entity does not
     *         have the trait.
     */
    const void* get_trait(flecs::entity trait, flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_w_entity(m_world, m_id, ecs_trait(
            component.id(), trait.id()));
    }

    /** Get mutable trait value.
     * This operation returns a mutable pointer to the trait. If the entity
     * did not yet have the trait, it will be added. If a base entity had
     * the trait, it will be overridden, and the value of the base trait
     * will be copied to the entity before this function returns.
     *
     * @tparam T The trait to get.
     * @tparam C The component for which to get the trait.
     * @param is_added If provided, this parameter will be set to true if the trait was added.
     * @return Pointer to the trait value.
     */
    template <typename T, typename C>
    T* get_trait_mut(bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto t_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(
                m_world, m_id, ecs_trait(_::component_info<C>::id(m_world), 
                    t_id), is_added));
    }    

    /** Get mutable trait value.
     * This operation returns a mutable pointer to the trait. If the entity
     * did not yet have the trait, it will be added. If a base entity had
     * the trait, it will be overridden, and the value of the base trait
     * will be copied to the entity before this function returns.
     *
     * @tparam T The trait to get.
     * @param component The component for which to get the trait.
     * @param is_added If provided, this parameter will be set to true if the trait was added.
     * @return Pointer to the trait value.
     */
    template <typename T>
    T* get_trait_mut(flecs::entity component, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(
                m_world, m_id, ecs_trait( comp_id, component.id()), is_added));
    }

    /** Get mutable trait tag value.
     * This operation returns a mutable pointer to the trait. If the entity
     * did not yet have the trait, it will be added. If a base entity had
     * the trait, it will be overridden, and the value of the base trait
     * will be copied to the entity before this function returns.
     *
     * The trait passed to the function should not be a component.
     *
     * @tparam C The component for which to get the trait.
     * @param trait The trait to get.
     * @param is_added If provided, this parameter will be set to true if the trait was added.
     * @return Pointer to the trait value.
     */
    template <typename C>
    C* get_trait_tag_mut(flecs::entity trait, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(_::component_info<C>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);
        return static_cast<C*>(
            ecs_get_mut_w_entity(
                m_world, m_id, ecs_trait(
                    _::component_info<C>::id(m_world),
                    trait.id()),
                    is_added));
    }    

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        ecs_modified_w_entity(m_world, m_id, comp_id);
    }

    /** Signal that component was modified.
     *
     * @param component component that was modified.
     */
    void modified(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, component.id());
    }

    /** Signal that component was modified.
     *
     * @param component id of component that was modified.
     */
    void modified(entity_t component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, component);
    }        

    /** Get reference to component.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T>
    ref<T> get_ref() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        // Ensure component is registered
        _::component_info<T>::id(m_world);

        ecs_assert(_::component_info<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return ref<T>(m_world, m_id);
    }

    /** Get parent from an entity.
     * This operation retrieves the parent entity that has the specified 
     * component. If no parent with the specified component is found, an entity
     * with id 0 is returned. If multiple parents have the specified component,
     * the operation returns the first encountered one.
     *
     * @tparam T The component for which to find the parent.
     * @return The parent entity.
     */
    template <typename T>
    flecs::entity get_parent() {
        return flecs::entity(m_world, ecs_get_parent_w_entity(m_world, m_id, 
            _::component_info<T>::id(m_world)));
    }

    flecs::entity get_parent(flecs::entity e) {
        return flecs::entity(m_world, ecs_get_parent_w_entity(m_world, m_id, e.id()));
    }    

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity id.
     */
    void clear() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_clear(m_world, m_id);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * flecs::entity object goes out of scope.
     */
    void destruct() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_delete(m_world, m_id);
    }

    /** Lookup an entity by name.
     * Lookup an entity in the scope of this entity. The provided path may
     * contain double colons as scope separators, for example: "Foo::Bar".
     *
     * @param path The name of the entity to lookup.
     * @return The found entity, or entity::null if no entity matched.
     */
    entity lookup(const char *path) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::");
        return entity(m_world, id);
    }

    /** Check if entity has the provided entity.
     *
     * @param entity The entity id to check.
     * @return True if the entity has the provided entity id, false otherwise.
     */
    bool has(entity_t e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, e);
    }

    /** Check if entity has the provided parent.
     *
     * @param parent The parent id to check.
     * @return True if the entity has the provided parent id, false otherwise.
     */
    bool has_childof(entity_t parent) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ECS_CHILDOF | parent);
    }    

    /** Check if entity has the provided base.
     *
     * @param base The entity id to check.
     * @return True if the entity has the provided base id, false otherwise.
     */
    bool has_instanceof(entity_t base) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ECS_INSTANCEOF | base);
    }

    /** Check if entity has the provided type.
     *
     * @param entity The type pointer to check.
     * @return True if the entity has the provided type, false otherwise.
     */
    bool has(type_t type) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_type(m_world, m_id, type);
    }

    /** Check if entity has the provided entity.
     *
     * @param entity The entity to check.
     * @return True if the entity has the provided entity, false otherwise.
     */
    bool has(const entity& e) const {
        return has(e.id());
    }

    /** Check if entity has the provided parent.
     *
     * @param parent The entity to check.
     * @return True if the entity has the provided parent, false otherwise.
     */
    bool has_childof(const entity& parent) const {
        return has_childof(parent.id());
    }  

    /** Check if entity has the provided base.
     *
     * @param base The entity to check.
     * @return True if the entity has the provided base, false otherwise.
     */
    bool has_instanceof(const entity& base) const {
        return has_instanceof(base.id());
    }        

    /** Check if entity has the provided component.
     *
     * @tparam T The component to check.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename T>
    bool has() const {
        return has(_::component_info<T>::id(m_world));
    }

    /** Check if entity owns the provided entity id.
     * An entity id is owned if it is not shared from a base entity.
     *
     * @param entity The entity id to check.
     * @return True if the entity owns the provided entity id, false otherwise.
     */
    bool owns(entity_t e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_owns_entity(m_world, m_id, e, true);
    }

    /** Check if entity owns the provided type.
     * An type is owned if it is not shared from a base entity.
     *
     * @param type The type to check.
     * @return True if the entity owns the provided type, false otherwise.
     */
    bool owns(type_t type) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_type_owns_type(m_world, ecs_get_type(m_world, m_id), type, true);
    }

    /** Check if entity owns the provided entity.
     * An entity is owned if it is not shared from a base entity.
     *
     * @param entity The entity to check.
     * @return True if the entity owns the provided entity, false otherwise.
     */
    bool owns(const entity& e) const {
        return owns(e.id());
    }

    /** Check if entity owns the provided component.
     * An component is owned if it is not shared from a base entity.
     *
     * @tparam T The component to check.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename T>
    bool owns() const {
        return owns(_::component_info<T>::id(m_world));
    }

    /** Check if entity has the provided trait.
     *
     * @tparam T The trait to check.
     * @tparam C The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    template<typename T, typename C>
    bool has_trait() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            _::component_info<C>::id(m_world), 
            _::component_info<T>::id(m_world)));
    }

    /** Check if entity has the provided trait.
     *
     * @tparam T The trait to check.
     * @param component The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    template<typename T>
    bool has_trait(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            component.id(), _::component_info<T>::id(m_world)));
    }

    /** Check if entity has the provided trait tag.
     * The provided trait tag should not be a component.
     *
     * @tparam C The component for which to check the trait tag.
     * @param trait The trait tag to check.
     * @return True if the entity has the provided trait tag, false otherwise.
     */
    template<typename C>
    bool has_trait_tag(flecs::entity trait) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
           _::component_info<C>::id(m_world), trait.id()));
    }

    /** Check if entity has the provided trait.
     * The provided trait should not be a component.
     *
     * @param trait The trait to check.
     * @param component The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    bool has_trait(flecs::entity trait, flecs::entity e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            e.id(), trait.id()));
    }

    /** Check if entity has the provided switch.
     *
     * @param sw The switch to check.
     * @return True if the entity has the provided switch, false otherwise.
     */
    bool has_switch(flecs::type sw) const;

    /** Check if entity has the provided case id.
     *
     * @param sw_case The case id to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(entity_t sw_case) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, flecs::Case | sw_case);
    }

    /** Check if entity has the provided case.
     *
     * @param sw_case The case to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(flecs::entity sw_case) const {
        return this->has_case(sw_case.id());
    }

    template<typename T>
    bool has_case() const {
        return this->has_case(_::component_info<T>::id(m_world));
    }

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return True if the entity has the provided case, false otherwise.
     */
    flecs::entity get_case(flecs::type sw) const;

    /** Test if component is enabled.
     *
     * @tparam T The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    template<typename T>
    bool is_enabled() {
        return ecs_is_component_enabled_w_entity(
            m_world, m_id, _::component_info<T>::id(m_world));
    }

    /** Test if component is enabled.
     *
     * @param id The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(flecs::entity_t id) {
        return ecs_is_component_enabled_w_entity(
            m_world, m_id, id);
    }

    /** Test if component is enabled.
     *
     * @param entity The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity& e) {
        return is_enabled(e.id());
    }

    /** Get current delta time.
     * Convenience function so system implementations can get delta_time, even
     * if they are using the .each() function.
     *
     * @return Current delta_time.
     */
    FLECS_FLOAT delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    /** Return iterator to entity children.
     * Enables depth-first iteration over entity children.
     *
     * @return Iterator to child entities.
     */
    child_iterator children() const;

    /** Used by builder class. Do not invoke. */
    template <typename Func>
    void invoke(Func&& action) const {
        action(m_world, m_id);
    }    

protected:

    world_t *m_world;
    entity_t m_id; 
};

/** Prefab class */
class prefab final : public entity {
public:
    explicit prefab(const flecs::world& world, const char *name = nullptr) 
        : entity(world, name)
    {
        this->add(flecs::Prefab);
    }
};


////////////////////////////////////////////////////////////////////////////////
//// A collection of component ids used to describe the contents of a table
////////////////////////////////////////////////////////////////////////////////

class type final : entity {
public:
    explicit type(const flecs::world& world, const char *name = nullptr, const char *expr = nullptr)
        : entity(world, ecs_new_type(world.c_ptr(), 0, name, expr))
    { 
        sync_from_flecs();
    }

    type(const flecs::world& world, type_t t)
        : entity( world.c_ptr(), 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type(world_t *world, type_t t)
        : entity( world, 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type& add(const type& t) {
        m_type = ecs_type_add(m_world, m_type, t.id());
        m_normalized = ecs_type_merge(m_world, m_normalized, t.c_ptr(), nullptr);
        sync_from_me();
        return *this;
    }

    type& add(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id());
        m_normalized = ecs_type_add(m_world, m_normalized, e.id());
        sync_from_me();
        return *this;
    }

    type& add_instanceof(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id() | ECS_INSTANCEOF);
        m_normalized = ecs_type_add(m_world, m_normalized, e.id() | ECS_INSTANCEOF);
        sync_from_me();
        return *this;
    }

    type& add_childof(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id() | ECS_CHILDOF);
        m_normalized = ecs_type_add(m_world, m_normalized, e.id() | ECS_CHILDOF);
        sync_from_me();
        return *this;
    }

    template <typename T, typename C>
    type& add_trait() {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(_::component_info<C>::id(m_world),
                      _::component_info<T>::id(m_world)));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(_::component_info<C>::id(m_world),
                      _::component_info<T>::id(m_world)));
        
        sync_from_me();
        return *this;
    }

    template <typename T>
    type& add_trait(flecs::entity component) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(component.id(),
                      _::component_info<T>::id(m_world)));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(component.id(),
                      _::component_info<T>::id(m_world)));
        
        sync_from_me();
        return *this;
    }

    type& add_trait(flecs::entity trait, flecs::entity component) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(component.id(), trait.id()));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(component.id(), trait.id()));
        
        sync_from_me();
        return *this;
    }      

    template <typename C>
    type& add_trait_tag(flecs::entity trait) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(_::component_info<C>::id(), trait.id()));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(_::component_info<C>::id(), trait.id()));
        
        sync_from_me();
        return *this;
    }            

    template <typename ... Components>
    type& add() {
        std::stringstream str;
        if (!_::pack_args_to_string<Components...>(m_world, str)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        std::string expr = str.str();
        ecs_type_t t = ecs_type_from_str(m_world, expr.c_str());
        m_type = ecs_type_merge(m_world, m_type, t, nullptr);
        m_normalized = ecs_type_merge(m_world, m_normalized, t, nullptr);
        sync_from_me();

        return *this;
    }    

    std::string str() const {
        char *str = ecs_type_str(m_world, m_type);
        std::string result(str);
        ecs_os_free(str);
        return result;
    }

    type_t c_ptr() const {
        return m_type;
    }

    // Expose entity id without making the entity class public.
    entity_t id() const {
        return m_id;
    }

    type_t c_normalized() const {
        return m_normalized;
    }

    void enable() const {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        ecs_enable(m_world, m_id, true);
    }

    void disable() const {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        ecs_enable(m_world, m_id, false);
    }

    flecs::vector<entity_t> vector() {
        return flecs::vector<entity_t>( (ecs_vector_t*)m_normalized );
    }

private:
    void sync_from_me() {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        EcsType *tc = ecs_get_mut(m_world, m_id, EcsType, NULL);
        if (tc) {
            tc->type = m_type;
            tc->normalized = m_normalized;
        }
    }

    void sync_from_flecs() {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        EcsType *tc = ecs_get_mut(m_world, m_id, EcsType, NULL);
        if (tc) {
            m_type = tc->type;
            m_normalized = tc->normalized;
        }
    }    

    type_t m_type;
    type_t m_normalized;
};


////////////////////////////////////////////////////////////////////////////////
//// Entity range, allows for operating on a range of consecutive entities
////////////////////////////////////////////////////////////////////////////////

class entity_range final : public entity_builder<entity_range> {
public:
    entity_range(const world& world, std::int32_t count) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, nullptr, count))
        , m_count(count) { }

    entity_range(const world& world, std::int32_t count, flecs::type type) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, type.c_ptr(), count))
        , m_count(count) { }

    template <typename Func>
    void invoke(Func&& action) const {
        for (int i = 0; i < m_count; i ++) {
            action(m_world, m_ids[i]);
        }
    }

private:
    world_t *m_world;
    const entity_t *m_ids;
    std::int32_t m_count;
};


////////////////////////////////////////////////////////////////////////////////
//// Register component, provide global access to component handles / metadata
////////////////////////////////////////////////////////////////////////////////

namespace _
{
    // Trick to obtain typename from type, as described here
    // https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
    //
    // The code from the link has been modified to work with more types, and across
    // multiple compilers.
    //
    struct name_util {
        /* Remove parts from typename that aren't needed for component name */
        static void trim_name(char *typeName) {
            ecs_size_t len = ecs_os_strlen(typeName);
            
            /* Remove 'const' */
            ecs_size_t const_len = ecs_os_strlen("const ");
            if ((len > const_len) && !ecs_os_strncmp(typeName, "const ", const_len)) {
                ecs_os_memmove(typeName, typeName + const_len, len - const_len);
                typeName[len - const_len] = '\0';
                len -= const_len;
            }

            /* Remove 'struct' */
            ecs_size_t struct_len = ecs_os_strlen("struct ");
            if ((len > struct_len) && !ecs_os_strncmp(typeName, "struct ", struct_len)) {
                ecs_os_memmove(typeName, typeName + struct_len, len - struct_len);
                typeName[len - struct_len] = '\0';
                len -= struct_len;
            }

            /* Remove 'class' */
            ecs_size_t class_len = ecs_os_strlen("class ");
            if ((len > class_len) && !ecs_os_strncmp(typeName, "class ", class_len)) {
                ecs_os_memmove(typeName, typeName + class_len, len - class_len);
                typeName[len - class_len] = '\0';
                len -= class_len;
            }            

            while (typeName[len - 1] == ' ' ||
                   typeName[len - 1] == '&' ||
                   typeName[len - 1] == '*') 
            {
                len --;
                typeName[len] = '\0';
            }

            /* Remove const at end of string */
            if (len > const_len) {
                if (!ecs_os_strncmp(&typeName[len - const_len], " const", const_len)) {
                    typeName[len - const_len] = '\0';
                }
            }
        }
    };

// Compiler-specific conversion from __PRETTY_FUNCTION__ to component name. 
// This code uses a trick that instantiates a function for the component type. 
// Then __PRETTY_FUNCTION__ is used to obtain the name of the function. Because
// the result of __PRETTY_FUNCTION__ is not standardized, there are different
// implementations for clang, gcc and msvc. Code that uses a different compiler
// needs to register component names explicitly.
#if defined(__clang__)
  static const unsigned int FRONT_SIZE = sizeof("static const char* flecs::_::name_helper<") - 1u;
  static const unsigned int BACK_SIZE = sizeof(">::name() [T = ]") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = (sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE) / 2 + 1u;
      static char typeName[size + 6] = {};
      memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };    
#elif defined(__GNUC__)
  static const unsigned int FRONT_SIZE = sizeof("static const char* flecs::_::name_helper<T>::name() [with T = ") - 1u;
  static const unsigned int BACK_SIZE = sizeof("]") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE;
      static char typeName[size + 6] = {};
      memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };
#elif defined(_WIN32)
  static const unsigned int FRONT_SIZE = sizeof("flecs::_::name_helper<") - 1u;
  static const unsigned int BACK_SIZE = sizeof(">::name") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
      static char typeName[size + 6] = {};
      memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a langauge-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
struct symbol_helper
{
    static char* symbol(void) {
        const char *name = name_helper<T>::name();

        // Symbol is same as name, but with '::' replaced with '.'
        char *ptr, *sym = ecs_os_strdup(name);
        ecs_size_t i, len = ecs_os_strlen(sym);
        ptr = sym;
        for (i = 0, ptr = sym; i < len && *ptr; i ++, ptr ++) {
            if (*ptr == ':') {
                sym[i] = '.';
                ptr ++;
            } else {
                sym[i] = *ptr;
            }
        }

        sym[i] = '\0';

        return sym;
    }
};

// The following functions are lifecycle callbacks that are automatically
// registered with flecs to ensure component lifecycle is handled correctly. Not
// all types require this, yet callbacks are registered by default, which
// introduces some overhead when working with components.
//
// An application can optimize this by explicitly registering a component as a
// plain old datatype, with world.pod_component<T>().

template <typename T>
void component_ctor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_ptr = static_cast<T*>(ptr);
    
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&t_ptr[i], T);
    }
} 

template <typename T>
void component_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_ptr = static_cast<T*>(ptr);
    
    for (int i = 0; i < count; i ++) {
        t_ptr[i].~T();
    }
}

template <typename T>
void component_copy(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    const void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)dst_entity;
    (void)src_entity;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_dst_ptr = static_cast<T*>(dst_ptr);
    const T *t_src_ptr = static_cast<const T*>(src_ptr);
    
    for (int i = 0; i < count; i ++) {
        t_dst_ptr[i] = t_src_ptr[i];
    }
}

template <typename T>
void component_move(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)dst_entity;
    (void)src_entity;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_dst_ptr = static_cast<T*>(dst_ptr);
    T *t_src_ptr = static_cast<T*>(src_ptr);
    
    for (int i = 0; i < count; i ++) {
        t_dst_ptr[i] = std::move(t_src_ptr[i]);
    }
}

// Register component lifecycle callbacks with flecs.
template<typename T>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component,
    bool ctor,
    bool dtor,
    bool copy,
    bool move)
{
    if (!ecs_component_has_actions(world, component)) {
        EcsComponentLifecycle cl{};
        if (ctor) {
            cl.ctor = _::component_ctor<
                typename std::remove_const<
                    typename std::remove_pointer<T>::type>::type>;
        }
        if (dtor) {
            cl.dtor = _::component_dtor<
                typename std::remove_const<
                    typename std::remove_pointer<T>::type>::type>;
        }
        if (copy) {
            cl.copy = _::component_copy<
                typename std::remove_const<
                    typename std::remove_pointer<T>::type>::type>;
        }
        if (move) {
            cl.move = _::component_move<
                typename std::remove_const<
                    typename std::remove_pointer<T>::type>::type>;
        }

        ecs_set_component_actions_w_entity( world, component, &cl);
    }
}

// Class that holds component identifier and name.
// This class is used by most component operations to obtain the component id in
// a transparent way for the application. The component id is stored in a static
// member of a class, and is shared by all worlds in an application. As such,
// each world in an application must (and will) have the same id.
//
// Because of how global (templated) variables are instantiated, it is possible
// that different instances for the same component exist across different
// translation units. This is handled transparently by flecs. When a component
// id is requested from the component_info class, but the id is uninitialized, a 
// lookup by name will be performed for the component on the world, which will 
// return the id with which the component was already registered. This means 
// component identifiers are eventually consistent across translation units.
//
// When a component id is requested for a world that had not yet registered the
// component, it will be registered with the existing identifier. This ensures
// that multiple worlds in the same application use the same component ids.
//
// There are a few limitations of this approach.
//
// 1) When two worlds register components in different orders, it is possible
//    that different components receive the same identifier. When a world 
//    attempts to re-register a component with a different identifier, an error
//    will be thrown. To prevent this from happening, worlds should register
//    components in the same order.
//
// 2) Component names are used to ensure that different translation units use
//    the same identifiers. If a component is registered under different names
//    in the same application, id conflicts can occur.
//
// Known issues:
//
// It seems like component registration does not always work correctly in Unreal
// engine when recreating a world. A plausible cause for this is the hot 
// reloading of dynamic libraries by the engine. A workaround for this issue is
// to call flecs::_::component_info<T>::reset() before recreating the world.
// This will reset the global component state and avoids conflicts. The exact
// cause of the issue is investigated here: 
//   https://github.com/SanderMertens/flecs/issues/293

template <typename T>
class component_info final {
public:
    // Initialize component identifier
    static void init(world_t* world, entity_t entity, bool allow_tag = true) {
        // If an identifier was already set, check for consistency
        if (s_id) {
            // If an identifier was registered, a name should've been registered
            // as well.
            ecs_assert(s_name.c_str() != nullptr, ECS_INTERNAL_ERROR, NULL);

            // A component cannot be registered using a different identifier.
            ecs_assert(s_id == entity, ECS_INCONSISTENT_COMPONENT_ID, 
                _::name_helper<T>::name());

            ecs_assert(allow_tag == s_allow_tag, ECS_INTERNAL_ERROR, NULL);

            // Component was already registered and data is consistent with new
            // identifier, so nothing else to be done.
            return;
        }

        // Component wasn't registered yet, set the values. Register component
        // name as the fully qualified flecs path.
        char *path = ecs_get_fullpath(world, entity);
        s_id = entity;
        s_name = path;
        s_allow_tag = allow_tag;

        // s_name is an std::string, so it will have made a copy
        ecs_os_free(path);
    }

    // Obtain a component identifier without registering lifecycle callbacks.
    static entity_t id_no_lifecycle(world_t *world = nullptr, 
        const char *name = nullptr, bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            const char *n = _::name_helper<T>::name();
            
            if (!name) {
                // If no name was provided, retrieve the name implicitly from
                // the name_helper class.
                name = n;
            }

            s_allow_tag = allow_tag;

            // If no world was provided, we can't implicitly register the 
            // component. While there are a few cases where calling this 
            // function without a world is OK, in general functions should 
            // always provide a world to enable implicit registration.
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);

            // Create entity with name first. This ensurs that the component is
            // created in the right location in the hierarchy, in case a scoped
            // name is used. This allows registering components with the same
            // name in different namespaces.
            //
            // If the component was already registered for this world, this will 
            // resolve the existing component identifier. This enables 
            // transparent component registeration across multiple translation 
            // units, as long as the same world is used.
            //
            // The last parameter ('true') ensures that when the component was
            // not yet registered, a new component identifier is created. 
            // Component identifiers and entity identifiers are equivalent,
            // though a pool of identifiers at the start of the id range is 
            // reserved for components. This is a performance optimization, as
            // low ids in some parts of the code allow for direct indexing.
            flecs::world w(world);
            flecs::entity result = entity(w, name, true);
            
            // Init the component_info instance with the identiifer.
            init(world, result.id(), allow_tag);

            // Now use the resulting identifier to register the component. Note
            // that the name is not passed into this function, as the entity was
            // already created with the correct name.
            ecs_entity_t entity = ecs_new_component(
                world, result.id(), nullptr, 
                size(), 
                alignment());

            ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

            // Set the symbol in the Name component to the actual C++ name.
            // Comparing symbols allows for verifying whether a different 
            // component is being registered under the same name. We can't use
            // the name used for registration, because it is possible that a
            // user (erroneously) attempts to register the same datatype with
            // the same name. Without verifying that the actual C++ type name
            // matches, that scenario would go undetected.
            EcsName *name_comp = ecs_get_mut(world, entity, EcsName, NULL);
            char *symbol = symbol_helper<T>::symbol();

            if (name_comp->symbol) {
                ecs_assert( !strcmp(name_comp->symbol, symbol), 
                    ECS_COMPONENT_NAME_IN_USE, name);
            } else {
                name_comp->symbol = symbol;
            }
            
            // The identifier returned by the function should be the same as the
            // identifier that was passed in.
            ecs_assert(entity == result.id(), ECS_INTERNAL_ERROR, NULL);

        } else if (world && !ecs_exists(world, s_id)) {
            const char *n = _::name_helper<T>::name();
            
            if (!name) {
                // If no name was provided, retrieve the name implicitly from
                // the name_helper class.
                name = n;
            }

            ecs_entity_t entity = ecs_new_component(
                world, s_id, name,
                size(), 
                alignment());
                
            (void)entity;

            ecs_assert(entity == s_id, ECS_INTERNAL_ERROR, NULL);

            init(world, s_id, allow_tag);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component identifier, register lifecycle callbacks if this is
    // the first time the component is used.
    static entity_t id(world_t *world = nullptr, const char *name = nullptr, 
        bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            // This will register a component id, but will not register 
            // lifecycle callbacks.
            id_no_lifecycle(world, name, allow_tag);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size()) {
                register_lifecycle_actions<T>(world, s_id,
                    true, true, true, true);
            }
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component name
    static const char* name(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // If the id is set, the name should also have been set
        return s_name.c_str();
    }

    // Obtain a component name, don't register lifecycle if the component hadn't
    // been registered yet. While functionally the same could be achieved by
    // first calling id_no_lifecycle() and then name(), this function ensures
    // that the lifecycle callback templates are not instantiated. This allows
    // some types (such as module classes) to be created without a default
    // constructor.
    static const char* name_no_lifecycle(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id_no_lifecycle(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // Return 
        return s_name.c_str();
    }    

    // Return the type of a component.
    // The type is a vector of component ids. This will return a type with just
    // the current component id.
    static type_t type(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);        

        // Create a type from the component id.
        if (!s_type) {
            s_type = ecs_type_from_entity(world, s_id);
        }

        ecs_assert(s_type != nullptr, ECS_INTERNAL_ERROR, NULL);

        return s_type;
    }

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        
        // C++ types that have no members still have a size. Use std::is_empty
        // to check if the type is empty. If so, use 0 for the component size.
        //
        // If s_allow_tag is set to false, the size returned by C++ is used.
        // This is useful in cases where class instances are still required, as
        // is the case with module classes.
        if (s_allow_tag && std::is_empty<T>::value) {
            return 0;
        } else {
            return sizeof(typename std::remove_pointer<T>::type);
        }
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        if (size() == 0) {
            return 0;
        } else {
            return alignof(typename std::remove_pointer<T>::type);
        }
    }

    // Was the component already registered.
    static bool registered() {
        return s_id != 0;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_type = NULL;
        s_name.clear();
    }

private:
    static entity_t s_id;
    static type_t s_type;
    static std::string s_name;
    static std::string s_symbol;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t component_info<T>::s_id( 0 );
template <typename T> type_t component_info<T>::s_type( nullptr );
template <typename T> std::string component_info<T>::s_name("");
template <typename T> bool component_info<T>::s_allow_tag( true );


////////////////////////////////////////////////////////////////////////////////
//// Register a component with flecs
////////////////////////////////////////////////////////////////////////////////

} // namespace _

/** Plain old datatype, no lifecycle actions are registered */
template <typename T>
flecs::entity pod_component(const flecs::world& world, const char *name = nullptr, bool allow_tag = true) {
    if (!name) {
        name = _::name_helper<T>::name();
    }

    world_t *world_ptr = world.c_ptr();
    entity_t id = 0;

    if (_::component_info<T>::registered()) {
        /* Obtain component id. Because the component is already registered,
         * this operation does nothing besides returning the existing id */
        id = _::component_info<T>::id_no_lifecycle(world_ptr, name, allow_tag);

        /* If entity is not empty check if the name matches */
        if (ecs_get_type(world_ptr, id) != nullptr) {
            if (id >= EcsFirstUserComponentId) {
                char *path = ecs_get_path_w_sep(
                    world_ptr, 0, id, 0, "::", nullptr);
                ecs_assert(!strcmp(path, name), 
                    ECS_INCONSISTENT_COMPONENT_NAME, name);
                ecs_os_free(path);
            }
        }

        /* Register name with entity, so that when the entity is created the
         * correct id will be resolved from the name. */
        ecs_add_path_w_sep(world_ptr, id, 0, name, "::", "::");

        /* If a component was already registered with this id but with a 
         * different size, the ecs_new_component function will fail. */

        /* We need to explicitly call ecs_new_component here again. Even though
         * the component was already registered, it may have been registered
         * with a different world. This ensures that the component is registered
         * with the same id for the current world. 
         * If the component was registered already, nothing will change. */
        ecs_entity_t entity = ecs_new_component(
            world.c_ptr(), id, nullptr, 
            _::component_info<T>::size(), 
            _::component_info<T>::alignment());
        
        (void)entity;
        
        ecs_assert(entity == id, ECS_INTERNAL_ERROR, NULL);

        /* This functionality could have been put in id_no_lifecycle, but since
         * this code happens when a component is registered, and the entire API
         * calls id_no_lifecycle, this would add a lot of overhead to each call.
         * This is why when using multiple worlds, components should be 
         * registered explicitly. */
    } else {
        /* If the component is not yet registered, ensure no other component
         * or entity has been registered with this name */
        ecs_entity_t entity = ecs_lookup_fullpath(world_ptr, name);

        /* If entity exists, compare symbol name to ensure that the component
         * we are trying to register under this name is the same */
        if (entity) {
            const EcsName *name_comp = ecs_get_mut(world.c_ptr(), entity, EcsName, NULL);
            ecs_assert(name_comp != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(name_comp->symbol != NULL, ECS_INTERNAL_ERROR, NULL);

            const char *symbol = _::name_helper<T>::name();

            ecs_assert(!strcmp(name_comp->symbol, symbol), 
                ECS_COMPONENT_NAME_IN_USE, name);

            (void)name_comp;
            (void)symbol;
        }

        /* Register id as usual */
        id = _::component_info<T>::id_no_lifecycle(world_ptr, name, allow_tag);
    }

    _::component_info<T>::init(world_ptr, id, allow_tag);
    _::component_info<const T>::init(world_ptr, id, allow_tag);
    _::component_info<T*>::init(world_ptr, id, allow_tag);
    _::component_info<T&>::init(world_ptr, id, allow_tag);
    
    return world.entity(id);
}

/** Regular component with ctor, dtor copy and move actions */
template <typename T>
flecs::entity component(const flecs::world& world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    if (_::component_info<T>::size()) {
        _::register_lifecycle_actions<T>(world.c_ptr(), result.id(),
            true, true, true, true);
    }

    return result;
}

/** Trivially relocatable component that can be memcpy'd. */
template <typename T>
flecs::entity relocatable_component(const flecs::world& world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    _::register_lifecycle_actions<T>(world.c_ptr(), result.id(),
        true, true, true, false);

    return result;
}

template <typename T>
flecs::entity_t type_id() {
    return _::component_info<T>::id();
}


////////////////////////////////////////////////////////////////////////////////
//// Define a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
flecs::entity module(const flecs::world& world, const char *name = nullptr) {
    ecs_set_scope(world.c_ptr(), 0);
    flecs::entity result = pod_component<T>(world, name, false);
    ecs_set_scope(world.c_ptr(), result.id());
    return result;
}


////////////////////////////////////////////////////////////////////////////////
//// Import a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
ecs_entity_t do_import(world& world) {
    ecs_trace_1("import %s", _::name_helper<T>::name());
    ecs_log_push();

    ecs_entity_t scope = ecs_get_scope(world.c_ptr());

    // Allocate module, so the this ptr will remain stable
    // TODO: make sure memory is cleaned up with world
    T *module_data = FLECS_NEW(T)(world);

    ecs_set_scope(world.c_ptr(), scope);

    // It should now be possible to lookup the module
    char *symbol = _::symbol_helper<T>::symbol();
    ecs_entity_t m = ecs_lookup_symbol(world.c_ptr(), symbol);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_os_free(symbol);

    _::component_info<T>::init(world.c_ptr(), m, false);

    ecs_assert(_::component_info<T>::size() != 0, ECS_INTERNAL_ERROR, NULL);

    // Set module singleton component

    ecs_set_ptr_w_entity(
        world.c_ptr(), m,
        _::component_info<T>::id_no_lifecycle(world.c_ptr()), 
        _::component_info<T>::size(),
        module_data);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::name_helper<T>::name();

    ecs_entity_t m = ecs_lookup_symbol(world.c_ptr(), symbol);
    
    if (!_::component_info<T>::registered()) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::component_info<T>::init(world.c_ptr(), m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = do_import<T>(world);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = do_import<T>(world);
    }

    return flecs::entity(world, m);
}


////////////////////////////////////////////////////////////////////////////////
//// A filter is used to match subsets of tables
////////////////////////////////////////////////////////////////////////////////
 
class filter {
public:
    filter() 
        : m_world( nullptr )
        , m_filter{ } {}

    explicit filter(const world& world) 
        : m_world( world.c_ptr() )
        , m_filter{ } { }

    filter& include(type type) {
        m_filter.include = ecs_type_merge(m_world, m_filter.include, type.c_ptr(), nullptr);
        return *this;
    }

    filter& include(entity entity) {
        m_filter.include = ecs_type_add(m_world, m_filter.include, entity.id());
        return *this;
    }

    template <typename T>
    filter& include() {
        m_filter.include = ecs_type_add(m_world, m_filter.include, _::component_info<T>::id(m_world));
        return *this;
    }

    filter& include_kind(match_kind kind) {
        m_filter.include_kind = static_cast<ecs_match_kind_t>(kind);
        return *this;
    }

    type include() {
        return type(m_world, m_filter.include);
    }

    filter& exclude(type type) {
        m_filter.exclude = ecs_type_merge(m_world, m_filter.exclude, type.c_ptr(), nullptr);
        return *this;
    }

    filter& exclude(entity entity) {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, entity.id());
        return *this;
    }

    template <typename T>
    filter& exclude() {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, _::component_info<T>::id(m_world));
        return *this;
    }
 
    filter& exclude_kind(match_kind kind) {
        m_filter.exclude_kind = static_cast<ecs_match_kind_t>(kind);
        return *this;
    }

    type exclude() {
        return type(m_world, m_filter.exclude);
    }  

    const filter_t* c_ptr() const {
        if (m_world) {
            return &m_filter;
        } else {
            return nullptr;
        }
    }

private:
    world_t *m_world;
    filter_t m_filter;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility to convert template argument pack to array of columns
////////////////////////////////////////////////////////////////////////////////

namespace _ {

template <typename ... Components>
class column_args {
public:    
    struct Column {
        void *ptr;
        bool is_shared;
    };

    using Columns = std::array<Column, sizeof...(Components)>;

    column_args(ecs_iter_t* iter) {
        populate_columns(iter, 0, (typename std::remove_reference<typename std::remove_pointer<Components>::type>::type*)nullptr...);
    }

    Columns m_columns;

private:
    /* Dummy function when last component has been added */
    void populate_columns(ecs_iter_t *iter, size_t index) { 
        (void)iter;
        (void)index;
    }

    /* Populate columns array recursively */
    template <typename T, typename... Targs>
    void populate_columns(ecs_iter_t *iter, size_t index, T comp, Targs... comps) {
        int32_t column = static_cast<int32_t>(index + 1);
        void *ptr = ecs_column_w_size(iter, sizeof(*comp), column);
        m_columns[index].ptr = ptr;
        m_columns[index].is_shared = !ecs_is_owned(iter, column) && ptr != nullptr;
        populate_columns(iter, index + 1, comps ...);
    }
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class each_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit each_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit each_invoker(const Func& func) noexcept : m_func(func) { }

    // Invoke system
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        flecs::iter iter_wrapper(iter);
        (void)index;
        (void)columns;

        // Use any_column so we can transparently use shared components
        for (auto row : iter_wrapper) {
            func(iter_wrapper.entity(row), (_::any_column<typename std::remove_reference<Components>::type>(
                 (typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr, 
                    static_cast<size_t>(iter->count), comps.is_shared))[row]...);
        }
    }

    // Add components one by one to parameter pack
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        each_invoker::call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    // Callback provided to flecs system
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        each_invoker *self = (each_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class action_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit action_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit action_invoker(const Func& func) noexcept : m_func(func) { }

    /* Invoke system */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, int index, Columns& columns, Targs... comps) {
        (void)index;
        (void)columns;

        flecs::iter iter_wrapper(iter);
        
        func(iter_wrapper, (column<typename std::remove_reference< typename std::remove_pointer<Components>::type >::type>(
            (typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr, iter->count, comps.is_shared))...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, int index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        action_invoker *self = (action_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class iter_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit iter_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit iter_invoker(const Func& func) noexcept : m_func(func) { }

    /* Invoke system */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        (void)index;
        (void)columns;
        flecs::iter iter_wrapper(iter);
        func(iter_wrapper, ((typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr)...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        iter_invoker *self = (iter_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};

} // namespace _


////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

class query_base {
public:
    /** Get pointer to C query object.
     */
    query_t* c_ptr() const {
        return m_query;
    }

    /** Sort the output of a query.
     * This enables sorting of entities across matched tables. As a result of this
     * operation, the order of entities in the matched tables may be changed. 
     * Resorting happens when a query iterator is obtained, and only if the table
     * data has changed.
     *
     * If multiple queries that match the same (sub)set of tables specify different 
     * sorting functions, resorting is likely to happen every time an iterator is
     * obtained, which can significantly slow down iterations.
     *
     * The sorting function will be applied to the specified component. Resorting
     * only happens if that component has changed, or when the entity order in the
     * table has changed. If no component is provided, resorting only happens when
     * the entity order changes.
     *
     * @tparam T The component used to sort.
     * @param compare The compare function used to sort the components.
     */
    template <typename T>
    void order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_query_order_by(m_world, m_query, 
            flecs::_::component_info<T>::id(m_world),
            (ecs_compare_action_t)compare);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */
    void order_by(flecs::entity component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        ecs_query_order_by(m_world, m_query, component.id(), compare);
    }    

    /** Group and sort matched tables.
     * Similar yo ecs_query_order_by, but instead of sorting individual entities, this
     * operation only sorts matched tables. This can be useful of a query needs to
     * enforce a certain iteration order upon the tables it is iterating, for 
     * example by giving a certain component or tag a higher priority.
     *
     * The sorting function assigns a "rank" to each type, which is then used to
     * sort the tables. Tables with higher ranks will appear later in the iteration.
     * 
     * Resorting happens when a query iterator is obtained, and only if the set of
     * matched tables for a query has changed. If table sorting is enabled together
     * with entity sorting, table sorting takes precedence, and entities will be
     * sorted within each set of tables that are assigned the same rank.
     *
     * @tparam T The component used to determine the group rank.
     * @param rank The rank action.
     */
    template <typename T>
    void group_by(int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_query_group_by(m_world, m_query, 
            flecs::_::component_info<T>::id(m_world), rank);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    void group_by(flecs::entity component, int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_query_group_by(m_world, m_query, component.id(), rank);
    }

    /** Returns whether the query data changed since the last iteration.
     * This operation must be invoked before obtaining the iterator, as this will
     * reset the changed state. The operation will return true after:
     * - new entities have been matched with
     * - matched entities were deleted
     * - matched components were changed
     * 
     * @return true if entities changed, otherwise false.
     */
    bool changed() {
        return ecs_query_changed(m_query);
    }

    /** Returns whether query is orphaned.
     * When the parent query of a subquery is deleted, it is left in an orphaned
     * state. The only valid operation on an orphaned query is deleting it. Only
     * subqueries can be orphaned.
     *
     * @return true if query is orphaned, otherwise false.
     */
    bool orphaned() {
        return ecs_query_orphaned(m_query);
    }

    /** Free the query.
     */
    void destruct() {
        ecs_query_free(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

protected:
    world_t *m_world;
    query_t *m_query;
};

template<typename ... Components>
class query : public query_base {
    using Columns = typename _::column_args<Components...>::Columns;

public:
    query() { 
        m_world = nullptr;
        m_query = nullptr;
    }

    explicit query(const world& world) {
        std::stringstream str;
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        m_world = world.c_ptr();
        m_query = ecs_query_new(world.c_ptr(), str.str().c_str());
    }

    explicit query(const world& world, query_base& parent) {
        std::stringstream str;
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        m_world = world.c_ptr();
        m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), str.str().c_str());
    }

    explicit query(const world& world, const char *expr) {
        std::stringstream str;
        m_world = world.c_ptr();
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            m_query = ecs_query_new(world.c_ptr(), expr);
        } else {
            str << "," << expr;
            m_query = ecs_query_new(world.c_ptr(), str.str().c_str());
        }
    }

    explicit query(const world& world, query_base& parent, const char *expr) {
        std::stringstream str;
        m_world = world.c_ptr();
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), expr);
        } else {
            str << "," << expr;
            m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), str.str().c_str());
        }
    }

    query_iterator<Components...> begin() const;

    query_iterator<Components...> end() const;

    template <typename Func>
    void each(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::each_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }

    /* DEPRECATED */
    template <typename Func>
    void action(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::action_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }

    template <typename Func>
    void iter(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::iter_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Snapshots make a copy of the world state that can be restored
////////////////////////////////////////////////////////////////////////////////

class snapshot final {
public:
    explicit snapshot(const world& world)
        : m_world( world )
        , m_snapshot( nullptr ) { }

    snapshot(const snapshot& obj) 
        : m_world( obj.m_world )
    { 
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot, nullptr);
        m_snapshot = ecs_snapshot_take_w_iter(&it, ecs_snapshot_next);
    }

    snapshot(snapshot&& obj) 
        : m_world(obj.m_world)
        , m_snapshot(obj.m_snapshot)
    {
        obj.m_snapshot = nullptr;
    }

    snapshot& operator=(const snapshot& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot, nullptr);
        m_snapshot = ecs_snapshot_take_w_iter(&it, ecs_snapshot_next);        
        return *this;
    }

    snapshot& operator=(snapshot&& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        m_snapshot = obj.m_snapshot;
        obj.m_snapshot = nullptr;
        return *this;
    }

    void take() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        m_snapshot = ecs_snapshot_take(m_world.c_ptr());
    }

    void take(flecs::filter filter) {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        ecs_iter_t it = ecs_filter_iter(m_world.c_ptr(), filter.c_ptr());
        m_snapshot = ecs_snapshot_take_w_iter(
            &it, ecs_filter_next);
    }

    void restore() {
        if (m_snapshot) {
            ecs_snapshot_restore(m_world.c_ptr(), m_snapshot);
            m_snapshot = nullptr;
        }
    }

    ~snapshot() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }
    }

    snapshot_t* c_ptr() const {
        return m_snapshot;
    }

    snapshot_filter filter(const filter& filter);

    filter_iterator begin();

    filter_iterator end();
private:
    const world& m_world;
    snapshot_t *m_snapshot;
};


////////////////////////////////////////////////////////////////////////////////
//// Fluent interface to run a system manually
////////////////////////////////////////////////////////////////////////////////

class system_runner_fluent {
public:
    system_runner_fluent(world_t *world, entity_t id, FLECS_FLOAT delta_time, void *param)
        : m_world(world)
        , m_id(id)
        , m_delta_time(delta_time)
        , m_param(param)
        , m_filter()
        , m_offset(0)
        , m_limit(0) { }

    system_runner_fluent& filter(filter filter) {
        m_filter = filter;
        return *this;
    }

    system_runner_fluent& offset(std::int32_t offset) {
        m_offset = offset;
        return *this;
    }

    system_runner_fluent& limit(std::int32_t limit) {
        m_limit = limit;
        return *this;
    }

    ~system_runner_fluent() {
        ecs_run_w_filter(
            m_world, m_id, m_delta_time, m_offset, m_limit, m_filter.c_ptr(), m_param);
    }
private:
    world_t *m_world;
    entity_t m_id;
    FLECS_FLOAT m_delta_time;
    void *m_param;
    flecs::filter m_filter;
    std::int32_t m_offset;
    std::int32_t m_limit;
};


////////////////////////////////////////////////////////////////////////////////
//// Register a system with Flecs
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
class system final : public entity {
public:
    explicit system(const flecs::world& world, const char *name = nullptr, const char *signature = nullptr) 
        : entity(world, name)
        , m_kind(static_cast<ecs_entity_t>(OnUpdate)) 
        , m_signature(signature)
        , m_interval(0.0)
        , m_on_demand(false)
        , m_hidden(false)
        , m_finalized(false) { 
            ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);
        }

    explicit system(const flecs::world& world, flecs::entity id) 
        : entity(world, id.id())
        , m_finalized(true) { }

    system& signature(const char *signature) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(!m_signature, ECS_INVALID_PARAMETER, NULL);
        m_signature = signature;
        return *this;
    }

    system& kind(entity_t kind) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_kind = static_cast<ecs_entity_t>(kind);
        return *this;
    }

    system& interval(FLECS_FLOAT interval) {
        if (!m_finalized) {
            m_interval = interval;
        } else {
            ecs_set_interval(m_world, m_id, interval);
        }
        return *this;
    }

    FLECS_FLOAT interval() {
        return ecs_get_interval(m_world, m_id);
    }

    // DEPRECATED: use interval instead
    system& period(FLECS_FLOAT period) {
        return this->interval(period);
    }

    system& on_demand() {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_on_demand = true;
        return *this;
    }

    system& hidden() {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_hidden = true;
        return *this;
    }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

    void set_period(FLECS_FLOAT period) const {
        ecs_set_interval(m_world, m_id, period);
    }

    void set_context(void *ctx) const {
        EcsContext ctx_value = { ctx };
        ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);
    }

    void* get_context() const {
        const EcsContext *ctx = ecs_get(m_world, m_id, EcsContext);
        if (ctx) {
            return (void*)ctx->ctx;
        } else {
            return NULL;
        }
    }

    system_runner_fluent run(FLECS_FLOAT delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, delta_time, param);
    }

    /* DEPRECATED. Use iter instead. */
    template <typename Func>
    system& action(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        using invoker_t = typename _::action_invoker<typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(invoker_t)(std::forward<Func>(func));        

        create_system(invoker_t::run, false);

        EcsContext ctx_value = {ctx};
        ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);

        return *this;
    }

     /* Iter (or each) is mandatory and always the last thing that 
      * is added in the fluent method chain. Create system signature from both 
      * template parameters and anything provided by the signature method. */
    template <typename Func>
    system& iter(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        using invoker_t = typename _::iter_invoker<typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(invoker_t)(std::forward<Func>(func));

        create_system(invoker_t::run, false);

        EcsContext ctx_value = {ctx};
        ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);

        return *this;
    }    

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    system& each(Func&& func) {
        using invoker_t = typename _::each_invoker<typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(invoker_t)(std::forward<Func>(func));

        create_system(invoker_t::run, true);

        EcsContext ctx_value = {ctx};
        ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);

        return *this;
    }

    ~system() = default;
private:
    template <typename Invoker>
    entity_t create_system(Invoker invoker, bool is_each) {
        ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);

        entity_t e;
        bool is_trigger = m_kind == flecs::OnAdd || m_kind == flecs::OnRemove;

        if (is_trigger) {
            // Don't add ANY source to each function if this is a trigger
            is_each = false;
        }

        std::string signature = build_signature(is_each);

        if (!signature.length()) {
            signature = "0";
        }

        if (is_trigger) {
            e = ecs_new_trigger(
                m_world, 
                m_id,
                nullptr, 
                m_kind, 
                signature.c_str(), 
                invoker);
        } else {
            e = ecs_new_system(
                m_world, 
                m_id,
                nullptr, 
                m_kind, 
                signature.c_str(), 
                invoker);
        }

        ecs_assert(e == m_id, ECS_INTERNAL_ERROR, NULL);

        if (m_interval != 0) {
            ecs_set_interval(m_world, e, m_interval);
        }

        m_finalized = true;

        return e;
    }

    std::string build_signature(bool is_each) {
        bool is_set = false;

        std::stringstream str;
        if (_::pack_args_to_string<Components ...>(m_world, str, is_each)) {
            is_set = true;
        }

        if (m_signature) {
            if (is_set) {
                str << ",";
            }
            str << m_signature;
            is_set = true;
        }

        if (m_hidden) {
            if (is_set) {
                str << ",";
            }            
            str << "SYSTEM:Hidden";
            is_set = true;
        }    

        if (m_on_demand) {
            if (is_set) {
                str << ",";
            }            
            str << "SYSTEM:EcsOnDemand";
            is_set = true;
        } 

        return str.str();       
    }

    ecs_entity_t m_kind;
    const char *m_signature = nullptr;
    FLECS_FLOAT m_interval;
    bool m_on_demand;
    bool m_hidden;
    bool m_finalized; // After set to true, call no more fluent functions
};


////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
class query_iterator
{
public:
    query_iterator()
        : m_has_next(false)
        , m_iter{ } { }

    query_iterator(const query<Components...>& query) 
        : m_iter( ecs_query_iter(query.c_ptr()) )
    {
        m_has_next = ecs_query_next(&m_iter);
    }

    bool operator!=(query_iterator const& other) const {
        return m_has_next != other.m_has_next;
    }

    flecs::iter const operator*() const {
        return flecs::iter(&m_iter);
    }

    query_iterator& operator++() {
        m_has_next = ecs_query_next(&m_iter);
        return *this;
    }

private:
    bool m_has_next;
    ecs_iter_t m_iter;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility for iterating over tables that match a filter
////////////////////////////////////////////////////////////////////////////////

class filter_iterator
{
public:
    filter_iterator(ecs_iter_next_action_t action)
        : m_world(nullptr)
        , m_has_next(false)
        , m_iter{ } 
        , m_action(action) { }

    filter_iterator(const world& world, const filter& filter, ecs_iter_next_action_t action)
        : m_world( world.c_ptr() )
        , m_iter( ecs_filter_iter(m_world, filter.c_ptr()) ) 
        , m_action(action)
    { 
        m_has_next = m_action(&m_iter);
    }

    filter_iterator(const world& world, const snapshot& snapshot, const filter& filter, ecs_iter_next_action_t action) 
        : m_world( world.c_ptr() )
        , m_iter( ecs_snapshot_iter(snapshot.c_ptr(), filter.c_ptr()) )
        , m_action(action)
    {
        m_has_next = m_action(&m_iter);
    }

    bool operator!=(filter_iterator const& other) const {
        return m_has_next != other.m_has_next;
    }

    flecs::iter const operator*() const {
        return flecs::iter(&m_iter);
    }

    filter_iterator& operator++() {
        m_has_next = m_action(&m_iter);
        return *this;
    }

private:
    world_t *m_world;
    bool m_has_next;
    ecs_iter_t m_iter;
    ecs_iter_next_action_t m_action;
};


////////////////////////////////////////////////////////////////////////////////
//// Tree iterator
////////////////////////////////////////////////////////////////////////////////

class tree_iterator
{
public:
    tree_iterator()
        : m_has_next(false)
        , m_iter{ } { }

    tree_iterator(flecs::entity entity) 
        : m_iter( ecs_scope_iter(entity.world().c_ptr(), entity.id()) )
    {
        m_has_next = ecs_scope_next(&m_iter);
    }

    bool operator!=(tree_iterator const& other) const {
        return m_has_next != other.m_has_next;
    }

    flecs::iter const operator*() const {
        return flecs::iter(&m_iter);
    }

    tree_iterator& operator++() {
        m_has_next = ecs_scope_next(&m_iter);
        return *this;
    }

private:
    bool m_has_next;
    ecs_iter_t m_iter;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility for creating a world-based filter iterator
////////////////////////////////////////////////////////////////////////////////

class world_filter {
public:
    world_filter(const world& world, const filter& filter) 
        : m_world( world )
        , m_filter( filter ) { }

    inline filter_iterator begin() const {
        return filter_iterator(m_world, m_filter, ecs_filter_next);
    }

    inline filter_iterator end() const {
        return filter_iterator(ecs_filter_next);
    }

private:
    const world& m_world;
    const filter& m_filter;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility for creating a snapshot-based filter iterator
////////////////////////////////////////////////////////////////////////////////

class snapshot_filter {
public:
    snapshot_filter(const world& world, const snapshot& snapshot, const filter& filter) 
        : m_world( world )
        , m_snapshot( snapshot )
        , m_filter( filter ) { }

    inline filter_iterator begin() const {
        return filter_iterator(m_world, m_snapshot, m_filter, ecs_snapshot_next);
    }

    inline filter_iterator end() const {
        return filter_iterator(ecs_snapshot_next);
    }

private:
    const world& m_world;
    const snapshot& m_snapshot;
    const filter& m_filter;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility for creating a child table iterator
////////////////////////////////////////////////////////////////////////////////

class child_iterator {
public:
    child_iterator(const entity& entity) 
        : m_parent( entity ) { }

    inline tree_iterator begin() const {
        return tree_iterator(m_parent);
    }

    inline tree_iterator end() const {
        return tree_iterator();
    }

private:
    const entity& m_parent;
};


////////////////////////////////////////////////////////////////////////////////
//// Reader for world/snapshot serialization
////////////////////////////////////////////////////////////////////////////////

class reader final {
public:
    explicit reader(world& world) {
        m_reader = ecs_reader_init(world.c_ptr());
    }

    reader(world& world, snapshot& snapshot) {
        (void)world;
        ecs_iter_t it = ecs_snapshot_iter(snapshot.c_ptr(), nullptr);
        m_reader = ecs_reader_init_w_iter(&it, ecs_snapshot_next);
    }

    int32_t read(char *buffer, std::int64_t size) {
        return ecs_reader_read(buffer, static_cast<int32_t>(size), &m_reader);
    }

private:
    ecs_reader_t m_reader;
};


////////////////////////////////////////////////////////////////////////////////
//// Writer for world deserialization
////////////////////////////////////////////////////////////////////////////////

class writer final {
public:
    explicit writer(world& world) {
        m_writer = ecs_writer_init(world.c_ptr());
    }

    int32_t write(const char *buffer, std::int64_t size) {
        return ecs_writer_write(buffer, static_cast<int32_t>(size), &m_writer);
    }

private:
    ecs_writer_t m_writer;
};


////////////////////////////////////////////////////////////////////////////////
//// Filter fwd declared functions
////////////////////////////////////////////////////////////////////////////////

inline snapshot_filter snapshot::filter(const flecs::filter& filter) {
    return snapshot_filter(m_world, *this, filter);
}

inline filter_iterator snapshot::begin() {
    return filter_iterator(m_world, *this, flecs::filter(m_world), ecs_snapshot_next);
}

inline filter_iterator snapshot::end() {
    return filter_iterator(ecs_snapshot_next);
}


////////////////////////////////////////////////////////////////////////////////
//// Query fwd declared functions
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
inline query_iterator<Components...> query<Components...>::begin() const {
    return query_iterator<Components...>(*this);
}

template<typename ... Components>
inline query_iterator<Components...> query<Components...>::end() const {
    return query_iterator<Components...>();
}


////////////////////////////////////////////////////////////////////////////////
//// Cached ptr fwd declared functions
////////////////////////////////////////////////////////////////////////////////

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_entity);
}


////////////////////////////////////////////////////////////////////////////////
//// Entity fwd declared functions
////////////////////////////////////////////////////////////////////////////////

inline flecs::type entity::type() const {
    return flecs::type(m_world, ecs_get_type(m_world, m_id));
}

inline flecs::type entity::to_type() const {
    ecs_type_t type = ecs_type_from_entity(m_world, m_id);
    return flecs::type(m_world, type);
}

inline child_iterator entity::children() const {
    return flecs::child_iterator(*this);
}

////////////////////////////////////////////////////////////////////////////////
//// Entity fluent fwd declared functions
////////////////////////////////////////////////////////////////////////////////

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add(const entity& entity) const {
    return add(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add(type type) const {
    return add(type.c_ptr());
}

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait(flecs::entity component) const {
    return add_trait(_::component_info<T>::id(), component.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait_tag(flecs::entity trait) const {
    return add_trait(trait.id(), _::component_info<C>::id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait(flecs::entity trait, flecs::entity entity) const {
    return add_trait(trait.id(), entity.id()); 
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove(const entity& entity) const {
    return remove(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove(type type) const {
    return remove(type.c_ptr());
}

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait(flecs::entity component) const {
    return remove_trait(_::component_info<T>::id(), component.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait_tag(flecs::entity trait) const {
    return remove_trait(trait.id(), _::component_info<C>::id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait(flecs::entity trait, flecs::entity entity) const {
    return remove_trait(trait.id(), entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_childof(const entity& entity) const {
    return add_childof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_childof(const entity& entity) const {
    return remove_childof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_instanceof(const entity& entity) const {
    return add_instanceof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_instanceof(const entity& entity) const {
    return remove_instanceof(entity.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::set_trait_tag(flecs::entity trait, const C& value) const
{
    static_cast<base_type*>(this)->invoke(
    [trait, &value](world_t *world, entity_t id) {
        ecs_set_ptr_w_entity(world, id, 
            ecs_trait(_::component_info<C>::id(world), trait.id()),
            sizeof(C), &value);
    });
    return *static_cast<base_type*>(this);
}  

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::set_trait(const T& value, flecs::entity tag) const
{
    static_cast<base_type*>(this)->invoke(
    [tag, &value](world_t *world, entity_t id) {
        ecs_set_ptr_w_entity(world, id, 
            ecs_trait(tag.id(), _::component_info<T>::id(world)),
            sizeof(T), &value);
    });
    return *static_cast<base_type*>(this);
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_owned(flecs::type type) const {
    return add_owned(type.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_switch(const entity& sw) const {
    return add_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_switch(const type& sw) const {
    return add_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_switch(const entity& sw) const {
    return remove_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_switch(const type& sw) const {
    return remove_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_case(const entity& sw_case) const {
    return add_case(sw_case.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_case(const entity& sw_case) const {
    return remove_case(sw_case.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::enable(const entity& e) const {
    return enable(e.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::disable(const entity& e) const {
    return disable(e.id());
}

inline bool entity::has_switch(flecs::type type) const {
    return ecs_has_entity(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity entity::get_case(flecs::type sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
}


////////////////////////////////////////////////////////////////////////////////
//// Iter fwd declared functions
////////////////////////////////////////////////////////////////////////////////

inline flecs::entity iter::system() const {
    return flecs::entity(m_iter->world, m_iter->system);
}

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < (size_t)m_iter->count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(m_iter->world, m_iter->entities[row]);
}

/* Obtain column source (0 if self) */
inline flecs::entity iter::column_source(int32_t col) const {
    return flecs::entity(m_iter->world, ecs_column_source(m_iter, col));
}

/* Obtain component/tag entity of column */
inline flecs::entity iter::column_entity(int32_t col) const {
    return flecs::entity(m_iter->world, ecs_column_entity(m_iter, col));
}

/* Obtain type of column */
inline type iter::column_type(int32_t col) const {
    return flecs::type(m_iter->world, ecs_column_type(m_iter, col));
}

/* Obtain type of table being iterated over */
inline type iter::table_type() const {
    return flecs::type(m_iter->world, ecs_iter_type(m_iter));
}


////////////////////////////////////////////////////////////////////////////////
//// World fwd declared functions
////////////////////////////////////////////////////////////////////////////////

inline void world::delete_entities(flecs::filter filter) const {
    ecs_bulk_delete(m_world, filter.c_ptr());
}

template <typename T>
inline void world::add(flecs::filter filter) const {
    ecs_bulk_add_remove_type(
        m_world, _::component_info<T>::type(m_world), nullptr, filter.c_ptr());
}

inline void world::add(flecs::type t) const {
    ecs_bulk_add_remove_type(m_world, t.c_ptr(), nullptr, nullptr);
}

inline void world::add(flecs::type t, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, t.c_ptr(), nullptr, filter.c_ptr());
}

inline void world::add(class flecs::entity e) const {
    ecs_bulk_add_remove_type(m_world, e.to_type().c_ptr(), nullptr, nullptr);
}

inline void world::add(class flecs::entity e, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, e.to_type().c_ptr(), nullptr, filter.c_ptr());
}

template <typename T>
inline void world::remove(flecs::filter filter) const {
    ecs_bulk_add_remove_type(
        m_world, nullptr, _::component_info<T>::type(m_world), filter.c_ptr());
}

inline void world::remove(flecs::type t) const {
    ecs_bulk_add_remove_type(m_world, nullptr, t.c_ptr(), nullptr);
}

inline void world::remove(flecs::type t, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, nullptr, t.c_ptr(), filter.c_ptr());
}

inline void world::remove(class entity e) const {
    ecs_bulk_add_remove_type(m_world, nullptr, e.to_type().c_ptr(), nullptr);
}

inline void world::remove(class entity e, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, nullptr, e.to_type().c_ptr(), filter.c_ptr());
}

inline flecs::world_filter world::filter(const flecs::filter& filter) const {
    return flecs::world_filter(*this, filter);
}

inline filter_iterator world::begin() const {
    return filter_iterator(*this, flecs::filter(*this), ecs_filter_next);
}

inline filter_iterator world::end() const {
    return filter_iterator(ecs_filter_next);
}

inline int world::count(flecs::filter filter) const {
    return ecs_count_w_filter(m_world, filter.c_ptr());
}

inline void world::init_builtin_components() {
    pod_component<Component>("flecs::core::Component");
    pod_component<Type>("flecs::core::Type");
    pod_component<Name>("flecs::core::Name");
}

template <typename T>
inline flecs::entity world::use(const char *alias) {
    entity_t id = _::component_info<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, id);
    }
    ecs_use(m_world, id, name);
    return flecs::entity(m_world, id);
}

inline flecs::entity world::use(const char *name, const char *alias) {
    entity_t id = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::");
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_use(m_world, id, alias);
    return flecs::entity(m_world, id);
}

inline void world::use(flecs::entity e, const char *alias) {
    entity_t id = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        ecs_get_name(m_world, id);
    }
    ecs_use(m_world, id, alias);
}

inline entity world::lookup(const char *name) const {
    auto id = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::");
    return flecs::entity(*this, id);
}

inline entity world::lookup(std::string& name) const {
    auto id = ecs_lookup_path_w_sep(m_world, 0, name.c_str(), "::", "::");
    return flecs::entity(*this, id);
}

template <typename T>
void world::set(T value) const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    e.set<T>(value);
}

template <typename T>
T* world::get_mut() const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    return e.get_mut<T>();
}

template <typename T>
void world::modified() const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    return e.modified<T>();
}

template <typename T>
void world::patch(std::function<void(T&)> func) const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    e.patch<T>(func);
} 

template <typename T>
const T* world::get() const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    return e.get<T>();
}

template <typename T>
bool world::has() const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    return e.has<T>();
}

template <typename T>
void world::remove() const {
    flecs::entity e(m_world, _::component_info<T>::id(m_world));
    e.remove<T>();
}

template <typename T>
flecs::entity world::singleton() {
    return flecs::entity(m_world, _::component_info<T>::id(m_world));
}

template <typename... Args>
inline flecs::entity world::entity(Args &&... args) const {
    return flecs::entity(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::entity world::prefab(Args &&... args) const {
    return flecs::prefab(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::type world::type(Args &&... args) const {
    return flecs::type(*this, std::forward<Args>(args)...);
}

template <typename... Comps, typename... Args>
inline flecs::system<Comps...> world::system(Args &&... args) const {
    return flecs::system<Comps...>(*this, std::forward<Args>(args)...);
}

template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query<Comps...>(*this, std::forward<Args>(args)...);
}

template <typename Module, typename... Args>
inline flecs::entity world::module(Args &&... args) const {
    return flecs::module<Module>(*this, std::forward<Args>(args)...);
}

template <typename Module>
inline flecs::entity world::import() {
    return flecs::import<Module>(*this);
}

template <typename T, typename... Args>
inline flecs::entity world::component(Args &&... args) const {
    return flecs::component<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline flecs::entity world::pod_component(Args &&... args) const {
    return flecs::pod_component<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline flecs::entity world::relocatable_component(Args &&... args) const {
    return flecs::relocatable_component<T>(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::snapshot world::snapshot(Args &&... args) const {
    return flecs::snapshot(*this, std::forward<Args>(args)...);
}

/** Utilities to convert type trait to flecs signature syntax */

namespace _
{

template <typename T,
    typename std::enable_if< std::is_const<T>::value == true, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "[in] ";
}

template <typename T,
    typename std::enable_if< std::is_reference<T>::value == true, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "[out] ";
}

template <typename T,
    typename std::enable_if<std::is_const<T>::value == false && std::is_reference<T>::value == false, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "";
}

template <typename T,
    typename std::enable_if< std::is_pointer<T>::value == true, void>::type* = nullptr>
constexpr const char *optional_modifier() {
    return "?";
}

template <typename T,
    typename std::enable_if< std::is_pointer<T>::value == false, void>::type* = nullptr>
constexpr const char *optional_modifier() {
    return "";
} 

/** Convert template arguments to string */
template <typename ...Components>
bool pack_args_to_string(world_t *world, std::stringstream& str, bool is_each) {
    (void)world;

    std::array<const char*, sizeof...(Components)> ids = {
        (_::component_info<Components>::name(world))...
    };

    std::array<const char*, sizeof...(Components)> inout_modifiers = {
        (inout_modifier<Components>())...
    }; 

    std::array<const char*, sizeof...(Components)> optional_modifiers = {
        (optional_modifier<Components>())...
    };        

    size_t i = 0;
    for (auto id : ids) {
        if (i) {
            str << ",";
        }
        
        str << inout_modifiers[i];
        str << optional_modifiers[i];

        if (is_each) {
            str << "ANY:";
        }
        str << id;
        i ++;
    }  

    return i != 0;
}

} // namespace _

} // namespace flecs

