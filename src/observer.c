#include "private_api.h"

static
void observer_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->world;
    
    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->table->table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = it->table->table->type;
    if (ecs_filter_match_type(world, &o->filter, type)) {
        ecs_iter_t user_it = *it;
        user_it.ctx = o->ctx;
        o->action(&user_it);
    }
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

    bool added = false;
    EcsObserver *comp = ecs_get_mut(world, entity, EcsObserver, &added);
    if (added) {
        ecs_observer_t *observer = ecs_sparse_add(
            world->observers, ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        observer->id = ecs_sparse_last_id(world->observers);

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.name = desc->entity.name;

        /* Parse filter */
        if (ecs_filter_init(world, &observer->filter, &filter_desc)) {
            ecs_observer_fini(world, observer);
            return 0;
        }

        /* Create a trigger for each term in the filter */
        observer->triggers = ecs_os_malloc(ECS_SIZEOF(ecs_entity_t) * 
            observer->filter.term_count);
        
        int i;
        for (i = 0; i < observer->filter.term_count; i ++) {
            const ecs_term_t *t = &desc->filter.terms[i];
            if (t->oper == EcsNot) {
                /* No need to trigger on components that the entity should not
                 * have. */
                continue;
            }

            ecs_trigger_desc_t trigger_desc = {
                .term = *t,
                .callback = observer_callback,
                .ctx = observer,
                .binding_ctx = desc->binding_ctx
            };

            ecs_os_memcpy(trigger_desc.events, desc->events, 
                ECS_SIZEOF(ecs_entity_t) * ECS_TRIGGER_DESC_EVENT_COUNT_MAX);
            observer->triggers[i] = ecs_trigger_init(world, &trigger_desc);
        }

        observer->action = desc->callback;
        observer->ctx = desc->ctx;
        observer->binding_ctx = desc->binding_ctx;
        observer->ctx_free = desc->ctx_free;
        observer->binding_ctx_free = desc->binding_ctx_free;
        observer->event_count = 0;
        ecs_os_memcpy(observer->events, desc->events, 
            observer->event_count * ECS_SIZEOF(ecs_entity_t));
        observer->entity = entity;        

        comp->observer = observer;
    } else {
        ecs_assert(comp->observer != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If existing entity handle was provided, override existing params */
        if (existing) {
            if (desc->callback) {
                ((ecs_observer_t*)comp->observer)->action = desc->callback;
            }
            if (desc->ctx) {
                ((ecs_observer_t*)comp->observer)->ctx = desc->ctx;
            }
            if (desc->binding_ctx) {
                ((ecs_observer_t*)comp->observer)->binding_ctx = 
                    desc->binding_ctx;
            }
        }        
    }

    return entity; 
}

void ecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    ecs_filter_fini(&observer->filter);

    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    ecs_os_free(observer->triggers);

    ecs_sparse_remove(world->observers, observer->id);
}
