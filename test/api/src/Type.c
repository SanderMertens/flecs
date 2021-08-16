#include <api.h>
#include <flecs/type.h>

static
char* type_str(ecs_world_t *world, ecs_entity_t type_ent) {
    const EcsType *type_comp = ecs_get(world, type_ent, EcsType);
    test_assert(type_comp != NULL);
    return ecs_type_str(world, type_comp->normalized);
}

void Type_setup() {
    ecs_tracing_enable(-2);
}

void Type_type_of_1_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position);

    char *str = type_str(world, Type);
    
    test_str(str, "Position");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    char *str = type_str(world, Type);
    
    test_str(str, "Position,Velocity");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr_no_id() {
    ecs_world_t *world = ecs_init();

    ecs_type_t t = ecs_type_add(world, NULL, 100);
    t = ecs_type_add(world, t, 200);
    
    char *str = ecs_type_str(world, t);
    
    test_str(str, "100,200");

    free(str);

    ecs_fini(world);
}

void Type_type_redefine() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t type = ecs_type_init(world, &(ecs_type_desc_t){
        .entity.name = "Type",
        .ids_expr = "Position"
    });

    ecs_entity_t type_2 = ecs_type_init(world, &(ecs_type_desc_t){
        .entity.name = "Type",
        .ids_expr = "Position"
    });    

    test_assert(type == type_2);

    ecs_fini(world);
}

void Type_invalid_container_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, PARENT:Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_entity_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Entity, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, Entity:Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_system_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, SYSTEM:Position, Velocity);

    ecs_fini(world);
}

void Type_get_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_type_t t = ecs_get_type(world, e);
    test_assert(t != NULL);
    test_int(ecs_vector_count(t), 1);
    
    ecs_entity_t *type_array = ecs_vector_first(t, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(type_array[0], ecs_id(Position));

    ecs_fini(world);
}

void Type_get_type_from_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);

    ecs_type_t t = ecs_get_type(world, e);
    test_assert(t == NULL);

    ecs_fini(world);
}

void Type_get_type_from_0() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ecs_get_type(world, 0);
}

void Type_type_to_expr_1_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position);

    char *expr = type_str(world, Type);
    test_str(expr, "Position");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    char *expr = type_str(world, Type);
    test_str(expr, "Position,Velocity");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_instanceof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, (IsA, Position));

    char *expr = type_str(world, Type);
    test_str(expr, "(IsA,Position)");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, (ChildOf, Position));

    char *expr = type_str(world, Type);
    test_str(expr, "(ChildOf,Position)");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_pair() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    /* Cannot create a type that just sets the hi id */
    ECS_TYPE(world, Type, (Position, *));
}

void Type_type_to_expr_pair_w_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, (Position, Velocity));

    char *expr = type_str(world, Type);
    test_str(expr, "(Position,Velocity)");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_scope() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, scope, 0);
    ecs_set_scope(world, scope);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ecs_set_scope(world, 0);

    char *expr = type_str(world, Type);
    test_str(expr, "Position,scope.Velocity");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_entity_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, e, 0);

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "e");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_path_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, parent, 0);
    ECS_ENTITY(world, e, (ChildOf, parent));

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "parent.e");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_instanceof_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ecs_pair(EcsIsA, Foo);

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "(IsA,Foo)");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_childof_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ecs_pair(EcsChildOf, Foo);

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "(ChildOf,Foo)");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_pair_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);
    ECS_ENTITY(world, Bar, 0);

    ecs_entity_t e = ecs_pair(Bar, Foo);

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "(Bar,Foo)");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_switch_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_SWITCH | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "SWITCH|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_case_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_CASE | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "CASE|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_and_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_AND | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "AND|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_or_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_OR | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "OR|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_xor_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_XOR | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "XOR|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_not_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_NOT | Foo;

    char buffer[256];
    size_t result = ecs_id_str(world, e, buffer, 256);
    test_str(buffer, "NOT|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_str_small_buffer() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ecs_pair(EcsChildOf, Foo);

    char buffer[10];
    size_t result = ecs_id_str(world, e, buffer, 10);
    test_str(buffer, "(ChildOf,");
    test_int(strlen(buffer), 9);
    test_int(strlen("(ChildOf,Foo)"), result);

    ecs_fini(world);
}

void Type_role_pair_str() {
    ecs_entity_t e = ECS_PAIR;
    test_str(ecs_role_str(e), "PAIR");
}

void Type_role_switch_str() {
    ecs_entity_t e = ECS_SWITCH;
    test_str(ecs_role_str(e), "SWITCH");
}

void Type_role_case_str() {
    ecs_entity_t e = ECS_CASE;
    test_str(ecs_role_str(e), "CASE");
}

void Type_role_and_str() {
    ecs_entity_t e = ECS_AND;
    test_str(ecs_role_str(e), "AND");
}

void Type_role_or_str() {
    ecs_entity_t e = ECS_OR;
    test_str(ecs_role_str(e), "OR");
}

void Type_role_xor_str() {
    ecs_entity_t e = ECS_XOR;
    test_str(ecs_role_str(e), "XOR");
}

void Type_role_not_str() {
    ecs_entity_t e = ECS_NOT;
    test_str(ecs_role_str(e), "NOT");
}

void Type_role_owned_str() {
    ecs_entity_t e = ECS_OWNED;
    test_str(ecs_role_str(e), "OWNED");
}

void Type_role_disabled_str() {
    ecs_entity_t e = ECS_DISABLED;
    test_str(ecs_role_str(e), "DISABLED");
}

void Type_large_type_expr() {
    ecs_world_t *world = ecs_init();

    int i;
    for (i = 0; i < 64; i ++) {
        char buff[4] = { 'e' };
        sprintf(&buff[1], "%d", i + 1);
        ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = buff
        });
    }

    ecs_entity_t type_ent = ecs_type_init(world, &(ecs_type_desc_t) {
        .ids_expr = "e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,"
                "e11, e12, e13, e14, e15, e16, e17, e18, e19, e20,"
                "e21, e22, e23, e24, e25, e26, e27, e28, e29, e30,"
                "e31, e32, e33, e34, e35, e36, e37, e38, e39, e40,"
                "e41, e42, e43, e44, e45, e46, e47, e48, e49, e50,"
                "e51, e52, e53, e54, e55, e56, e57, e58, e59, e60,"
                "e61, e62, e63, e64"
    });

    test_assert(type_ent != 0);

    const EcsType *ptr = ecs_get(world, type_ent, EcsType);
    test_assert(ptr != NULL);
    test_assert(ecs_vector_count(ptr->type) == 64);
    test_assert(ecs_vector_count(ptr->normalized) == 64);

    for (i = 0; i < 64; i ++) {
        char buff[4] = { 'e' };
        sprintf(&buff[1], "%d", i + 1);
        ecs_entity_t e = ecs_lookup(world, buff);
        test_assert(e != 0);
        test_str(ecs_get_name(world, e), buff);

        test_assert(ecs_type_index_of(ptr->type, 0, e) == i);
    }

    ecs_fini(world);
}

void Type_large_type_expr_limit() {
    ecs_world_t *world = ecs_init();

    test_assert(ECS_MAX_ADD_REMOVE == 32);

    int i;
    for (i = 0; i < 32; i ++) {
        char buff[4] = { 'e' };
        sprintf(&buff[1], "%d", i + 1);
        ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = buff
        });
    }

    ecs_entity_t type_ent = ecs_type_init(world, &(ecs_type_desc_t) {
        .ids_expr = "e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,"
                "e11, e12, e13, e14, e15, e16, e17, e18, e19, e20,"
                "e21, e22, e23, e24, e25, e26, e27, e28, e29, e30,"
                "e31, e32"
    });

    test_assert(type_ent != 0);

    const EcsType *ptr = ecs_get(world, type_ent, EcsType);
    test_assert(ptr != NULL);
    test_assert(ecs_vector_count(ptr->type) == 32);
    test_assert(ecs_vector_count(ptr->normalized) == 32);

    for (i = 0; i < 32; i ++) {
        char buff[4] = { 'e' };
        sprintf(&buff[1], "%d", i + 1);
        ecs_entity_t e = ecs_lookup(world, buff);
        test_assert(e != 0);
        test_str(ecs_get_name(world, e), buff);

        test_assert(ecs_type_index_of(ptr->type, 0, e) == i);
    }

    ecs_fini(world);
}
