/**
 * @file addons/cpp/utils/builder.hpp
 * @brief Builder base class.
 * 
 * Generic functionality for builder classes.
 */

#pragma once

namespace flecs {
namespace _ {

template <typename T, typename TDesc, typename Base,
    template <typename, typename...> class IBuilder, typename... Components>
struct builder : IBuilder<Base, Components...> {
    using IBase = IBuilder<Base, Components...>;

    explicit builder(world_t *world, const char *name = nullptr)
        : IBase(&desc_), desc_{}, world_(world)
    {
        set_name(name);
    }

    builder(const builder& other)
        : IBase(&desc_, other.term_index_), desc_(other.desc_), world_(other.world_) { }

    builder(builder&& other) noexcept : builder(other) { }

    operator TDesc*() {
        return &desc_;
    }

    operator const TDesc*() const {
        return &desc_;
    }

    T build() const {
        return T(world_, &desc_);
    }

    template <typename Func, typename... Each>
    T run(Func&& func, Each&&... each_func) {
        using Delegate = run_delegate<decay_t<Func>>;
        set_callback<Delegate, Delegate::run, true>(desc_, FLECS_FWD(func));
        if constexpr (sizeof...(Each)) {
            return each(FLECS_FWD(each_func)...);
        } else {
            return build();
        }
    }

    template <typename Func>
    T each(Func&& func) {
        set_each_callback<false>(desc_, FLECS_FWD(func), arg_list<Components...>{});
        return build();
    }

    template <typename Func>
    T run_each(Func&& func) {
        set_each_callback<true>(desc_, FLECS_FWD(func), arg_list<Components...>{});
        return build();
    }

protected:
    world_t* world_v() override {
        return world_;
    }

    void set_name(const char *name) {
        if (name) {
            ecs_entity_desc_t desc = {};
            desc.name = name;
            desc.sep = "::";
            desc.root_sep = "::";
            desc_.entity = ecs_entity_init(world_, &desc);
        }
    }

    TDesc desc_;
    world_t *world_;

};

} // namespace _
} // namespace flecs
