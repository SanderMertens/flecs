////////////////////////////////////////////////////////////////////////////////
//// Flecs STL (FTL?)
//// Minimalistic utilities that allow for STL like functionality without having
//// to depend on the actual STL.
////////////////////////////////////////////////////////////////////////////////

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
template<class Ty> inline void free_obj(Ty* _ptr) { 
    if (_ptr) {
        destruct_obj(_ptr); 
        ecs_os_free(_ptr); 
    }
}

} // namespace _

} // namespace flecs

inline void* operator new(size_t,   flecs::_::placement_new_tag_t, void* _ptr) noexcept { return _ptr; }
inline void  operator delete(void*, flecs::_::placement_new_tag_t, void*)      noexcept {              }

namespace flecs
{

// C++11/C++14 convenience template replacements

template <bool V, typename T, typename F>
using conditional_t = typename std::conditional<V, T, F>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

template <bool V, typename T = void>
using enable_if_t = typename std::enable_if<V, T>::type;

template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

using std::is_base_of;
using std::is_empty;
using std::is_const;
using std::is_pointer;
using std::is_reference;
using std::is_volatile;
using std::is_same;


// Apply cv modifiers from source type to destination type
// (from: https://stackoverflow.com/questions/52559336/add-const-to-type-if-template-arg-is-const)
template<class Src, class Dst>
using transcribe_const_t = conditional_t<is_const<Src>::value, Dst const, Dst>;

template<class Src, class Dst>
using transcribe_volatile_t = conditional_t<is_volatile<Src>::value, Dst volatile, Dst>;

template<class Src, class Dst>
using transcribe_cv_t = transcribe_const_t< Src, transcribe_volatile_t< Src, Dst> >;


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


// String handling

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

    bool operator==(const flecs::string& str) const {
        if (str.m_const_str == m_const_str) {
            return true;
        }

        if (!m_const_str || !str.m_const_str) {
            return false;
        }

        if (str.m_length != m_length) {
            return false;
        }

        return ecs_os_strcmp(str, m_const_str) == 0;
    }

    bool operator!=(const flecs::string& str) const {
        return !(*this == str);
    }    

    bool operator==(const char *str) const {
        if (m_const_str == str) {
            return true;
        }

        if (!m_const_str || !str) {
            return false;
        }

        return ecs_os_strcmp(str, m_const_str) == 0;
    }

    bool operator!=(const char *str) const {
        return !(*this == str);
    }    

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
class array<T, Size, enable_if_t<Size != 0> > {
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

    size_t size() {
        return Size;
    }

    T* ptr() {
        return m_array;
    }
private:
    T m_array[Size];
};

// Specialized class for zero-sized array
template <typename T, size_t Size>
class array<T, Size, enable_if_t<Size == 0>> {
public:
    array() {};
    array(const T* (&elems)) { (void)elems; }
    T operator[](size_t index) { abort(); (void)index; return T(); }
    array_iterator<T> begin() { return array_iterator<T>(nullptr, 0); }
    array_iterator<T> end() { return array_iterator<T>(nullptr, 0); }

    size_t size() {
        return 0;
    }

    T* ptr() {
        return NULL;
    }
};

namespace _
{

// Utility to prevent static assert from immediately triggering
template <class... T>
struct always_false {
    static const bool value = false;
};

} // namespace _

} // namespace flecs
