#pragma once

namespace flecs {

struct type {
    type() : m_world(nullptr), m_type(nullptr) { }

    type(world_t *world, const type_t *t)
        : m_world(world)
        , m_type(t) { }

    flecs::string str() const {
        return flecs::string(ecs_type_str(m_world, m_type));
    }

    int32_t count() const {
        if (!m_type) {
            return 0;
        }
        return m_type->count;
    }

    flecs::id_t* array() const {
        if (!m_type) {
            return nullptr;
        }
        return m_type->array;
    }

    flecs::id get(int32_t index) const {
        ecs_assert(m_type != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_type->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!m_type) {
            return flecs::id();
        }
        return flecs::id(m_world, m_type->array[index]);
    }

    /* Implicit conversion to type_t */
    operator const type_t*() const {
        return m_type;
    }
private:
    world_t *m_world;
    const type_t *m_type;
};

}
