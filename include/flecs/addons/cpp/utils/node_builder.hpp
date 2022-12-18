/**
 * @file addons/cpp/utils/node_builder.hpp
 * @brief Base builder class for node objects, like systems, observers.
 */

#pragma once

namespace flecs {
namespace _ {

// Macros for template types so we don't go cross-eyed
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<typename T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct node_builder : IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    explicit node_builder(flecs::world_t* world, const char *name = nullptr)
        : IBase(&m_desc)
        , m_desc{}
        , m_world(world)
        , m_instanced(false)
    {
        ecs_entity_desc_t entity_desc = {};
        entity_desc.name = name;
        entity_desc.sep = "::";
        m_desc.entity = ecs_entity_init(m_world, &entity_desc);
    }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    T iter(Func&& func) {
        using Invoker = typename _::iter_invoker<
            typename std::decay<Func>::type, Components...>;
        return build<Invoker>(FLECS_FWD(func));
    }

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    T each(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, Components...>;
        m_instanced = true;
        return build<Invoker>(FLECS_FWD(func));
    }

protected:
    flecs::world_t* world_v() override { return m_world; }
    TDesc m_desc;
    flecs::world_t *m_world;
    bool m_instanced;

private:
    template <typename Invoker, typename Func>
    T build(Func&& func) {
        auto ctx = FLECS_NEW(Invoker)(FLECS_FWD(func));
        m_desc.callback = Invoker::run;
        m_desc.binding_ctx = ctx;
        m_desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);
        
        return T(m_world, &m_desc, m_instanced);
    }
};

#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs
