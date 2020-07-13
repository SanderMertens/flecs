#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort = test_abort;
    ecs_os_set_api(&os_api);
    ecs_tracing_enable(-2);
}

static
bool dummy_invoked = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_invoked = true;
    probe_system(it);
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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM:);

    ecs_fini(world);
}

void SystemMisc_invalid_container_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, PARENT:);

    ecs_fini(world);
}

void SystemMisc_invalid_cascade_without_id() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CASCADE:);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, $:);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, || Position);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_or_w_space() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate,|| Position);

    ecs_fini(world);
}

void SystemMisc_invalid_or_w_not() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position || !Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_not_w_or() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !Position || Velocity);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM:0);

    ecs_fini(world);
}

void SystemMisc_invalid_0_w_from_container() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, PARENT:0);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, :Position || :Velocity);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Foo:Position);

    ecs_fini(world);
}

void SystemMisc_invalid_or_from_system() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, SYSTEM:Position || SYSTEM:Velocity);

    ecs_fini(world);
}

void SystemMisc_invalid_null_string() {
    ecs_world_t *world = ecs_init();

    ecs_new_system(world, 0, "Dummy", EcsOnUpdate, NULL, Dummy);

    ecs_progress(world, 0);

    test_assert(dummy_invoked == true);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string() {
    ecs_world_t *world = ecs_init();

    ecs_new_system(world, 0, "Dummy", EcsOnUpdate, "", Dummy);

    ecs_progress(world, 0);

    test_assert(dummy_invoked == true);    

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string_w_space() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ecs_new_system(world, 0, "Dummy", EcsOnUpdate, "  ", Dummy);

    ecs_fini(world);
}

void SystemMisc_redefine_row_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s;
    {
        ECS_TRIGGER(world, Dummy, EcsOnAdd, Position);
        s = Dummy;
    }

    ECS_TRIGGER(world, Dummy, EcsOnAdd, Position);

    test_assert(s == Dummy);

    ecs_fini(world);
}

static int is_invoked;

static
void IsInvoked(ecs_iter_t *it) {
    is_invoked ++;
}

void SystemMisc_system_w_or_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab || EcsDisabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_system_w_or_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Entity, Position, EcsDisabled);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab || EcsDisabled);

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

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, EcsPrefab || EcsDisabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 2);
    is_invoked = false;

    ecs_fini(world);
}

static
void TableColumns(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    ecs_type_t type = ecs_iter_type(it);
    test_int(2, ecs_vector_count(type));

    ecs_entity_t *components = ecs_vector_first(type, ecs_entity_t);
    test_int(components[0], ecs_entity(Position));
    test_int(components[1], ecs_entity(Velocity));

    void *column_0 = ecs_table_column(it, 0);
    test_assert(column_0 == p);

    void *column_1 = ecs_table_column(it, 1);
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

void SystemMisc_dont_enable_after_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, ANY:Position, ANY:Velocity);

    /* Create an entity that is watched. Whenever components are added/removed
     * to and/or from watched entities, a rematch is triggered. */
    ECS_PREFAB(world, Prefab, Position);

    ECS_ENTITY(world, Entity, INSTANCEOF | Prefab);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* System is enabled but doesn't match with any entities */
    
    test_bool(ecs_has_entity(world, Dummy, EcsDisabled), false);
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    /* Explicitly disable system before triggering a rematch */
    ecs_enable(world, Dummy, false);
    test_bool(ecs_has_entity(world, Dummy, EcsDisabled), true);

    /* Trigger a rematch. System should still be disabled after this */
    ecs_add(world, Prefab, Velocity);
    test_bool(ecs_has_entity(world, Dummy, EcsDisabled), true);

    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    /* Enable system. It is matched, so should now be invoked */
    ecs_enable(world, Dummy, true);
    test_bool(ecs_has_entity(world, Dummy, EcsDisabled), false);

    ecs_progress(world, 1);
    test_int(ctx.count, 1);

    ecs_fini(world);
}

static void SysA(ecs_iter_t *it)
{
    ECS_COLUMN_COMPONENT(it, Velocity, 2);
    ecs_add(it->world, it->entities[0], Velocity);
}

static int b_invoked;
static ecs_entity_t b_entity;

static void SysB(ecs_iter_t *it)
{
    b_invoked ++;
    b_entity = it->entities[0];
}

void SystemMisc_ensure_single_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, SysA, EcsOnLoad, Position, !Velocity);
    ECS_SYSTEM(world, SysB, EcsPostLoad, Velocity);

    ECS_ENTITY(world, MyEntity, Position);

    ecs_progress(world, 0);

    test_assert(b_invoked == 1);

    ecs_fini(world);
}

static int test_table_count_invoked;

static void TestTableCount(ecs_iter_t *it) {
    test_int(it->table_count, 2);
    test_int(it->inactive_table_count, 1);
    test_table_count_invoked ++;
}

void SystemMisc_table_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, TestTableCount, EcsOnUpdate, Position);

    // Create two active tables
    ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    // Create one inactive table
    ecs_entity_t e3 = ecs_new(world, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);
    ecs_delete(world, e3);

    ecs_progress(world, 0);

    test_int(test_table_count_invoked, 2);

    ecs_fini(world);
}

void SystemMisc_match_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, SysA, 0, SYSTEM:Position);
    ECS_SYSTEM(world, SysB, 0, Position);

    ecs_run(world, SysB, 0, NULL);

    test_assert(b_invoked != 0);
    test_assert(b_entity == SysA);
    
    ecs_fini(world);
}

void SystemMisc_match_system_w_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, SysA, 0, SYSTEM:Position);
    ECS_SYSTEM(world, SysB, 0, Position);

    ecs_run_w_filter(world, SysB, 0, 0, 0, &(ecs_filter_t){
        .include = ecs_type(Position)
    }, NULL);

    test_assert(b_invoked != 0);
    test_assert(b_entity == SysA);
    
    ecs_fini(world);
}

void SystemMisc_system_initial_state() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, SysA, 0, Position);

    test_assert( ecs_has_entity(world, SysA, EcsInactive));
    test_assert( !ecs_has_entity(world, SysA, EcsDisabled));
    test_assert( !ecs_has_entity(world, SysA, EcsDisabledIntern));

    ecs_fini(world);
}

static
void FooSystem(ecs_iter_t *it) { }

static
void BarSystem(ecs_iter_t *it) { }

void SystemMisc_add_own_component() {
    ecs_world_t * world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, FooSystem, 0, Position);
    ECS_SYSTEM(world, BarSystem, 0, Position);

    ecs_set_ptr(world, BarSystem, Position, NULL );

    /* Make sure code didn't assert */
    test_assert(true);

    ecs_fini(world);
}

static bool action_a_invoked;
static bool action_b_invoked;

static
void ActionA(ecs_iter_t *it) {
    action_a_invoked = true;
}

static
void ActionB(ecs_iter_t *it) {
    action_b_invoked = true;
}

void SystemMisc_change_system_action() {
    ecs_world_t * world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t sys = ecs_new_system(
        world, 0, "Sys", EcsOnUpdate, "Position", ActionA);

    ecs_new(world, Position);

    test_bool(action_a_invoked, false);
    test_bool(action_b_invoked, false);

    ecs_progress(world, 0);

    test_bool(action_a_invoked, true);
    test_bool(action_b_invoked, false);

    action_a_invoked = false;

    ecs_set(world, sys, EcsIterAction, {ActionB});

    ecs_progress(world, 0);

    test_bool(action_a_invoked, false);
    test_bool(action_b_invoked, true);

    ecs_fini(world);
}

void SystemMisc_system_readeactivate() {
    ecs_world_t * world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    /* No entities, system should be deactivated */
    test_assert( ecs_has_entity(world, Dummy, EcsInactive));

    ecs_entity_t e = ecs_new(world, Position);

    /* System should be active, one entity is matched */
    test_assert( !ecs_has_entity(world, Dummy, EcsInactive));

    ecs_delete(world, e);

    /* System is not automatically deactivated */
    test_assert( !ecs_has_entity(world, Dummy, EcsInactive));

    /* Manually deactivate system that aren't matched with entities */
    ecs_deactivate_systems(world);

    /* System should be deactivated */
    test_assert( ecs_has_entity(world, Dummy, EcsInactive));

    ecs_fini(world);
}

static
void Dummy1(ecs_iter_t *it) { }

static
void Dummy2(ecs_iter_t *it) { }

void SystemMisc_system_readeactivate_w_2_systems() {
    ecs_world_t * world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    
    ECS_SYSTEM(world, Dummy1, EcsOnUpdate, Position);
    ECS_SYSTEM(world, Dummy2, EcsOnUpdate, Mass);

    /* No entities, system should be deactivated */
    test_assert( ecs_has_entity(world, Dummy1, EcsInactive));
    test_assert( ecs_has_entity(world, Dummy2, EcsInactive));

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_new(world, Mass);

    /* Systems should be active, one entity is matched */
    test_assert( !ecs_has_entity(world, Dummy1, EcsInactive));
    test_assert( !ecs_has_entity(world, Dummy2, EcsInactive));

    ecs_delete(world, e1);

    /* System is not automatically deactivated */
    test_assert( !ecs_has_entity(world, Dummy1, EcsInactive));
    test_assert( !ecs_has_entity(world, Dummy2, EcsInactive));

    /* Manually deactivate system that aren't matched with entities */
    ecs_deactivate_systems(world);

    /* System should be deactivated */
    test_assert( ecs_has_entity(world, Dummy1, EcsInactive));
    test_assert( !ecs_has_entity(world, Dummy2, EcsInactive));

    ecs_fini(world);
}
