// Array class. Simple std::array like utility that is mostly there to aid
// template code where template expansion would lead to an array with size 0.

namespace flecs {

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

}
