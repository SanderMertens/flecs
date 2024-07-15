/**
 * @file addons/cpp/mixins/term/impl.hpp
 * @brief Term implementation.
 */

#pragma once

#include "builder_i.hpp"

namespace flecs {

/** Class that describes a term.
 * 
 * @ingroup cpp_core_queries
 */
struct term final : term_builder_i<term> {
    term()
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) { }

    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) { }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            value = t;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, id_t id)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, entity_t r, entity_t o)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            value.id = ecs_pair(r, o);
            this->set_term(&value);
        }

    term(id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) { 
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
        }

    term(id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) { 
            value.id = ecs_pair(r, o);
        }

    void reset() {
        value = {};
        this->set_term(nullptr);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    flecs::id id() {
        return flecs::id(world_, value.id);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    flecs::entity get_src() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.src));
    }

    flecs::entity get_first() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.first));
    }

    flecs::entity get_second() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.second));
    }

    operator flecs::term_t() const {
        return value;
    }

    flecs::term_t value;

protected:
    flecs::world_t* world_v() override { return world_; }

private:
    flecs::world_t *world_;
};

// Term mixin implementation
template <typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(world_, FLECS_FWD(args)...);
}

template <typename T>
inline flecs::term world::term() const {
    return flecs::term(world_, _::type<T>::id(world_));
}

template <typename First, typename Second>
inline flecs::term world::term() const {
    return flecs::term(world_, ecs_pair(
        _::type<First>::id(world_),
        _::type<Second>::id(world_)));
}

}
