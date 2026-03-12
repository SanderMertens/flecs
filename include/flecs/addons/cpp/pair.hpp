/**
 * @file addons/cpp/pair.hpp
 * @brief Utilities for working with compile time pairs.
 */

#pragma once

namespace flecs {

namespace _ {
    struct pair_base { };
} // namespace _


/**
 * @defgroup cpp_pair_type Pair type
 * @ingroup cpp_core
 * Compile time utilities for working with relationship pairs.
 *
 * @{
 */

/** Type that represents a pair.
 * The pair type can be used to represent a pair at compile time, and is able
 * to automatically derive the storage type associated with the pair, accessible
 * through pair::type.
 *
 * The storage type is derived using the following rules:
 * - if pair::first is non-empty, the storage type is pair::first
 * - if pair::first is empty and pair::second is non-empty, the storage type is pair::second
 *
 * The pair type can hold a temporary value so that it can be used in the
 * signatures of queries
 */
template <typename First, typename Second>
struct pair : _::pair_base {
    using type = conditional_t<!is_empty<First>::value || is_empty<Second>::value, First, Second>; /**< The storage type of the pair. */
    using first = First; /**< The first element type of the pair. */
    using second = Second; /**< The second element type of the pair. */

    /** Construct pair from a mutable reference to the storage type.
     *
     * @param v Reference to the value.
     */
    pair(type& v) : ref_(v) { }

    /** Construct pair from a const reference to the storage type.
     * This allows the class to be used as a temporary object.
     *
     * @param v Const reference to the value.
     */
    pair(const type& v) : ref_(const_cast<type&>(v)) { }

    /** Conversion to mutable reference of the storage type. */
    operator type&() {
        return ref_;
    }

    /** Conversion to const reference of the storage type. */
    operator const type&() const {
        return ref_;
    }

    /** Arrow operator for mutable access. */
    type* operator->() {
        return &ref_;
    }

    /** Arrow operator for const access. */
    const type* operator->() const {
        return &ref_;
    }

    /** Dereference operator for mutable access. */
    type& operator*() {
        return ref_;
    }

    /** Dereference operator for const access. */
    const type& operator*() const {
        return ref_;
    }

private:
    type& ref_;
};

/** Alias for pair when First is empty (tag). */
template <typename First, typename Second, if_t<is_empty<First>::value> = 0>
using pair_object = pair<First, Second>;

/** Get raw type by removing pointer and reference qualifiers. */
template <typename T>
using raw_type_t = remove_pointer_t<remove_reference_t<T>>;

/** Test if type is a pair. */
template <typename T>
struct is_pair {
    static constexpr bool value = is_base_of_v<_::pair_base, raw_type_t<T>>;
};

/** Convenience variable template to check if type is a pair. */
template <typename T>
inline constexpr bool is_pair_v = is_pair<T>::value;

/** Get pair::first from pair while preserving cv qualifiers. */
template <typename P>
using pair_first_t = transcribe_cv_t<remove_reference_t<P>, typename raw_type_t<P>::first>;

/** Get pair::second from pair while preserving cv qualifiers. */
template <typename P>
using pair_second_t = transcribe_cv_t<remove_reference_t<P>, typename raw_type_t<P>::second>;

/** Get pair::type type from pair while preserving cv qualifiers and pointer type. */
template <typename P>
using pair_type_t = transcribe_cvp_t<remove_reference_t<P>, typename raw_type_t<P>::type>;

/** Get actual type from a regular type or pair. */
template <typename T, typename U = int>
struct actual_type;

template <typename T>
struct actual_type<T, if_not_t< is_pair<T>::value >> {
    using type = T;
};

template <typename T>
struct actual_type<T, if_t< is_pair<T>::value >> {
    using type = pair_type_t<T>;
};

template <typename T>
using actual_type_t = typename actual_type<T>::type;


/** Get type without const, *, &. */
template<typename T>
struct base_type {
    using type = decay_t< actual_type_t<T> >;
};

/** Convenience alias for base_type. */
template <typename T>
using base_type_t = typename base_type<T>::type;


/** Get type without *, & (retains const which is useful for function args). */
template<typename T>
struct base_arg_type {
    using type = remove_pointer_t< remove_reference_t< actual_type_t<T> > >;
};

/** Convenience alias for base_arg_type. */
template <typename T>
using base_arg_type_t = typename base_arg_type<T>::type;


/** Test if type is the same as its actual type. */
template <typename T>
struct is_actual {
    static constexpr bool value = is_same_v<T, actual_type_t<T>>;
};

/** Convenience variable template to check if type is its own actual type. */
template <typename T>
inline constexpr bool is_actual_v = is_actual<T>::value;

/** @} */

} // namespace flecs
