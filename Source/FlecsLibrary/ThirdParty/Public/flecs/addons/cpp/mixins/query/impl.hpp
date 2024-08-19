/**
 * @file addons/cpp/mixins/query/impl.hpp
 * @brief Query implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

struct query_base {
    query_base() { }

    query_base(query_t *q)
        : query_(q) { 
            flecs_poly_claim(q);
        }

    query_base(const query_t *q)
        : query_(ECS_CONST_CAST(query_t*, q)) { 
            flecs_poly_claim(q);
        }

    query_base(world_t *world, ecs_query_desc_t *desc) {
        if (desc->entity && desc->terms[0].id == 0) {
            const flecs::Poly *query_poly = ecs_get_pair(
                world, desc->entity, EcsPoly, EcsQuery);
            if (query_poly) {
                query_ = static_cast<flecs::query_t*>(query_poly->poly);
                flecs_poly_claim(query_);
                return;
            }
        }

        query_ = ecs_query_init(world, desc);
    }

    query_base(const query_base& obj) {
        this->query_ = obj.query_;
        flecs_poly_claim(this->query_);
    }

    query_base& operator=(const query_base& obj) {
        this->query_ = obj.query_;
        flecs_poly_claim(this->query_);
        return *this; 
    }

    query_base(query_base&& obj) noexcept {
        this->query_ = obj.query_;
        obj.query_ = nullptr;
    }

    query_base& operator=(query_base&& obj) noexcept {
        this->query_ = obj.query_;
        obj.query_ = nullptr;
        return *this; 
    }

    flecs::entity entity() {
        return flecs::entity(query_->world, query_->entity);
    }

    const flecs::query_t* c_ptr() const {
        return query_;
    }

    operator const flecs::query_t*() const {
        return query_;
    }

    operator bool() const {
        return query_ != nullptr;
    }

    /** Free persistent query.
     * A persistent query is a query that is associated with an entity, such as
     * system queries and named queries. Persistent queries must be deleted with
     * destruct(), or will be deleted automatically at world cleanup. 
     */
    void destruct() {
        ecs_assert(query_->entity != 0, ECS_INVALID_OPERATION, "destruct() "
            "should only be called on queries associated with entities");
        ecs_query_fini(query_);
        query_ = nullptr;
    }

    ~query_base() {
        /* Only free if query is not associated with entity, such as system
         * queries and named queries. Named queries have to be either explicitly
         * deleted with the .destruct() method, or will be deleted when the
         * world is deleted. */
        if (query_ && !query_->entity) {
            if (!flecs_poly_release(query_)) {
                ecs_query_fini(query_);
                query_ = nullptr;
            }
        }
    }

    /** Returns whether the query data changed since the last iteration.
     * This operation must be invoked before obtaining the iterator, as this will
     * reset the changed state. The operation will return true after:
     * - new entities have been matched with
     * - matched entities were deleted
     * - matched components were changed
     * 
     * @return true if entities changed, otherwise false.
     */
    bool changed() const {
        return ecs_query_changed(query_);
    }

    /** Get info for group. 
     * 
     * @param group_id The group id for which to retrieve the info.
     * @return The group info.
     */
    const flecs::query_group_info_t* group_info(uint64_t group_id) const {
        return ecs_query_get_group_info(query_, group_id);
    }

    /** Get context for group. 
     * 
     * @param group_id The group id for which to retrieve the context.
     * @return The group context.
     */
    void* group_ctx(uint64_t group_id) const {
        const flecs::query_group_info_t *gi = group_info(group_id);
        if (gi) {
            return gi->ctx;
        } else {
            return NULL;
        }
    }

    template <typename Func>
    void each_term(const Func& func) {
        for (int i = 0; i < query_->term_count; i ++) {
            flecs::term t(query_->world, query_->terms[i]);
            func(t);
            t.reset(); // prevent freeing resources
        }
    }

    flecs::term term(int32_t index) {
        return flecs::term(query_->world, query_->terms[index]);
    }

    int32_t term_count() {
        return query_->term_count;
    }

    int32_t field_count() {
        return query_->field_count;
    }

    int32_t find_var(const char *name) {
        return ecs_query_find_var(query_, name);
    }

    flecs::string str() {
        char *result = ecs_query_str(query_);
        return flecs::string(result);
    }

    /** Returns a string representing the query plan.
     * This can be used to analyze the behavior & performance of the query.
     * @see ecs_query_plan
     */
    flecs::string plan() const {
        char *result = ecs_query_plan(query_);
        return flecs::string(result);
    }

    operator query<>() const;

protected:
    query_t *query_ = nullptr;
};

template<typename ... Components>
struct query : query_base, iterable<Components...> {
private:
    using Fields = typename _::field_ptrs<Components...>::array;

public:
    using query_base::query_base;

    query() : query_base() { } // necessary not to confuse msvc

    query(const query& obj) : query_base(obj) { }

    query& operator=(const query& obj) {
        query_base::operator=(obj);
        return *this;
    }

    query(query&& obj) noexcept : query_base(FLECS_MOV(obj)) { }

    query& operator=(query&& obj) noexcept {
        query_base::operator=(FLECS_FWD(obj));
        return *this;
    }

private:
    ecs_iter_t get_iter(flecs::world_t *world) const override {
        ecs_assert(query_ != nullptr, ECS_INVALID_PARAMETER, 
            "cannot iterate invalid query");
        if (!world) {
            world = query_->world;
        }
        return ecs_query_iter(world, query_);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_query_next;
    }
};

// World mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(world_, FLECS_FWD(args)...)
        .build();
}

inline flecs::query<> world::query(flecs::entity query_entity) const {
    ecs_query_desc_t desc = {};
    desc.entity = query_entity;
    return flecs::query<>(world_, &desc);
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(world_, FLECS_FWD(args)...);
}

// world::each
namespace _ {

// Each with entity parameter
template<typename Func, typename ... Args>
struct query_delegate_w_ent;

template<typename Func, typename E, typename ... Args>
struct query_delegate_w_ent<Func, arg_list<E, Args ...> >
{
    query_delegate_w_ent(const flecs::world& world, Func&& func) {
        auto f = world.query<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Each without entity parameter
template<typename Func, typename ... Args>
struct query_delegate_no_ent;

template<typename Func, typename ... Args>
struct query_delegate_no_ent<Func, arg_list<Args ...> >
{
    query_delegate_no_ent(const flecs::world& world, Func&& func) {
        auto f = world.query<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Switch between function with & without entity parameter
template<typename Func, typename T = int>
struct query_delegate;

template <typename Func>
struct query_delegate<Func, if_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    query_delegate(const flecs::world& world, Func&& func) {
        query_delegate_w_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

template <typename Func>
struct query_delegate<Func, if_not_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    query_delegate(const flecs::world& world, Func&& func) {
        query_delegate_no_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

}

template <typename Func>
inline void world::each(Func&& func) const {
    _::query_delegate<Func> f_delegate(*this, FLECS_MOV(func));
}

template <typename T, typename Func>
inline void world::each(Func&& func) const {
    ecs_iter_t it = ecs_each_id(world_, _::type<T>::id());

    while (ecs_each_next(&it)) {
        _::each_delegate<Func, T>(func).invoke(&it);
    }
}

template <typename Func>
inline void world::each(flecs::id_t each_id, Func&& func) const {
    ecs_iter_t it = ecs_each_id(world_, each_id);

    while (ecs_each_next(&it)) {
        _::each_delegate<Func>(func).invoke(&it);
    }
}

// query_base implementation
inline query_base::operator flecs::query<> () const {
    return flecs::query<>(query_);
}

}
