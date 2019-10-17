#include <api.h>

static bool invoked = false;
static bool invoked2 = false;

static
void OutSystem(ecs_rows_t *rows) {
    invoked = true;
}

static
void OutSystem2(ecs_rows_t *rows) {
    invoked2 = true;
}

static
void InSystem(ecs_rows_t *rows) {
}

static
void InSystem2(ecs_rows_t *rows) {
}

void SystemOnDemand_enable_out_after_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_out_after_in_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_out_after_in_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ecs_new(world, Type);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_table_after_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    ecs_new(world, Position);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_table_after_out_and_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    /* The InSystem is not active yet since there are no matching entities for
     * it, and therefore OutSystem won't be active either. */
    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_new(world, Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_table_after_out_and_in_overlapping_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, OutType, Position, Velocity);
    ECS_TYPE(world, InType, Position, Mass);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, Velocity, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Mass);

    /* The InSystem is not active yet since there are no matching entities for
     * it, and therefore OutSystem won't be active either. */
    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    /* Create entity that will activate InSystem but not OutSystem. OutSystem
     * should now be enabled, but it is not active yet. */
    ecs_new(world, InType);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    /* Create entity that matches the signature of OutType. The system should
     * now be enabled and active. */
    ecs_new(world, OutType);     

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_1_out_system_2_in_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create entity that will activate InSystems and OutSystem */
    ecs_new(world, Position);    

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);  

    ecs_fini(world);
}

void SystemOnDemand_1_out_system_2_in_systems_different_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    /* Create entity that will activate InSystems and OutSystem */
    ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);  

    ecs_fini(world);
}

void SystemOnDemand_1_out_system_2_in_systems_overlapping_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    /* Create entity that will activate InSystems and OutSystem */
    ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, [out] Mass, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Velocity, [in] Mass);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);  

    ecs_fini(world);
}

void SystemOnDemand_disable_after_inactive_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_new(world, Position);

    /* Create an entity that when deleted disables the InSystem */
    ecs_entity_t in_entity = ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, which should disable OutSystem */
    ecs_delete(world, in_entity);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_disable_after_2_inactive_in_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_new(world, Position);

    /* Create an entity that when deleted disables the InSystem */
    ecs_entity_t in_entity = ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);
    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position, Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, which should disable OutSystem */
    ecs_delete(world, in_entity);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_disable_after_2_inactive_in_systems_different_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Position, Mass);

    ecs_new(world, Position);

    /* Create an entity that when deleted disables the InSystem */
    ecs_entity_t in_entity = ecs_new(world, Type1);
    ecs_entity_t in2_entity = ecs_new(world, Type2);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);
    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position, Mass);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, but OutSystem should still be enabled */
    ecs_delete(world, in_entity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem2 should now deactivate, which should disable OutSystem */
    ecs_delete(world, in2_entity);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_new(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(ecs_is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(ecs_is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system_different_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Velocity, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(ecs_is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(ecs_is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system_overlapping_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    ecs_new(world, Type);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM.EcsOnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Position, [out] Mass, SYSTEM.EcsOnDemand);

    test_assert(ecs_is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(ecs_is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity, [in] Mass);

    test_assert(ecs_is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(ecs_is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}
