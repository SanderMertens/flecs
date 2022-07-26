
namespace flecs {

namespace _ {
    struct pair_base { };   
} // _


// Type that represents a pair and can encapsulate a temporary value
template <typename First, typename Second>
struct pair : _::pair_base { 
    // Traits used to deconstruct the pair

    // The actual type of the pair is determined by which type of the pair is
    // empty. If both types are empty or not empty, the pair assumes the type
    // of the first element.
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


// Utilities to test if type is a pair
template <typename T>
struct is_pair {
    static constexpr bool value = is_base_of<_::pair_base, remove_reference_t<T> >::value;
};


// Get actual type, relationship or object from pair while preserving cv qualifiers.
template <typename P>
using pair_first_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::first>;

template <typename P>
using pair_second_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::second>;

template <typename P>
using pair_type_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::type>;


// Get actual type from a regular type or pair
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
