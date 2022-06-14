#include "private_api.h"

static
bool observer_run(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->real_world;

    ecs_assert(o->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    if (o->last_event_id == world->event_id) {
        /* Already handled this event */
        return false;
    }

    o->last_event_id = world->event_id;

    ecs_iter_t user_it = *it;
    user_it.term_count = o->filter.term_count_actual;
    user_it.terms = o->filter.terms;
    user_it.flags = 0;
    ECS_BIT_COND(user_it.flags, EcsIterIsFilter,    
        ECS_BIT_IS_SET(o->filter.flags, EcsFilterIsFilter));
    user_it.ids = NULL;
    user_it.columns = NULL;
    user_it.subjects = NULL;
    user_it.sizes = NULL;
    user_it.ptrs = NULL;
    flecs_iter_init(&user_it, flecs_iter_cache_all);

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;
    int32_t pivot_term = it->term_index;
    ecs_term_t *term = &o->filter.terms[pivot_term];

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

    static int obs_count = 0;
    obs_count ++;

    /* Populate the column for the term that triggered. This will allow the
     * matching algorithm to pick the right column in case the term is a
     * wildcard matching multiple columns. */
    user_it.columns[0] = 0;

    /* Normalize id */
    int32_t column = it->columns[0];
    if (column < 0) {
        column = -column;
    }

    user_it.columns[pivot_term] = column;

    if (flecs_filter_match_table(world, &o->filter, table, user_it.ids, 
        user_it.columns, user_it.subjects, NULL, NULL, false, -1, 
        user_it.flags))
    {
        /* Monitor observers only trigger when the filter matches for the first
         * time with an entity */
        if (o->is_monitor) {
            if (flecs_filter_match_table(world, &o->filter, prev_table, 
                NULL, NULL, NULL, NULL, NULL, true, -1, user_it.flags)) 
            {
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

        flecs_iter_populate_data(world, &user_it, 
            it->table, it->offset, it->count, user_it.ptrs, user_it.sizes);

        user_it.ids[it->term_index] = it->event_id;
        user_it.system = o->entity;
        user_it.term_index = it->term_index;
        user_it.self = o->self;
        user_it.ctx = o->ctx;
        user_it.term_count = o->filter.term_count_actual;
        flecs_iter_validate(&user_it);

        o->callback(&user_it);

        ecs_iter_fini(&user_it);
        return true;
    }

done:
    ecs_iter_fini(&user_it);
    return false;
}

bool ecs_observer_default_run_action(ecs_iter_t *it) {
    return observer_run(it);
}

static 
void default_observer_run_callback(ecs_iter_t *it) {
    observer_run(it);
}

/* For convenience, so applications can (in theory) use a single run callback 
 * that uses ecs_iter_next to iterate results */
static 
bool default_observer_next_callback(ecs_iter_t *it) {
    if (it->interrupted_by) {
        return false;
    } else {
        it->interrupted_by = it->system;
        return true;
    }
}

static
void observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_run_action_t run = o->run;

    if (run) {
        it->next = default_observer_next_callback;
        it->callback = default_observer_run_callback;
        it->interrupted_by = 0;
        run(it);
    } else {
        observer_run(it);
    }
}

static
void observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    ecs_run_action_t run = observer->run;
    if (!run) {
        run = default_observer_run_callback;
    }

    int32_t pivot_term = ecs_filter_pivot_term(world, &observer->filter);
    if (pivot_term < 0) {
        return;
    }

    /* If yield existing is enabled, trigger for each thing that matches
     * the event, if the event is iterable. */
    int i, count = observer->event_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t evt = observer->events[i];
        const EcsIterable *iterable = ecs_get(world, evt, EcsIterable);
        if (!iterable) {
            continue;
        }

        ecs_iter_t it;
        iterable->init(world, world, &it, &observer->filter.terms[pivot_term]);
        it.terms = observer->filter.terms;
        it.term_count = 1;
        it.term_index = pivot_term;
        it.system = observer->entity;
        it.ctx = observer;
        it.binding_ctx = observer->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            run(&it);
            world->event_id ++;
        }
    }
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_entity_t entity = 0;
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, NULL);
    ecs_check(desc->callback != NULL || desc->run != NULL, 
        ECS_INVALID_OPERATION, NULL);

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    entity = ecs_entity_init(world, &desc->entity);
    if (!existing && !desc->entity.name) {
        ecs_add_pair(world, entity, EcsChildOf, EcsFlecsHidden);
    }

    bool added = false;
    EcsObserver *comp = ecs_get_mut(world, entity, EcsObserver, &added);
    if (added) {
        ecs_observer_t *observer = flecs_sparse_add(
            world->observers, ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        observer->id = flecs_sparse_last_id(world->observers);
        comp->observer = observer;

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.name = desc->entity.name;

        /* Parse filter */
        ecs_filter_t *filter = &observer->filter;
        if (ecs_filter_init(world, filter, &filter_desc)) {
            flecs_observer_fini(world, observer);
            return 0;
        }

        /* Creating an observer with no terms has no effect */
        ecs_assert(observer->filter.term_count != 0, 
            ECS_INVALID_PARAMETER, NULL);

        int i, e;
        for (i = 0; i < ECS_TRIGGER_DESC_EVENT_COUNT_MAX; i ++) {
            ecs_entity_t event = desc->events[i];
            if (!event) {
                break;
            }

            if (event == EcsMonitor) {
                /* Monitor event must be first and last event */
                ecs_check(i == 0, ECS_INVALID_PARAMETER, NULL);

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
        ecs_check(observer->event_count != 0, ECS_INVALID_PARAMETER, NULL);

        observer->callback = desc->callback;
        observer->run = desc->run;
        observer->self = desc->self;
        observer->ctx = desc->ctx;
        observer->binding_ctx = desc->binding_ctx;
        observer->ctx_free = desc->ctx_free;
        observer->binding_ctx_free = desc->binding_ctx_free;
        observer->entity = entity;
        comp->observer = observer;

        /* Create a trigger for each term in the filter */
        ecs_trigger_desc_t tdesc = {
            .callback = observer_run_callback,
            .ctx = observer,
            .binding_ctx = desc->binding_ctx,
            .match_prefab = ECS_BIT_IS_SET(filter->flags, EcsFilterMatchPrefab),
            .match_disabled = ECS_BIT_IS_SET(filter->flags, EcsFilterMatchDisabled),
            .last_event_id = &observer->last_event_id
        };

        bool optional_only = filter->flags & EcsFilterMatchThis;
        for (i = 0; i < filter->term_count; i ++) {
            if (filter->terms[i].oper != EcsOptional) {
                if (filter->terms[i].subj.entity == EcsThis) {
                    optional_only = false;
                }
            }
        }

        for (i = 0; i < filter->term_count; i ++) {
            tdesc.term = filter->terms[i];
            ecs_oper_kind_t oper = tdesc.term.oper;
            ecs_id_t id = tdesc.term.id;

            bool is_tag = ecs_id_is_tag(world, id);

            if (is_tag) {
                /* If id is a tag, convert OnSet/UnSet to OnAdd/OnRemove. This
                 * allows for creating OnSet observers with both components and
                 * tags that only fire when the entity has all ids */
                for (e = 0; e < observer->event_count; e ++) {
                    if (observer->events[e] == EcsOnSet) {
                        tdesc.events[e] = EcsOnAdd;
                    } else
                    if (observer->events[e] == EcsUnSet) {
                        tdesc.events[e] = EcsOnRemove;
                    } else {
                        tdesc.events[e] = observer->events[e];
                    }
                }
            } else {
                ecs_os_memcpy_n(tdesc.events, observer->events, ecs_entity_t,
                    observer->event_count);
            }

            /* AndFrom & OrFrom terms insert multiple triggers */
            if (oper == EcsAndFrom || oper == EcsOrFrom) {
                const ecs_type_t *type = ecs_get_type(world, id);
                int32_t ti, ti_count = type->count;
                ecs_id_t *ti_ids = type->array;

                /* Correct operator will be applied when a trigger occurs, and
                 * the observer is evaluated on the trigger source */
                tdesc.term.oper = EcsAnd;
                for (ti = 0; ti < ti_count; ti ++) {
                    ecs_id_t ti_id = ti_ids[ti];
                    ecs_id_record_t *idr = flecs_id_record_get(world, ti_id);
                    if (idr->flags & EcsIdDontInherit) {
                        continue;
                    }
                    tdesc.term.pred.name = NULL;
                    tdesc.term.pred.entity = ti_ids[ti];
                    tdesc.term.id = ti_ids[ti];

                    ecs_entity_t t = ecs_vector_add(&observer->triggers, 
                        ecs_entity_t)[0] = ecs_trigger_init(world, &tdesc);
                    if (!t) {
                        goto error;
                    }
                }
                continue;
            }

            /* If observer only contains optional terms, match everything */
            if (optional_only) {
                tdesc.term.id = EcsAny;
                tdesc.term.pred.entity = EcsAny;
                tdesc.term.subj.entity = EcsThis;
                tdesc.term.subj.var = EcsVarIsVariable;
                tdesc.term.obj.entity = 0;
            }

            ecs_entity_t t = ecs_vector_add(&observer->triggers, ecs_entity_t)
                [0] = ecs_trigger_init(world, &tdesc);
            if (!t) {
                goto error;
            }

            if (optional_only) {
                break;
            }
        }

        if (desc->entity.name) {
            ecs_trace("#[green]observer#[reset] %s created", 
                ecs_get_name(world, entity));
        }

        if (desc->yield_existing) {
            observer_yield_existing(world, observer);
        }
    } else {
        ecs_assert(comp->observer != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If existing entity handle was provided, override existing params */
        if (existing) {
            if (desc->callback) {
                ((ecs_observer_t*)comp->observer)->callback = desc->callback;
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
error:
    if (entity) {
        ecs_delete(world, entity);
    }
    return 0;
}

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    /* Cleanup triggers */
    int i, count = ecs_vector_count(observer->triggers);
    ecs_entity_t *triggers = ecs_vector_first(observer->triggers, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t t = triggers[i];
        if (!t) continue;
        ecs_delete(world, triggers[i]);
    }
    ecs_vector_free(observer->triggers);

    /* Cleanup filters */
    ecs_filter_fini(&observer->filter);

    /* Cleanup context */
    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    /* Cleanup observer storage */
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
