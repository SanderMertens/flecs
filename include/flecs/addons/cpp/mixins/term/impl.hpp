#pragma once

#include "builder_i.hpp"

namespace flecs {

// Class that describes a term
struct term final : term_builder_i<term> {
    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { value.move = true; }

    term(id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            value.move = true; 
            this->id(id);
        }

    term(flecs::world_t *world_ptr, id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(id);
        }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value = t;
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(r, o);
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
        return ecs_term_finalize(m_world, nullptr, &value);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    bool is_trivial() {
        return ecs_term_is_trivial(&value);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    flecs::entity get_subj() {
        return flecs::entity(world(), value.subj.entity);
    }

    flecs::entity get_pred() {
        return flecs::entity(world(), value.pred.entity);
    }

    flecs::entity get_obj() {
        return flecs::entity(world(), value.obj.entity);
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

template <typename T, typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...).id<T>();
}

template <typename R, typename O, typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...).id<R, O>();
}

}
