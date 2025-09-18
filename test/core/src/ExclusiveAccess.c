#include <core.h>

static ecs_entity_t thr_entity = 0;
static ecs_id_t thr_component = 0;
static ecs_ref_t thr_ref = {0};
static ecs_query_t *thr_query = NULL;

void ExclusiveAccess_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    test_assert(ecs_is_alive(world, e));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Velocity));

    ecs_clear(world, e);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_defer_begin(world);

    e = ecs_new(world);
    test_assert(ecs_is_alive(world, e));

    ecs_add(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_exclusive_access_end(world, false);

    ecs_fini(world);
}

void ExclusiveAccess_self_world_fini(void) {
    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_fini(world);

    test_assert(true);
}

void ExclusiveAccess_begin_twice(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    test_expect_abort();
    ecs_exclusive_access_begin(world, NULL);
}

void ExclusiveAccess_end_without_begin(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_exclusive_access_end(world, false);
}

static
void* thread_exclusive_access_mismatching_begin(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_exclusive_access_begin(world, NULL);
    return NULL;
}

void ExclusiveAccess_mismatching_begin(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_mismatching_begin, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_lock_world(void) {
    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_exclusive_access_end(world, true); // no mutations allowed

    ecs_exclusive_access_begin(world, NULL); // make sure we can obtain access again

    test_assert(true);

    ecs_fini(world);
}

void ExclusiveAccess_fini_while_locked_world(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // no mutations allowed

    test_expect_abort();
    ecs_fini(world);
}

void ExclusiveAccess_fini_while_locked_world_unlocked(void) {
    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);

    test_assert(true);
}

static
void* thread_exclusive_access_mismatching_end(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_exclusive_access_end(world, false);
    return NULL;
}

void ExclusiveAccess_mismatching_end(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_mismatching_end, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_new(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_new(world);
    return NULL;
}

void ExclusiveAccess_other_new(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_new, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_new_low_id(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_new_low_id(world);
    return NULL;
}

void ExclusiveAccess_other_world_new_low_id(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_new_low_id, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_delete(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_delete(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_delete(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_delete, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_clear(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_clear(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_clear(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_clear, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_add(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_add_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_add(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_add, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_add_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_add, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_remove(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_remove_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_remove(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_remove, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_remove_non_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_remove, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_set(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    Position p = {10, 20};
    ecs_set_id(world, thr_entity, thr_component, sizeof(Position), &p);
    return NULL;
}

void ExclusiveAccess_other_set(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_set_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_ensure(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_ensure_id(world, thr_entity, thr_component, sizeof(Position));
    return NULL;
}

void ExclusiveAccess_other_ensure(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ensure, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_ensure_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ensure, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_emplace(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    bool is_new;
    ecs_emplace_id(world, thr_entity, thr_component, sizeof(Position), &is_new);
    return NULL;
}

void ExclusiveAccess_other_emplace(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_emplace, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_emplace_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_emplace, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_defer_begin(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_defer_begin(world);
    return NULL;
}

void ExclusiveAccess_other_defer_begin(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_defer_begin, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_defer_end(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_defer_end(world);
    return NULL;
}

void ExclusiveAccess_other_defer_end(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_defer_begin(world);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_defer_end, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_create_query(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ thr_component, .inout = EcsIn }}
    });
    ecs_query_fini(q);
    return NULL;
}

static
void* thread_exclusive_access_other_create_mut_query(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ thr_component }}
    });
    ecs_query_fini(q);
    return NULL;
}

void ExclusiveAccess_other_create_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_create_mut_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_mut_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_create_cached_query(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ thr_component }}, 
        .cache_kind = EcsQueryCacheAll 
    });
    ecs_query_fini(q);

    return NULL;
}

void ExclusiveAccess_other_create_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_cached_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_iter_query(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_query_iter(world, thr_query);
    return NULL;
}

void ExclusiveAccess_other_iter_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position), .inout = EcsIn }}
    });

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_iter_mut_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position) }}
    });

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_iter_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position), .inout = EcsIn }},
        .cache_kind = EcsQueryCacheAll
    });

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_iter_mut_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position) }},
        .cache_kind = EcsQueryCacheAll
    });

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_iter_children(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_children(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_iter_children(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_children, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_each(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_each_id(world, thr_component);
    return NULL;
}

void ExclusiveAccess_other_each(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_each, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_create_table(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_table_find(world, &thr_component, 1);
    return NULL;
}

void ExclusiveAccess_other_create_table(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_table, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_register_component(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ECS_COMPONENT(world, Position);
    return NULL;
}

void ExclusiveAccess_other_register_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_register_component, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_fini(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_fini(world);
    return NULL;
}

void ExclusiveAccess_other_world_fini(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_fini, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_bulk_init(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_bulk_init(world, &(ecs_bulk_desc_t) {
        .count = 3
    });
    return NULL;
}

void ExclusiveAccess_other_world_bulk_init(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_bulk_init, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_get(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_mut(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_mut_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_get_mut(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_mut, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_target(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_target(world, thr_entity, EcsChildOf, 0);
    return NULL;
}

void ExclusiveAccess_other_world_get_target(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_target, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_target_for(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_target_for_id(world, thr_entity, EcsChildOf, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_get_target_for(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_target_for, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_parent(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_parent(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_world_get_parent(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_parent, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_ref_init(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_ref_init_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_ref_init(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ref_init, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_ref_get(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_ref_get_id(world, &thr_ref, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_ref_get(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_ref = ecs_ref_init(world, ecs_new(world), Position);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ref_get, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_has(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_has_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_has(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_has, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_owns(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_owns_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_world_owns(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_owns, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_disable_component(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_enable_id(world, thr_entity, thr_component, false);
    return NULL;
}

void ExclusiveAccess_other_disable_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new_w(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_disable_component, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_is_component_enabled(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_is_enabled_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_other_is_component_enabled(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new_w(world, Position);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_component_enabled, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_set_child_order(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_set_child_order(world, thr_entity, NULL, 0);
    return NULL;
}

void ExclusiveAccess_other_set_child_order(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set_child_order, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_depth(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_depth(world, thr_entity, EcsChildOf);
    return NULL;
}

void ExclusiveAccess_other_get_depth(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_depth, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_is_valid(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_is_valid(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_is_valid(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_valid, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_is_alive(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_is_alive(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_is_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_alive, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_alive(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_alive(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_get_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_alive, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_make_alive(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_make_alive(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_make_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_make_alive, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_exists(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_exists(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_exists(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_exists, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_set_version(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_set_version(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_set_version(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set_version, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_table(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_table(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_get_table(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_table, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_type(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_type(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_get_type(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_type, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_name(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    ecs_get_name(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_other_get_name(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_name, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_path(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort();
    char *r = ecs_get_path(world, thr_entity);
    ecs_os_free(r);
    return NULL;
}

void ExclusiveAccess_other_get_path(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_path, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_new(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_new, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_world_new_low_id(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_new_low_id, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}


void ExclusiveAccess_locked_delete(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_delete, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_clear(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_clear, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_add(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_add, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_add_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_add, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_remove(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_remove, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_remove_non_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_remove, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_set(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_set_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_ensure(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ensure, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_ensure_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ensure, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_emplace(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_emplace, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_emplace_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);
    thr_component = ecs_id(Position);

    ecs_add(world, thr_entity, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_emplace, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_defer_begin(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_defer_begin, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_defer_end(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world


    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_defer_end, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_create_query_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ thr_component, .inout = EcsIn }}
    });
    ecs_query_fini(q);
    return NULL;
}

void ExclusiveAccess_locked_create_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_query_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_create_mut_query_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ thr_component }}
    });
    ecs_query_fini(q);
    return NULL;
}

void ExclusiveAccess_locked_create_mut_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_mut_query_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_create_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_cached_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_iter_query_locked(void *arg) {
    ecs_world_t *world = arg;
    test_expect_abort(); // Iterator requires stack allocator from stage
    ecs_query_iter(world, thr_query);
    // ecs_iter_fini(&it); 
    return NULL;
}

void ExclusiveAccess_locked_iter_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position), .inout = EcsIn }}
    });

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query_locked, world);

    ecs_os_thread_join(thr);

    ecs_query_fini(thr_query);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_iter_mut_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position) }}
    });

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_iter_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position), .inout = EcsIn }},
        .cache_kind = EcsQueryCacheAll
    });

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query_locked, world);

    ecs_os_thread_join(thr);

    ecs_exclusive_access_end(world, false);

    ecs_query_fini(thr_query);

    test_assert(true);

    ecs_fini(world);
}

void ExclusiveAccess_locked_iter_mut_cached_query(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_query = ecs_query(world, { 
        .terms = {{ ecs_id(Position) }},
        .cache_kind = EcsQueryCacheAll
    });

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_query, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_iter_children_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_children(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_iter_children(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_iter_children_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_each(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_each, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_create_table(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_create_table, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_register_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_register_component, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_world_fini(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_fini, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_locked_world_bulk_init(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_bulk_init, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_get(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_world_get_mut(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_mut, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_target_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_target(world, thr_entity, EcsChildOf, 0);
    return NULL;
}

void ExclusiveAccess_locked_world_get_target(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_target_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_target_for_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_target_for_id(world, thr_entity, EcsChildOf, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_get_target_for(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_target_for_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_parent_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_parent(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_world_get_parent(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_parent_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_ref_init_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_ref_init_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_ref_init(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ref_init_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_ref_get_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_ref_get_id(world, &thr_ref, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_ref_get(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_ref = ecs_ref_init(world, ecs_new(world), Position);
    thr_component = ecs_id(Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_ref_get_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_has_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_has_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_has(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_has_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_has_owns_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_owns_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_world_owns(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_has_owns_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_disable_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new_w(world, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_disable_component, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_is_component_enabled_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_is_enabled_id(world, thr_entity, thr_component);
    return NULL;
}

void ExclusiveAccess_locked_is_component_enabled(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new_w(world, Position);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_component_enabled_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_set_child_order(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set_child_order, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_depth_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_depth(world, thr_entity, EcsChildOf);
    return NULL;
}

void ExclusiveAccess_locked_get_depth(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_depth_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_is_valid_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_is_valid(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_is_valid(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_valid_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_is_alive_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_is_alive(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_is_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_is_alive_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_alive_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_alive(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_get_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_alive_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_make_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_make_alive, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_exists_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_exists(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_exists(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_exists_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_locked_set_version(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_set_version, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

static
void* thread_exclusive_access_other_get_table_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_table(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_get_table(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_table_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_type_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_type(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_get_type(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_type_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_name_locked(void *arg) {
    ecs_world_t *world = arg;
    ecs_get_name(world, thr_entity);
    return NULL;
}

void ExclusiveAccess_locked_get_name(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_name_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

static
void* thread_exclusive_access_other_get_path_locked(void *arg) {
    ecs_world_t *world = arg;
    char *str = ecs_get_path(world, thr_entity);
    ecs_os_free(str);
    return NULL;
}

void ExclusiveAccess_locked_get_path(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, NULL);
    ecs_exclusive_access_end(world, true); // lock world

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get_path_locked, world);

    ecs_os_thread_join(thr);

    test_assert(true);

    ecs_exclusive_access_end(world, false); // unlock world

    ecs_fini(world);
}

void ExclusiveAccess_other_w_name_read(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    thr_component = ecs_id(Position);
    thr_entity = ecs_new(world);

    ecs_exclusive_access_begin(world, "foo_thread");

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_get, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}

void ExclusiveAccess_other_w_name_write(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_exclusive_access_begin(world, "foo_thread");

    thr_entity = ecs_new(world);

    ecs_os_thread_t thr = 
        ecs_os_thread_new(thread_exclusive_access_other_delete, world);

    ecs_os_thread_join(thr);

    test_assert(false); // should not get here
}
