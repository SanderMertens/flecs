#include "private_api.h"

static
void observer_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->world;

    if (o->last_event_id == world->event_id) {
        /* Already handled this event */
        return;
    }

    ecs_iter_t user_it = *it;
    user_it.term_count = o->filter.term_count_actual,
    user_it.ids = NULL;
    user_it.columns = NULL;
    user_it.subjects = NULL;
    user_it.sizes = NULL;
    user_it.ptrs = NULL;

    flecs_iter_init(&user_it);

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;
    ecs_term_t *term = &o->filter.terms[it->term_index];

    if (term->oper == EcsNot) {
        table = it->other_table;
        prev_table = it->table;
    }

    if (!table) {
        table = &world->store.root;
    }
    if (!prev_table) {
        prev_table = &world->store.root;
    }

    ecs_type_t type = table->type;
    ecs_type_t prev_type = prev_table->type;

    /* Populate the column for the term that triggered. This will allow the
     * matching algorithm to pick the right column in case the term is a
     * wildcard matching multiple columns. */
    user_it.columns[0] = 0;
    user_it.columns[it->term_index] = it->columns[0];

    if (flecs_filter_match_table(world, &o->filter, table, type, user_it.offset,
        user_it.ids, user_it.columns, user_it.subjects, user_it.sizes, 
        user_it.ptrs, false, -1)) 
    {
        /* Monitor observers only trigger when the filter matches for the first
         * time with an entity */
        if (o->is_monitor) {
            if (world->is_fini) {
                goto done;
            }

            if (flecs_filter_match_table(world, &o->filter, prev_table, 
                prev_type, 0, NULL, NULL, NULL, NULL, NULL, true, -1)) {
                goto done;
            }

            if (term->oper == EcsNot) {
                /* Flip event if this is a Not, so OnAdd and OnRemove can be
                 * reliably used to check if we're entering or leaving the
                 * monitor */
                if (it->event == EcsOnAdd) {
                    user_it.event = EcsOnRemove;
                } else if (it->event == EcsOnRemove) {
                    user_it.event = EcsOnAdd;
                }
            }
        }

        user_it.ids[it->term_index] = it->event_id;
        user_it.system = o->entity;
        user_it.term_index = it->term_index;
        user_it.self = o->self;
        user_it.ctx = o->ctx;
        user_it.term_count = o->filter.term_count_actual;

        o->action(&user_it);
        o->last_event_id = world->event_id;
    }

done:
    flecs_iter_fini(&user_it);
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

    bool added = false;
    EcsObserver *comp = ecs_get_mut(world, entity, EcsObserver, &added);
    if (added) {
        ecs_observer_t *observer = flecs_sparse_add(
            world->observers, ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        observer->id = flecs_sparse_last_id(world->observers);

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.name = desc->entity.name;

        /* Parse filter */
        if (ecs_filter_init(world, &observer->filter, &filter_desc)) {
            flecs_observer_fini(world, observer);
            return 0;
        }

        ecs_filter_t *filter = &observer->filter;

        int i;
        for (i = 0; i < ECS_TRIGGER_DESC_EVENT_COUNT_MAX; i ++) {
            ecs_entity_t event = desc->events[i];
            if (!event) {
                break;
            }

            if (event == EcsMonitor) {
                /* Monitor event must be first and last event */
                ecs_assert(i == 0, ECS_INVALID_PARAMETER, NULL);

                observer->events[0] = EcsOnAdd;
                observer->events[1] = EcsOnRemove;
                observer->event_count ++;
                observer->is_monitor = true;
            } else {
                observer->events[i] = event;
            }

            observer->event_count ++;
        }

        /* Observer must have at least one event */
        ecs_assert(observer->event_count != 0, ECS_INVALID_PARAMETER, NULL);

        /* Create a trigger for each term in the filter */
        observer->triggers = ecs_os_malloc_n(ecs_entity_t, 
            observer->filter.term_count);

        for (i = 0; i < filter->term_count; i ++) {
            const ecs_term_t *terms = filter->terms;
            const ecs_term_t *t = &terms[i];

            if (terms[i].args[0].entity != EcsThis) {
                observer->triggers[i] = 0;
                continue;
            }

            ecs_trigger_desc_t trigger_desc = {
                .term = *t,
                .callback = observer_callback,
                .ctx = observer,
                .binding_ctx = desc->binding_ctx,
                .match_prefab = observer->filter.match_prefab,
                .match_disabled = observer->filter.match_disabled
            };

            ecs_os_memcpy(trigger_desc.events, observer->events, 
                ECS_SIZEOF(ecs_entity_t) * observer->event_count);

            observer->triggers[i] = ecs_trigger_init(world, &trigger_desc);
        }

        observer->action = desc->callback;
        observer->self = desc->self;
        observer->ctx = desc->ctx;
        observer->binding_ctx = desc->binding_ctx;
        observer->ctx_free = desc->ctx_free;
        observer->binding_ctx_free = desc->binding_ctx_free;
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

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    int i, count = observer->filter.term_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t trigger = observer->triggers[i];
        if (trigger) {
            ecs_delete(world, trigger);
        }
    }
    ecs_os_free(observer->triggers);

    ecs_filter_fini(&observer->filter);

    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    flecs_sparse_remove(world->observers, observer->id);
}

void* ecs_get_observer_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsObserver *o = ecs_get(world, observer, EcsObserver);
    if (o) {
        return o->observer->ctx;
    } else {
        return NULL;
    }     
}

void* ecs_get_observer_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsObserver *o = ecs_get(world, observer, EcsObserver);
    if (o) {
        return o->observer->binding_ctx;
    } else {
        return NULL;
    }      
}
