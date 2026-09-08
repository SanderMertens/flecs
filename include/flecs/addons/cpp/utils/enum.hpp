/**
 * @file addons/cpp/utils/enum.hpp
 * @brief Compile-time enum reflection utilities.
 *
 * Discover at compile time the valid enumeration constants for an enumeration type
 * and their names. This is used to automatically register enum constants.
 */

#include <utility>

// 126, so that FLECS_ENUM_MAX_COUNT is 127, which is the largest value
// representable by an int8_t.
#define FLECS_ENUM_MAX(T) _::to_constant<T, 126>::value
#define FLECS_ENUM_MAX_COUNT (FLECS_ENUM_MAX(int) + 1)

// Flag to turn off enum reflection.
#ifdef FLECS_CPP_NO_ENUM_REFLECTION
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 0
#endif

// Test if we're using a compiler that supports the required features.
#ifndef FLECS_CPP_ENUM_REFLECTION_SUPPORT
#if !defined(__clang__) && defined(__GNUC__)
#if __GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5)
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 1
#else
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 0
#endif
#else
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 1
#endif
#endif

#if defined(__clang__) && __clang_major__ >= 16
// https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
#define flecs_enum_cast(T, v) __builtin_bit_cast(T, v)
#elif defined(__GNUC__) && __GNUC__ > 10
#define flecs_enum_cast(T, v) __builtin_bit_cast(T, v)
#else
#define flecs_enum_cast(T, v) static_cast<T>(v)
#endif

namespace flecs {

/** Int to enum. */
namespace _ {
/** @private Convert an integral value to an enum constant. */
template <typename E, underlying_type_t<E> Value>
struct to_constant {
    static constexpr E value = flecs_enum_cast(E, Value);
};

template <typename E, underlying_type_t<E> Value>
constexpr E to_constant<E, Value>::value;
}

/** Convenience type with enum reflection data. */
template <typename E>
struct enum_data;

/** Get enum reflection data for an enum type.
 * @tparam E The enum type.
 * @param world The world.
 * @return Enum data wrapper.
 */
template <typename E>
static enum_data<E> enum_type(flecs::world_t *world);

/** Trait to define the last valid enum value for reflection.
 * @tparam E The enum type.
 */
template <typename E>
struct enum_last {
    static constexpr E value = FLECS_ENUM_MAX(E);
};

/* Utility macro to override the enum_last trait. */
#define FLECS_ENUM_LAST(T, Last)\
    namespace flecs {\
    template<>\
    struct enum_last<T> {\
        static constexpr T value = Last;\
    };\
    }

namespace _ {

#if defined(ECS_TARGET_CLANG) || defined(ECS_TARGET_GNU)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    const char *name = ECS_FUNC_NAME;
    size_t pos = sizeof(ECS_FUNC_NAME) - 1;
    while (pos && name[pos - 1] != '=') {
        pos --;
    }
    while (name[pos] == ' ') {
        pos ++;
    }
    return name[pos] != '(' && name[pos] != '-' &&
        !(name[pos] >= '0' && name[pos] <= '9');
}
#else
template <size_t N>
constexpr size_t enum_template_arg_separator(
    const char (&func_name)[N],
    size_t pos,
    size_t end,
    size_t depth = 0)
{
    return pos >= end
        ? end
        : func_name[pos] == '<'
            ? enum_template_arg_separator(func_name, pos + 1, end, depth + 1)
            : func_name[pos] == '>'
                ? enum_template_arg_separator(
                    func_name, pos + 1, end, depth ? depth - 1 : 0)
                : (func_name[pos] == ',' && !depth)
                    ? pos
                    : enum_template_arg_separator(
                        func_name, pos + 1, end, depth);
}

/* Use a different trick on MSVC, since it uses a hexadecimal representation for
 * invalid enum constants. We can leverage that MSVC inserts a C-style cast
 * into the name. Find the template argument separator structurally instead of
 * relying on the exact spelling of __FUNCSIG__ for the enum type. */
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return enum_template_arg_separator(
        ECS_FUNC_NAME,
        ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid),
        sizeof(ECS_FUNC_NAME) - ECS_FUNC_NAME_BACK - 1u) <
            (sizeof(ECS_FUNC_NAME) - ECS_FUNC_NAME_BACK - 1u) &&
        ECS_FUNC_NAME[enum_template_arg_separator(
            ECS_FUNC_NAME,
            ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid),
            sizeof(ECS_FUNC_NAME) - ECS_FUNC_NAME_BACK - 1u) + 1u] != '(';
}
#endif

/** @private Wrapper for enum_constant_is_valid() using the underlying type. */
template <typename E, underlying_type_t<E> C>
constexpr bool enum_constant_is_valid_wrap() {
    return enum_constant_is_valid<E, flecs_enum_cast(E, C)>();
}

/** @private Check if an enum constant is valid (value trait). */
template <typename E, E C>
struct enum_is_valid {
    static constexpr bool value = enum_constant_is_valid<E, C>();
};

/** @private Extract the name of a constant from the compiler string. */
template <typename E, E C>
static const char* enum_constant_to_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(
        const char*, enum_constant_to_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    return ecs_cpp_get_constant_name(
        result, ECS_FUNC_NAME, string::length(ECS_FUNC_NAME),
            ECS_FUNC_NAME_BACK);
}

/** Enumeration constant data.
 * @tparam T The underlying type of the enum.
 */
template<typename T>
struct enum_constant {
#ifdef FLECS_MULTI_WORLD
    /** Global index used to obtain a world-local entity ID. */
    int32_t index;
#else
    /** Entity ID for the constant. */
    flecs::entity_t id;
#endif
    /** The constant value. */
    T value;
    /** The constant name. */
    const char *name;

    bool discovered() const {
#ifdef FLECS_MULTI_WORLD
        return index != 0;
#else
        return name != nullptr || id != 0;
#endif
    }
};

/** @private Class that scans an enum for constants, extracts names, and creates entities. */
template <typename E>
struct enum_type {
private:
    using U = underlying_type_t<E>;
    using UU = typename std::make_unsigned<U>::type;
    static constexpr size_t linear_count =
        static_cast<size_t>(enum_last<E>::value) + 1;

    static constexpr size_t mask_start() {
        size_t result = 0;
        for (UU v = static_cast<UU>(enum_last<E>::value); v; v >>= 1) {
            result ++;
        }
        return result;
    }

    using candidates = std::make_index_sequence<
        linear_count + sizeof(U) * 8 - mask_start()>;

    template <size_t I>
    static constexpr U candidate() {
        if constexpr (I < linear_count) {
            return static_cast<U>(I);
        } else {
            return static_cast<U>(UU(1) << (I - linear_count + mask_start()));
        }
    }

    template <size_t... I>
    static constexpr unsigned int count_constants(std::index_sequence<I...>) {
        bool valid[] = {enum_constant_is_valid_wrap<E, candidate<I>()>()...};
        unsigned int result = 0;
        for (bool value : valid) {
            result += value;
        }
        return result;
    }

    template <U Value>
    void add_constant() {
        if constexpr (enum_constant_is_valid_wrap<E, Value>()) {
            auto& constant = constants[++max];
            if (static_cast<U>(max) == Value && contiguous_until == max) {
                contiguous_until ++;
            }
            constant.value = Value;
            constant.name = enum_constant_to_name<E, flecs_enum_cast(E, Value)>();
#ifdef FLECS_MULTI_WORLD
            constant.index = flecs_component_ids_index_get();
#endif
        }
    }

    template <size_t... I>
    void init(std::index_sequence<I...>) {
        int result[] = {(add_constant<candidate<I>()>(), 0)...};
        (void)result;
    }

public:
    enum_type() {
#if FLECS_CPP_ENUM_REFLECTION_SUPPORT
        init(candidates{});
#endif
    }

    int index_by_value(U value) const {
#ifdef FLECS_CPP_NO_ENUM_REFLECTION
        return value >= 0 && static_cast<UU>(value) < static_cast<UU>(contiguous_until)
            ? static_cast<int>(value) : -1;
#else
        if (value >= 0 && static_cast<UU>(value) < static_cast<UU>(contiguous_until)) {
            return static_cast<int>(value);
        }
        for (int i = contiguous_until; i <= max; i ++) {
            if (constants[i].value == value) {
                return i;
            }
        }
        return -1;
#endif
    }

    /** Get the singleton instance of enum_type for the given enum. */
    static enum_type<E>& get() {
        static _::enum_type<E> instance;
        return instance;
    }

#ifndef FLECS_MULTI_WORLD
    /** Get entity for a given enum value. */
    flecs::entity_t entity(E value) const {
        int index = index_by_value(static_cast<U>(value));
        if (index >= 0) {
            return constants[index].id;
        }
        return 0;
    }
#endif

    /** Register enum constants for a world. */
    void register_for_world(flecs::world_t *world, flecs::entity_t id) {
#if !FLECS_CPP_ENUM_REFLECTION_SUPPORT
        ecs_abort(ECS_UNSUPPORTED, "enum reflection requires gcc 7.5 or higher")
#endif

        ecs_log_push();
        ecs_cpp_enum_init(world, id, type<U>::id(world));

        for (int v = 0; v <= max; v ++) {
            if (constants[v].discovered()) {
                flecs::entity_t constant = ecs_cpp_enum_constant_register(world,
                    type<E>::id(world), 0, constants[v].name, &constants[v].value,
                    type<U>::id(world), sizeof(U));

#ifdef FLECS_MULTI_WORLD
                flecs_component_ids_set(world, constants[v].index, constant);
#else
                constants[v].id = constant;
#endif
            }
        }

        ecs_log_pop();
    }

    int min = 0;
    int max = -1;
    int contiguous_until = 0;

#if FLECS_CPP_ENUM_REFLECTION_SUPPORT
    static constexpr unsigned int constants_size = count_constants(candidates{});
    enum_constant<U> constants[constants_size ? constants_size : 1] = {};
#else
    static constexpr unsigned int constants_size = 0;
    enum_constant<U> constants[128] = {};
#endif
};

/** @private Initialize enum reflection for a world. */
template <typename E>
inline static void init_enum(flecs::world_t *world, flecs::entity_t id) {
    (void)world; (void)id;
    if constexpr (is_enum_v<E>) {
        _::enum_type<E>::get().register_for_world(world, id);
    }
}

} // namespace _

/** Enumeration type data wrapper with world pointer. */
template <typename E>
struct enum_data {
    using U = underlying_type_t<E>;

    /** Construct enum_data from a world and an enum_type implementation. */
    enum_data(flecs::world_t *world, _::enum_type<E>& impl)
        : world_(world)
        , impl_(impl) { }

	/**
     * @brief Check if a given integral value is a valid enum value.
     *
     * @param value The integral value.
     * @return true If the value is a valid enum value.
     * @return false If the value is not a valid enum value.
     */
    bool is_valid(U value) {
        int index = index_by_value(value);
        if (index < 0) {
            return false;
        }
        return impl_.constants[index].discovered();
    }

    /**
     * @brief Check if a given enum value is valid.
     *
     * @param value The enum value.
     * @return true If the value is valid.
     * @return false If the value is not valid.
     */
    bool is_valid(E value) {
        return is_valid(static_cast<U>(value));
    }

    /**
     * @brief Find the index into the constants array for a value, if one exists.
     *
     * @param value The underlying integral value.
     * @return int The index of the enum value.
     */
    int index_by_value(U value) const {
        return impl_.index_by_value(value);
    }

    /**
     * @brief Find the index into the constants array for an enum value, if one exists.
     *
     * @param value The enum value.
     * @return int The index of the enum value.
     */
    int index_by_value(E value) const {
        return index_by_value(static_cast<U>(value));
    }

    /** Return the index of the first constant. */
    int first() const {
        return impl_.min;
    }

    /** Return the index of the last constant. */
    int last() const {
        return impl_.max;
    }

    /** Return the next constant index after the given one. */
    int next(int cur) const {
        return cur + 1;
    }

    /** Get entity for the enum type. */
    flecs::entity entity() const;
    /** Get entity for a given underlying enum value. */
    flecs::entity entity(U value) const;
    /** Get entity for a given enum value. */
    flecs::entity entity(E value) const;

    /**
     * @brief Manually register a constant for an enum.
     *
     * If automatic enum reflection is not supported, provide a method for
     * manually registering a constant.
     */
    #ifdef FLECS_CPP_NO_ENUM_REFLECTION
    void register_constant(flecs::world_t *world, U v, flecs::entity_t e) {
        if (v >= 0 && v < 128) {
            int index = static_cast<int>(v);
#ifdef FLECS_MULTI_WORLD
            if (!impl_.constants[index].index) {
                impl_.constants[index].index = flecs_component_ids_index_get();
            }
#endif

#ifdef FLECS_MULTI_WORLD
            flecs_component_ids_set(world, impl_.constants[index].index, e);
#else
            (void)world;
            impl_.constants[index].id = e;
#endif

            impl_.max ++;

            if (impl_.contiguous_until <= index) {
                impl_.contiguous_until = index + 1;
            }
        }
    }
    #endif

    flecs::world_t *world_;
    _::enum_type<E>& impl_;
};

/** Convenience function for getting enum reflection data. */
template <typename E>
enum_data<E> enum_type(flecs::world_t *world) {
    _::type<E>::id(world); // Ensure the enum is registered.
    auto& ref = _::enum_type<E>::get();
    return enum_data<E>(world, ref);
}

} // namespace flecs
