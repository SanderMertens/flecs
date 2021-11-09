#pragma once

#include "trigger_i.hpp"

namespace flecs 
{

template<typename ... Components>
class trigger_builder final
    : public trigger_builder_i<trigger_builder<Components...>, Components...>
{
    using Class = trigger_builder<Components...>;
public:
    explicit trigger_builder(flecs::world_t* world, const char *name = nullptr, flecs::id_t the_id = 0)
        : trigger_builder_i<Class, Components...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
            this->template populate_term_from_pack<Components...>();
            
            if (the_id) {
                /* Id should not be set if term is populated from template */
                ecs_assert(!m_desc.term.id, ECS_INVALID_PARAMETER, NULL);
                this->id(the_id);
            }
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    trigger<Components...> iter(Func&& func);

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    trigger<Components...> each(Func&& func);

    ecs_trigger_desc_t m_desc;

protected:
    flecs::world_t* world_v() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func) {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_trigger_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        entity_t result = ecs_trigger_init(m_world, &desc);

        ecs_term_fini(&m_desc.term);
        
        return result;
    }
};

}
