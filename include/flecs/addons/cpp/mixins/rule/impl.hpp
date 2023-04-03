/**
 * @file addons/cpp/mixins/rule/impl.hpp
 * @brief Rule implementation.
 */

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
        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }
    }

    bool is_valid() const {
        return m_rule != nullptr;
    }

    operator rule_t*() const {
        return m_rule;
    }

    flecs::entity entity() {
        return flecs::entity(m_world, ecs_get_entity(m_rule));
    }

    /** Free the rule. */
    void destruct() {
        if (m_rule) {
            ecs_rule_fini(m_rule);
            m_world = nullptr;
            m_rule = nullptr;
        }
    }

    /** Move the rule. */
    void move(flecs::rule_base&& obj) {
        this->destruct();
        this->m_world = obj.m_world;
        this->m_rule = obj.m_rule;
        obj.m_world = nullptr;
        obj.m_rule = nullptr;
    }

    flecs::filter_base filter() const {
        return filter_base(m_world, ecs_rule_get_filter(m_rule));
    }

    /** Converts this rule to a string expression
     * @see ecs_filter_str
     */
    flecs::string str() const {
        const ecs_filter_t *f = ecs_rule_get_filter(m_rule);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }


    /** Converts this rule to a string that can be used to aid debugging
     * the behavior of the rule.
     * @see ecs_rule_str
     */
    flecs::string rule_str() const {
        char *result = ecs_rule_str(m_rule);
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

    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_world;
        }
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
