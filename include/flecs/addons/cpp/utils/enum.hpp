#include <string.h>
#include <stdio.h>

#define FLECS_ENUM_MAX(T) _::to_constant<T, 128>::value
#define FLECS_ENUM_MAX_COUNT (FLECS_ENUM_MAX(int) + 1)

namespace flecs {

/** Int to enum */
namespace _ {
template <typename E, int Value>
struct to_constant {
    static constexpr E value = static_cast<E>(Value);
};

template <typename E, int Value>
constexpr E to_constant<E, Value>::value;
}

/** Convenience type with enum reflection data */
template <typename E>
struct enum_data;

template <typename E>
static enum_data<E> enum_type(flecs::world_t *world);

template <typename E>
struct enum_max {
    static constexpr E value = FLECS_ENUM_MAX(E);
};

namespace _ {

/** Test if value is valid for enumeration.
 * This function leverages that when a valid value is provided, 
 * __PRETTY_FUNCTION__ contains the enumeration name, whereas if a value is
 * invalid, the string contains a number. */
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !(
        (__PRETTY_FUNCTION__[string::length(__PRETTY_FUNCTION__) - 2] >= '0') &&
        (__PRETTY_FUNCTION__[string::length(__PRETTY_FUNCTION__) - 2] <= '9')
    );
}

template <typename E, E C>
struct enum_is_valid {
    static constexpr bool value = enum_constant_is_valid<E, C>();
};

/** Extract name of constant from string */
template <typename E, E C>
static char* enum_constant_to_name() {
    const char *name = __PRETTY_FUNCTION__;
    ecs_size_t len = ecs_os_strlen(name);
    const char *last_space = strrchr(name, ' ');
    const char *last_paren = strrchr(name, ')');
    const char *last_colon = strrchr(name, ':');
    const char *start = (last_space > last_paren ? last_space : last_paren);
    start = start > last_colon ? start : last_colon;
    start ++;
    ecs_size_t constant_len = static_cast<ecs_size_t>(len - (start - name) - 1);
    char *result = ecs_os_malloc_n(char, constant_len + 1);
    ecs_os_memcpy_n(result, start, char, constant_len);
    result[constant_len] = '\0';
    return result;
}

/** Enumeration constant data */
struct enum_constant_data {
    flecs::entity_t id;
    int next;
};

/** Enumeration type data */
struct enum_data_impl {
    flecs::entity_t id;
    int min;
    int max;
    enum_constant_data constants[FLECS_ENUM_MAX_COUNT];
};

/** Class that scans an enum for constants, extracts names & creates entities */
template <typename E>
struct enum_type {
    static enum_data_impl data;

    static enum_type<E>& get(flecs::world_t *world) {
        flecs::entity_t enum_id = _::cpp_type<E>::id(world);
        return get(world, enum_id);
    }

    static enum_type<E>& get(flecs::world_t *world, flecs::entity_t enum_id) {
        static _::enum_type<E> instance(world, enum_id);
        return instance;
    }

private:
    template <E Value>
    static constexpr int to_int() {
        return static_cast<int>(Value);
    }

    template <int Value>
    static constexpr E from_int() {
        return to_constant<E, Value>::value;
    }

    template <E Value>
    static constexpr int is_not_0() {
        return static_cast<int>(Value != from_int<0>());
    }

    static void init_constant_entity(
        flecs::world_t *world,
        flecs::entity_t parent,
        const char *name,
        int v)
    {
        flecs::entity_t prev = ecs_set_scope(world, parent);
        ecs_entity_desc_t desc = {};
        desc.entity = data.constants[v].id;
        desc.name = name;
        flecs::entity_t id = ecs_entity_init(world, &desc);
        ecs_assert(id != 0, ECS_INVALID_OPERATION, name);
        data.constants[v].id = id;
        ecs_set_scope(world, prev);
    }

    template <E Value, flecs::if_not_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t*) { }

    template <E Value, flecs::if_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t *world) {
        int v = to_int<Value>();
        char *name = enum_constant_to_name<E, Value>();
        data.constants[v].next = data.min;

        if (v < data.min) {
            data.min = v;
        }
        if (v > data.max) {
            data.max = v;
        }

        init_constant_entity(world, data.id, name, v);
        ecs_os_free(name);
    }

    template <E Value = FLECS_ENUM_MAX(E) >
    static void init(flecs::world_t *world) {
        init_constant<Value>(world);
        if (is_not_0<Value>()) {
            init<from_int<to_int<Value>() - is_not_0<Value>()>()>(world);
        }
    }

    enum_type(flecs::world_t *world, flecs::entity_t id) {
        data.id = id;
        data.min = FLECS_ENUM_MAX(int);
        init< enum_max<E>::value >(world);
    }
};

template <typename E>
enum_data_impl enum_type<E>::data;

template <typename E, if_t< is_enum<E>::value > = 0>
inline static void init_enum(flecs::world_t *world, flecs::entity_t id) {
    _::enum_type<E>::get(world, id);
}

template <typename E, if_not_t< is_enum<E>::value > = 0>
inline static void init_enum(flecs::world_t*, flecs::entity_t) { }

} // namespace _

/** Enumeration type data wrapper with world pointer */
template <typename E>
struct enum_data {
    enum_data(flecs::world_t *world, _::enum_data_impl& impl) 
        : world_(world)
        , impl_(impl) { }

    bool is_valid(int value) {
        return impl_.constants[value].id != 0;
    }

    int first() {
        return impl_.min;
    }

    int next(int cur) {
        return impl_.constants[cur].next;
    }

    flecs::entity entity(int value);

    flecs::entity entity(E value);

    flecs::world_t *world_;
    _::enum_data_impl& impl_;
};

/** Convenience function for getting enum reflection data */
template <typename E>
enum_data<E> enum_type(flecs::world_t *world) {
    auto& ref = _::enum_type<E>::get(world, _::cpp_type<E>::id(world));
    return enum_data<E>(world, ref.data);
}

} // namespace flecs
