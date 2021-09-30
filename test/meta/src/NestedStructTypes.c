#include <meta.h>

void NestedStructTypes_1_bool() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_int32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_w_bool_member() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        bool x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void NestedStructTypes_1_bool_w_int32_member() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void NestedStructTypes_1_int32_w_bool_member() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
        ecs_bool_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void NestedStructTypes_1_int32_w_int32_member() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void NestedStructTypes_1_bool_2_same() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_2_bool() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_2_i32() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        ecs_i32_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_2_bool() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        ecs_bool_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_2_bool_bool() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_bool_2_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N1;

    typedef struct {
        ecs_bool_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_bool_2_same() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_bool_bool_2_bool_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N1;

    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_2_same() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_2_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        ecs_i32_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_2_i32_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_i32_2_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        ecs_i32_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_i32_2_same() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n1);

    ecs_fini(world);
}

void NestedStructTypes_1_i32_i32_2_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, n_1, n1);
    meta_test_member(world, t, T, n_2, n2);

    ecs_fini(world);
}
