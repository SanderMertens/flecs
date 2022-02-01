#include <string.h>

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
struct enum_last {
    static constexpr E value = FLECS_ENUM_MAX(E);
};

/* Utility macro to override enum_last trait */
#define FLECS_ENUM_LAST(T, Last)\
    namespace flecs {\
    template<>\
    struct enum_last<T> {\
        static constexpr T value = Last;\
    };\
    }

namespace _ {

/** Test if value is valid for enumeration.
 * This function leverages that when a valid value is provided, 
 * __PRETTY_FUNCTION__ contains the enumeration name, whereas if a value is
 * invalid, the string contains a number. */
#ifndef ECS_TARGET_MSVC
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !(
        (ECS_FUNC_NAME[string::length(ECS_FUNC_NAME) - (ECS_FUNC_NAME_BACK + 1)] >= '0') &&
        (ECS_FUNC_NAME[string::length(ECS_FUNC_NAME) - (ECS_FUNC_NAME_BACK + 1)] <= '9')
    );
}
#else
/* Use different trick on MSVC, since it uses hexadecimal representation for
 * invalid enum constants. We can leverage that msvc inserts a C-style cast
 * into the name, and the location of its first character ('(') is known. */
template <typename E>
constexpr size_t enum_type_len() {
    return ECS_FUNC_TYPE_LEN(, enum_type_len, ECS_FUNC_NAME) 
        - (sizeof("unsigned __int64") - 1u);
}

template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
        enum_type_len<E>() + 1] != '(';
}
#endif

template <typename E, E C>
struct enum_is_valid {
    static constexpr bool value = enum_constant_is_valid<E, C>();
};

/** Extract name of constant from string */
template <typename E, E C>
static const char* enum_constant_to_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, enum_constant_to_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    return ecs_cpp_get_constant_name(
        result, ECS_FUNC_NAME, string::length(ECS_FUNC_NAME));
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

    flecs::entity_t entity(E value) const {
        return data.constants[static_cast<int>(value)].id;
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

    template <E Value, flecs::if_not_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t*) { }

    template <E Value, flecs::if_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t *world) {
        int v = to_int<Value>();
        const char *name = enum_constant_to_name<E, Value>();
        data.constants[v].next = data.min;
        data.min = v;
        if (!data.max) {
            data.max = v;
        }

        data.constants[v].id = ecs_cpp_enum_constant_register(
            world, data.id, data.constants[v].id, name, v);
    }

    template <E Value = FLECS_ENUM_MAX(E) >
    static void init(flecs::world_t *world) {
        init_constant<Value>(world);
        if (is_not_0<Value>()) {
            init<from_int<to_int<Value>() - is_not_0<Value>()>()>(world);
        }
    }

    enum_type(flecs::world_t *world, flecs::entity_t id) {
        ecs_add_id(world, id, flecs::Exclusive);
        ecs_add_id(world, id, flecs::Tag);
        data.id = id;
        data.min = FLECS_ENUM_MAX(int);
        init< enum_last<E>::value >(world);
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

    int last() {
        return impl_.max;
    }

    int next(int cur) {
        return impl_.constants[cur].next;
    }

    flecs::entity entity();
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
