#include "flecs_private.h"

const ecs_vector_params_t char_arr_params = {
    .element_size = sizeof(char)
};

const ecs_vector_params_t type_node_params = {
    .element_size = sizeof(ecs_type_node_t)
};

const ecs_vector_params_t link_params = {
    .element_size = sizeof(ecs_type_link_t*)
};

static
ecs_type_t find_or_create_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_node_t *root,
    ecs_entity_t *array,
    uint32_t count,
    bool create,
    bool normalized);

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int parse_type_action(
    ecs_world_t *world,
    const char *system_id,
    const char *sig,
    int column,
    ecs_system_expr_elem_kind_t elem_kind,
    ecs_system_expr_oper_kind_t oper_kind,
    ecs_system_expr_inout_kind_t inout_kind,
    const char *entity_id,
    const char *source_id,
    void *data)
{
    ecs_vector_t **array = data;
    (void)source_id;
    (void)inout_kind;

    if (strcmp(entity_id, "0")) {
        ecs_entity_t entity = 0;

        if (elem_kind != EcsFromSelf) {
            ecs_parser_error(system_id, sig, column, 
                "source modifiers not supported for type expressions");
            return -1;
        }

        if (!strcmp(entity_id, "INSTANCEOF")) {
            entity = ECS_INSTANCEOF;
        } else if (!strcmp(entity_id, "CHILDOF")) {
            entity = ECS_CHILDOF;
        } else {
            entity = ecs_lookup(world, entity_id);
        }
        
        if (!entity) {
            ecs_parser_error(system_id, sig, column, 
                "unresolved identifier '%s'", entity_id);
            return -1;
        }

        if (oper_kind == EcsOperAnd) {
            ecs_entity_t* e_ptr = ecs_vector_add(array, &handle_arr_params);
            *e_ptr = entity;
        } else if (oper_kind == EcsOperOr) {
            ecs_entity_t *e_ptr = ecs_vector_last(*array, &handle_arr_params);

            /* If using an OR operator, the array should at least have one 
             * element. */
            ecs_assert(e_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            if (*e_ptr & ECS_ENTITY_MASK) {
                if (entity & ECS_ENTITY_MASK) {
                    /* An expression should not OR entity ids, only entities +
                     * entity flags. */
                    ecs_parser_error(system_id, sig, column, 
                        "| operator unsupported in type expression");
                    return -1;
                }
            }

            *e_ptr |= entity;
        } else {
            /* Only AND and OR operators are supported for type expressions */
            ecs_parser_error(system_id, sig, column, 
                "invalid operator for type expression");
            return -1;
        }
    }

    return 0;
}

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    ecs_entity_t e1 = *(ecs_entity_t*)p1;
    ecs_entity_t e2 = *(ecs_entity_t*)p2;
    return e1 > e2 ? 1 : e1 < e2 ? -1 : 0;
}

/** Hash array of handles */
static
uint32_t hash_array(
    ecs_entity_t* array,
    uint32_t count)
{
    uint32_t hash = 0;
    uint32_t i;
    for (i = 0; i < count; i ++) {
        ecs_hash(&array[i], sizeof(ecs_entity_t), &hash);
    }
    return hash;
}

static
void notify_systems_array_of_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *systems,
    ecs_type_t type)
{
    ecs_entity_t *buffer = ecs_vector_first(systems);
    uint32_t i, count = ecs_vector_count(systems);

    for (i = 0; i < count; i ++) {
        ecs_row_system_notify_of_type(world, stage, buffer[i], type);
    }
}

static
void notify_systems_of_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type)
{
    notify_systems_array_of_type(world, stage, world->add_systems, type);
    notify_systems_array_of_type(world, stage, world->remove_systems, type);
    notify_systems_array_of_type(world, stage, world->set_systems, type);
}

static
ecs_entity_t split_entity_id(
    ecs_entity_t id,
    ecs_entity_t *entity)
{
    *entity = (id & ECS_ENTITY_MASK);
    return id;
}

static
ecs_type_t ecs_type_from_array(
    ecs_entity_t *array,
    uint32_t count)
{
    ecs_vector_t *vector = ecs_vector_new(&handle_arr_params, count);
    ecs_vector_set_count(&vector, &handle_arr_params, count);
    ecs_entity_t *vector_first = ecs_vector_first(vector);
    memcpy(vector_first, array, sizeof(ecs_entity_t) * count);

    return vector;
}

/* Algorithm that guarantees each entity only occurs once in a type. Even though
 * the elements of a type are ordered, the same entity with flags or without
 * flags typically is stored at a different index, as a flag changes the element
 * value, and thus the ordering.
 * This complicates merging of two types, which, if both types are perfectly
 * ordered and have no flags, is an O(n) operation. With flags however, a merge
 * still needs to yield a type in which each entity occurs no more than once, 
 * and this cannot be done in O(n).
 * Type merging is a common operation which can happen often in the main loop,
 * thus it needs to be fast. Similarly, it would be possible to ignore type
 * flags when ordering, but then other common operations (those that check for
 * prefabs / containers) become more expensive (O(1) to O(n)).
 * To move this complexity out of the main loop, the algorithm that ensures each
 * entity only occurs once in a type (this function) is only performed when a
 * new type is registered. The result of this operation is that the type tree
 * contains an entry for the non-normalized type which points to the normalized
 * type. This ensures that a type merge can produce a non-normalized array,
 * which when looked up, is guaranteed to return a normalized type.
 * This results in some extra memory usage for extra entries in the type tree.
 * For example, a type [A] may occur multiple times:
 *
 * - [A]
 * - [A, INSTANCEOF|A]
 * - [A, CHILDOF|A]
 * - [A, INSTANCEOF|CHILDOF|A]
 * - [INSTANCEOF|A]
 * - [CHILDOF|A]
 * - [INSTANCEOF|CHILDOF|A]
 *
 * While worst case this could result in a lot of memory overhead, in practice
 * this is manageable. Entities are typically only used in combination with a
 * single flag, and types typically have comparatively low numbers of entities
 * with flags. If in the previous example, "A" would always be used as a prefab,
 * it would only create this entry:
 *
 * - [INSTANCEOF|A]
 */
static
ecs_type_t ecs_type_from_array_normalize(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *array,
    uint32_t count)
{
    ecs_entity_t *dst_array = ecs_os_alloca(ecs_entity_t, count);
    uint32_t dst_count = 0;
    
    uint32_t i, j;
    for (i = 0; i < count; i ++) {
        ecs_entity_t ie = array[i];
        ecs_entity_t ie_e = ie & ECS_ENTITY_MASK;
        ecs_entity_t found = 0;

        for (j = count - 1; j > i; j --) {
            ecs_entity_t je = array[j];
            ecs_entity_t je_e = je & ECS_ENTITY_MASK;

            if (je_e == ie_e) {
                found = je;
                break;
            }
        }

        if (!found) {
            dst_array[dst_count ++] = ie;
        } else {
            array[j] |= ie;
        }
    }

    return find_or_create_type(
        world, stage, &stage->type_root, dst_array, dst_count, true, true);
}

static
void mark_parents(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *array,
    uint32_t count)
{
    int i;
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = array[i];

        if (e & (ECS_CHILDOF | ECS_INSTANCEOF)) {
            ecs_set_watch(world, stage, e & ECS_ENTITY_MASK);
        } else if (!(e & ECS_ENTITY_FLAGS_MASK)) {
            break;
        }
    }   
}

static
ecs_type_t register_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_link_t *link,
    ecs_entity_t *array,
    uint32_t count,
    bool normalized)
{
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count < ECS_MAX_ENTITIES_IN_TYPE, ECS_TYPE_TOO_LARGE, NULL);

    ecs_type_t result;
    bool has_flags = (array[count - 1] & ECS_ENTITY_FLAGS_MASK) != 0;
    
    if (!normalized && has_flags) {
        result = ecs_type_from_array_normalize(world, stage, array, count);
    } else {
        result = ecs_type_from_array(array, count);

        if (has_flags) {
            mark_parents(world, stage, array, count);
        }
    }

    link->type = result;

    if (!stage->last_link) {
        stage->last_link = world->main_stage.last_link;
    }

    ecs_assert(stage->last_link != NULL, ECS_INTERNAL_ERROR, NULL);

    stage->last_link->next = link;
    stage->last_link = link;

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    if (normalized || !has_flags) {
        /* Only notify systems of normalized type */
        notify_systems_of_type(world, stage, result);
    }
    
    return result;
}

static
ecs_type_t find_type_in_vector(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t **vector,
    ecs_entity_t *array,
    uint32_t count,
    bool create,
    bool normalized)
{
    uint32_t i, types_count = ecs_vector_count(*vector);
    ecs_type_link_t **type_array = ecs_vector_first(*vector);

    for (i = 0; i < types_count; i ++) {
        ecs_type_t type = type_array[i]->type;
        if (!type) {
            continue;
        }

        uint32_t type_count = ecs_vector_count(type);

        if (type_count != count) {
            continue;
        }

        ecs_entity_t *type_array = ecs_vector_first(type);

        uint32_t j;
        for (j = 0; j < type_count; j ++) {
            if (type_array[j] != array[j]) {
                break;
            }
        }

        if (j == type_count) {
            return type;
        }
    }

    /* Type has not been found, add it */
    if (create) {
        ecs_type_link_t **link = ecs_vector_add(vector, &link_params);
        *link = ecs_os_calloc(1, sizeof(ecs_type_link_t));
        ecs_type_t result = register_type(world, stage, *link, array, count, normalized);
        return result;
    }
    
    return NULL;
}

static
ecs_type_t find_or_create_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_node_t *root,
    ecs_entity_t *array,
    uint32_t count,
    bool create,
    bool normalized)
{
    ecs_type_node_t *node = root;
    ecs_type_t type = NULL;
    ecs_entity_t offset = 0;
    uint32_t i;

    ecs_assert(count < ECS_MAX_ENTITIES_IN_TYPE, ECS_TYPE_TOO_LARGE, NULL);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        ecs_entity_t rel = e - offset;

        /* Lookup next node in nodes array */
        if (rel < ECS_TYPE_DB_MAX_CHILD_NODES) {
            ecs_type_node_t *node_array = NULL;

            if (!node->nodes && create) {
                /* Create vector */
                ecs_vector_set_count(&node->nodes, &type_node_params, 
                    ECS_TYPE_DB_MAX_CHILD_NODES);

                node_array = ecs_vector_first(node->nodes);

                memset(node_array, 0, 
                    ECS_TYPE_DB_MAX_CHILD_NODES * sizeof(ecs_type_node_t));
            } else {
                node_array = ecs_vector_first(node->nodes);
            }
            
            if (node_array) {
                node = &node_array[rel];
                type = node->link.type;
                
                if (!type && create) {
                    type = register_type(
                        world, stage, &node->link, array, i + 1, normalized);

                    ecs_assert(
                        ecs_vector_count(type) == i + 1, 
                        ECS_INTERNAL_ERROR, 
                        NULL);
                }
            } else {
                type = NULL;
            }

        } else {
            if (!node->types) {
                if (create) {
                    node->types = ecs_os_calloc(
                        ECS_TYPE_DB_BUCKET_COUNT, sizeof(ecs_vector_t*));
                } else {
                    return NULL;
                }
            }

            uint32_t hash = hash_array(&array[i], count - i);
            uint32_t index = hash % ECS_TYPE_DB_BUCKET_COUNT;

            if (!node->types[index]) {
                if (create) {
                    node->types[index] = ecs_vector_new(&link_params, 1);
                } else {
                    return NULL;
                }
            }

            type = find_type_in_vector(
                world, stage, &node->types[index], array, count, create, 
                normalized);

            if (type) {
                break;
            }

            if (create) {
                return NULL;
            }
        }
        
        offset = e;
    }
    
    ecs_assert(!create || type != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!create || !normalized || ecs_vector_count(type) == count, ECS_INTERNAL_ERROR, NULL);

    return type;
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

            if (ecs_type_has_entity_intern(
                world, prefab_type, component, false)) 
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
    uint32_t count)
{
    ecs_type_t type = NULL;

    if (!stage) {
        stage = &world->main_stage;
    }

    if (stage == &world->main_stage) {
        type = find_or_create_type(
            world, stage, &world->main_stage.type_root, array, count, true, false);
    } else {
        type = find_or_create_type(
            world, stage, &world->main_stage.type_root, array, count, false, false);
    }

    if (!type && stage != &world->main_stage) {
        type = find_or_create_type(
            world, stage, &world->main_stage.type_root, array, count, true, false);
    }

    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    return type;
}

/** Extend existing type with additional entity */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = ecs_vector_count(type);
    ecs_entity_t *new_array = ecs_os_alloca(ecs_entity_t, count + 1);
    ecs_entity_t *old_array = ecs_vector_first(type);
    void *new_buffer = new_array;

    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

    uint32_t i, pos = 0;
    bool in_type = false;
    for (i = 0; i < count; i ++) {
        ecs_entity_t elem = old_array[i];
        if (elem < e) {
            new_array[pos ++] = elem;
        } else if (elem > e && !in_type) {
            new_array[pos ++] = e;
            new_array[pos ++] = elem;
            in_type = true;
        } else {
            new_array[pos ++] = elem;
            in_type = true;
        }
    }

    if (!in_type && i == pos) {
        new_array[pos ++] = e;
    }

    ecs_type_t result = ecs_type_find_intern(world, stage, new_buffer, pos);
    ecs_assert(ecs_vector_count(result) == pos, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pos - count <= 1, ECS_INTERNAL_ERROR, NULL);

    return result;
}

/** Remove entity from type */
ecs_type_t ecs_type_remove_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!type) {
        return NULL;
    }

    uint32_t count = ecs_vector_count(type);
    ecs_entity_t *new_array = ecs_os_alloca(ecs_entity_t, count);
    ecs_entity_t *old_array = ecs_vector_first(type);
    void *new_buffer = new_array;

    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

    uint32_t i, pos = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t elem = old_array[i];
        if (elem != e) {
            new_array[pos ++] = elem;
        }
    }

    ecs_type_t result = ecs_type_find_intern(world, stage, new_buffer, pos);
    ecs_assert(ecs_vector_count(result) == pos, ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_type_t ecs_type_merge_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t arr_cur,
    ecs_type_t to_add,
    ecs_type_t to_del)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!to_del) {
        if (arr_cur && !to_add) {
            return arr_cur;
        } else if (to_add && !arr_cur) {
            return to_add;
        } else if (to_add == arr_cur) {
            return arr_cur;
        }
    } else if (to_del == arr_cur) {
        return to_add;
    }

    ecs_entity_t *buf_add = NULL, *buf_del = NULL, *buf_cur = NULL;
    ecs_entity_t cur = 0, add = 0, del = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    del_count = ecs_vector_count(to_del);
    if (del_count) {
        buf_del = ecs_vector_first(to_del);
        del = buf_del[0];
    }

    cur_count = ecs_vector_count(arr_cur);
    if (cur_count) {
        buf_cur = ecs_vector_first(arr_cur);
        cur = buf_cur[0];
    }

    add_count = ecs_vector_count(to_add);
    if (add_count) {
        buf_add = ecs_vector_first(to_add);
        add = buf_add[0];
    }

    ecs_entity_t *buf_new = NULL; 
    if (cur_count + add_count) {
        ecs_assert((cur_count + add_count) < ECS_MAX_ENTITIES_IN_TYPE, ECS_TYPE_TOO_LARGE, NULL);
        buf_new = ecs_os_alloca(ecs_entity_t, cur_count + add_count);
    }

    uint32_t new_count = 0;

    do {
        if (add && (!cur || add < cur)) {
            ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
            buf_new[new_count] = add;
            new_count ++;
            i_add ++;
            if (i_add < add_count) {
                add = buf_add[i_add];
            } else {
                add = 0;
            }
        } else if (cur && (!add || cur < add)) {
            while (del && del < cur) {
                i_del ++;
                if (del_count && i_del < del_count) {
                    del = buf_del[i_del];
                } else {
                    del = 0;
                }
            }

            if (del != cur) {
                ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
                buf_new[new_count] = cur;
                new_count ++;
            } else if (del == cur) {
                i_del ++;
                if (i_del < del_count) {
                    del = buf_del[i_del];
                } else {
                    del = 0;
                }
            }

            i_cur ++;
            if (i_cur < cur_count) {
                cur = buf_cur[i_cur];
            } else {
                cur = 0;
            }
        } else if (add && add == cur) {
            ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
            buf_new[new_count] = add;
            new_count ++;
            i_cur ++;
            i_add ++;

            if (i_add < add_count) {
                add = buf_add[i_add];
            } else {
                add = 0;
            }

            if (i_cur < cur_count) {
                cur = buf_cur[i_cur];
            } else {
                cur = 0;
            }
        }
    } while (cur || add);

    if (new_count) {
        ecs_assert(
            (ecs_vector_count(arr_cur) + 
             ecs_vector_count(to_add)) >= new_count, ECS_INTERNAL_ERROR, 0);

        ecs_type_t result = ecs_type_find_intern(world, stage, buf_new, new_count);
        return result;
    } else {
        return 0;
    }
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

    uint32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1);
    ecs_entity_t *t2_array = ecs_vector_first(type_2);

    ecs_assert(t1_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t2_array != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t t1_count = ecs_vector_count(type_1);
    uint32_t t2_count = ecs_vector_count(type_2);

    for (i_2 = 0; i_2 < t2_count; i_2 ++) {
        ecs_entity_t e2 = t2_array[i_2] & ECS_ENTITY_MASK;

        if (i_1 >= t1_count) {
            return 0;
        }

        e1 = t1_array[i_1] & ECS_ENTITY_MASK;

        if (e2 > e1) {
            do {
                i_1 ++;
                if (i_1 >= t1_count) {
                    return 0;
                }
                e1 = t1_array[i_1] & ECS_ENTITY_MASK;
            } while (e2 > e1);
        }

        if (e1 != e2) {
            if (match_prefab && e2 != EEcsId && e2 != EEcsPrefab && e2 != EEcsDisabled) {
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
                e1 = t1_array[i_1] & ECS_ENTITY_MASK;
            }
        }
    }

    if (match_all) {
        return e1;
    } else {
        return 0;
    }
}

bool ecs_type_has_entity_intern(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool match_prefab)
{
    ecs_entity_t *array = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i++) {
        ecs_entity_t e = array[i];
        if ((e == entity) || (e & ECS_ENTITY_MASK) == entity) {
            return true;
        }
    }

    if (match_prefab) {
        if (ecs_find_entity_in_prefabs(world, 0, type, entity, 0)) {
            return true;
        }
    }

    return false;
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

static
EcsTypeComponent type_from_vec(
    ecs_world_t *world,
    ecs_vector_t *vec)
{
    EcsTypeComponent result = {0, 0};
    ecs_entity_t *array = ecs_vector_first(vec);
    uint32_t i, count = ecs_vector_count(vec);

    for (i = 0; i < count; i ++) {
        ecs_entity_t entity = array[i];

        ecs_type_t entity_type = ecs_type_find_intern(world, &world->main_stage, &entity, 1);
        ecs_type_t resolved_type = ecs_type_from_entity(world, entity);
        assert(resolved_type != 0);

        result.type = ecs_type_merge_intern(
            world, &world->main_stage, result.type, entity_type, 0);

        result.resolved = ecs_type_merge_intern(
            world, &world->main_stage, result.resolved, resolved_type, 0);
    }

    return result;
}

static
EcsTypeComponent type_from_expr(
    ecs_world_t *world,
    const char *id,
    const char *expr)
{
    if (expr) {
        ecs_vector_t *vec = ecs_vector_new(&handle_arr_params, 1);
        ecs_parse_component_expr(world, expr, parse_type_action, id, &vec);
        EcsTypeComponent result = type_from_vec(world, vec);
        ecs_vector_free(vec);
        return result;
    } else {
        return (EcsTypeComponent){0, 0};
    }
}

/* -- Public API -- */

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    const char *id,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);  

    EcsTypeComponent type = type_from_expr(world, id, expr);
    ecs_entity_t result = ecs_lookup(world, id);

    if (result) {
        EcsTypeComponent *type_ptr = ecs_get_ptr(world, result, EcsTypeComponent);
        if (type_ptr) {
            if (type_ptr->type != type.type || 
                type_ptr->resolved != type.resolved) 
            {
                ecs_abort(ECS_ALREADY_DEFINED, id);
            }
        } else {
            ecs_abort(ECS_ALREADY_DEFINED, id);
        }
    } else if (!result) {
        result = _ecs_new(world, world->t_type);
        ecs_set(world, result, EcsId, {id});
        ecs_set(world, result, EcsTypeComponent, {
            .type = type.type, .resolved = type.resolved
        });

        /* Register named types with world, so applications can automatically
         * detect features (amongst others). */
        ecs_map_set(world->type_handles, (uintptr_t)type.type, &result);
    }

    return result;
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    const char *id,
    const char *expr)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
   
    EcsTypeComponent type = type_from_expr(world, id, expr);
    type.resolved = ecs_type_merge_intern(
        world, NULL, world->t_prefab, type.resolved, 0);

    ecs_entity_t result = ecs_lookup(world, id);
    if (result) {
        if (ecs_get_type(world, result) != type.resolved) {
            ecs_abort(ECS_ALREADY_DEFINED, id);
        }
    } else {
        result = _ecs_new(world, type.resolved);
        ecs_set(world, result, EcsId, {id});
    }

    return result;
}

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    const char *id,
    const char *expr)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
   
    EcsTypeComponent type = type_from_expr(world, id, expr);

    ecs_entity_t result = ecs_lookup(world, id);
    if (result) {
        if (ecs_get_type(world, result) != type.resolved) {
            ecs_abort(ECS_ALREADY_DEFINED, id);
        }
    } else {
        result = _ecs_new(world, type.resolved);
        ecs_set(world, result, EcsId, {id});
    }

    return result;
}

int16_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_entity_t *buf = ecs_vector_first(type);
    int i, count = ecs_vector_count(type);
    
    for (i = 0; i < count; i ++) {
        if ((buf[i] & ECS_ENTITY_MASK) == entity) {
            return i; 
        }
    }

    return -1;
}

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t type_add,
    ecs_type_t type_remove)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_merge_intern(world, stage, type, type_add, type_remove);
}

ecs_type_t ecs_type_find(
    ecs_world_t *world,
    ecs_entity_t *array,
    uint32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    qsort(array, count, sizeof(ecs_entity_t), compare_handle);

    return ecs_type_find_intern(world, stage, array, count);
}

ecs_entity_t ecs_type_get_entity(
    ecs_world_t *world,
    ecs_type_t type,
    uint32_t index)
{
    (void)world;
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!type) {
        return 0;
    }
    
    ecs_entity_t *array = ecs_vector_first(type);

    if (ecs_vector_count(type) > index) {
        return array[index];
    } else {
        return 0;
    }
}

bool ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    return ecs_type_has_entity_intern(world, type, entity, false);
}

ecs_type_t ecs_expr_to_type(
    ecs_world_t *world,
    const char *expr)
{
    EcsTypeComponent type = type_from_expr(world, "<type>", expr);
    return type.resolved;
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

char* ecs_type_to_expr(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_vector_t *chbuf = ecs_vector_new(&char_arr_params, 32);
    char *dst;
    uint32_t len;
    char buf[15];

    ecs_entity_t *handles = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t h;
        ecs_entity_t flags = split_entity_id(handles[i], &h) & ECS_ENTITY_FLAGS_MASK;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, &char_arr_params) = ',';
        }

        if (flags & ECS_INSTANCEOF) {
            int len = sizeof("INSTANCEOF|") - 1;
            dst = ecs_vector_addn(&chbuf, &char_arr_params, len);
            memcpy(dst, "INSTANCEOF|", len);
        }

        if (flags & ECS_CHILDOF) {
            int len = sizeof("CHILDOF|") - 1;
            dst = ecs_vector_addn(&chbuf, &char_arr_params, len);
            memcpy(dst, "CHILDOF|", len);
        }

        const char *str = NULL;
        EcsId *id = ecs_get_ptr(world, h, EcsId);
        if (id) {
            str = *id;
        } else {
            int h_int = h;
            sprintf(buf, "%u", h_int);
            str = buf;
        }
        len = strlen(str);
        dst = ecs_vector_addn(&chbuf, &char_arr_params, len);
        memcpy(dst, str, len);
    }

    *(char*)ecs_vector_add(&chbuf, &char_arr_params) = '\0';

    char *result = strdup(ecs_vector_first(chbuf));
    ecs_vector_free(chbuf);
    return result;
}

bool ecs_type_match_w_filter(
    ecs_world_t *world,
    ecs_type_t type,
    const ecs_filter_t *filter)
{
    if (!filter) {
        return true;
    }
    
    if (filter->include) {
        /* If filter kind is exact, types must be the same */
        if (filter->include_kind == EcsMatchExact) {
            if (type != filter->include) {
                return false;
            }

        /* Default for include_kind is MatchAll */
        } else if (!ecs_type_contains(world, type, filter->include, 
            filter->include_kind != EcsMatchAny, true)) 
        {
            return false;
        }
    
    /* If no include filter is specified, make sure that builtin components
        * aren't matched by default. */
    } else {
        if (ecs_type_contains(
            world, type, world->t_builtins, false, false))
        {
            /* Continue if table contains any of the builtin components */
            return false;
        }
    }

    if (filter->exclude) {
        /* If filter kind is exact, types must be the same */
        if (filter->exclude_kind == EcsMatchExact) {
            if (type == filter->exclude) {
                return false;
            }
        
        /* Default for exclude_kind is MatchAny */                
        } else if (ecs_type_contains(world, type, filter->exclude, 
            filter->exclude_kind == EcsMatchAll, true))
        {
            return false;
        }
    }

    return true;
}
