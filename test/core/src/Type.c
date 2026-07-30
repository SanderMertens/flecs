#include <core.h>
#include <stdio.h>

void Type_setup(void) {
    ecs_log_set_level(-2);
}

void Type_type_of_1_tostr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_type_t t = { .array = (ecs_id_t[]){ ecs_id(Position) }, .count = 1 };

    char *str = ecs_type_str(world, &t);
    
    test_str(str, "Position");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t t = { .array = (ecs_id_t[]){ 
        ecs_id(Position), ecs_id(Velocity) 
    }, .count = 2 };

    char *str = ecs_type_str(world, &t);
    
    test_str(str, "Position, Velocity");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr_no_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 100);
    ecs_make_alive(world, 200);
    ecs_type_t t = { .array = (ecs_id_t[]){ 100, 200 }, .count = 2 };
    
    char *str = ecs_type_str(world, &t);
    
    test_str(str, "#100, #200");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_get_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    const ecs_type_t *t = ecs_get_type(world, e);
    test_assert(t != NULL);
    test_int(t->count, 1);
    
    ecs_entity_t *type_array = t->array;
    test_assert(type_array != NULL);
    test_int(type_array[0], ecs_id(Position));

    ecs_fini(world);
}

void Type_get_type_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);

    const ecs_type_t *t = ecs_get_type(world, e);
    test_assert(t != NULL);
    test_int(t->count, 0);

    ecs_fini(world);
}

void Type_get_type_from_0(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();

    ecs_get_type(world, 0);
}

void Type_type_to_expr_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 100);
    ecs_make_alive(world, 200);
    ecs_type_t t = { .array = (ecs_id_t[]){ ecs_pair(100, 200) }, .count = 1 };
    
    char *str = ecs_type_str(world, &t);

    test_str(str, "(#100,#200)");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_type_to_expr_pair_w_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t t = { .array = (ecs_id_t[]){ 
        ecs_pair(ecs_id(Position), ecs_id(Velocity)) 
    }, .count = 1 };
    
    char *str = ecs_type_str(world, &t);

    test_str(str, "(Position,Velocity)");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_type_to_expr_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t scope = ecs_entity(world, { .name = "scope" });
    ecs_set_scope(world, scope);
    ECS_COMPONENT(world, Velocity);
    ecs_set_scope(world, 0);

    ecs_type_t t = { .array = (ecs_id_t[]){ 
        ecs_pair(ecs_id(Position), ecs_id(Velocity)) 
    }, .count = 1 };
    
    char *str = ecs_type_str(world, &t);

    test_str(str, "(Position,scope.Velocity)");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_type_to_expr_pair_w_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t t = { .array = (ecs_id_t[]){ 
        ECS_AUTO_OVERRIDE | ecs_pair(ecs_id(Position), ecs_id(Velocity)) 
    }, .count = 1 };
    
    char *str = ecs_type_str(world, &t);

    test_str(str, "AUTO_OVERRIDE|(Position,Velocity)");

    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_str(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    char *str = ecs_id_str(world, e);
    test_str(str, "e");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_path_str(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_pair(world, e, EcsChildOf, parent);

    char *str = ecs_id_str(world, e);
    test_str(str, "parent.e");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_instanceof_str(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Foo = ecs_entity(world, { .name = "Foo" });

    ecs_entity_t e = ecs_pair(EcsIsA, Foo);

    char *str = ecs_id_str(world, e);
    test_str(str, "(IsA,Foo)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_childof_str(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Foo = ecs_entity(world, { .name = "Foo" });

    ecs_entity_t e = ecs_pair(EcsChildOf, Foo);

    char *str = ecs_id_str(world, e);
    test_str(str, "(ChildOf,Foo)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_pair_str(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t Bar = ecs_entity(world, { .name = "Bar" });

    ecs_entity_t e = ecs_pair(Bar, Foo);

    char *str = ecs_id_str(world, e);
    test_str(str, "(Bar,Foo)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_entity_str_small_buffer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Foo = ecs_entity(world, { .name = "Foo" });

    ecs_entity_t e = ecs_pair(EcsChildOf, Foo);

    char *str = ecs_id_str(world, e);
    test_str(str, "(ChildOf,Foo)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Type_role_pair_str(void) {
    ecs_entity_t e = ECS_PAIR;
    test_str(ecs_id_flag_str(e), "PAIR");
}

void Type_role_owned_str(void) {
    ecs_entity_t e = ECS_AUTO_OVERRIDE;
    test_str(ecs_id_flag_str(e), "AUTO_OVERRIDE");
}

void Type_role_disabled_str(void) {
    ecs_entity_t e = ECS_TOGGLE;
    test_str(ecs_id_flag_str(e), "TOGGLE");
}
