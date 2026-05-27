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

    const EcsType *mt = ecs_get(world, t, EcsType);
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
    if (m) {
        test_assert(ecs_has(world, m, EcsMember));

        const EcsMember *mptr = ecs_get(world, m, EcsMember);
        test_assert(mptr != NULL);
        test_assert(mptr->type == type);
    }

    ecs_member_t *member = ecs_struct_get_member(world, t, name);
    test_assert(member != NULL);

    test_str(member->name, name);
    test_assert(member->type == type);
    test_int(member->offset, offset);
    test_int(member->count, elem_count);
}

static
int32_t meta_member_count(ecs_world_t *world, ecs_entity_t t) {
    int32_t n = 0;
    while (ecs_struct_get_nth_member(world, t, n)) {
        n ++;
    }
    return n;
}

void StructTypes_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_i32_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_bool_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_bool_bool_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_bool_i32_bool_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, w, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_bool(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_bool_bool(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_bool_bool_bool(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_bool_t), 0);
    meta_test_member(world, t, T, w, ecs_id(ecs_bool_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_i64(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t), 0);

    ecs_fini(world);
}

void StructTypes_i32_i64_i32(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i64_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_incomplete_member(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_new(world);

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
    test_int(ecs_vec_count(&ptr->members), 2);

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_partial_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {{ .name = "x", .type = ecs_id(ecs_i32_t) }}
    });

    test_assert(s == ecs_id(Position));

    const EcsComponent *cptr = ecs_get(world, s, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(Position));
    test_int(cptr->alignment, ECS_ALIGNOF(Position));

    const EcsType *mptr = ecs_get(world, s, EcsType);
    test_assert(mptr != NULL);
    test_bool(mptr->partial, true);
    test_bool(mptr->existing, true);

    meta_test_struct(world, s, Position);
    meta_test_member(world, s, Position, x, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_partial_type_custom_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Vec3);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Vec3),
        .members = {{ 
            .name = "y",
            .type = ecs_id(ecs_i32_t),
            .offset = offsetof(Vec3, y)
        }}
    });

    test_assert(s == ecs_id(Vec3));

    const EcsComponent *cptr = ecs_get(world, s, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(Vec3));
    test_int(cptr->alignment, ECS_ALIGNOF(Vec3));

    const EcsType *mptr = ecs_get(world, s, EcsType);
    test_assert(mptr != NULL);
    test_bool(mptr->partial, true);
    test_bool(mptr->existing, true);

    meta_test_struct(world, s, Vec3);
    meta_test_member(world, s, Vec3, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_struct_w_short_notation(void) {
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
    meta_test_member(world, t, T, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_value_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .range = {-2, 2}}
        },
        .create_member_entities = true
    });

    test_assert(s == ecs_id(Position));

    ecs_entity_t x = ecs_lookup(world, "Position.x");
    test_assert(x != 0);
    ecs_entity_t y = ecs_lookup(world, "Position.y");
    test_assert(y != 0);

    const EcsMemberRanges *xr = ecs_get(world, x, EcsMemberRanges);
    test_assert(xr != NULL);
    test_int(xr->value.min, -1);
    test_int(xr->value.max, 1);
    test_int(xr->warning.min, 0);
    test_int(xr->warning.max, 0);
    test_int(xr->error.min, 0);
    test_int(xr->error.max, 0);

    const EcsMemberRanges *yr = ecs_get(world, y, EcsMemberRanges);
    test_assert(yr != NULL);
    test_int(yr->value.min, -2);
    test_int(yr->value.max, 2);
    test_int(xr->warning.min, 0);
    test_int(xr->warning.max, 0);
    test_int(xr->error.min, 0);
    test_int(xr->error.max, 0);

    ecs_fini(world);
}

void StructTypes_error_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .error_range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .error_range = {-2, 2}}
        },
        .create_member_entities = true
    });

    test_assert(s == ecs_id(Position));

    ecs_entity_t x = ecs_lookup(world, "Position.x");
    test_assert(x != 0);
    ecs_entity_t y = ecs_lookup(world, "Position.y");
    test_assert(y != 0);

    const EcsMemberRanges *xr = ecs_get(world, x, EcsMemberRanges);
    test_assert(xr != NULL);
    test_int(xr->error.min, -1);
    test_int(xr->error.max, 1);
    test_int(xr->warning.min, 0);
    test_int(xr->warning.max, 0);

    const EcsMemberRanges *yr = ecs_get(world, y, EcsMemberRanges);
    test_assert(yr != NULL);
    test_int(yr->error.min, -2);
    test_int(yr->error.max, 2);
    test_int(yr->warning.min, 0);
    test_int(yr->warning.max, 0);

    ecs_fini(world);
}

void StructTypes_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .warning_range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .warning_range = {-2, 2}}
        },
        .create_member_entities = true
    });

    test_assert(s == ecs_id(Position));

    ecs_entity_t x = ecs_lookup(world, "Position.x");
    test_assert(x != 0);
    ecs_entity_t y = ecs_lookup(world, "Position.y");
    test_assert(y != 0);

    const EcsMemberRanges *xr = ecs_get(world, x, EcsMemberRanges);
    test_assert(xr != NULL);
    test_int(xr->warning.min, -1);
    test_int(xr->warning.max, 1);
    test_int(xr->error.min, 0);
    test_int(xr->error.max, 0);

    const EcsMemberRanges *yr = ecs_get(world, y, EcsMemberRanges);
    test_assert(yr != NULL);
    test_int(yr->warning.min, -2);
    test_int(yr->warning.max, 2);
    test_int(yr->error.min, 0);
    test_int(yr->error.max, 0);

    ecs_fini(world);
}

void StructTypes_value_range_no_member_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .range = {-2, 2}}
        }
    });

    test_assert(s == ecs_id(Position));
    test_assert(ecs_lookup(world, "Position.x") == 0);
    test_assert(ecs_lookup(world, "Position.y") == 0);

    const EcsStruct *sptr = ecs_get(world, s, EcsStruct);
    test_assert(sptr != NULL);

    ecs_member_t *members = ecs_vec_first(&sptr->members);

    test_int(members[0].range.min, -1);
    test_int(members[0].range.max, 1);
    test_int(members[0].warning_range.min, 0);
    test_int(members[0].warning_range.max, 0);
    test_int(members[0].error_range.min, 0);
    test_int(members[0].error_range.max, 0);

    test_int(members[1].range.min, -2);
    test_int(members[1].range.max, 2);
    test_int(members[1].warning_range.min, 0);
    test_int(members[1].warning_range.max, 0);
    test_int(members[1].error_range.min, 0);
    test_int(members[1].error_range.max, 0);

    ecs_fini(world);
}

void StructTypes_error_range_no_member_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .error_range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .error_range = {-2, 2}}
        }
    });

    test_assert(s == ecs_id(Position));
    test_assert(ecs_lookup(world, "Position.x") == 0);
    test_assert(ecs_lookup(world, "Position.y") == 0);

    const EcsStruct *sptr = ecs_get(world, s, EcsStruct);
    test_assert(sptr != NULL);

    ecs_member_t *members = ecs_vec_first(&sptr->members);

    test_int(members[0].range.min, 0);
    test_int(members[0].range.max, 0);
    test_int(members[0].warning_range.min, 0);
    test_int(members[0].warning_range.max, 0);
    test_int(members[0].error_range.min, -1);
    test_int(members[0].error_range.max, 1);

    test_int(members[1].range.min, 0);
    test_int(members[1].range.max, 0);
    test_int(members[1].warning_range.min, 0);
    test_int(members[1].warning_range.max, 0);
    test_int(members[1].error_range.min, -2);
    test_int(members[1].error_range.max, 2);

    ecs_fini(world);
}

void StructTypes_warning_range_no_member_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .warning_range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .warning_range = {-2, 2}}
        }
    });

    test_assert(s == ecs_id(Position));
    test_assert(ecs_lookup(world, "Position.x") == 0);
    test_assert(ecs_lookup(world, "Position.y") == 0);

    const EcsStruct *sptr = ecs_get(world, s, EcsStruct);
    test_assert(sptr != NULL);

    ecs_member_t *members = ecs_vec_first(&sptr->members);

    test_int(members[0].range.min, 0);
    test_int(members[0].range.max, 0);
    test_int(members[0].warning_range.min, -1);
    test_int(members[0].warning_range.max, 1);
    test_int(members[0].error_range.min, 0);
    test_int(members[0].error_range.max, 0);

    test_int(members[1].range.min, 0);
    test_int(members[1].range.max, 0);
    test_int(members[1].warning_range.min, -2);
    test_int(members[1].warning_range.max, 2);
    test_int(members[1].error_range.min, 0);
    test_int(members[1].error_range.max, 0);

    ecs_fini(world);
}

void StructTypes_error_and_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t), .error_range = {-2, 2}, .warning_range = {-1, 1}},
            {"y", ecs_id(ecs_f32_t), .error_range = {-4, 4}, .warning_range = {-2, 2}}
        },
        .create_member_entities = true
    });

    test_assert(s == ecs_id(Position));

    ecs_entity_t x = ecs_lookup(world, "Position.x");
    test_assert(x != 0);
    ecs_entity_t y = ecs_lookup(world, "Position.y");
    test_assert(y != 0);

    const EcsMemberRanges *xr = ecs_get(world, x, EcsMemberRanges);
    test_assert(xr != NULL);
    test_int(xr->error.min, -2);
    test_int(xr->error.max, 2);
    test_int(xr->warning.min, -1);
    test_int(xr->warning.max, 1);

    const EcsMemberRanges *yr = ecs_get(world, y, EcsMemberRanges);
    test_assert(yr != NULL);
    test_int(yr->error.min, -4);
    test_int(yr->error.max, 4);
    test_int(yr->warning.min, -2);
    test_int(yr->warning.max, 2);

    ecs_fini(world);
}

void StructTypes_error_range_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_string_t), .error_range = {-1, 1}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_warning_range_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_string_t), .warning_range = {-1, 1}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_invalid_value_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .range = {1, -1}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_invalid_error_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .error_range = {1, -1}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_invalid_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .warning_range = {1, -1}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_overlapping_error_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .error_range = {-1, 1}, .warning_range = {-2, 2}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_overlapping_value_error_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .range = {-1, 1}, .error_range = {-2, 2}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_overlapping_value_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .range = {-1, 1}, .warning_range = {-2, 2}},
        }
    });

    test_assert(s == 0);

    ecs_fini(world);
}

void StructTypes_struct_w_16_alignment(void) {
#ifndef _MSC_VER
    typedef __attribute((aligned(16)))
#else
    typedef __declspec(align(16))
#endif
    struct T {
        float x;
        float y;
        float z;
        float w;
    } T;

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, T);

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_id(T),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)},
            {"z", ecs_id(ecs_f32_t)},
            {"w", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");

    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, x, ecs_id(ecs_f32_t), 0);
    meta_test_member(world, t, T, y, ecs_id(ecs_f32_t), 0);
    meta_test_member(world, t, T, z, ecs_id(ecs_f32_t), 0);
    meta_test_member(world, t, T, w, ecs_id(ecs_f32_t), 0);

    const EcsComponent *cptr = ecs_get(world, t, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(T));
    test_int(cptr->alignment, 16);

    const EcsType *mptr = ecs_get(world, t, EcsType);
    test_assert(mptr != NULL);
    test_bool(mptr->partial, false);
    test_bool(mptr->existing, true);

    ecs_fini(world);
}

void StructTypes_struct_w_use_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {{ 
            .name = "y",
            .type = ecs_id(ecs_i32_t),
            .offset = offsetof(Position, y),
            .use_offset = true
        }, { 
            .name = "x",
            .type = ecs_id(ecs_i32_t),
            .offset = offsetof(Position, x),
            .use_offset = true
        }}
    });

    test_assert(s == ecs_id(Position));

    const EcsComponent *cptr = ecs_get(world, s, EcsComponent);
    test_assert(cptr != NULL);
    test_int(cptr->size, sizeof(Position));
    test_int(cptr->alignment, ECS_ALIGNOF(Position));

    const EcsType *mptr = ecs_get(world, s, EcsType);
    test_assert(mptr != NULL);
    // test_bool(mptr->partial, false); TODO
    test_bool(mptr->existing, true);

    meta_test_struct(world, s, Position);
    meta_test_member(world, s, Position, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, s, Position, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructTypes_direct_cycle(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_component(world, {
        .entity = ecs_entity(world, {.name = "A"}),
        .type.size = ECS_SIZEOF(ecs_i32_t),
        .type.alignment = ECS_ALIGNOF(ecs_i32_t)
    });

    test_expect_abort();

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = a,
        .members = {
            {"value", a}
        }
    });
}

void StructTypes_indirect_cycle(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_component(world, {
        .entity = ecs_entity(world, {.name = "A"}),
        .type.size = ECS_SIZEOF(ecs_i32_t),
        .type.alignment = ECS_ALIGNOF(ecs_i32_t)
    });

    ecs_entity_t b = ecs_component(world, {
        .entity = ecs_entity(world, {.name = "B"}),
        .type.size = ECS_SIZEOF(ecs_i32_t),
        .type.alignment = ECS_ALIGNOF(ecs_i32_t)
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = a,
        .members = {
            {"value", b}
        }
    });

    test_expect_abort();

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = b,
        .members = {
            {"value", a}
        }
    });
}

void StructTypes_use_before_registering_reflection(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", .type = ecs_id(ecs_i32_t) },
            { "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    test_assert(s != 0);

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}


void StructTypes_use_before_registering_reflection_w_hooks(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", .type = ecs_id(ecs_i32_t) },
            { "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    test_assert(s != 0);

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}


void StructTypes_inherit_layout(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } Point2D;

    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
    } Point3D;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point2D"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = ecs_entity(world, {.name = "Point3D"});
    ecs_add_pair(world, derived, EcsIsA, base);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t == derived);

    meta_test_struct(world, base, Point2D);
    meta_test_member(world, base, Point2D, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, base, Point2D, y, ecs_id(ecs_i32_t), 0);

    meta_test_struct(world, t, Point3D);
    meta_test_member(world, t, Point3D, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, Point3D, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, Point3D, z, ecs_id(ecs_i32_t), 0);

    test_int(meta_member_count(world, t), 3);

    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);
    test_str(ecs_struct_get_nth_member(world, t, 1)->name, "y");
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, 4);
    test_str(ecs_struct_get_nth_member(world, t, 2)->name, "z");
    test_int(ecs_struct_get_nth_member(world, t, 2)->offset, 8);

    ecs_fini(world);
}

void StructTypes_inherit_multi_level(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
        ecs_i32_t w;
    } Point4D;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point2D"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t base3 = ecs_entity(world, {.name = "Point3D"});
    ecs_add_pair(world, base3, EcsIsA, base2);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base3,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_entity(world, {.name = "Point4D"});
    ecs_add_pair(world, t, EcsIsA, base3);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"w", ecs_id(ecs_i32_t)}
        }
    });

    meta_test_struct(world, t, Point4D);
    meta_test_member(world, t, Point4D, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, Point4D, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, Point4D, z, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, t, Point4D, w, ecs_id(ecs_i32_t), 0);

    test_int(meta_member_count(world, t), 4);

    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);
    test_str(ecs_struct_get_nth_member(world, t, 1)->name, "y");
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, 4);
    test_str(ecs_struct_get_nth_member(world, t, 2)->name, "z");
    test_int(ecs_struct_get_nth_member(world, t, 2)->offset, 8);
    test_str(ecs_struct_get_nth_member(world, t, 3)->name, "w");
    test_int(ecs_struct_get_nth_member(world, t, 3)->offset, 12);

    ecs_fini(world);
}

void StructTypes_inherit_base_change_propagates(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    {
        const EcsComponent *c = ecs_get(world, derived, EcsComponent);
        test_assert(c != NULL);
        test_int(c->size, 8);

        test_int(meta_member_count(world, derived), 2);
        test_str(ecs_struct_get_nth_member(world, derived, 0)->name, "x");
        test_int(ecs_struct_get_nth_member(world, derived, 0)->offset, 0);
        test_str(ecs_struct_get_nth_member(world, derived, 1)->name, "z");
        test_int(ecs_struct_get_nth_member(world, derived, 1)->offset, 4);
    }

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base,
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    {
        const EcsComponent *c = ecs_get(world, derived, EcsComponent);
        test_assert(c != NULL);
        test_int(c->size, 12);

        test_int(meta_member_count(world, derived), 3);
        test_str(ecs_struct_get_nth_member(world, derived, 0)->name, "x");
        test_int(ecs_struct_get_nth_member(world, derived, 0)->offset, 0);
        test_str(ecs_struct_get_nth_member(world, derived, 1)->name, "y");
        test_int(ecs_struct_get_nth_member(world, derived, 1)->offset, 4);
        test_str(ecs_struct_get_nth_member(world, derived, 2)->name, "z");
        test_int(ecs_struct_get_nth_member(world, derived, 2)->offset, 8);
    }

    ecs_fini(world);
}

void StructTypes_inherit_base_tail_padding(void) {
    typedef struct {
        ecs_i32_t i;
        ecs_char_t c;
    } BaseTP;

    typedef struct {
        BaseTP base;
        ecs_char_t d;
    } DerivedTP;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "BaseTP"}),
        .members = {
            {"i", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_char_t)}
        }
    });

    ecs_entity_t t = ecs_entity(world, {.name = "DerivedTP"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"d", ecs_id(ecs_char_t)}
        }
    });

    meta_test_struct(world, base, BaseTP);
    meta_test_struct(world, t, DerivedTP);

    test_int(meta_member_count(world, t), 3);

    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "i");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);
    test_str(ecs_struct_get_nth_member(world, t, 1)->name, "c");
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, 4);

    test_str(ecs_struct_get_nth_member(world, t, 2)->name, "d");
    test_int(ecs_struct_get_nth_member(world, t, 2)->offset, 8);
    test_int(ecs_struct_get_nth_member(world, t, 2)->offset, offsetof(DerivedTP, d));

    ecs_fini(world);
}

void StructTypes_inherit_derived_larger_align(void) {
    typedef struct {
        ecs_i32_t x;
    } Base;

    typedef struct {
        Base base;
        ecs_f64_t v;
    } Derived;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"v", ecs_id(ecs_f64_t)}
        }
    });

    meta_test_struct(world, base, Base);
    meta_test_struct(world, t, Derived);

    test_int(meta_member_count(world, t), 2);

    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);

    test_str(ecs_struct_get_nth_member(world, t, 1)->name, "v");
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, 8);
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, offsetof(Derived, v));

    ecs_fini(world);
}

void StructTypes_inherit_shadow_member_fails(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t == 0);

    ecs_fini(world);
}

void StructTypes_inherit_empty_base(void) {
    typedef struct {
        ecs_i32_t z;
    } Derived;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"})
    });
    test_assert(base != 0);
    test_assert(!ecs_has(world, base, EcsType));

    ecs_entity_t t = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_entity_t r = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(r == t);

    meta_test_struct(world, t, Derived);

    test_int(meta_member_count(world, t), 1);
    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "z");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);

    ecs_fini(world);
}

void StructTypes_inherit_empty_derived(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } Point2D;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point2D"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_entity_t r = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t
    });
    test_assert(r == t);

    test_assert(ecs_has(world, t, EcsType));
    meta_test_struct(world, t, Point2D);

    test_int(meta_member_count(world, t), 2);
    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);
    test_str(ecs_struct_get_nth_member(world, t, 1)->name, "y");
    test_int(ecs_struct_get_nth_member(world, t, 1)->offset, 4);

    ecs_fini(world);
}

void StructTypes_inherit_empty_both(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"})
    });
    test_assert(base != 0);

    ecs_entity_t t = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_entity_t r = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t
    });
    test_assert(r == t);

    test_int(meta_member_count(world, t), 0);

    ecs_fini(world);
}

void StructTypes_inherit_from_non_component(void) {
    typedef struct {
        ecs_i32_t x;
    } Derived;

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_entity(world, {.name = "NotAType"});

    ecs_entity_t t = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, t, EcsIsA, base);
    ecs_entity_t r = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(r == t);

    meta_test_struct(world, t, Derived);

    test_int(meta_member_count(world, t), 1);
    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);

    ecs_fini(world);
}

void StructTypes_inherit_propagate_stress(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t d[16];
    int i;
    for (i = 0; i < 16; i ++) {
        d[i] = ecs_new(world);
        ecs_add_pair(world, d[i], EcsIsA, base);
        ecs_add_id(world, d[i], ecs_id(EcsStruct));
        test_assert(!ecs_has(world, d[i], EcsType));
    }

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base,
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    for (i = 0; i < 16; i ++) {
        test_int(meta_member_count(world, d[i]), 2);
        test_assert(ecs_has(world, d[i], EcsType));
    }

    ecs_entity_t mid = ecs_entity(world, {.name = "Mid"});
    ecs_add_pair(world, mid, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = mid, .members = {{"z", ecs_id(ecs_i32_t)}}
    });

    ecs_entity_t leaf = ecs_entity(world, {.name = "Leaf"});
    ecs_add_pair(world, leaf, EcsIsA, mid);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = leaf, .members = {{"w", ecs_id(ecs_i32_t)}}
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base,
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_int(meta_member_count(world, mid), 3);
    test_int(meta_member_count(world, leaf), 4);

    ecs_fini(world);
}

void StructTypes_inherit_base_member_type_change(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    {
        test_int(meta_member_count(world, derived), 2);
        test_uint(ecs_struct_get_nth_member(world, derived, 0)->type, ecs_id(ecs_i32_t));
        test_int(ecs_struct_get_nth_member(world, derived, 0)->offset, 0);
        test_int(ecs_struct_get_nth_member(world, derived, 1)->offset, 4);
        test_int(ecs_get(world, derived, EcsComponent)->size, 8);
    }

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base,
        .members = {
            {"x", ecs_id(ecs_i64_t)}
        }
    });

    {
        test_int(meta_member_count(world, derived), 2);
        test_str(ecs_struct_get_nth_member(world, derived, 0)->name, "x");
        test_uint(ecs_struct_get_nth_member(world, derived, 0)->type, ecs_id(ecs_i64_t));
        test_int(ecs_struct_get_nth_member(world, derived, 0)->offset, 0);
        test_str(ecs_struct_get_nth_member(world, derived, 1)->name, "z");
        test_int(ecs_struct_get_nth_member(world, derived, 1)->offset, 8);
        test_int(ecs_get(world, derived, EcsComponent)->size, 16);
    }

    ecs_fini(world);
}

void StructTypes_inherit_explicit_offset_fails(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i32_t), .offset = 64}
        }
    });

    test_assert(t == 0);

    ecs_fini(world);
}

void StructTypes_inherit_base_redefined_fewer_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    test_int(meta_member_count(world, derived), 3);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = base,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_int(meta_member_count(world, base), 1);
    test_int(meta_member_count(world, derived), 2);

    test_str(ecs_struct_get_nth_member(world, derived, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, derived, 0)->offset, 0);
    test_str(ecs_struct_get_nth_member(world, derived, 1)->name, "z");
    test_int(ecs_struct_get_nth_member(world, derived, 1)->offset, 4);

    test_assert(ecs_struct_get_member(world, derived, "y") == NULL);
    test_int(ecs_get(world, derived, EcsComponent)->size, 8);

    ecs_fini(world);
}

void StructTypes_redefine_fewer_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_int(meta_member_count(world, t), 2);
    test_int(ecs_get(world, t, EcsComponent)->size, 8);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = t,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_int(meta_member_count(world, t), 1);
    test_str(ecs_struct_get_nth_member(world, t, 0)->name, "x");
    test_int(ecs_struct_get_nth_member(world, t, 0)->offset, 0);
    test_assert(ecs_struct_get_member(world, t, "y") == NULL);
    test_int(ecs_get(world, t, EcsComponent)->size, 4);

    ecs_fini(world);
}
