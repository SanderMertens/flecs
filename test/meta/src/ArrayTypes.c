#include <meta.h>

void ArrayTypes_array_bool_1() {
    typedef struct {
        ecs_bool_t x[1];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 1}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_bool_2() {
    typedef struct {
        ecs_bool_t x[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 2);

    ecs_fini(world);
}

void ArrayTypes_array_bool_3() {
    typedef struct {
        ecs_bool_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 3}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 3);

    ecs_fini(world);
}

void ArrayTypes_array_bool_1_before_i32_member() {
    typedef struct {
        ecs_bool_t x[1];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 1},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_bool_2_before_i32_member() {
    typedef struct {
        ecs_bool_t x[2];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 2);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_bool_3_before_i32_member() {
    typedef struct {
        ecs_bool_t x[3];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 3},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 3);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_i32_3() {
    typedef struct {
        ecs_i32_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t), 3},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 3);

    ecs_fini(world);
}

void ArrayTypes_array_i32_3_before_i32_member() {
    typedef struct {
        ecs_i32_t x[3];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t), 3},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 3);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_struct_bool_3() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();
    
    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n, 3},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n, 3);

    ecs_fini(world);
}

void ArrayTypes_array_struct_bool_3_before_i32_member() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1[3];
        int32_t x;
    } T;

    ecs_world_t *world = ecs_init();
    
    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n, 3},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n, 3);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_array_struct_array_bool_3() {
    typedef struct {
        ecs_bool_t x[2];
    } N1;

    typedef struct {
        N1 n_1[3];
        int32_t x;
    } T;

    ecs_world_t *world = ecs_init();
    
    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n, 3},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n, 3);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void ArrayTypes_2_arrays_1_bool_1_i32() {
    typedef struct {
        ecs_bool_t x[3];
        ecs_i32_t y[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 3},
            {"y", ecs_id(ecs_i32_t), 2}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 3);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 2);

    ecs_fini(world);
}

void ArrayTypes_2_arrays_1_i32_1_bool() {
    typedef struct {
        ecs_i32_t x[3];
        ecs_bool_t y[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t), 3},
            {"y", ecs_id(ecs_bool_t), 2}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 3);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 2);

    ecs_fini(world);
}

void ArrayTypes_8_arrays_bool_w_padded_member() {
    typedef struct {
        ecs_bool_t a[1];
        ecs_i16_t p1;
        ecs_bool_t b[2];
        ecs_bool_t c[3];
        ecs_i32_t p2;
        ecs_bool_t d[4];
        ecs_bool_t e[5];
        ecs_bool_t f[6];
        ecs_bool_t g[7];
        ecs_i64_t p3;
        ecs_bool_t h[8];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"a", ecs_id(ecs_bool_t), 1},
            {"p1", ecs_id(ecs_i16_t), 1},
            {"b", ecs_id(ecs_bool_t), 2},
            {"c", ecs_id(ecs_bool_t), 3},
            {"p2", ecs_id(ecs_i32_t), 1},
            {"d", ecs_id(ecs_bool_t), 4},
            {"e", ecs_id(ecs_bool_t), 5},
            {"f", ecs_id(ecs_bool_t), 6},
            {"g", ecs_id(ecs_bool_t), 7},
            {"p3", ecs_id(ecs_i64_t), 1},
            {"h", ecs_id(ecs_bool_t), 8},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, a, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, p1, ecs_id(ecs_i16_t), 1);
    meta_test_member(world, t, T, b, ecs_id(ecs_bool_t), 2);
    meta_test_member(world, t, T, c, ecs_id(ecs_bool_t), 3);
    meta_test_member(world, t, T, p2, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, d, ecs_id(ecs_bool_t), 4);
    meta_test_member(world, t, T, e, ecs_id(ecs_bool_t), 5);
    meta_test_member(world, t, T, f, ecs_id(ecs_bool_t), 6);
    meta_test_member(world, t, T, g, ecs_id(ecs_bool_t), 7);
    meta_test_member(world, t, T, p3, ecs_id(ecs_i64_t), 1);
    meta_test_member(world, t, T, h, ecs_id(ecs_bool_t), 8);

    ecs_fini(world);
}

void ArrayTypes_standaline_array_bool_1() {
    typedef struct {
        ecs_bool_t x[1];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 1
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 1);

    ecs_fini(world);
}

void ArrayTypes_standaline_array_bool_2() {
    typedef struct {
        ecs_bool_t x[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 2
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 1);

    ecs_fini(world);
}

void ArrayTypes_standaline_array_bool_3() {
    typedef struct {
        ecs_bool_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 3
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 1);

    ecs_fini(world);
}

void ArrayTypes_array_of_standaline_array_bool_1() {
    typedef ecs_bool_t bool_1[1];
    typedef struct {
        bool_1 x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 1
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a, 3}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 3);

    ecs_fini(world);
}

void ArrayTypes_array_of_standaline_array_bool_2() {
    typedef ecs_bool_t bool_2[2];
    typedef struct {
        bool_2 x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 2
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a, 3}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 3);

    ecs_fini(world);
}

void ArrayTypes_array_of_standaline_array_bool_3() {
    typedef ecs_bool_t bool_2[3];
    typedef struct {
        bool_2 x[4];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 3
    });

    test_assert(a != 0);
    test_str(ecs_get_name(world, a), "A");

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a, 4}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, a, 4);

    ecs_fini(world);
}
