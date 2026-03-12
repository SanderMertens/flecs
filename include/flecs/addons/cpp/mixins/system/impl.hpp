/**
 * @file addons/cpp/mixins/system/impl.hpp
 * @brief System module implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

/** Fluent interface for running a system.
 *
 * @ingroup cpp_addons_systems
 */
struct system_runner_fluent {
    /** Construct a system runner. */
    system_runner_fluent(
        world_t *world,
        entity_t id,
        int32_t stage_current,
        int32_t stage_count,
        ecs_ftime_t delta_time,
        void *param)
        : stage_(world)
        , id_(id)
        , delta_time_(delta_time)
        , param_(param)
        , stage_current_(stage_current)
        , stage_count_(stage_count) { }

    /** Set the offset for the system runner. */
    system_runner_fluent& offset(int32_t offset) {
        offset_ = offset;
        return *this;
    }

    /** Set the limit for the system runner. */
    system_runner_fluent& limit(int32_t limit) {
        limit_ = limit;
        return *this;
    }

    /** Set the stage for the system runner. */
    system_runner_fluent& stage(flecs::world& stage) {
        stage_ = stage.c_ptr();
        return *this;
    }

    /** Destructor. Runs the system on destruction. */
    ~system_runner_fluent() {
        if (stage_count_) {
            ecs_run_worker(
                stage_, id_, stage_current_, stage_count_, delta_time_,
                param_);            
        } else {
            ecs_run(stage_, id_, delta_time_, param_);
        }
    }

private:
    world_t *stage_;
    entity_t id_;
    ecs_ftime_t delta_time_;
    void *param_;
    int32_t offset_;
    int32_t limit_;
    int32_t stage_current_;
    int32_t stage_count_;
};

/** System.
 *
 * @ingroup cpp_addons_systems
 */
struct system final : entity
{
    using entity::entity;

    /** Default constructor. */
    explicit system() {
        id_ = 0;
        world_ = nullptr;
    }

    /** Construct from a world and system descriptor. */
    explicit system(flecs::world_t *world, ecs_system_desc_t *desc) {
        world_ = world;
        id_ = ecs_system_init(world, desc);
    }

    /** Set the system context. */
    void ctx(void *ctx) {
        ecs_system_desc_t desc = {};
        desc.entity = id_;
        desc.ctx = ctx;
        ecs_system_init(world_, &desc);
    }

    /** Get the system context. */
    void* ctx() const {
        return ecs_system_get(world_, id_)->ctx;
    }

    /** Get the query for this system. */
    flecs::query<> query() const {
        return flecs::query<>(ecs_system_get(world_, id_)->query);
    }

    /** Set the query group */
    system& set_group(uint64_t group_id) {
        ecs_system_set_group(world_, id_, group_id);
        return *this;
    }

    /** Set the query group */
    template <typename Group>
    system& set_group() {
        ecs_system_set_group(world_, id_, _::type<Group>().id(world_));
        return *this;
    }

    /** Run the system. */
    system_runner_fluent run(ecs_ftime_t delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(world_, id_, 0, 0, delta_time, param);
    }

    /** Run the system on a specific worker stage. */
    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        ecs_ftime_t delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            world_, id_, stage_current, stage_count, delta_time, param);
    }

#   ifdef FLECS_TIMER
#   include "../timer/system_mixin.inl"
#   endif

};

/** Mixin implementation. */
inline system world::system(flecs::entity e) const {
    return flecs::system(world_, e);
}

template <typename... Comps, typename... Args>
inline system_builder<Comps...> world::system(Args &&... args) const {
    return flecs::system_builder<Comps...>(world_, FLECS_FWD(args)...);
}

namespace _ {

template <typename ArgList>
struct system_each_normalize_args;

template <typename ... Args>
struct system_each_normalize_args<arg_list<Args...>> {
    using type = arg_list<remove_reference_t<Args>...>;
};

template <typename ArgList, typename = int>
struct system_each_callback_args {
    using type = typename system_each_normalize_args<ArgList>::type;
};

template <typename First, typename ... Args>
struct system_each_callback_args<arg_list<First, Args...>,
    if_t<is_same<decay_t<First>, flecs::entity>::value>> {
    using type = typename system_each_normalize_args<arg_list<Args...>>::type;
};

template <typename First, typename Second, typename ... Args>
struct system_each_callback_args<arg_list<First, Second, Args...>,
    if_t<is_same<decay_t<First>, flecs::iter>::value>> {
    using type = typename system_each_normalize_args<arg_list<Args...>>::type;
};

inline void system_init(flecs::world& world) {
    world.component<TickSource>("flecs::system::TickSource");
}

} // namespace _

template <typename ... Components>
template <typename Func>
inline system system_builder<Components...>::each(Func&& func) {
    if constexpr (sizeof...(Components) == 0) {
        using CallbackComponents =
            typename _::system_each_callback_args<arg_list_t<Func>>::type;
        return this->each_callback(CallbackComponents{}, FLECS_FWD(func));
    } else {
        // Faster version of each() that iterates the query on the C++ side.
        return this->run_each(FLECS_FWD(func));
    }
}

template <typename ... Components>
template <typename ... CallbackComponents, typename Func>
inline system system_builder<Components...>::each_callback(
    _::arg_list<CallbackComponents...>,
    Func&& func)
{
    this->template prepend_each_callback_signature<CallbackComponents...>();

    using Delegate = typename _::each_delegate<
        typename std::decay<Func>::type, CallbackComponents...>;

    auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
    this->desc_.run = Delegate::run_each;
    this->desc_.run_ctx = ctx;
    this->desc_.run_ctx_free = _::free_obj<Delegate>;
    return system(this->world_, &this->desc_);
}

template <typename ... Components>
template <typename ... CallbackComponents>
inline void system_builder<Components...>::prepend_each_callback_signature() {
    if constexpr (sizeof...(Components) == 0 && sizeof...(CallbackComponents) != 0) {
        constexpr int32_t callback_term_count =
            static_cast<int32_t>(sizeof...(CallbackComponents));

        ecs_assert(this->term_index_ + callback_term_count <= FLECS_TERM_COUNT_MAX,
            ECS_INVALID_PARAMETER, "maximum number of terms exceeded");

        const int32_t existing_term_count = this->term_index_;
        this->set_term(nullptr);

        for (int32_t i = existing_term_count - 1; i >= 0; i --) {
            this->desc_.query.terms[i + callback_term_count] =
                this->desc_.query.terms[i];
        }

        for (int32_t i = 0; i < callback_term_count; i ++) {
            this->desc_.query.terms[i] = ecs_term_t{};
        }

        this->term_index_ = 0;
        _::sig<CallbackComponents...>(this->world_).populate(this);
        this->term_index_ = existing_term_count + callback_term_count;
    }
}

} // namespace flecs
