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
    {
        ecs_entity_desc_t entity_desc = {};
        entity_desc.name = name;
        entity_desc.sep = "::";
        entity_desc.root_sep = "::";
        desc_.entity = ecs_entity_init(world_, &entity_desc);
    }

    template <typename Func>
    T run(Func&& func) {
        using Delegate = typename _::run_delegate<
            typename std::decay<Func>::type>;

        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        desc_.run = Delegate::run;
        desc_.run_ctx = ctx;
        desc_.run_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Delegate>);
        return T(world_, &desc_);
    }

    template <typename Func, typename EachFunc>
    T run(Func&& func, EachFunc&& each_func) {
        using Delegate = typename _::run_delegate<
            typename std::decay<Func>::type>;

        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        desc_.run = Delegate::run;
        desc_.run_ctx = ctx;
        desc_.run_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Delegate>);
        return each(FLECS_FWD(each_func));
    }

    template <typename Func>
    T each(Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        desc_.callback = Delegate::run;
        desc_.callback_ctx = ctx;
        desc_.callback_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Delegate>);
        return T(world_, &desc_);
    }

protected:
    flecs::world_t* world_v() override { return world_; }
    TDesc desc_;
    flecs::world_t *world_;
};

#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs
