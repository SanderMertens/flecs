#include <meta.h>

void meta_test_vector(
    ecs_world_t *world, 
    ecs_entity_t t, 
    ecs_entity_t elem_type) 
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_int(ct->size, ECS_SIZEOF(ecs_vector_t*));
    test_int(ct->alignment, ECS_ALIGNOF(ecs_vector_t*));

    const EcsMetaType *mt = ecs_get(world, t, EcsMetaType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsVectorType);

    const EcsVector *v = ecs_get(world, t, EcsVector);
    test_assert(v != NULL);
    test_assert(v->type == elem_type);
}

void VectorTypes_vector_bool() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(v != 0);
    test_str(ecs_get_name(world, v), "V");
    
    meta_test_vector(world, v, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void VectorTypes_vector_i32() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(v != 0);
    test_str(ecs_get_name(world, v), "V");
    
    meta_test_vector(world, v, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void VectorTypes_vector_struct() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = t
    });

    test_assert(v != 0);
    test_str(ecs_get_name(world, v), "V");
    
    meta_test_vector(world, v, t);

    ecs_fini(world);
}
