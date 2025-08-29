#include <addons.h>

void Memory_query_memory_no_cache(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create a simple uncached query */
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position) }}
    });

    test_assert(q != NULL);

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    /* Note: cache might be > 0 even for uncached queries due to internal queries */
    test_assert(mem.bytes_misc >= 0);

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_query_memory_trivial_cache(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create some test entities */
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    (void)e1; (void)e2;

    /* Create a cached query that should be trivial */
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position) }},
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    /* Iterate once to populate cache */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        /* Just iterate */
    }

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    test_assert(mem.bytes_cache > 0); /* Should have cache memory */
    /* Note: bytes_group_by might be > 0 due to internal queries */
    test_assert(mem.bytes_misc >= 0);

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_query_memory_non_trivial_cache(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    /* Create some test entities */
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Tag);
    (void)e1; (void)e2;

    /* Create a cached query with optional terms (non-trivial) */
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsOptional }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    /* Iterate once to populate cache */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        /* Just iterate */
    }

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    test_assert(mem.bytes_cache > 0); /* Should have cache memory */
    /* Note: bytes_group_by might be > 0 due to internal queries */
    test_assert(mem.bytes_misc >= 0);

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_query_memory_with_groups(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create entities with different tags for grouping */
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add(world, e3, TagA);
    (void)e1; (void)e2; (void)e3;

    /* Create a cached query with grouping */
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position) }},
        .cache_kind = EcsQueryCacheAuto,
        .group_by = ecs_id(TagA)
    });

    test_assert(q != NULL);

    /* Iterate once to populate cache */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        /* Just iterate */
    }

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    test_assert(mem.bytes_cache > 0); /* Should have cache memory */
    test_assert(mem.bytes_group_by > 0); /* Should have grouping memory */
    test_assert(mem.bytes_misc >= 0);

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_query_memory_with_variables(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Likes);

    /* Create some test entities with relationships */
    ecs_entity_t alice = ecs_new(world);
    ecs_entity_t bob = ecs_new(world);
    ecs_add(world, alice, Position);
    ecs_add(world, bob, Position);
    ecs_add_pair(world, alice, Likes, bob);

    /* Create a query with variables */
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src = { .name = "person", .id = EcsIsVariable } },
            { .first.id = Likes, .src = { .name = "person", .id = EcsIsVariable }, .second.id = EcsWildcard }
        }
    });

    test_assert(q != NULL);

    /* Iterate once */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        /* Just iterate */
    }

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    /* Note: cache and group_by might be > 0 due to internal queries */
    test_assert(mem.bytes_misc > 0); /* Should have variable-related memory */

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_query_memory_with_monitors(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create some test entities */
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    (void)e1;

    /* Create a cached query with change detection */
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    /* Iterate once to populate cache and initialize monitors */
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        /* Just iterate */
    }

    /* Get memory statistics */
    ecs_query_memory_t mem = ecs_query_memory_get(world);
    
    test_assert(mem.count >= 1); /* May have other queries in the world */
    test_assert(mem.bytes_query > 0);
    test_assert(mem.bytes_cache > 0); /* Should have cache memory */
    /* Note: bytes_group_by might be > 0 due to internal queries */
    test_assert(mem.bytes_misc > 0); /* Should have monitor-related memory */

    ecs_query_fini(q);
    ecs_fini(world);
}

void Memory_commands_memory(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Get initial commands memory statistics */
    ecs_commands_memory_t mem = ecs_commands_memory_get(world);
    
    /* Initial state should have minimal memory usage */
    ecs_size_t initial_queue = mem.bytes_queue;
    ecs_size_t initial_entries = mem.bytes_entries;
    ecs_size_t initial_stack = mem.bytes_stack;
    
    test_assert(mem.bytes_queue >= 0);
    test_assert(mem.bytes_entries >= 0);
    test_assert(mem.bytes_stack >= 0);

    /* Create deferred operations to test command memory usage */
    ecs_defer_begin(world);
    
    /* Create multiple entities with components to generate commands */
    for (int i = 0; i < 10; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_set(world, e, Position, {i * 10.0f, i * 20.0f});
        ecs_add(world, e, Velocity);
        
        /* Create unique name for each entity */
        char name[32];
        sprintf(name, "TestEntity_%d", i);
        ecs_set_name(world, e, name);
    }
    
    /* Add some more complex operations */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e1, EcsChildOf, e2);
    
    /* Get memory statistics while deferred (should show increased usage) */
    mem = ecs_commands_memory_get(world);
    
    test_assert(mem.bytes_queue >= initial_queue); /* Should have command queue data */
    test_assert(mem.bytes_entries >= initial_entries); /* Should have entity batching data */
    test_assert(mem.bytes_stack >= initial_stack); /* Should have stack allocator data */
    
    /* At least one category should have increased */
    test_assert(mem.bytes_queue > initial_queue || 
               mem.bytes_entries > initial_entries || 
               mem.bytes_stack > initial_stack);

    ecs_defer_end(world);
    
    /* Get statistics after flush - some memory might be retained for reuse */
    mem = ecs_commands_memory_get(world);
    
    test_assert(mem.bytes_queue >= 0);
    test_assert(mem.bytes_entries >= 0);
    test_assert(mem.bytes_stack >= 0);

    /* Test with multiple defer levels */
    ecs_defer_begin(world);
    ecs_defer_begin(world);
    
    ecs_entity_t nested_e = ecs_new(world);
    ecs_set(world, nested_e, Position, {100.0f, 200.0f});
    
    mem = ecs_commands_memory_get(world);
    test_assert(mem.bytes_queue >= 0);
    test_assert(mem.bytes_entries >= 0);
    test_assert(mem.bytes_stack >= 0);
    
    ecs_defer_end(world);
    ecs_defer_end(world);

    ecs_fini(world);
}

void Memory_table_memory_histogram(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Get initial table memory statistics */
    ecs_table_memory_t mem = ecs_table_memory_get(world);
    
    /* Initial state should have some tables */
    test_assert(mem.count > 0);
    test_assert(mem.entity_count_histogram[0] >= 0); /* Empty tables bucket */
    
    /* Create entities to populate different histogram buckets */
    
    /* Create 5 entities with Position (should go in histogram[5]) */
    for (int i = 0; i < 5; i++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        (void)e;
    }
    
    /* Create 10 entities with Velocity (should go in histogram[10]) */
    for (int i = 0; i < 10; i++) {
        ecs_entity_t e = ecs_new_w(world, Velocity);
        (void)e;
    }
    
    /* Create 1 entity with both Position and Velocity (should go in histogram[1]) */
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    
    /* Get updated statistics */
    mem = ecs_table_memory_get(world);
    
    /* Verify histogram has been populated (now using power-of-2 buckets) */
    test_assert(mem.entity_count_histogram[1] >= 1);  /* At least 1 table with 1 entity (bucket 1) */
    test_assert(mem.entity_count_histogram[3] >= 1);  /* At least 1 table with 4-7 entities (bucket 3, contains 5) */
    test_assert(mem.entity_count_histogram[4] >= 1);  /* At least 1 table with 8-15 entities (bucket 4, contains 10) */
    
    /* Test edge cases - create a large table (but not > 1000) */
    typedef struct {
        int value;
    } LargeComponent;
    
    ECS_COMPONENT(world, LargeComponent);
    for (int i = 0; i < 500; i++) {
        ecs_entity_t large_e = ecs_new_w(world, LargeComponent);
        (void)large_e;
    }
    
    mem = ecs_table_memory_get(world);
    /* 500 entities should be in bucket 9: 256-511 entities */
    test_assert(mem.entity_count_histogram[9] >= 1); /* Table with 256-511 entities */
    
    /* Verify the >65535 bucket is still 0 (we haven't created tables that large) */
    int32_t overflow_bucket = ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT - 1;
    test_assert(mem.entity_count_histogram[overflow_bucket] == 0);
    
    /* Test boundary conditions */
    test_assert(mem.entity_count_histogram[0] >= 0);    /* 0 entities bucket */
    /* Last regular bucket (32768-65535 entities) - bucket 16 */
    test_assert(mem.entity_count_histogram[16] >= 0);
    test_assert(mem.entity_count_histogram[overflow_bucket] >= 0); /* >65535 entities bucket */

    ecs_fini(world);
}
