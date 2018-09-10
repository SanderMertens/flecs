#include <reflecs/entity.h>
#include <reflecs/vector.h>
#include <reflecs/hash.h>
#include <reflecs/components.h>
#include "entity.h"
#include "table.h"

EcsEntity *EcsType_e;

static const EcsVectorParams components_vec_params = {
    .element_size = sizeof(EcsComponentElement),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

static
EcsTable* ecs_entity_get_table(
    EcsEntity *entity,
    uint64_t component_hash)
{
    EcsTable *result = ecs_world_lookup_table(entity->world, component_hash);

    if (!result) {
        result = ecs_table_new(entity->world);
        EcsIter it = ecs_vector_iter(entity->components, &components_vec_params);
        while (ecs_iter_hasnext(&it)) {
            EcsComponentElement *elem = ecs_iter_next(&it);
            ecs_table_add_component(result, elem->component);
        }
        ecs_table_finalize(result);
    }

    return result;
}

static
void ecs_entity_insert(
    EcsEntity *entity,
    EcsTable *table)
{
    void *row = ecs_table_insert(table, entity);
    uint32_t column = 0;

    EcsIter it = ecs_vector_iter(entity->components, &components_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsComponentElement *elem = ecs_iter_next(&it);
        void *column_ptr = ecs_table_column(table, row, column);

        if (elem->data) {
            uint32_t column_size = ecs_table_column_size(table, column);
            memcpy(column_ptr, elem->data, column_size);
        }

        elem->data = column_ptr;
        column ++;
    }
}

/* Generate random id */
static
char* random_id(
    uint16_t n)
{
    static char *alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static char *alphanum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890";
    uint16_t i;
    char *result = malloc(n + 1);

    result[0] = alpha[rand() % strlen(alpha)];
    for (i = 1; i < n; i++) {
        result[i] = alphanum[rand() % strlen(alphanum)];
    }

    result[i] = '\0';

    return result;
}

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from)
{
    EcsIter it = ecs_vector_iter(entity->components, &components_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsComponentElement *elem = ecs_iter_next(&it);
        if (elem->data) {
            uint32_t offset = elem->data - from;
            elem->data = ECS_OFFSET(to, offset);
        }
    }
}

EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id)
{
    EcsEntity *result = ecs_world_alloc_entity(world);
    if (id) {
        result->id = strdup(id);
    } else {
        result->id = random_id(8);
        id = result->id;
    }

    result->id_hash = 0;
    ecs_hash(id, strlen(id), &result->id_hash);
    result->base = NULL;
    result->components = NULL;
    result->world = world;
    ecs_world_add_entity(world, result);
    return result;
}

void ecs_add(
    EcsEntity *entity,
    EcsEntity *component)
{
    if (!entity->components) {
        entity->components = ecs_vector_new(&components_vec_params);
    }

    EcsComponentElement *elem = ecs_vector_add(
        entity->components, &components_vec_params);

    elem->component = component;
    elem->data = NULL;
}

void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component)
{
    EcsIter it = ecs_vector_iter(entity->components, &components_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsComponentElement *elem = ecs_iter_next(&it);
        if (elem->component == component) {
            return elem->data;
        }
    }

    return NULL;
}

static
bool ecs_components_compare(
    void *a,
    void *b)
{
    return false;
}

void ecs_commit(
    EcsEntity *entity)
{
    uint64_t components_hash = 0;
    uint64_t old_components_hash = entity->components_hash;
    EcsIter it = ecs_vector_iter(entity->components, &components_vec_params);

    ecs_vector_sort(
        entity->components, &components_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsComponentElement *elem = ecs_iter_next(&it);
        ecs_hash(&elem->component, sizeof(EcsEntity*), &components_hash);
    }

    if (old_components_hash != components_hash) {
        EcsTable *table = ecs_entity_get_table(entity, components_hash);
        EcsTable *old_table;

        ecs_entity_insert(entity, table);

        if (old_components_hash) {
            old_table = ecs_world_lookup_table(
                entity->world, old_components_hash);

            EcsComponentElement *old_elem = ecs_vector_get(
                entity->components, &components_vec_params, 0);

            uint32_t offset = (uintptr_t)ecs_table_column(old_table, NULL, 0);
            void *row = ECS_OFFSET(old_elem->data, -offset);

            ecs_table_remove(old_table, row);
        }
    }
}

void ecs_init(void)
{
}

void ecs_fini(void)
{
}
