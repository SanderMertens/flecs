/**
 * @file addons/cpp/mixins/id/decl.hpp
 * @brief Id class.
 */

#pragma once

namespace flecs {

struct id;
struct entity;

/**
 * @defgroup cpp_ids Ids
 * @ingroup cpp_core
 * Class for working with entity, component, tag and pair ids.
 *
 * @{
 */

/** Class that wraps around a flecs::id_t.
 * A flecs id is an identifier that can be added to entities. Ids can be:
 * - entities (including components, tags)
 * - pair ids
 * - entities with id flags set (like flecs::AUTO_OVERRIDE, flecs::TOGGLE)
 */
struct id {
    id()
        : world_(nullptr)
        , id_(0) { }

    explicit id(flecs::id_t value)
        : world_(nullptr)
        , id_(value) { }

    explicit id(flecs::world_t *world, flecs::id_t value = 0)
        : world_(world)
        , id_(value) { }

    explicit id(flecs::world_t *world, flecs::id_t first, flecs::id_t second)
        : world_(world)
        , id_(ecs_pair(first, second)) { }

    explicit id(flecs::id_t first, flecs::id_t second)
        : world_(nullptr)
        , id_(ecs_pair(first, second)) { }

    explicit id(const flecs::id& first, const flecs::id& second)
        : world_(first.world_)
        , id_(ecs_pair(first.id_, second.id_)) { }

    /** Test if id is pair (has first, second) */
    bool is_pair() const {
        return (id_ & ECS_ID_FLAGS_MASK) == flecs::PAIR;
    }

    /** Test if id is a wildcard */
    bool is_wildcard() const {
        return ecs_id_is_wildcard(id_);
    }

    /** Test if id is entity */
    bool is_entity() const {
        return !(id_ & ECS_ID_FLAGS_MASK);
    }

    /** Return id as entity (only allowed when id is valid entity) */
    flecs::entity entity() const;

    /** Return id with role added */
    flecs::entity add_flags(flecs::id_t flags) const;

    /** Return id with role removed */
    flecs::entity remove_flags(flecs::id_t flags) const;

    /** Return id without role */
    flecs::entity remove_flags() const;

    /** Return id without role */
    flecs::entity remove_generation() const;

    /** Return component type of id */
    flecs::entity type_id() const;

    /** Test if id has specified role */
    bool has_flags(flecs::id_t flags) const {
        return ((id_ & flags) == flags);
    }

    /** Test if id has any role */
    bool has_flags() const {
        return (id_ & ECS_ID_FLAGS_MASK) != 0;
    }

    /** Return id flags set on id */
    flecs::entity flags() const;

    /** Test if id has specified first */
    bool has_relation(flecs::id_t first) const {
        if (!is_pair()) {
            return false;
        }
        return ECS_PAIR_FIRST(id_) == first;
    }

    /** Get first element from a pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count. */
    flecs::entity first() const;

    /** Get second element from a pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count. */
    flecs::entity second() const;

    /* Convert id to string */
    flecs::string str() const {
        return flecs::string(ecs_id_str(world_, id_));
    }

    /** Convert role of id to string. */
    flecs::string flags_str() const {
        return flecs::string_view( ecs_id_flag_str(id_ & ECS_ID_FLAGS_MASK));
    }

    /** Return flecs::id_t value */
    flecs::id_t raw_id() const {
        return id_;
    }

    operator flecs::id_t() const {
        return id_;
    }

    flecs::world world() const;

protected:
    /* World is optional, but guarantees that entity identifiers extracted from
     * the id are valid */
    flecs::world_t *world_;
    flecs::id_t id_;
};

/** @} */

}
