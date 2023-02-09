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

/** Serializer object, used for serializing opaque types */
using serializer = ecs_meta_serializer_t;

/** Serializer function, used to serialize opaque types */
using serialize_t = ecs_meta_serialize_t;

/** Type safe variant of serializer function */
template <typename T>
using serialize = int(*)(const serializer *, const T*);

/** Type safe interface for opaque types */
template <typename T, typename ElemType = void>
struct opaque {
    opaque(flecs::world_t *world = nullptr) : world(world) {
        if (world) {
            desc.entity = _::cpp_type<T>::id(world);
        }
    }

    /** Type that describes the type kind/structure of the opaque type */
    opaque& as_type(flecs::id_t as_type) {
        this->desc.type.as_type = as_type;
        return *this;
    }

    /** Serialize function */
    opaque& serialize(flecs::serialize<T> serialize) {
        this->desc.type.serialize = 
            reinterpret_cast<decltype(
                this->desc.type.serialize)>(serialize);
        return *this;
    }

    /** Assign bool value */
    opaque& assign_bool(void (*assign_bool)(T *dst, bool value)) {
        this->desc.type.assign_bool = 
            reinterpret_cast<decltype(
                this->desc.type.assign_bool)>(assign_bool);
        return *this;
    }

    /** Assign char value */
    opaque& assign_char(void (*assign_char)(T *dst, char value)) {
        this->desc.type.assign_char = 
            reinterpret_cast<decltype(
                this->desc.type.assign_char)>(assign_char);
        return *this;
    }

    /** Assign int value */
    opaque& assign_int(void (*assign_int)(T *dst, int64_t value)) {
        this->desc.type.assign_int = 
            reinterpret_cast<decltype(
                this->desc.type.assign_int)>(assign_int);
        return *this;
    }

    /** Assign unsigned int value */
    opaque& assign_uint(void (*assign_uint)(T *dst, uint64_t value)) {
        this->desc.type.assign_uint = 
            reinterpret_cast<decltype(
                this->desc.type.assign_uint)>(assign_uint);
        return *this;
    }

    /** Assign float value */
    opaque& assign_float(void (*assign_float)(T *dst, double value)) {
        this->desc.type.assign_float = 
            reinterpret_cast<decltype(
                this->desc.type.assign_float)>(assign_float);
        return *this;
    }

    /** Assign string value */
    opaque& assign_string(void (*assign_string)(T *dst, const char *value)) {
        this->desc.type.assign_string = 
            reinterpret_cast<decltype(
                this->desc.type.assign_string)>(assign_string);
        return *this;
    }

    /** Assign entity value */
    opaque& assign_entity(
        void (*assign_entity)(T *dst, ecs_world_t *world, ecs_entity_t entity)) 
    {
        this->desc.type.assign_entity = 
            reinterpret_cast<decltype(
                this->desc.type.assign_entity)>(assign_entity);
        return *this;
    }

    /** Assign null value */
    opaque& assign_null(void (*assign_null)(T *dst)) {
        this->desc.type.assign_null = 
            reinterpret_cast<decltype(
                this->desc.type.assign_null)>(assign_null);
        return *this;
    }

    /** Clear collection elements */
    opaque& clear(void (*clear)(T *dst)) {
        this->desc.type.clear = 
            reinterpret_cast<decltype(
                this->desc.type.clear)>(clear);
        return *this;
    }

    /** Ensure & get collection element */
    opaque& ensure_element(ElemType* (*ensure_element)(T *dst, size_t elem)) {
        this->desc.type.ensure_element = 
            reinterpret_cast<decltype(
                this->desc.type.ensure_element)>(ensure_element);
        return *this;
    }

    /** Ensure & get element */
    opaque& ensure_member(void* (*ensure_member)(T *dst, const char *member)) {
        this->desc.type.ensure_member = 
            reinterpret_cast<decltype(
                this->desc.type.ensure_member)>(ensure_member);
        return *this;
    }

    /** Return number of elements */
    opaque& count(size_t (*count)(const T *dst)) {
        this->desc.type.count = 
            reinterpret_cast<decltype(
                this->desc.type.count)>(count);
        return *this;
    }
    
    /** Resize to number of elements */
    opaque& resize(void (*resize)(T *dst, size_t count)) {
        this->desc.type.resize = 
            reinterpret_cast<decltype(
                this->desc.type.resize)>(resize);
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
