/**
 * @file addons/cpp/utils/utils.hpp
 * @brief Flecs STL (FTL?)
 * 
 * Flecs STL (FTL?)
 * Minimalistic utilities that allow for STL like functionality without having
 * to depend on the actual STL.
 */

// Macros so that C++ new calls can allocate using ecs_os_api memory allocation functions
// Rationale:
//  - Using macros here instead of a templated function bc clients might override ecs_os_malloc
//    to contain extra debug info like source tracking location. Using a template function
//    in that scenario would collapse all source location into said function vs. the
//    actual call site
//  - FLECS_PLACEMENT_NEW(): exists to remove any naked new calls/make it easy to identify any regressions
//    by grepping for new/delete

#define FLECS_PLACEMENT_NEW(_ptr, _type)  ::new(flecs::_::placement_new_tag, _ptr) _type
#define FLECS_NEW(_type)                  FLECS_PLACEMENT_NEW(ecs_os_malloc(sizeof(_type)), _type)
#define FLECS_DELETE(_ptr)          \
  do {                              \
    if (_ptr) {                     \
      flecs::_::destruct_obj(_ptr); \
      ecs_os_free(_ptr);            \
    }                               \
  } while (false)

/* Faster (compile time) alternatives to std::move / std::forward. From:
 *   https://www.foonathan.net/2020/09/move-forward/
 */

#define FLECS_MOV(...) \
  static_cast<flecs::remove_reference_t<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)

#define FLECS_FWD(...) \
  static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

namespace flecs 
{

namespace _
{

// Dummy Placement new tag to disambiguate from any other operator new overrides
struct placement_new_tag_t{};
constexpr placement_new_tag_t placement_new_tag{};
template<class Ty> inline void destruct_obj(Ty* _ptr) { _ptr->~Ty(); }
template<class Ty> inline void free_obj(void* _ptr) { 
    if (_ptr) {
        destruct_obj(static_cast<Ty*>(_ptr)); 
        ecs_os_free(_ptr); 
    }
}

} // namespace _

} // namespace flecs

// Allows overriding flecs_static_assert, which is useful when testing
#ifndef flecs_static_assert
#define flecs_static_assert(cond, str) static_assert(cond, str)
#endif

inline void* operator new(size_t,   flecs::_::placement_new_tag_t, void* _ptr) noexcept { return _ptr; }
inline void  operator delete(void*, flecs::_::placement_new_tag_t, void*)      noexcept {              }

namespace flecs
{

// faster (compile time) alternative to std::conditional
template <bool> struct condition;

template <> struct condition<false> {
    template <typename T, typename F> using type = F;
};

template <> struct condition<true> {
    template <typename T, typename F> using type = T;
};

// C++11/C++14 convenience template replacements

template <bool C, typename T, typename F>
using conditional_t = typename condition<C>::template type<T, F>;

template <typename T>
using decay_t = typename std::decay<T>::type;

template <bool V, typename T = void>
using enable_if_t = typename std::enable_if<V, T>::type;

template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template <typename T>
using underlying_type_t = typename std::underlying_type<T>::type;

using std::is_base_of;
using std::is_empty;
using std::is_const;
using std::is_pointer;
using std::is_reference;
using std::is_volatile;
using std::is_same;
using std::is_enum;

// GCC 4.9.2 compatibility: missing C++11 type traits
#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 9)
// Direct implementations for missing type traits in GCC 4.9.2
} // namespace flecs

namespace std {
    // Only implement the ones that are actually missing in GCC 4.9.2
    template<typename T>
    struct is_trivially_constructible {
        static const bool value = __is_pod(T);
    };

    template<typename T>
    struct is_trivially_move_assignable {
        static const bool value = __is_pod(T);
    };

    template<typename T>
    struct is_trivially_copy_assignable {
        static const bool value = __is_pod(T);
    };

    template<typename T>
    struct is_trivially_copy_constructible {
        static const bool value = __is_pod(T);
    };

    template<typename T>
    struct is_trivially_move_constructible {
        static const bool value = __is_pod(T);
    };

    template<typename T>
    struct is_trivially_copyable {
        static const bool value = __is_pod(T);
    };
}

namespace flecs {
#else
using std::is_trivially_constructible;
using std::is_trivially_move_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_copyable;
#endif

// These exist in GCC 4.9.2, so we can always use them
using std::is_move_assignable;
using std::is_move_constructible;
using std::is_copy_constructible;
using std::is_trivially_destructible;

// Determine constness even if T is a pointer type
template <typename T>
using is_const_p = is_const< remove_pointer_t<T> >;

// Apply cv modifiers from source type to destination type
// (from: https://stackoverflow.com/questions/52559336/add-const-to-type-if-template-arg-is-const)
template<class Src, class Dst>
using transcribe_const_t = conditional_t<is_const<Src>::value, Dst const, Dst>;

template<class Src, class Dst>
using transcribe_volatile_t = conditional_t<is_volatile<Src>::value, Dst volatile, Dst>;

template<class Src, class Dst>
using transcribe_cv_t = transcribe_const_t< Src, transcribe_volatile_t< Src, Dst> >;

template<class Src, class Dst>
using transcribe_pointer_t = conditional_t<is_pointer<Src>::value, Dst*, Dst>;

template<class Src, class Dst>
using transcribe_cvp_t = transcribe_cv_t< Src, transcribe_pointer_t< Src, Dst> >;


// More convenience templates. The if_*_t templates use int as default type
// instead of void. This enables writing code that's a bit less cluttered when
// the templates are used in a template declaration:
//
//     enable_if_t<true>* = nullptr
// vs:
//     if_t<true> = 0

template <bool V>
using if_t = enable_if_t<V, int>;

template <bool V>
using if_not_t = enable_if_t<false == V, int>;

namespace _
{

// Utility to prevent static assert from immediately triggering
template <class... T>
struct always_false {
    static const bool value = false;
};

} // namespace _

} // namespace flecs

#include "array.hpp"
#include "string.hpp"
#include "enum.hpp"
#include "stringstream.hpp"
#include "function_traits.hpp"
