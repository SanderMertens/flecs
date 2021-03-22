
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

namespace flecs 
{

namespace _
{

// Dummy Placement new tag to disambiguate from any other operator new overrides
struct placement_new_tag_t{};
constexpr placement_new_tag_t placement_new_tag{};
template<class Ty> inline void destruct_obj(Ty* _ptr) { _ptr->~Ty(); }

} // namespace _

} // namespace flecs

inline void* operator new(size_t,   flecs::_::placement_new_tag_t, void* _ptr) noexcept { return _ptr; }
inline void  operator delete(void*, flecs::_::placement_new_tag_t, void*)      noexcept {              }


////////////////////////////////////////////////////////////////////////////////
//// Utility to convert template argument pack to array of columns
////////////////////////////////////////////////////////////////////////////////

namespace flecs 
{
    // Utility to get actual type
    template<typename Type>
    struct base_type {
        typedef typename std::remove_pointer<
            typename std::decay<Type>::type>::type type;
    };

namespace _ 
{

template <typename ... Components>
class column_args {
public:    
    struct Column {
        void *ptr;
        bool is_shared;
    };

    using Columns = std::array<Column, sizeof...(Components)>;

    column_args(ecs_iter_t* iter) {
        populate_columns(iter, 0, (typename std::remove_reference<typename std::remove_pointer<Components>::type>::type*)nullptr...);
    }

    Columns m_columns;

private:
    /* Dummy function when last component has been added */
    void populate_columns(ecs_iter_t *iter, size_t index) { 
        (void)iter;
        (void)index;
    }

    /* Populate columns array recursively */
    template <typename T, typename... Targs>
    void populate_columns(ecs_iter_t *iter, size_t index, T comp, Targs... comps) {
        int32_t column = static_cast<int32_t>(index + 1);
        void *ptr = ecs_column_w_size(iter, sizeof(*comp), column);
        m_columns[index].ptr = ptr;
        m_columns[index].is_shared = !ecs_is_owned(iter, column) && ptr != nullptr;
        populate_columns(iter, index + 1, comps ...);
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Utility to convert type trait to flecs signature syntax */
////////////////////////////////////////////////////////////////////////////////

template <typename T,
    typename std::enable_if< std::is_const<T>::value == true, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "[in] ";
}

template <typename T,
    typename std::enable_if< std::is_reference<T>::value == true, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "[out] ";
}

template <typename T,
    typename std::enable_if<std::is_const<T>::value == false && std::is_reference<T>::value == false, void>::type* = nullptr>
constexpr const char *inout_modifier() {
    return "";
}

template <typename T,
    typename std::enable_if< std::is_pointer<T>::value == true, void>::type* = nullptr>
constexpr const char *optional_modifier() {
    return "?";
}

template <typename T,
    typename std::enable_if< std::is_pointer<T>::value == false, void>::type* = nullptr>
constexpr const char *optional_modifier() {
    return "";
} 

/** Convert template arguments to string */
template <typename ...Components>
bool pack_args_to_string(world_t *world, std::stringstream& str, bool is_each = false) {
    (void)world;

    std::array<const char*, sizeof...(Components)> ids = {
        (_::component_info<Components>::name(world))...
    };

    std::array<const char*, sizeof...(Components)> inout_modifiers = {
        (inout_modifier<Components>())...
    }; 

    std::array<const char*, sizeof...(Components)> optional_modifiers = {
        (optional_modifier<Components>())...
    };        

    size_t i = 0;
    for (auto id : ids) {
        if (i) {
            str << ",";
        }
        
        str << inout_modifiers[i];
        str << optional_modifiers[i];

        if (is_each) {
            str << "ANY:";
        }
        str << id;
        i ++;
    }  

    return i != 0;
}

} // namespace _

} // namespace flecs
