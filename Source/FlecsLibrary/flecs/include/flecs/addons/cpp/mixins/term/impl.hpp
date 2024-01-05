/**
 * @file addons/cpp/mixins/term/impl.hpp
 * @brief Term implementation.
 */

#pragma once

#include "builder_i.hpp"

namespace flecs {

/** Class that describes a term.
 * 
 * \ingroup cpp_core_filters
 */
struct term final : term_builder_i<term> {
    term()
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { value.move = true; }

    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { value.move = true; }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value = t;
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, id_t id)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, entity_t r, entity_t o)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value.id = ecs_pair(r, o);
            value.move = false;
            this->set_term(&value);
        }

    term(id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
            value.move = true; 
        }

    term(id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            value.id = ecs_pair(r, o);
            value.move = true; 
        }

    term(const term& t) : term_builder_i<term>(&value) {
        m_world = t.m_world;
        value = ecs_term_copy(&t.value);
        this->set_term(&value);
    }

    term(term&& t) : term_builder_i<term>(&value) {
        m_world = t.m_world;
        value = ecs_term_move(&t.value);
        t.reset();
        this->set_term(&value);
    }

    term& operator=(const term& t) {
        ecs_assert(m_world == t.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = ecs_term_copy(&t.value);
        this->set_term(&value);
        return *this;
    }

    term& operator=(term&& t) {
        ecs_assert(m_world == t.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = t.value;
        this->set_term(&value);
        t.reset();
        return *this;
    }   

    ~term() {
        ecs_term_fini(&value);
    }

    void reset() {
        value = {};
        this->set_term(nullptr);
    }

    int finalize() {
        return ecs_term_finalize(m_world, &value);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    flecs::id id() {
        return flecs::id(m_world, value.id);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    flecs::entity get_src() {
        return flecs::entity(m_world, value.src.id);
    }

    flecs::entity get_first() {
        return flecs::entity(m_world, value.first.id);
    }

    flecs::entity get_second() {
        return flecs::entity(m_world, value.second.id);
    }

    ecs_term_t move() { /* explicit move to ecs_term_t */
        return ecs_term_move(&value);
    }

    ecs_term_t value;

protected:
    flecs::world_t* world_v() override { return m_world; }

private:
    flecs::world_t *m_world;
};

// Term mixin implementation
template <typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...);
}

template <typename T>
inline flecs::term world::term() const {
    return flecs::term(m_world, _::cpp_type<T>::id(m_world));
}

template <typename First, typename Second>
inline flecs::term world::term() const {
    return flecs::term(m_world, ecs_pair(
        _::cpp_type<First>::id(m_world),
        _::cpp_type<Second>::id(m_world)));
}

}
