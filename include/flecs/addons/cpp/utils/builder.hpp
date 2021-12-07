#pragma once

namespace flecs
{

namespace _
{

// Base class just used to initialize m_desc to 0
template <typename TDesc>
struct builder_desc_initializer {
    builder_desc_initializer(TDesc *desc) { *desc = {}; }
};

// Macros for template types so we don't go cross-eyed
#define FLECS_TBUILDER template<typename ... Components> class
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<FLECS_TBUILDER T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct builder_base
    : builder_desc_initializer<TDesc>, IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    builder_base(flecs::world_t *world)
        : builder_desc_initializer<TDesc>(&m_desc)
        , IBase(world, &m_desc)
        , m_world(world) { }

    builder_base(const builder_base& f) 
        : builder_desc_initializer<TDesc>(&m_desc)
        , IBase(f.m_world, &m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    builder_base(builder_base&& f) 
        : builder_base<T, TDesc, Base, IBuilder, Components...>(f) { }

    operator TDesc*() {
        return &m_desc;
    }

    T<Components ...> build() {
        return T<Components...>(m_world, *static_cast<Base*>(this));
    }

    flecs::world_t* world_v() override { return m_world; }

protected:
    TDesc m_desc;
    flecs::world_t *m_world;
};

template<FLECS_TBUILDER T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct builder : builder_base<T, TDesc, Base, IBuilder, Components...> {
    using builder_base<T, TDesc, Base, IBuilder, Components...>::builder_base;

};

#undef FLECS_TBUILDER
#undef FLECS_IBUILDER

} // namespace _

} // namespace flecs
