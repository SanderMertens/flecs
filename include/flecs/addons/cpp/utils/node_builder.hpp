#pragma once

namespace flecs 
{

namespace _ 
{

// Base class just used to initialize m_desc to 0
template <typename TDesc>
struct node_desc_initializer {
    node_desc_initializer(TDesc *desc) { *desc = {}; }
};

// Macros for template types so we don't go cross-eyed
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<typename T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct node_builder
    : node_desc_initializer<TDesc>, IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    explicit node_builder(flecs::world_t* world, const char *name = nullptr)
        : node_desc_initializer<TDesc>(&m_desc)
        , IBase(world, &m_desc)
        , m_world(world) 
    {
        m_desc.entity.name = name;
        m_desc.entity.sep = "::";
    }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    T iter(Func&& func) {
        using Invoker = typename _::iter_invoker<
            typename std::decay<Func>::type, Components...>;
        return build<Invoker>(std::forward<Func>(func));
    }

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    T each(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, Components...>;
        return build<Invoker>(std::forward<Func>(func));
    }

protected:
    flecs::world_t* world_v() override { return m_world; }
    TDesc m_desc;
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    T build(Func&& func) {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        m_desc.callback = Invoker::run;
        m_desc.binding_ctx = ctx;
        m_desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);
        
        return T(m_world, &m_desc);
    }
};

#undef FLECS_IBUILDER

} // namespace _

} // namespace flecs
