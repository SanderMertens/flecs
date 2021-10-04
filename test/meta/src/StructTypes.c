#include <meta.h>

void _meta_test_struct(
    ecs_world_t *world, 
    ecs_entity_t t, 
    ecs_size_t size, 
    ecs_size_t alignment) 
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_int(ct->size, size);
    test_int(ct->alignment, alignment);

    const EcsMetaType *mt = ecs_get(world, t, EcsMetaType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsStructType);
}

void _meta_test_member(
    ecs_world_t *world, 
    ecs_entity_t t, 
    const char *name,
    ecs_entity_t type,
    ecs_size_t offset) 
{
    ecs_entity_t m = ecs_lookup_child(world, t, name);
    test_assert(m != 0);
    test_assert(ecs_has(world, m, EcsMember));

    const EcsMember *mptr = ecs_get(world, m, EcsMember);
    test_assert(mptr != NULL);
    test_assert(mptr->type == type);

    const EcsStruct *sptr = ecs_get(world, t, EcsStruct);
    test_assert(sptr != NULL);

    ecs_member_t *members = ecs_vector_first(sptr->members, ecs_member_t);
    int i, count = ecs_vector_count(sptr->members);

    for (i = 0; i < count; i ++) {
        if (members[i].member == m) {
            break;
        }
    }

    /* Make sure member was found */
    test_assert(i != count);

    test_str(members[i].name, name);
    test_assert(members[i].type == type);
    test_int(members[i].offset, offset);
}

#define meta_test_struct(world, t, T)\
    _meta_test_struct(world, t, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define meta_test_member(world, t, T, name, type)\
    _meta_test_member(world, t, #name, type, offsetof(T, name))

void StructTypes_i32() {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_i32_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_bool_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_bool_i32_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
        ecs_bool_t z;
        ecs_i32_t w;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_bool_t)},
            {"w", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, w, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void StructTypes_i32_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void StructTypes_i32_bool_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
        ecs_bool_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void StructTypes_i32_bool_bool_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
        ecs_bool_t z;
        ecs_bool_t w;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_bool_t)},
            {"w", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t));
    meta_test_member(world, t, T, w, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void StructTypes_i32_i64() {
    typedef struct {
        ecs_i32_t x;
        ecs_i64_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i64_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t));

    ecs_fini(world);
}

void StructTypes_i32_i64_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i64_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i64_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t));
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t));
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

