/**
 * @file addons/cpp/mixins/meta/opaque.hpp
 * @brief Helpers for opaque type registration.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Class for reading/writing dynamic values.
 * 
 * \ingroup cpp_addons_meta
 */
struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        m_cursor = ecs_meta_cursor(world, type_id, ptr);
    }

    /** Push value scope (such as a nested struct) */
    int push() {
        return ecs_meta_push(&m_cursor);
    }

    /** Pop value scope */
    int pop() {
        return ecs_meta_pop(&m_cursor);
    }

    /** Move to next member/element */
    int next() {
        return ecs_meta_next(&m_cursor);
    }

    /** Move to member by name */
    int member(const char *name) {
        return ecs_meta_member(&m_cursor, name);
    }

    /** Move to element by index */
    int elem(int32_t elem) {
        return ecs_meta_elem(&m_cursor, elem);
    }

    /** Test if current scope is a collection type */
    bool is_collection() {
        return ecs_meta_is_collection(&m_cursor);
    }

    /** Get member name */
    flecs::string_view get_member() const {
        return flecs::string_view(ecs_meta_get_member(&m_cursor));
    }

    /** Get type of value */
    flecs::entity get_type() const;

    /** Get unit of value */
    flecs::entity get_unit() const;

    /** Get untyped pointer to value */
    void* get_ptr() {
        return ecs_meta_get_ptr(&m_cursor);
    }

    /** Set boolean value */
    int set_bool(bool value) {
        return ecs_meta_set_bool(&m_cursor, value);
    }

    /** Set char value */
    int set_char(char value) {
        return ecs_meta_set_char(&m_cursor, value);
    }

    /** Set signed int value */
    int set_int(int64_t value) {
        return ecs_meta_set_int(&m_cursor, value);
    }

    /** Set unsigned int value */
    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&m_cursor, value);
    }

    /** Set float value */
    int set_float(double value) {
        return ecs_meta_set_float(&m_cursor, value);
    }

    /** Set string value */
    int set_string(const char *value) {
        return ecs_meta_set_string(&m_cursor, value);
    }

    /** Set string literal value */
    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&m_cursor, value);
    }

    /** Set entity value */
    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&m_cursor, value);
    }

    /** Set null value */
    int set_null() {
        return ecs_meta_set_null(&m_cursor);
    }

    /** Get boolean value */
    bool get_bool() const {
        return ecs_meta_get_bool(&m_cursor);
    }

    /** Get char value */
    char get_char() const {
        return ecs_meta_get_char(&m_cursor);
    }

    /** Get signed int value */
    int64_t get_int() const {
        return ecs_meta_get_int(&m_cursor);
    }

    /** Get unsigned int value */
    uint64_t get_uint() const {
        return ecs_meta_get_uint(&m_cursor);
    }

    /** Get float value */
    double get_float() const {
        return ecs_meta_get_float(&m_cursor);
    }

    /** Get string value */
    const char *get_string() const {
        return ecs_meta_get_string(&m_cursor);
    }

    /** Get entity value */
    flecs::entity get_entity() const;

    /** Cursor object */
    ecs_meta_cursor_t m_cursor;
};

/** @} */

}
