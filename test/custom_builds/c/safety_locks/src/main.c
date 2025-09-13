#include <safety_locks.h>
#include <stdio.h>

// Test component type
typedef struct {
    int32_t x, y;
} Position;

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
    test_table_component_safety_info();
    test_sparse_non_fragmenting_safety_info();
    test_sparse_fragmenting_safety_info();
    test_get_vs_get_mut_safety_info();
    test_null_component_safety_info();

    printf("safety info tests passed\n");
    
    return 0;
}
