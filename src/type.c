#include "flecs_private.h"

const ecs_vector_params_t char_arr_params = {
    .element_size = sizeof(char)
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
    EcsTypeComponent *type = data;
    ecs_stage_t *stage = &world->main_stage;
    (void)source_id;

    if (oper_kind != EcsOperAnd) {
        return -1;
    }

    if (!strcmp(entity_id, "0")) {
        *type = (EcsTypeComponent){0, 0};
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

        ecs_type_t type_id = ecs_type_register(
            world, stage, entity, NULL);
        assert(type_id != 0);

        ecs_type_t resolved_id = ecs_type_from_handle(
            world, stage, entity, NULL);
        assert(resolved_id != 0);

        type->type = ecs_type_merge(
            world, stage, type->type, type_id, 0);

        type->resolved = ecs_type_merge(
            world, stage, type->resolved, resolved_id, 0);
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
uint32_t hash_handle_array(
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
void notify_create_type(
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
ecs_type_t register_type_from_buffer(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *buf,
    uint32_t count)
{
    if (!stage) {
        stage = &world->main_stage;
    }

    ecs_type_t new_id = hash_handle_array(buf, count);
    ecs_map_t *type_index = stage->type_index;

    ecs_vector_t *new_array = ecs_map_get(world->main_stage.type_index, new_id);
    if (!new_array) {
        new_array = ecs_map_get(type_index, new_id);
    }
    
    if (!new_array) {
        new_array = ecs_vector_new_from_buffer(&handle_arr_params, count, buf);
        ecs_map_set(type_index, new_id, new_array);

        if (!world->threads_running) {
            notify_create_type(world, stage, world->add_systems, new_id);
            notify_create_type(world, stage, world->remove_systems, new_id);
            notify_create_type(world, stage, world->set_systems, new_id);
        }
    }

    return new_id;
}

/* -- Private functions -- */

ecs_vector_t* ecs_type_get(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id)
{
    if (!type_id) {
        return NULL;
    }
    
    ecs_vector_t *result = ecs_map_get(world->main_stage.type_index, type_id);
    if (!result) {
        if (world->threads_running) {
            assert(stage != NULL);
            result = ecs_map_get(stage->type_index, type_id);
        }
    }

    return result;
}

/** Get type id from entity handle */
ecs_type_t ecs_type_from_handle(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    if (entity == 0) {
        return 0;
    }

    ecs_table_t *table = NULL;
    ecs_table_column_t *columns = NULL;
    uint32_t index = 0;
    ecs_entity_t component = 0;
    ecs_type_t type = 0;

    if (!info) {
        uint64_t row_64 = ecs_map_get64(world->main_stage.entity_index, entity);
        if (!row_64 && world->in_progress) {
            row_64 = ecs_map_get64(stage->entity_index, entity);
            if (!row_64) {
                return 0;
            }
        }

        ecs_row_t row = ecs_to_row(row_64);
        if (row.type_id) {
            table = ecs_world_get_table(world, stage, row.type_id);
            columns = table->columns;
            index = row.index - 1;
        }
    } else {
        table = info->table;
        columns = info->columns;
        index = info->index - 1;
    }

    if (table) {
        ecs_entity_t *components = ecs_vector_first(table->type);
        component = components[0];
    }

    if (component == EEcsTypeComponent) {
        ecs_vector_params_t params = {.element_size = sizeof(EcsTypeComponent)};
        EcsTypeComponent *fe = ecs_vector_get(columns[1].data, &params, index);
        type = fe->resolved;
    } else {
        type = ecs_type_register(world, stage, entity, NULL);
    }

    assert(type != 0);

    return type;
}


/** Register a new type, optionally extending from existing type */
ecs_type_t ecs_type_register(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t to_add,
    ecs_vector_t *set)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = ecs_vector_count(set);
    ecs_entity_t *new_set = ecs_os_alloca(ecs_entity_t, count + 1);
    void *new_buffer = new_set;

    if (to_add) {
        void *buffer = ecs_vector_first(set);
        if (count) {
            memcpy(new_set, buffer, sizeof(ecs_entity_t) * count);
        }
        new_set[count] = to_add;
        qsort(new_set, count + 1, sizeof(ecs_entity_t), compare_handle);
        count ++;
    } else if (set) {
        void *buffer = ecs_vector_first(set);
        new_buffer = buffer;
    } else {
        return 0;
    }

    return register_type_from_buffer(world, stage, new_buffer, count);
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component)
{
    ecs_vector_t *array = ecs_type_get(world, stage, type);
    assert(!type || array != NULL);
    return ecs_type_register(world, stage, component, array);
}

ecs_type_t ecs_type_merge_arr(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *arr_cur,
    ecs_vector_t *to_add,
    ecs_vector_t *to_del)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *buf_add = NULL, *buf_del = NULL, *buf_cur = NULL;
    ecs_entity_t cur = 0, add = 0, del = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    if (to_del) {
        del_count = ecs_vector_count(to_del);
        buf_del = ecs_vector_first(to_del);
        del = buf_del[0];
    }

    if (arr_cur) {
        cur_count = ecs_vector_count(arr_cur);
        buf_cur = ecs_vector_first(arr_cur);
        cur = buf_cur[0];
    }

    if (to_add) {
        add_count = ecs_vector_count(to_add);
        buf_add = ecs_vector_first(to_add);
        add = buf_add[0];
    }

    ecs_entity_t *buf_new = NULL; 
    if (cur_count + add_count) {
        buf_new = ecs_os_alloca(ecs_entity_t, cur_count + add_count);
    }

    uint32_t new_count = 0;

    do {
        if (add && (!cur || add < cur)) {
            ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
            buf_new[new_count] = add;
            new_count ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
        } else if (cur && (!add || cur < add)) {
            while (del && del < cur) {
                i_del ++;
                del = i_del < del_count ? buf_del[i_del] : 0;
            }

            if (del != cur) {
                ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
                buf_new[new_count] = cur;
                new_count ++;
            } else if (del == cur) {
                i_del ++;
                del = i_del < del_count ? buf_del[i_del] : 0;
            }

            i_cur ++;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        } else if (add && add == cur) {
            ecs_assert(buf_new != NULL, ECS_INTERNAL_ERROR, NULL);
            buf_new[new_count] = add;
            new_count ++;
            i_cur ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        }
    } while (cur || add);

    if (new_count) {
        ecs_assert(
            (ecs_vector_count(arr_cur) + 
             ecs_vector_count(to_add)) >= new_count, ECS_INTERNAL_ERROR, 0);

        return register_type_from_buffer(world, stage, buf_new, new_count);
    } else {
        return 0;
    }
}

/** O(n) algorithm to merge families */
ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t cur_id,
    ecs_type_t to_add_id,
    ecs_type_t to_remove_id)
{
    if (!to_remove_id) {
        if (cur_id && !to_add_id) {
            return cur_id;
        } else if (to_add_id && !cur_id) {
            return to_add_id;
        } else if (to_add_id == cur_id) {
            return cur_id;
        }
    } else if (to_remove_id == cur_id) {
        return to_add_id;
    }

    ecs_vector_t *arr_cur = ecs_type_get(world, stage, cur_id);
    ecs_vector_t *to_add = NULL, *to_del = NULL;

    if (to_remove_id) {
        to_del = ecs_type_get(world, stage, to_remove_id);
    }

    if (to_add_id) {
        to_add = ecs_type_get(world, stage, to_add_id);
    }

    return ecs_type_merge_arr(world, stage, arr_cur, to_add, to_del);
}

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab)
{
    if (!type_id_1) {
        return 0;
    }

    ecs_assert(type_id_2 != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *f_1 = ecs_type_get(world, stage, type_id_1);
    ecs_assert(f_1 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_id_1 == type_id_2) {
        return *(ecs_entity_t*)ecs_vector_first(f_1);
    }

    ecs_vector_t *f_2 = ecs_type_get(world, stage, type_id_2);
    ecs_assert(f_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t i_2, i_1 = 0;
    ecs_entity_t *h2p, *h1p = ecs_vector_get(f_1, &handle_arr_params, i_1);
    ecs_entity_t h1 = 0, prefab = 0;
    bool prefab_searched = false;

    for (i_2 = 0; (h2p = ecs_vector_get(f_2, &handle_arr_params, i_2)); i_2 ++) {
        ecs_entity_t h2 = *h2p;

        if (!h1p) {
            return 0;
        }

        h1 = *h1p;

        if (h2 > h1) {
            do {
                i_1 ++;
                h1p = ecs_vector_get(f_1, &handle_arr_params, i_1);
            } while (h1p && (h1 = *h1p) && h2 > h1);
        }

        if (h1 != h2) {
            if (match_prefab && !prefab_searched) {
                prefab = ecs_map_get64(world->prefab_index, type_id_1);
                prefab_searched = true;
            }

            if (prefab) {
                if (h2 != EEcsId && h2 != EEcsPrefab) {
                    ecs_type_t type = ecs_type_from_entity(world, h2);
                    if (_ecs_has(world, prefab, type)) {
                        h1 = h2;
                    }
                }
            }

            if (h1 != h2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return h1;
            }
        } else {
            if (!match_all) return h1;
            i_1 ++;
            h1p = ecs_vector_get(f_1, &handle_arr_params, i_1);
        }
    }

    if (match_all) {
        return h1;
    } else {
        return 0;
    }
}

bool ecs_type_contains_component(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id,
    ecs_entity_t component,
    bool match_prefab)
{
    ecs_vector_t *type = ecs_type_get(world, stage, type_id);
    ecs_entity_t *buffer = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i++) {
        if (buffer[i] == component) {
            return true;
        }
    }

    if (match_prefab) {
        ecs_entity_t prefab = ecs_map_get64(world->prefab_index, type_id);
        if (prefab) {
            ecs_type_t component_type = ecs_type_from_entity(world, component);
            if (_ecs_has(world, prefab, component_type)) {
                return true;
            }
        }
    }

    return false;
}

int32_t ecs_type_container_depth(
   ecs_world_t *world,
   ecs_type_t type,
   ecs_entity_t component)     
{
    ecs_vector_t *arr = ecs_map_get(world->main_stage.type_index, type);
    ecs_assert(arr != NULL, ECS_INTERNAL_ERROR, NULL);

    int result = 0;

    int32_t i, count = ecs_vector_count(arr);
    ecs_entity_t *buffer = ecs_vector_first(arr);

    for (i = 0; i < count; i ++) {
        uint64_t row64 = ecs_map_get64(world->main_stage.entity_index, buffer[i]);
        if (row64) {
            ecs_row_t row = ecs_to_row(row64);
            ecs_type_t c_type = row.type_id;
            ecs_vector_t *c_arr = ecs_map_get(world->main_stage.type_index, c_type);
            int32_t j, c_count = ecs_vector_count(c_arr);
            ecs_entity_t *c_buffer = ecs_vector_first(c_arr);

            bool found_container = false;
            bool found_component = false;

            for (j = 0; j < c_count; j ++) {
                if (c_buffer[j] == EEcsContainer) {
                    found_container = true;
                }
                if (c_buffer[j] == component) {
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

    ecs_entity_t type_entity = ecs_map_get64(
        world->type_handles, type.type);

    if (type_entity) {
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

        ecs_map_set64(world->type_handles, type.type, result);

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

    type.resolved = ecs_type_merge(
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
    ecs_vector_t *type,
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

ecs_type_t _ecs_merge_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t type_add,
    ecs_type_t type_remove)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_merge(world, stage, type, type_add, type_remove);
}

ecs_entity_t ecs_type_get_component(
    ecs_world_t *world,
    ecs_type_t type_id,
    uint32_t index)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_vector_t *components = ecs_map_get(stage->type_index, type_id);
    ecs_entity_t *buffer = ecs_vector_first(components);

    if (ecs_vector_count(components) > index) {
        return buffer[index];
    } else {
        return 0;
    }
}

char* _ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type_id)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_vector_t *type = ecs_type_get(world, stage, type_id);
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
