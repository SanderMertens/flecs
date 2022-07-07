#include <meta.h>

static
void meta_test_bitmask(
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
    test_assert(mt->kind == EcsBitmaskType);

    const EcsBitmask *bt = ecs_get(world, t, EcsBitmask);
    test_assert(bt != NULL);
    test_int(ecs_map_count(bt->constants), count);
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

    const EcsBitmask *bt = ecs_get(world, t, EcsBitmask);
    test_assert(bt != NULL);

    ecs_map_iter_t it = ecs_map_iter(bt->constants);
    ecs_bitmask_constant_t *c;
    ecs_map_key_t key;
    bool constant_found = false;
    while ((c = ecs_map_next(&it, ecs_bitmask_constant_t, &key))) {
        test_int(c->value, key);
        test_assert(c->constant != 0);
        test_str(c->name, ecs_get_name(world, c->constant));

        if (!ecs_os_strcmp(c->name, name)) {
            test_int(c->value, value);

            const ecs_u32_t *vptr = ecs_get_pair_object(world, c->constant, 
                EcsConstant, ecs_u32_t);
            if (vptr) {
                test_int(*vptr, value);
            }

            constant_found = true;
        }
    }

    test_assert(constant_found == true);
}

void BitmaskTypes_bitmask_1_constant() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}
        }
    });

    meta_test_bitmask(world, b, 1);
    meta_test_constant(world, b, "Lettuce", 1);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_2_constants() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}
        }
    });

    meta_test_bitmask(world, b, 2);
    meta_test_constant(world, b, "Lettuce", 1);
    meta_test_constant(world, b, "Bacon", 2);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_3_constants() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    meta_test_bitmask(world, b, 3);
    meta_test_constant(world, b, "Lettuce", 1);
    meta_test_constant(world, b, "Bacon", 2);
    meta_test_constant(world, b, "Tomato", 4);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_4_constants() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });

    meta_test_bitmask(world, b, 4);
    meta_test_constant(world, b, "Lettuce", 1);
    meta_test_constant(world, b, "Bacon", 2);
    meta_test_constant(world, b, "Tomato", 4);
    meta_test_constant(world, b, "Cheese", 8);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_4_constants_manual_values() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce", 8}, {"Bacon", 4}, {"Tomato", 2}, {"BLT", 16}
        }
    });

    meta_test_bitmask(world, b, 4);
    meta_test_constant(world, b, "Lettuce", 8);
    meta_test_constant(world, b, "Bacon", 4);
    meta_test_constant(world, b, "Tomato", 2);
    meta_test_constant(world, b, "BLT", 16);

    ecs_fini(world);
}

void BitmaskTypes_struct_w_bitmask() {
    typedef struct {
        ecs_bool_t before;
        ecs_u32_t v;
        ecs_bool_t after;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"before", ecs_id(ecs_bool_t)},
            {"v", b},
            {"after", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, before, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, v, b, 1);
    meta_test_member(world, t, T, after, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}
