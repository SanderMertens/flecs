#pragma once

namespace flecs {

template <typename Base>
struct type_base {
    explicit type_base(
        world_t *world, const char *name = nullptr, const char *expr = nullptr)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.name = name;
        desc.ids_expr = expr;
        m_entity = flecs::entity(world, ecs_type_init(world, &desc));
        sync_from_flecs();
    }

    explicit type_base(
        world_t *world, entity_t e)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.entity = e;
        m_entity = flecs::entity(world, ecs_type_init(world, &desc));
        ecs_assert(!e || e == m_entity, ECS_INTERNAL_ERROR, nullptr);
        sync_from_flecs();
    }

    explicit type_base(world_t *world, table_t *t)
        : m_entity( world, static_cast<flecs::id_t>(0) )
        , m_table( t ) { }

    type_base(table_t *t)
        : m_table( t ) { }

    Base& add(id_t id) {
        if (!m_table) {
            for (auto type_id : this->vector()) {
                m_table = ecs_table_add_id(world(), m_table, type_id);
            }
        }

        m_table = ecs_table_add_id(world(), m_table, id);
        sync_from_me();
        return *this;
    }

    template <typename T>
    Base& add() {
        return this->add(_::cpp_type<T>::id(world()));
    }

    Base& add(entity_t relation, entity_t object) {
        return this->add(ecs_pair(relation, object));
    }

    template <typename Relation, typename Object>
    Base& add() {
        return this->add<Relation>(_::cpp_type<Object>::id(world()));
    }

    Base& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    Base& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }    

    template <typename Relation>
    Base& add(entity_t object) {
        return this->add(_::cpp_type<Relation>::id(world()), object);
    }     

    template <typename Object>
    Base& add_w_object(entity_t relation) {
        return this->add(relation, _::cpp_type<Object>::id(world()));
    }

    bool has(id_t id) {
        const flecs::world_t *w = ecs_get_world(world());
        return ecs_search(w, m_table, id, 0) != -1;
    }

    bool has(id_t relation, id_t object) {
        return this->has(ecs_pair(relation, object));
    }    

    template <typename T>
    bool has() {
        return this->has(_::cpp_type<T>::id(world()));
    }

    template <typename Relation, typename Object>
    bool has() {
        return this->has(_::cpp_type<flecs::pair<Relation, Object>>::id(world()));
    }

    flecs::string str() const {
        const flecs::world_t *w = ecs_get_world(world());
        char *str = ecs_type_str(w, ecs_table_get_type(m_table));
        return flecs::string(str);
    }

    type_t c_ptr() const {
        return ecs_table_get_type(m_table);
    }

    flecs::id_t id() const { 
        return m_entity.id(); 
    }

    flecs::entity entity() const {
        return m_entity;
    }

    flecs::world world() const { 
        return m_entity.world();
    } 

    void enable() const {
        ecs_enable(world(), id(), true);
    }

    void disable() const {
        ecs_enable(world(), id(), false);
    }

    flecs::vector<flecs::id_t> vector() {
        return flecs::vector<flecs::id_t>( const_cast<ecs_vector_t*>(
            ecs_table_get_type(m_table)));
    }

    flecs::id get(int32_t index) {
        const flecs::world_t *w = ecs_get_world(world());
        return flecs::id(const_cast<flecs::world_t*>(w), vector().get(index));
    }

    size_t count() {
        return vector().count();
    }

    /* Implicit conversion to type_t */
    operator type_t() const { return ecs_table_get_type(m_table); }

    operator Base&() { return *static_cast<Base*>(this); }

private:
    void sync_from_me() {
        if (!id()) {
            return;
        }

        EcsType *tc = ecs_get_mut(world(), id(), EcsType, NULL);
        ecs_assert(tc != NULL, ECS_INTERNAL_ERROR, NULL);
        tc->type = ecs_table_get_type(m_table);
        tc->normalized = m_table;
        ecs_modified(world(), id(), EcsType);
    }

    void sync_from_flecs() {
        if (!id()) {
            return;
        }

        const EcsType *tc = ecs_get(world(), id(), EcsType);
        if (!tc) {
            m_table = nullptr;
        } else {
            m_table = tc->normalized;
        }
    }

    flecs::entity m_entity;
    table_t *m_table = nullptr;
};

struct type : type_base<type> { 
    using type_base<type>::type_base;
};

}
