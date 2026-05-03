/**
 * @file addons/cpp/utils/map.hpp
 * @brief Minimal C++ wrapper for ecs_map_t iteration.
 */

#pragma once

namespace flecs {
namespace _ {

template <typename V>
inline enable_if_t<is_pointer<V>::value, V>
map_value_cast(ecs_map_val_t v) {
    return reinterpret_cast<V>(static_cast<uintptr_t>(v));
}

template <typename V>
inline enable_if_t<!is_pointer<V>::value, V>
map_value_cast(ecs_map_val_t v) {
    return static_cast<V>(v);
}

} // namespace _

template <typename K, typename V>
struct map_entry {
    K first;
    V second;
};

template <typename K, typename V>
struct map_iterator {
    map_iterator()
        : map_(nullptr)
        , done_(true) { }

    explicit map_iterator(const ecs_map_t *m)
        : map_(m)
    {
        if (m) {
            it_ = ecs_map_iter(m);
            advance();
        } else {
            done_ = true;
        }
    }

    bool operator!=(const map_iterator& other) const {
        return done_ != other.done_;
    }

    const map_entry<K, V>& operator*() const {
        return entry_;
    }

    const map_entry<K, V>* operator->() const {
        return &entry_;
    }

    map_iterator& operator++() {
        advance();
        return *this;
    }

private:
    void advance() {
        if (ecs_map_next(&it_)) {
            entry_.first = static_cast<K>(ecs_map_key(&it_));
            entry_.second = _::map_value_cast<V>(ecs_map_value(&it_));
            done_ = false;
        } else {
            done_ = true;
        }
    }

    const ecs_map_t *map_;
    ecs_map_iter_t it_ = {};
    map_entry<K, V> entry_ = {};
    bool done_ = false;
};

template <typename K, typename V>
struct map {
    explicit map(const ecs_map_t *m) : map_(m) { }

    map_iterator<K, V> begin() const {
        return map_iterator<K, V>(map_);
    }

    map_iterator<K, V> end() const {
        return map_iterator<K, V>();
    }

private:
    const ecs_map_t *map_;
};

}
