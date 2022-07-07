#pragma once

#include "builder.hpp"

namespace flecs 
{

struct filter_base {
    filter_base()
        : m_world(nullptr)
        , m_filter({})
        , m_filter_ptr(nullptr) { }

    filter_base(world_t *world, const ecs_filter_t *filter)
        : m_world(world)
        , m_filter({})
        , m_filter_ptr(filter) { }

    filter_base(world_t *world, ecs_filter_t *filter)
        : m_world(world)
        , m_filter_ptr(&m_filter) {
            ecs_filter_move(&m_filter, filter);
        }

    filter_base(world_t *world, ecs_filter_desc_t *desc) 
        : m_world(world)
    {
        desc->storage = &m_filter;

        if (ecs_filter_init(world, desc) == NULL) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }

        m_filter_ptr = &m_filter;
    }

    filter_base(const filter_base& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_copy(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(const filter_base& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_copy(&m_filter, &obj.m_filter);
        return *this; 
    }

    filter_base(filter_base&& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_move(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(filter_base&& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_move(&m_filter, &obj.m_filter);
        return *this; 
    }

    operator const flecs::filter_t*() const {
        return m_filter_ptr;
    }

    /** Free the filter.
     */
    ~filter_base() {
        if ((&m_filter == m_filter_ptr) && m_filter_ptr) {
            ecs_filter_fini(&m_filter);
        }
    }

    template <typename Func>
    void each_term(const Func& func) {
        for (int i = 0; i < m_filter_ptr->term_count; i ++) {
            flecs::term t(m_world, m_filter_ptr->terms[i]);
            func(t);
        }
    }

    flecs::term term(int32_t index) {
        return flecs::term(m_world, m_filter_ptr->terms[index]);
    }

    int32_t term_count() {
        return m_filter_ptr->term_count;
    }

    flecs::string str() {
        char *result = ecs_filter_str(m_world, m_filter_ptr);
        return flecs::string(result);
    }

    operator filter<>() const;

protected:
    world_t *m_world;
    filter_t m_filter = ECS_FILTER_INIT;
    const filter_t *m_filter_ptr;
};

template<typename ... Components>
struct filter : filter_base, iterable<Components...> {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    using filter_base::filter_base;

    filter() : filter_base() { } // necessary not not confuse msvc

    filter(const filter& obj) : filter_base(obj) { }

    filter& operator=(const filter& obj) {
        filter_base::operator=(obj);
        return *this;
    }

    filter(filter&& obj) : filter_base(FLECS_MOV(obj)) { }

    filter& operator=(filter&& obj) {
        filter_base(FLECS_MOV(obj));
        return *this;
    }

private:
    ecs_iter_t get_iter() const override {
        return ecs_filter_iter(m_world, m_filter_ptr);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_filter_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_filter_next_instanced;
    }
};

// World mixin implementation
template <typename... Comps, typename... Args>
inline flecs::filter<Comps...> world::filter(Args &&... args) const {
    return flecs::filter_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::filter_builder<Comps...> world::filter_builder(Args &&... args) const {
    return flecs::filter_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// world::each
namespace _ {

// Each with entity parameter
template<typename Func, typename ... Args>
struct filter_invoker_w_ent;

template<typename Func, typename E, typename ... Args>
struct filter_invoker_w_ent<Func, arg_list<E, Args ...> >
{
    filter_invoker_w_ent(const flecs::world& world, Func&& func) {
        auto f = world.filter<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Each without entity parameter
template<typename Func, typename ... Args>
struct filter_invoker_no_ent;

template<typename Func, typename ... Args>
struct filter_invoker_no_ent<Func, arg_list<Args ...> >
{
    filter_invoker_no_ent(const flecs::world& world, Func&& func) {
        auto f = world.filter<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Switch between function with & without entity parameter
template<typename Func, typename T = int>
struct filter_invoker;

template <typename Func>
struct filter_invoker<Func, if_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    filter_invoker(const flecs::world& world, Func&& func) {
        filter_invoker_w_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

template <typename Func>
struct filter_invoker<Func, if_not_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    filter_invoker(const flecs::world& world, Func&& func) {
        filter_invoker_no_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

}

template <typename Func>
inline void world::each(Func&& func) const {
    _::filter_invoker<Func> f_invoker(*this, FLECS_MOV(func));
}

template <typename T, typename Func>
inline void world::each(Func&& func) const {
    ecs_term_t t = {};
    t.id = _::cpp_type<T>::id();
    ecs_iter_t it = ecs_term_iter(m_world, &t);

    while (ecs_term_next(&it)) {
        _::each_invoker<Func, T>(func).invoke(&it);
    }
}

template <typename Func>
inline void world::each(flecs::id_t term_id, Func&& func) const {
    ecs_term_t t = {};
    t.id = term_id;
    ecs_iter_t it = ecs_term_iter(m_world, &t);

    while (ecs_term_next(&it)) {
        _::each_invoker<Func>(func).invoke(&it);
    }
}

// filter_base implementation
inline filter_base::operator flecs::filter<> () const {
    flecs::filter<> f;
    ecs_filter_copy(&f.m_filter, &this->m_filter);
    f.m_filter_ptr = &f.m_filter;
    f.m_world = this->m_world;
    return f;
}

}
