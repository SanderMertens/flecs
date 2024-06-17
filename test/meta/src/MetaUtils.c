#include <meta.h>

ECS_STRUCT(Struct_2_i32, {
    int32_t x;
    int32_t y;
});

ECS_STRUCT(Struct_2_bool, {
    bool x;
    bool y;
});

ECS_STRUCT(Struct_2_char, {
    char x;
    char y;
});

ECS_STRUCT(Struct_2_string, {
    char *x;
    char *y;
});

ECS_STRUCT(Struct_2_f32, {
    float x;
    float y;
});

ECS_STRUCT(Struct_2_f64, {
    double x;
    double y;
});

ECS_STRUCT(_Struct_w_underscore, {
    double value;
});

ECS_STRUCT(Struct_w_underscore_member_name, {
    double _value;
});

ECS_STRUCT(Struct_w_underscore_member_type, {
    _Struct_w_underscore value;
});

ECS_STRUCT(Struct_w_ptrs, {
    void *ptr_a;
    void* ptr_b;
});

ECS_STRUCT(Struct_w_private, {
    double x;
    double y;
ECS_PRIVATE
    double z;
});

ECS_ENUM(Enum_Default, {
    Red, Green, Blue
});

ECS_ENUM(Enum_Default_Multiline, {
    Red_1, 
    Green_1, 
    Blue_1
});

ECS_ENUM(Enum_Assign, {
    Red_2 = 1, Green_2 = 3, Blue_2 = 6
});

ECS_BITMASK(Bitmask, {
    Bacon = 1, Lettuce = 2, Tomato = 4, Blt = 7
});

ECS_ENUM(Enum_Nospace, {EnumNospace_A,EnumNospace_B,EnumNospace_C});
ECS_STRUCT(Struct_Nospace, {float x;float y;});

ECS_ENUM(TestNamePrefix, {
    TestA,
    TestB,
    TestC
});

ECS_ENUM(TestTypePrefix, {
    TestTypePrefixA,
    TestTypePrefixB,
    TestTypePrefixC
});

ECS_ENUM(TestNameTypePrefix, {
    TestNameTypePrefixA,
    TestNameTypePrefixB,
    TestNameTypePrefixC
});

ECS_STRUCT(Struct_3_enum, {
    Enum_Default one;
    Enum_Default two;
    Enum_Default three;
});

ECS_STRUCT(Struct_3_enum_multiline, {
    Enum_Default_Multiline one;
    Enum_Default_Multiline two;
    Enum_Default_Multiline three;
});

ECS_STRUCT(Struct_3_enum_assign, {
    Enum_Assign one;
    Enum_Assign two;
    Enum_Assign three;
});

ECS_STRUCT(Struct_3_bitmask, {
    Bitmask one;
    Bitmask two;
    Bitmask three;
    Bitmask four;
});

ECS_STRUCT(Struct_3_entities, {
    ecs_entity_t one;
    ecs_entity_t two;
    ecs_entity_t three;
});

ECS_STRUCT(Struct_2_array_3_i32, {
    int32_t one[3];
    int32_t two[3];
});

ECS_STRUCT(Struct_nested, {
    Struct_2_i32 one;
});

ECS_STRUCT(Struct_2_nested, {
    Struct_2_i32 one;
    Struct_2_i32 two;
});

void MetaUtils_struct_w_2_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_i32);

    Struct_2_i32 v = {10, 20};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_i32), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: 10, y: 20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_bool(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_bool);

    Struct_2_bool v = {true, false};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_bool), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: true, y: false}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_char(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_char);

    Struct_2_char v = {'a', 'b'};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_char), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: \"a\", y: \"b\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_string(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_string);

    Struct_2_string v = {"Hello", "World"};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_string), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: \"Hello\", y: \"World\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_f32(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_f32);

    Struct_2_f32 v = {10.5, 20.5};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_f32), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: 10.5, y: 20.5}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_f64(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_f64);

    Struct_2_f64 v = {10.5, 20.5};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_f64), &v);
    test_assert(expr != NULL);
    test_str(expr, "{x: 10.5, y: 20.5}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_3_enum(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Enum_Default);
    ECS_META_COMPONENT(world, Struct_3_enum);

    Struct_3_enum v = {Red, Green, Blue};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_3_enum), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: Red, two: Green, three: Blue}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_3_enum_multiline(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Enum_Default_Multiline);
    ECS_META_COMPONENT(world, Struct_3_enum_multiline);

    Struct_3_enum_multiline v = {Red_1, Green_1, Blue_1};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_3_enum_multiline), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: Red_1, two: Green_1, three: Blue_1}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_3_enum_w_assignment(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Enum_Assign);
    ECS_META_COMPONENT(world, Struct_3_enum_assign);

    Struct_3_enum_assign v = {Red_2, Green_2, Blue_2};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_3_enum_assign), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: Red_2, two: Green_2, three: Blue_2}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_4_bitmask(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Bitmask);
    ECS_META_COMPONENT(world, Struct_3_bitmask);

    Struct_3_bitmask v = {0, Tomato, Bacon | Tomato, Blt};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_3_bitmask), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: 0, two: Tomato, three: Tomato|Bacon, four: Lettuce|Tomato|Bacon}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_3_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_3_entities);

    Struct_3_entities v = {0, EcsFlecs, EcsFlecsCore};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_3_entities), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: #0, two: flecs, three: flecs.core}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_array_3_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_array_3_i32);

    Struct_2_array_3_i32 v = {{1, 2, 3}, {6, 5, 4}};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_array_3_i32), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: [1, 2, 3], two: [6, 5, 4]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_i32);
    ECS_META_COMPONENT(world, Struct_nested);

    Struct_nested v = {{1, 2}};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_nested), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: {x: 1, y: 2}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_struct_w_2_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_2_i32);
    ECS_META_COMPONENT(world, Struct_2_nested);

    Struct_2_nested v = {{1, 2}, {4, 3}};
    char *expr = ecs_ptr_to_expr(world, ecs_id(Struct_2_nested), &v);
    test_assert(expr != NULL);
    test_str(expr, "{one: {x: 1, y: 2}, two: {x: 4, y: 3}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void MetaUtils_enum_nospace(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Enum_Nospace);

    ecs_entity_t e = ecs_id(Enum_Nospace);
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Enum_Nospace");
    test_assert(ecs_has(world, e, EcsEnum));

    ecs_entity_t c_a = ecs_lookup_child(world, e, "EnumNospace_A");
    test_assert(c_a != 0);

    ecs_entity_t c_b = ecs_lookup_child(world, e, "EnumNospace_B");
    test_assert(c_b != 0);

    ecs_entity_t c_c = ecs_lookup_child(world, e, "EnumNospace_C");
    test_assert(c_c != 0);

    ecs_fini(world);
}

void MetaUtils_struct_nospace(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_Nospace);

    ecs_entity_t e = ecs_id(Struct_Nospace);
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Struct_Nospace");
    test_assert(ecs_has(world, e, EcsStruct));

    ecs_entity_t c_a = ecs_lookup_child(world, e, "x");
    test_assert(c_a != 0);

    ecs_entity_t c_b = ecs_lookup_child(world, e, "y");
    test_assert(c_b != 0);

    ecs_fini(world);
}

void MetaUtils_identifier_w_underscore(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, _Struct_w_underscore);
    ECS_META_COMPONENT(world, Struct_w_underscore_member_name);
    ECS_META_COMPONENT(world, Struct_w_underscore_member_type);

    test_assert(ecs_id(_Struct_w_underscore) != 0);
    test_assert(ecs_id(Struct_w_underscore_member_name) != 0);
    test_assert(ecs_id(Struct_w_underscore_member_type) != 0);

    {
        ecs_entity_t s = ecs_id(_Struct_w_underscore);
        ecs_entity_t m = ecs_lookup_child(world, s, "value");
        test_assert(m != 0);
    }

    {
        ecs_entity_t s = ecs_id(Struct_w_underscore_member_name);
        ecs_entity_t m = ecs_lookup_child(world, s, "_value");
        test_assert(m != 0);
    }

    {
        ecs_entity_t s = ecs_id(Struct_w_underscore_member_type);
        ecs_entity_t m = ecs_lookup_child(world, s, "value");
        test_assert(m != 0);
    }

    ecs_fini(world);
}

void MetaUtils_struct_w_ptr(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_w_ptrs);

    test_assert(ecs_id(Struct_w_ptrs) != 0);
    ecs_entity_t s = ecs_id(Struct_w_ptrs);

    {
        ecs_entity_t m = ecs_lookup_child(world, s, "ptr_a");
        test_assert(m != 0);
    }
    {
        ecs_entity_t m = ecs_lookup_child(world, s, "ptr_b");
        test_assert(m != 0);
    }

    ecs_fini(world);
}

void MetaUtils_private_members(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, Struct_w_private);

    test_assert(ecs_id(Struct_w_private) != 0);
    ecs_entity_t s = ecs_id(Struct_w_private);

    {
        ecs_entity_t m = ecs_lookup_child(world, s, "x");
        test_assert(m != 0);
    }
    {
        ecs_entity_t m = ecs_lookup_child(world, s, "y");
        test_assert(m != 0);
    }
    {
        ecs_entity_t m = ecs_lookup_child(world, s, "z");
        test_assert(m == 0);
    }

    ecs_fini(world);
}

void MetaUtils_enum_constant_w_name_prefix(void) {
    ecs_world_t *world = ecs_init();

    ecs_set_name_prefix(world, "Test");

    ECS_META_COMPONENT(world, TestNamePrefix);

    test_assert(ecs_id(TestNamePrefix) != 0);
    ecs_entity_t e = ecs_id(TestNamePrefix);

    test_str(ecs_get_name(world, e), "NamePrefix");

    {
        ecs_entity_t c_a = ecs_lookup_child(world, e, "A");
        test_assert(c_a != 0);
    }

    {
        ecs_entity_t c_b = ecs_lookup_child(world, e, "B");
        test_assert(c_b != 0);
    }

    {
        ecs_entity_t c_c = ecs_lookup_child(world, e, "C");
        test_assert(c_c != 0);
    }

    ecs_fini(world);
}

void MetaUtils_enum_constant_w_type_prefix(void) {
    ecs_world_t *world = ecs_init();

    ECS_META_COMPONENT(world, TestTypePrefix);

    test_assert(ecs_id(TestTypePrefix) != 0);
    ecs_entity_t e = ecs_id(TestTypePrefix);

    {
        ecs_entity_t c_a = ecs_lookup_child(world, e, "A");
        test_assert(c_a != 0);
    }

    {
        ecs_entity_t c_b = ecs_lookup_child(world, e, "B");
        test_assert(c_b != 0);
    }

    {
        ecs_entity_t c_c = ecs_lookup_child(world, e, "C");
        test_assert(c_c != 0);
    }

    ecs_fini(world);
}

void MetaUtils_enum_constant_w_name_type_prefix(void) {
    ecs_world_t *world = ecs_init();

    ecs_set_name_prefix(world, "Test");

    ECS_META_COMPONENT(world, TestNameTypePrefix);

    test_assert(ecs_id(TestNameTypePrefix) != 0);
    ecs_entity_t e = ecs_id(TestNameTypePrefix);

    test_str(ecs_get_name(world, e), "NameTypePrefix");

    {
        ecs_entity_t c_a = ecs_lookup_child(world, e, "A");
        test_assert(c_a != 0);
    }

    {
        ecs_entity_t c_b = ecs_lookup_child(world, e, "B");
        test_assert(c_b != 0);
    }

    {
        ecs_entity_t c_c = ecs_lookup_child(world, e, "C");
        test_assert(c_c != 0);
    }

    ecs_fini(world);
}
