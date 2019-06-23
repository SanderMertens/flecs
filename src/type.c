#include "flecs_private.h"

const ecs_vector_params_t char_arr_params = {
    .element_size = sizeof(char)
};

const ecs_vector_params_t type_node_params = {
    .element_size = sizeof(ecs_type_node_t)
};

const ecs_vector_params_t ptr_params = {
    .element_size = sizeof(void*)
};

const ecs_vector_params_t link_params = {
    .element_size = sizeof(ecs_type_link_t)
};

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int add_type(
    ecs_world_t *world,
    ecs_system_expr_elem_kind_t elem_kind,
    ecs_system_expr_oper_kind_t oper_kind,
    const char *entity_id,
    const char *source_id,
    void *data)
{
    EcsTypeComponent *type_component = data;
    ecs_stage_t *stage = &world->main_stage;
    (void)source_id;

    if (oper_kind != EcsOperAnd) {
        return -1;
    }

    if (!strcmp(entity_id, "0")) {
        *type_component = (EcsTypeComponent){0, 0};
    } else {
        ecs_entity_t entity = ecs_lookup(world, entity_id);
        if (!entity) {
            ecs_os_err("%s not found", entity_id);
            return -1;
        }

        if (elem_kind == EcsFromContainer) {
            if (!ecs_has(world, entity, EcsContainer)) {
                ecs_add(world, entity, EcsContainer);
                ecs_set_watching(world, entity, true);
            }
        } else if (elem_kind != EcsFromSelf) {
            ecs_os_err("invalid prefix for component '%s'", entity_id);
            return -1;
        }

        ecs_type_t type = ecs_type_find_intern(world, stage, &entity, 1);
        ecs_type_t resolved_id = ecs_type_from_entity(world, entity);
        assert(resolved_id != 0);

        type_component->type = ecs_type_merge_intern(
            world, stage, type_component->type, type, 0);

        type_component->resolved = ecs_type_merge_intern(
            world, stage, type_component->resolved, resolved_id, 0);
    }

    return 0;
}

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(ecs_entity_t*)p1 - *(ecs_entity_t*)p2;
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

static
ecs_type_t register_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_link_t *link,
    ecs_entity_t *array,
    uint32_t count)
{
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count < ECS_MAX_ENTITIES_IN_TYPE, ECS_TYPE_TOO_LARGE, NULL);

    ecs_type_t result = ecs_type_from_array(array, count);
    link->type = result;

    if (stage->last_link) {
        stage->last_link->next = link;
        stage->last_link = link;
    }

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(result) == count, ECS_INTERNAL_ERROR, NULL);

    notify_systems_of_type(world, stage, result);
    
    return result;
}

static
ecs_type_t find_type_in_vector(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *vector,
    ecs_entity_t *array,
    uint32_t count,
    bool create)
{
    uint32_t i, types_count = ecs_vector_count(vector);
    ecs_type_t *type_array = ecs_vector_first(vector);

    for (i = 0; i < types_count; i ++) {
        ecs_type_t type = type_array[i];
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
        ecs_type_link_t *link = ecs_vector_add(&vector, &link_params);
        return register_type(world, stage, link, array, count);
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
    bool create)
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
                        world, stage, &node->link, array, i + 1);

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
                world, stage, node->types[index], array, count, create);
            if (create && !type) {
                return NULL;
            }
        }
        
        offset = e;
    }
    
    ecs_assert(!create || type != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!create || ecs_vector_count(type) == count, ECS_INTERNAL_ERROR, NULL);

    return type;
}

static
ecs_entity_t find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_type_t entity_type = ecs_type_from_entity(world, entity);
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type);

    /* Walk from back to front, as prefabs are always located
     * at the end of the type. */
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = array[i];
        if (e & EcsAsPrefab) {
            ecs_entity_t prefab = e & ECS_ENTITY_MASK;
            if (_ecs_has(world, prefab, entity_type)) {
                return prefab;
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
            world, stage, &world->main_stage.type_root, array, count, true);
    } else {
        type = find_or_create_type(
            world, stage, &world->main_stage.type_root, array, count, false);
    }

    if (!type && stage != &world->main_stage) {
        type = find_or_create_type(
            world, stage, &world->main_stage.type_root, array, count, true);
    }

    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(ecs_vector_count(type) == count, ECS_INTERNAL_ERROR, NULL);

    return type;
}

/** Extend existing type with additional entity */
ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = ecs_vector_count(type);
    ecs_entity_t *new_array = ecs_os_alloca(ecs_entity_t, count + 1);
    void *new_buffer = new_array;

    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

    if (count) {
        memcpy(new_array, ecs_vector_first(type), sizeof(ecs_entity_t) * count);
    }

    new_array[count] = e;
    qsort(new_array, count + 1, sizeof(ecs_entity_t), compare_handle);
    count ++;

    return ecs_type_find_intern(world, stage, new_buffer, count);
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
    ecs_entity_t cur_flags = 0, add_flags = 0, del_flags = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    (void)del_flags;

    del_count = ecs_vector_count(to_del);
    if (del_count) {
        buf_del = ecs_vector_first(to_del);
        del_flags = split_entity_id(buf_del[0], &del);
    }

    cur_count = ecs_vector_count(arr_cur);
    if (cur_count) {
        buf_cur = ecs_vector_first(arr_cur);
        cur_flags = split_entity_id(buf_cur[0], &cur);
    }

    add_count = ecs_vector_count(to_add);
    if (add_count) {
        buf_add = ecs_vector_first(to_add);
        add_flags = split_entity_id(buf_add[0], &add);
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
            buf_new[new_count] = add | add_flags;
            new_count ++;
            i_add ++;
            if (i_add < add_count) {
                add_flags = split_entity_id(buf_add[i_add], &add);
            } else {
                add = 0;
            }
        } else if (cur && (!add || cur < add)) {
            while (del && del < cur) {
                i_del ++;
                if (i_del < del_count) {
                    del_flags = split_entity_id(buf_del[i_del], &del);
                } else {
                    del = 0;
                }
            }

            if (del != cur) {
                ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
                buf_new[new_count] = cur | cur_flags;
                new_count ++;
            } else if (del == cur) {
                i_del ++;
                if (i_del < del_count) {
                    del_flags = split_entity_id(buf_del[i_del], &del);
                } else {
                    del = 0;
                }
            }

            i_cur ++;
            if (i_cur < cur_count) {
                cur_flags = split_entity_id(buf_cur[i_cur], &cur);
            } else {
                cur = 0;
            }
        } else if (add && add == cur) {
            ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
            buf_new[new_count] = add | add_flags | cur_flags;
            new_count ++;
            i_cur ++;
            i_add ++;

            if (i_add < add_count) {
                add_flags = split_entity_id(buf_add[i_add], &add);
            } else {
                add = 0;
            }

            if (i_cur < cur_count) {
                cur_flags = split_entity_id(buf_cur[i_cur], &cur);
            } else {
                cur = 0;
            }
        }
    } while (cur || add);

    if (new_count) {
        ecs_assert(
            (ecs_vector_count(arr_cur) + 
             ecs_vector_count(to_add)) >= new_count, ECS_INTERNAL_ERROR, 0);

        return ecs_type_find_intern(world, stage, buf_new, new_count);
    } else {
        return 0;
    }
}

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_stage_t *stage,
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
            if (match_prefab && e2 != EEcsId && e2 != EEcsPrefab) {
                if (find_entity_in_prefabs(world, type_1, e2)) {
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
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t entity,
    bool match_prefab)
{
    ecs_entity_t *array = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i++) {
        ecs_entity_t e = array[i];
        if ((e & ECS_ENTITY_MASK) == entity) {
            return true;
        }
    }

    if (match_prefab) {
        if (find_entity_in_prefabs(world, type, entity)) {
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

    for (i = 0; i < count; i ++) {
        ecs_row_t row;
        if (ecs_map_has(world->main_stage.entity_index, array[i], &row)) {
            ecs_type_t c_type = row.type;
            int32_t j, c_count = ecs_vector_count(c_type);
            ecs_entity_t *c_array = ecs_vector_first(c_type);

            bool found_container = false;
            bool found_component = false;

            for (j = 0; j < c_count; j ++) {
                if (c_array[j] == EEcsContainer) {
                    found_container = true;
                }
                if (c_array[j] == component) {
                    found_component = true;
                }

                if (found_component && found_container) {
                    result ++;
                    result += ecs_type_container_depth(world, c_type, component);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }
        }
    }

    return result;
}

/* -- Public API -- */

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    const char *id,
    const char *sig)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsTypeComponent type = {0};

    ecs_entity_t result;

    if (ecs_parse_component_expr(world, sig, add_type, &type) != 0) {
        return 0;
    }

    ecs_entity_t type_entity;

    if (ecs_map_has(world->type_handles, (uintptr_t)type.type, &type_entity)) {
        EcsId *id_ptr = ecs_get_ptr(world, type_entity, EcsId);

        ecs_assert(id_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        
        if(strcmp(*id_ptr, id)) {
            ecs_abort(ECS_ENTITY_ALREADY_DEFINED, id);
        }

        return type_entity;
    } else {
        result = _ecs_new(world, world->t_type);
        ecs_set(world, result, EcsId, {id});
        ecs_set(world, result, EcsTypeComponent, {
            .type = type.type, .resolved = type.resolved
        });

        ecs_map_set(world->type_handles, (uintptr_t)type.type, &result);

        return result;
    }
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    const char *id,
    const char *sig)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETERS, NULL);
    EcsTypeComponent type = {0};

    ecs_entity_t result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    if (ecs_parse_component_expr(world, sig, add_type, &type) != 0) {
        return 0;
    }

    type.resolved = ecs_type_merge_intern(
        world, NULL, world->t_prefab, type.resolved, 0);

    result = _ecs_new(world, type.resolved);
    ecs_set(world, result, EcsId, {id});

    return result;
}

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    const char *id,
    const char *components)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETERS, NULL);
    EcsTypeComponent type = {0};

    ecs_entity_t result = ecs_lookup(world, id);
    ecs_assert(!result, ECS_ENTITY_ALREADY_DEFINED, id);

    if (ecs_parse_component_expr(world, components, add_type, &type) != 0) {
        return 0;
    }

    result = _ecs_new(world, type.resolved);

    ecs_set(world, result, EcsId, {id});

    return result;
}

int16_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t component)
{
    ecs_entity_t *buf = ecs_vector_first(type);
    int i, count = ecs_vector_count(type);
    
    for (i = 0; i < count; i ++) {
        if (buf[i] == component) {
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
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

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

ecs_entity_t ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    return ecs_type_has_entity_intern(world, stage, type, entity, false);
}

char* _ecs_type_str(
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
        ecs_entity_t h = handles[i];
        if (i) {
            *(char*)ecs_vector_add(&chbuf, &char_arr_params) = ',';
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

