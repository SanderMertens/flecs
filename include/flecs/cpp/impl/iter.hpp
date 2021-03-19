
namespace flecs 
{

inline flecs::entity iter::system() const {
    return flecs::entity(m_iter->world, m_iter->system);
}

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < (size_t)m_iter->count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    if (!this->world().is_readonly()) {
        return flecs::entity(m_iter->entities[row])
            .mut(this->world());
    } else {
        return flecs::entity(this->world().c_ptr(), m_iter->entities[row]);
    }
}

/* Obtain column source (0 if self) */
inline flecs::entity iter::column_source(int32_t col) const {
    return flecs::entity(m_iter->world, ecs_column_source(m_iter, col));
}

/* Obtain component/tag entity of column */
inline flecs::entity iter::column_entity(int32_t col) const {
    return flecs::entity(m_iter->world, ecs_column_entity(m_iter, col));
}

/* Obtain type of column */
inline type iter::column_type(int32_t col) const {
    return flecs::type(m_iter->world, ecs_column_type(m_iter, col));
}

/* Obtain type of table being iterated over */
inline type iter::table_type() const {
    return flecs::type(m_iter->world, ecs_iter_type(m_iter));
}

} // namespace flecs
