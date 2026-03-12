/**
 * @file addons/cpp/utils/array.hpp
 * @brief Array class.
 *
 * Array class. Simple std::array-like utility that is mostly there to aid
 * template code where template expansion would lead to an array with size 0.
 */

namespace flecs {

/** Iterator for flecs::array. */
template <typename T>
struct array_iterator
{
    /** Construct an iterator from a pointer and index. */
    explicit array_iterator(T* value, int index) {
        value_ = value;
        index_ = index;
    }

    /** Inequality comparison operator. */
    bool operator!=(array_iterator const& other) const
    {
        return index_ != other.index_;
    }

    /** Dereference operator. */
    T & operator*() const
    {
        return value_[index_];
    }

    /** Pre-increment operator. */
    array_iterator& operator++()
    {
        ++index_;
        return *this;
    }

private:
    T* value_;
    int index_;
};

/** Array class (primary template, disabled). */
template <typename T, size_t Size, class Enable = void>
struct array final { };

/** Array class specialization for non-zero sizes. */
template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size != 0> > final {
    /** Default constructor. */
    array() {};

    /** Construct from a C array. */
    array(const T (&elems)[Size]) {
        int i = 0;
        for (auto it = this->begin(); it != this->end(); ++ it) {
            *it = elems[i ++];
        }
    }

    /** Element access by int index. */
    T& operator[](int index) {
        return array_[index];
    }

    /** Element access by size_t index. */
    T& operator[](size_t index) {
        return array_[index];
    }

    /** Return an iterator to the beginning. */
    array_iterator<T> begin() {
        return array_iterator<T>(array_, 0);
    }

    /** Return an iterator to the end. */
    array_iterator<T> end() {
        return array_iterator<T>(array_, Size);
    }

    /** Return the number of elements. */
    size_t size() {
        return Size;
    }

    /** Return a pointer to the underlying data. */
    T* ptr() {
        return array_;
    }

    /** Invoke a function for each element. */
    template <typename Func>
    void each(const Func& func) {
        for (auto& elem : *this) {
            func(elem);
        }
    }

private:
    T array_[Size];
};

/** Create a flecs::array from a C array. */
template<typename T, size_t Size>
array<T, Size> to_array(const T (&elems)[Size]) {
    return array<T, Size>(elems);
}

/** Array class specialization for zero-sized arrays. */
template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size == 0>> final {
    /** Default constructor. */
    array() {};
    /** Construct from a pointer (no-op). */
    array(const T* (&elems)) { (void)elems; }
    /** Element access (aborts, array is empty). */
    T operator[](size_t index) { ecs_os_abort(); (void)index; return T(); }
    /** Return an iterator to the beginning (empty range). */
    array_iterator<T> begin() { return array_iterator<T>(nullptr, 0); }
    /** Return an iterator to the end (empty range). */
    array_iterator<T> end() { return array_iterator<T>(nullptr, 0); }

    /** Return the number of elements (always 0). */
    size_t size() {
        return 0;
    }

    /** Return a null pointer (no data). */
    T* ptr() {
        return NULL;
    }
};

}
