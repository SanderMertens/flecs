/**
 * @file addons/cpp/mixins/id/decl.hpp
 * @brief ID class.
 */

#pragma once

namespace flecs {

struct id;
struct entity;

/**
 * @defgroup cpp_ids IDs
 * @ingroup cpp_core
 * Class for working with entity, component, tag, and pair IDs.
 *
 * @{
 */

/** Class that wraps around a flecs::id_t.
 * A flecs ID is an identifier that can be added to entities. IDs can be:
 * - entities (including components, tags)
 * - pair IDs
 * - entities with ID flags set (like flecs::AUTO_OVERRIDE, flecs::TOGGLE)
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

    explicit id(flecs::world_t *world, const char *expr)
        : world_(world)
        , id_(ecs_id_from_str(world, expr)) { }

    explicit id(flecs::id_t first, flecs::id_t second)
        : world_(nullptr)
        , id_(ecs_pair(first, second)) { }

    explicit id(const flecs::id& first, const flecs::id& second)
        : world_(first.world_)
        , id_(ecs_pair(first.id_, second.id_)) { }

    /** Test if ID is a pair (has first, second). */
    bool is_pair() const {
        return (id_ & ECS_ID_FLAGS_MASK) == flecs::PAIR;
    }

    /** Test if ID is a wildcard. */
    bool is_wildcard() const {
        return ecs_id_is_wildcard(id_);
    }

    /** Test if ID is an entity. */
    bool is_entity() const {
        return !(id_ & ECS_ID_FLAGS_MASK);
    }

    /** Return ID as entity (only allowed when ID is a valid entity). */
    flecs::entity entity() const;

    /** Return ID with flags added. */
    flecs::entity add_flags(flecs::id_t flags) const;

    /** Return ID with flags removed. */
    flecs::entity remove_flags(flecs::id_t flags) const;

    /** Return ID without flags. */
    flecs::entity remove_flags() const;

    /** Return ID without generation. */
    flecs::entity remove_generation() const;

    /** Return component type of ID. */
    flecs::entity type_id() const;

    /** Test if ID has specified flags. */
    bool has_flags(flecs::id_t flags) const {
        return ((id_ & flags) == flags);
    }

    /** Test if ID has any flags. */
    bool has_flags() const {
        return (id_ & ECS_ID_FLAGS_MASK) != 0;
    }

    /** Return ID flags set on ID. */
    flecs::entity flags() const;

    /** Test if ID has the specified first element. */
    bool has_relation(flecs::id_t first) const {
        if (!is_pair()) {
            return false;
        }
        return ECS_PAIR_FIRST(id_) == first;
    }

    /** Get first element from a pair.
     * If the ID is not a pair, this operation will fail. When the ID has a
     * world, the operation will ensure that the returned ID has the correct
     * generation count. */
    flecs::entity first() const;

    /** Get second element from a pair.
     * If the ID is not a pair, this operation will fail. When the ID has a
     * world, the operation will ensure that the returned ID has the correct
     * generation count. */
    flecs::entity second() const;

    /** Convert ID to string. */
    flecs::string str() const {
        return flecs::string(ecs_id_str(world_, id_));
    }

    /** Convert flags of ID to string. */
    flecs::string flags_str() const {
        return flecs::string_view( ecs_id_flag_str(id_ & ECS_ID_FLAGS_MASK));
    }

    /** Return flecs::id_t value. */
    flecs::id_t raw_id() const {
        return id_;
    }

    operator flecs::id_t() const {
        return id_;
    }

    /** Get the world. */
    flecs::world world() const;

protected:
    /** World is optional, but guarantees that entity identifiers extracted from
     * the ID are valid. */
    flecs::world_t *world_;
    /** The raw ID value. */
    flecs::id_t id_;
};

/** @} */

}
