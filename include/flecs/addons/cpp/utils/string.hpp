/**
 * @file addons/cpp/utils/string.hpp
 * @brief String utility that doesn't implicitly allocate memory.
 */

namespace flecs {

struct string_view;

/** Owned string wrapper.
 * This removes dependencies on std::string (and therefore STL) and allows the
 * API to return allocated strings without incurring additional allocations when
 * wrapping in an std::string.
 */
struct string {
    /** Default constructor. */
    explicit string()
        : str_(nullptr)
        , const_str_("")
        , length_(0) { }

    /** Construct from owned char pointer. */
    explicit string(char *str)
        : str_(str)
        , const_str_(str ? str : "")
        , length_(str ? ecs_os_strlen(str) : 0) { }

    /** Destructor. Frees owned string if set. */
    ~string() {
        // If flecs is included in a binary but is not used, it is possible that
        // the OS API is not initialized. Calling ecs_os_free in that case could
        // crash the application during exit. However, if a string has been set
        // flecs has been used, and OS API should have been initialized.
        if (str_) {
            ecs_os_free(str_);
        }
    }

    /** Move constructor. */
    string(string&& str) noexcept {
        ecs_os_free(str_);
        str_ = str.str_;
        const_str_ = str.const_str_;
        length_ = str.length_;
        str.str_ = nullptr;
    }

    /** Implicit conversion to const char*. */
    operator const char*() const {
        return const_str_;
    }

    /** Move assignment operator. */
    string& operator=(string&& str) noexcept {
        ecs_os_free(str_);
        str_ = str.str_;
        const_str_ = str.const_str_;
        length_ = str.length_;
        str.str_ = nullptr;
        return *this;
    }

    /** Ban implicit copies/allocations. */
    string& operator=(const string& str) = delete;
    /** Ban implicit copies/allocations. */
    string(const string& str) = delete;

    /** Equality operator. */
    bool operator==(const flecs::string& str) const {
        if (str.const_str_ == const_str_) {
            return true;
        }

        if (!const_str_ || !str.const_str_) {
            return false;
        }

        if (str.length_ != length_) {
            return false;
        }

        return ecs_os_strcmp(str, const_str_) == 0;
    }

    /** Inequality operator. */
    bool operator!=(const flecs::string& str) const {
        return !(*this == str);
    }

    /** Equality operator for C string. */
    bool operator==(const char *str) const {
        if (const_str_ == str) {
            return true;
        }

        if (!const_str_ || !str) {
            return false;
        }

        return ecs_os_strcmp(str, const_str_) == 0;
    }

    /** Inequality operator for C string. */
    bool operator!=(const char *str) const {
        return !(*this == str);
    }

    /** Return C string. */
    const char* c_str() const {
        return const_str_;
    }

    /** Return string length. */
    std::size_t length() const {
        return static_cast<std::size_t>(length_);
    }

    /** Return length of a string literal at compile time. */
    template <size_t N>
    static constexpr size_t length( char const (&)[N] ) {
        return N - 1;
    }

    /** Return string size (same as length). */
    std::size_t size() const {
        return length();
    }

    /** Clear the string, freeing owned memory. */
    void clear() {
        ecs_os_free(str_);
        str_ = nullptr;
        const_str_ = nullptr;
    }

    /** Check if string contains a substring. */
    bool contains(const char *substr) {
        if (const_str_) {
            return strstr(const_str_, substr) != nullptr;
        } else {
            return false;
        }
    }

protected:
    /** Construct from non-owned C string.
     * Must be constructed through string_view. This allows for using the string
     * class for both owned and non-owned strings, which can reduce allocations
     * when code conditionally should store a literal or an owned string.
     * Making this constructor private forces the code to explicitly create a
     * string_view which emphasizes that the string won't be freed by the class.
     */
    string(const char *str)
        : str_(nullptr)
        , const_str_(str ? str : "")
        , length_(str ? ecs_os_strlen(str) : 0) { }

    char *str_ = nullptr;
    const char *const_str_;
    ecs_size_t length_;
};

/** Non-owning string view.
 * For consistency, the API returns a string_view where it could have returned
 * a const char*, so an application won't have to think about whether to call
 * c_str() or not. The string_view is a thin wrapper around a string that forces
 * the API to indicate explicitly when a string is owned or not.
 */
struct string_view : string {
    /** Construct from C string (non-owning). */
    explicit string_view(const char *str)
        : string(str) { }
};

}
