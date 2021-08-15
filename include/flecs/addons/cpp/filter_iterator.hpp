
namespace flecs
{

////////////////////////////////////////////////////////////////////////////////
//// Utility for iterating over tables that match a filter
////////////////////////////////////////////////////////////////////////////////

class filter_iterator
{
public:
    filter_iterator(ecs_iter_next_action_t action)
        : m_world(nullptr)
        , m_has_next(false)
        , m_iter{ } 
        , m_action(action) { }

    filter_iterator(const world& world, ecs_iter_next_action_t action)
        : m_world( world.c_ptr() )
        , m_iter( ecs_filter_iter(m_world, NULL) ) 
        , m_action(action)
    { 
        m_has_next = m_action(&m_iter);
    }

    filter_iterator(const world& world, const snapshot& snapshot, ecs_iter_next_action_t action) 
        : m_world( world.c_ptr() )
        , m_iter( ecs_snapshot_iter(snapshot.c_ptr(), NULL) )
        , m_action(action)
    {
        m_has_next = m_action(&m_iter);
    }

    bool operator!=(filter_iterator const& other) const {
        return m_has_next != other.m_has_next;
    }

    flecs::iter const operator*() const {
        return flecs::iter(&m_iter);
    }

    filter_iterator& operator++() {
        m_has_next = m_action(&m_iter);
        return *this;
    }

private:
    world_t *m_world;
    bool m_has_next;
    ecs_iter_t m_iter;
    ecs_iter_next_action_t m_action;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility for creating a world-based filter iterator
////////////////////////////////////////////////////////////////////////////////

class world_filter {
public:
    world_filter(const world& world) 
        : m_world( world ) { }

    inline filter_iterator begin() const {
        return filter_iterator(m_world, ecs_filter_next);
    }

    inline filter_iterator end() const {
        return filter_iterator(ecs_filter_next);
    }

private:
    const world& m_world;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility for creating a snapshot-based filter iterator
////////////////////////////////////////////////////////////////////////////////

class snapshot_filter {
public:
    snapshot_filter(const world& world, const snapshot& snapshot) 
        : m_world( world )
        , m_snapshot( snapshot ) { }

    inline filter_iterator begin() const {
        return filter_iterator(m_world, m_snapshot, ecs_snapshot_next);
    }

    inline filter_iterator end() const {
        return filter_iterator(ecs_snapshot_next);
    }

private:
    const world& m_world;
    const snapshot& m_snapshot;
};

////////////////////////////////////////////////////////////////////////////////
//// Filter fwd declared functions
////////////////////////////////////////////////////////////////////////////////

inline filter_iterator snapshot::begin() {
    return filter_iterator(m_world, *this, ecs_snapshot_next);
}

inline filter_iterator snapshot::end() {
    return filter_iterator(ecs_snapshot_next);
}

}
