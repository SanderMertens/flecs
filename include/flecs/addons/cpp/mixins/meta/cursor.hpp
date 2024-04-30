/**
 * @file addons/cpp/mixins/meta/opaque.hpp
 * @brief Helpers for opaque type registration.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @ingroup cpp_addons
 * Flecs reflection framework.
 *
 * @{
 */

/** Class for reading/writing dynamic values.
 *
 * @ingroup cpp_addons_meta
 */
struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        cursor_ = ecs_meta_cursor(world, type_id, ptr);
    }

    /** Push value scope (such as a nested struct) */
    int push() {
        return ecs_meta_push(&cursor_);
    }

    /** Pop value scope */
    int pop() {
        return ecs_meta_pop(&cursor_);
    }

    /** Move to next member/element */
    int next() {
        return ecs_meta_next(&cursor_);
    }

    /** Move to member by name */
    int member(const char *name) {
        return ecs_meta_member(&cursor_, name);
    }

    /** Move to element by index */
    int elem(int32_t elem) {
        return ecs_meta_elem(&cursor_, elem);
    }

    /** Test if current scope is a collection type */
    bool is_collection() {
        return ecs_meta_is_collection(&cursor_);
    }

    /** Get member name */
    flecs::string_view get_member() const {
        return flecs::string_view(ecs_meta_get_member(&cursor_));
    }

    /** Get type of value */
    flecs::entity get_type() const;

    /** Get unit of value */
    flecs::entity get_unit() const;

    /** Get untyped pointer to value */
    void* get_ptr() {
        return ecs_meta_get_ptr(&cursor_);
    }

    /** Set boolean value */
    int set_bool(bool value) {
        return ecs_meta_set_bool(&cursor_, value);
    }

    /** Set char value */
    int set_char(char value) {
        return ecs_meta_set_char(&cursor_, value);
    }

    /** Set signed int value */
    int set_int(int64_t value) {
        return ecs_meta_set_int(&cursor_, value);
    }

    /** Set unsigned int value */
    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&cursor_, value);
    }

    /** Set float value */
    int set_float(double value) {
        return ecs_meta_set_float(&cursor_, value);
    }

    /** Set string value */
    int set_string(const char *value) {
        return ecs_meta_set_string(&cursor_, value);
    }

    /** Set string literal value */
    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&cursor_, value);
    }

    /** Set entity value */
    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&cursor_, value);
    }

    /** Set (component) id value */
    int set_id(flecs::id_t value) {
        return ecs_meta_set_id(&cursor_, value);
    }

    /** Set null value */
    int set_null() {
        return ecs_meta_set_null(&cursor_);
    }

    /** Get boolean value */
    bool get_bool() const {
        return ecs_meta_get_bool(&cursor_);
    }

    /** Get char value */
    char get_char() const {
        return ecs_meta_get_char(&cursor_);
    }

    /** Get signed int value */
    int64_t get_int() const {
        return ecs_meta_get_int(&cursor_);
    }

    /** Get unsigned int value */
    uint64_t get_uint() const {
        return ecs_meta_get_uint(&cursor_);
    }

    /** Get float value */
    double get_float() const {
        return ecs_meta_get_float(&cursor_);
    }

    /** Get string value */
    const char *get_string() const {
        return ecs_meta_get_string(&cursor_);
    }

    /** Get entity value */
    flecs::entity get_entity() const;

    /** Cursor object */
    ecs_meta_cursor_t cursor_;
};

/** @} */

}
