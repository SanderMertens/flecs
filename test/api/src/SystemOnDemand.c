#include <api.h>

static bool invoked = false;
static bool invoked2 = false;

static
void OutSystem(ecs_iter_t *it) {
    invoked = true;
}

static
void OutSystem2(ecs_iter_t *it) {
    invoked2 = true;
}

static
void InSystem(ecs_iter_t *it) {
}

static
void InSystem2(ecs_iter_t *it) {
}

bool is_enabled(
    ecs_world_t *world, 
    ecs_entity_t system) 
{
    return !ecs_has_id(world, system, EcsDisabled) && 
        !ecs_has_id(world, system, EcsDisabledIntern);
}

void SystemOnDemand_enable_out_after_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_out_after_in_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_out_after_in_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_in_after_out_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in_2_out_1_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_disable_after_disable_in_1_out_2_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Create dummy entity so system won't be disabled all the time. Make sure
     * entity matches InSystem as well as otherwise the system won't be active
     * and the OutSystem won't be enabled. */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* Disabling the [in] system should disable the [out] system */
    ecs_enable(world, InSystem, false);
    invoked = false;

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_table_after_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_table_after_out_and_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    /* The InSystem is not active yet since there are no matching entities for
     * it, and therefore OutSystem won't be active either. */
    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_table_after_out_and_in_overlapping_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, Velocity, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Mass);

    /* The InSystem is not active yet since there are no matching entities for
     * it, and therefore OutSystem won't be active either. */
    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    /* Create entity that will activate InSystem but not OutSystem. OutSystem
     * should now be enabled, but it is not active yet. */
    ECS_ENTITY(world, in_entity, Position, Mass);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    /* Create entity that matches the signature of OutType. The system should
     * now be enabled and active. */
    ECS_ENTITY(world, out_entity, Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_1_out_system_2_in_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create entity that will activate InSystems and OutSystem */
    ecs_new(world, Position);    

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);  

    ecs_fini(world);
}

void SystemOnDemand_1_out_system_2_in_systems_different_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create entity that will activate InSystems and OutSystem */
    ECS_ENTITY(world, Dummy, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(is_enabled(world, OutSystem) == false);
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

    /* Create entity that will activate InSystems and OutSystem */
    ECS_ENTITY(world, Dummy, Position, Velocity, Mass);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, [out] Mass, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem,  EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Velocity, [in] Mass);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);    

    ecs_enable(world, InSystem, false);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);  
    
    ecs_enable(world, InSystem2, false);
        
    test_assert(is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);  

    ecs_fini(world);
}

void SystemOnDemand_disable_after_inactive_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_new(world, Position);

    /* Create an entity that when deleted disables the InSystem */
    ECS_ENTITY(world, in_entity, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, which should disable OutSystem */
    ecs_delete(world, in_entity);

    test_assert(is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_disable_after_2_inactive_in_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_new(world, Position);

    /* Create an entity that when deleted disables the InSystem */
    ECS_ENTITY(world, in_entity, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);
    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position, Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, which should disable OutSystem */
    ecs_delete(world, in_entity);

    test_assert(is_enabled(world, OutSystem) == false);
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
    ECS_ENTITY(world, in_entity, Position, Velocity);
    ECS_ENTITY(world, in2_entity, Position, Mass);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);
    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, Velocity);
    ECS_SYSTEM(world, InSystem2, EcsOnUpdate, [in] Position, Mass);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem should now deactivate, but OutSystem should still be enabled */
    ecs_delete(world, in_entity);

    test_assert(is_enabled(world, OutSystem) == true);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* InSystem2 should now deactivate, which should disable OutSystem */
    ecs_delete(world, in2_entity);

    test_assert(is_enabled(world, OutSystem) == false);
    invoked = false;
    ecs_progress(world, 0);
    test_assert(invoked == false);    

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_new(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system_different_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, Dummy, Position, Velocity);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Velocity, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}

void SystemOnDemand_enable_2_output_1_input_system_overlapping_columns() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, Dummy, Position, Velocity, Mass);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, [out] Velocity, SYSTEM:OnDemand);
    ECS_SYSTEM(world, OutSystem2, EcsOnUpdate, [out] Position, [out] Mass, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    test_assert(is_enabled(world, OutSystem2) == false);
    ecs_progress(world, 0);
    test_assert(invoked2 == false);    

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position, [in] Velocity, [in] Mass);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    test_assert(is_enabled(world, OutSystem2) == true);
    ecs_progress(world, 0);
    test_assert(invoked2 == true);

    ecs_fini(world);
}

void SystemOnDemand_out_not_column() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_new(world, Position);

    /* If a system has a NOT column that is also an [out] column, the system
     * explicitly states that it will create the component. Therefore the system
     * will always be enabled if there are enabled input systems, even if those
     * input systems are not active (no matched entities). This way, OutSystem
     * can become enabled even though there are no entities with Velocity yet */
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, Position, [out] !Velocity, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, Position, [in] Velocity);

    /* There are no matching entities with the InSystem, but the OutSystem
     * will be enabled nonetheless, as it explicitly stated it will create the
     * Velocity component. */
    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_trigger_on_manual() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new(world, Position);

    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, 0, [in] Position);

    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_trigger_on_manual_not_column() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_new(world, Position);

    /* If a system has a NOT column that is also an [out] column, the system
     * explicitly states that it will create the component. Therefore the system
     * will always be enabled if there are enabled input systems, even if those
     * input systems are not active (no matched entities). This way, OutSystem
     * can become enabled even though there are no entities with Velocity yet */
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, Position, [out] !Velocity, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, 0, Position, [in] Velocity);

    /* There are no matching entities with the InSystem, but the OutSystem
     * will be enabled nonetheless, as it explicitly stated it will create the
     * Velocity component. */
    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}

void SystemOnDemand_on_demand_task_w_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ECS_ENTITY(world, MyEntity, Position);

    /* If a system has an out column that matches an entity, it should still
     * behave like an OnDemand system, only enabling when there is interest for
     * the component in the [out] column. */
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] MyEntity:Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, 0, [in] Position);

    /* There are no matching entities with the InSystem, but the OutSystem
     * will be enabled nonetheless, as it explicitly stated it will create the
     * Velocity component. */
    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* When Position is removed from MyEntity, the system must be disabled */
    invoked = false;
    ecs_remove(world, MyEntity, Position);
    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_on_demand_task_w_not_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ECS_ENTITY(world, MyEntity, 0);

    /* If a system has an out column that matches an entity, it should still
     * behave like an OnDemand system, only enabling when there is interest for
     * the component in the [out] column, even if that column has a NOT operator */
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] !MyEntity:Position, SYSTEM:OnDemand);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, 0, [in] Position);

    /* There are no matching entities with the InSystem, but the OutSystem
     * will be enabled nonetheless, as it explicitly stated it will create the
     * Velocity component. */
    test_assert(is_enabled(world, OutSystem) == true);
    ecs_progress(world, 0);
    test_assert(invoked == true);

    /* When Position is added to MyEntity, the system should not be ran */
    invoked = false;
    ecs_add(world, MyEntity, Position);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ecs_fini(world);
}

void SystemOnDemand_enable_after_user_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, OutSystem, EcsOnUpdate, [out] Position, SYSTEM:OnDemand);

    /* Explicitly disable system. System should not automatically enable once
     * demand is created */
    ecs_enable(world, OutSystem, false);

    /* Create dummy entity so system won't be disabled all the time */
    ecs_new(world, Position);

    test_assert(is_enabled(world, OutSystem) == false);
    ecs_progress(world, 0);
    test_assert(invoked == false);

    ECS_SYSTEM(world, InSystem, EcsOnUpdate, [in] Position);

    /* System should still be disabled, since user explicitly disabled it */
    test_assert(is_enabled(world, OutSystem) == false);

    /* Explicitly enable system. This should enable the system, since now there
     * is also demand */
    ecs_enable(world, OutSystem, true);
    test_assert(is_enabled(world, OutSystem) == true);

    ecs_progress(world, 0);
    test_assert(invoked == true);

    ecs_fini(world);
}
