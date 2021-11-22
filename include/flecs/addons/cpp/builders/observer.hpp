#pragma once

#include "observer_i.hpp"

namespace flecs 
{

template<typename ... Components>
class observer_builder final
    : public observer_builder_i<observer_builder<Components ...>, Components ...>
{
    using Class = observer_builder<Components ...>;
public:
    explicit observer_builder(flecs::world_t* world, const char *name = nullptr, const char *expr = nullptr) 
        : observer_builder_i<Class, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
            m_desc.filter.expr = expr;
            this->populate_filter_from_pack();
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    observer<Components...> iter(Func&& func) const;

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    observer<Components...> each(Func&& func) const;

    ecs_observer_desc_t m_desc;

protected:
    flecs::world_t* world_v() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func, bool is_each) const {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_observer_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        if (is_each) {
            desc.filter.instanced = true;
        }

        ecs_entity_t result = ecs_observer_init(m_world, &desc);

        if (this->m_desc.filter.terms_buffer) {
            ecs_os_free(m_desc.filter.terms_buffer);
        }

        return result;
    }
};

}
