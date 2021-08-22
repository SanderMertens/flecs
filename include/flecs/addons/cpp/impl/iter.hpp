
namespace flecs
{

inline flecs::entity iter::system() const {
    return flecs::entity(m_iter->world, m_iter->system);
}

inline flecs::entity iter::event() const {
    return flecs::entity(m_iter->world, m_iter->event);
}

inline flecs::id iter::event_id() const {
    return flecs::id(m_iter->world, m_iter->event_id);
}

 inline flecs::entity iter::self() const {
    return flecs::entity(m_iter->world, m_iter->self);
}

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < static_cast<size_t>(m_iter->count), ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    if (!this->world().is_readonly()) {
        return flecs::entity(m_iter->entities[row])
            .mut(this->world());
    } else {
        return flecs::entity(this->world().c_ptr(), m_iter->entities[row]);
    }
}

template <typename T>
inline column<T>::column(iter &iter, int32_t index) {
    *this = iter.term<T>(index);
}

inline flecs::entity iter::term_source(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_term_source(m_iter, index));
}

inline flecs::entity iter::term_id(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_term_id(m_iter, index));
}

/* Obtain type of iter */
inline flecs::type iter::type() const {
    return flecs::type(m_iter->world, m_iter->type);
}

} // namespace flecs
