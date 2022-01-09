#pragma once

namespace flecs {

struct snapshot final {
    explicit snapshot(const world& world)
        : m_world( world )
        , m_snapshot( nullptr ) { }

    snapshot(const snapshot& obj) 
        : m_world( obj.m_world )
    { 
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot);
        m_snapshot = ecs_snapshot_take_w_iter(&it);
    }

    snapshot(snapshot&& obj) 
        : m_world(obj.m_world)
        , m_snapshot(obj.m_snapshot)
    {
        obj.m_snapshot = nullptr;
    }

    snapshot& operator=(const snapshot& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot);
        m_snapshot = ecs_snapshot_take_w_iter(&it);        
        return *this;
    }

    snapshot& operator=(snapshot&& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        m_snapshot = obj.m_snapshot;
        obj.m_snapshot = nullptr;
        return *this;
    }

    void take() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        m_snapshot = ecs_snapshot_take(m_world.c_ptr());
    }

    template <typename F>
    void take(const F& f) {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        ecs_iter_t it = ecs_filter_iter(m_world, f.c_ptr());

        m_snapshot = ecs_snapshot_take_w_iter(&it);
    }    

    void restore() {
        if (m_snapshot) {
            ecs_snapshot_restore(m_world.c_ptr(), m_snapshot);
            m_snapshot = nullptr;
        }
    }

    ~snapshot() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }
    }

    snapshot_t* c_ptr() const {
        return m_snapshot;
    }

private:
    const world& m_world;
    snapshot_t *m_snapshot;
};

// Snapshot mixin implementation
template <typename... Args>
inline flecs::snapshot world::snapshot(Args &&... args) const {
    return flecs::snapshot(*this, FLECS_FWD(args)...);
}

}
