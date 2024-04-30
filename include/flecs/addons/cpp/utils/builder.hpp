/**
 * @file addons/cpp/utils/builder.hpp
 * @brief Builder base class.
 * 
 * Generic functionality for builder classes.
 */

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
        : IBase(&desc_)
        , desc_{}
        , world_(world) { }

    builder(const builder& f) 
        : IBase(&desc_, f.term_index_)
    {
        world_ = f.world_;
        desc_ = f.desc_;
    }

    builder(builder&& f)  noexcept
        : builder<T, TDesc, Base, IBuilder, Components...>(f) { }

    operator TDesc*() {
        return &desc_;
    }

    T<Components ...> build() {
        return T<Components...>(world_, *static_cast<Base*>(this));
    }

protected:
    flecs::world_t* world_v() override { return world_; }
    TDesc desc_;
    flecs::world_t *world_;
};

#undef FLECS_TBUILDER
#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs
