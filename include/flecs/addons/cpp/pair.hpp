/**
 * @file addons/cpp/pair.hpp
 * @brief Utilities for working with compile time pairs.
 */

#pragma once

namespace flecs {

namespace _ {
    struct pair_base { };   
} // _


/**
 * @defgroup cpp_pair_type Pair type
 * @brief Compile time utilities for working with relationship pairs.
 * 
 * \ingroup cpp_core
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
    using type = conditional_t<!is_empty<First>::value || is_empty<Second>::value, First, Second>;
    using first = First;
    using second = Second;

    pair(type& v) : ref_(v) { }

    // This allows the class to be used as a temporary object
    pair(const type& v) : ref_(const_cast<type&>(v)) { }

    operator type&() { 
        return ref_;
    }

    operator const type&() const { 
        return ref_;
    }    

    type* operator->() {
        return &ref_;
    }

    const type* operator->() const {
        return &ref_;
    }

    type& operator*() {
        return ref_;
    }

    const type& operator*() const {
        return ref_;
    }
    
private:
    type& ref_;
};

template <typename First, typename Second, if_t<is_empty<First>::value> = 0>
using pair_object = pair<First, Second>;


/** Test if type is a pair. */
template <typename T>
struct is_pair {
    static constexpr bool value = is_base_of<_::pair_base, remove_reference_t<T> >::value;
};


/** Get pair::first from pair while preserving cv qualifiers. */
template <typename P>
using pair_first_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::first>;

/** Get pair::second from pair while preserving cv qualifiers. */
template <typename P>
using pair_second_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::second>;

/** Get pair::type type from pair while preserving cv qualifiers. */
template <typename P>
using pair_type_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::type>;

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


// Get type without const, *, &
template<typename T>
struct base_type {
    using type = decay_t< remove_pointer_t< actual_type_t<T> > >;
};

template <typename T>
using base_type_t = typename base_type<T>::type;


// Get type without *, & (retains const which is useful for function args)
template<typename T>
struct base_arg_type {
    using type = remove_pointer_t< remove_reference_t< actual_type_t<T> > >;
};

template <typename T>
using base_arg_type_t = typename base_arg_type<T>::type;


// Test if type is the same as its actual type
template <typename T>
struct is_actual {
    static constexpr bool value = 
        std::is_same<T, actual_type_t<T> >::value && !is_enum<T>::value;
};

} // flecs
