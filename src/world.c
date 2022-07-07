#include "private_api.h"

/* Roles */
const ecs_id_t ECS_PAIR =      (ECS_ROLE | (0x7Aull << 56));
const ecs_id_t ECS_OVERRIDE =  (ECS_ROLE | (0x75ull << 56));
const ecs_id_t ECS_DISABLED =  (ECS_ROLE | (0x74ull << 56));

/** Builtin component ids */
const ecs_entity_t ecs_id(EcsComponent) =          1;
const ecs_entity_t ecs_id(EcsIdentifier) =         2;
const ecs_entity_t ecs_id(EcsIterable) =           3;
const ecs_entity_t ecs_id(EcsPoly) =               4;

const ecs_entity_t EcsQuery =                      5;
const ecs_entity_t EcsObserver =                   7;

/* System module component ids */
const ecs_entity_t EcsSystem =                     10;
const ecs_entity_t ecs_id(EcsTickSource) =         11;

/** Timer module component ids */
const ecs_entity_t ecs_id(EcsTimer) =              13;
const ecs_entity_t ecs_id(EcsRateFilter) =         14;

/** Meta module component ids */
const ecs_entity_t ecs_id(EcsMetaType) =           15;
const ecs_entity_t ecs_id(EcsMetaTypeSerialized) = 16;
const ecs_entity_t ecs_id(EcsPrimitive) =          17;
const ecs_entity_t ecs_id(EcsEnum) =               18;
const ecs_entity_t ecs_id(EcsBitmask) =            19;
const ecs_entity_t ecs_id(EcsMember) =             20;
const ecs_entity_t ecs_id(EcsStruct) =             21;
const ecs_entity_t ecs_id(EcsArray) =              22;
const ecs_entity_t ecs_id(EcsVector) =             23;
const ecs_entity_t ecs_id(EcsUnit) =               24;
const ecs_entity_t ecs_id(EcsUnitPrefix) =         25;

/* Core scopes & entities */
const ecs_entity_t EcsWorld =                 ECS_HI_COMPONENT_ID + 0;
const ecs_entity_t EcsFlecs =                 ECS_HI_COMPONENT_ID + 1;
const ecs_entity_t EcsFlecsCore =             ECS_HI_COMPONENT_ID + 2;
const ecs_entity_t EcsFlecsInternals =           ECS_HI_COMPONENT_ID + 3;
const ecs_entity_t EcsModule =                ECS_HI_COMPONENT_ID + 4;
const ecs_entity_t EcsPrivate =               ECS_HI_COMPONENT_ID + 5;
const ecs_entity_t EcsPrefab =                ECS_HI_COMPONENT_ID + 6;
const ecs_entity_t EcsDisabled =              ECS_HI_COMPONENT_ID + 7;

/* Relation properties */
const ecs_entity_t EcsWildcard =              ECS_HI_COMPONENT_ID + 10;
const ecs_entity_t EcsAny =                   ECS_HI_COMPONENT_ID + 11;
const ecs_entity_t EcsThis =                  ECS_HI_COMPONENT_ID + 12;
const ecs_entity_t EcsVariable =              ECS_HI_COMPONENT_ID + 13;

const ecs_entity_t EcsTransitive =            ECS_HI_COMPONENT_ID + 14;
const ecs_entity_t EcsReflexive =             ECS_HI_COMPONENT_ID + 15;
const ecs_entity_t EcsSymmetric =             ECS_HI_COMPONENT_ID + 16;
const ecs_entity_t EcsFinal =                 ECS_HI_COMPONENT_ID + 17;
const ecs_entity_t EcsDontInherit =           ECS_HI_COMPONENT_ID + 18;
const ecs_entity_t EcsTag =                   ECS_HI_COMPONENT_ID + 19;
const ecs_entity_t EcsUnion =                 ECS_HI_COMPONENT_ID + 20;
const ecs_entity_t EcsExclusive =             ECS_HI_COMPONENT_ID + 21;
const ecs_entity_t EcsAcyclic =               ECS_HI_COMPONENT_ID + 22;
const ecs_entity_t EcsWith =                  ECS_HI_COMPONENT_ID + 23;
const ecs_entity_t EcsOneOf =                 ECS_HI_COMPONENT_ID + 24;

/* Builtin relations */
const ecs_entity_t EcsChildOf =               ECS_HI_COMPONENT_ID + 25;
const ecs_entity_t EcsIsA =                   ECS_HI_COMPONENT_ID + 26;
const ecs_entity_t EcsDependsOn =             ECS_HI_COMPONENT_ID + 27;

/* Identifier tags */
const ecs_entity_t EcsName =                  ECS_HI_COMPONENT_ID + 30;
const ecs_entity_t EcsSymbol =                ECS_HI_COMPONENT_ID + 31;
const ecs_entity_t EcsAlias =                 ECS_HI_COMPONENT_ID + 32;

/* Events */
const ecs_entity_t EcsOnAdd =                 ECS_HI_COMPONENT_ID + 33;
const ecs_entity_t EcsOnRemove =              ECS_HI_COMPONENT_ID + 34;
const ecs_entity_t EcsOnSet =                 ECS_HI_COMPONENT_ID + 35;
const ecs_entity_t EcsUnSet =                 ECS_HI_COMPONENT_ID + 36;
const ecs_entity_t EcsOnDelete =              ECS_HI_COMPONENT_ID + 37;
const ecs_entity_t EcsOnCreateTable =         ECS_HI_COMPONENT_ID + 38;
const ecs_entity_t EcsOnDeleteTable =         ECS_HI_COMPONENT_ID + 39;
const ecs_entity_t EcsOnTableEmpty =          ECS_HI_COMPONENT_ID + 40;
const ecs_entity_t EcsOnTableFill =           ECS_HI_COMPONENT_ID + 41;
const ecs_entity_t EcsOnCreateTrigger =       ECS_HI_COMPONENT_ID + 42;
const ecs_entity_t EcsOnDeleteTrigger =       ECS_HI_COMPONENT_ID + 43;
const ecs_entity_t EcsOnDeleteObservable =    ECS_HI_COMPONENT_ID + 44;
const ecs_entity_t EcsOnComponentHooks =      ECS_HI_COMPONENT_ID + 45;
const ecs_entity_t EcsOnDeleteObject =        ECS_HI_COMPONENT_ID + 46;

/* Actions */
const ecs_entity_t EcsRemove =                ECS_HI_COMPONENT_ID + 50;
const ecs_entity_t EcsDelete =                ECS_HI_COMPONENT_ID + 51;
const ecs_entity_t EcsPanic =                 ECS_HI_COMPONENT_ID + 52;

/* Misc */
const ecs_entity_t EcsDefaultChildComponent = ECS_HI_COMPONENT_ID + 55;

/* Systems */
const ecs_entity_t EcsMonitor =               ECS_HI_COMPONENT_ID + 61;
const ecs_entity_t EcsEmpty =                 ECS_HI_COMPONENT_ID + 63;
const ecs_entity_t ecs_id(EcsPipeline) =      ECS_HI_COMPONENT_ID + 64;
const ecs_entity_t EcsPreFrame =              ECS_HI_COMPONENT_ID + 65;
const ecs_entity_t EcsOnLoad =                ECS_HI_COMPONENT_ID + 66;
const ecs_entity_t EcsPostLoad =              ECS_HI_COMPONENT_ID + 67;
const ecs_entity_t EcsPreUpdate =             ECS_HI_COMPONENT_ID + 68;
const ecs_entity_t EcsOnUpdate =              ECS_HI_COMPONENT_ID + 69;
const ecs_entity_t EcsOnValidate =            ECS_HI_COMPONENT_ID + 70;
const ecs_entity_t EcsPostUpdate =            ECS_HI_COMPONENT_ID + 71;
const ecs_entity_t EcsPreStore =              ECS_HI_COMPONENT_ID + 72;
const ecs_entity_t EcsOnStore =               ECS_HI_COMPONENT_ID + 73;
const ecs_entity_t EcsPostFrame =             ECS_HI_COMPONENT_ID + 74;

const ecs_entity_t EcsPhase =                 ECS_HI_COMPONENT_ID + 75;

/* Meta primitive components (don't use low ids to save id space) */
const ecs_entity_t ecs_id(ecs_bool_t) =       ECS_HI_COMPONENT_ID + 80;
const ecs_entity_t ecs_id(ecs_char_t) =       ECS_HI_COMPONENT_ID + 81;
const ecs_entity_t ecs_id(ecs_byte_t) =       ECS_HI_COMPONENT_ID + 82;
const ecs_entity_t ecs_id(ecs_u8_t) =         ECS_HI_COMPONENT_ID + 83;
const ecs_entity_t ecs_id(ecs_u16_t) =        ECS_HI_COMPONENT_ID + 84;
const ecs_entity_t ecs_id(ecs_u32_t) =        ECS_HI_COMPONENT_ID + 85;
const ecs_entity_t ecs_id(ecs_u64_t) =        ECS_HI_COMPONENT_ID + 86;
const ecs_entity_t ecs_id(ecs_uptr_t) =       ECS_HI_COMPONENT_ID + 87;
const ecs_entity_t ecs_id(ecs_i8_t) =         ECS_HI_COMPONENT_ID + 88;
const ecs_entity_t ecs_id(ecs_i16_t) =        ECS_HI_COMPONENT_ID + 89;
const ecs_entity_t ecs_id(ecs_i32_t) =        ECS_HI_COMPONENT_ID + 90;
const ecs_entity_t ecs_id(ecs_i64_t) =        ECS_HI_COMPONENT_ID + 91;
const ecs_entity_t ecs_id(ecs_iptr_t) =       ECS_HI_COMPONENT_ID + 92;
const ecs_entity_t ecs_id(ecs_f32_t) =        ECS_HI_COMPONENT_ID + 93;
const ecs_entity_t ecs_id(ecs_f64_t) =        ECS_HI_COMPONENT_ID + 94;
const ecs_entity_t ecs_id(ecs_string_t) =     ECS_HI_COMPONENT_ID + 95;
const ecs_entity_t ecs_id(ecs_entity_t) =     ECS_HI_COMPONENT_ID + 96;
const ecs_entity_t EcsConstant =              ECS_HI_COMPONENT_ID + 97;
const ecs_entity_t EcsQuantity =              ECS_HI_COMPONENT_ID + 98;

/* Doc module components */
const ecs_entity_t ecs_id(EcsDocDescription) =ECS_HI_COMPONENT_ID + 100;
const ecs_entity_t EcsDocBrief =              ECS_HI_COMPONENT_ID + 101;
const ecs_entity_t EcsDocDetail =             ECS_HI_COMPONENT_ID + 102;
const ecs_entity_t EcsDocLink =               ECS_HI_COMPONENT_ID + 103;
const ecs_entity_t EcsDocColor =              ECS_HI_COMPONENT_ID + 104;

/* REST module components */
const ecs_entity_t ecs_id(EcsRest) =          ECS_HI_COMPONENT_ID + 105;

/* Default lookup path */
static ecs_entity_t ecs_default_lookup_path[2] = { 0, 0 };

/* -- Private functions -- */

const ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world)
{
    ecs_assert(ecs_poly_is(world, ecs_world_t) ||
               ecs_poly_is(world, ecs_stage_t),
               ECS_INTERNAL_ERROR,
               NULL);

    if (ecs_poly_is(world, ecs_world_t)) {
        return &world->stages[0];

    } else if (ecs_poly_is(world, ecs_stage_t)) {
        return (ecs_stage_t*)world;
    }
    
    return NULL;    
}

ecs_stage_t *flecs_stage_from_world(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(ecs_poly_is(world, ecs_world_t) ||
               ecs_poly_is(world, ecs_stage_t),
               ECS_INTERNAL_ERROR,
               NULL);

    if (ecs_poly_is(world, ecs_world_t)) {
        ecs_assert(!(world->flags & EcsWorldReadonly) || (ecs_get_stage_count(world) <= 1), 
            ECS_INVALID_OPERATION, NULL);
        return &world->stages[0];

    } else if (ecs_poly_is(world, ecs_stage_t)) {
        ecs_stage_t *stage = (ecs_stage_t*)world;
        *world_ptr = stage->world;
        return stage;
    }
    
    return NULL;
}

ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *stage_world,
    ecs_suspend_readonly_state_t *state)
{
    ecs_assert(stage_world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(state != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_world_t *world = (ecs_world_t*)ecs_get_world(stage_world);
    ecs_poly_assert(world, ecs_world_t);

    bool is_readonly = ECS_BIT_IS_SET(world->flags, EcsWorldReadonly);
    bool is_deferred = ecs_is_deferred(world);

    if (!is_readonly && !is_deferred) {
        state->is_readonly = false;
        state->is_deferred = false;
        return world;
    }

    ecs_dbg_3("suspending readonly mode");

    /* Cannot suspend when running with multiple threads */
    ecs_assert(ecs_get_stage_count(world) <= 1, 
        ECS_INVALID_WHILE_READONLY, NULL);

    state->is_readonly = is_readonly;
    state->is_deferred = is_deferred;

    /* Silence readonly checks */
    world->flags &= ~EcsWorldReadonly;

    /* Hack around safety checks (this ought to look ugly) */
    ecs_world_t *temp_world = world;
    ecs_stage_t *stage = flecs_stage_from_world(&temp_world);
    state->defer_count = stage->defer;
    state->defer_queue = stage->defer_queue;
    state->defer_stack = stage->defer_stack;
    flecs_stack_init(&stage->defer_stack);
    state->scope = stage->scope;
    state->with = stage->with;
    stage->defer = 0;
    stage->defer_queue = NULL;
    
    return world;
}

void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(state != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_world_t *temp_world = world;
    ecs_stage_t *stage = flecs_stage_from_world(&temp_world);

    if (state->is_readonly || state->is_deferred) {
        ecs_dbg_3("resuming readonly mode");
        
        ecs_run_aperiodic(world, 0);

        /* Restore readonly state / defer count */
        ECS_BIT_COND(world->flags, EcsWorldReadonly, state->is_readonly);
        stage->defer = state->defer_count;
        if (stage->defer_queue) {
            ecs_assert(ecs_vector_count(stage->defer_queue) == 0, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_vector_free(stage->defer_queue);
        }
        stage->defer_queue = state->defer_queue;
        flecs_stack_fini(&stage->defer_stack);
        stage->defer_stack = state->defer_stack;
        stage->scope = state->scope;
        stage->with = state->with;
    }
}

/* Evaluate component monitor. If a monitored entity changed it will have set a
 * flag in one of the world's component monitors. Queries can register 
 * themselves with component monitors to determine whether they need to rematch
 * with tables. */
static
void eval_component_monitor(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);

    if (!world->monitors.is_dirty) {
        return;
    }

    world->monitors.is_dirty = false;

    ecs_map_iter_t it = ecs_map_iter(&world->monitors.monitors);
    ecs_monitor_t *m;
    while ((m = ecs_map_next(&it, ecs_monitor_t, NULL))) {
        if (!m->is_dirty) {
            continue;
        }

        m->is_dirty = false;

        ecs_vector_each(m->queries, ecs_query_t*, q_ptr, {
            flecs_query_notify(world, *q_ptr, &(ecs_query_event_t) {
                .kind = EcsQueryTableRematch
            });
        });
    }
}

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_map_t *monitors = &world->monitors.monitors;

    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    if (ecs_map_is_initialized(monitors)) {
        ecs_monitor_t *m = ecs_map_get(monitors, ecs_monitor_t, id);
        if (m) {
            m->is_dirty = true;
            world->monitors.is_dirty = true;
        }
    }
}

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *monitors = &world->monitors.monitors;

    if (!ecs_map_is_initialized(monitors)) {
        ecs_map_init(monitors, ecs_monitor_t, 1);
    }

    ecs_monitor_t *m = ecs_map_ensure(monitors, ecs_monitor_t, id);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);        

    ecs_query_t **q = ecs_vector_add(&m->queries, ecs_query_t*);
    *q = query;
}

void flecs_monitor_unregister(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *monitors = &world->monitors.monitors;

    if (!ecs_map_is_initialized(monitors)) {
        return;
    }

    ecs_monitor_t *m = ecs_map_get(monitors, ecs_monitor_t, id);
    if (!m) {
        return;
    }

    int32_t i, count = ecs_vector_count(m->queries);
    ecs_query_t **queries = ecs_vector_first(m->queries, ecs_query_t*);
    for (i = 0; i < count; i ++) {
        if (queries[i] == query) {
            ecs_vector_remove(m->queries, ecs_query_t*, i);
            count --;
            break;
        }
    }

    if (!count) {
        ecs_vector_free(m->queries);
        ecs_map_remove(monitors, id);
    }

    if (!ecs_map_count(monitors)) {
        ecs_map_fini(monitors);
    }
}

static
void init_store(
    ecs_world_t *world) 
{
    ecs_os_memset(&world->store, 0, ECS_SIZEOF(ecs_store_t));
    
    /* Initialize entity index */
    flecs_sparse_init(&world->store.entity_index, ecs_record_t);
    flecs_sparse_set_id_source(&world->store.entity_index, 
        &world->info.last_id);

    /* Initialize root table */
    flecs_sparse_init(&world->store.tables, ecs_table_t);

    /* Initialize table map */
    flecs_table_hashmap_init(&world->store.table_map);

    /* Initialize one root table per stage */
    flecs_init_root_table(world);
}

static
void clean_tables(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(&world->store.tables);

    /* Ensure that first table in sparse set has id 0. This is a dummy table
     * that only exists so that there is no table with id 0 */
    ecs_table_t *first = flecs_sparse_get_dense(&world->store.tables, 
        ecs_table_t, 0);
    ecs_assert(first->id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)first;

    for (i = 1; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(&world->store.tables, 
            ecs_table_t, i);
        flecs_table_release(world, t);
    }

    /* Free table types separately so that if application destructors rely on
     * a type it's still valid. */
    for (i = 1; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(&world->store.tables, 
            ecs_table_t, i);
        flecs_table_free_type(t);
    }

    /* Clear the root table */
    if (count) {
        flecs_table_reset(world, &world->store.root);
    }
}

static
void fini_roots(ecs_world_t *world) {
    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_pair(EcsChildOf, 0));

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_table_cache_iter_t it;
    bool has_roots = flecs_table_cache_iter(&idr->cache, &it);
    ecs_assert(has_roots == true, ECS_INTERNAL_ERROR, NULL);
    (void)has_roots;

    /* Delete root entities that are not modules. This prioritizes deleting 
     * regular entities first, which reduces the chance of components getting
     * destructed in random order because it got deleted before entities,
     * thereby bypassing the OnDeleteObject policy. */
    ecs_defer_begin(world);

    const ecs_table_record_t *tr;
    while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
        ecs_table_t *table = tr->hdr.table;
        if (table->flags & EcsTableHasBuiltins) {
            continue; /* Filter out modules */
        }

        int32_t i, count = table->data.entities.count;
        ecs_entity_t *entities = table->data.entities.array;

        /* Count backwards so that we're always deleting the last entity in the
         * table which reduces moving components around */
        for (i = count - 1; i >= 0; i --) {
            ecs_record_t *r = flecs_entities_get(world, entities[i]);
            ecs_flags32_t flags = ECS_RECORD_TO_ROW_FLAGS(r->row);
            if (!(flags & EcsEntityObservedObject)) {
                continue; /* Filter out entities that aren't objects */
            }

            ecs_delete(world, entities[i]);
        }
    }

    ecs_defer_end(world);
}

static
void fini_store(ecs_world_t *world) {
    clean_tables(world);
    flecs_sparse_fini(&world->store.tables);
    flecs_table_release(world, &world->store.root);
    flecs_sparse_clear(&world->store.entity_index);
    flecs_hashmap_fini(&world->store.table_map);
    ecs_vector_free(world->store.records);
    ecs_vector_free(world->store.marked_ids);

    ecs_graph_edge_hdr_t *cur, *next = world->store.first_free;
    while ((cur = next)) {
        next = cur->next;
        ecs_os_free(cur);
    }
}

/* Implementation for iterable mixin */
static
bool world_iter_next(
    ecs_iter_t *it)
{
    if (ECS_BIT_IS_SET(it->flags, EcsIterIsValid)) {
        ECS_BIT_CLEAR(it->flags, EcsIterIsValid);
        return false;
    }

    ecs_world_t *world = it->real_world;
    ecs_sparse_t *entity_index = &world->store.entity_index;
    it->entities = (ecs_entity_t*)flecs_sparse_ids(entity_index);
    it->count = flecs_sparse_count(entity_index);
    flecs_iter_validate(it);

    return true;
}

static
void world_iter_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter)
{
    ecs_poly_assert(poly, ecs_world_t);
    (void)poly;

    if (filter) {
        iter[0] = ecs_term_iter(world, filter);
    } else {
        iter[0] = (ecs_iter_t){
            .world = (ecs_world_t*)world,
            .real_world = (ecs_world_t*)ecs_get_world(world),
            .next = world_iter_next
        };
    }
}

static
void log_addons(void) {
    ecs_trace("addons included in build:");
    ecs_log_push();
    #ifdef FLECS_CPP
        ecs_trace("FLECS_CPP");
    #endif
    #ifdef FLECS_MODULE
        ecs_trace("FLECS_MODULE");
    #endif
    #ifdef FLECS_PARSER
        ecs_trace("FLECS_PARSER");
    #endif
    #ifdef FLECS_PLECS
        ecs_trace("FLECS_PLECS");
    #endif
    #ifdef FLECS_RULES
        ecs_trace("FLECS_RULES");
    #endif
    #ifdef FLECS_SNAPSHOT
        ecs_trace("FLECS_SNAPSHOT");
    #endif
    #ifdef FLECS_STATS
        ecs_trace("FLECS_STATS");
    #endif
    #ifdef FLECS_MONITOR
        ecs_trace("FLECS_MONITOR");
    #endif
    #ifdef FLECS_SYSTEM
        ecs_trace("FLECS_SYSTEM");
    #endif
    #ifdef FLECS_PIPELINE
        ecs_trace("FLECS_PIPELINE");
    #endif
    #ifdef FLECS_TIMER
        ecs_trace("FLECS_TIMER");
    #endif
    #ifdef FLECS_META
        ecs_trace("FLECS_META");
    #endif
    #ifdef FLECS_META_C
        ecs_trace("FLECS_META_C");
    #endif
    #ifdef FLECS_UNITS
        ecs_trace("FLECS_UNITS");
    #endif
    #ifdef FLECS_EXPR
        ecs_trace("FLECS_EXPR");
    #endif
    #ifdef FLECS_JSON
        ecs_trace("FLECS_JSON");
    #endif
    #ifdef FLECS_DOC
        ecs_trace("FLECS_DOC");
    #endif
    #ifdef FLECS_COREDOC
        ecs_trace("FLECS_COREDOC");
    #endif
    #ifdef FLECS_LOG
        ecs_trace("FLECS_LOG");
    #endif
    #ifdef FLECS_APP
        ecs_trace("FLECS_APP");
    #endif
    #ifdef FLECS_OS_API_IMPL
        ecs_trace("FLECS_OS_API_IMPL");
    #endif
    #ifdef FLECS_HTTP
        ecs_trace("FLECS_HTTP");
    #endif
    #ifdef FLECS_REST
        ecs_trace("FLECS_REST");
    #endif
    ecs_log_pop();
}

/* -- Public functions -- */

ecs_world_t *ecs_mini(void) {
#ifdef FLECS_OS_API_IMPL
    ecs_set_os_api_impl();
#endif
    ecs_os_init();

    ecs_trace("#[bold]bootstrapping world");
    ecs_log_push();

    ecs_trace("tracing enabled, call ecs_log_set_level(-1) to disable");

    if (!ecs_os_has_heap()) {
        ecs_abort(ECS_MISSING_OS_API, NULL);
    }

    if (!ecs_os_has_threading()) {
        ecs_trace("threading unavailable, to use threads set OS API first (see examples)");
    }

    if (!ecs_os_has_time()) {
        ecs_trace("time management not available");
    }

    log_addons();

#ifdef FLECS_SANITIZE
    ecs_trace("sanitize build, rebuild without FLECS_SANITIZE for (much) "
        "improved performance");
#elif defined(FLECS_DEBUG)
    ecs_trace("debug build, rebuild with NDEBUG or FLECS_NDEBUG for improved "
        "performance");
#else
    ecs_trace("#[green]release#[reset] build");
#endif

#ifdef __clang__
    ecs_trace("compiled with clang %s", __clang_version__);
#elif defined(__GNUC__)
    ecs_trace("compiled with gcc %d.%d", __GNUC__, __GNUC_MINOR__);
#elif defined (_MSC_VER)
    ecs_trace("compiled with msvc %d", _MSC_VER);
#endif

    ecs_world_t *world = ecs_os_calloc_t(ecs_world_t);
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_poly_init(world, ecs_world_t);

    world->self = world;
    world->type_info = flecs_sparse_new(ecs_type_info_t);
    ecs_map_init(&world->id_index, ecs_id_record_t*, ECS_HI_COMPONENT_ID);
    flecs_observable_init(&world->observable);
    world->iterable.init = world_iter_init;
    world->pending_tables = flecs_sparse_new(ecs_table_t*);
    world->pending_buffer = flecs_sparse_new(ecs_table_t*);
    flecs_name_index_init(&world->aliases);
    flecs_name_index_init(&world->symbols);

    world->info.time_scale = 1.0;

    if (ecs_os_has_time()) {
        ecs_os_get_time(&world->world_start_time);
    }

    ecs_set_stage_count(world, 1);
    ecs_default_lookup_path[0] = EcsFlecsCore;
    ecs_set_lookup_path(world, ecs_default_lookup_path);
    init_store(world);

    flecs_bootstrap(world);

    ecs_trace("world ready!");
    ecs_log_pop();

    return world;
}

ecs_world_t *ecs_init(void) {
    ecs_world_t *world = ecs_mini();

#ifdef FLECS_MODULE_H
    ecs_trace("#[bold]import addons");
    ecs_log_push();
    ecs_trace("use ecs_mini to create world without importing addons");
#ifdef FLECS_SYSTEM
    ECS_IMPORT(world, FlecsSystem);
#endif
#ifdef FLECS_PIPELINE
    ECS_IMPORT(world, FlecsPipeline);
#endif
#ifdef FLECS_TIMER
    ECS_IMPORT(world, FlecsTimer);
#endif
#ifdef FLECS_META
    ECS_IMPORT(world, FlecsMeta);
#endif
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
#endif
#ifdef FLECS_COREDOC
    ECS_IMPORT(world, FlecsCoreDoc);
#endif
#ifdef FLECS_REST
    ECS_IMPORT(world, FlecsRest);
#endif
#ifdef FLECS_UNITS
    ecs_trace("#[green]module#[reset] flecs.units is not automatically imported");
#endif
    ecs_trace("addons imported!");
    ecs_log_pop();
#endif
    return world;
}

#define ARG(short, long, action)\
    if (i < argc) {\
        if (argv[i][0] == '-') {\
            if (argv[i][1] == '-') {\
                if (long && !strcmp(&argv[i][2], long ? long : "")) {\
                    action;\
                    parsed = true;\
                }\
            } else {\
                if (short && argv[i][1] == short) {\
                    action;\
                    parsed = true;\
                }\
            }\
        }\
    }

ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[])
{
    ecs_world_t *world = ecs_init();

    (void)argc;
    (void) argv;

#ifdef FLECS_DOC
    if (argc) {
        char *app = argv[0];
        char *last_elem = strrchr(app, '/');
        if (!last_elem) {
            last_elem = strrchr(app, '\\');
        }
        if (last_elem) {
            app = last_elem + 1;
        }
        ecs_set_pair(world, EcsWorld, EcsDocDescription, EcsName, {app});
    }
#endif

    return world;
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);
    world->flags |= EcsWorldQuit;
error:
    return;
}

bool ecs_should_quit(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return ECS_BIT_IS_SET(world->flags, EcsWorldQuit);
error:
    return true;
}

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event)
{
    ecs_poly_assert(world, ecs_world_t);

    /* If no id is specified, broadcast to all tables */
    if (!id) {
        ecs_sparse_t *tables = &world->store.tables;
        int32_t i, count = flecs_sparse_count(tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
            flecs_table_notify(world, table, event);
        }

    /* If id is specified, only broadcast to tables with id */
    } else {
        ecs_id_record_t *idr = flecs_id_record_get(world, id);
        if (!idr) {
            return;
        }

        ecs_table_cache_iter_t it;
        const ecs_table_record_t *tr;

        flecs_table_cache_all_iter(&idr->cache, &it);
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            flecs_table_notify(world, tr->hdr.table, event);
        }
    }
}

void ecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ti)
{
    ecs_os_memset(ptr, 0, ti->size * count);
}

static
void default_copy_ctor(void *dst_ptr, const void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->copy(dst_ptr, src_ptr, count, ti);
}

static
void default_move_ctor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void default_ctor_w_move_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void default_move_ctor_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move_ctor(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void default_move(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void default_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    /* When there is no move, destruct the destination component & memcpy the
     * component to dst. The src component does not have to be destructed when
     * a component has a trivial move. */
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->dtor(dst_ptr, count, ti);
    ecs_os_memcpy(dst_ptr, src_ptr, flecs_uto(ecs_size_t, ti->size) * count);
}

static
void default_move_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    /* If a component has a move, the move will take care of memcpying the data
     * and destroying any data in dst. Because this is not a trivial move, the
     * src component must also be destructed. */
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

void ecs_set_hooks_id(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_hooks_t *h)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_stage_from_world(&world);

    /* Ensure that no tables have yet been created for the component */
    ecs_assert( ecs_id_in_use(world, component) == false, 
        ECS_ALREADY_IN_USE, ecs_get_name(world, component));
    ecs_assert( ecs_id_in_use(world, ecs_pair(component, EcsWildcard)) == false, 
        ECS_ALREADY_IN_USE, ecs_get_name(world, component));

    ecs_type_info_t *ti = flecs_type_info_ensure(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!ti->component || ti->component == component, 
        ECS_INCONSISTENT_COMPONENT_ACTION, NULL);

    if (!ti->size) {
        const EcsComponent *component_ptr = ecs_get(
            world, component, EcsComponent);

        /* Cannot register lifecycle actions for things that aren't a component */
        ecs_check(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
        /* Cannot register lifecycle actions for components with size 0 */
        ecs_check(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);

        ti->size = component_ptr->size;
        ti->alignment = component_ptr->alignment;
    }

    if (h->ctor) ti->hooks.ctor = h->ctor;
    if (h->dtor) ti->hooks.dtor = h->dtor;
    if (h->copy) ti->hooks.copy = h->copy;
    if (h->move) ti->hooks.move = h->move;
    if (h->copy_ctor) ti->hooks.copy_ctor = h->copy_ctor;
    if (h->move_ctor) ti->hooks.move_ctor = h->move_ctor;
    if (h->ctor_move_dtor) ti->hooks.ctor_move_dtor = h->ctor_move_dtor;
    if (h->move_dtor) ti->hooks.move_dtor = h->move_dtor;

    if (h->on_add) ti->hooks.on_add = h->on_add;
    if (h->on_remove) ti->hooks.on_remove = h->on_remove;
    if (h->on_set) ti->hooks.on_set = h->on_set;

    if (h->ctx) ti->hooks.ctx = h->ctx;
    if (h->binding_ctx) ti->hooks.binding_ctx = h->binding_ctx;
    if (h->ctx_free) ti->hooks.ctx_free = h->ctx_free;
    if (h->binding_ctx_free) ti->hooks.binding_ctx_free = h->binding_ctx_free;

    /* If no constructor is set, invoking any of the other lifecycle actions 
     * is not safe as they will potentially access uninitialized memory. For 
     * ease of use, if no constructor is specified, set a default one that 
     * initializes the component to 0. */
    if (!h->ctor && (h->dtor || h->copy || h->move)) {
        ti->hooks.ctor = ecs_default_ctor;   
    }

    /* Set default copy ctor, move ctor and merge */
    if (h->copy && !h->copy_ctor) {
        ti->hooks.copy_ctor = default_copy_ctor;
    }

    if (h->move && !h->move_ctor) {
        ti->hooks.move_ctor = default_move_ctor;
    }

    if (!h->ctor_move_dtor) {
        if (h->move) {
            if (h->dtor) {
                if (h->move_ctor) {
                    /* If an explicit move ctor has been set, use callback 
                     * that uses the move ctor vs. using a ctor+move */
                    ti->hooks.ctor_move_dtor = default_move_ctor_w_dtor;
                } else {
                    /* If no explicit move_ctor has been set, use
                     * combination of ctor + move + dtor */
                    ti->hooks.ctor_move_dtor = default_ctor_w_move_w_dtor;
                }
            } else {
                /* If no dtor has been set, this is just a move ctor */
                ti->hooks.ctor_move_dtor = ti->hooks.move_ctor;
            }            
        }
    }

    if (!h->move_dtor) {
        if (h->move) {
            if (h->dtor) {
                ti->hooks.move_dtor = default_move_w_dtor;
            } else {
                ti->hooks.move_dtor = default_move;
            }
        } else {
            if (h->dtor) {
                ti->hooks.move_dtor = default_dtor;
            }
        }
    }

error:
    return;
}

const ecs_type_hooks_t* ecs_get_hooks_id(
    ecs_world_t *world,
    ecs_entity_t id)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    if (ti) {
        return &ti->hooks;
    }
    return NULL;
}

void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(action != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_action_elem_t *elem = ecs_vector_add(&world->fini_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;
error:
    return;
}

void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(action != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_action_elem_t *elem = ecs_vector_add(&stage->post_frame_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx; 
error:
    return;
}

/* Unset data in tables */
static
void fini_unset_tables(
    ecs_world_t *world)
{
    ecs_sparse_t *tables = &world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
        flecs_table_remove_actions(world, table);
    }
}

/* Invoke fini actions */
static
void fini_actions(
    ecs_world_t *world)
{
    ecs_vector_each(world->fini_actions, ecs_action_elem_t, elem, {
        elem->action(world, elem->ctx);
    });

    ecs_vector_free(world->fini_actions);
}

/* Cleanup remaining type info elements */
static
void fini_type_info(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(world->type_info);
    ecs_sparse_t *type_info = world->type_info;
    for (i = 0; i < count; i ++) {
        ecs_type_info_t *ti = flecs_sparse_get_dense(type_info, 
            ecs_type_info_t, i);
        flecs_type_info_fini(ti);
    }
    flecs_sparse_free(world->type_info);
}

ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    if (ecs_has_id(world, e, EcsOneOf)) {
        return e;
    } else {
        return ecs_get_object(world, e, EcsOneOf, 0);
    }
}

/* The destroyer of worlds */
int ecs_fini(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->stages[0].defer == 0, ECS_INVALID_OPERATION, 
        "call defer_end before destroying world");

    ecs_trace("#[bold]shutting down world");
    ecs_log_push();

    world->flags |= EcsWorldQuit;

    /* Delete root entities first using regular APIs. This ensures that cleanup
     * policies get a chance to execute. */
    ecs_dbg_1("#[bold]cleanup root entities");
    ecs_log_push_1();
    fini_roots(world);
    ecs_log_pop_1();

    world->flags |= EcsWorldFini;

    /* Run fini actions (simple callbacks ran when world is deleted) before
     * destroying the storage */
    ecs_dbg_1("#[bold]run fini actions");
    ecs_log_push_1();
    fini_actions(world);
    ecs_log_pop_1();

    ecs_dbg_1("#[bold]cleanup remaining entities");
    ecs_log_push_1();

    /* Operations invoked during UnSet/OnRemove/destructors are deferred and
     * will be discarded after world cleanup */
    ecs_defer_begin(world);

    /* Run UnSet/OnRemove actions for components while the store is still
     * unmodified by cleanup. */
    fini_unset_tables(world);

    /* This will destroy all entities and components. After this point no more
     * user code is executed. */
    fini_store(world);

    /* Purge deferred operations from the queue. This discards operations but
     * makes sure that any resources in the queue are freed */
    flecs_defer_purge(world, &world->stages[0]);
    ecs_log_pop_1();

    /* All queries are cleaned up, so monitors should've been cleaned up too */
    ecs_assert(!ecs_map_is_initialized(&world->monitors.monitors), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_dbg_1("#[bold]cleanup world datastructures");
    ecs_log_push_1();
    flecs_sparse_fini(&world->store.entity_index);
    flecs_fini_id_records(world);
    fini_type_info(world);
    flecs_observable_fini(&world->observable);
    flecs_name_index_fini(&world->aliases);
    flecs_name_index_fini(&world->symbols);
    ecs_set_stage_count(world, 0);
    ecs_log_pop_1();

    /* End of the world */
    ecs_poly_free(world, ecs_world_t);
    ecs_os_fini();

    ecs_trace("world destroyed, bye!");
    ecs_log_pop();

    return 0;
}

bool ecs_is_fini(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return ECS_BIT_IS_SET(world->flags, EcsWorldFini);
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    ecs_poly_assert(world, ecs_world_t);
    flecs_entities_set_size(world, entity_count + ECS_HI_COMPONENT_ID);
}

void flecs_eval_component_monitors(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);  
    flecs_process_pending_tables(world);  
    eval_component_monitor(world);
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (world->info.target_fps == 0.0f || enable) {
        ECS_BIT_COND(world->flags, EcsWorldMeasureFrameTime, enable);
    }
error:
    return;
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);
    ECS_BIT_COND(world->flags, EcsWorldMeasureSystemTime, enable);
error:
    return;
}

void ecs_set_target_fps(
    ecs_world_t *world,
    ecs_ftime_t fps)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    ecs_measure_frame_time(world, true);
    world->info.target_fps = fps;
error:
    return;
}

void* ecs_get_context(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    world = ecs_get_world(world);
    return world->context;
error:
    return NULL;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_poly_assert(world, ecs_world_t);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(!id_end || id_end > id_start, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!id_end || id_end > world->info.last_id, 
        ECS_INVALID_PARAMETER, NULL);

    if (world->info.last_id < id_start) {
        world->info.last_id = id_start - 1;
    }

    world->info.min_id = id_start;
    world->info.max_id = id_end;
error:
    return;
}

bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable)
{
    ecs_poly_assert(world, ecs_world_t);    
    bool old_value = world->range_check_enabled;
    world->range_check_enabled = enable;
    return old_value;
}

void ecs_set_entity_generation(
    ecs_world_t *world,
    ecs_entity_t entity_with_generation)
{
    flecs_sparse_set_generation(
        &world->store.entity_index, entity_with_generation);
}

const ecs_type_info_t* flecs_type_info_get(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_poly_assert(world, ecs_world_t);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    return flecs_sparse_get(world->type_info, ecs_type_info_t, component);
}

ecs_type_info_t* flecs_type_info_ensure(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    const ecs_type_info_t *ti = flecs_type_info_get(world, component);
    ecs_type_info_t *ti_mut = NULL;
    if (!ti) {
        ti_mut = flecs_sparse_ensure(
            world->type_info, ecs_type_info_t, component);
        ecs_assert(ti_mut != NULL, ECS_INTERNAL_ERROR, NULL);
        ti_mut->component = component;
    } else {
        ti_mut = (ecs_type_info_t*)ti;
    }

    return ti_mut;
}

bool flecs_type_info_init_id(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_size_t size,
    ecs_size_t alignment,
    const ecs_type_hooks_t *li)
{
    bool changed = false;

    flecs_entities_ensure(world, component);

    ecs_type_info_t *ti = NULL;
    if (!size || !alignment) {
        ecs_assert(size == 0 && alignment == 0, 
            ECS_INVALID_COMPONENT_SIZE, NULL);
        ecs_assert(li == NULL, ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        flecs_sparse_remove(world->type_info, component);
    } else {
        ti = flecs_type_info_ensure(world, component);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        changed |= ti->size != size;
        changed |= ti->alignment != alignment;
        ti->size = size;
        ti->alignment = alignment;
        if (li) {
            ecs_set_hooks_id(world, component, li);
        }
    }

    /* Set type info for id record of component */
    ecs_id_record_t *idr = flecs_id_record_ensure(world, component);
    changed |= flecs_id_record_set_type_info(world, idr, ti);
    bool is_tag = idr->flags & EcsIdTag;

    /* All id records with component as relation inherit type info */
    idr = flecs_id_record_ensure(world, ecs_pair(component, EcsWildcard));
    do {
        if (is_tag) {
            changed |= flecs_id_record_set_type_info(world, idr, NULL);
        } else if (ti) {
            changed |= flecs_id_record_set_type_info(world, idr, ti);
        } else if ((idr->type_info != NULL) && 
            (idr->type_info->component == component))
        {
            changed |= flecs_id_record_set_type_info(world, idr, NULL);
        }
    } while ((idr = idr->first.next));

    /* All non-tag id records with component as object inherit type info,
     * if relation doesn't have type info */
    idr = flecs_id_record_ensure(world, ecs_pair(EcsWildcard, component));
    do {
        if (!(idr->flags & EcsIdTag) && !idr->type_info) {
            changed |= flecs_id_record_set_type_info(world, idr, ti);
        }
    } while ((idr = idr->first.next));

    /* Type info of (*, component) should always point to component */
    ecs_assert(flecs_id_record_get(world, ecs_pair(EcsWildcard, component))->
        type_info == ti, ECS_INTERNAL_ERROR, NULL);

    return changed;
}

void flecs_type_info_fini(
    ecs_type_info_t *ti)
{
    if (ti->hooks.ctx_free) {
        ti->hooks.ctx_free(ti->hooks.ctx);
    }
    if (ti->hooks.binding_ctx_free) {
        ti->hooks.binding_ctx_free(ti->hooks.binding_ctx);
    }
}

void flecs_type_info_free(
    ecs_world_t *world,
    ecs_entity_t component)
{
    if (world->flags & EcsWorldFini) {
        /* If world is in the final teardown stages, cleanup policies are no
         * longer applied and it can't be guaranteed that a component is not
         * deleted before entities that use it. The remaining type info elements
         * will be deleted after the store is finalized. */
        return;
    }
    ecs_type_info_t *ti = flecs_sparse_remove_get(
        world->type_info, ecs_type_info_t, component);
    if (ti) {
        flecs_type_info_fini(ti);
    }
}

static
ecs_ftime_t flecs_insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    ecs_poly_assert(world, ecs_world_t);  

    ecs_time_t start = *stop;
    ecs_ftime_t delta_time = (ecs_ftime_t)ecs_time_measure(stop);

    if (world->info.target_fps == (ecs_ftime_t)0.0) {
        return delta_time;
    }

    ecs_ftime_t target_delta_time = 
        ((ecs_ftime_t)1.0 / (ecs_ftime_t)world->info.target_fps);

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    ecs_ftime_t sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 4 times smaller than the time one frame
     * should take. */
    ecs_ftime_t sleep_time = sleep / (ecs_ftime_t)4.0;

    do {
        /* Only call sleep when sleep_time is not 0. On some platforms, even
         * a sleep with a timeout of 0 can cause stutter. */
        if (sleep_time != 0) {
            ecs_sleepf((double)sleep_time);
        }

        ecs_time_t now = start;
        delta_time = (ecs_ftime_t)ecs_time_measure(&now);
    } while ((target_delta_time - delta_time) > 
        (sleep_time / (ecs_ftime_t)2.0));

    return delta_time;
}

static
ecs_ftime_t flecs_start_measure_frame(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    ecs_poly_assert(world, ecs_world_t);  

    ecs_ftime_t delta_time = 0;

    if ((world->flags & EcsWorldMeasureFrameTime) || (user_delta_time == 0)) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.nanosec || world->frame_start_time.sec){ 
                delta_time = flecs_insert_sleep(world, &t);

                ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->info.target_fps != 0) {
                    delta_time = (ecs_ftime_t)1.0 / world->info.target_fps;
                } else {
                    /* Best guess */
                    delta_time = (ecs_ftime_t)1.0 / (ecs_ftime_t)60.0; 
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Keep track of total time passed in world */
        world->info.world_time_total_raw += (ecs_ftime_t)delta_time;
    }

    return (ecs_ftime_t)delta_time;
}

static
void flecs_stop_measure_frame(
    ecs_world_t* world)
{
    ecs_poly_assert(world, ecs_world_t);  

    if (world->flags & EcsWorldMeasureFrameTime) {
        ecs_time_t t = world->frame_start_time;
        world->info.frame_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }
}

ecs_ftime_t ecs_frame_begin(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);
    ecs_check(user_delta_time != 0 || ecs_os_has_time(), 
        ECS_MISSING_OS_API, "get_time");

    /* Start measuring total frame time */
    ecs_ftime_t delta_time = flecs_start_measure_frame(world, user_delta_time);
    if (user_delta_time == 0) {
        user_delta_time = delta_time;
    }  

    world->info.delta_time_raw = user_delta_time;
    world->info.delta_time = user_delta_time * world->info.time_scale;

    /* Keep track of total scaled time passed in world */
    world->info.world_time_total += world->info.delta_time;

    ecs_run_aperiodic(world, 0);

    return world->info.delta_time;
error:
    return (ecs_ftime_t)0;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);

    world->info.frame_count_total ++;
    
    ecs_stage_t *stages = world->stages;
    int32_t i, count = world->stage_count;
    for (i = 0; i < count; i ++) {
        flecs_stage_merge_post_frame(world, &stages[i]);
    }

    flecs_stop_measure_frame(world);
error:
    return;
}

const ecs_world_info_t* ecs_get_world_info(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return &world->info;
}

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event)
{
    ecs_poly_assert(world, ecs_world_t); 

    ecs_id_record_t *idr = flecs_id_record_get(world, 
        ecs_pair(ecs_id(EcsPoly), EcsQuery));
    if (!idr) {
        return;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);
    
    ecs_table_cache_iter_t it;
    const ecs_table_record_t *tr;
    if (flecs_table_cache_iter(&idr->cache, &it)) {
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            EcsPoly *queries = ecs_table_get_column(table, tr->column);
            int32_t i, count = ecs_table_count(table);

            for (i = 0; i < count; i ++) {
                ecs_query_t *query = queries[i].poly;
                if (query->flags & EcsQueryIsSubquery) {
                    continue;
                }

                ecs_poly_assert(query, ecs_query_t);
                flecs_query_notify(world, query, event);
            }
        }
    }    
}

void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t); 
    flecs_table_release(world, table);
}

static
void flecs_process_empty_queries(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t); 

    ecs_id_record_t *idr = flecs_id_record_get(world, 
        ecs_pair(ecs_id(EcsPoly), EcsQuery));
    if (!idr) {
        return;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    /* Make sure that we defer adding the inactive tags until after iterating
     * the query */
    flecs_defer_begin(world, &world->stages[0]);

    ecs_table_cache_iter_t it;
    const ecs_table_record_t *tr;
    if (flecs_table_cache_iter(&idr->cache, &it)) {
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            EcsPoly *queries = ecs_table_get_column(table, tr->column);
            int32_t i, count = ecs_table_count(table);

            for (i = 0; i < count; i ++) {
                ecs_query_t *query = queries[i].poly;
                ecs_entity_t *entities = table->data.entities.array;
                if (!ecs_query_table_count(query)) {
                    ecs_add_id(world, entities[i], EcsEmpty);
                }
            }
        }
    }

    flecs_defer_end(world, &world->stages[0]);
}

/** Walk over tables that had a state change which requires bookkeeping */
void flecs_process_pending_tables(
    const ecs_world_t *world_r)
{
    ecs_poly_assert(world_r, ecs_world_t);

    /* We can't update the administration while in readonly mode, but we can
     * ensure that when this function is called there are no pending events. */
    if (world_r->flags & EcsWorldReadonly) {
        ecs_assert(flecs_sparse_count(world_r->pending_tables) == 0,
            ECS_INTERNAL_ERROR, NULL);
        return;
    }

    /* Safe to cast, world is not readonly */
    ecs_world_t *world = (ecs_world_t*)world_r;
    
    /* If pending buffer is NULL there already is a stackframe that's iterating
     * the table list. This can happen when an observer for a table event results
     * in a mutation that causes another table to change state. A typical 
     * example of this is a system that becomes active/inactive as the result of
     * a query (and as a result, its matched tables) becoming empty/non empty */
    if (!world->pending_buffer) {
        return;
    }

    /* Swap buffer. The logic could in theory have been implemented with a
     * single sparse set, but that would've complicated (and slowed down) the
     * iteration. Additionally, by using a double buffer approach we can still
     * keep most of the original ordering of events intact, which is desirable
     * as it means that the ordering of tables in the internal datastructures is
     * more predictable. */
    int32_t i, count = flecs_sparse_count(world->pending_tables);
    if (!count) {
        return;
    }

    do {
        ecs_sparse_t *pending_tables = world->pending_tables;
        world->pending_tables = world->pending_buffer;
        world->pending_buffer = NULL;

        /* Make sure that any ECS operations that occur while delivering the
         * events does not cause inconsistencies, like sending an Empty 
         * notification for a table that just became non-empty. */
        ecs_defer_begin(world);

        for (i = 0; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense(
                pending_tables, ecs_table_t*, i)[0];
            if (!table->id) {
                /* Table is being deleted, ignore empty events */
                continue;
            }

            /* For each id in the table, add it to the empty/non empty list
             * based on its current state */
            if (flecs_table_records_update_empty(table)) {
                /* Only emit an event when there was a change in the 
                 * administration. It is possible that a table ended up in the
                 * pending_tables list by going from empty->non-empty, but then
                 * became empty again. By the time we run this code, no changes
                 * in the administration would actually be made. */
                int32_t table_count = ecs_table_count(table);
                flecs_emit(world, world, &(ecs_event_desc_t){
                    .event = table_count
                        ? EcsOnTableFill 
                        : EcsOnTableEmpty
                        ,
                    .table = table,
                    .ids = &table->type,
                    .observable = world,
                    .table_event = true
                });

                world->info.empty_table_count += (table_count == 0) * 2 - 1;
            }
        }
        flecs_sparse_clear(pending_tables);

        ecs_defer_end(world);

        world->pending_buffer = pending_tables;
    } while ((count = flecs_sparse_count(world->pending_tables)));
}

void flecs_table_set_empty(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ecs_table_count(table)) {
        table->generation = 0;
    }

    flecs_sparse_set_generation(world->pending_tables, (uint32_t)table->id);
    flecs_sparse_ensure(world->pending_tables, ecs_table_t*, 
        (uint32_t)table->id)[0] = table;
}

bool ecs_id_in_use(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return false;
    }
    return (flecs_table_cache_count(&idr->cache) != 0) ||
        (flecs_table_cache_empty_count(&idr->cache) != 0);
}

void ecs_run_aperiodic(
    ecs_world_t *world,
    ecs_flags32_t flags)
{
    ecs_poly_assert(world, ecs_world_t);
    
    if (!flags || (flags & EcsAperiodicEmptyTables)) {
        flecs_process_pending_tables(world);
    }
    if ((flags & EcsAperiodicEmptyQueries)) {
        flecs_process_empty_queries(world);
    }
    if (!flags || (flags & EcsAperiodicComponentMonitors)) {
        flecs_eval_component_monitors(world);
    }
}

int32_t ecs_delete_empty_tables(
    ecs_world_t *world,
    ecs_id_t id,
    uint16_t clear_generation,
    uint16_t delete_generation,
    int32_t min_id_count,
    double time_budget_seconds)
{
    ecs_poly_assert(world, ecs_world_t);

    /* Make sure empty tables are in the empty table lists */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_time_t start = {0}, cur = {0};
    int32_t delete_count = 0, clear_count = 0;
    bool time_budget = false;

    ecs_time_measure(&start);
    if (time_budget_seconds != 0) {
        time_budget = true;
    }

    if (!id) {
        id = EcsAny; /* Iterate all empty tables */
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    ecs_table_cache_iter_t it;
    if (idr && flecs_table_cache_empty_iter((ecs_table_cache_t*)idr, &it)) {
        ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            if (time_budget) {
                cur = start;
                if (ecs_time_measure(&cur) > time_budget_seconds) {
                    goto done;
                }
            }

            ecs_table_t *table = tr->hdr.table;
            ecs_assert(ecs_table_count(table) == 0, ECS_INTERNAL_ERROR, NULL);
            if (table->refcount > 1) {
                /* Don't delete claimed tables */
                continue;
            }

            if (table->type.count < min_id_count) {
                continue;
            }

            uint16_t gen = ++ table->generation;
            if (delete_generation && (gen > delete_generation)) {
                if (flecs_table_release(world, table)) {
                    delete_count ++;
                }
            } else if (clear_generation && (gen > clear_generation)) {
                if (flecs_table_shrink(world, table)) {
                    clear_count ++;
                }
            }
        }
    }

done:
    if (delete_count) {
        ecs_dbg_1("#[red]deleted#[normal] %d empty tables in %.2fs", 
            delete_count, ecs_time_measure(&start));
    }
    if (clear_count) {
        ecs_dbg_1("#[red]cleared#[normal] %d empty tables in %.2fs", 
            clear_count, ecs_time_measure(&start));
    }
    return delete_count;
}
