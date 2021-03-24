
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

namespace flecs
{

////////////////////////////////////////////////////////////////////////////////
//// Flecs STL (FTL?)
//// Minimalistic utilities that allow for STL like functionality without having
//// to depend on the actual STL.
////////////////////////////////////////////////////////////////////////////////

class string_view;

// This removes dependencies on std::string (and therefore STL) and allows the 
// API to return allocated strings without incurring additional allocations when
// wrapping in an std::string.
class string {
public:
    explicit string() 
        : m_str(nullptr)
        , m_const_str("")
        , m_length(0) { }

    explicit string(char *str) 
        : m_str(str)
        , m_const_str(str ? str : "")
        , m_length(str ? ecs_os_strlen(str) : 0) { }

    ~string() {
        // If flecs is included in a binary but is not used, it is possible that
        // the OS API is not initialized. Calling ecs_os_free in that case could
        // crash the application during exit. However, if a string has been set
        // flecs has been used, and OS API should have been initialized.
        if (m_str) {
            ecs_os_free(m_str);
        }
    }

    string(string&& str) {
        ecs_os_free(m_str);
        m_str = str.m_str;
        m_const_str = str.m_const_str;
        m_length = str.m_length;
        str.m_str = nullptr;
    }

    operator const char*() const {
        return m_const_str;
    }

    string& operator=(string&& str) {
        ecs_os_free(m_str);
        m_str = str.m_str;
        m_const_str = str.m_const_str;
        m_length = str.m_length;
        str.m_str = nullptr;
        return *this;
    }

    // Ban implicit copies/allocations
    string& operator=(const string& str) = delete;
    string(const string& str) = delete;

    const char* c_str() const {
        return m_const_str;
    }

    std::size_t length() {
        return static_cast<std::size_t>(m_length);
    }

    std::size_t size() {
        return length();
    }

    void clear() {
        ecs_os_free(m_str);
        m_str = nullptr;
        m_const_str = nullptr;
    }

protected:
    // Must be constructed through string_view. This allows for using the string
    // class for both owned and non-owned strings, which can reduce allocations
    // when code conditionally should store a literal or an owned string.
    // Making this constructor private forces the code to explicitly create a
    // string_view which emphasizes that the string won't be freed by the class.
    string(const char *str)
        : m_str(nullptr)
        , m_const_str(str ? str : "")
        , m_length(str ? ecs_os_strlen(str) : 0) { }

    char *m_str = nullptr;
    const char *m_const_str;
    ecs_size_t m_length;
};

// For consistency, the API returns a string_view where it could have returned
// a const char*, so an application won't have to think about whether to call
// c_str() or not. The string_view is a thin wrapper around a string that forces
// the API to indicate explicitly when a string is owned or not.
class string_view : public string {
public:
    explicit string_view(const char *str)
        : string(str) { }
};

// Wrapper around ecs_strbuf_t that provides a simple stringstream like API.
class stringstream {
public:
    explicit stringstream() 
        : m_buf({}) { }

    ~stringstream() {
        ecs_strbuf_reset(&m_buf);
    }

    stringstream(stringstream&& str) {
        ecs_strbuf_reset(&m_buf);
        m_buf = str.m_buf;
        str.m_buf = {};
    }

    stringstream& operator=(stringstream&& str) {
        ecs_strbuf_reset(&m_buf);
        m_buf = str.m_buf;
        str.m_buf = {};
        return *this;
    }

    // Ban implicit copies/allocations
    stringstream& operator=(const stringstream& str) = delete;
    stringstream(const stringstream& str) = delete;    

    stringstream& operator<<(const char* str) {
        ecs_strbuf_appendstr(&m_buf, str);
        return *this;
    }

    flecs::string str() {
        return flecs::string(ecs_strbuf_get(&m_buf));
    }

private:
    ecs_strbuf_t m_buf;
};

// Array class. Simple std::array like utility that is mostly there to aid
// template code, where the expanded array size would be 0.
template <typename T>
class array_iterator
{
public:
    explicit array_iterator(T* value, int index) {
        m_value = value;
        m_index = index;
    }

    bool operator!=(array_iterator const& other) const
    {
        return m_index != other.m_index;
    }

    T & operator*() const
    {
        return m_value[m_index];
    }

    array_iterator& operator++()
    {
        ++m_index;
        return *this;
    }

private:
    T* m_value;
    int m_index;
};

template <typename T, size_t Size, class Enable = void> 
class array { };

template <typename T, size_t Size>
class array<T, Size, typename std::enable_if<Size != 0>::type> {
public:
    array() {};

    array(const T (&elems)[Size]) {
        int i = 0;
        for (auto it = this->begin(); it != this->end(); ++ it) {
            *it = elems[i ++];
        }
    }

    T& operator[](size_t index) {
        return m_array[index];
    }

    array_iterator<T> begin() {
        return array_iterator<T>(m_array, 0);
    }

    array_iterator<T> end() {
        return array_iterator<T>(m_array, Size);
    }
private:
    T m_array[Size];
};

// Specialized class for zero-sized array
template <typename T, size_t Size>
class array<T, Size, typename std::enable_if<Size == 0>::type> {
public:
    array() {};
    array(const T* (&elems)) { (void)elems; }
    T operator[](size_t index) { abort(); (void)index; return T(); }
    array_iterator<T> begin() { return array_iterator<T>(nullptr, 0); }
    array_iterator<T> end() { return array_iterator<T>(nullptr, 0); }
};


////////////////////////////////////////////////////////////////////////////////
//// Utility to convert template argument pack to array of columns
////////////////////////////////////////////////////////////////////////////////
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

    using Columns = flecs::array<Column, sizeof...(Components)>;

    column_args(ecs_iter_t* iter) {
        populate_columns(iter, 0, static_cast<typename std::remove_reference<typename std::remove_pointer<Components>::type>::type*>(nullptr)...);
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
bool pack_args_to_string(world_t *world, flecs::stringstream& str, bool is_each = false) {
    (void)world;

    flecs::array<const char*, sizeof...(Components)> ids ({
        (_::cpp_type<Components>::name(world))...
    });
    
    flecs::array<const char*, sizeof...(Components)> inout_modifiers ({
        (inout_modifier<Components>())...
    });

    flecs::array<const char*, sizeof...(Components)> optional_modifiers ({
        (optional_modifier<Components>())...
    });

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
