#include <meta.h>

static
void meta_test_enum(
    ecs_world_t *world, 
    ecs_entity_t t,
    int32_t count) 
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_int(ct->size, ECS_SIZEOF(ecs_i32_t));
    test_int(ct->alignment, ECS_ALIGNOF(ecs_i32_t));

    const EcsMetaType *mt = ecs_get(world, t, EcsMetaType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsEnumType);

    const EcsEnum *et = ecs_get(world, t, EcsEnum);
    test_assert(et != NULL);
    test_int(ecs_map_count(&et->constants), count);
}

static
void meta_test_constant(
    ecs_world_t *world, 
    ecs_entity_t t, 
    const char *name,
    int32_t value)
{
    ecs_entity_t m = ecs_lookup_child(world, t, name);
    test_assert(m != 0);
    test_assert(ecs_has_id(world, m, EcsConstant) || 
        ecs_has_pair(world, m, EcsConstant, EcsWildcard));

    const EcsEnum *et = ecs_get(world, t, EcsEnum);
    test_assert(et != NULL);

    ecs_map_iter_t it = ecs_map_iter(&et->constants);
    bool constant_found = false;
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);

        test_int(c->value, key);
        test_assert(c->constant != 0);
        test_str(c->name, ecs_get_name(world, c->constant));

        if (!ecs_os_strcmp(c->name, name)) {
            test_int(c->value, value);

            const ecs_i32_t *vptr = ecs_get_pair_object(world, c->constant, 
                EcsConstant, ecs_i32_t);
            if (vptr) {
                test_int(*vptr, value);
            }

            constant_found = true;
        }
    }

    test_assert(constant_found == true);
}

void EnumTypes_enum_1_constant() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}
        }
    });

    test_assert(e != 0);

    meta_test_enum(world, e, 1);
    meta_test_constant(world, e, "Red", 0);

    ecs_fini(world);
}

void EnumTypes_enum_2_constants() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}
        }
    });

    test_assert(e != 0);

    meta_test_enum(world, e, 2);
    meta_test_constant(world, e, "Red", 0);
    meta_test_constant(world, e, "Blue", 1);

    ecs_fini(world);
}

void EnumTypes_enum_3_constants() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    meta_test_enum(world, e, 3);
    meta_test_constant(world, e, "Red", 0);
    meta_test_constant(world, e, "Blue", 1);
    meta_test_constant(world, e, "Green", 2);

    ecs_fini(world);
}

void EnumTypes_enum_3_constants_manual_values() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red", 3}, {"Blue", 2}, {"Green", 1}
        }
    });

    test_assert(e != 0);

    meta_test_enum(world, e, 3);
    meta_test_constant(world, e, "Red", 3);
    meta_test_constant(world, e, "Blue", 2);
    meta_test_constant(world, e, "Green", 1);

    ecs_fini(world);
}

void EnumTypes_struct_w_enum() {
    typedef struct {
        ecs_bool_t before;
        ecs_i32_t v;
        ecs_bool_t after;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"before", ecs_id(ecs_bool_t)},
            {"v", e},
            {"after", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, before, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, v, e, 1);
    meta_test_member(world, t, T, after, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}

void EnumTypes_zero_initialized() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, EcsComponent));

    ecs_entity_t ent = ecs_new_id(world);
    ecs_add_id(world, ent, e);

    const int32_t *ptr = ecs_get_id(world, ent, e);
    test_assert(ptr != NULL);
    test_int(*ptr, 0);

    ecs_fini(world);
}

void EnumTypes_enum_relation() {
    ecs_world_t *world = ecs_init();

    enum Color {
        Red, Blue, Green
    };

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, EcsComponent));
    test_assert(ecs_has_id(world, e, EcsExclusive));
    test_assert(ecs_has_id(world, e, EcsTag));

    ecs_entity_t red = ecs_lookup_child(world, e, "Red");
    ecs_entity_t green = ecs_lookup_child(world, e, "Green");
    ecs_entity_t blue = ecs_lookup_child(world, e, "Blue");

    test_assert(red != 0);
    test_assert(green != 0);
    test_assert(blue != 0);

    test_assert(ecs_get_typeid(world, ecs_pair(e, red)) == 0);

    ecs_entity_t ent = ecs_new_id(world);
    ecs_add_pair(world, ent, e, red);
    test_assert( ecs_has_pair(world, ent, e, red));

    ecs_add_pair(world, ent, e, green);
    test_assert( ecs_has_pair(world, ent, e, green));
    test_assert( !ecs_has_pair(world, ent, e, red));

    ecs_add_pair(world, ent, e, blue);
    test_assert( ecs_has_pair(world, ent, e, blue));
    test_assert( !ecs_has_pair(world, ent, e, green));
    test_assert( !ecs_has_pair(world, ent, e, red));

    ecs_fini(world);
}

void EnumTypes_enum_w_short_notation() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, EcsEnum));

    ecs_fini(world);
}
