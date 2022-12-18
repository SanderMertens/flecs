/**
 * @file addons/cpp/utils/array.hpp
 * @brief Array class.
 * 
 * Array class. Simple std::array like utility that is mostly there to aid
 * template code where template expansion would lead to an array with size 0.
 */

namespace flecs {

template <typename T>
struct array_iterator
{
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
struct array final { };

template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size != 0> > final {
    array() {};

    array(const T (&elems)[Size]) {
        int i = 0;
        for (auto it = this->begin(); it != this->end(); ++ it) {
            *it = elems[i ++];
        }
    }

    T& operator[](int index) {
        return m_array[index];
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

    template <typename Func>
    void each(const Func& func) {
        for (auto& elem : *this) {
            func(elem);
        }
    }

private:
    T m_array[Size];
};

template<typename T, size_t Size>
array<T, Size> to_array(const T (&elems)[Size]) {
    return array<T, Size>(elems);
}

// Specialized class for zero-sized array
template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size == 0>> final {
    array() {};
    array(const T* (&elems)) { (void)elems; }
    T operator[](size_t index) { ecs_os_abort(); (void)index; return T(); }
    array_iterator<T> begin() { return array_iterator<T>(nullptr, 0); }
    array_iterator<T> end() { return array_iterator<T>(nullptr, 0); }

    size_t size() {
        return 0;
    }

    T* ptr() {
        return NULL;
    }
};

}
