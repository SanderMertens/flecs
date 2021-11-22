#pragma once

#include "builder_i.hpp"

namespace flecs 
{

template<typename ... Components>
struct system_builder final
    : system_builder_i<system_builder<Components ...>, Components ...>
{
private:
    using Class = system_builder<Components ...>;

public:
    explicit system_builder(flecs::world_t* world, const char *name = nullptr, const char *expr = nullptr) 
        : system_builder_i<Class, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
#ifdef FLECS_PIPELINE
            m_desc.entity.add[0] = flecs::OnUpdate;
#endif
            m_desc.query.filter.expr = expr;
            this->populate_filter_from_pack();
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    system iter(Func&& func) const;

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    system each(Func&& func) const;

    ecs_system_desc_t m_desc;

protected:
    flecs::world_t* world_v() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func, bool is_each) const {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_system_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.self = m_desc.self;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        if (is_each) {
            desc.query.filter.instanced = true;
        }

        entity_t e = ecs_system_init(m_world, &desc);

        if (this->m_desc.query.filter.terms_buffer) {
            ecs_os_free(m_desc.query.filter.terms_buffer);
        }

        return e;
    }
};

}
