
#include "private_api.h"

static
bool path_append(
    ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    ecs_entity_t component,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_type_t type = ecs_get_type(world, child);
    ecs_entity_t cur = ecs_find_in_type(world, type, component, ECS_CHILDOF);
    
    if (cur) {
        if (cur != parent && cur != EcsFlecsCore) {
            path_append(world, parent, cur, component, sep, prefix, buf);
            ecs_strbuf_appendstr(buf, sep);
        }
    } else if (prefix) {
        ecs_strbuf_appendstr(buf, prefix);
    }

    char buff[22];
    const char *name = ecs_get_name(world, child);
    if (!name) {
        ecs_os_sprintf(buff, "%u", (uint32_t)child);
        name = buff;
    }

    ecs_strbuf_appendstr(buf, name);

    return cur != 0;
}

char* ecs_get_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    ecs_entity_t component,
    const char *sep,
    const char *prefix)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (parent != child) {
        path_append(world, parent, child, component, sep, prefix, &buf);
    } else {
        ecs_strbuf_appendstr(&buf, "");
    }

    return ecs_strbuf_get(&buf);
}

static 
ecs_entity_t name_to_id(
    const char *name)
{
    long int result = atol(name);
    ecs_assert(result >= 0, ECS_INTERNAL_ERROR, NULL);
    return (ecs_entity_t)result;
}

static
ecs_entity_t find_child_in_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    const char *name)
{
    /* If table doesn't have EcsName, then don't bother */
    int32_t name_index = ecs_type_index_of(table->type, ecs_entity(EcsName));
    if (name_index == -1) {
        return 0;
    }

    ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
    if (!data || !data->columns) {
        return 0;
    }

    int32_t i, count = ecs_vector_count(data->entities);
    if (!count) {
        return 0;
    }

    int is_number = isdigit(name[0]);
    if (is_number) {
        return name_to_id(name);
    }

    ecs_column_t *column = &data->columns[name_index];
    EcsName *names = ecs_vector_first(column->data, EcsName);

    for (i = 0; i < count; i ++) {
        const char *cur_name = names[i].value;
        const char *cur_sym = names[i].symbol;
        if ((cur_name && !strcmp(cur_name, name)) || (cur_sym && !strcmp(cur_sym, name))) {
            return *ecs_vector_get(data->entities, ecs_entity_t, i);
        }
    }

    return 0;
}

static
ecs_entity_t find_child_in_stage(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t parent,
    const char *name)
{
    (void)parent;
    
    ecs_sparse_each(stage->tables, ecs_table_t, table, {
        ecs_entity_t result = find_child_in_table(world, stage, table, name);
        if (result) {
            return result;
        }
    });    

    return 0;
}

ecs_entity_t ecs_lookup_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_entity_t result = 0;
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, parent);
    
    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, table_ptr, {
            ecs_table_t *table = *table_ptr;

            result = find_child_in_table(world, stage, table, name);
            if (!result) {
                if (stage != &world->stage) {
                    result = find_child_in_table(world, &world->stage, table, 
                        name);
                }
            }

            if (result) {
                return result;
            }
        });
    }

    /* If child hasn't been found it is possible that it was
     * created in a new table while staged, and the table hasn't
     * been registered with the child_table map yet. In that case we
     * have to look for the entity in the staged tables.
     * This edge case should rarely result in a lot of overhead,
     * since the number of tables should stabilize over time, which
     * means table creation while staged should be infrequent. */    
    if (!result && stage != &world->stage) {
        result = find_child_in_stage(world, stage, parent, name);
    }

    return result;
}

ecs_entity_t ecs_lookup(
    ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    if (isdigit(name[0])) {
        return name_to_id(name);
    }
    
    return ecs_lookup_child(world, 0, name);
}

ecs_entity_t ecs_lookup_symbol(
    ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    if (isdigit(name[0])) {
        return name_to_id(name);
    }
    
    return find_child_in_stage(world, &world->stage, 0, name);
}

static
bool is_sep(
    const char **ptr,
    const char *sep)
{
    ecs_size_t len = ecs_os_strlen(sep);

    if (!ecs_os_strncmp(*ptr, sep, len)) {
        *ptr += len - 1;
        return true;
    } else {
        return false;
    }
}

static
const char *path_elem(
    const char *path,
    char *buff,
    const char *sep)
{
    const char *ptr;
    char *bptr, ch;

    for (bptr = buff, ptr = path; (ch = *ptr); ptr ++) {
        if (is_sep(&ptr, sep)) {
            *bptr = '\0';
            return ptr + 1;
        } else {
            *bptr = ch;
            bptr ++;
        }
    }

    if (bptr != buff) {
        *bptr = '\0';
        return ptr;
    } else {
        return NULL;
    }
}

static
ecs_entity_t get_parent_from_path(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char **path_ptr,
    const char *prefix)
{
    bool start_from_root = false;
    const char *path = *path_ptr;

    if (prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(path, prefix, len)) {
            path += len;
            parent = 0;
            start_from_root = true;
        }
    }

    if (!start_from_root && !parent) {
        ecs_stage_t *stage = ecs_get_stage(&world);
        parent = stage->scope;
    }

    *path_ptr = path;

    return parent;
}

ecs_entity_t ecs_lookup_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr;
    ecs_entity_t cur;
    bool core_searched = false;

    if (!sep) {
        sep = ".";
    }

    parent = get_parent_from_path(world, parent, &path, prefix);

retry:
    cur = parent;
    ptr = path;

    while ((ptr = path_elem(ptr, buff, sep))) {
        cur = ecs_lookup_child(world, cur, buff);
        if (!cur) {
            goto tail;
        }
    }

tail:
    if (!cur) {
        if (!core_searched) {
            if (parent) {
                parent = ecs_get_parent_w_entity(world, parent, 0);
            } else {
                parent = EcsFlecsCore;
                core_searched = true;
            }
            goto retry;
        }
    }

    return cur;
}

ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_t e = ECS_CHILDOF | scope;
    ecs_entities_t to_add = {
        .array = &e,
        .count = 1
    };

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    if (scope) {
        stage->scope_table = ecs_table_traverse_add(
            world, stage, &world->stage.root, &to_add, NULL);
    } else {
        stage->scope_table = &world->stage.root;
    }

    return cur;
}

ecs_entity_t ecs_get_scope(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return stage->scope;
}

int32_t ecs_get_child_count(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_vector_t *tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, entity);
    if (!tables) {
        return 0;
    } else {
        int32_t count = 0;

        ecs_vector_each(tables, ecs_table_t*, table_ptr, {
            ecs_table_t *table = *table_ptr;
            count += ecs_table_count(table);
        });

        return count;
    }
}

ecs_iter_t ecs_scope_iter(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_scope_iter_t iter = {
        .tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, parent),
        .index = 0
    };

    return (ecs_iter_t) {
        .world = world,
        .iter.parent = iter
    };
}

ecs_iter_t ecs_scope_iter_w_filter(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_filter_t *filter)
{
    ecs_scope_iter_t iter = {
        .filter = *filter,
        .tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, parent),
        .index = 0
    };

    return (ecs_iter_t) {
        .world = world,
        .iter.parent = iter,
        .table_count = ecs_vector_count(iter.tables)
    };
}

bool ecs_scope_next(
    ecs_iter_t *it)
{
    ecs_scope_iter_t *iter = &it->iter.parent;
    ecs_vector_t *tables = iter->tables;
    ecs_filter_t filter = iter->filter;

    int32_t count = ecs_vector_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = *ecs_vector_get(tables, ecs_table_t*, i);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_vector_first(table->data, ecs_data_t);
        if (!data) {
            continue;
        }

        it->count = ecs_table_count(table);
        if (!it->count) {
            continue;
        }

        if (filter.include || filter.exclude) {
            if (!ecs_table_match_filter(it->world, table, &filter)) {
                continue;
            }
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;    
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    const char *old_prefix = world->name_prefix;
    world->name_prefix = prefix;
    return old_prefix;
}

ecs_entity_t ecs_add_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr = path;

    parent = get_parent_from_path(world, parent, &path, prefix);

    ecs_entity_t cur = parent;

    while ((ptr = path_elem(ptr, buff, sep))) {
        ecs_entity_t e = ecs_lookup_child(world, cur, buff);
        if (!e) {
            char *name = ecs_os_strdup(buff);

            /* If this is the last entity in the path, use the provided id */
            if (entity && !path_elem(ptr, buff, sep)) {
                e = entity;
            }
            
            e = ecs_set(world, e, EcsName, {
                .value = name,
                .alloc_value = name
            });

            ecs_os_free(name);

            if (cur) {
                ecs_add_entity(world, e, ECS_CHILDOF | cur);
            }
        }

        cur = e;
    }

    return cur;
}

ecs_entity_t ecs_new_from_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    return ecs_add_path_w_sep(world, 0, parent, path, sep, prefix);
}
