#pragma once

#include "builder.hpp"

namespace flecs {

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

struct rule_base {
    rule_base()
        : m_world(nullptr)
        , m_rule(nullptr) { }    
    
    rule_base(world_t *world, rule_t *rule = nullptr)
        : m_world(world)
        , m_rule(rule) { }

    rule_base(world_t *world, ecs_filter_desc_t *desc) 
        : m_world(world)
    {
        m_rule = ecs_rule_init(world, desc);

        if (!m_rule) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }
    }

    operator rule_t*() const {
        return m_rule;
    }

    /** Free the rule.
     */
    void destruct() {
        ecs_rule_fini(m_rule);
        m_world = nullptr;
        m_rule = nullptr;
    }

    flecs::string str() {
        const ecs_filter_t *f = ecs_rule_get_filter(m_rule);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }

    operator rule<>() const;

protected:
    world_t *m_world;
    rule_t *m_rule;
};

template<typename ... Components>
struct rule final : rule_base, iterable<Components...> {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

    ecs_iter_t get_iter() const override {
        return ecs_rule_iter(m_world, m_rule);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_rule_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_rule_next_instanced;
    }

public:
    using rule_base::rule_base;

    int32_t find_var(const char *name) {
        return ecs_rule_find_var(m_rule, name);
    }
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::rule<Comps...> world::rule(Args &&... args) const {
    return flecs::rule_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::rule_builder<Comps...> world::rule_builder(Args &&... args) const {
    return flecs::rule_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// rule_base implementation
inline rule_base::operator rule<>() const {
    return flecs::rule<>(m_world, m_rule);
}

} // namespace flecs
