#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

// Event builder interface
template <typename Base, typename E>
struct event_builder_base {
    event_builder_base(flecs::world_t *world, flecs::entity_t event)
        : m_world(world)
        , m_desc{}
        , m_ids{}
        , m_ids_array{}
    {
        m_desc.event = event;
    }

    /** Add component to emit for */
    template <typename T>
    Base& id() {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = _::cpp_type<T>().id(m_world);
        m_ids.count ++;
        return *this;
    }
    
    /** 
     * Add pair to emit for
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template <typename R, typename O>
    Base& id() {
        return id(
            ecs_pair(_::cpp_type<R>::id(this->m_world), 
                _::cpp_type<O>::id(this->m_world)));
    }

    /** 
     * Add pair to emit for
     * @tparam R the relation type.
     * @param object The object id.
     */
    template <typename R>
    Base& id(entity_t object) {
        return id(ecs_pair(_::cpp_type<R>::id(this->m_world), object));
    }

    /** 
     * Add pair to emit for
     * @param relation The relation type.
     * @param object The object id.
     */
    Base& id(entity_t relation, entity_t object) {
        return id(ecs_pair(relation, object));
    }

    /** Add (component) id to emit for */
    Base& id(flecs::id_t id) {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = id;
        m_ids.count ++;
        return *this;
    }

    /** Set entity for which to emit event */
    Base& entity(flecs::entity_t e) {
        ecs_record_t *r = ecs_record_find(m_world, e);
        
        /* Can't emit for empty entity */
        ecs_assert(r != nullptr, ECS_INVALID_PARAMETER, nullptr);
        ecs_assert(r->table != nullptr, ECS_INVALID_PARAMETER, nullptr);

        m_desc.table = r->table;
        m_desc.offset = ECS_RECORD_TO_ROW(r->row);
        m_desc.count = 1;
        return *this;
    }

    /* Set table for which to emit event */
    Base& table(flecs::table_t *t, int32_t offset = 0, int32_t count = 0) {
        m_desc.table = t;
        m_desc.offset = offset;
        m_desc.count = count;
        return *this;
    }

    /* Set event data */
    Base& ctx(const E* ptr) {
        m_desc.param = ptr;
        return *this;
    }

    void emit() {
        ecs_assert(m_ids.count != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_desc.table != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_ids.array = m_ids_array;
        m_desc.ids = &m_ids;
        m_desc.observable = const_cast<flecs::world_t*>(ecs_get_world(m_world));
        ecs_emit(m_world, &m_desc);
    }

protected:
    flecs::world_t *m_world;
    ecs_event_desc_t m_desc;
    flecs::type_t m_ids;
    flecs::id_t m_ids_array[ECS_EVENT_DESC_ID_COUNT_MAX];

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

struct event_builder : event_builder_base<event_builder, void> {
    using event_builder_base::event_builder_base;
};

template <typename E>
struct event_builder_typed : event_builder_base<event_builder_typed<E>, E> {
private:
    using Class = event_builder_typed<E>;

public:
    using event_builder_base<Class, E>::event_builder_base;

    /* Set event data */
    Class& ctx(const E& ptr) {
        this->m_desc.param = &ptr;
        return *this;
    }
};

}
