#include <safety_locks.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int32_t x, y;
} Position;

typedef struct {
    int32_t x, y;
} Velocity;

// Test single-threaded lock operations without conflicts
void test_singlethreaded_no_conflicts(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_table_t *table = ecs_get_table(world, e);
    int16_t pos_column = (int16_t)ecs_table_get_column_index(world, table, ecs_id(Position));
    ecs_component_record_t *cr = flecs_components_get(world, ecs_id(Position));
    
    bool result;
    
    // Table column locks - basic operations
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_read_end(table, pos_column);
    assert(result == false);
    
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_write_end(table, pos_column);
    assert(result == false);
    
    // Multiple reads should work
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == false);
    flecs_table_column_lock_read_end(table, pos_column);
    flecs_table_column_lock_read_end(table, pos_column);
    
    // Sparse component record locks - basic operations
    result = flecs_sparse_id_record_lock_read_begin(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_read_end(cr);
    assert(result == false);
    
    result = flecs_sparse_id_record_lock_write_begin(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_write_end(cr);
    assert(result == false);
    
    // Multiple sparse reads should work
    result = flecs_sparse_id_record_lock_read_begin(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_read_begin(cr);
    assert(result == false);
    flecs_sparse_id_record_lock_read_end(cr);
    flecs_sparse_id_record_lock_read_end(cr);
    
    ecs_fini(world);
}

// Test single-threaded lock operations with conflicts
void test_singlethreaded_with_conflicts(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_table_t *table = ecs_get_table(world, e);
    int16_t pos_column = (int16_t)ecs_table_get_column_index(world, table, ecs_id(Position));
    ecs_component_record_t *cr = flecs_components_get(world, ecs_id(Position));
    
    bool result;
    
    // Table column locks - read/write conflicts
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == true); // Should conflict
    flecs_table_column_lock_write_end(table, pos_column);
    flecs_table_column_lock_read_end(table, pos_column);
    
    // Write/write conflicts
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == true); // Should conflict
    flecs_table_column_lock_write_end(table, pos_column);
    flecs_table_column_lock_write_end(table, pos_column);
    
    // Write/read conflicts
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == false);
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == true); // Should conflict
    flecs_table_column_lock_read_end(table, pos_column);
    flecs_table_column_lock_write_end(table, pos_column);
    
    // Sparse component record locks - read/write conflicts
    result = flecs_sparse_id_record_lock_read_begin(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_write_begin(cr);
    assert(result == true); // Should conflict
    flecs_sparse_id_record_lock_write_end(cr);
    flecs_sparse_id_record_lock_read_end(cr);
    
    // Sparse write/write conflicts
    result = flecs_sparse_id_record_lock_write_begin(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_write_begin(cr);
    assert(result == true); // Should conflict
    flecs_sparse_id_record_lock_write_end(cr);
    flecs_sparse_id_record_lock_write_end(cr);
    
    ecs_fini(world);
}

// Test multithreaded lock operations without conflicts (stage independence)
void test_multithreaded_no_conflicts(void) {
    ecs_world_t *world = ecs_init();
    ecs_set_stage_count(world, 3);
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_table_t *table = ecs_get_table(world, e);
    int16_t pos_column = (int16_t)ecs_table_get_column_index(world, table, ecs_id(Position));
    ecs_component_record_t *cr = flecs_components_get(world, ecs_id(Position));
    
    bool result;
    
    // Different stages should not conflict - table column locks
    result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, 0);
    assert(result == false);
    result = flecs_table_column_lock_write_begin_multithreaded(table, pos_column, 1);
    assert(result == false); // Different stage, no conflict
    result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, 2);
    assert(result == false); // Different stage, no conflict
    
    // Multiple reads on same stage should work
    result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, 0);
    assert(result == false);
    
    flecs_table_column_lock_read_end_multithreaded(table, pos_column, 0);
    flecs_table_column_lock_read_end_multithreaded(table, pos_column, 0);
    flecs_table_column_lock_write_end_multithreaded(table, pos_column, 1);
    flecs_table_column_lock_read_end_multithreaded(table, pos_column, 2);
    
    // Sparse component record locks are global - should conflict
    result = flecs_sparse_id_record_lock_read_begin_multithreaded(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_write_begin_multithreaded(cr);
    assert(result == true); // Multithreaded sparse locks are global, should conflict
    
    flecs_sparse_id_record_lock_read_end_multithreaded(cr);
    flecs_sparse_id_record_lock_write_end_multithreaded(cr);
    
    ecs_fini(world);
}

// Test multithreaded lock operations with conflicts (same stage)
void test_multithreaded_with_conflicts(void) {
    ecs_world_t *world = ecs_init();
    ecs_set_stage_count(world, 2);
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_table_t *table = ecs_get_table(world, e);
    int16_t pos_column = (int16_t)ecs_table_get_column_index(world, table, ecs_id(Position));
    ecs_component_record_t *cr = flecs_components_get(world, ecs_id(Position));
    
    bool result;
    
    // Same stage should still have conflicts - table column locks
    result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, 0);
    assert(result == false);
    result = flecs_table_column_lock_write_begin_multithreaded(table, pos_column, 0);
    assert(result == true); // Same stage, should conflict
    flecs_table_column_lock_write_end_multithreaded(table, pos_column, 0);
    flecs_table_column_lock_read_end_multithreaded(table, pos_column, 0);
    
    // Write/write conflicts on same stage
    result = flecs_table_column_lock_write_begin_multithreaded(table, pos_column, 1);
    assert(result == false);
    result = flecs_table_column_lock_write_begin_multithreaded(table, pos_column, 1);
    assert(result == true); // Same stage, should conflict
    flecs_table_column_lock_write_end_multithreaded(table, pos_column, 1);
    flecs_table_column_lock_write_end_multithreaded(table, pos_column, 1);
    
    // Sparse component record locks - conflicts (global scope)
    result = flecs_sparse_id_record_lock_read_begin_multithreaded(cr);
    assert(result == false);
    result = flecs_sparse_id_record_lock_write_begin_multithreaded(cr);
    assert(result == true); // Should conflict globally
    flecs_sparse_id_record_lock_write_end_multithreaded(cr);
    flecs_sparse_id_record_lock_read_end_multithreaded(cr);
    
    ecs_fini(world);
}

// Test stage count resizing and lock array allocation
void test_resizing(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_table_t *table = ecs_get_table(world, e);
    int16_t pos_column = (int16_t)ecs_table_get_column_index(world, table, ecs_id(Position));
    
    bool result;
    
    // Test operations work with initial stage count (1)
    result = flecs_table_column_lock_read_begin(table, pos_column);
    assert(result == false);
    flecs_table_column_lock_read_end(table, pos_column);
    
    // Resize to 2 stages
    ecs_set_stage_count(world, 2);
    result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, 0);
    assert(result == false);
    result = flecs_table_column_lock_write_begin_multithreaded(table, pos_column, 1);
    assert(result == false);
    flecs_table_column_lock_read_end_multithreaded(table, pos_column, 0);
    flecs_table_column_lock_write_end_multithreaded(table, pos_column, 1);
    
    // Resize to 4 stages
    ecs_set_stage_count(world, 4);
    for (int stage = 0; stage < 4; stage++) {
        result = flecs_table_column_lock_read_begin_multithreaded(table, pos_column, stage);
        assert(result == false);
    }
    for (int stage = 0; stage < 4; stage++) {
        flecs_table_column_lock_read_end_multithreaded(table, pos_column, stage);
    }
    
    // Resize back to 1 stage
    ecs_set_stage_count(world, 1);
    result = flecs_table_column_lock_write_begin(table, pos_column);
    assert(result == false);
    flecs_table_column_lock_write_end(table, pos_column);
    
    ecs_fini(world);
}

// 


void test_table_component_safety_info(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    // Get the record and test safety info
    ecs_record_t *r = ecs_record_find(world, e);
    assert(r != NULL);
    
    ecs_get_ptr_t result = flecs_get_id_from_record(world, e, r, ecs_id(Position));
    assert(result.component_ptr != NULL);
    
    // For table components: table should be set, cr should be NULL, column_index should be valid
    assert(result.si.table != NULL);
    assert(result.si.cr == NULL);
    assert(result.si.column_index >= 0);
    
    ecs_fini(world);
}

void test_sparse_fragmenting_safety_info(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsSparse);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {5, 10});
    
    ecs_record_t *r = ecs_record_find(world, e);
    assert(r != NULL);
    
    ecs_get_ptr_t result = flecs_get_id_from_record(world, e, r, ecs_id(Position));
    assert(result.component_ptr != NULL);
    
    // For sparse fragmenting components: cr should be set, table should be NULL, column_index should be -1
    assert(result.si.cr != NULL);
    assert(result.si.table == NULL);
    assert(result.si.column_index == -1);
    
    ecs_fini(world);
}

void test_sparse_non_fragmenting_safety_info(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {15, 25});
    
    ecs_record_t *r = ecs_record_find(world, e);
    assert(r != NULL);
    
    ecs_get_ptr_t result = flecs_get_id_from_record(world, e, r, ecs_id(Position));
    assert(result.component_ptr != NULL);

    // For sparse non-fragmenting components: cr should be set, table should be NULL, column_index should be -1
    assert(result.si.cr != NULL);
    assert(result.si.table == NULL);
    assert(result.si.column_index == -1);
    
    ecs_fini(world);
}

void test_get_vs_get_mut_safety_info(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {15, 25});
    
    ecs_record_t *r = ecs_record_find(world, e);
    assert(r != NULL);
    
    // Test both get and get_mut return same safety info for owned components
    ecs_get_ptr_t get_result = flecs_get_id_from_record(world, e, r, ecs_id(Position));
    ecs_get_ptr_t get_mut_result = flecs_get_mut_id_from_record(world, r, ecs_id(Position));
    
    assert(get_result.component_ptr != NULL);
    assert(get_mut_result.component_ptr != NULL);
    
    // Safety info should be the same for owned components
    assert(get_result.si.table == get_mut_result.si.table);
    assert(get_result.si.cr == get_mut_result.si.cr);
    assert(get_result.si.column_index == get_mut_result.si.column_index);
    
    ecs_fini(world);
}

void test_null_component_safety_info(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world);
    
    ecs_record_t *r = ecs_record_find(world, e);
    assert(r != NULL);
    
    // Test safety info for non-existent component
    ecs_get_ptr_t result = flecs_get_id_from_record(world, e, r, ecs_id(Position));
    
    // Should return null pointer and zeroed safety info
    assert(result.component_ptr == NULL);
    assert(result.si.table == NULL);
    assert(result.si.cr == NULL);
    assert(result.si.column_index == 0);
    
    ecs_fini(world);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    test_singlethreaded_no_conflicts();
    test_singlethreaded_with_conflicts();
    test_multithreaded_no_conflicts();
    test_multithreaded_with_conflicts();
    test_resizing();

    test_table_component_safety_info();
    test_sparse_non_fragmenting_safety_info();
    test_sparse_fragmenting_safety_info();
    test_get_vs_get_mut_safety_info();
    test_null_component_safety_info();

    return 0;
}
