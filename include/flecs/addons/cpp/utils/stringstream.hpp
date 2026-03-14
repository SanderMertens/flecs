/**
 * @file addons/cpp/utils/stringstream.hpp
 * @brief Wrapper around ecs_strbuf_t that provides a simple stringstream-like API.
 */

namespace flecs {

/** Simple stringstream wrapper around ecs_strbuf_t. */
struct stringstream {
    /** Default constructor. */
    explicit stringstream()
        : buf_({}) { }

    /** Destructor. Reset the internal buffer. */
    ~stringstream() {
        ecs_strbuf_reset(&buf_);
    }

    /** Move constructor. */
    stringstream(stringstream&& str) noexcept {
        ecs_strbuf_reset(&buf_);
        buf_ = str.buf_;
        str.buf_ = {};
    }

    /** Move assignment operator. */
    stringstream& operator=(stringstream&& str) noexcept {
        ecs_strbuf_reset(&buf_);
        buf_ = str.buf_;
        str.buf_ = {};
        return *this;
    }

    /** Ban implicit copies/allocations. */
    stringstream& operator=(const stringstream& str) = delete;
    /** Ban implicit copies/allocations. */
    stringstream(const stringstream& str) = delete;

    /** Append a C string to the stream. */
    stringstream& operator<<(const char* str) {
        ecs_strbuf_appendstr(&buf_, str);
        return *this;
    }

    /** Get the accumulated string as an owned flecs::string. */
    flecs::string str() {
        return flecs::string(ecs_strbuf_get(&buf_));
    }

private:
    ecs_strbuf_t buf_;
};

}
