/**
 * @file addons/cpp/utils/enum.hpp
 * @brief Compile time enum reflection utilities.
 * 
 * Discover at compile time valid enumeration constants for an enumeration type
 * and their names. This is used to automatically register enum constants.
 */

#include <string.h>
#include <limits>

#define FLECS_ENUM_MAX(T) _::to_constant<T, 128>::value
#define FLECS_ENUM_MAX_COUNT (FLECS_ENUM_MAX(int) + 1)

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

namespace flecs {

/** Int to enum */
namespace _ {
template <typename E, underlying_type_t<E> Value>
struct to_constant {
#if defined(__clang__) && __clang_major__ >= 16
    // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
    static constexpr E value = __builtin_bit_cast(E, Value);
#else
    static constexpr E value = static_cast<E>(Value);
#endif
};

template <typename E, underlying_type_t<E> Value>
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

#if INTPTR_MAX == INT64_MAX
    #ifdef ECS_TARGET_MSVC
        #if _MSC_VER >= 1929
            #define ECS_SIZE_T_STR "unsigned __int64"
        #else
            #define ECS_SIZE_T_STR "unsigned int"
        #endif 
    #elif defined(__clang__)
        #define ECS_SIZE_T_STR "size_t"
    #else
        #ifdef ECS_TARGET_WINDOWS
            #define ECS_SIZE_T_STR "constexpr size_t; size_t = long long unsigned int"
        #else
            #define ECS_SIZE_T_STR "constexpr size_t; size_t = long unsigned int"
        #endif
    #endif
#else
    #ifdef ECS_TARGET_MSVC
        #if _MSC_VER >= 1929
            #define ECS_SIZE_T_STR "unsigned __int32"
        #else
            #define ECS_SIZE_T_STR "unsigned int"
        #endif 
    #elif defined(__clang__)
        #define ECS_SIZE_T_STR "size_t"
    #else
        #ifdef ECS_TARGET_WINDOWS
            #define ECS_SIZE_T_STR "constexpr size_t; size_t = unsigned int"
        #else
            #define ECS_SIZE_T_STR "constexpr size_t; size_t = unsigned int"
        #endif
    #endif
#endif

template <typename E>
constexpr size_t enum_type_len() {
    return ECS_FUNC_TYPE_LEN(, enum_type_len, ECS_FUNC_NAME) 
        - (sizeof(ECS_SIZE_T_STR) - 1u);
}

/** Test if value is valid for enumeration.
 * This function leverages that when a valid value is provided, 
 * __PRETTY_FUNCTION__ contains the enumeration name, whereas if a value is
 * invalid, the string contains a number or a negative (-) symbol. */
#if defined(ECS_TARGET_CLANG)
#if ECS_CLANG_VERSION < 13
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !((
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] >= '0') &&
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] <= '9')) ||
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] == '-'));
}
#else
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
        enum_type_len<E>() + 6 /* ', E C = ' */] != '(');
}
#endif
#elif defined(ECS_TARGET_GNU)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(constexpr bool, enum_constant_is_valid) +
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
        result, ECS_FUNC_NAME, string::length(ECS_FUNC_NAME),
            ECS_FUNC_NAME_BACK);
}

/** Enumeration constant data */
template<typename T>
struct enum_constant_data {
    flecs::entity_t id;
    T offset;
};

/**
 * @brief Provides utilities for enum reflection.
 * 
 * This struct provides static functions for enum reflection, including conversion
 * between enum values and their underlying integral types, and iteration over enum
 * values.
 * 
 * @tparam E The enum type.
 * @tparam Handler The handler for enum reflection operations.
 */
template <typename E, template<typename> class Handler>
struct enum_reflection {
    template <E Value>
    static constexpr underlying_type_t<E> to_int() {
        return static_cast<underlying_type_t<E>>(Value);
    }

    template <underlying_type_t<E> Value>
    static constexpr E from_int() {
        return to_constant<E, Value>::value;
    }

    template <E Value>
    static constexpr underlying_type_t<E> is_not_0() {
        return static_cast<underlying_type_t<E>>(Value != from_int<0>());
    }

    /**
     * @brief Iterates over the range [Low, High] of enum values between Low and High.
     *
     * Recursively divide and conquers the search space to reduce the template-depth. Once
     * recursive division is complete, calls Handle<E>::handle_constant in ascending order,
     * passing the values computed up the chain.
     * 
     * @tparam Low The lower bound of the search range, inclusive.
     * @tparam High The upper bound of the search range, inclusive.
     * @tparam Args Additional arguments to be passed through to Handler<E>::handle_constant
     * @param last_value The last value processed in the iteration.
     * @param args Additional arguments to be passed through to Handler<E>::handle_constant
     * @return constexpr underlying_type_t<E> The result of the iteration.
     */
    template <E Low, E High, typename... Args>
    static constexpr underlying_type_t<E> each_enum_range(underlying_type_t<E> last_value, Args... args) {
        return to_int<High>() - to_int<Low>() <= 1
            ? to_int<High>() == to_int<Low>()
                ? Handler<E>::template handle_constant<Low>(last_value, args...)
                : Handler<E>::template handle_constant<High>(Handler<E>::template handle_constant<Low>(last_value, args...), args...)
            : each_enum_range<from_int<(to_int<Low>()+to_int<High>()) / 2 + 1>(), High>(
                    each_enum_range<Low, from_int<(to_int<Low>()+to_int<High>()) / 2>()>(last_value, args...),
                    args...
              );
    }

    /**
     * @brief Iterates over the mask range (Low, High] of enum values between Low and High.
     *
     * Recursively iterates the search space, looking for enums defined as multiple-of-2 
     * bitmasks. Each iteration, shifts bit to the right until it hits Low, then calls
     * Handler<E>::handle_constant for each bitmask in ascending order.
     * 
     * @tparam Low The lower bound of the search range, not inclusive
     * @tparam High The upper bound of the search range, inclusive.
     * @tparam Args Additional arguments to be passed through to Handler<E>::handle_constant
     * @param last_value The last value processed in the iteration.
     * @param args Additional arguments to be passed through to Handler<E>::handle_constant
     * @return constexpr underlying_type_t<E> The result of the iteration.
     */
    template <E Low, E High, typename... Args>
    static constexpr underlying_type_t<E> each_mask_range(underlying_type_t<E> last_value, Args... args) {
        // If Low shares any bits with Current Flag, or if High is less than/equal to Low (and High isn't negative because max-flag signed)
        return (to_int<Low>() & to_int<High>()) || (to_int<High>() <= to_int<Low>() && to_int<High>() != high_bit)
            ? last_value
            : Handler<E>::template handle_constant<High>(
                each_mask_range<Low, from_int<((to_int<High>() >> 1) & ~high_bit)>()>(last_value, args...),
                args...
              );
    }

    /**
     * @brief Handles enum iteration for gathering reflection data.
     *
     * Iterates over all enum values up to a specified maximum value 
     * (each_enum_range<0, Value>), then iterates the rest of the possible bitmasks
     * (each_mask_range<Value, high_bit>).
     * 
     * @tparam Value The maximum enum value to iterate up to.
     * @tparam Args Additional arguments to be passed through to Handler<E>::handle_constant
     * @param args Additional arguments to be passed through to Handler<E>::handle_constant
     * @return constexpr underlying_type_t<E> The result of the iteration.
     */
    template <E Value = FLECS_ENUM_MAX(E), typename... Args>
    static constexpr underlying_type_t<E> each_enum(Args... args) {
        return each_mask_range<Value, from_int<high_bit>()>(each_enum_range<from_int<0>(), Value>(0, args...), args...);
    }

    static const underlying_type_t<E> high_bit = static_cast<underlying_type_t<E>>(1) << (sizeof(underlying_type_t<E>) * 8 - 1);
};

/** Enumeration type data */
template<typename E>
struct enum_data_impl {
private:
    /**
     * @brief Handler struct for generating compile-time count of enum constants.
     */
    template<typename Enum>
    struct reflection_count {
        template <Enum Value, flecs::if_not_t< enum_constant_is_valid<Enum, Value>() > = 0>
        static constexpr underlying_type_t<Enum> handle_constant(underlying_type_t<E> last_value) {
            return last_value;
        }

        template <Enum Value, flecs::if_t< enum_constant_is_valid<Enum, Value>() > = 0>
        static constexpr underlying_type_t<Enum> handle_constant(underlying_type_t<E> last_value) {
            return 1 + last_value;
        }
    };
public:
    flecs::entity_t id;
    int min;
    int max;
    bool has_contiguous;
	// If enum constants start not-sparse, contiguous_until will be the index of the first sparse value, or end of the constants array
    underlying_type_t<E> contiguous_until;
	// Compile-time generated count of enum constants.
    static constexpr unsigned int constants_size = enum_reflection<E, reflection_count>::template each_enum< enum_last<E>::value >();
    // Constants array is sized to the number of found-constants, or 1 (to avoid 0-sized array)
    enum_constant_data<underlying_type_t<E>> constants[constants_size? constants_size: 1];
};

/** Class that scans an enum for constants, extracts names & creates entities */
template <typename E>
struct enum_type {
private:
    /**
     * @brief Helper struct for filling enum_type's static `enum_data_impl<E>` member with reflection data.
     *
     * Because reflection occurs in-order, we can use current value/last value to determine continuity, and
     * use that as a lookup heuristic later on.
     * 
     * @tparam Enum The enum type.
     */
    template <typename Enum>
    struct reflection_init {
        template <Enum Value, flecs::if_not_t< enum_constant_is_valid<Enum, Value>() > = 0>
        static underlying_type_t<Enum> handle_constant(underlying_type_t<Enum> last_value, flecs::world_t*) {
            // Search for constant failed. Pass last valid value through.
            return last_value;
        }

        template <Enum Value, flecs::if_t< enum_constant_is_valid<Enum, Value>() > = 0>
        static underlying_type_t<Enum> handle_constant(underlying_type_t<Enum> last_value, flecs::world_t *world) {
            // Constant is valid, so fill reflection data.
            auto v = enum_reflection<E, reflection_init>::template to_int<Value>();
            const char *name = enum_constant_to_name<Enum, Value>();
            
            ++enum_type<Enum>::data.max; // Increment cursor as we build constants array.

            // If the enum was previously contiguous, and continues to be through the current value...
            if (enum_type<Enum>::data.has_contiguous && static_cast<underlying_type_t<Enum>>(enum_type<Enum>::data.max) == v && enum_type<Enum>::data.contiguous_until == v) {
                ++enum_type<Enum>::data.contiguous_until;
            }
            // else, if the enum was never contiguous and hasn't been set as not contiguous...
            else if (!enum_type<Enum>::data.contiguous_until && enum_type<Enum>::data.has_contiguous) {
                enum_type<Enum>::data.has_contiguous = false;
            }

            ecs_assert(!(last_value > 0 && v < std::numeric_limits<underlying_type_t<Enum>>::min() + last_value), ECS_UNSUPPORTED,
                "Signed integer enums causes integer overflow when recording offset from high positive to"
                " low negative. Consider using unsigned integers as underlying type.");
            enum_type<Enum>::data.constants[enum_type<Enum>::data.max].offset = v - last_value;
            enum_type<Enum>::data.constants[enum_type<Enum>::data.max].id = ecs_cpp_enum_constant_register(
                world, enum_type<Enum>::data.id, 0, name, static_cast<int32_t>(v));
            return v;
        }
    };
public:

    static enum_data_impl<E> data;

    static enum_type<E>& get() {
        static _::enum_type<E> instance;
        return instance;
    }

    flecs::entity_t entity(E value) const {
        int index = index_by_value(value);
        if (index >= 0) {
            return data.constants[index].id;
        }
        return 0;
    }

    void init(flecs::world_t *world, flecs::entity_t id) {
#if !FLECS_CPP_ENUM_REFLECTION_SUPPORT
        ecs_abort(ECS_UNSUPPORTED, "enum reflection requires gcc 7.5 or higher")
#endif
        // Initialize/reset reflection data values to default state.
        data.min = 0;
        data.max = -1;
        data.has_contiguous = true;
        data.contiguous_until = 0;

        ecs_log_push();
        ecs_cpp_enum_init(world, id);
        data.id = id;

        // Generate reflection data
        enum_reflection<E, reflection_init>::template each_enum< enum_last<E>::value >(world);
        ecs_log_pop();
    }

};

template <typename E>
enum_data_impl<E> enum_type<E>::data;

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
    enum_data(flecs::world_t *world, _::enum_data_impl<E>& impl)
        : world_(world)
        , impl_(impl) { }
    
	/**
     * @brief Checks if a given integral value is a valid enum value.
     * 
     * @param value The integral value.
     * @return true If the value is a valid enum value.
     * @return false If the value is not a valid enum value.
     */
    bool is_valid(underlying_type_t<E> value) {
        int index = index_by_value(value);
        if (index < 0) {
            return false;
        }
        return impl_.constants[index].id != 0;
    }

    /**
     * @brief Checks if a given enum value is valid.
     * 
     * @param value The enum value.
     * @return true If the value is valid.
     * @return false If the value is not valid.
     */
    bool is_valid(E value) {
        return is_valid(static_cast<underlying_type_t<E>>(value));
    }

    /**
     * @brief Finds the index into the constants array for a value, if one exists
     * 
     * @param value The enum value.
     * @return int The index of the enum value.
     */
    int index_by_value(underlying_type_t<E> value) const {
        if (!impl_.max) {
            return -1;
        }
        // Check if value is in contiguous lookup section
        if (impl_.has_contiguous && value < impl_.contiguous_until && value >= 0) {
            return static_cast<int>(value);
        }
        underlying_type_t<E> accumulator = impl_.contiguous_until? impl_.contiguous_until - 1: 0;
        for (int i = static_cast<int>(impl_.contiguous_until); i <= impl_.max; ++i) {
            accumulator += impl_.constants[i].offset;
            if (accumulator == value) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Finds the index into the constants array for an enum value, if one exists
     * 
     * @param value The enum value.
     * @return int The index of the enum value.
     */
    int index_by_value(E value) const {
        return index_by_value(static_cast<underlying_type_t<E>>(value));
    }

    int first() const {
        return impl_.min;
    }

    int last() const {
        return impl_.max;
    }

    int next(int cur) const {
        return cur + 1;
    }

    flecs::entity entity() const;
    flecs::entity entity(underlying_type_t<E> value) const;
    flecs::entity entity(E value) const;

    flecs::world_t *world_;
    _::enum_data_impl<E>& impl_;
};

/** Convenience function for getting enum reflection data */
template <typename E>
enum_data<E> enum_type(flecs::world_t *world) {
    _::type<E>::id(world); // Ensure enum is registered
    auto& ref = _::enum_type<E>::get();
    return enum_data<E>(world, ref.data);
}

} // namespace flecs
