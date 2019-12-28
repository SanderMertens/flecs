#pragma once

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <iostream>
#include <type_traits>
#include <memory>
#include <cstdint>

namespace flecs {

/* -- Forward declarations and types -- */

using world_t = ecs_world_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using snapshot_t = ecs_snapshot_t;
using filter_t = ecs_filter_t;

class entity;
class type;
class rows;

template <typename T>
class component_base;

/* -- Constants -- */

using Component = EcsComponent;
using TypeComponent = EcsTypeComponent;
using Prefab = EcsPrefab;
static const entity_t PrefabParent = EEcsPrefabParent;
static const entity_t PrefabBuilder = EEcsPrefabBuilder;
static const entity_t RowSystem = EEcsRowSystem;
static const entity_t ColSystem = EEcsColSystem;
using Id = EcsId;
static const entity_t Hidden = EEcsHidden;
static const entity_t Disabled = EEcsDisabled;
static const entity_t OnDemand = EEcsOnDemand;

static const entity_t World = EcsWorld;
static const entity_t Singleton = EcsSingleton;
static const entity_t Invalid = EcsInvalid;

/* -- Types -- */

enum system_kind {
    OnLoad = EcsOnLoad,
    PostLoad = EcsPostLoad,
    PreUpdate = EcsPreUpdate,
    OnUpdate = EcsOnUpdate,
    OnValidate = EcsOnValidate,
    PostUpdate = EcsPostUpdate,
    PreStore = EcsPreStore,
    OnStore = EcsOnStore,
    Manual = EcsManual,
    OnAdd = EcsOnAdd,
    OnRemove = EcsOnRemove,
    OnSet = EcsOnSet
};

/** Utility to protect against out of bound reads on component arrays */
template <typename T>
class column final {
public:
    column(T* array, size_t count, bool is_shared = false)
        : m_array(array)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    column(flecs::rows &rows, int column);

    T& operator[](size_t index) {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array[index];
    }

    T* operator->() {
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array;
    }

    bool is_set() {
        return m_array != nullptr;
    }

    bool is_shared() {
        return m_is_shared;
    }
private:
    T* m_array;
    size_t m_count;
    bool m_is_shared;
};

/** Provides an integer range to iterate over */
template <typename T>
class range_iterator
{
public:
    range_iterator(T value)
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

/** Wrapper around ecs_rows_t */
class rows final {
    using row_iterator = range_iterator<int>;
public:    
    rows(const ecs_rows_t *rows) : m_rows(rows) { 
        m_begin = 0;
        m_end = rows->count;
    }

    row_iterator begin() const {
        return row_iterator(m_begin);
    }

    row_iterator end() const {
        return row_iterator(m_end);
    }

    /* Obtain handle to current system */
    flecs::entity system() const;

    /* Number of entities to iterate over */
    uint32_t count() const {
        return m_rows->count;
    }

    /* Is column shared */
    bool is_shared(uint32_t column) const {
        return ecs_is_shared(m_rows, column);
    }

    /* Is column readonly */
    bool is_readonly(uint32_t column) const {
        return ecs_is_readonly(m_rows, column);
    }

    /* Obtain entity being iterated over for row */
    flecs::entity entity(uint32_t row) const;

    /* Obtain column source (0 if self) */
    flecs::entity column_source(uint32_t column) const;

    /* Obtain component/tag entity of column */
    flecs::entity column_entity(uint32_t column) const;

    /* Obtain type of column */
    flecs::type column_type(uint32_t column) const;

    /* Obtain type of table being iterated over */
    flecs::type table_type() const;

    /* Obtain untyped pointer to table column */
    void* table_column(uint32_t table_column) {
        return ecs_table_column(m_rows, table_column);
    }   

    /* Obtain column with a const type */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    flecs::column<T> column(unsigned int column) const {
        return get_column<T>(column);
    }

    /* Obtain column with non-const type. Ensure that column is not readonly */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    flecs::column<T> column(uint32_t column) const {
        ecs_assert(!ecs_is_readonly(m_rows, column), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_column<T>(column);
    }

    /* Get owned */
    template <typename T>
    flecs::column<T> owned(uint32_t column) const {
        ecs_assert(!ecs_is_shared(m_rows, column), ECS_COLUMN_IS_SHARED, NULL);
        return this->column<T>(column);
    }

    /* Get shared */
    template <typename T>
    const T& shared(uint32_t column) const {
        ecs_assert(ecs_column_entity(m_rows, column) == component_base<T>::s_entity, ECS_COLUMN_TYPE_MISMATCH, NULL);
        ecs_assert(ecs_is_shared(m_rows, column), ECS_COLUMN_IS_NOT_SHARED, NULL);
        return *static_cast<T*>(_ecs_column(m_rows, sizeof(T), column));
    }

    /* Get single field of a const type */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>    
    T& field(uint32_t column, uint32_t row) const {
        return get_field<T>(column, row);
    }

    /* Get single field of a non-const type. Ensure that column is not readonly */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    T& field(uint32_t column, uint32_t row) const {
        ecs_assert(!ecs_is_readonly(m_rows, column), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_field<T>(column, row);
    }

private:
    /* Get column, check if correct type is used */
    template <typename T>
    flecs::column<T> get_column(uint32_t column_id) const {
        ecs_assert(ecs_column_entity(m_rows, column_id) == component_base<T>::s_entity, ECS_COLUMN_TYPE_MISMATCH, NULL);
        uint32_t count;
        bool is_shared = ecs_is_shared(m_rows, column_id);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = m_rows->count;
        }

        return flecs::column<T>(static_cast<T*>(_ecs_column(m_rows, sizeof(T), column_id)), count, is_shared);
    }   

    /* Get single field, check if correct type is used */
    template <typename T>
    T& get_field(uint32_t column, uint32_t row) const {
        ecs_assert(ecs_column_entity(m_rows, column) == component_base<T>::s_entity, ECS_COLUMN_TYPE_MISMATCH, NULL);
        return *static_cast<T*>(_ecs_field(m_rows, sizeof(T), column, row));
    }       

    const ecs_rows_t *m_rows;
    uint32_t m_begin;
    uint32_t m_end;
};

template <typename T>
inline column<T>::column(flecs::rows &rows, int column) {
    *this = rows.column<T>(column);
}

/** Wrapper around world_t */
class world final {
public:
    world() 
        : m_world( ecs_init() )
        , m_owner( true ) { init_builtin_components(); }

    world(int argc, char *argv[])
        : m_world( ecs_init_w_args(argc, argv) )
        , m_owner( true ) { init_builtin_components(); }

    world(world_t *world) 
        : m_world( world )
        , m_owner( false ) { init_builtin_components(); }

    world(const world& obj) {
        m_world = obj.m_world;
        m_owner = false;
    }

    world(world&& obj) {
        m_world = obj.m_world;
        m_owner = true;
        obj.m_owner = false;
    }

    world& operator=(const world& obj) {
        m_world = obj.m_world;
        m_owner = false;
        return *this;
    }

    world& operator=(world&& obj) {
        m_world = obj.m_world;
        m_owner = true;
        obj.m_owner = false;
        return *this;
    }
    
    ~world() { 
        if (m_owner) {
            ecs_fini(m_world); 
        }
    }

    world_t* c() const {
        return m_world;
    }

    bool progress(float delta_time = 0.0) const {
        return ecs_progress(m_world, delta_time);
    }

    void set_threads(std::uint32_t threads) const {
        ecs_set_threads(m_world, threads);
    }

    std::uint32_t get_threads() const {
        return ecs_get_threads(m_world);
    }

    std::uint32_t get_thread_index() const {
        return ecs_get_thread_index(m_world);
    }

    void set_target_fps(std::uint32_t target_fps) const {
        ecs_set_target_fps(m_world, target_fps);
    }

    std::uint32_t get_target_fps() const {
        return ecs_get_target_fps(m_world);
    }

    void set_context(void* ctx) const {
        ecs_set_context(m_world, ctx);
    }

    void* get_context() const {
        return ecs_get_context(m_world);
    }

    std::uint32_t get_tick() const {
        return ecs_get_tick(m_world);
    }

    void dim(std::int32_t entity_count) const {
        ecs_dim(m_world, entity_count);
    }

    void dim_type(flecs::type_t type, std::int32_t entity_count) const {
        _ecs_dim_type(m_world, type, entity_count);
    }

    void set_entity_range(entity_t min, entity_t max) const {
        ecs_set_entity_range(m_world, min, max);
    }

    void enable_range_check(bool enabled) const {
        ecs_enable_range_check(m_world, enabled);
    }

    entity lookup(const char *name) const;

private:
    void init_builtin_components();

    world_t *m_world;
    bool m_owner;
};

/** Fluent interface for entity class */
template <typename base>
class entity_fluent {
    using base_type = const base;
public:

    /* -- adopt -- */

    base_type& add(entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity](world_t *world, entity_t id) {
            ecs_add_entity(world, id, entity);
        });
        return *static_cast<base_type*>(this);         
    }

    template <typename T>
    base_type& add() const {
        return add(component_base<T>::s_entity);
    }

    base_type& add(const entity& entity) const;

    base_type& add(flecs::type_t type) const {
        static_cast<base_type*>(this)->invoke(
        [type](world_t *world, entity_t id) {
            _ecs_add(world, id, type);
        });
        return *static_cast<base_type*>(this); 
    }

    base_type& add(flecs::type type) const;

    /* -- remove -- */

    base_type& remove(entity_t entity) const {
        static_cast<base_type*>(this)->invoke(
        [entity](world_t *world, entity_t id) {
            ecs_remove_entity(world, id, entity);
        });
        return *static_cast<base_type*>(this);         
    }    

    template <typename T>
    base_type& remove() {
        return remove(component_base<T>::s_entity);
    }

    base_type& remove(const entity& entity) const;

    base_type& remove(flecs::type_t type) const {
        static_cast<base_type*>(this)->invoke(
        [type](world_t *world, entity_t id) {
            _ecs_remove(world, id, type);
        });
        return *static_cast<base_type*>(this);         
    }

    base_type& remove(flecs::type type) const;

    /* -- adopt -- */

    base_type& add_childof(entity_t parent) const {
        static_cast<base_type*>(this)->invoke(
        [parent](world_t *world, entity_t id) {
            ecs_adopt(world, id, parent);
        });
        return *static_cast<base_type*>(this);  
    }

    base_type& add_childof(const entity& parent) const;

    /* -- orphan -- */

    base_type& remove_childof(entity_t parent) const {
        static_cast<base_type*>(this)->invoke(
        [parent](world_t *world, entity_t id) {
            ecs_orphan(world, id, parent);
        });
        return *static_cast<base_type*>(this);  
    }

    base_type& remove_childof(const entity& parent) const;

    /* -- inherit -- */

    base_type& add_instanceof(entity_t base_entity) const {
        static_cast<base_type*>(this)->invoke(
        [base_entity](world_t *world, entity_t id) {
            ecs_inherit(world, id, base_entity);
        });
        return *static_cast<base_type*>(this);  
    }

    base_type& add_instanceof(const entity& base_entity) const;  

    /* -- disinherit -- */

    base_type& remove_instanceof(entity_t base_entity) const {
        static_cast<base_type*>(this)->invoke(
        [base_entity](world_t *world, entity_t id) {
            ecs_disinherit(world, id, base_entity);
        });
        return *static_cast<base_type*>(this);
    }

    base_type& remove_instanceof(const entity& base_entity) const;

    /* -- set -- */

    template <typename T>
    const base_type& set(const T&& value) const {
        static_cast<base_type*>(this)->invoke(
        [value](world_t *world, entity_t id) {
            _ecs_set_ptr(world, id, component_base<T>::s_entity, sizeof(T), &value);
        });
        return *static_cast<base_type*>(this);
    }   

    template <typename T>
    const base_type& set(const T& value) const {
        static_cast<base_type*>(this)->invoke(
        [value](world_t *world, entity_t id) {
            _ecs_set_ptr(world, id, component_base<T>::s_entity, sizeof(T), &value);
        });
        return *static_cast<base_type*>(this);
    }        
};

/** Entity class */
class entity : public entity_fluent<entity> {
public:
    entity(const world& world) 
        : m_world( world.c() )
        , m_id( _ecs_new(m_world, 0) ) { }

    entity(const world& world, entity_t id) 
        : m_world( world.c() )
        , m_id(id) { }

    entity(const world& world, const char *name) 
        : m_world( world.c() )
        , m_id( ecs_new_entity(m_world, name, 0) ) { }

    entity() 
        : m_world(nullptr)
        , m_id(0) { }

    entity_t id() const {
        return m_id;
    }

    std::string name() const {
        EcsId *name = (EcsId*)_ecs_get_ptr(m_world, m_id, TEcsId);
        if (name) {
            return std::string(*name);
        } else {
            return std::string();
        }
    }

    flecs::type type() const;

    template<typename T>
    T get() const {
        return *(T*)_ecs_get_ptr(m_world, m_id, component_base<T>::s_type);
    }

    template<typename T>
    T* get_ptr() const {
        return static_cast<T*>(
            _ecs_get_ptr(m_world, m_id, component_base<T>::s_type));
    }

    template <typename Func>
    void invoke(Func action) const {
        action(m_world, m_id);
    } 

    void destruct() const {
        ecs_delete(m_world, m_id);
    }

    entity lookup(const char *name) const {
        auto id = ecs_lookup_child(m_world, m_id, name);
        return entity(m_world, id);
    }

    /* -- has -- */

    bool has(entity_t id) const {
        return ecs_has_entity(m_world, m_id, id);
    }

    bool has(flecs::type_t type) const {
        return _ecs_has(m_world, m_id, type);
    }

    bool has(const entity& entity) const {
        return has(entity.id());
    }

    template <typename T>
    bool has() const {
        return has(component_base<T>::s_entity);
    }

    bool has_owned(entity_t id) const {
        return ecs_has_entity_owned(m_world, m_id, id);
    }

    bool has_owned(flecs::type_t type) const {
        return _ecs_has_owned(m_world, m_id, type);
    }

    bool has_owned(const entity& entity) const {
        return has_owned(entity.id());
    }

    template <typename T>
    bool has_owned() const {
        return has_owned(component_base<T>::s_entity);
    }    

protected:
    world_t *m_world;
    entity_t m_id; 
};

/** Prefab class */
class prefab final : public entity {
public:
    prefab(const world& world, const char *name) 
        : entity(world, name)
    {
        this->add<flecs::Prefab>();
    }

    prefab(const world& world, const char *name, entity parent)
        : entity(world, name)
    {
        this->set<flecs::Prefab>({parent.id()});
    }
};

/** Entity range class */
class entity_range final : entity_fluent<entity_range> {
    using entity_iterator = range_iterator<entity_t>;
public:
    entity_range(const world& world, std::int32_t count) 
        : m_world(world.c())
        , m_id_start( _ecs_new_w_count(m_world, nullptr, count))
        , m_count(count) { }

    template <typename Func>
    void invoke(Func action) {
        for (auto id : *this) {
            action(m_world, id);
        }
    }
    
    entity_iterator begin() {
        return entity_iterator(m_id_start);
    }

    entity_iterator end() {
        return entity_iterator(m_id_start + m_count);
    }

private:
    world_t *m_world;
    entity_t m_id_start;
    std::int32_t m_count;
};

/** Type */

class type final : entity {
public:
    type(const world& world, const char *name, const char *expr = nullptr)
        : entity(world, ecs_new_type(world.c(), name, expr))
    { 
        sync_from_flecs();
    }

    type(const world& world, const char *name, entity parent, const char *expr)
        : entity(world, ecs_new_type(world.c(), name, expr))
    { 
        this->set<EcsPrefab>({parent.id()});
        sync_from_flecs();
    }

    type(const world& world, type_t type)
        : entity(world)
        , m_type( type )
        , m_normalized( type ) { }

    type& add(const type& type) {
        m_type = ecs_type_add(m_world, m_type, type.id());
        m_normalized = ecs_type_merge(m_world, m_normalized, type.c(), nullptr);
        sync_from_me();
        return *this;
    }

    type& add(const entity& entity) {
        m_type = ecs_type_add(m_world, m_type, entity.id());
        m_normalized = ecs_type_add(m_world, m_normalized, entity.id());
        sync_from_me();
        return *this;
    }

    template<typename T>
    type& add() {
        m_type = ecs_type_add(m_world, m_type, component_base<T>::s_entity);
        m_normalized = ecs_type_add(m_world, m_normalized, component_base<T>::s_entity);
        sync_from_me();
        return *this;
    }    

    type& add_instanceof(const entity& entity) {
        m_type = ecs_type_add(m_world, m_type, entity.id() | ECS_INSTANCEOF);
        m_normalized = ecs_type_add(m_world, m_normalized, entity.id() | ECS_INSTANCEOF);
        sync_from_me();
        return *this;
    }

    type& add_childof(const entity& entity) {
        m_type = ecs_type_add(m_world, m_type, entity.id() | ECS_CHILDOF);
        m_normalized = ecs_type_add(m_world, m_normalized, entity.id() | ECS_CHILDOF);
        sync_from_me();
        return *this;
    }

    std::string str() const {
        char *str = ecs_type_to_expr(m_world, m_type);
        std::string result(str);
        free(str);
        return result;
    }

    type_t c() const {
        return m_type;
    }

    type_t c_normalized() const {
        return m_normalized;
    }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

private:
    void sync_from_me() {
        EcsTypeComponent *tc = ecs_get_ptr(m_world, m_id, EcsTypeComponent);
        if (tc) {
            tc->type = m_type;
            tc->resolved = m_normalized;
        }
    }

    void sync_from_flecs() {
        EcsTypeComponent *tc = ecs_get_ptr(m_world, m_id, EcsTypeComponent);
        if (tc) {
            m_type = tc->type;
            m_normalized = tc->resolved;
        }
    }

    type_t m_type;
    type_t m_normalized;
};

/* -- entity implementation -- */

inline flecs::type entity::type() const {
    return flecs::type(world(m_world), ecs_get_type(m_world, m_id));
}

/* -- entity_fluent implementation -- */

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::add(const entity& entity) const {
    return add(entity.id());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::add(flecs::type type) const {
    return add(type.c());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::remove(const entity& entity) const {
    return remove(entity.id());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::remove(flecs::type type) const {
    return remove(type.c());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::add_childof(const entity& entity) const {
    return add_childof(entity.id());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::remove_childof(const entity& entity) const {
    return remove_childof(entity.id());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::add_instanceof(const entity& entity) const {
    return add_instanceof(entity.id());
}

template <typename base>
inline typename entity_fluent<base>::base_type& entity_fluent<base>::remove_instanceof(const entity& entity) const {
    return remove_instanceof(entity.id());
}

inline entity world::lookup(const char *name) const {
    auto id = ecs_lookup(m_world, name);
    return entity(*this, id);
}

/* -- rows implementation -- */

inline flecs::entity rows::system() const {
    return flecs::entity(m_rows->world, m_rows->system);
}

inline flecs::entity rows::entity(uint32_t row) const {
    ecs_assert(row < m_rows->count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(m_rows->world, m_rows->entities[row]);
}

/* Obtain column source (0 if self) */
inline flecs::entity rows::column_source(uint32_t column) const {
    return flecs::entity(m_rows->world, ecs_column_source(m_rows, column));
}

/* Obtain component/tag entity of column */
inline flecs::entity rows::column_entity(uint32_t column) const {
    return flecs::entity(m_rows->world, ecs_column_entity(m_rows, column));
}

/* Obtain type of column */
inline flecs::type rows::column_type(uint32_t column) const {
    return flecs::type(m_rows->world, ecs_column_type(m_rows, column));
}

/* Obtain type of table being iterated over */
inline flecs::type rows::table_type() const {
    return flecs::type(m_rows->world, ecs_table_type(m_rows));
}

/** Register component, provide global access to component handles / metadata */
template <typename T>
class component_base final {
public:
    static void init(const world& world, const char *name) {
        s_entity = ecs_new_component(world.c(), name, sizeof(T));
        s_type = ecs_type_from_entity(world.c(), s_entity);
        s_name = name;
    }

    static void init_existing(entity_t entity, flecs::type_t type, const char *name) {
        s_entity = entity;
        s_type = type;
        s_name = name;
    }

    static entity_t s_entity;
    static flecs::type_t s_type;
    static const char *s_name;
};

template <typename T> entity_t component_base<T>::s_entity( 0 );
template <typename T> flecs::type_t component_base<T>::s_type( nullptr );
template <typename T> const char* component_base<T>::s_name( nullptr );

template <typename T>
class component : public entity {
public:
    component(world world, const char *name) { 
        component_base<T>::init(world, name);

        /* Register as well for both const and reference versions of type */
        component_base<const T>::init_existing(
            component_base<T>::s_entity, 
            component_base<T>::s_type, 
            component_base<T>::s_name);

        component_base<T&>::init_existing(
            component_base<T>::s_entity, 
            component_base<T>::s_type, 
            component_base<T>::s_name);    

        entity(world, component_base<T>::s_entity);
    }
};

/* Register builtin components with C++ API */
inline void world::init_builtin_components() {
    flecs::component<flecs::Component>(*this, "EcsComponent");
    flecs::component<flecs::TypeComponent>(*this, "EcsTypeComponent");
    flecs::component<flecs::Prefab>(*this, "EcsPrefab");
    flecs::component<flecs::Id>(*this, "EcsId");
}

/** Class that represens a type filter */
class filter {
public:
    filter(world world) 
        : m_world( world.c() )
        , m_filter{} { }

    filter& include(flecs::type type) {
        m_filter.include = ecs_type_merge(m_world, m_filter.include, type.c(), nullptr);
        return *this;
    }

    filter& include(flecs::entity entity) {
        m_filter.include = ecs_type_add(m_world, m_filter.include, entity.id());
        return *this;
    }

    template <typename T>
    filter& include() {
        m_filter.include = ecs_type_add(m_world, m_filter.include, component_base<T>::s_entity);
        return *this;
    }

    flecs::type include() {
        return flecs::type(m_world, m_filter.include);
    }

    filter& exclude(flecs::type type) {
        m_filter.exclude = ecs_type_merge(m_world, m_filter.exclude, type.c(), nullptr);
        return *this;
    }

    filter exclude(flecs::entity entity) {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, entity.id());
        return *this;
    }

    template <typename T>
    filter& exclude() {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, component_base<T>::s_entity);
        return *this;
    }
 
    flecs::type exclude() {
        return flecs::type(m_world, m_filter.exclude);
    }

    filter_t* c() {
        return &m_filter;
    }
private:
    world_t *m_world;
    filter_t m_filter;
};

/* Class that encapsulates a snapshot */
class snapshot final {
public:
    snapshot(flecs::world world)
        : m_world( world.c() )
        , m_snapshot( nullptr )
        { }

    void take() {
        if (m_snapshot) {
            ecs_snapshot_free(m_world, m_snapshot);
        }

        m_snapshot = ecs_snapshot_take(m_world, nullptr);
    }

    void take(flecs::filter filter) {
        if (m_snapshot) {
            ecs_snapshot_free(m_world, m_snapshot);
        }

        m_snapshot = ecs_snapshot_take(m_world, filter.c());
    }

    void restore() {
        if (m_snapshot) {
            ecs_snapshot_restore(m_world, m_snapshot);
            m_snapshot = nullptr;
        }
    }

    ~snapshot() {
        if (m_snapshot) {
            ecs_snapshot_free(m_world, m_snapshot);
        }
    }
private:
    world_t *m_world;
    snapshot_t *m_snapshot;
};

/** Class that wraps around compile-time type safe system callbacks */
template <typename Func, typename ... Components>
class system_ctx {
    using columns = std::array<void*, sizeof...(Components)>;

public:
    system_ctx(Func func) : m_func(func) { }

    /* Dummy function when last component has been added */
    static void populate_columns(ecs_rows_t *rows, int index, columns& columns) { }

    /* Populate columns array recursively */
    template <typename T, typename... Targs>
    static void populate_columns(ecs_rows_t *rows, int index, columns& columns, T comp, Targs... comps) {
        columns[index] = _ecs_column(rows, sizeof(*comp), index + 1);
        populate_columns(rows, index + 1, columns, comps ...);
    }

    /* Invoke system */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_rows_t *rows, int index, columns& columns, Targs... comps) {
        system_ctx *self = (system_ctx*)
            ecs_get_system_context(rows->world, rows->system);

        Func func = self->m_func;

        flecs::rows rows_wrapper(rows);
        
        func(rows_wrapper, (flecs::column<typename std::remove_reference<Components>::type>(
            (typename std::remove_reference<Components>::type*)comps, rows->count))...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_rows_t *rows, int index, columns& columns, Targs... comps) {
        call_system(rows, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_rows_t *rows) {
        columns columns;
        populate_columns(rows, 0, columns, (typename std::remove_reference<Components>::type*)nullptr...);
        call_system(rows, 0, columns);
    }   

private:
    Func m_func;
};

class system_runner {
public:
    system_runner(world_t *world, entity_t id, float delta_time, void *param)
        : m_world(world)
        , m_id(id)
        , m_delta_time(delta_time)
        , m_param(param)
        , m_filter(nullptr)
        , m_offset(0)
        , m_limit(0) { }

    system_runner& filter(flecs::filter filter) {
        ecs_assert(filter.exclude().c() == nullptr, ECS_UNSUPPORTED, NULL);
        m_filter = filter.include().c();
        return *this;
    }

    system_runner& offset(std::int32_t offset) {
        m_offset = offset;
        return *this;
    }

    system_runner& limit(std::int32_t limit) {
        m_limit = limit;
        return *this;
    }

    ~system_runner() {
        _ecs_run_w_filter(
            m_world, m_id, m_delta_time, m_offset, m_limit, m_filter, m_param);
    }
private:
    world_t *m_world;
    entity_t m_id;
    float m_delta_time;
    void *m_param;
    ecs_type_t m_filter;
    std::int32_t m_offset;
    std::int32_t m_limit;
};

/** Helper class that constructs a new system */
template<typename ... Components>
class system final : public entity {
public:
    system(world world, const char *name = nullptr)
        : m_kind(static_cast<EcsSystemKind>(OnUpdate))
        , m_name(name) 
        , m_on_demand(false)
        , m_hidden(false) { 
            m_world = world.c();
        }

    system& signature(const char *signature) {
        m_signature = signature;
        return *this;
    }

    system& kind(system_kind kind) {
        m_kind = static_cast<EcsSystemKind>(kind);
        return *this;
    }

    system& on_demand() {
        m_on_demand = true;
        return *this;
    }

    system& hidden() {
        m_hidden = true;
        return *this;
    }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

    bool is_enabled() const {
        return ecs_is_enabled(m_world, m_id);
    }

    void set_period(float period) const {
        ecs_set_period(m_world, m_id, period);
    }

    void set_context(void *ctx) const {
        ecs_set_system_context(m_world, m_id, ctx);
    }

    void* get_context() const {
        return ecs_get_system_context(m_world, m_id);
    }

    system_runner run(float delta_time = 0.0f, void *param = nullptr) const {
        return system_runner(m_world, m_id, delta_time, param);
    }

    /* Action is mandatory and always the last thing that is added in the fluent
     * method chain. Create system signature from both template parameters and
     * anything provided by the signature method. */
    template <typename Func>
    system& action(Func func) {
        auto ctx = new system_ctx<Func, Components...>(func);

        std::stringstream str;
        std::array<const char*, sizeof...(Components)> ids = {
            component_base<Components>::s_name...
        };

        std::array<const char*, sizeof...(Components)> inout_modifiers = {
            inout_modifier<Components>()...
        };    

        int i = 0;
        for (auto id : ids) {
            if (i) {
                str << ",";
            }
            str << inout_modifiers[i];
            str << id;
            i ++;
        }           

        if (m_signature) {
            if (i) {
                str << ",";
            }

            str << m_signature;
            i++;
        }

        if (m_hidden) {
            if (i) {
                str << ",";
            }            
            str << "SYSTEM.EcsHidden";
            i ++;
        }    

        if (m_on_demand) {
            if (i) {
                str << ",";
            }            
            str << "SYSTEM.EcsOnDemand";
            i ++;
        }         

        std::string signature = str.str();

        entity_t e = ecs_new_system(
            m_world, 
            m_name, 
            m_kind, 
            signature.c_str(), 
            system_ctx<Func, Components...>::run);

        ecs_set_system_context(m_world, e, ctx);

        m_id = e;

        return *this;
    }

    ~system() = default;
private:

    /** Utilities to convert type trait to flecs signature syntax */
    template <typename T,
        typename std::enable_if< std::is_const<T>::value == true, void>::type* = nullptr>
    constexpr const char *inout_modifier() const {
        return "[in] ";
    }

    template <typename T,
        typename std::enable_if< std::is_reference<T>::value == true, void>::type* = nullptr>
    constexpr const char *inout_modifier() const {
        return "[out] ";
    }

    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false && std::is_reference<T>::value == false, void>::type* = nullptr>
    constexpr const char *inout_modifier() const {
        return "";
    }

    EcsSystemKind m_kind;
    const char *m_name;
    const char *m_signature = nullptr;
    bool m_on_demand;
    bool m_hidden;
};

/* Class that imports a module */
template <typename T>
class import {
public:
    import(flecs::world world, int flags) {
        if (!component_base<T>::s_name) {
            T module_data = T(world, flags);

            flecs::entity s(world, EcsSingleton);

            s.set<T>(module_data);
        }
    }
};

/* Class that defines a module */
template <typename T>
class module final : public component<T> {
public:
    module(flecs::world world, const char *name) : component<T>(world, name) { }
};

}
