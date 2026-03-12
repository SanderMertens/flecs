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
    /** Default constructor. */
    term()
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) { }

    /** Construct from a world. */
    term(flecs::world_t *world_ptr)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) { }

    /** Construct from a world and an existing term descriptor. */
    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            value = t;
            this->set_term(&value);
        }

    /** Construct from a world and a component ID. */
    term(flecs::world_t *world_ptr, id_t component_id)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            if (component_id & ECS_ID_FLAGS_MASK) {
                value.id = component_id;
            } else {
                value.first.id = component_id;
            }
            this->set_term(&value);
        }

    /** Construct from a world and a pair of entity IDs. */
    term(flecs::world_t *world_ptr, entity_t first, entity_t second)
        : term_builder_i<term>(&value)
        , value({})
        , world_(world_ptr) {
            value.id = ecs_pair(first, second);
            this->set_term(&value);
        }

    /** Construct from a component ID (no world). */
    term(id_t component_id)
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) {
            if (component_id & ECS_ID_FLAGS_MASK) {
                value.id = component_id;
            } else {
                value.first.id = component_id;
            }
        }

    /** Construct from a pair of IDs (no world). */
    term(id_t first, id_t second)
        : term_builder_i<term>(&value)
        , value({})
        , world_(nullptr) {
            value.first.id = first;
            value.second.id = second;
        }

    /** Reset the term to its default state. */
    void reset() {
        value = {};
        this->set_term(nullptr);
    }

    /** Check if the term is initialized. */
    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    /** Get the term ID. */
    flecs::id id() {
        return flecs::id(world_, value.id);
    }

    /** Get the inout kind of the term. */
    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    /** Get the operator kind of the term. */
    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    /** Get the source entity of the term. */
    flecs::entity get_src() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.src));
    }

    /** Get the first element of the term. */
    flecs::entity get_first() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.first));
    }

    /** Get the second element of the term. */
    flecs::entity get_second() {
        return flecs::entity(world_, ECS_TERM_REF_ID(&value.second));
    }

    /** Convert to the underlying term_t. */
    operator flecs::term_t() const {
        return value;
    }

    /** The underlying term value. */
    flecs::term_t value;

protected:
    flecs::world_t* world_v() override { return world_; }

private:
    flecs::world_t *world_;
};

/** Term mixin implementation. */
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
