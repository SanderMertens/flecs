#include <addons.h>

static
int32_t dummy_invoked = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_invoked = true;
    probe_iter(it);
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

    test_assert(true);

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

void SystemMisc_invalid_0_w_from_entity() {
    install_test_abort();

    ecs_world_t *world = ecs_init();
    
    ECS_ENTITY(world, Foo, 0);

    test_expect_abort();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Foo.0);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position(NotValid));

    ecs_fini(world);
}

void SystemMisc_invalid_null_string() {
    ecs_world_t *world = ecs_init();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "Dummy", .add = {ecs_dependson(EcsOnUpdate)} },
        .callback = Dummy
    });

    ecs_progress(world, 0);

    test_assert(dummy_invoked == true);

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string() {
    ecs_world_t *world = ecs_init();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "Dummy", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "",
        .callback = Dummy
    });

    ecs_progress(world, 0);

    test_assert(dummy_invoked == true);    

    ecs_fini(world);
}

void SystemMisc_invalid_empty_string_w_space() {
    ecs_world_t *world = ecs_init();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "Dummy", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "  ",
        .callback = Dummy
    });

    ecs_progress(world, 0);

    test_assert(dummy_invoked == true);    

    ecs_fini(world);
}

void SystemMisc_invalid_mixed_src_modifier() {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "Dummy", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "Position(super) || Velocity",
        .callback = Dummy
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void SystemMisc_redefine_row_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s;
    {
        ECS_OBSERVER(world, Dummy, EcsOnAdd, Position);
        s = Dummy;
    }

    ECS_OBSERVER(world, Dummy, EcsOnAdd, Position);

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

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, flecs.core.Prefab || Disabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_system_w_or_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Entity, Position, Disabled);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, Prefab || Disabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_system_w_or_disabled_and_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);
    ECS_ENTITY(world, Entity, Position, Disabled);

    ECS_SYSTEM(world, IsInvoked, EcsOnUpdate, Position, flecs.core.Prefab || Disabled);

    test_int(is_invoked, 0);

    ecs_progress(world, 1);

    test_int(is_invoked, 2);
    is_invoked = false;

    ecs_fini(world);
}

static
void TableColumns(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    const ecs_type_t *type = ecs_table_get_type(it->table);
    test_int(2, type->count);

    ecs_entity_t *components = type->array;
    test_int(components[0], ecs_id(Position));
    test_int(components[1], ecs_id(Velocity));

    void *column_0 = ecs_iter_column_w_size(it, sizeof(Position), 0);
    test_assert(column_0 == p);

    void *column_1 = ecs_iter_column_w_size(it, sizeof(Velocity), 1);
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

void SystemMisc_dont_enable_after_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position(self|super), Velocity(self|super));

    /* Create an entity that is watched. Whenever components are added/removed
     * to and/or from watched entities, a rematch is triggered. */
    ECS_PREFAB(world, Prefab, Position);

    ECS_ENTITY(world, Entity, (IsA, Prefab));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* System is enabled but doesn't match with any entities */
    
    test_bool(ecs_has_id(world, Dummy, EcsDisabled), false);
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    /* Explicitly disable system before triggering a rematch */
    ecs_enable(world, Dummy, false);
    test_bool(ecs_has_id(world, Dummy, EcsDisabled), true);

    /* Trigger a rematch. System should still be disabled after this */
    ecs_add(world, Prefab, Velocity);
    test_bool(ecs_has_id(world, Dummy, EcsDisabled), true);

    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    /* Enable system. It is matched, so should now be invoked */
    ecs_enable(world, Dummy, true);
    test_bool(ecs_has_id(world, Dummy, EcsDisabled), false);

    ecs_progress(world, 1);
    test_int(ctx.count, 1);

    ecs_fini(world);
}

static void SysA(ecs_iter_t *it)
{
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);
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

    ECS_SYSTEM(world, SysA, 0, Position(SysA));
    ECS_SYSTEM(world, SysB, 0, Position);

    ecs_run(world, SysB, 0, NULL);

    test_assert(b_invoked != 0);
    test_assert(b_entity == SysA);
    
    ecs_fini(world);
}

void SystemMisc_system_initial_state() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, SysA, 0, Position);

    test_assert( ecs_has_id(world, SysA, EcsEmpty));
    test_assert( !ecs_has_id(world, SysA, EcsDisabled));

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

    ecs_set_ptr(world, BarSystem, Position, NULL);

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
    
    ecs_entity_t sys = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "Sys", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = ActionA
    });

    ecs_new(world, Position);

    test_bool(action_a_invoked, false);
    test_bool(action_b_invoked, false);

    ecs_progress(world, 0);

    test_bool(action_a_invoked, true);
    test_bool(action_b_invoked, false);

    action_a_invoked = false;

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .entity = sys},
        .callback = ActionB
    });

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
    test_assert( ecs_has_id(world, Dummy, EcsEmpty));

    ecs_entity_t e = ecs_new(world, Position);
    ecs_run_aperiodic(world, 0);

    /* System should be active, one entity is matched */
    test_assert( !ecs_has_id(world, Dummy, EcsEmpty));

    ecs_delete(world, e);
    ecs_run_aperiodic(world, 0);

    /* System is not automatically deactivated */
    test_assert( !ecs_has_id(world, Dummy, EcsEmpty));

    /* Manually deactivate system that aren't matched with entities */
    ecs_run_aperiodic(world, EcsAperiodicEmptyQueries);

    /* System should be deactivated */
    test_assert( ecs_has_id(world, Dummy, EcsEmpty));

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
    test_assert( ecs_has_id(world, Dummy1, EcsEmpty));
    test_assert( ecs_has_id(world, Dummy2, EcsEmpty));

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_new(world, Mass);
    ecs_run_aperiodic(world, 0);

    /* Systems should be active, one entity is matched */
    test_assert( !ecs_has_id(world, Dummy1, EcsEmpty));
    test_assert( !ecs_has_id(world, Dummy2, EcsEmpty));

    ecs_delete(world, e1);
    ecs_run_aperiodic(world, 0);

    /* System is not automatically deactivated */
    test_assert( !ecs_has_id(world, Dummy1, EcsEmpty));
    test_assert( !ecs_has_id(world, Dummy2, EcsEmpty));

    /* Manually deactivate system that aren't matched with entities */
    ecs_run_aperiodic(world, EcsAperiodicEmptyQueries);

    /* System should be deactivated */
    test_assert( ecs_has_id(world, Dummy1, EcsEmpty));
    test_assert( !ecs_has_id(world, Dummy2, EcsEmpty));

    ecs_fini(world);
}

void SystemMisc_add_to_system_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_defer_begin(world);

    ecs_add(world, Dummy, Tag);

    ecs_defer_end(world);

    ecs_progress(world, 0);
    test_assert(dummy_invoked == true);

    ecs_fini(world);
}

static
void Action(ecs_iter_t *it) { }

void SystemMisc_redefine_null_signature() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s_1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "System", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = NULL,
        .callback = Action
    });

    ecs_entity_t s_2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "System", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = NULL,
        .callback = Action
    });      

    test_assert(s_1 == s_2);

    ecs_fini(world);
}

void SystemMisc_redefine_0_signature() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s_1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "System", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0",
        .callback = Action
    });

    ecs_entity_t s_2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "System", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0",
        .callback = Action
    }); 

    test_assert(s_1 == s_2);

    ecs_fini(world);
}

void SystemMisc_one_named_column_of_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f, .terms = {
        { ecs_id(Position), .name = "pos" },
        { ecs_id(Velocity) }
    }}));

    test_int(f.term_count, 2);
    test_int(f.term_count_actual, 2);

    ecs_term_t *
    term = &f.terms[0];
    test_assert(term->oper == EcsAnd);
    test_assert(term->subj.set.mask == (EcsSelf|EcsSuperSet));
    test_assert(term->subj.entity == EcsThis);
    test_assert(term->inout == EcsInOutDefault);
    test_assert(term->id == ecs_id(Position));
    test_str(term->name, "pos");

    term = &f.terms[1];
    test_assert(term->oper == EcsAnd);
    test_assert(term->subj.set.mask == (EcsSelf|EcsSuperSet));
    test_assert(term->subj.entity == EcsThis);
    test_assert(term->inout == EcsInOutDefault);
    test_assert(term->id == ecs_id(Velocity));
    test_str(term->name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void SystemMisc_two_named_columns_of_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f, .terms = {
        { ecs_id(Position), .name = "pos" },
        { ecs_id(Velocity), .name = "vel" }
    }}));    

    test_int(f.term_count, 2);
    test_int(f.term_count_actual, 2);

    ecs_term_t *
    term = &f.terms[0];
    test_assert(term->oper == EcsAnd);
    test_assert(term->subj.set.mask == (EcsSelf|EcsSuperSet));
    test_assert(term->subj.entity == EcsThis);
    test_assert(term->inout == EcsInOutDefault);
    test_assert(term->id == ecs_id(Position));
    test_str(term->name, "pos");

    term = &f.terms[1];
    test_assert(term->oper == EcsAnd);
    test_assert(term->subj.set.mask == (EcsSelf|EcsSuperSet));
    test_assert(term->subj.entity == EcsThis);
    test_assert(term->inout == EcsInOutDefault);
    test_assert(term->id == ecs_id(Velocity));
    test_str(term->name, "vel");

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void SystemMisc_redeclare_system_explicit_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.name = "Move", .add = {ecs_dependson(EcsOnUpdate)} }, 
        .query.filter.expr = "Position, Velocity", 
        .callback = Dummy
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = s1, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "Position, Velocity", 
        .callback = Dummy
    });

    test_assert(s1 == s2);

    ecs_fini(world);
}

void SystemMisc_redeclare_system_explicit_id_null_expr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.name = "Move", .add = {ecs_dependson(EcsOnUpdate)} }, 
        .query.filter.expr = NULL, 
        .callback = Dummy
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = s1, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = NULL, 
        .callback = Dummy
    });

    test_assert(s1 == s2);

    ecs_fini(world);
}

void SystemMisc_redeclare_system_explicit_id_no_name() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.name = "Move", .add = {ecs_dependson(EcsOnUpdate)} }, 
        .query.filter.expr = "Position, Velocity", 
        .callback = Dummy
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = s1, .name = NULL, .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "Position, Velocity", 
        .callback = Dummy
    });

    test_assert(s1 == s2);
    test_str(ecs_get_name(world, s1), "Move");

    ecs_fini(world);
}

void SystemMisc_declare_different_id_same_name() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);

    ecs_entity_t s_1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = e1, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0", 
        .callback = Dummy
    });
    test_assert(e1 == s_1);

    test_expect_abort();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = e2, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0", 
        .callback = Dummy
    });
}

void SystemMisc_declare_different_id_same_name_w_scope() {
    install_test_abort();
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_new(world, 0);
    ecs_set_scope(world, scope);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);

    ecs_entity_t s_1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = e1, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0", 
        .callback = Dummy
    });
    test_assert(e1 == s_1);

    test_expect_abort();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = e2, .name = "Move", .add = {ecs_dependson(EcsOnUpdate)} },
        .query.filter.expr = "0", 
        .callback = Dummy
    });
}

void SystemMisc_rw_in_implicit_any() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity(self|super)");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == false);

    ecs_fini(world);
}

void SystemMisc_rw_in_implicit_shared() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity(super)");

    ecs_entity_t base = ecs_new(world, Velocity);
    ecs_entity_t e = ecs_new(world, Position);
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == true);

    ecs_fini(world);
}

void SystemMisc_rw_in_implicit_from_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity()");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == true);

    ecs_fini(world);
}

void SystemMisc_rw_in_implicit_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_ENTITY(world, f, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity(f)");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == true);

    ecs_fini(world);
}

void SystemMisc_rw_out_explicit_any() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, [out] Velocity(self|super)");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == false);

    ecs_fini(world);
}

void SystemMisc_rw_out_explicit_shared() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, [out] Velocity(super)");

    ecs_entity_t base = ecs_new(world, Velocity);
    ecs_entity_t e = ecs_new(world, Position);
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == false);

    ecs_fini(world);
}

void SystemMisc_rw_out_explicit_from_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, [out] Velocity()");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == false);

    ecs_fini(world);
}

void SystemMisc_rw_out_explicit_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_ENTITY(world, f, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, [out] Velocity(f)");

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it) == true);
    test_assert(ecs_term_is_readonly(&it, 1) == false);
    test_assert(ecs_term_is_readonly(&it, 2) == false);

    ecs_fini(world);
}

void SystemMisc_activate_system_for_table_w_n_pairs() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pair);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, (Pair, *));

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);    

    ECS_ENTITY(world, e, (Pair, TagA), (Pair, TagB));
    test_assert(e != 0);

    ecs_progress(world, 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e);

    test_int(ctx.c[0][0], ecs_pair(Pair, TagA));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[1][0], ecs_pair(Pair, TagB));
    test_int(ctx.s[1][0], 0);    

    ecs_fini(world);
}

void SystemMisc_get_query() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_set(world, 0, Position, {0, 0});
    ecs_set(world, 0, Position, {1, 0});
    ecs_set(world, 0, Position, {2, 0});

    ecs_query_t *q = ecs_system_get_query(world, Dummy);
    test_assert(q != NULL);

    int32_t count = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        test_assert(p != NULL);

        int i;
        for (i = 0; i < it.count; i ++) {
            test_int(p[i].x, i);
            count ++;
        }
    }

    test_int(count, 3);

    ecs_fini(world);
}

void SystemMisc_set_get_context() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int32_t ctx_a, ctx_b;

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.name = "MySystem",
        .query.filter.terms = {{Tag}},
        .callback = Dummy,
        .ctx = &ctx_a
    });
    test_assert(s != 0);

    test_assert(ecs_get_system_ctx(world, s) == &ctx_a);

    test_assert(ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = s,
        .ctx = &ctx_b
    }) == s);

    test_assert(ecs_get_system_ctx(world, s) == &ctx_b);

    ecs_fini(world);
}

void SystemMisc_set_get_binding_context() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int32_t ctx_a, ctx_b;

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.name = "MySystem",
        .query.filter.terms = {{Tag}},
        .callback = Dummy,
        .binding_ctx = &ctx_a
    });
    test_assert(s != 0);

    test_assert(ecs_get_system_binding_ctx(world, s) == &ctx_a);

    test_assert(ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = s,
        .binding_ctx = &ctx_b
    }) == s);

    test_assert(ecs_get_system_binding_ctx(world, s) == &ctx_b);

    ecs_fini(world);
}

void SystemMisc_deactivate_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    ecs_run_aperiodic(world, 0);
    
    test_assert(!ecs_has_id(world, Dummy, EcsEmpty));

    ecs_enable(world, Dummy, false);
    test_assert(!ecs_has_id(world, Dummy, EcsEmpty));
    test_assert(ecs_has_id(world, Dummy, EcsDisabled));

    ecs_delete(world, e);
    ecs_run_aperiodic(world, 0);

    test_assert(!ecs_has_id(world, Dummy, EcsEmpty));
    test_assert(ecs_has_id(world, Dummy, EcsDisabled));

    ecs_fini(world);
}

void SystemMisc_delete_system() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.name = "Foo",
        .query.filter.terms = {{.id = Tag}},
        .callback = Dummy
    });
    test_assert(system != 0);

    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, Tag);

    ecs_run(world, system, 0, NULL);
    test_int(ctx.count, 1);
    test_assert(ctx.system == system);

    ecs_delete(world, system);
    test_assert(!ecs_is_alive(world, system));

    ecs_fini(world);
}

void SystemMisc_delete_pipeline_system() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    Probe ctx = {0};

    // Create system before
    test_assert(ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .query.filter.terms = {{.id = Tag}},
        .callback = Dummy
    }) != 0);

    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .query.filter.terms = {{.id = Tag}},
        .callback = Dummy
    });
    test_assert(system != 0);

    // Create system after
    test_assert(ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .query.filter.terms = {{.id = Tag}},
        .callback = Dummy
    }) != 0);

    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, Tag);

    ecs_progress(world, 0);
    test_int(ctx.count, 3);

    ctx.count = 0;

    ecs_delete(world, system);

    ecs_progress(world, 0);
    test_int(ctx.count, 2);

    ecs_fini(world);
}

static int ctx_value;
static
void ctx_free(void *ctx) {
    test_assert(&ctx_value == ctx);
    ctx_value ++;
}

static int binding_ctx_value;
static
void binding_ctx_free(void *ctx) {
    test_assert(&binding_ctx_value == ctx);
    binding_ctx_value ++;
}

void SystemMisc_delete_system_w_ctx() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{.id = Tag}},
        .callback = Dummy,
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .binding_ctx = &binding_ctx_value,
        .binding_ctx_free = binding_ctx_free
    });
    test_assert(system != 0);

    test_assert(ecs_get_system_ctx(world, system) == &ctx_value);
    test_assert(ecs_get_system_binding_ctx(world, system) 
        == &binding_ctx_value);

    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, Tag);

    ecs_run(world, system, 0, NULL);
    test_int(ctx.count, 1);
    test_assert(ctx.system == system);

    ecs_delete(world, system);
    test_assert(!ecs_is_alive(world, system));

    ecs_fini(world);
}

static int run_invoked = 0;

static void Run(ecs_iter_t *it) {
    while (ecs_iter_next(it)) {
        probe_iter(it);
    }

    run_invoked ++;
}

static int run_2_invoked = 0;

static void Run2(ecs_iter_t *it) {
    run_2_invoked ++;
}

static void Run_call_callback(ecs_iter_t *it) {
    run_invoked ++;
    
    test_assert(it != NULL);
    test_assert(it->next != NULL);
    test_assert(it->callback != NULL);

    while (ecs_iter_next(it)) {
        it->callback(it);
    }
}

void SystemMisc_run_custom_run_action() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ .id = TagA }},
        .run = Run,
        .callback = Dummy,
        .ctx = &ctx,
    });
    test_assert(system != 0);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add(world, e3, TagB); // 2 tables

    ecs_run(world, system, 0, NULL);

    test_bool(dummy_invoked, false);
    test_int(run_invoked, 1);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    ecs_fini(world);
}

void SystemMisc_run_w_offset_limit_custom_run_action() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ .id = TagA }},
        .run = Run,
        .callback = Dummy,
        .ctx = &ctx,
    });
    test_assert(system != 0);

    ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add(world, e3, TagB); // 2 tables

    ecs_run_w_filter(world, system, 0, 1, 1, NULL);

    test_bool(dummy_invoked, false);
    test_int(run_invoked, 1);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);

    ecs_fini(world);
}

void SystemMisc_pipeline_custom_run_action() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ .id = TagA }},
        .run = Run,
        .callback = Dummy,
        .ctx = &ctx,
        .entity.add = {ecs_dependson(EcsOnUpdate)}
    });
    test_assert(system != 0);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add(world, e3, TagB); // 2 tables

    ecs_progress(world, 0);

    test_bool(dummy_invoked, false);
    test_int(run_invoked, 1);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    ecs_fini(world);
}

void SystemMisc_change_custom_run_action() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ .id = TagA }},
        .run = Run,
        .callback = Dummy,
        .ctx = &ctx,
    });
    test_assert(system != 0);

    ecs_new(world, TagA);

    ecs_run(world, system, 0, NULL);
    test_bool(dummy_invoked, false);
    test_int(run_invoked, 1);
    test_int(run_2_invoked, 0);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = system,
        .run = Run2
    });

    ecs_run(world, system, 0, NULL);
    test_bool(dummy_invoked, false);
    test_int(run_invoked, 1);
    test_int(run_2_invoked, 1);

    ecs_fini(world);
}

void SystemMisc_custom_run_action_call_next() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t system = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ .id = TagA }},
        .run = Run_call_callback,
        .callback = Dummy,
        .ctx = &ctx,
        .entity.add = {ecs_dependson(EcsOnUpdate)}
    });
    test_assert(system != 0);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add(world, e3, TagB); // 2 tables

    ecs_progress(world, 0);

    test_bool(dummy_invoked, true);
    test_int(run_invoked, 1);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    ecs_fini(world);
}
