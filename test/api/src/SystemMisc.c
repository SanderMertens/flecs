#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort = test_abort;
    ecs_os_set_api(&os_api);
}

static
void Dummy(ecs_rows_t *rows) {
    // ...
}

void SystemMisc_invalid_not_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !);

    ecs_fini(world);
}

void SystemMisc_invalid_optional_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, ?);

    ecs_fini(world);
}

void SystemMisc_invalid_system_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM.);

    ecs_fini(world);
}

void SystemMisc_invalid_container_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CONTAINER.);

    ecs_fini(world);
}

void SystemMisc_invalid_cascade_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CASCADE.);

    ecs_fini(world);
}

void SystemMisc_invalid_entity_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, FOO.);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, .);

    ecs_fini(world);
}

void SystemMisc_invalid_singleton_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, $.);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_element() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate,, Position);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_element_w_space() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, , Position);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_or() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, | Position);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_or_w_space() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate,| Position);

    ecs_fini(world);
}

void SystemMisc_invalid_or_w_not() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position | !Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_not_w_or() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !Position | Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_and() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, 0, Position);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_system() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM.0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_container() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CONTAINER.0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_cascade() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CASCASDE.0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_entity() {
    install_test_abort();

    ecs_world_t *world = ecs_init();
    
    ECS_ENTITY(world, Foo, 0);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Foo.0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_empty() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, .0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_singleton() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, $.0);

    ecs_fini(world);
}

void SystemMisc_invalid_or_w_empty() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, .Position | .Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_phase() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, -1, 0);

    ecs_fini(world);
}

void SystemMisc_invalid_component_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_fini(world);
}

void SystemMisc_invalid_entity_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Foo.Position);

    ecs_fini(world);
}

void SystemMisc_invalid_or_from_system() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM.Position | SYSTEM.Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_null_string() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ecs_new_system(world, "Dummy", EcsOnUpdate, NULL, Dummy);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ecs_new_system(world, "Dummy", EcsOnUpdate, "", Dummy);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string_w_space() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ecs_new_system(world, "Dummy", EcsOnUpdate, "  ", Dummy);

    ecs_fini(world);
}

void SystemMisc_redefine_row_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s;
    {
        ECS_SYSTEM(world, Dummy, EcsOnAdd, Position);
        s = Dummy;
    }

    ECS_SYSTEM(world, Dummy, EcsOnAdd, Position);

    test_assert(s == Dummy);

    ecs_fini(world);
}

static int is_invoked;

static
void IsInvoked(ecs_rows_t *rows) {
    is_invoked ++;
}

void SystemMisc_system_w_or_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab | EcsDisabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_system_w_or_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Entity, Position, EcsDisabled);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab | EcsDisabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_system_w_or_disabled_and_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);
    ECS_ENTITY(world, Entity, Position, EcsDisabled);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab | EcsDisabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 2);
    is_invoked = false;

    ecs_fini(world);
}

static
void TableColumns(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    ecs_type_t type = ecs_table_type(rows);
    test_int(2, ecs_vector_count(type));

    ecs_entity_t *components = ecs_vector_first(type);
    test_int(components[0], ecs_entity(Position));
    test_int(components[1], ecs_entity(Velocity));

    void *column_0 = ecs_table_column(rows, 0);
    test_assert(column_0 == p);

    void *column_1 = ecs_table_column(rows, 1);
    test_assert(column_1 == v);

    is_invoked ++;
}

void SystemMisc_table_columns_access() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ECS_SYSTEM(world, TableColumns, EcsOnUpdate, Position, Velocity);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);
    is_invoked = false;

    ecs_fini(world);
}

static bool system_status_action_invoked = false;
static ecs_system_status_t enable_status = EcsSystemStatusNone;
static ecs_system_status_t active_status = EcsSystemStatusNone;

static
void status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx)
{
    system_status_action_invoked = true;

    if (status == EcsSystemEnabled || status == EcsSystemDisabled) {
        enable_status = status;
    } else if (status == EcsSystemActivated || status == EcsSystemDeactivated) {
        active_status = status;
    }
}

static
void reset_status() {
    system_status_action_invoked = false;
    enable_status = EcsSystemStatusNone;
    active_status = EcsSystemStatusNone;
}

void SystemMisc_status_enable_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. There are no entities, so the system
     * should not be active */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Enable enabled system. Should not trigger status. */
    reset_status();
    ecs_enable(world, Dummy, true);
    test_assert(system_status_action_invoked == false);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemStatusNone); 

    /* After cleaning up the world, the Disabled status should have been
     * triggered. There were no entities, so the Deactivated status shouldn't */
    reset_status();
    ecs_fini(world);

    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);   
}

void SystemMisc_status_enable_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. There are no entities, so the system
     * should not be active */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Disable system, should trigger status */
    reset_status();
    ecs_enable(world, Dummy, false);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Enable enabled system. Should trigger status. */
    reset_status();
    ecs_enable(world, Dummy, true);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* After cleaning up the world, the Disabled status should have been
     * triggered. There were no entities, so the Deactivated status shouldn't */
    reset_status();
    ecs_fini(world);

    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);  
}

void SystemMisc_status_disable_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. There are no entities, so the system
     * should not be active */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Disable system, should trigger status action */
    reset_status();
    ecs_enable(world, Dummy, false);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Since the system is already disabled, no status should be triggered */
    reset_status();
    ecs_fini(world);

    test_assert(system_status_action_invoked == false);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemStatusNone);  
}

void SystemMisc_status_disable_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. There are no entities, so the system
     * should not be active */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Disable system, should trigger status action */
    reset_status();
    ecs_enable(world, Dummy, false);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Disable system again, should not trigger status action */
    reset_status();
    ecs_enable(world, Dummy, false);
    test_assert(system_status_action_invoked == false);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemStatusNone);    

    /* Since the system is already disabled, no status should be triggered */
    ecs_fini(world);

    test_assert(system_status_action_invoked == false);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemStatusNone);  
}

void SystemMisc_status_activate_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. There are no entities, so the system
     * should not be active */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    /* Add entity with Position. Should activate system. */
    reset_status();
    ecs_new(world, Position);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemActivated);

    /* After cleaning up the world, the Disabled and Deactivated status should 
     * have been triggered. */
    reset_status();
    ecs_fini(world);

    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemDeactivated);
}

void SystemMisc_status_deactivate_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_set_system_status_action(world, Dummy, status_action, NULL);

    /* Setting the status action should have triggered the enabled status since
     * the system is already enabled. The system should be active since it has
     * been matched with an entity. */
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemActivated);

    /* Delete the entity. Should trigger the Deactivated status */
    reset_status();
    ecs_delete(world, e);
    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemDeactivated);

    /* After cleaning up the world, the Disabled status should have been
     * triggered. */
    reset_status();
    ecs_fini(world);

    test_assert(system_status_action_invoked == true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemStatusNone);
}
