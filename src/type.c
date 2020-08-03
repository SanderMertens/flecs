#include "private_api.h"

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
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

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

            if (ecs_type_owns_entity(
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
        return *(ecs_vector_first(type_1, ecs_entity_t));
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
            if (match_prefab && e2 != 
                ecs_entity(EcsName) && e2 != 
                EcsPrefab && e2 != 
                EcsDisabled) 
            {
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

int32_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == entity) {
            return c_ptr_i; 
        }
    });

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
    ecs_entities_t add_array = ecs_type_to_entities(to_add);
    ecs_entities_t remove_array = ecs_type_to_entities(to_remove);
    
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
    ecs_entity_t trait = 0;

    if (!entity) {
        return true;
    }

    if (entity & ECS_TRAIT) {
        trait = entity & ECS_ENTITY_MASK;
    }

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        ecs_entity_t e = *c_ptr;
        if (e == entity) {
            return true;
        }

        if (e & ECS_INSTANCEOF && entity != EcsPrefab && entity != EcsDisabled){
            ecs_entity_t base = e & ECS_ENTITY_MASK;
            if (ecs_has_entity(world, base, entity)) {
                return true;
            }
        } else 
        if (trait && e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (trait == ecs_entity_t_hi(e)) {
                return true;
            }
        }
    });

    return false;
}

bool ecs_type_owns_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    bool is_trait = false;

    if (!type) {
        return false;
    }

    if (entity & ECS_TRAIT) {
        is_trait = true;
        entity = entity & ECS_ENTITY_MASK;
    }
    
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    int i, count = ecs_vector_count(type);

    if (owned) {
        if (is_trait) {
             for (i = 0; i < count; i ++) {
                 ecs_entity_t e = array[i];
                 if (e & ECS_TRAIT) {
                     e &= ECS_ENTITY_MASK;
                     if (ecs_entity_t_hi(e) == entity) {
                         return true;
                     }
                 }
             }
        } else {
            ecs_entity_t e = array[0];
            for (i = 0; i < count && entity != e && e < entity; i ++) {
                e = array[i];
            }
            return e == entity;
        }
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
    return ecs_type_contains(world, type, has, true, false) != 0;
}

bool ecs_type_owns_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return ecs_type_contains(world, type, has, true, !owned) != 0;
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

static
void append_name(
    ecs_world_t *world,
    ecs_vector_t **chbuf,
    ecs_entity_t h)
{
    const char *str = NULL;
    char *dst;

    if (h == 1) {
        /* Prevent issues during bootstrap */
        str = "EcsComponent";
    } else {
        str = ecs_get_fullpath(world, h);
    }

    ecs_assert(str != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t len = ecs_os_strlen(str);
    dst = ecs_vector_addn(chbuf, char, len);
    ecs_os_memcpy(dst, str, len);
    if (h != 1) {
        ecs_os_free((char*)str);
    }
}

char* ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;

    ecs_entity_t *handles = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t h;
        ecs_entity_t trait = 0;
        ecs_entity_t flags = split_entity_id(handles[i], &h) & ECS_TYPE_ROLE_MASK;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (flags & ECS_INSTANCEOF) {
            int len = sizeof("INSTANCEOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "INSTANCEOF|", len);
        }

        if (flags & ECS_CHILDOF) {
            int len = sizeof("CHILDOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "CHILDOF|", len);
        }

        if (flags & ECS_TRAIT) {
            int len = sizeof("TRAIT|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "TRAIT|", len);
            trait = ecs_entity_t_hi(h);
            h = ecs_entity_t_lo(h);
        }

        if (flags & ECS_XOR) {
            int len = sizeof("XOR|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "XOR|", len);
        }

        if (flags & ECS_OR) {
            int len = sizeof("OR|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "OR|", len);
        }

        if (flags & ECS_AND) {
            int len = sizeof("AND|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "AND|", len);
        }

        if (flags & ECS_NOT) {
            int len = sizeof("NOT|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "NOT|", len);
        }

        append_name(world, &chbuf, h);

        if (trait) {
            char *ch = ecs_vector_add(&chbuf, char);
            *ch = '<';
            append_name(world, &chbuf, trait);
        }
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
        ecs_type_owns_entity(world, type, ECS_XOR | xor, true),
        ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get(world, xor, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t xor_type = type_ptr->normalized;
    ecs_assert(xor_type != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        if (ecs_type_owns_entity(world, xor_type, array[i], true)) {
            return array[i];
        }
    }

    return 0;
}

int32_t ecs_type_trait_index_of(
    ecs_type_t type, 
    int32_t start_index, 
    ecs_entity_t trait)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    for (i = start_index; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (trait == ecs_entity_t_hi(e)) {
                return i;
            }
        }
    }

    return -1;
}
