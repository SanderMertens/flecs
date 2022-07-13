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
    int32_t elem_count,
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
    test_int(members[i].count, elem_count);
}

void StructTypes_i32() {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
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

void StructTypes_bool_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 1);

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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, w, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_bool_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
        ecs_bool_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 1);

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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, w, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_i64() {
    typedef struct {
        ecs_i32_t x;
        ecs_i64_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i64_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t), 1);

    ecs_fini(world);
}

void StructTypes_i32_i64_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i64_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i64_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_incomplete_member() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_new_id(world);

    ecs_entity_t m_x = ecs_new_w_pair(world, EcsChildOf, t);
    ecs_set_name(world, m_x, "x");
    ecs_set(world, m_x, EcsMember, {.type = ecs_id(ecs_i32_t)});

    ecs_entity_t m_y = ecs_new_w_pair(world, EcsChildOf, t);
    ecs_set_name(world, m_y, "y");
    ecs_set(world, m_y, EcsMember, {.type = ecs_id(ecs_i32_t)});

    ecs_entity_t m_z = ecs_new_w_pair(world, EcsChildOf, t);
    ecs_set_name(world, m_z, "z");

    ecs_log_set_level(-4);
    ecs_set(world, m_z, EcsMember, {.type = 0});

    const EcsStruct *ptr = ecs_get(world, t, EcsStruct);
    test_assert(ptr != NULL);
    test_assert(ptr->members != NULL);
    test_int(ecs_vector_count(ptr->members), 2);

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}

void StructTypes_partial_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {{ .name = "x", .type = ecs_id(ecs_f32_t) }}
    });

    test_assert(s == ecs_id(Position));

    const EcsComponent *cptr = ecs_get(world, s, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(Position));
    test_int(cptr->alignment, ECS_ALIGNOF(Position));

    const EcsMetaType *mptr = ecs_get(world, s, EcsMetaType);
    test_assert(mptr != NULL);
    test_bool(mptr->partial, true);
    test_bool(mptr->existing, true);

    meta_test_struct(world, s, Position);
    meta_test_member(world, s, Position, x, ecs_id(ecs_f32_t), 1);

    ecs_fini(world);
}

void StructTypes_partial_type_custom_offset() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {{ 
            .name = "y", 
            .type = ecs_id(ecs_f32_t), 
            .offset = offsetof(Position, y) 
        }}
    });

    test_assert(s == ecs_id(Position));

    const EcsComponent *cptr = ecs_get(world, s, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(Position));
    test_int(cptr->alignment, ECS_ALIGNOF(Position));

    const EcsMetaType *mptr = ecs_get(world, s, EcsMetaType);
    test_assert(mptr != NULL);
    test_bool(mptr->partial, true);
    test_bool(mptr->existing, true);

    meta_test_struct(world, s, Position);
    meta_test_member(world, s, Position, y, ecs_id(ecs_f32_t), 1);

    ecs_fini(world);
}

void StructTypes_struct_w_short_notation() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 1);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 1);

    ecs_fini(world);
}
