/**
 * @file addons/cpp/utils/stringstream.hpp
 * @brief Wrapper around ecs_strbuf_t that provides a simple stringstream like API.
 */

namespace flecs {

struct stringstream {
    explicit stringstream() 
        : buf_({}) { }

    ~stringstream() {
        ecs_strbuf_reset(&buf_);
    }

    stringstream(stringstream&& str) noexcept {
        ecs_strbuf_reset(&buf_);
        buf_ = str.buf_;
        str.buf_ = {};
    }

    stringstream& operator=(stringstream&& str) noexcept {
        ecs_strbuf_reset(&buf_);
        buf_ = str.buf_;
        str.buf_ = {};
        return *this;
    }

    // Ban implicit copies/allocations
    stringstream& operator=(const stringstream& str) = delete;
    stringstream(const stringstream& str) = delete;    

    stringstream& operator<<(const char* str) {
        ecs_strbuf_appendstr(&buf_, str);
        return *this;
    }

    flecs::string str() {
        return flecs::string(ecs_strbuf_get(&buf_));
    }

private:
    ecs_strbuf_t buf_;
};

}
