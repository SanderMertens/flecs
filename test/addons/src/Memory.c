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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_query_memory_t mem = ecs_queries_memory_get(world);
    
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
    ecs_misc_memory_t mem = ecs_misc_memory_get(world);
    
    /* Initial state should have minimal memory usage */
    ecs_size_t initial_commands = mem.bytes_commands;
    test_assert(mem.bytes_commands >= 0);

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
    mem = ecs_misc_memory_get(world);
    
    test_assert(mem.bytes_commands >= initial_commands); /* Should have command queue data */
    test_assert(mem.bytes_commands > initial_commands);

    ecs_defer_end(world);
    
    /* Get statistics after flush - some memory might be retained for reuse */
    mem = ecs_misc_memory_get(world);
    
    test_assert(mem.bytes_commands >= 0);

    /* Test with multiple defer levels */
    ecs_defer_begin(world);
    ecs_defer_begin(world);
    
    ecs_entity_t nested_e = ecs_new(world);
    ecs_set(world, nested_e, Position, {100.0f, 200.0f});
    
    mem = ecs_misc_memory_get(world);
    test_assert(mem.bytes_commands >= 0);
    
    ecs_defer_end(world);
    ecs_defer_end(world);

    ecs_fini(world);
}

void Memory_table_memory_histogram(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    /* Get initial histogram (should have some empty tables) */
    ecs_table_histogram_t hist = ecs_table_histogram_get(world);
    
    /* Initially, all buckets should be zero or have minimal values */
    int32_t initial_total = 0;
    for (int i = 0; i < ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT; i++) {
        initial_total += hist.entity_counts[i];
        test_assert(hist.entity_counts[i] >= 0);
    }
    
    /* Create tables with different entity counts to test histogram buckets */
    
    /* Bucket 0: Empty table (0 entities) - create a table but don't populate it */
    ecs_entity_t empty_archetype = ecs_new_w(world, Position);
    ecs_delete(world, empty_archetype); /* This creates an empty table */
    
    /* Bucket 1: 1 entity */
    ecs_entity_t e1 = ecs_new_w(world, Position);
    (void)e1;
    
    /* Bucket 2: 2-3 entities */
    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_entity_t e4 = ecs_new(world);
    ecs_add(world, e4, Position);
    ecs_add(world, e4, Velocity);
    
    /* Bucket 3: 4-7 entities */
    for (int i = 0; i < 5; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add(world, e, Position);
        ecs_add(world, e, TagA);
    }
    
    /* Bucket 4: 8-15 entities */
    for (int i = 0; i < 10; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add(world, e, Position);
        ecs_add(world, e, TagB);
    }
    
    /* Bucket 5: 16-31 entities */
    for (int i = 0; i < 20; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add(world, e, Position);
        ecs_add(world, e, TagC);
    }
    
    /* Get histogram after creating entities */
    hist = ecs_table_histogram_get(world);
    
    /* Verify that we have tables in the expected buckets */
    /* Note: There might be additional internal tables, so we check for >= instead of == */
    
    /* Should have at least one table with 1 entity */
    test_assert(hist.entity_counts[1] >= 1);
    
    /* Should have at least one table with 2-3 entities */
    test_assert(hist.entity_counts[2] >= 1);
    
    /* Should have at least one table with 4-7 entities */
    test_assert(hist.entity_counts[3] >= 1);
    
    /* Should have at least one table with 8-15 entities */
    test_assert(hist.entity_counts[4] >= 1);
    
    /* Should have at least one table with 16-31 entities */
    test_assert(hist.entity_counts[5] >= 1);
    
    /* Verify total count increased */
    int32_t final_total = 0;
    for (int i = 0; i < ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT; i++) {
        final_total += hist.entity_counts[i];
    }
    test_assert(final_total >= initial_total);
    
    /* Test edge case: Create a table with maximum entities for a specific bucket */
    /* Create exactly 32 entities to test bucket boundary (should go to bucket 6: 32-63) */
    for (int i = 0; i < 32; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add(world, e, Velocity);
        ecs_add(world, e, TagA);
    }
    
    hist = ecs_table_histogram_get(world);
    
    /* Should have at least one table with 32-63 entities (bucket 6) */
    test_assert(hist.entity_counts[6] >= 1);
    
    /* Verify all bucket counts are non-negative */
    for (int i = 0; i < ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT; i++) {
        test_assert(hist.entity_counts[i] >= 0);
    }

    ecs_fini(world);
}

void Memory_sparse_component_memory(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components == 0);
    }

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components > 0);
    }

    ecs_new_w(world, Position);

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components > 0);
    }

    ecs_fini(world);
}

void Memory_sparse_tag_memory(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components == 0);
    }

    ecs_add_id(world, ecs_id(Foo), EcsSparse);

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components > 0);
    }

    ecs_add_id(world, Foo, EcsSparse);

    ecs_new_w(world, Foo);

    {
        ecs_component_memory_t mem = ecs_component_memory_get(world);
        test_assert(mem.bytes_sparse_components > 0);
    }

    ecs_fini(world);
}
