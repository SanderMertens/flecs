#include "private_api.h"

/* -- Private functions -- */

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t flecs_type_contains(
    const ecs_world_t *world,
    ecs_type_t type_1,
    ecs_type_t type_2,
    bool match_all,
    bool match_prefab)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);
    
    if (!type_1) {
        return 0;
    }

    ecs_assert(type_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_1 == type_2) {
        return *(ecs_vector_first(type_1, ecs_entity_t));
    }

    if (ecs_vector_count(type_2) == 1) {
        return ecs_type_has_id(world, type_1, 
            ecs_vector_first(type_2, ecs_id_t)[0], !match_prefab);
    }

    int32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1, ecs_entity_t);
    ecs_entity_t *t2_array = ecs_vector_first(type_2, ecs_entity_t);

    ecs_assert(t1_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t2_array != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t t1_count = ecs_vector_count(type_1);
    int32_t t2_count = ecs_vector_count(type_2);

    for (i_2 = 0; i_2 < t2_count; i_2 ++) {
        ecs_entity_t e2 = t2_array[i_2];

        if (i_1 >= t1_count) {
            return 0;
        }

        e1 = t1_array[i_1];

        if (e2 > e1) {
            do {
                i_1 ++;
                if (i_1 >= t1_count) {
                    return 0;
                }
                e1 = t1_array[i_1];
            } while (e2 > e1);
        }

        if (e1 != e2) {
            if (e1 != e2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return e1;
            }
        } else {
            if (!match_all) return e1;
            i_1 ++;
            if (i_1 < t1_count) {
                e1 = t1_array[i_1];
            }
        }
    }

    if (match_all) {
        return e1;
    } else {
        return 0;
    }
}

/* -- Public API -- */

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */    
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);
    
    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);
    ecs_ids_t add_array = flecs_type_to_ids(to_add);
    ecs_ids_t remove_array = flecs_type_to_ids(to_remove);
    
    table = flecs_table_traverse_remove(
        unsafe_world, table, &remove_array, NULL); 

    table = flecs_table_traverse_add(
        unsafe_world, table, &add_array, NULL); 

    if (!table) {
        return NULL;
    } else {
        return table->type;
    }
}

static
bool has_case(
    const ecs_world_t *world,
    ecs_entity_t sw_case,
    ecs_entity_t e)
{
    const EcsType *type_ptr = ecs_get(world, e & ECS_COMPONENT_MASK, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_type_has_id(world, type_ptr->normalized, sw_case, false);
}

static
bool match_id(
    const ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t match_with)
{
    if (ECS_HAS_ROLE(match_with, CASE)) {
        ecs_entity_t sw_case = match_with & ECS_COMPONENT_MASK;
        if (ECS_HAS_ROLE(id, SWITCH) && has_case(world, sw_case, id)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return ecs_id_match(id, match_with);
    }
}

static
int32_t search_type(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    int32_t depth,
    ecs_entity_t *out)
{
    if (!id) {
        return -1;
    }

    if (!type) {
        return -1;
    }

    if (max_depth && depth > max_depth) {
        return -1;
    }

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);

    if (depth >= min_depth) {
        if (table && !offset && !(ECS_HAS_ROLE(id, CASE))) {
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            if (tr) {
                return tr->column;
            }
        } else {
            for (i = offset; i < count; i ++) {
                if (match_id(world, ids[i], id)) {
                    return i;
                }
            }
        }
    }

    if (rel && id != EcsPrefab && id != EcsDisabled && 
        id != ecs_pair(ecs_id(EcsIdentifier), EcsName)) 
    {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = ids[i];
            if (!ECS_HAS_RELATION(e, rel)) {
                continue;
            }

            ecs_entity_t obj = ecs_pair_object(world, e);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *obj_table = ecs_get_table(world, obj);
            if (!obj_table) {
                continue;
            }

            if ((search_type(world, obj_table, obj_table->type, 0, id, 
                rel, min_depth, max_depth, depth + 1, out) != -1))
            {
                if (out && !*out) {
                    *out = obj;
                }
                return i;

            /* If the id could not be found on the object and the relationship
             * is not IsA, try substituting the object type with IsA */
            } else if (rel != EcsIsA) {
                if (search_type(world, obj_table, obj_table->type, 0, 
                    id, EcsIsA, 1, 0, 0, out) != -1) 
                {
                    if (out && !*out) {
                        *out = obj;
                    }
                    return i;
                }
            }
        }
    }

    return -1;
}

bool ecs_type_has_id(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_id_t id,
    bool owned)
{
    return search_type(world, NULL, type, 0, id, owned ? 0 : EcsIsA, 0, 0, 0, NULL) != -1;
}

int32_t ecs_type_index_of(
    ecs_type_t type, 
    int32_t offset, 
    ecs_id_t id)
{
    return search_type(NULL, NULL, type, offset, id, 0, 0, 0, 0, NULL);
}

int32_t ecs_type_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *out)
{
    if (out) {
        *out = 0;
    }
    return search_type(world, table, type, offset, id, rel, min_depth, max_depth, 0, out);
}

bool ecs_type_has_type(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has)
{
    return flecs_type_contains(world, type, has, true, false) != 0;
}

bool ecs_type_owns_type(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return flecs_type_contains(world, type, has, true, !owned) != 0;
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);

    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);

    ecs_ids_t entities = {
        .array = &e,
        .count = 1
    };

    table = flecs_table_traverse_add(unsafe_world, table, &entities, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    
    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);
    
    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);

    ecs_ids_t entities = {
        .array = &e,
        .count = 1
    };

    table = flecs_table_traverse_remove(unsafe_world, table, &entities, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;    
}

char* ecs_type_str(
    const ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;

    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        char buffer[256];
        ecs_size_t len;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (e == 1) {
            ecs_os_strcpy(buffer, "EcsComponent");
            len = ecs_os_strlen("EcsComponent");
        } else {
            len = flecs_from_size_t(ecs_id_str(world, e, buffer, 256));
        }

        dst = ecs_vector_addn(&chbuf, char, len);
        ecs_os_memcpy(dst, buffer, len);
    }

    *(char*)ecs_vector_add(&chbuf, char) = '\0';

    char* result = ecs_os_strdup(ecs_vector_first(chbuf, char));
    ecs_vector_free(chbuf);
    return result;
}

ecs_entity_t ecs_type_get_entity_for_xor(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t xor)
{
    ecs_assert(
        ecs_type_has_id(world, type, ECS_XOR | xor, true),
        ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get(world, xor, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t xor_type = type_ptr->normalized;
    ecs_assert(xor_type != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        if (ecs_type_has_id(world, xor_type, array[i], true)) {
            return array[i];
        }
    }

    return 0;
}
