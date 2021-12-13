#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

// Event builder interface
struct event_builder {
    event_builder(flecs::world_t *world, flecs::entity_t event) 
        : m_world(world)
        , m_desc{}
        , m_ids{}
        , m_ids_array{}
    {
        m_desc.event = event;
    }

    /** Add component to trigger on */
    template <typename T>
    event_builder& id() {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = _::cpp_type<T>().id(m_world);
        m_ids.count ++;
        return *this;
    }

    /** Add (component) id to trigger on */
    event_builder& id(flecs::id_t id) {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = id;
        m_ids.count ++;
        return *this;
    }

    /** Set entity for which to trigger */
    event_builder& entity(flecs::entity_t e) {
        ecs_record_t *r = ecs_record_find(m_world, e);
        
        /* can't trigger for empty entity */
        ecs_assert(r != nullptr, ECS_INVALID_PARAMETER, nullptr);
        ecs_assert(r->table != nullptr, ECS_INVALID_PARAMETER, nullptr);

        m_desc.table = r->table;
        m_desc.offset = ECS_RECORD_TO_ROW(r->row);
        m_desc.count = 1;
        return *this;
    }

    /* Set table for which to trigger */
    event_builder& table(flecs::table_t *t, int32_t offset = 0, int32_t count = 0) {
        m_desc.table = t;
        m_desc.offset = offset;
        m_desc.count = count;
        return *this;
    }

    void emit() {
        ecs_assert(m_ids.count != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_desc.table != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_ids.array = m_ids_array;
        m_desc.ids = &m_ids;
        m_desc.observable = m_world;
        ecs_emit(m_world, &m_desc);
    }

private:
    flecs::world_t *m_world;
    ecs_event_desc_t m_desc;
    flecs::ids_t m_ids;
    flecs::id_t m_ids_array[ECS_EVENT_DESC_ID_COUNT_MAX];
};

}
