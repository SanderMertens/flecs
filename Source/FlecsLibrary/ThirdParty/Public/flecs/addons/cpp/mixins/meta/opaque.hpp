/**
 * @file addons/cpp/mixins/meta/opaque.hpp
 * @brief Helpers for opaque type registration.
 */

#pragma once

#include <stdio.h>

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @ingroup cpp_addons
 * Flecs reflection framework.
 *
 * @{
 */

/** Serializer object, used for serializing opaque types */
using serializer = ecs_serializer_t;

/** Serializer function, used to serialize opaque types */
using serialize_t = ecs_meta_serialize_t;

/** Type safe variant of serializer function */
template <typename T>
using serialize = int(*)(const serializer *, const T*);

/** Type safe interface for opaque types */
template <typename T, typename ElemType = void>
struct opaque {
    opaque(flecs::world_t *w = nullptr) : world(w) {
        if (world) {
            desc.entity = _::type<T>::id(world);
        }
    }

    /** Type that describes the type kind/structure of the opaque type */
    opaque& as_type(flecs::id_t func) {
        this->desc.type.as_type = func;
        return *this;
    }

    /** Serialize function */
    opaque& serialize(flecs::serialize<T> func) {
        this->desc.type.serialize =
            reinterpret_cast<decltype(
                this->desc.type.serialize)>(func);
        return *this;
    }

    /** Assign bool value */
    opaque& assign_bool(void (*func)(T *dst, bool value)) {
        this->desc.type.assign_bool =
            reinterpret_cast<decltype(
                this->desc.type.assign_bool)>(func);
        return *this;
    }

    /** Assign char value */
    opaque& assign_char(void (*func)(T *dst, char value)) {
        this->desc.type.assign_char =
            reinterpret_cast<decltype(
                this->desc.type.assign_char)>(func);
        return *this;
    }

    /** Assign int value */
    opaque& assign_int(void (*func)(T *dst, int64_t value)) {
        this->desc.type.assign_int =
            reinterpret_cast<decltype(
                this->desc.type.assign_int)>(func);
        return *this;
    }

    /** Assign unsigned int value */
    opaque& assign_uint(void (*func)(T *dst, uint64_t value)) {
        this->desc.type.assign_uint =
            reinterpret_cast<decltype(
                this->desc.type.assign_uint)>(func);
        return *this;
    }

    /** Assign float value */
    opaque& assign_float(void (*func)(T *dst, double value)) {
        this->desc.type.assign_float =
            reinterpret_cast<decltype(
                this->desc.type.assign_float)>(func);
        return *this;
    }

    /** Assign string value */
    opaque& assign_string(void (*func)(T *dst, const char *value)) {
        this->desc.type.assign_string =
            reinterpret_cast<decltype(
                this->desc.type.assign_string)>(func);
        return *this;
    }

    /** Assign entity value */
    opaque& assign_entity(
        void (*func)(T *dst, ecs_world_t *world, ecs_entity_t entity))
    {
        this->desc.type.assign_entity =
            reinterpret_cast<decltype(
                this->desc.type.assign_entity)>(func);
        return *this;
    }

    /** Assign (component) id value */
    opaque& assign_id(
        void (*func)(T *dst, ecs_world_t *world, ecs_id_t id))
    {
        this->desc.type.assign_id =
            reinterpret_cast<decltype(
                this->desc.type.assign_id)>(func);
        return *this;
    }

    /** Assign null value */
    opaque& assign_null(void (*func)(T *dst)) {
        this->desc.type.assign_null =
            reinterpret_cast<decltype(
                this->desc.type.assign_null)>(func);
        return *this;
    }

    /** Clear collection elements */
    opaque& clear(void (*func)(T *dst)) {
        this->desc.type.clear =
            reinterpret_cast<decltype(
                this->desc.type.clear)>(func);
        return *this;
    }

    /** Ensure & get collection element */
    opaque& ensure_element(ElemType* (*func)(T *dst, size_t elem)) {
        this->desc.type.ensure_element =
            reinterpret_cast<decltype(
                this->desc.type.ensure_element)>(func);
        return *this;
    }

    /** Ensure & get element */
    opaque& ensure_member(void* (*func)(T *dst, const char *member)) {
        this->desc.type.ensure_member =
            reinterpret_cast<decltype(
                this->desc.type.ensure_member)>(func);
        return *this;
    }

    /** Return number of elements */
    opaque& count(size_t (*func)(const T *dst)) {
        this->desc.type.count =
            reinterpret_cast<decltype(
                this->desc.type.count)>(func);
        return *this;
    }

    /** Resize to number of elements */
    opaque& resize(void (*func)(T *dst, size_t count)) {
        this->desc.type.resize =
            reinterpret_cast<decltype(
                this->desc.type.resize)>(func);
        return *this;
    }

    ~opaque() {
        if (world) {
            ecs_opaque_init(world, &desc);
        }
    }

    /** Opaque type descriptor */
    flecs::world_t *world = nullptr;
    ecs_opaque_desc_t desc = {};
};

/** @} */

}
