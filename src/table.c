#include <ecs/components.h>
#include <ecs/hash.h>
#include <ecs/entity.h>
#include "table.h"
#include "entity.h"

const EcsVectorParams components_vec_params = {
    .element_size = sizeof(EcsEntity*),
    .chunk_count = ECS_INITIAL_CHUNK_COUNT
};

EcsTable* ecs_table_new(
    EcsWorld *world)
{
    EcsTable *result = ecs_world_add_table(world);
    result->world = world;
    result->components = ecs_vector_new(&components_vec_params);
    result->components_hash = 0;
    result->rows = NULL;
    return result;
}

void ecs_table_add_component(
    EcsTable *table,
    EcsEntity *component_type)
{
    if (!table->rows) {
        EcsEntity **el = ecs_vector_add(
            table->components, &components_vec_params);
        *el = component_type;
    }
}

EcsResult ecs_table_finalize(
    EcsTable *table)
{
    EcsIter it = ecs_vector_iter(table->components, &components_vec_params);
    size_t table_size = 0;
    uint64_t components_hash = 0;

    if (!ecs_vector_count(table->components)) {
        goto error;
    }

    ecs_vector_sort(table->components, &components_vec_params, ecs_vector_compare_ptr);

    while (ecs_iter_hasnext(&it)) {
        EcsEntity **e = ecs_iter_next(&it);
        EcsType *t = ecs_entity_get_component(*e, EcsType_e);
        if (!t) {
            goto error;
        }

        table_size += t->size;
        ecs_hash(*e, sizeof(EcsEntity*), &components_hash);
    }

    table->rows_params.element_size = table_size;
    table->rows_params.chunk_count = ECS_INITIAL_CHUNK_COUNT;
    table->components_hash = components_hash;
    table->rows = ecs_vector_new(&table->rows_params);

    ecs_world_finalize_table(table->world, table);
    return EcsOk;
error:
    ecs_world_remove_table(table->world, table);
    return EcsError;
}
