#include "flecs_private.h"

static
ecs_entity_t split_entity_id(
    ecs_entity_t id,
    ecs_entity_t *entity)
{
    *entity = (id & ECS_ENTITY_MASK);
    return id;
}

ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type);

    /* Walk from back to front, as prefabs are always located
     * at the end of the type. */
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = array[i];

        if (e & ECS_INSTANCEOF) {
            ecs_entity_t prefab = e & ECS_ENTITY_MASK;
            ecs_type_t prefab_type = ecs_get_type(world, prefab);

            if (prefab == previous) {
                continue;
            }

            if (ecs_type_has_owned_entity(
                world, prefab_type, component, true)) 
            {
                return prefab;
            } else {
                prefab = ecs_find_entity_in_prefabs(
                    world, prefab, prefab_type, component, entity);
                if (prefab) {
                    return prefab;
                }
            }
        } else {
            /* If this is not a prefab, the following entities won't
                * be prefabs either because the array is sorted, and
                * the prefab bit is 2^63 which ensures that prefabs are
                * guaranteed to be the last entities in the type */
            break;
        }
    }

    return 0;
}

/* -- Private functions -- */

ecs_type_t ecs_type_find_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *array,
    int32_t count)
{
    ecs_entities_t entities = {
        .array = array,
        .count = count
    };

    ecs_table_t *table = ecs_table_find_or_create(world, stage, &entities);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Extend existing type with additional entity */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_add(world, stage, table, &entities, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Remove entity from type */
ecs_type_t ecs_type_remove_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_remove(world, stage, table, &entities, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

int32_t ecs_type_container_depth(
   ecs_world_t *world,
   ecs_type_t type,
   ecs_entity_t component)     
{
    int result = 0;

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type);

    for (i = count - 1; i >= 0; i --) {
        if (array[i] & ECS_CHILDOF) {
            ecs_type_t c_type = ecs_get_type(world, array[i] & ECS_ENTITY_MASK);
            int32_t j, c_count = ecs_vector_count(c_type);
            ecs_entity_t *c_array = ecs_vector_first(c_type);

            for (j = 0; j < c_count; j ++) {
                if (c_array[j] == component) {
                    result ++;
                    result += ecs_type_container_depth(world, c_type, component);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }
        } else if (!(array[i] & ECS_ENTITY_FLAGS_MASK)) {
            /* No more parents after this */
            break;
        }
    }

    return result;
}

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_type_t type_1,
    ecs_type_t type_2,
    bool match_all,
    bool match_prefab)
{
    if (!type_1) {
        return 0;
    }

    ecs_assert(type_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_1 == type_2) {
        return *(ecs_entity_t*)ecs_vector_first(type_1);
    }

    int32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1);
    ecs_entity_t *t2_array = ecs_vector_first(type_2);

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
            if (match_prefab && e2 != EEcsName && e2 != EEcsPrefab && e2 != EEcsDisabled) {
                if (ecs_find_entity_in_prefabs(world, 0, type_1, e2, 0)) {
                    e1 = e2;
                }
            }

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

int16_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_entity_t *buf = ecs_vector_first(type);
    int i, count = ecs_vector_count(type);
    
    for (i = 0; i < count; i ++) {
        if (buf[i] == entity) {
            return i; 
        }
    }

    return -1;
}

ecs_type_t ecs_type_merge_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    ecs_entities_t add_array = {
        .array = ecs_vector_first(to_add),
        .count = ecs_vector_count(to_add)
    };

    ecs_entities_t remove_array = {
        .array = ecs_vector_first(to_remove),
        .count = ecs_vector_count(to_remove)
    };
    
    table = ecs_table_traverse_remove(
        world, stage, table, &remove_array, NULL); 

    table = ecs_table_traverse_add(
        world, stage, table, &add_array, NULL); 

    if (!table) {
        return NULL;
    } else {
        return table->type;
    }
}

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_merge_intern(world, stage, type, to_add, to_remove);
}

ecs_type_t ecs_type_find(
    ecs_world_t *world,
    ecs_entity_t *array,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_find_intern(world, stage, array, count);
}

bool ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    if (!entity) {
        return true;
    }

    ecs_entity_t *array = ecs_vector_first(type);
    int i, count = ecs_vector_count(type);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e == entity) {
            return true;
        }

        if (e & ECS_INSTANCEOF) {
            ecs_entity_t base = e & ECS_ENTITY_MASK;
            if (ecs_has_entity(world, base, entity)) {
                return true;
            }
        }
    }

    return false;
}

bool ecs_type_has_owned_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    if (!type) {
        return false;
    }
    
    ecs_entity_t *array = ecs_vector_first(type);
    int i, count = ecs_vector_count(type);

    if (owned) {
        ecs_entity_t e = array[0];
        for (i = 0; i < count && entity != e && e < entity; i ++) {
            e = array[i];
        }
        return e == entity;
    } else {
        for (i = count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (e < ECS_INSTANCEOF) {
                return false;
            } else
            if (e & ECS_INSTANCEOF) {
                ecs_entity_t base = e & ECS_ENTITY_MASK;
                if (ecs_has_entity(world, base, entity)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool ecs_type_has_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has)
{
    return ecs_type_contains(world, type, has, true, false);
}

bool ecs_type_has_owned_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return ecs_type_contains(world, type, has, true, !owned);
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_add_intern(world, stage, type, e);
}

ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_remove_intern(world, stage, type, e);
}

char* ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;
    int32_t len;
    char buf[15];

    ecs_entity_t *handles = ecs_vector_first(type);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t h;
        ecs_entity_t flags = split_entity_id(handles[i], &h) & ECS_ENTITY_FLAGS_MASK;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (flags & ECS_INSTANCEOF) {
            int len = sizeof("INSTANCEOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            memcpy(dst, "INSTANCEOF|", len);
        }

        if (flags & ECS_CHILDOF) {
            int len = sizeof("CHILDOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            memcpy(dst, "CHILDOF|", len);
        }

        const char *str = NULL;
        if (h == 1) {
            /* Prevent issues during bootstrap */
            str = "EcsComponent";
        } else {
            EcsName *id = ecs_get_ptr(world, h, EcsName);
            if (id) {
                str = *id;
            } else {
                int h_int = h;
                sprintf(buf, "%u", h_int);
                str = buf;
            }
        }
        len = strlen(str);
        dst = ecs_vector_addn(&chbuf, char, len);
        memcpy(dst, str, len);
    }

    *(char*)ecs_vector_add(&chbuf, char) = '\0';

    char* result = ecs_os_strdup(ecs_vector_first(chbuf));
    ecs_vector_free(chbuf);
    return result;
}
