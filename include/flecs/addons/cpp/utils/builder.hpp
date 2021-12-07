#pragma once

namespace flecs {
namespace _ {

// Macros for template types so we don't go cross-eyed
#define FLECS_TBUILDER template<typename ... Components> class
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<FLECS_TBUILDER T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct builder : IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    builder(flecs::world_t *world)
        : IBase(&m_desc)
        , m_desc{}
        , m_world(world) { }

    builder(const builder& f) 
        : IBase(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    builder(builder&& f) 
        : builder<T, TDesc, Base, IBuilder, Components...>(f) { }

    operator TDesc*() {
        return &m_desc;
    }

    T<Components ...> build() {
        return T<Components...>(m_world, *static_cast<Base*>(this));
    }

protected:
    flecs::world_t* world_v() override { return m_world; }
    TDesc m_desc;
    flecs::world_t *m_world;
};

#undef FLECS_TBUILDER
#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs
