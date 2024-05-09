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
        : IBase(&desc_)
        , desc_{}
        , world_(world)
        , instanced_(false)
    {
        ecs_entity_desc_t entity_desc = {};
        entity_desc.name = name;
        entity_desc.sep = "::";
        entity_desc.root_sep = "::";
        desc_.entity = ecs_entity_init(world_, &entity_desc);
    }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    T iter(Func&& func) {
        using Delegate = typename _::iter_delegate<
            typename std::decay<Func>::type, Components...>;
        return build<Delegate>(FLECS_FWD(func));
    }

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    T each(Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, Components...>;
        instanced_ = true;
        return build<Delegate>(FLECS_FWD(func));
    }

protected:
    flecs::world_t* world_v() override { return world_; }
    TDesc desc_;
    flecs::world_t *world_;
    bool instanced_;

private:
    template <typename Delegate, typename Func>
    T build(Func&& func) {
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        desc_.callback = Delegate::run;
        desc_.callback_ctx = ctx;
        desc_.callback_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Delegate>);
        
        return T(world_, &desc_, instanced_);
    }
};

#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs
