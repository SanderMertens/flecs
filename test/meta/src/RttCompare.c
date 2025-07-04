#include <meta.h>
#include "flecs.h"

static
void* test_ecs_ensure(ecs_world_t *world, ecs_entity_t entity, ecs_id_t component) {
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    test_assert(ti != NULL);
    return ecs_ensure_id(world, entity, component, ti->size);
}

static
int cmp(const ecs_world_t *world, ecs_entity_t component, ecs_entity_t ea,
          ecs_entity_t eb) {
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);

    const void *a = ecs_get_id(world, ea, component);
    const void *b = ecs_get_id(world, eb, component);

    return ti->hooks.cmp(a, b, ti);
}

static
bool equals(const ecs_world_t *world, ecs_entity_t component, ecs_entity_t ea,
          ecs_entity_t eb) {
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);

    const void *a = ecs_get_id(world, ea, component);
    const void *b = ecs_get_id(world, eb, component);

    return ti->hooks.equals(a, b, ti);
}

typedef struct OpaqueType {
    int16_t value;
} OpaqueType;

int opaque_type_compare(const void *a, const void *b, const ecs_type_info_t* ti) {
    const OpaqueType* op_a = a;
    const OpaqueType* op_b = b;
    return op_a->value - op_b->value;
}

ecs_entity_t define_opaque_type(ecs_world_t *world) {
    ECS_COMPONENT(world, OpaqueType);

    ecs_type_hooks_t hooks = *ecs_get_hooks(world, OpaqueType);
    hooks.cmp = opaque_type_compare;
    hooks.flags &= ~ECS_TYPE_HOOK_CMP_ILLEGAL;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, ecs_id(OpaqueType), &hooks);

    ecs_entity_t descriptor = ecs_struct(world, {
        .members = {
            {.name = "value", .type = ecs_id(ecs_i16_t)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueType),
        .type = {.as_type = descriptor}
    });

    return ecs_id(OpaqueType);
}

void RttCompare_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    StructWithInts *ptr1 = test_ecs_ensure(world, e1, struct_with_ints);
    ptr1->a = 10;
    ptr1->b = 20;

    StructWithInts *ptr2 = test_ecs_ensure(world, e2, struct_with_ints);
    ptr2->a = 10;
    ptr2->b = 25;

    StructWithInts *ptr3 = test_ecs_ensure(world, e3, struct_with_ints);
    ptr3->a = 10;
    ptr3->b = 20;

    /* Test "less" */
    /* {10, 20} < {10, 25} */
    test_assert(cmp(world, struct_with_ints, e1, e2) < 0); 

    /* Test "greater" */
    /* {10, 25} > {10, 20} */
    test_assert(cmp(world, struct_with_ints, e2, e1) > 0); 

    /* Test "equal" */
    /* {10, 20} == {10, 20} */
    test_assert(cmp(world, struct_with_ints, e1, e3) == 0); 
    test_assert(equals(world, struct_with_ints, e1, e3) == true); 
    test_assert(cmp(world, struct_with_ints, e1, e1) == 0); 
    test_assert(equals(world, struct_with_ints, e1, e1) == true); 

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings =
        ecs_struct(world, {.members = {
                               {"a", ecs_id(ecs_string_t)},
                               {"b", ecs_id(ecs_i32_t)},
                               {"c", ecs_id(ecs_string_t)},
                           }});
    ecs_add_id(world, struct_with_strings, EcsSparse);

    /* Create three entities with StructWithStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    StructWithStrings *ptr1 = test_ecs_ensure(world, e1, struct_with_strings);
    ptr1->a = ecs_os_strdup("AA");
    ptr1->b = 20;
    ptr1->c = ecs_os_strdup("CC");

    StructWithStrings *ptr2 = test_ecs_ensure(world, e2, struct_with_strings);
    ptr2->a = ecs_os_strdup("AA");
    ptr2->b = 25;
    ptr2->c = ecs_os_strdup("BB");

    StructWithStrings *ptr3 = test_ecs_ensure(world, e3, struct_with_strings);
    ptr3->a = ecs_os_strdup("AA");
    ptr3->b = 20;
    ptr3->c = ecs_os_strdup("CC");

    /* Test "less" */
    /* {"AA", 20, "CC"} < {"AA", 25, "BB"} */
    test_assert(cmp(world, struct_with_strings, e1, e2) < 0); 

    /* Test "greater" */
    /* {"AA", 25, "BB"} > {"AA", 20, "CC"} */
    test_assert(cmp(world, struct_with_strings, e2, e1) > 0);

    /* Test "equal" */
    /* {"AA", 20, "CC"} == {"AA", 20, "CC"} */
    test_assert(cmp(world, struct_with_strings, e1, e3) == 0);
    test_assert(equals(world, struct_with_strings, e1, e3) == true);
    test_assert(cmp(world, struct_with_strings, e1, e1) == 0);
    test_assert(equals(world, struct_with_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_struct_with_opaque(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        OpaqueType a;
    } StructWithOpaque;

    ecs_entity_t opaque = define_opaque_type(world);

    ecs_entity_t struct_with_opaque = ecs_struct(world, {
        .members = {
            {"a", opaque},
        }
    });

    /* Create three entities with StructWithOpaque component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    StructWithOpaque *ptr1 = test_ecs_ensure(world, e1, struct_with_opaque);
    ptr1->a.value = 10;

    StructWithOpaque *ptr2 = test_ecs_ensure(world, e2, struct_with_opaque);
    ptr2->a.value = 15;

    StructWithOpaque *ptr3 = test_ecs_ensure(world, e3, struct_with_opaque);
    ptr3->a.value = 10;

    /* Test "less" */
    /* {10} < {15} */
    test_assert(cmp(world, struct_with_opaque, e1, e2) < 0);

    /* Test "greater" */
    /* {15} > {10} */
    test_assert(cmp(world, struct_with_opaque, e2, e1) > 0);

    /* Test "equal" */
    /* {10} == {10} */
    test_assert(cmp(world, struct_with_opaque, e1, e3) == 0);
    test_assert(equals(world, struct_with_opaque, e1, e3) == true);
    test_assert(cmp(world, struct_with_opaque, e1, e1) == 0);
    test_assert(equals(world, struct_with_opaque, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_nested_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    typedef struct {
        StructWithStrings a;
        ecs_i32_t b;
        StructWithStrings c;
    } NestedStructWithStrings;

    ecs_entity_t nested_struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", struct_with_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", struct_with_strings},
        }
    });

    /* Create three entities with NestedStructWithStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    NestedStructWithStrings *ptr1 = test_ecs_ensure(world, e1, nested_struct_with_strings);
    ptr1->a.a = ecs_os_strdup("AA");
    ptr1->a.b = 10;
    ptr1->a.c = ecs_os_strdup("BB");
    ptr1->b = 20;
    ptr1->c.a = ecs_os_strdup("CC");
    ptr1->c.b = 30;
    ptr1->c.c = ecs_os_strdup("DD");

    NestedStructWithStrings *ptr2 = test_ecs_ensure(world, e2, nested_struct_with_strings);
    ptr2->a.a = ecs_os_strdup("AA");
    ptr2->a.b = 15;
    ptr2->a.c = ecs_os_strdup("BB");
    ptr2->b = 25;
    ptr2->c.a = ecs_os_strdup("CC");
    ptr2->c.b = 35;
    ptr2->c.c = ecs_os_strdup("DD");

    NestedStructWithStrings *ptr3 = test_ecs_ensure(world, e3, nested_struct_with_strings);
    ptr3->a.a = ecs_os_strdup("AA");
    ptr3->a.b = 10;
    ptr3->a.c = ecs_os_strdup("BB");
    ptr3->b = 20;
    ptr3->c.a = ecs_os_strdup("CC");
    ptr3->c.b = 30;
    ptr3->c.c = ecs_os_strdup("DD");

    /* Test "less" */
    test_assert(cmp(world, nested_struct_with_strings, e1, e2) < 0);

    /* Test "greater" */
    test_assert(cmp(world, nested_struct_with_strings, e2, e1) > 0);

    /* Test "equal" */
    test_assert(cmp(world, nested_struct_with_strings, e1, e3) == 0);
    test_assert(equals(world, nested_struct_with_strings, e1, e3) == true);
    test_assert(cmp(world, nested_struct_with_strings, e1, e1) == 0);
    test_assert(equals(world, nested_struct_with_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_struct_with_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    typedef struct {
        ecs_string_t a[3];
        ecs_i32_t b;
    } StructWithArrayOfStrings;

    ecs_entity_t struct_with_array_of_strings = ecs_struct(world, {
        .members = {
            {"a", array_of_strings},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    /* Create five entities with StructWithArrayOfStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    StructWithArrayOfStrings *ptr1 = test_ecs_ensure(world, e1,
        struct_with_array_of_strings);
    ptr1->a[0] = ecs_os_strdup("AA");
    ptr1->a[1] = ecs_os_strdup("BB");
    ptr1->a[2] = ecs_os_strdup("CC");
    ptr1->b = 10;

    StructWithArrayOfStrings *ptr2 = test_ecs_ensure(world, e2,
        struct_with_array_of_strings);
    ptr2->a[0] = ecs_os_strdup("AA");
    ptr2->a[1] = ecs_os_strdup("BB");
    ptr2->a[2] = ecs_os_strdup("CC");
    ptr2->b = 20;

    StructWithArrayOfStrings *ptr3 = test_ecs_ensure(world, e3,
        struct_with_array_of_strings);
    ptr3->a[0] = ecs_os_strdup("AA");
    ptr3->a[1] = ecs_os_strdup("BB");
    ptr3->a[2] = ecs_os_strdup("CC");
    ptr3->b = 10;

    StructWithArrayOfStrings *ptr4 = test_ecs_ensure(world, e4,
        struct_with_array_of_strings);
    ptr4->a[0] = ecs_os_strdup("AA");
    ptr4->a[1] = ecs_os_strdup("ZZ");
    ptr4->a[2] = ecs_os_strdup("CC");
    ptr4->b = 10;

    StructWithArrayOfStrings *ptr5 = test_ecs_ensure(world, e5,
        struct_with_array_of_strings);
    ptr5->a[0] = ecs_os_strdup("AA");
    ptr5->a[1] = ecs_os_strdup("AA");
    ptr5->a[2] = ecs_os_strdup("DD");
    ptr5->b = 15;

    /* Test "less" */
    /* {"AA", "BB", "CC", 10} < {"AA", "BB", "CC", 20} */
    test_assert(cmp(world, struct_with_array_of_strings, e1, e2) < 0); 

    /* {"AA", "BB", "CC", 10} < {"AA", "ZZ", "CC", 10} */
    test_assert(cmp(world, struct_with_array_of_strings, e1, e4) < 0);

    /* {"AA", "AA", "DD", 15} < {"AA", "BB", "CC", 20} */
    test_assert(cmp(world, struct_with_array_of_strings, e5, e2) < 0);

    /* Test "greater" */
    /* {"AA", "BB", "CC", 20} > {"AA", "BB", "CC", 10} */
    test_assert(cmp(world, struct_with_array_of_strings, e2, e1) > 0);

    /* {"AA", "ZZ", "CC", 10} > {"AA", "BB", "CC", 10} */
    test_assert(cmp(world, struct_with_array_of_strings, e4, e1) > 0);

    /* {"AA", "BB", "CC", 20} > {"AA", "AA", "DD", 15} */
    test_assert(cmp(world, struct_with_array_of_strings, e2, e5) > 0);

    /* Test "equal" */
    /* {"AA", "BB", "CC", 10} == {"AA", "BB", "CC", 10} */
    test_assert(cmp(world, struct_with_array_of_strings, e1, e3) == 0); 
    test_assert(equals(world, struct_with_array_of_strings, e1, e3) == true); 
    test_assert(cmp(world, struct_with_array_of_strings, e1, e1) == 0);
    test_assert(equals(world, struct_with_array_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_struct_with_array_of_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* ecs_string_t[3][3] */ array_of_array_of_strings =
        ecs_array(world, {.type = array_of_strings, .count = 3});

    typedef struct {
        ecs_string_t a[3][3];
        ecs_string_t b;
    } StructWithArrayOfArrayOfStrings;

    ecs_entity_t struct_with_array_of_array_of_strings = ecs_struct(world, {
        .members = {
            {"a", array_of_array_of_strings},
            {"b", ecs_id(ecs_string_t)},
        }
    });

    /* Create five entities with StructWithArrayOfArrayOfStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    StructWithArrayOfArrayOfStrings *ptr1 = test_ecs_ensure(world, e1,
        struct_with_array_of_array_of_strings);
    for (int i = 0; i < 3; i++) {
        ptr1->a[i][0] = ecs_os_strdup("AA");
        ptr1->a[i][1] = ecs_os_strdup("BB");
        ptr1->a[i][2] = ecs_os_strdup("CC");
    }
    ptr1->b = ecs_os_strdup("DD");

    StructWithArrayOfArrayOfStrings *ptr2 = test_ecs_ensure(world, e2,
        struct_with_array_of_array_of_strings);
    for (int i = 0; i < 3; i++) {
        ptr2->a[i][0] = ecs_os_strdup("AA");
        ptr2->a[i][1] = ecs_os_strdup("BB");
        ptr2->a[i][2] = ecs_os_strdup("CC");
    }
    ptr2->b = ecs_os_strdup("EE");

    StructWithArrayOfArrayOfStrings *ptr3 = test_ecs_ensure(world, e3,
        struct_with_array_of_array_of_strings);
    for (int i = 0; i < 3; i++) {
        ptr3->a[i][0] = ecs_os_strdup("AA");
        ptr3->a[i][1] = ecs_os_strdup("BB");
        ptr3->a[i][2] = ecs_os_strdup("CC");
    }
    ptr3->b = ecs_os_strdup("DD");

    StructWithArrayOfArrayOfStrings *ptr4 = test_ecs_ensure(world, e4,
        struct_with_array_of_array_of_strings);
    for (int i = 0; i < 3; i++) {
        ptr4->a[i][0] = ecs_os_strdup("AA");
        ptr4->a[i][1] = ecs_os_strdup("ZZ");
        ptr4->a[i][2] = ecs_os_strdup("CC");
    }
    ptr4->b = ecs_os_strdup("DD");

    StructWithArrayOfArrayOfStrings *ptr5 = test_ecs_ensure(world, e5,
        struct_with_array_of_array_of_strings);
    for (int i = 0; i < 3; i++) {
        ptr5->a[i][0] = ecs_os_strdup("XX");
        ptr5->a[i][1] = ecs_os_strdup("BB");
        ptr5->a[i][2] = ecs_os_strdup("YY");
    }
    ptr5->b = ecs_os_strdup("FF");

    /* Test "less" */
    /* {"AA", "BB", "CC", "DD"} < {"AA", "BB", "CC", "EE"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e1, e2) < 0);

    /* {"AA", "BB", "CC", "DD"} < {"AA", "ZZ", "CC", "DD"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e1, e4) < 0);

    /* {"AA", "BB", "CC", "DD"} < {"XX", "BB", "YY", "FF"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e1, e5) < 0);

    /* Test "greater" */
    /* {"AA", "BB", "CC", "EE"} > {"AA", "BB", "CC", "DD"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e2, e1) > 0);

    /* {"AA", "ZZ", "CC", "DD"} > {"AA", "BB", "CC", "DD"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e4, e1) > 0);

    /* {"XX", "BB", "YY", "FF"} > {"AA", "BB", "CC", "DD"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e5, e1) > 0);

    /* Test "equal" */
    /* {"AA", "BB", "CC", "DD"} == {"AA", "BB", "CC", "DD"} */
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e1, e3) == 0);
    test_assert(equals(world, struct_with_array_of_array_of_strings, e1, e3) == true);
    test_assert(cmp(world, struct_with_array_of_array_of_strings, e1, e1) == 0);
    test_assert(equals(world, struct_with_array_of_array_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_struct_with_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    typedef struct {
        ecs_vec_t a;
    } StructWithVectorOfInts;

    ecs_entity_t struct_with_vector_of_ints = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
        }
    });

    /* Create four entities with StructWithVectorOfInts component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    StructWithVectorOfInts *ptr1 = test_ecs_ensure(world, e1, struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v1 = ecs_vec_first(&ptr1->a);
    v1[0] = 10;
    v1[1] = 20;
    v1[2] = 30;

    StructWithVectorOfInts *ptr2 = test_ecs_ensure(world, e2, struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr2->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v2 = ecs_vec_first(&ptr2->a);
    v2[0] = 15;
    v2[1] = 25;
    v2[2] = 35;

    StructWithVectorOfInts *ptr3 = test_ecs_ensure(world, e3, struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr3->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v3 = ecs_vec_first(&ptr3->a);
    v3[0] = 10;
    v3[1] = 20;
    v3[2] = 30;

    StructWithVectorOfInts *ptr4 = test_ecs_ensure(world, e4, struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr4->a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *v4 = ecs_vec_first(&ptr4->a);
    v4[0] = 10;
    v4[1] = 20;

    /* Test "less" */
    /* {10, 20, 30} < {15, 25, 35} */
    test_assert(cmp(world, struct_with_vector_of_ints, e1, e2) < 0);

    /* {10, 20} < {10, 20, 30} (because fewer elements) */
    test_assert(cmp(world, struct_with_vector_of_ints, e4, e1) < 0);

    /* Test "greater" */
    /* {15, 25, 35} > {10, 20, 30} */
    test_assert(cmp(world, struct_with_vector_of_ints, e2, e1) > 0);

    /* {10, 20, 30} > {10, 20} (because more elements) */
    test_assert(cmp(world, struct_with_vector_of_ints, e1, e4) > 0);

    /* Test "equal" */
    /* {10, 20, 30} == {10, 20, 30} */
    test_assert(cmp(world, struct_with_vector_of_ints, e1, e3) == 0);
    test_assert(equals(world, struct_with_vector_of_ints, e1, e3) == true);
    test_assert(cmp(world, struct_with_vector_of_ints, e1, e1) == 0);
    test_assert(equals(world, struct_with_vector_of_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);

    ecs_fini(world);
}

void RttCompare_struct_with_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    typedef struct {
        ecs_vec_t a;
    } StructWithVectorOfStrings;

    ecs_entity_t struct_with_vector_of_strings = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
        }
    });

    /* Create four entities with StructWithVectorOfStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    StructWithVectorOfStrings *ptr1 = test_ecs_ensure(world, e1, struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_string_t), 3);
    ecs_string_t *va1 = ecs_vec_first(&ptr1->a);
    va1[0] = ecs_os_strdup("AA");
    va1[1] = ecs_os_strdup("BB");
    va1[2] = ecs_os_strdup("CC");

    StructWithVectorOfStrings *ptr2 = test_ecs_ensure(world, e2, struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr2->a, sizeof(ecs_string_t), 3);
    ecs_string_t *va2 = ecs_vec_first(&ptr2->a);
    va2[0] = ecs_os_strdup("AA");
    va2[1] = ecs_os_strdup("BB");
    va2[2] = ecs_os_strdup("DD");

    StructWithVectorOfStrings *ptr3 = test_ecs_ensure(world, e3, struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr3->a, sizeof(ecs_string_t), 3);
    ecs_string_t *va3 = ecs_vec_first(&ptr3->a);
    va3[0] = ecs_os_strdup("AA");
    va3[1] = ecs_os_strdup("BB");
    va3[2] = ecs_os_strdup("CC");

    StructWithVectorOfStrings *ptr4 = test_ecs_ensure(world, e4, struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr4->a, sizeof(ecs_string_t), 2);
    ecs_string_t *va4 = ecs_vec_first(&ptr4->a);
    va4[0] = ecs_os_strdup("AA");
    va4[1] = ecs_os_strdup("BB");

    /* Test "less" */
    /* {"AA", "BB", "CC"} < {"AA", "BB", "DD"} */
    test_assert(cmp(world, struct_with_vector_of_strings, e1, e2) < 0);

    /* {"AA", "BB"} < {"AA", "BB", "CC"} (because fewer elements) */
    test_assert(cmp(world, struct_with_vector_of_strings, e4, e1) < 0);

    /* Test "greater" */
    /* {"AA", "BB", "DD"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, struct_with_vector_of_strings, e2, e1) > 0);

    /* {"AA", "BB", "CC"} > {"AA", "BB"} (because more elements) */
    test_assert(cmp(world, struct_with_vector_of_strings, e1, e4) > 0);

    /* Test "equal" */
    /* {"AA", "BB", "CC"} == {"AA", "BB", "CC"} */
    test_assert(cmp(world, struct_with_vector_of_strings, e1, e3) == 0);
    test_assert(equals(world, struct_with_vector_of_strings, e1, e3) == true);
    test_assert(cmp(world, struct_with_vector_of_strings, e1, e1) == 0);
    test_assert(equals(world, struct_with_vector_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);

    ecs_fini(world);
}

void RttCompare_nested_struct_with_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        ecs_vec_t c;
    } InnerStruct1;

    ecs_entity_t inner_struct_1 = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
            {"b", ecs_id(ecs_i32_t)},
            {"c", vector_of_ints},
        }
    });

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        InnerStruct1 c;
    } NestedStructWithVectorOfInts;

    ecs_entity_t nested_struct_with_vector_of_ints = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
            {"b", ecs_id(ecs_i32_t)},
            {"c", inner_struct_1},
        }
    });

    /* Create five entities with NestedStructWithVectorOfInts component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    /* Entity e1 */
    NestedStructWithVectorOfInts *ptr1 = test_ecs_ensure(world, e1,
        nested_struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va1 = ecs_vec_first(&ptr1->a);
    va1[0] = 1;
    va1[1] = 2;
    va1[2] = 3;
    ptr1->b = 10;

    ecs_vec_set_count(NULL, &ptr1->c.a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *vca1 = ecs_vec_first(&ptr1->c.a);
    vca1[0] = 4;
    vca1[1] = 5;
    ptr1->c.b = 15;

    ecs_vec_set_count(NULL, &ptr1->c.c, sizeof(ecs_i32_t), 1);
    ecs_i32_t *vcc1 = ecs_vec_first(&ptr1->c.c);
    vcc1[0] = 6;

    /* Entity e2 */
    NestedStructWithVectorOfInts *ptr2 = test_ecs_ensure(world, e2,
        nested_struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr2->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va2 = ecs_vec_first(&ptr2->a);
    va2[0] = 1;
    va2[1] = 2;
    va2[2] = 3;
    ptr2->b = 20;

    ecs_vec_set_count(NULL, &ptr2->c.a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *vca2 = ecs_vec_first(&ptr2->c.a);
    vca2[0] = 4;
    vca2[1] = 5;
    ptr2->c.b = 15;

    ecs_vec_set_count(NULL, &ptr2->c.c, sizeof(ecs_i32_t), 1);
    ecs_i32_t *vcc2 = ecs_vec_first(&ptr2->c.c);
    vcc2[0] = 6;

    /* Entity e3 */
    NestedStructWithVectorOfInts *ptr3 = test_ecs_ensure(world, e3,
        nested_struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr3->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va3 = ecs_vec_first(&ptr3->a);
    va3[0] = 1;
    va3[1] = 2;
    va3[2] = 3;
    ptr3->b = 10;

    ecs_vec_set_count(NULL, &ptr3->c.a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *vca3 = ecs_vec_first(&ptr3->c.a);
    vca3[0] = 4;
    vca3[1] = 5;
    ptr3->c.b = 15;

    ecs_vec_set_count(NULL, &ptr3->c.c, sizeof(ecs_i32_t), 1);
    ecs_i32_t *vcc3 = ecs_vec_first(&ptr3->c.c);
    vcc3[0] = 6;

    /* Entity e4 - different vector values */
    NestedStructWithVectorOfInts *ptr4 = test_ecs_ensure(world, e4,
        nested_struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr4->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va4 = ecs_vec_first(&ptr4->a);
    va4[0] = 3;
    va4[1] = 2;
    va4[2] = 1;
    ptr4->b = 10;

    ecs_vec_set_count(NULL, &ptr4->c.a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *vca4 = ecs_vec_first(&ptr4->c.a);
    vca4[0] = 4;
    vca4[1] = 5;
    ptr4->c.b = 15;

    ecs_vec_set_count(NULL, &ptr4->c.c, sizeof(ecs_i32_t), 1);
    ecs_i32_t *vcc4 = ecs_vec_first(&ptr4->c.c);
    vcc4[0] = 6;

    /* Entity e5 - different nested structure values */
    NestedStructWithVectorOfInts *ptr5 = test_ecs_ensure(world, e5,
        nested_struct_with_vector_of_ints);
    ecs_vec_set_count(NULL, &ptr5->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va5 = ecs_vec_first(&ptr5->a);
    va5[0] = 1;
    va5[1] = 2;
    va5[2] = 3;
    ptr5->b = 30;

    ecs_vec_set_count(NULL, &ptr5->c.a, sizeof(ecs_i32_t), 2);
    ecs_i32_t *vca5 = ecs_vec_first(&ptr5->c.a);
    vca5[0] = 7;
    vca5[1] = 8;
    ptr5->c.b = 25;

    ecs_vec_set_count(NULL, &ptr5->c.c, sizeof(ecs_i32_t), 1);
    ecs_i32_t *vcc5 = ecs_vec_first(&ptr5->c.c);
    vcc5[0] = 9;

    /* Test "less" */
    /* {1, 2, 3, 10, {4, 5, 15, 6}} < {1, 2, 3, 20, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e1, e2) < 0); 
    /* {1, 2, 3, 10, {4, 5, 15, 6}} < {3, 2, 1, 10, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e1, e4) < 0); 
    /* {1, 2, 3, 10, {4, 5, 15, 6}} < {1, 2, 3, 30, {7, 8, 25, 9}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e1, e5) < 0); 

    /* Test "greater" */
    /* {1, 2, 3, 20, {4, 5, 15, 6}} > {1, 2, 3, 10, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e2, e1) > 0); 
    /* {3, 2, 1, 10, {4, 5, 15, 6}} > {1, 2, 3, 10, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e4, e1) > 0);
    /* {1, 2, 3, 30, {7, 8, 25, 9}} > {1, 2, 3, 10, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e5, e1) > 0);

    /* Test "equal" */
    /* {1, 2, 3, 10, {4, 5, 15, 6}} == {1, 2, 3, 10, {4, 5, 15, 6}} */
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e1, e3) == 0); 
    test_assert(equals(world, nested_struct_with_vector_of_ints, e1, e3) == true); 
    test_assert(cmp(world, nested_struct_with_vector_of_ints, e1, e1) == 0);
    test_assert(equals(world, nested_struct_with_vector_of_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_nested_struct_with_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    typedef struct {
        ecs_vec_t a; /* vector<ecs_string_t> */
        ecs_i32_t b;
        ecs_vec_t c; /* vector<ecs_string_t> */
    } InnerStruct2;

    ecs_entity_t inner_struct_2 = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", vector_of_strings},
        }
    });

    typedef struct {
        ecs_vec_t a; /* vector<ecs_string_t> */
        ecs_i32_t b;
        InnerStruct2 c;
    } NestedStructWithVectorOfStrings;

    ecs_entity_t nested_struct_with_vector_of_strings = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", inner_struct_2},
        }
    });

    /* Create five entities with NestedStructWithVectorOfStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    NestedStructWithVectorOfStrings *ptr1 = test_ecs_ensure(world, e1,
        nested_struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_string_t), 3);
    ecs_string_t *a1 = ecs_vec_first(&ptr1->a);
    a1[0] = ecs_os_strdup("AA");
    a1[1] = ecs_os_strdup("BB");
    a1[2] = ecs_os_strdup("CC");
    ptr1->b = 10;
    ecs_vec_set_count(NULL, &ptr1->c.a, sizeof(ecs_string_t), 2);
    ecs_string_t *c_a1 = ecs_vec_first(&ptr1->c.a);
    c_a1[0] = ecs_os_strdup("XX");
    c_a1[1] = ecs_os_strdup("YY");
    ptr1->c.b = 5;
    ecs_vec_set_count(NULL, &ptr1->c.c, sizeof(ecs_string_t), 1);
    ecs_string_t *c_c1 = ecs_vec_first(&ptr1->c.c);
    c_c1[0] = ecs_os_strdup("ZZ");

    NestedStructWithVectorOfStrings *ptr2 = test_ecs_ensure(world, e2,
        nested_struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr2->a, sizeof(ecs_string_t), 3);
    ecs_string_t *a2 = ecs_vec_first(&ptr2->a);
    a2[0] = ecs_os_strdup("AA");
    a2[1] = ecs_os_strdup("BB");
    a2[2] = ecs_os_strdup("CC");
    ptr2->b = 15;
    ecs_vec_set_count(NULL, &ptr2->c.a, sizeof(ecs_string_t), 2);
    ecs_string_t *c_a2 = ecs_vec_first(&ptr2->c.a);
    c_a2[0] = ecs_os_strdup("XX");
    c_a2[1] = ecs_os_strdup("YY");
    ptr2->c.b = 5;
    ecs_vec_set_count(NULL, &ptr2->c.c, sizeof(ecs_string_t), 1);
    ecs_string_t *c_c2 = ecs_vec_first(&ptr2->c.c);
    c_c2[0] = ecs_os_strdup("ZZ");

    NestedStructWithVectorOfStrings *ptr3 = test_ecs_ensure(world, e3,
        nested_struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr3->a, sizeof(ecs_string_t), 3);
    ecs_string_t *a3 = ecs_vec_first(&ptr3->a);
    a3[0] = ecs_os_strdup("AA");
    a3[1] = ecs_os_strdup("BB");
    a3[2] = ecs_os_strdup("CC");
    ptr3->b = 10;
    ecs_vec_set_count(NULL, &ptr3->c.a, sizeof(ecs_string_t), 2);
    ecs_string_t *c_a3 = ecs_vec_first(&ptr3->c.a);
    c_a3[0] = ecs_os_strdup("XX");
    c_a3[1] = ecs_os_strdup("YY");
    ptr3->c.b = 5;
    ecs_vec_set_count(NULL, &ptr3->c.c, sizeof(ecs_string_t), 1);
    ecs_string_t *c_c3 = ecs_vec_first(&ptr3->c.c);
    c_c3[0] = ecs_os_strdup("ZZ");

    NestedStructWithVectorOfStrings *ptr4 = test_ecs_ensure(world, e4,
        nested_struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr4->a, sizeof(ecs_string_t), 2);
    ecs_string_t *a4 = ecs_vec_first(&ptr4->a);
    a4[0] = ecs_os_strdup("AA");
    a4[1] = ecs_os_strdup("DD");
    ptr4->b = 20;
    ecs_vec_set_count(NULL, &ptr4->c.a, sizeof(ecs_string_t), 3);
    ecs_string_t *c_a4 = ecs_vec_first(&ptr4->c.a);
    c_a4[0] = ecs_os_strdup("XX");
    c_a4[1] = ecs_os_strdup("YY");
    c_a4[2] = ecs_os_strdup("ZZ");
    ptr4->c.b = 10;
    ecs_vec_set_count(NULL, &ptr4->c.c, sizeof(ecs_string_t), 2);
    ecs_string_t *c_c4 = ecs_vec_first(&ptr4->c.c);
    c_c4[0] = ecs_os_strdup("WW");
    c_c4[1] = ecs_os_strdup("QQ");

    NestedStructWithVectorOfStrings *ptr5 = test_ecs_ensure(world, e5,
        nested_struct_with_vector_of_strings);
    ecs_vec_set_count(NULL, &ptr5->a, sizeof(ecs_string_t), 1);
    ecs_string_t *a5 = ecs_vec_first(&ptr5->a);
    a5[0] = ecs_os_strdup("EE");
    ptr5->b = 5;
    ecs_vec_set_count(NULL, &ptr5->c.a, sizeof(ecs_string_t), 2);
    ecs_string_t *c_a5 = ecs_vec_first(&ptr5->c.a);
    c_a5[0] = ecs_os_strdup("MM");
    c_a5[1] = ecs_os_strdup("NN");
    ptr5->c.b = 8;
    ecs_vec_set_count(NULL, &ptr5->c.c, sizeof(ecs_string_t), 3);
    ecs_string_t *c_c5 = ecs_vec_first(&ptr5->c.c);
    c_c5[0] = ecs_os_strdup("OO");
    c_c5[1] = ecs_os_strdup("PP");
    c_c5[2] = ecs_os_strdup("RR");

    /* Test "less" */
    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} < */
    /* {{"AA", "BB", "CC"}, 15, {{"XX", "YY"}, 5, {"ZZ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e1, e2) < 0);

    /* {{"EE"}, 5, {{"MM", "NN"}, 8, {"OO", "PP", "RR"}}} < */
    /* {{"AA", "BB", "CC"}, 15, {{"XX", "YY"}, 5, {"ZZ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e5, e2) < 0);

    /* {{"AA", "DD"}, 20, {{"XX", "YY", "ZZ"}, 10, {"WW", "QQ"}}} > */
    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e4, e1) < 0);

    /* Test "greater" */
    /* {{"AA", "BB", "CC"}, 15, {{"XX", "YY"}, 5, {"ZZ"}}} > */
    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e2, e1) > 0);

    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} < */
    /* {{"AA", "DD"}, 20, {{"XX", "YY", "ZZ"}, 10, {"WW", "QQ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e1, e4) > 0);


    /* {{"AA", "BB", "CC"}, 15, {{"XX", "YY"}, 5, {"ZZ"}}} > */
    /* {{"EE"}, 5, {{"MM", "NN"}, 8, {"OO", "PP", "RR"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e2, e5) > 0);

    /* Test "equal" */
    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} == */
    /* {{"AA", "BB", "CC"}, 10, {{"XX", "YY"}, 5, {"ZZ"}}} */
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e1, e3) == 0);
    test_assert(equals(world, nested_struct_with_vector_of_strings, e1, e3) == true);
    test_assert(cmp(world, nested_struct_with_vector_of_strings, e1, e1) == 0);
    test_assert(equals(world, nested_struct_with_vector_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_i32_t[3] */ array_of_ints =
        ecs_array(world, {.type = ecs_id(ecs_i32_t), .count = 3});

    /* Create five entities, each with an array of integers component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_i32_t *arr1 = test_ecs_ensure(world, e1, array_of_ints);
    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    ecs_i32_t *arr2 = test_ecs_ensure(world, e2, array_of_ints);
    arr2[0] = 1;
    arr2[1] = 2;
    arr2[2] = 4;

    ecs_i32_t *arr3 = test_ecs_ensure(world, e3, array_of_ints);
    arr3[0] = 1;
    arr3[1] = 2;
    arr3[2] = 3;

    ecs_i32_t *arr4 = test_ecs_ensure(world, e4, array_of_ints);
    arr4[0] = 0;
    arr4[1] = 5;
    arr4[2] = 6;

    ecs_i32_t *arr5 = test_ecs_ensure(world, e5, array_of_ints);
    arr5[0] = 1;
    arr5[1] = 2;
    arr5[2] = 2;

    /* Test "less" */
    /* {1, 2, 3} < {1, 2, 4} */
    test_assert(cmp(world, array_of_ints, e1, e2) < 0);

    /* {0, 5, 6} < {1, 2, 3} */
    test_assert(cmp(world, array_of_ints, e4, e1) < 0);

    /* Test "greater" */
    /* {1, 2, 4} > {1, 2, 3} */
    test_assert(cmp(world, array_of_ints, e2, e1) > 0);

    /* {1, 2, 3} > {1, 2, 2} */
    test_assert(cmp(world, array_of_ints, e1, e5) > 0);

    /* Test "equal" */
    /* {1, 2, 3} == {1, 2, 3} */
    test_assert(cmp(world, array_of_ints, e1, e3) == 0);
    test_assert(equals(world, array_of_ints, e1, e3) == true);
    test_assert(cmp(world, array_of_ints, e1, e1) == 0);
    test_assert(equals(world, array_of_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    /* Create five entities, each with an array of strings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_string_t *arr1 = test_ecs_ensure(world, e1, array_of_strings);
    arr1[0] = ecs_os_strdup("AA");
    arr1[1] = ecs_os_strdup("BB");
    arr1[2] = ecs_os_strdup("CC");

    ecs_string_t *arr2 = test_ecs_ensure(world, e2, array_of_strings);
    arr2[0] = ecs_os_strdup("AA");
    arr2[1] = ecs_os_strdup("BB");
    arr2[2] = ecs_os_strdup("DD");

    ecs_string_t *arr3 = test_ecs_ensure(world, e3, array_of_strings);
    arr3[0] = ecs_os_strdup("AA");
    arr3[1] = ecs_os_strdup("BB");
    arr3[2] = ecs_os_strdup("CC");

    ecs_string_t *arr4 = test_ecs_ensure(world, e4, array_of_strings);
    arr4[0] = ecs_os_strdup("ZZ");
    arr4[1] = ecs_os_strdup("YY");
    arr4[2] = ecs_os_strdup("XX");

    ecs_string_t *arr5 = test_ecs_ensure(world, e5, array_of_strings);
    arr5[0] = ecs_os_strdup("AA");
    arr5[1] = ecs_os_strdup("AB");
    arr5[2] = ecs_os_strdup("AC");

    /* Test "less" */
    /* {"AA", "BB", "CC"} < {"AA", "BB", "DD"} */
    test_assert(cmp(world, array_of_strings, e1, e2) < 0);

    /* {"AA", "AB", "AC"} < {"AA", "BB", "CC"} */
    test_assert(cmp(world, array_of_strings, e5, e1) < 0);

    /* Test "greater" */
    /* {"ZZ", "YY", "XX"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, array_of_strings, e4, e1) > 0);

    /* {"AA", "BB", "DD"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, array_of_strings, e2, e1) > 0);

    /* Test "equal" */
    /* {"AA", "BB", "CC"} == {"AA", "BB", "CC"} */
    test_assert(cmp(world, array_of_strings, e1, e3) == 0);
    test_assert(equals(world, array_of_strings, e1, e3) == true);
    test_assert(cmp(world, array_of_strings, e1, e1) == 0);
    test_assert(equals(world, array_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t /* StructWithInts[3] */ array_of_struct_with_ints =
        ecs_array(world, {.type = struct_with_ints, .count = 3});

    /* Create five entities with array of StructWithInts component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    StructWithInts *arr1 = test_ecs_ensure(world, e1, array_of_struct_with_ints);
    arr1[0].a = 1; arr1[0].b = 2;
    arr1[1].a = 3; arr1[1].b = 4;
    arr1[2].a = 5; arr1[2].b = 6;

    StructWithInts *arr2 = test_ecs_ensure(world, e2, array_of_struct_with_ints);
    arr2[0].a = 1; arr2[0].b = 2;
    arr2[1].a = 3; arr2[1].b = 4;
    arr2[2].a = 5; arr2[2].b = 10;

    StructWithInts *arr3 = test_ecs_ensure(world, e3, array_of_struct_with_ints);
    arr3[0].a = 1; arr3[0].b = 2;
    arr3[1].a = 3; arr3[1].b = 4;
    arr3[2].a = 5; arr3[2].b = 6;

    StructWithInts *arr4 = test_ecs_ensure(world, e4, array_of_struct_with_ints);
    arr4[0].a = 0; arr4[0].b = 1;
    arr4[1].a = 2; arr4[1].b = 3;
    arr4[2].a = 4; arr4[2].b = 5;

    StructWithInts *arr5 = test_ecs_ensure(world, e5, array_of_struct_with_ints);
    arr5[0].a = 7; arr5[0].b = 8;
    arr5[1].a = 9; arr5[1].b = 10;
    arr5[2].a = 11; arr5[2].b = 12;

    /* Test "less" */
    /* {{1, 2}, {3, 4}, {5, 6}} < {{1, 2}, {3, 4}, {5, 10}} */
    test_assert(cmp(world, array_of_struct_with_ints, e1, e2) < 0); 

    /* {{0, 1}, {2, 3}, {4, 5}} < {{1, 2}, {3, 4}, {5, 6}} */
    test_assert(cmp(world, array_of_struct_with_ints, e4, e1) < 0);

    /* Test "greater" */
    /* {{1, 2}, {3, 4}, {5, 10}} > {{1, 2}, {3, 4}, {5, 6}} */
    test_assert(cmp(world, array_of_struct_with_ints, e2, e1) > 0);

    /* {{7, 8}, {9, 10}, {11, 12}} > {{1, 2}, {3, 4}, {5, 6}} */
    test_assert(cmp(world, array_of_struct_with_ints, e5, e1) > 0);

    /* Test "equal" */
    /* {{1, 2}, {3, 4}, {5, 6}} == {{1, 2}, {3, 4}, {5, 6}} */
    test_assert(cmp(world, array_of_struct_with_ints, e1, e3) == 0); 
    test_assert(equals(world, array_of_struct_with_ints, e1, e3) == true); 
    test_assert(cmp(world, array_of_struct_with_ints, e1, e1) == 0);
    test_assert(equals(world, array_of_struct_with_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* StructWithStrings[3] */ array_of_struct_with_strings =
        ecs_array(world, {.type = struct_with_strings, .count = 3});

    typedef struct {
        StructWithStrings items[3];
    } ArrayOfStructWithStrings;

    ecs_entity_t struct_array_entity = ecs_struct(world, {
        .members = {
            {"items", array_of_struct_with_strings},
        }
    });

    /* Create five entities with ArrayOfStructWithStrings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    /* Initialize e1 */
    ArrayOfStructWithStrings *ptr1 = test_ecs_ensure(world, e1, struct_array_entity);
    ptr1->items[0].a = ecs_os_strdup("AA");
    ptr1->items[0].b = 10;
    ptr1->items[0].c = ecs_os_strdup("BB");
    ptr1->items[1].a = ecs_os_strdup("CC");
    ptr1->items[1].b = 15;
    ptr1->items[1].c = ecs_os_strdup("DD");
    ptr1->items[2].a = ecs_os_strdup("EE");
    ptr1->items[2].b = 20;
    ptr1->items[2].c = ecs_os_strdup("FF");

    /* Initialize e2 with different value in the second struct */
    ArrayOfStructWithStrings *ptr2 = test_ecs_ensure(world, e2, struct_array_entity);
    ptr2->items[0].a = ecs_os_strdup("AA");
    ptr2->items[0].b = 10;
    ptr2->items[0].c = ecs_os_strdup("BB");
    ptr2->items[1].a = ecs_os_strdup("CC");
    ptr2->items[1].b = 18; // Different value
    ptr2->items[1].c = ecs_os_strdup("DD");
    ptr2->items[2].a = ecs_os_strdup("EE");
    ptr2->items[2].b = 20;
    ptr2->items[2].c = ecs_os_strdup("FF");

    /* Initialize e3 identical to e1 */
    ArrayOfStructWithStrings *ptr3 = test_ecs_ensure(world, e3, struct_array_entity);
    ptr3->items[0].a = ecs_os_strdup("AA");
    ptr3->items[0].b = 10;
    ptr3->items[0].c = ecs_os_strdup("BB");
    ptr3->items[1].a = ecs_os_strdup("CC");
    ptr3->items[1].b = 15;
    ptr3->items[1].c = ecs_os_strdup("DD");
    ptr3->items[2].a = ecs_os_strdup("EE");
    ptr3->items[2].b = 20;
    ptr3->items[2].c = ecs_os_strdup("FF");

    /* Initialize e4 with completely different strings */
    ArrayOfStructWithStrings *ptr4 = test_ecs_ensure(world, e4, struct_array_entity);
    ptr4->items[0].a = ecs_os_strdup("XX");
    ptr4->items[0].b = 5;
    ptr4->items[0].c = ecs_os_strdup("YY");
    ptr4->items[1].a = ecs_os_strdup("ZZ");
    ptr4->items[1].b = 25;
    ptr4->items[1].c = ecs_os_strdup("WW");
    ptr4->items[2].a = ecs_os_strdup("VV");
    ptr4->items[2].b = 30;
    ptr4->items[2].c = ecs_os_strdup("UU");

    /* Initialize e5 with mixture of similar and different values */
    ArrayOfStructWithStrings *ptr5 = test_ecs_ensure(world, e5, struct_array_entity);
    ptr5->items[0].a = ecs_os_strdup("AA");
    ptr5->items[0].b = 10;
    ptr5->items[0].c = ecs_os_strdup("GG"); // Different from e1
    ptr5->items[1].a = ecs_os_strdup("CC");
    ptr5->items[1].b = 15;
    ptr5->items[1].c = ecs_os_strdup("HH"); // Different from e1
    ptr5->items[2].a = ecs_os_strdup("EE");
    ptr5->items[2].b = 20;
    ptr5->items[2].c = ecs_os_strdup("FF");

    /* Test "less" */
    test_assert(cmp(world, struct_array_entity, e1, e2) < 0);/*15 < 18 */
    test_assert(cmp(world, struct_array_entity, e5, e4) < 0);/*"AA" < "XX" */
    test_assert(cmp(world, struct_array_entity, e1, e4) < 0);/*"CC" < "ZZ" */

    /* Test "greater" */
    /* e2 > e1 because ptr2->items[1].b (18) > ptr1->items[1].b (15) */
    test_assert(cmp(world, struct_array_entity, e2, e1) > 0);/* 18 > 15 */
    test_assert(cmp(world, struct_array_entity, e5, e1) > 0);/* "GG" > "BB"*/

    /* e4 > e1 because  */
    test_assert(cmp(world, struct_array_entity, e4, e1) > 0);/* "XX" > "AA"*/

    /* Test "equal" */
    test_assert(cmp(world, struct_array_entity, e1, e3) == 0);
    test_assert(equals(world, struct_array_entity, e1, e3) == true);
    test_assert(cmp(world, struct_array_entity, e1, e1) == 0);
    test_assert(equals(world, struct_array_entity, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_struct_with_opaques(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t opaque = define_opaque_type(world);

    typedef struct {
        OpaqueType a;
    } StructWithOpaque;

    ecs_entity_t struct_with_opaque = ecs_struct(world, {
        .members = {
            {"a", opaque},
        }
    });

    ecs_entity_t /* StructWithOpaque[3] */ array_of_struct_with_opaques =
        ecs_array(world, {.type = struct_with_opaque, .count = 3});

    /* Create three entities with array of StructWithOpaque component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    StructWithOpaque *arr1 = test_ecs_ensure(world, e1, array_of_struct_with_opaques);
    arr1[0].a.value = 5;
    arr1[1].a.value = 10;
    arr1[2].a.value = 15;

    StructWithOpaque *arr2 = test_ecs_ensure(world, e2, array_of_struct_with_opaques);
    arr2[0].a.value = 5;
    arr2[1].a.value = 15;
    arr2[2].a.value = 20;

    StructWithOpaque *arr3 = test_ecs_ensure(world, e3, array_of_struct_with_opaques);
    arr3[0].a.value = 5;
    arr3[1].a.value = 10;
    arr3[2].a.value = 15;

    /* Test "less" */
    /* {{5}, {10}, {15}} < {{5}, {15}, {20}} */
    test_assert(cmp(world, array_of_struct_with_opaques, e1, e2) < 0);

    /* Test "greater" */
    /* {{5}, {15}, {20}} > {{5}, {10}, {15}} */
    test_assert(cmp(world, array_of_struct_with_opaques, e2, e1) > 0);

    /* Test "equal" */
    /* {{5}, {10}, {15}} == {{5}, {10}, {15}} */
    test_assert(cmp(world, array_of_struct_with_opaques, e1, e3) == 0);
    test_assert(equals(world, array_of_struct_with_opaques, e1, e3) == true);
    test_assert(cmp(world, array_of_struct_with_opaques, e1, e1) == 0);
    test_assert(equals(world, array_of_struct_with_opaques, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_array_of_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* ecs_string_t[3][3] */ array_of_array_of_strings =
        ecs_array(world, {.type = array_of_strings, .count = 3});

    /* Create multiple entities with array of arrays of strings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    ecs_string_t (*arr1)[3] = test_ecs_ensure(world, e1, array_of_array_of_strings);
    arr1[0][0] = ecs_os_strdup("AA");
    arr1[0][1] = ecs_os_strdup("BB");
    arr1[0][2] = ecs_os_strdup("CC");
    arr1[1][0] = ecs_os_strdup("AA");
    arr1[1][1] = ecs_os_strdup("BB");
    arr1[1][2] = ecs_os_strdup("CC");
    arr1[2][0] = ecs_os_strdup("AA");
    arr1[2][1] = ecs_os_strdup("BB");
    arr1[2][2] = ecs_os_strdup("CC");

    ecs_string_t (*arr2)[3] = test_ecs_ensure(world, e2, array_of_array_of_strings);
    arr2[0][0] = ecs_os_strdup("AA");
    arr2[0][1] = ecs_os_strdup("BB");
    arr2[0][2] = ecs_os_strdup("CC");
    arr2[1][0] = ecs_os_strdup("AA");
    arr2[1][1] = ecs_os_strdup("ZZ");
    arr2[1][2] = ecs_os_strdup("CC");
    arr2[2][0] = ecs_os_strdup("AA");
    arr2[2][1] = ecs_os_strdup("BB");
    arr2[2][2] = ecs_os_strdup("CC");

    ecs_string_t (*arr3)[3] = test_ecs_ensure(world, e3, array_of_array_of_strings);
    arr3[0][0] = ecs_os_strdup("AA");
    arr3[0][1] = ecs_os_strdup("BB");
    arr3[0][2] = ecs_os_strdup("CC");
    arr3[1][0] = ecs_os_strdup("AA");
    arr3[1][1] = ecs_os_strdup("BB");
    arr3[1][2] = ecs_os_strdup("CC");
    arr3[2][0] = ecs_os_strdup("AA");
    arr3[2][1] = ecs_os_strdup("BB");
    arr3[2][2] = ecs_os_strdup("CC");

    ecs_string_t (*arr4)[3] = test_ecs_ensure(world, e4, array_of_array_of_strings);
    arr4[0][0] = ecs_os_strdup("XX");
    arr4[0][1] = ecs_os_strdup("YY");
    arr4[0][2] = ecs_os_strdup("ZZ");
    arr4[1][0] = ecs_os_strdup("XX");
    arr4[1][1] = ecs_os_strdup("YY");
    arr4[1][2] = ecs_os_strdup("ZZ");
    arr4[2][0] = ecs_os_strdup("XX");
    arr4[2][1] = ecs_os_strdup("YY");
    arr4[2][2] = ecs_os_strdup("ZZ");

    /* Test "less" */
    /* {"AA", ...} < {"XX", ...} */
    test_assert(cmp(world, array_of_array_of_strings, e1, e4) < 0); 
    /* {"AA", ...} < {"AA", ..., "ZZ"} */
    test_assert(cmp(world, array_of_array_of_strings, e1, e2) < 0);

    /* Test "greater" */
    /* {"XX", ...} > {"AA", ...} */
    test_assert(cmp(world, array_of_array_of_strings, e4, e1) > 0); 
    /* {"AA", ..., "ZZ"} > {"AA", ...} */
    test_assert(cmp(world, array_of_array_of_strings, e2, e1) > 0);

    /* Test "equal" */
    test_assert(cmp(world, array_of_array_of_strings, e1, e3) == 0); 
    test_assert(equals(world, array_of_array_of_strings, e1, e3) == true); 
    test_assert(cmp(world, array_of_array_of_strings, e1, e1) == 0);
    test_assert(equals(world, array_of_array_of_strings, e1, e1) == true);


    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);

    ecs_fini(world);
}

void RttCompare_array_of_array_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* StructWithStrings[3] */ array_of_struct_with_strings =
        ecs_array(world, {.type = struct_with_strings, .count = 3});

    ecs_entity_t /* StructWithStrings[3][3] */
        array_of_array_of_struct_with_strings = ecs_array(
            world, {.type = array_of_struct_with_strings, .count = 3});

    /* Create five entities with the nested component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    StructWithStrings (*ptr1)[3] =
        test_ecs_ensure(world, e1, array_of_array_of_struct_with_strings);
    ptr1[0][0].a = ecs_os_strdup("AA");
    ptr1[0][0].b = 10;
    ptr1[0][0].c = ecs_os_strdup("CC");
    ptr1[1][1].a = ecs_os_strdup("BB");
    ptr1[1][1].b = 20;
    ptr1[1][1].c = ecs_os_strdup("DD");

    StructWithStrings (*ptr2)[3] =
        test_ecs_ensure(world, e2, array_of_array_of_struct_with_strings);
    ptr2[0][0].a = ecs_os_strdup("AA");
    ptr2[0][0].b = 10;
    ptr2[0][0].c = ecs_os_strdup("CC");
    ptr2[1][1].a = ecs_os_strdup("BB");
    ptr2[1][1].b = 25;
    ptr2[1][1].c = ecs_os_strdup("DD");

    StructWithStrings (*ptr3)[3] =
        test_ecs_ensure(world, e3, array_of_array_of_struct_with_strings);
    ptr3[0][0].a = ecs_os_strdup("AA");
    ptr3[0][0].b = 10;
    ptr3[0][0].c = ecs_os_strdup("CC");
    ptr3[1][1].a = ecs_os_strdup("BB");
    ptr3[1][1].b = 20;
    ptr3[1][1].c = ecs_os_strdup("DD");

    StructWithStrings (*ptr4)[3] =
        test_ecs_ensure(world, e4, array_of_array_of_struct_with_strings);
    ptr4[0][0].a = ecs_os_strdup("AA");
    ptr4[0][0].b = 5;
    ptr4[0][0].c = ecs_os_strdup("EE");
    ptr4[1][1].a = ecs_os_strdup("ZZ");
    ptr4[1][1].b = 20;
    ptr4[1][1].c = ecs_os_strdup("FF");

    StructWithStrings (*ptr5)[3] =
        test_ecs_ensure(world, e5, array_of_array_of_struct_with_strings);
    ptr5[0][0].a = ecs_os_strdup("AA");
    ptr5[0][0].b = 15;
    ptr5[0][0].c = ecs_os_strdup("GG");
    ptr5[1][1].a = ecs_os_strdup("BB");
    ptr5[1][1].b = 30;
    ptr5[1][1].c = ecs_os_strdup("HH");

    /* Test "less" */
    /* {{"AA", 10, "CC"}, {"BB", 20, "DD"}} < {{"AA", 10, "CC"}, {"BB", 25, "DD"}} */
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e1, e2) < 0);

    /* {{"AA", 5, "EE"}, {"ZZ", 20, "FF"}} < {{"AA", 15, "GG"}, {"BB", 30, "HH"}} */
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e4, e5) < 0);

    /* Test "greater" */
    /* {{"AA", 10, "CC"}, {"BB", 25, "DD"}} > {{"AA", 10, "CC"}, {"BB", 20, "DD"}} */
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e2, e1) > 0);

    /* {{"AA", 15, "GG"}, {"BB", 30, "HH"}} > {{"AA", 5, "EE"}, {"ZZ", 20, "FF"}} */
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e5, e4) > 0);

    /* Test "equal" */
    /* {{"AA", 10, "CC"}, {"BB", 20, "DD"}} == {{"AA", 10, "CC"}, {"BB", 20, "DD"}} */
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e1, e3) == 0);
    test_assert(equals(world, array_of_array_of_struct_with_strings, e1, e3) == true);
    test_assert(cmp(world, array_of_array_of_struct_with_strings, e1, e1) == 0);
    test_assert(equals(world, array_of_array_of_struct_with_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_vectors_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    ecs_entity_t /* ecs_vec_t[3] */ array_of_vectors_of_ints =
        ecs_array(world, {.type = vector_of_ints, .count = 3});

    /* Create four entities with array of vectors of integers component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    ecs_vec_t *arr1 = test_ecs_ensure(world, e1, array_of_vectors_of_ints);
    ecs_vec_set_count(NULL, &arr1[0], sizeof(ecs_i32_t), 3);
    ecs_i32_t *v1_0 = ecs_vec_first(&arr1[0]);
    v1_0[0] = 10;
    v1_0[1] = 20;
    v1_0[2] = 30;
    ecs_vec_set_count(NULL, &arr1[1], sizeof(ecs_i32_t), 2);
    ecs_i32_t *v1_1 = ecs_vec_first(&arr1[1]);
    v1_1[0] = 5;
    v1_1[1] = 15;
    ecs_vec_set_count(NULL, &arr1[2], sizeof(ecs_i32_t), 4);
    ecs_i32_t *v1_2 = ecs_vec_first(&arr1[2]);
    v1_2[0] = 1;
    v1_2[1] = 2;
    v1_2[2] = 3;
    v1_2[3] = 4;

    ecs_vec_t *arr2 = test_ecs_ensure(world, e2, array_of_vectors_of_ints);
    ecs_vec_set_count(NULL, &arr2[0], sizeof(ecs_i32_t), 3);
    ecs_i32_t *v2_0 = ecs_vec_first(&arr2[0]);
    v2_0[0] = 10;
    v2_0[1] = 20;
    v2_0[2] = 30;
    ecs_vec_set_count(NULL, &arr2[1], sizeof(ecs_i32_t), 2);
    ecs_i32_t *v2_1 = ecs_vec_first(&arr2[1]);
    v2_1[0] = 5;
    v2_1[1] = 15;
    ecs_vec_set_count(NULL, &arr2[2], sizeof(ecs_i32_t), 4);
    ecs_i32_t *v2_2 = ecs_vec_first(&arr2[2]);
    v2_2[0] = 1;
    v2_2[1] = 2;
    v2_2[2] = 3;
    v2_2[3] = 4;

    ecs_vec_t *arr3 = test_ecs_ensure(world, e3, array_of_vectors_of_ints);
    ecs_vec_set_count(NULL, &arr3[0], sizeof(ecs_i32_t), 3);
    ecs_i32_t *v3_0 = ecs_vec_first(&arr3[0]);
    v3_0[0] = 10;
    v3_0[1] = 20;
    v3_0[2] = 31; // Different from arr1
    ecs_vec_set_count(NULL, &arr3[1], sizeof(ecs_i32_t), 2);
    ecs_i32_t *v3_1 = ecs_vec_first(&arr3[1]);
    v3_1[0] = 5;
    v3_1[1] = 15;
    ecs_vec_set_count(NULL, &arr3[2], sizeof(ecs_i32_t), 4);
    ecs_i32_t *v3_2 = ecs_vec_first(&arr3[2]);
    v3_2[0] = 1;
    v3_2[1] = 2;
    v3_2[2] = 3;
    v3_2[3] = 4;

    ecs_vec_t *arr4 = test_ecs_ensure(world, e4, array_of_vectors_of_ints);
    ecs_vec_set_count(NULL, &arr4[0], sizeof(ecs_i32_t), 3);
    ecs_i32_t *v4_0 = ecs_vec_first(&arr4[0]);
    v4_0[0] = 12; // Different from arr1
    v4_0[1] = 22;
    v4_0[2] = 32;
    ecs_vec_set_count(NULL, &arr4[1], sizeof(ecs_i32_t), 2);
    ecs_i32_t *v4_1 = ecs_vec_first(&arr4[1]);
    v4_1[0] = 7; // Different from arr1
    v4_1[1] = 17;
    ecs_vec_set_count(NULL, &arr4[2], sizeof(ecs_i32_t), 4);
    ecs_i32_t *v4_2 = ecs_vec_first(&arr4[2]);
    v4_2[0] = 1;
    v4_2[1] = 2;
    v4_2[2] = 3;
    v4_2[3] = 4;

    /* Test "less" */
    test_assert(cmp(world, array_of_vectors_of_ints, e1, e3) < 0);

    /* Test "greater" */
    test_assert(cmp(world, array_of_vectors_of_ints, e3, e1) > 0);

    /* Test "equal" */
    test_assert(cmp(world, array_of_vectors_of_ints, e1, e2) == 0);
    test_assert(equals(world, array_of_vectors_of_ints, e1, e2) == true);
    test_assert(cmp(world, array_of_vectors_of_ints, e1, e1) == 0);
    test_assert(equals(world, array_of_vectors_of_ints, e1, e1) == true);

    /* Test when different in multiple fields */
    test_assert(cmp(world, array_of_vectors_of_ints, e1, e4) < 0);
    test_assert(cmp(world, array_of_vectors_of_ints, e4, e1) > 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);

    ecs_fini(world);
}

void RttCompare_array_of_vectors_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    ecs_entity_t /* ecs_vec_t[3] */ array_of_vectors_of_strings =
        ecs_array(world, {.type = vector_of_strings, .count = 3});

    /* Create five entities with array of vectors of strings */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    /* Initialize e1 */
    ecs_vec_t *arr1 = test_ecs_ensure(world, e1, array_of_vectors_of_strings);
    ecs_vec_set_count(NULL, &arr1[0], sizeof(ecs_string_t), 3);
    ecs_string_t *vec1_0 = ecs_vec_first(&arr1[0]);
    vec1_0[0] = ecs_os_strdup("AA");
    vec1_0[1] = ecs_os_strdup("BB");
    vec1_0[2] = ecs_os_strdup("CC");

    ecs_vec_set_count(NULL, &arr1[1], sizeof(ecs_string_t), 2);
    ecs_string_t *vec1_1 = ecs_vec_first(&arr1[1]);
    vec1_1[0] = ecs_os_strdup("XX");
    vec1_1[1] = ecs_os_strdup("YY");

    ecs_vec_set_count(NULL, &arr1[2], sizeof(ecs_string_t), 1);
    ecs_string_t *vec1_2 = ecs_vec_first(&arr1[2]);
    vec1_2[0] = ecs_os_strdup("ZZ");

    /* Initialize e2 */
    ecs_vec_t *arr2 = test_ecs_ensure(world, e2, array_of_vectors_of_strings);
    ecs_vec_set_count(NULL, &arr2[0], sizeof(ecs_string_t), 3);
    ecs_string_t *vec2_0 = ecs_vec_first(&arr2[0]);
    vec2_0[0] = ecs_os_strdup("AA");
    vec2_0[1] = ecs_os_strdup("BB");
    vec2_0[2] = ecs_os_strdup("CC");

    ecs_vec_set_count(NULL, &arr2[1], sizeof(ecs_string_t), 2);
    ecs_string_t *vec2_1 = ecs_vec_first(&arr2[1]);
    vec2_1[0] = ecs_os_strdup("XX");
    vec2_1[1] = ecs_os_strdup("YY");

    ecs_vec_set_count(NULL, &arr2[2], sizeof(ecs_string_t), 1);
    ecs_string_t *vec2_2 = ecs_vec_first(&arr2[2]);
    vec2_2[0] = ecs_os_strdup("AA");

    /* Initialize e3 */
    ecs_vec_t *arr3 = test_ecs_ensure(world, e3, array_of_vectors_of_strings);
    ecs_vec_set_count(NULL, &arr3[0], sizeof(ecs_string_t), 3);
    ecs_string_t *vec3_0 = ecs_vec_first(&arr3[0]);
    vec3_0[0] = ecs_os_strdup("AA");
    vec3_0[1] = ecs_os_strdup("BB");
    vec3_0[2] = ecs_os_strdup("CC");

    ecs_vec_set_count(NULL, &arr3[1], sizeof(ecs_string_t), 2);
    ecs_string_t *vec3_1 = ecs_vec_first(&arr3[1]);
    vec3_1[0] = ecs_os_strdup("XX");
    vec3_1[1] = ecs_os_strdup("YY");

    ecs_vec_set_count(NULL, &arr3[2], sizeof(ecs_string_t), 1);
    ecs_string_t *vec3_2 = ecs_vec_first(&arr3[2]);
    vec3_2[0] = ecs_os_strdup("ZZ");

    /* Initialize e4 with different strings */
    ecs_vec_t *arr4 = test_ecs_ensure(world, e4, array_of_vectors_of_strings);
    ecs_vec_set_count(NULL, &arr4[0], sizeof(ecs_string_t), 3);
    ecs_string_t *vec4_0 = ecs_vec_first(&arr4[0]);
    vec4_0[0] = ecs_os_strdup("AA");
    vec4_0[1] = ecs_os_strdup("BB");
    vec4_0[2] = ecs_os_strdup("CC");

    ecs_vec_set_count(NULL, &arr4[1], sizeof(ecs_string_t), 2);
    ecs_string_t *vec4_1 = ecs_vec_first(&arr4[1]);
    vec4_1[0] = ecs_os_strdup("XX");
    vec4_1[1] = ecs_os_strdup("ZZ");

    ecs_vec_set_count(NULL, &arr4[2], sizeof(ecs_string_t), 1);
    ecs_string_t *vec4_2 = ecs_vec_first(&arr4[2]);
    vec4_2[0] = ecs_os_strdup("AA");

    /* Initialize e5 with even more variation */
    ecs_vec_t *arr5 = test_ecs_ensure(world, e5, array_of_vectors_of_strings);
    ecs_vec_set_count(NULL, &arr5[0], sizeof(ecs_string_t), 3);
    ecs_string_t *vec5_0 = ecs_vec_first(&arr5[0]);
    vec5_0[0] = ecs_os_strdup("DD");
    vec5_0[1] = ecs_os_strdup("EE");
    vec5_0[2] = ecs_os_strdup("FF");

    ecs_vec_set_count(NULL, &arr5[1], sizeof(ecs_string_t), 2);
    ecs_string_t *vec5_1 = ecs_vec_first(&arr5[1]);
    vec5_1[0] = ecs_os_strdup("GG");
    vec5_1[1] = ecs_os_strdup("HH");

    ecs_vec_set_count(NULL, &arr5[2], sizeof(ecs_string_t), 1);
    ecs_string_t *vec5_2 = ecs_vec_first(&arr5[2]);
    vec5_2[0] = ecs_os_strdup("II");

    /* Test "less" */
    test_assert(cmp(world, array_of_vectors_of_strings, e2, e4) < 0);
    test_assert(cmp(world, array_of_vectors_of_strings, e2, e1) < 0);
    test_assert(cmp(world, array_of_vectors_of_strings, e1, e5) < 0);

    /* Test "greater" */
    test_assert(cmp(world, array_of_vectors_of_strings, e5, e1) > 0);
    test_assert(cmp(world, array_of_vectors_of_strings, e1, e2) > 0);
    test_assert(cmp(world, array_of_vectors_of_strings, e4, e2) > 0);

    /* Test "equal" */
    test_assert(cmp(world, array_of_vectors_of_strings, e1, e3) == 0);
    test_assert(equals(world, array_of_vectors_of_strings, e1, e3) == true);
    test_assert(cmp(world, array_of_vectors_of_strings, e3, e1) == 0);
    test_assert(equals(world, array_of_vectors_of_strings, e3, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_array_of_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t opaque_type = define_opaque_type(world);

    /* Define an array of OpaqueType with 3 elements */
    ecs_entity_t array_of_opaque = ecs_array(world, {
        .type = opaque_type, .count = 3
    });

    /* Create five entities with array of OpaqueType component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    OpaqueType *ptr1 = test_ecs_ensure(world, e1, array_of_opaque);
    ptr1[0].value = 5;
    ptr1[1].value = 10;
    ptr1[2].value = 15;

    OpaqueType *ptr2 = test_ecs_ensure(world, e2, array_of_opaque);
    ptr2[0].value = 5;
    ptr2[1].value = 10;
    ptr2[2].value = 20;

    OpaqueType *ptr3 = test_ecs_ensure(world, e3, array_of_opaque);
    ptr3[0].value = 5;
    ptr3[1].value = 10;
    ptr3[2].value = 15;

    OpaqueType *ptr4 = test_ecs_ensure(world, e4, array_of_opaque);
    ptr4[0].value = 7;
    ptr4[1].value = 8;
    ptr4[2].value = 9;

    OpaqueType *ptr5 = test_ecs_ensure(world, e5, array_of_opaque);
    ptr5[0].value = 3;
    ptr5[1].value = 10;
    ptr5[2].value = 15;

    /* Test "less" */
    /* {5, 10, 15} < {5, 10, 20} */
    test_assert(cmp(world, array_of_opaque, e1, e2) < 0);

    /* {3, 10, 15} < {7, 8, 9} */
    test_assert(cmp(world, array_of_opaque, e5, e4) < 0);

    /* Test "greater" */
    /* {5, 10, 20} > {5, 10, 15} */
    test_assert(cmp(world, array_of_opaque, e2, e1) > 0);

    /* {7, 8, 9} > {3, 10, 15} */
    test_assert(cmp(world, array_of_opaque, e4, e5) > 0);

    /* Test "equal" */
    /* {5, 10, 15} == {5, 10, 15} */
    test_assert(cmp(world, array_of_opaque, e1, e3) == 0);
    test_assert(equals(world, array_of_opaque, e1, e3) == true);
    test_assert(cmp(world, array_of_opaque, e1, e1) == 0);
    test_assert(equals(world, array_of_opaque, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    /* Create five entities with vector_of_ints component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_ints);
    ecs_vec_set_count(NULL, vec1, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v1_data = ecs_vec_first(vec1);
    v1_data[0] = 10;
    v1_data[1] = 20;
    v1_data[2] = 30;

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_ints);
    ecs_vec_set_count(NULL, vec2, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v2_data = ecs_vec_first(vec2);
    v2_data[0] = 10;
    v2_data[1] = 25;
    v2_data[2] = 30;

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_ints);
    ecs_vec_set_count(NULL, vec3, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v3_data = ecs_vec_first(vec3);
    v3_data[0] = 10;
    v3_data[1] = 20;
    v3_data[2] = 30;

    ecs_vec_t *vec4 = test_ecs_ensure(world, e4, vector_of_ints);
    ecs_vec_set_count(NULL, vec4, sizeof(ecs_i32_t), 2);
    ecs_i32_t *v4_data = ecs_vec_first(vec4);
    v4_data[0] = 10;
    v4_data[1] = 20;

    ecs_vec_t *vec5 = test_ecs_ensure(world, e5, vector_of_ints);
    ecs_vec_set_count(NULL, vec5, sizeof(ecs_i32_t), 3);
    ecs_i32_t *v5_data = ecs_vec_first(vec5);
    v5_data[0] = 40;
    v5_data[1] = 50;
    v5_data[2] = 60;

    /* Test "less" */
    /* {10, 20, 30} < {10, 25, 30} */
    test_assert(cmp(world, vector_of_ints, e1, e2) < 0);

    /* {10, 20, 30} < {40, 50, 60} */
    test_assert(cmp(world, vector_of_ints, e1, e5) < 0);

    /* {10, 20} < {10, 20, 30} */
    test_assert(cmp(world, vector_of_ints, e4, e1) < 0);

    /* Test "greater" */
    /* {10, 25, 30} > {10, 20, 30} */
    test_assert(cmp(world, vector_of_ints, e2, e1) > 0);

    /* {40, 50, 60} > {10, 20, 30} */
    test_assert(cmp(world, vector_of_ints, e5, e1) > 0);

    /* Test "equal" */
    /* {10, 20, 30} == {10, 20, 30} */
    test_assert(cmp(world, vector_of_ints, e1, e3) == 0);
    test_assert(equals(world, vector_of_ints, e1, e3) == true);
    test_assert(cmp(world, vector_of_ints, e1, e1) == 0);
    test_assert(equals(world, vector_of_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    /* Create five entities with a vector of strings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_strings);
    ecs_vec_set_count(NULL, vec1, sizeof(ecs_string_t), 3);
    ecs_string_t *v1_data = ecs_vec_first(vec1);
    v1_data[0] = ecs_os_strdup("AA");
    v1_data[1] = ecs_os_strdup("BB");
    v1_data[2] = ecs_os_strdup("CC");

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_strings);
    ecs_vec_set_count(NULL, vec2, sizeof(ecs_string_t), 3);
    ecs_string_t *v2_data = ecs_vec_first(vec2);
    v2_data[0] = ecs_os_strdup("AA");
    v2_data[1] = ecs_os_strdup("BB");
    v2_data[2] = ecs_os_strdup("DD");

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_strings);
    ecs_vec_set_count(NULL, vec3, sizeof(ecs_string_t), 3);
    ecs_string_t *v3_data = ecs_vec_first(vec3);
    v3_data[0] = ecs_os_strdup("AA");
    v3_data[1] = ecs_os_strdup("BB");
    v3_data[2] = ecs_os_strdup("CC");

    ecs_vec_t *vec4 = test_ecs_ensure(world, e4, vector_of_strings);
    ecs_vec_set_count(NULL, vec4, sizeof(ecs_string_t), 3);
    ecs_string_t *v4_data = ecs_vec_first(vec4);
    v4_data[0] = ecs_os_strdup("ZZ");
    v4_data[1] = ecs_os_strdup("AA");
    v4_data[2] = ecs_os_strdup("DD");

    ecs_vec_t *vec5 = test_ecs_ensure(world, e5, vector_of_strings);
    ecs_vec_set_count(NULL, vec5, sizeof(ecs_string_t), 3);
    ecs_string_t *v5_data = ecs_vec_first(vec5);
    v5_data[0] = ecs_os_strdup("AA");
    v5_data[1] = ecs_os_strdup("AA");
    v5_data[2] = ecs_os_strdup("BB");

    /* Test "less" */
    /* {"AA", "BB", "CC"} < {"AA", "BB", "DD"} */
    test_assert(cmp(world, vector_of_strings, e1, e2) < 0);

    /* {"AA", "AA", "BB"} < {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_strings, e5, e1) < 0);

    /* {"AA", "BB", "DD"} < {"ZZ", "AA", "DD"} */
    test_assert(cmp(world, vector_of_strings, e2, e4) < 0);

    /* Test "greater" */
    /* {"AA", "BB", "DD"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_strings, e2, e1) > 0);
    /* {"ZZ", "AA", "DD"} > {"AA", "BB", "DD"} */
    test_assert(cmp(world, vector_of_strings, e4, e2) > 0);

    /* {"AA", "BB", "CC"} > {"AA", "AA", "BB"} */
    test_assert(cmp(world, vector_of_strings, e1, e5) > 0);


    /* Test "equal" */
    /* {"AA", "BB", "CC"} == {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_strings, e1, e3) == 0);
    test_assert(equals(world, vector_of_strings, e1, e3) == true);
    test_assert(cmp(world, vector_of_strings, e1, e1) == 0);
    test_assert(equals(world, vector_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_vector_of_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t /* vector<StructWithInts> */ vector_of_struct_with_ints =
        ecs_vector(world, {.type = struct_with_ints});

    /* Create three entities with a vector of StructWithInts component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_struct_with_ints);
    ecs_vec_set_count(NULL, vec1, sizeof(StructWithInts), 3);
    StructWithInts *v1 = ecs_vec_first(vec1);
    v1[0] = (StructWithInts){.a = 10, .b = 20};
    v1[1] = (StructWithInts){.a = 15, .b = 25};
    v1[2] = (StructWithInts){.a = 20, .b = 30};

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_struct_with_ints);
    ecs_vec_set_count(NULL, vec2, sizeof(StructWithInts), 3);
    StructWithInts *v2 = ecs_vec_first(vec2);
    v2[0] = (StructWithInts){.a = 10, .b = 20};
    v2[1] = (StructWithInts){.a = 15, .b = 25};
    v2[2] = (StructWithInts){.a = 25, .b = 35};

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_struct_with_ints);
    ecs_vec_set_count(NULL, vec3, sizeof(StructWithInts), 3);
    StructWithInts *v3 = ecs_vec_first(vec3);
    v3[0] = (StructWithInts){.a = 10, .b = 20};
    v3[1] = (StructWithInts){.a = 15, .b = 25};
    v3[2] = (StructWithInts){.a = 20, .b = 30};

    /* Test "less" */
    /* vec1 < vec2 because v1[2].a < v2[2].a */
    test_assert(cmp(world, vector_of_struct_with_ints, e1, e2) < 0);

    /* Test "greater" */
    /* vec2 > vec1 because v2[2].a > v1[2].a */
    test_assert(cmp(world, vector_of_struct_with_ints, e2, e1) > 0);

    /* Test "equal" */
    /* vec1 == vec3 as they have identical values */
    test_assert(cmp(world, vector_of_struct_with_ints, e1, e3) == 0);
    test_assert(equals(world, vector_of_struct_with_ints, e1, e3) == true);
    test_assert(cmp(world, vector_of_struct_with_ints, e1, e1) == 0);
    test_assert(equals(world, vector_of_struct_with_ints, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ecs_fini(world);
}

void RttCompare_vector_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* vector<StructWithStrings> */ vector_of_struct_with_strings =
        ecs_vector(world, {.type = struct_with_strings});

    /* Create entities with vector<StructWithStrings> component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_struct_with_strings);
    ecs_vec_set_count(NULL, vec1, sizeof(StructWithStrings), 2);
    StructWithStrings *v1 = ecs_vec_first(vec1);
    v1[0].a = ecs_os_strdup("AA");
    v1[0].b = 10;
    v1[0].c = ecs_os_strdup("BB");
    v1[1].a = ecs_os_strdup("CC");
    v1[1].b = 15;
    v1[1].c = ecs_os_strdup("DD");

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_struct_with_strings);
    ecs_vec_set_count(NULL, vec2, sizeof(StructWithStrings), 2);
    StructWithStrings *v2 = ecs_vec_first(vec2);
    v2[0].a = ecs_os_strdup("AA");
    v2[0].b = 20;
    v2[0].c = ecs_os_strdup("BB");
    v2[1].a = ecs_os_strdup("CC");
    v2[1].b = 15;
    v2[1].c = ecs_os_strdup("DD");

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_struct_with_strings);
    ecs_vec_set_count(NULL, vec3, sizeof(StructWithStrings), 2);
    StructWithStrings *v3 = ecs_vec_first(vec3);
    v3[0].a = ecs_os_strdup("AA");
    v3[0].b = 10;
    v3[0].c = ecs_os_strdup("BB");
    v3[1].a = ecs_os_strdup("CC");
    v3[1].b = 15;
    v3[1].c = ecs_os_strdup("DD");

    ecs_vec_t *vec4 = test_ecs_ensure(world, e4, vector_of_struct_with_strings);
    ecs_vec_set_count(NULL, vec4, sizeof(StructWithStrings), 1);
    StructWithStrings *v4 = ecs_vec_first(vec4);
    v4[0].a = ecs_os_strdup("AA");
    v4[0].b = 5;
    v4[0].c = ecs_os_strdup("BB");

    /* Test "less" */
    /* vec1 < vec2, because {10} < {20} */
    test_assert(cmp(world, vector_of_struct_with_strings, e1, e2) < 0);

    /* vec4 < vec1, because vec4 has fewer elements */
    test_assert(cmp(world, vector_of_struct_with_strings, e4, e1) < 0);

    /* Test "greater" */
    /* vec2 > vec1, because {20} > {10} */
    test_assert(cmp(world, vector_of_struct_with_strings, e2, e1) > 0);

    /* vec1 > vec4, because vec1 has more elements */
    test_assert(cmp(world, vector_of_struct_with_strings, e1, e4) > 0);

    /* Test "equal" */
    /* vec1 == vec3, all elements are identical */
    test_assert(cmp(world, vector_of_struct_with_strings, e1, e3) == 0);
    test_assert(equals(world, vector_of_struct_with_strings, e1, e3) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);

    ecs_fini(world);
}

void RttCompare_vector_of_arrays_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* vector<ecs_string_t[3]> */ vector_of_arrays_of_strings =
        ecs_vector(world, {.type = array_of_strings});

    /* Create five entities with vector of arrays of strings component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_arrays_of_strings);
    ecs_vec_set_count(NULL, vec1, sizeof(ecs_string_t[3]), 1);
    ecs_string_t (*v1)[3] = ecs_vec_first(vec1);
    v1[0][0] = ecs_os_strdup("AA");
    v1[0][1] = ecs_os_strdup("BB");
    v1[0][2] = ecs_os_strdup("CC");

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_arrays_of_strings);
    ecs_vec_set_count(NULL, vec2, sizeof(ecs_string_t[3]), 1);
    ecs_string_t (*v2)[3] = ecs_vec_first(vec2);
    v2[0][0] = ecs_os_strdup("AA");
    v2[0][1] = ecs_os_strdup("BB");
    v2[0][2] = ecs_os_strdup("CC");

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_arrays_of_strings);
    ecs_vec_set_count(NULL, vec3, sizeof(ecs_string_t[3]), 1);
    ecs_string_t (*v3)[3] = ecs_vec_first(vec3);
    v3[0][0] = ecs_os_strdup("AA");
    v3[0][1] = ecs_os_strdup("ZZ");
    v3[0][2] = ecs_os_strdup("CC");

    ecs_vec_t *vec4 = test_ecs_ensure(world, e4, vector_of_arrays_of_strings);
    ecs_vec_set_count(NULL, vec4, sizeof(ecs_string_t[3]), 1);
    ecs_string_t (*v4)[3] = ecs_vec_first(vec4);
    v4[0][0] = ecs_os_strdup("ZZ");
    v4[0][1] = ecs_os_strdup("AA");
    v4[0][2] = ecs_os_strdup("DD");

    ecs_vec_t *vec5 = test_ecs_ensure(world, e5, vector_of_arrays_of_strings);
    ecs_vec_set_count(NULL, vec5, sizeof(ecs_string_t[3]), 1);
    ecs_string_t (*v5)[3] = ecs_vec_first(vec5);
    v5[0][0] = ecs_os_strdup("AA");
    v5[0][1] = ecs_os_strdup("BB");
    v5[0][2] = ecs_os_strdup("DD");

    /* Test "less" */
    /* {"AA", "BB", "CC"} < {"AA", "ZZ", "CC"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e1, e3) < 0);

    /* {"AA", "BB", "CC"} < {"ZZ", "AA", "DD"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e1, e4) < 0);

    /* {"AA", "BB", "DD"} < {"ZZ", "AA", "DD"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e5, e4) < 0);

    /* Test "greater" */
    /* {"ZZ", "AA", "DD"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e4, e1) > 0);

    /* {"AA", "ZZ", "CC"} > {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e3, e1) > 0);

    /* Test "equal" */
    /* {"AA", "BB", "CC"} == {"AA", "BB", "CC"} */
    test_assert(cmp(world, vector_of_arrays_of_strings, e1, e2) == 0);
    test_assert(equals(world, vector_of_arrays_of_strings, e1, e2) == true);
    test_assert(cmp(world, vector_of_arrays_of_strings, e1, e1) == 0);
    test_assert(equals(world, vector_of_arrays_of_strings, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}

void RttCompare_vector_of_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t opaque = define_opaque_type(world);

    ecs_entity_t /* vector<OpaqueType> */ vector_of_opaque =
        ecs_vector(world, {.type = opaque });

    /* Create five entities with vector_of_opaque component */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);

    ecs_vec_t *vec1 = test_ecs_ensure(world, e1, vector_of_opaque);
    ecs_vec_set_count(NULL, vec1, sizeof(OpaqueType), 3);
    OpaqueType *v1_data = ecs_vec_first(vec1);
    v1_data[0].value = 10;
    v1_data[1].value = 20;
    v1_data[2].value = 30;

    ecs_vec_t *vec2 = test_ecs_ensure(world, e2, vector_of_opaque);
    ecs_vec_set_count(NULL, vec2, sizeof(OpaqueType), 3);
    OpaqueType *v2_data = ecs_vec_first(vec2);
    v2_data[0].value = 10;
    v2_data[1].value = 25;
    v2_data[2].value = 30;

    ecs_vec_t *vec3 = test_ecs_ensure(world, e3, vector_of_opaque);
    ecs_vec_set_count(NULL, vec3, sizeof(OpaqueType), 3);
    OpaqueType *v3_data = ecs_vec_first(vec3);
    v3_data[0].value = 10;
    v3_data[1].value = 20;
    v3_data[2].value = 30;

    ecs_vec_t *vec4 = test_ecs_ensure(world, e4, vector_of_opaque);
    ecs_vec_set_count(NULL, vec4, sizeof(OpaqueType), 2);
    OpaqueType *v4_data = ecs_vec_first(vec4);
    v4_data[0].value = 5;
    v4_data[1].value = 15;

    ecs_vec_t *vec5 = test_ecs_ensure(world, e5, vector_of_opaque);
    ecs_vec_set_count(NULL, vec5, sizeof(OpaqueType), 4);
    OpaqueType *v5_data = ecs_vec_first(vec5);
    v5_data[0].value = 10;
    v5_data[1].value = 20;
    v5_data[2].value = 25;
    v5_data[3].value = 35;

    /* Test "less" */
    /* {10, 20, 30} < {10, 25, 30} */
    test_assert(cmp(world, vector_of_opaque, e1, e2) < 0);

    /* {5, 15} < {10, 20, 30} */
    test_assert(cmp(world, vector_of_opaque, e4, e1) < 0);

    /* Test "greater" */
    /* {10, 25, 30} > {10, 20, 30} */
    test_assert(cmp(world, vector_of_opaque, e2, e1) > 0);

    /* {10, 20, 30, 35} > {10, 20, 30} */
    test_assert(cmp(world, vector_of_opaque, e5, e1) > 0);

    /* Test "equal" */
    /* {10, 20, 30} == {10, 20, 30} */
    test_assert(cmp(world, vector_of_opaque, e1, e3) == 0);
    test_assert(equals(world, vector_of_opaque, e1, e3) == true);
    test_assert(cmp(world, vector_of_opaque, e1, e1) == 0);
    test_assert(equals(world, vector_of_opaque, e1, e1) == true);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    ecs_delete(world, e4);
    ecs_delete(world, e5);

    ecs_fini(world);
}
