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

#ifdef ECS_TARGET_MSVC
#define ECS_SIZE_T_STR "unsigned __int64"
#elif defined(__clang__)
#define ECS_SIZE_T_STR "size_t"
#else
#define ECS_SIZE_T_STR "constexpr size_t; size_t = long unsigned int"
#endif

template <typename E>
constexpr size_t enum_type_len() {
    return ECS_FUNC_TYPE_LEN(, enum_type_len, ECS_FUNC_NAME) 
        - (sizeof(ECS_SIZE_T_STR) - 1u);
}

/** Test if value is valid for enumeration.
 * This function leverages that when a valid value is provided, 
 * __PRETTY_FUNCTION__ contains the enumeration name, whereas if a value is
 * invalid, the string contains a number. */
#if defined(__clang__)
#if __clang_major__ < 13 || (defined(__APPLE__) && __clang_minor__ < 1)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !(
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] >= '0') &&
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] <= '9'));
}
#else
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
        enum_type_len<E>() + 6 /* ', E C = ' */] != '(');
}
#endif
#elif defined(__GNUC__)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(constepxr bool, enum_constant_is_valid) +
        enum_type_len<E>() + 8 /* ', E C = ' */] != '(');
}
#else
/* Use different trick on MSVC, since it uses hexadecimal representation for
 * invalid enum constants. We can leverage that msvc inserts a C-style cast
 * into the name, and the location of its first character ('(') is known. */
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

    static enum_type<E>& get() {
        static _::enum_type<E> instance;
        return instance;
    }

    flecs::entity_t entity(E value) const {
        return data.constants[static_cast<int>(value)].id;
    }

    void init(flecs::world_t *world, flecs::entity_t id) {
#if !defined(__clang__) && defined(__GNUC__)
        ecs_assert(__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5), 
            ECS_UNSUPPORTED, "enum component types require gcc 7.5 or higher");
#endif

        ecs_log_push();
        ecs_add_id(world, id, flecs::Exclusive);
        ecs_add_id(world, id, flecs::OneOf);
        ecs_add_id(world, id, flecs::Tag);
        data.id = id;
        data.min = FLECS_ENUM_MAX(int);
        init< enum_last<E>::value >(world);
        ecs_log_pop();
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
};

template <typename E>
enum_data_impl enum_type<E>::data;

template <typename E, if_t< is_enum<E>::value > = 0>
inline static void init_enum(flecs::world_t *world, flecs::entity_t id) {
    _::enum_type<E>::get().init(world, id);
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

    int first() const {
        return impl_.min;
    }

    int last() const {
        return impl_.max;
    }

    int next(int cur) const {
        return impl_.constants[cur].next;
    }

    flecs::entity entity() const;
    flecs::entity entity(int value) const;
    flecs::entity entity(E value) const;

    flecs::world_t *world_;
    _::enum_data_impl& impl_;
};

/** Convenience function for getting enum reflection data */
template <typename E>
enum_data<E> enum_type(flecs::world_t *world) {
    _::cpp_type<E>::id(world); // Ensure enum is registered
    auto& ref = _::enum_type<E>::get();
    return enum_data<E>(world, ref.data);
}

} // namespace flecs
