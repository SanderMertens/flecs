#include "private_api.h"

/* Roles */
const ecs_id_t ECS_PAIR =      (ECS_ROLE | (0x7Aull << 56));
const ecs_id_t ECS_OVERRIDE =  (ECS_ROLE | (0x75ull << 56));
const ecs_id_t ECS_DISABLED =  (ECS_ROLE | (0x74ull << 56));

/** Builtin component ids */
const ecs_entity_t ecs_id(EcsComponent) =          1;
const ecs_entity_t ecs_id(EcsComponentLifecycle) = 2;
const ecs_entity_t ecs_id(EcsType) =               3;
const ecs_entity_t ecs_id(EcsIdentifier) =         4;
const ecs_entity_t ecs_id(EcsTrigger) =            5;
const ecs_entity_t ecs_id(EcsQuery) =              6;
const ecs_entity_t ecs_id(EcsObserver) =           7;
const ecs_entity_t ecs_id(EcsIterable) =           8;

/* System module component ids */
const ecs_entity_t ecs_id(EcsSystem) =             10;
const ecs_entity_t ecs_id(EcsTickSource) =         11;

/** Pipeline module component ids */
const ecs_entity_t ecs_id(EcsPipelineQuery) =      12;

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
const ecs_entity_t EcsFlecsHidden =           ECS_HI_COMPONENT_ID + 3;
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

/* Identifier tags */
const ecs_entity_t EcsName =                  ECS_HI_COMPONENT_ID + 27;
const ecs_entity_t EcsSymbol =                ECS_HI_COMPONENT_ID + 28;
const ecs_entity_t EcsAlias =                 ECS_HI_COMPONENT_ID + 29;

/* Events */
const ecs_entity_t EcsOnAdd =                 ECS_HI_COMPONENT_ID + 30;
const ecs_entity_t EcsOnRemove =              ECS_HI_COMPONENT_ID + 31;
const ecs_entity_t EcsOnSet =                 ECS_HI_COMPONENT_ID + 32;
const ecs_entity_t EcsUnSet =                 ECS_HI_COMPONENT_ID + 33;
const ecs_entity_t EcsOnDelete =              ECS_HI_COMPONENT_ID + 34;
const ecs_entity_t EcsOnCreateTable =         ECS_HI_COMPONENT_ID + 35;
const ecs_entity_t EcsOnDeleteTable =         ECS_HI_COMPONENT_ID + 36;
const ecs_entity_t EcsOnTableEmpty =          ECS_HI_COMPONENT_ID + 37;
const ecs_entity_t EcsOnTableFill =           ECS_HI_COMPONENT_ID + 38;
const ecs_entity_t EcsOnCreateTrigger =       ECS_HI_COMPONENT_ID + 39;
const ecs_entity_t EcsOnDeleteTrigger =       ECS_HI_COMPONENT_ID + 40;
const ecs_entity_t EcsOnDeleteObservable =    ECS_HI_COMPONENT_ID + 41;
const ecs_entity_t EcsOnComponentLifecycle =  ECS_HI_COMPONENT_ID + 42;
const ecs_entity_t EcsOnDeleteObject =        ECS_HI_COMPONENT_ID + 43;

/* Actions */
const ecs_entity_t EcsRemove =                ECS_HI_COMPONENT_ID + 50;
const ecs_entity_t EcsDelete =                ECS_HI_COMPONENT_ID + 51;
const ecs_entity_t EcsPanic =                 ECS_HI_COMPONENT_ID + 52;

/* Misc */
const ecs_entity_t EcsDefaultChildComponent = ECS_HI_COMPONENT_ID + 55;

/* Systems */
const ecs_entity_t EcsMonitor =               ECS_HI_COMPONENT_ID + 61;
const ecs_entity_t EcsInactive =              ECS_HI_COMPONENT_ID + 63;
const ecs_entity_t EcsPipeline =              ECS_HI_COMPONENT_ID + 64;
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
        return &world->stage;

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
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
        return &world->stage;

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

    bool is_readonly = world->is_readonly;
    bool is_deferred = ecs_is_deferred(world);

    if (!world->is_readonly && !is_deferred) {
        state->is_readonly = false;
        state->is_deferred = false;
        return world;
    }

    ecs_dbg_3("suspending readonly mode");

    /* Cannot suspend when running with multiple threads */
    ecs_assert(ecs_get_stage_count(world) <= 1, 
        ECS_INVALID_WHILE_ITERATING, NULL);

    state->is_readonly = is_readonly;
    state->is_deferred = is_deferred;

    /* Silence readonly checks */
    world->is_readonly = false;

    /* Hack around safety checks (this ought to look ugly) */
    ecs_world_t *temp_world = world;
    ecs_stage_t *stage = flecs_stage_from_world(&temp_world);
    state->defer_count = stage->defer;
    state->defer_queue = stage->defer_queue;
    state->scope = world->stage.scope;
    state->with = world->stage.with;
    stage->defer = 0;
    stage->defer_queue = NULL;

    if (&world->stage != (ecs_stage_t*)stage_world) {
        world->stage.scope = stage->scope;
        world->stage.with = stage->with;
    }
    
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
        world->is_readonly = state->is_readonly;
        stage->defer = state->defer_count;
        stage->defer_queue = state->defer_queue;
        world->stage.scope = state->scope;
        world->stage.with = state->with;
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
void monitors_fini(
    ecs_world_t *world)
{
    ecs_map_t *monitors = &world->monitors.monitors;

    ecs_map_iter_t it = ecs_map_iter(monitors);
    ecs_monitor_t *m;
    while ((m = ecs_map_next(&it, ecs_monitor_t, NULL))) {
        ecs_vector_free(m->queries);
    }

    ecs_map_fini(monitors);
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
void fini_store(ecs_world_t *world) {
    clean_tables(world);
    flecs_sparse_fini(&world->store.tables);
    flecs_table_release(world, &world->store.root);
    flecs_sparse_clear(&world->store.entity_index);
    flecs_hashmap_fini(&world->store.table_map);
    ecs_vector_free(world->store.records);

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

    world->queries = flecs_sparse_new(ecs_query_t);
    world->triggers = flecs_sparse_new(ecs_trigger_t);
    world->observers = flecs_sparse_new(ecs_observer_t);
    
    world->pending_tables = flecs_sparse_new(ecs_table_t*);
    world->pending_buffer = flecs_sparse_new(ecs_table_t*);

    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    flecs_name_index_init(&world->aliases);
    flecs_name_index_init(&world->symbols);
    ecs_map_init(&world->type_handles, ecs_entity_t, 0);

    world->info.time_scale = 1.0;

    if (ecs_os_has_time()) {
        ecs_os_get_time(&world->world_start_time);
    }

    flecs_stage_init(world, &world->stage);
    ecs_set_stages(world, 1);

    ecs_default_lookup_path[0] = EcsFlecsCore;
    ecs_set_lookup_path(world, ecs_default_lookup_path);

    init_store(world);
    ecs_trace("table store initialized");

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
    world->should_quit = true;
error:
    return;
}

bool ecs_should_quit(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->should_quit;
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
        ecs_id_record_t *idr = flecs_get_id_record(world, id);
        if (!idr) {
            return;
        }

        ecs_table_cache_iter_t it;
        const ecs_table_record_t *tr;

        flecs_table_cache_iter(&idr->cache, &it);
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            flecs_table_notify(world, tr->hdr.table, event);
        }

        flecs_table_cache_empty_iter(&idr->cache, &it);
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
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->ctor(dst_ptr, count, ti);
    cl->copy(dst_ptr, src_ptr, count, ti);
}

static
void default_move_ctor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void default_ctor_w_move_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void default_move_ctor_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->move_ctor(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void default_move(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void default_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    /* When there is no move, destruct the destination component & memcpy the
     * component to dst. The src component does not have to be destructed when
     * a component has a trivial move. */
    const EcsComponentLifecycle *cl = &ti->lifecycle;
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
    const EcsComponentLifecycle *cl = &ti->lifecycle;
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_entity_t component,
    const EcsComponentLifecycle *lifecycle)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);

    ecs_type_info_t *ti = flecs_ensure_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_size_t size = ti->size;
    ecs_size_t alignment = ti->alignment;

    if (!size) {
        const EcsComponent *component_ptr = ecs_get(
            world, component, EcsComponent);

        /* Cannot register lifecycle actions for things that aren't a component */
        ecs_check(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
        /* Cannot register lifecycle actions for components with size 0 */
        ecs_check(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);

        size = component_ptr->size;
        alignment = component_ptr->alignment;
    }

    if (ti->lifecycle_set) {
        ecs_assert(ti->component == component, ECS_INTERNAL_ERROR, NULL);
        ecs_check(!lifecycle->ctor || ti->lifecycle.ctor == lifecycle->ctor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_check(!lifecycle->dtor || ti->lifecycle.dtor == lifecycle->dtor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_check(!lifecycle->copy || ti->lifecycle.copy == lifecycle->copy, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_check(!lifecycle->move || ti->lifecycle.move == lifecycle->move, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);

        if (!ti->lifecycle.on_set) {
            ti->lifecycle.on_set = lifecycle->on_set;
        }
        if (!ti->lifecycle.on_remove) {
            ti->lifecycle.on_remove = lifecycle->on_remove;
        }
    } else {
        ti->component = component;
        ti->lifecycle = *lifecycle;
        ti->lifecycle_set = true;
        ti->size = size;
        ti->alignment = alignment;

        /* If no constructor is set, invoking any of the other lifecycle actions 
         * is not safe as they will potentially access uninitialized memory. For 
         * ease of use, if no constructor is specified, set a default one that 
         * initializes the component to 0. */
        if (!lifecycle->ctor && 
            (lifecycle->dtor || lifecycle->copy || lifecycle->move)) 
        {
            ti->lifecycle.ctor = ecs_default_ctor;   
        }

        /* Set default copy ctor, move ctor and merge */
        if (lifecycle->copy && !lifecycle->copy_ctor) {
            ti->lifecycle.copy_ctor = default_copy_ctor;
        }

        if (lifecycle->move && !lifecycle->move_ctor) {
            ti->lifecycle.move_ctor = default_move_ctor;
        }

        if (!lifecycle->ctor_move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    if (lifecycle->move_ctor) {
                        /* If an explicit move ctor has been set, use callback 
                         * that uses the move ctor vs. using a ctor+move */
                        ti->lifecycle.ctor_move_dtor = 
                            default_move_ctor_w_dtor;
                    } else {
                        /* If no explicit move_ctor has been set, use
                         * combination of ctor + move + dtor */
                        ti->lifecycle.ctor_move_dtor = 
                            default_ctor_w_move_w_dtor;
                    }
                } else {
                    /* If no dtor has been set, this is just a move ctor */
                    ti->lifecycle.ctor_move_dtor = 
                        ti->lifecycle.move_ctor;
                }            
            }
        }

        if (!lifecycle->move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    ti->lifecycle.move_dtor = default_move_w_dtor;
                } else {
                    ti->lifecycle.move_dtor = default_move;
                }
            } else {
                if (lifecycle->dtor) {
                    ti->lifecycle.move_dtor = default_dtor;
                }
            }
        }

        /* Ensure that no tables have yet been created for the component */
        ecs_assert( ecs_id_in_use(world, component) == false, 
            ECS_ALREADY_IN_USE, ecs_get_name(world, component));
        ecs_assert( ecs_id_in_use(world, 
            ecs_pair(component, EcsWildcard)) == false, 
                ECS_ALREADY_IN_USE, ecs_get_name(world, component));
    }
error:
    return;
}

bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(component != 0, ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);
    const ecs_type_info_t *ti = flecs_get_type_info(world, component);
    return (ti != NULL) && ti->lifecycle_set;
error:
    return false;
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

/* Cleanup component lifecycle callbacks & systems */
static
void fini_component_lifecycle(
    ecs_world_t *world)
{
    flecs_sparse_free(world->type_info);
}

/* Cleanup queries */
static
void fini_queries(
    ecs_world_t *world)
{
    monitors_fini(world);
    
    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *query = flecs_sparse_get_dense(world->queries, ecs_query_t, 0);
        ecs_query_fini(query);
    }
    flecs_sparse_free(world->queries);
}

static
void fini_observers(
    ecs_world_t *world)
{
    flecs_sparse_free(world->observers);
}

/* Cleanup stages */
static
void fini_stages(
    ecs_world_t *world)
{
    flecs_stage_deinit(world, &world->stage);
    ecs_set_stages(world, 0);
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

/* Cleanup misc structures */
static
void fini_misc(
    ecs_world_t *world)
{
    ecs_map_fini(&world->type_handles);
    ecs_vector_free(world->fini_tasks);
}

/* The destroyer of worlds */
int ecs_fini(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    ecs_trace("#[bold]shutting down world");
    ecs_log_push();

    world->is_fini = true;

    /* Operations invoked during UnSet/OnRemove/destructors are deferred and
     * will be discarded after world cleanup */
    ecs_defer_begin(world);

    /* Run UnSet/OnRemove actions for components while the store is still
     * unmodified by cleanup. */
    fini_unset_tables(world);
    
    /* Run fini actions (simple callbacks ran when world is deleted) before
     * destroying the storage */
    fini_actions(world);

    /* This will destroy all entities and components. After this point no more
     * user code is executed. */
    fini_store(world);

    /* Purge deferred operations from the queue. This discards operations but
     * makes sure that any resources in the queue are freed */
    flecs_defer_purge(world, &world->stage);

    /* Entity index is kept alive until this point so that user code can do
     * validity checks on entity ids, even though after store cleanup the index
     * will be empty, so all entity ids are invalid. */
    flecs_sparse_fini(&world->store.entity_index);
    
    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    ecs_trace("table store deinitialized");

    fini_stages(world);

    fini_component_lifecycle(world);

    fini_queries(world);

    fini_observers(world);

    flecs_fini_id_records(world);

    flecs_observable_fini(&world->observable);

    flecs_sparse_free(world->triggers);

    flecs_name_index_fini(&world->aliases);
    flecs_name_index_fini(&world->symbols);
    
    fini_misc(world);

    ecs_os_enable_high_timer_resolution(false);

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
    return world->is_fini;
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_eis_set_size(world, entity_count + ECS_HI_COMPONENT_ID);
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
        world->measure_frame_time = enable;
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
    world->measure_system_time = enable;
error:
    return;
}

void ecs_set_target_fps(
    ecs_world_t *world,
    FLECS_FLOAT fps)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    ecs_measure_frame_time(world, true);
    world->info.target_fps = fps;
    ecs_os_enable_high_timer_resolution(fps >= 60.0f);
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

int32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->worker_stages);
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
    ecs_poly_assert(world, ecs_world_t);

    if (enable) {
        if (!world->locking_enabled) {
            world->mutex = ecs_os_mutex_new();
            world->thr_sync = ecs_os_mutex_new();
            world->thr_cond = ecs_os_cond_new();
        }
    } else {
        if (world->locking_enabled) {
            ecs_os_mutex_free(world->mutex);
            ecs_os_mutex_free(world->thr_sync);
            ecs_os_cond_free(world->thr_cond);
        }
    }

    bool old = world->locking_enabled;
    world->locking_enabled = enable;
    return old;
}

void ecs_lock(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->mutex);
}

void ecs_unlock(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->mutex);
}

void ecs_begin_wait(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->thr_sync);
    ecs_os_cond_wait(world->thr_cond, world->thr_sync);
}

void ecs_end_wait(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->thr_sync);
}

const ecs_type_info_t* flecs_get_type_info(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_poly_assert(world, ecs_world_t);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    return flecs_sparse_get(world->type_info, ecs_type_info_t, component);
}

ecs_type_info_t* flecs_ensure_type_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    const ecs_type_info_t *ti = flecs_get_type_info(world, component);
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

static
void flecs_remove_type_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    flecs_sparse_remove(world->type_info, component);
}

bool flecs_init_type_info_id(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_size_t size,
    ecs_size_t alignment,
    const EcsComponentLifecycle *li)
{
    bool changed = false;

    ecs_eis_ensure(world, component);

    ecs_type_info_t *ti = NULL;
    if (!size || !alignment) {
        ecs_assert(size == 0 && alignment == 0, 
            ECS_INVALID_COMPONENT_SIZE, NULL);
        ecs_assert(li == NULL, ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        flecs_remove_type_info(world, component);
    } else {
        ti = flecs_ensure_type_info(world, component);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        changed |= ti->size != size;
        changed |= ti->alignment != alignment;
        ti->size = size;
        ti->alignment = alignment;
        if (li) {
            ecs_set_component_actions_w_id(world, component, li);
        }
    }

    /* Set type info for id record of component */
    ecs_id_record_t *idr = flecs_ensure_id_record(world, component);
    changed |= flecs_set_type_info_for_id_record(world, idr, ti);
    bool is_tag = idr->flags & EcsIdTag;

    /* All id records with component as relation inherit type info */
    idr = flecs_ensure_id_record(world, ecs_pair(component, EcsWildcard));
    do {
        if (is_tag) {
            changed |= flecs_set_type_info_for_id_record(world, idr, NULL);
        } else if (ti) {
            changed |= flecs_set_type_info_for_id_record(world, idr, ti);
        } else if ((idr->type_info != NULL) && 
            (idr->type_info->component == component))
        {
            changed |= flecs_set_type_info_for_id_record(world, idr, NULL);
        }
    } while ((idr = idr->first.next));

    /* All non-tag id records with component as object inherit type info,
     * if relation doesn't have type info */
    idr = flecs_ensure_id_record(world, ecs_pair(EcsWildcard, component));
    do {
        if (!(idr->flags & EcsIdTag) && !idr->type_info) {
            changed |= flecs_set_type_info_for_id_record(world, idr, ti);
        }
    } while ((idr = idr->first.next));

    /* Type info of (*, component) should always point to component */
    ecs_assert(flecs_get_id_record(world, ecs_pair(EcsWildcard, component))->
        type_info == ti, ECS_INTERNAL_ERROR, NULL);

    return changed;
}

static
FLECS_FLOAT insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    ecs_poly_assert(world, ecs_world_t);  

    ecs_time_t start = *stop;
    FLECS_FLOAT delta_time = (FLECS_FLOAT)ecs_time_measure(stop);

    if (world->info.target_fps == (FLECS_FLOAT)0.0) {
        return delta_time;
    }

    FLECS_FLOAT target_delta_time = 
        ((FLECS_FLOAT)1.0 / (FLECS_FLOAT)world->info.target_fps);

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    FLECS_FLOAT sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 4 times smaller than the time one frame
     * should take. */
    FLECS_FLOAT sleep_time = sleep / (FLECS_FLOAT)4.0;

    do {
        /* Only call sleep when sleep_time is not 0. On some platforms, even
         * a sleep with a timeout of 0 can cause stutter. */
        if (sleep_time != 0) {
            ecs_sleepf((double)sleep_time);
        }

        ecs_time_t now = start;
        delta_time = (FLECS_FLOAT)ecs_time_measure(&now);
    } while ((target_delta_time - delta_time) > 
        (sleep_time / (FLECS_FLOAT)2.0));

    return delta_time;
}

static
FLECS_FLOAT start_measure_frame(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_poly_assert(world, ecs_world_t);  

    FLECS_FLOAT delta_time = 0;

    if (world->measure_frame_time || (user_delta_time == 0)) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.nanosec || world->frame_start_time.sec){ 
                delta_time = insert_sleep(world, &t);

                ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->info.target_fps != 0) {
                    delta_time = (FLECS_FLOAT)1.0 / world->info.target_fps;
                } else {
                    /* Best guess */
                    delta_time = (FLECS_FLOAT)1.0 / (FLECS_FLOAT)60.0; 
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Keep track of total time passed in world */
        world->info.world_time_total_raw += (FLECS_FLOAT)delta_time;
    }

    return (FLECS_FLOAT)delta_time;
}

static
void stop_measure_frame(
    ecs_world_t* world)
{
    ecs_poly_assert(world, ecs_world_t);  

    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        world->info.frame_time_total += (FLECS_FLOAT)ecs_time_measure(&t);
    }
}

FLECS_FLOAT ecs_frame_begin(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);
    ecs_check(user_delta_time != 0 || ecs_os_has_time(), 
        ECS_MISSING_OS_API, "get_time");

    if (world->locking_enabled) {
        ecs_lock(world);
    }

    /* Start measuring total frame time */
    FLECS_FLOAT delta_time = start_measure_frame(world, user_delta_time);
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
    return (FLECS_FLOAT)0;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    world->info.frame_count_total ++;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        flecs_stage_merge_post_frame(world, stage);
    });        

    if (world->locking_enabled) {
        ecs_unlock(world);

        ecs_os_mutex_lock(world->thr_sync);
        ecs_os_cond_broadcast(world->thr_cond);
        ecs_os_mutex_unlock(world->thr_sync);
    }

    stop_measure_frame(world);
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

    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *query = flecs_sparse_get_dense(
            world->queries, ecs_query_t, i);
        if (query->flags & EcsQueryIsSubquery) {
            continue;
        }
        
        flecs_query_notify(world, query, event);
    }    
}

void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t); 
    flecs_table_release(world, table);
}

/** Walk over tables that had a state change which requires bookkeeping */
void flecs_process_pending_tables(
    const ecs_world_t *world_r)
{
    ecs_poly_assert(world_r, ecs_world_t);

    /* We can't update the administration while in readonly mode, but we can
     * ensure that when this function is called there are no pending events. */
    if (world_r->is_readonly) {
        ecs_assert(flecs_sparse_count(world_r->pending_tables) == 0,
            ECS_INTERNAL_ERROR, NULL);
        return;
    }

    /* Safe to cast, world is not readonly */
    ecs_world_t *world = (ecs_world_t*)world_r;
    
    /* If pending buffer is NULL there already is a stackframe that's iterating
     * the table list. This can happen when a trigger for a table event results
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
                flecs_emit(world, world, &(ecs_event_desc_t) {
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

        world->pending_buffer = pending_tables;
    } while ((count = flecs_sparse_count(world->pending_tables)));
}

void flecs_table_set_empty(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

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
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return false;
    }
    return (ecs_table_cache_count(&idr->cache) != 0) ||
        (ecs_table_cache_empty_count(&idr->cache) != 0);
}

void ecs_run_aperiodic(
    ecs_world_t *world,
    ecs_flags32_t flags)
{
    ecs_poly_assert(world, ecs_world_t);
    
    if (!flags || (flags & EcsAperiodicEmptyTableEvents)) {
        flecs_process_pending_tables(world);
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

    ecs_table_cache_iter_t it;
    if (flecs_empty_table_iter(world, id, &it)) {
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
