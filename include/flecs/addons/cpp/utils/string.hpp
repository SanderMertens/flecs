// String utility that doesn't implicitly allocate memory.

namespace flecs {

struct string_view;

// This removes dependencies on std::string (and therefore STL) and allows the 
// API to return allocated strings without incurring additional allocations when
// wrapping in an std::string.
struct string {
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

    template <size_t N>
    static constexpr size_t length( char const (&)[N] ) {
        return N - 1;
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
struct string_view : string {
    explicit string_view(const char *str)
        : string(str) { }
};

}
