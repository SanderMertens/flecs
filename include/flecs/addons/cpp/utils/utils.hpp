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

using std::conditional_t;
using std::decay_t;
using std::enable_if_t;
using std::remove_pointer_t;
using std::remove_reference_t;
using std::underlying_type_t;
using std::is_base_of;
using std::is_empty;
using std::is_const;
using std::is_pointer;
using std::is_reference;
using std::is_volatile;
using std::is_same;
using std::is_enum;
using std::is_trivially_constructible;
using std::is_trivially_move_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;
using std::is_trivially_copyable;
using std::is_move_assignable;
using std::is_move_constructible;
using std::is_copy_constructible;
using std::is_trivially_destructible;
using std::is_empty_v;
using std::is_const_v;
using std::is_pointer_v;
using std::is_reference_v;
using std::is_volatile_v;
using std::is_same_v;
using std::is_enum_v;
using std::is_base_of_v;
using std::is_trivially_constructible_v;
using std::is_trivially_destructible_v;
using std::is_trivially_copyable_v;
using std::is_trivially_move_constructible_v;
using std::is_trivially_copy_constructible_v;
using std::is_trivially_move_assignable_v;
using std::is_trivially_copy_assignable_v;
using std::is_default_constructible_v;
using std::is_move_constructible_v;
using std::is_copy_constructible_v;
using std::is_move_assignable_v;
using std::is_copy_assignable_v;
using std::is_destructible_v;

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

namespace flecs {
namespace _ {

// Trick to obtain typename from type, as described here
// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
//
// The code from the link has been modified to work with more types, and across
// multiple compilers. The resulting string should be the same on all platforms
// for all compilers.
//

#if defined(__GNUC__) || defined(_WIN32)
template <typename T>
inline const char* type_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, type_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    static const size_t front_len = ECS_FUNC_NAME_FRONT(const char*, type_name);
    static const char* cppTypeName = ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len, front_len);
    return cppTypeName;
}
#else
#error "implicit component registration not supported"
#endif

}
}
