
#include "private_api.h"

static
bool path_append(
    const ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    ecs_entity_t component,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = ecs_get_type(world, child);
    ecs_entity_t cur = ecs_find_in_type(world, type, component, EcsChildOf);
    
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

static
ecs_entity_t find_as_alias(
    const ecs_world_t *world,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(world->aliases);
    ecs_alias_t *aliases = ecs_vector_first(world->aliases, ecs_alias_t);
    for (i = 0; i < count; i ++) {
        if (!strcmp(aliases[i].name, name)) {
            return aliases[i].entity;
        }
    }

    return 0;
}

static
bool is_number(
    const char *name)
{
    if (!isdigit(name[0])) {
        return false;
    }

    ecs_size_t i, s = ecs_os_strlen(name);
    for (i = 1; i < s; i ++) {
        if (!isdigit(name[i])) {
            break;
        }
    }

    return i >= s;
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
    const ecs_table_t *table,
    const char *name)
{
    /* If table doesn't have EcsName, then don't bother */
    int32_t name_index = ecs_type_index_of(table->type, ecs_id(EcsName));
    if (name_index == -1) {
        return 0;
    }

    ecs_data_t *data = ecs_table_get_data(table);
    if (!data || !data->columns) {
        return 0;
    }

    int32_t i, count = ecs_vector_count(data->entities);
    if (!count) {
        return 0;
    }

    if (is_number(name)) {
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
ecs_entity_t find_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{        
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    (void)parent;
    
    ecs_sparse_each(world->store.tables, ecs_table_t, table, {
        ecs_entity_t result = find_child_in_table(table, name);
        if (result) {
            return result;
        }
    });    

    return 0;
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
        ecs_assert(bptr - buff < ECS_MAX_NAME_LENGTH, ECS_INVALID_PARAMETER, 
            NULL);
            
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
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char **path_ptr,
    const char *prefix,
    bool new_entity)
{
    bool start_from_root = false;
    const char *path = *path_ptr;
   
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(path, prefix, len)) {
            path += len;
            parent = 0;
            start_from_root = true;
        }
    }

    if (!start_from_root && !parent && new_entity) {
        parent = ecs_get_scope(world);
    }

    *path_ptr = path;

    return parent;
}

char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    ecs_entity_t component,
    const char *sep,
    const char *prefix)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    if (!sep) {
        sep = ".";
    }
        
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (parent != child) {
        path_append(world, parent, child, component, sep, prefix, &buf);
    } else {
        ecs_strbuf_appendstr(&buf, "");
    }

    return ecs_strbuf_get(&buf);
}

ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);
    ecs_entity_t result = 0;

    ecs_id_record_t *r = ecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {        
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            result = find_child_in_table(tr->table, name);
            if (result) {
                return result;
            }            
        }
    }

    return result;
}

ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);    

    if (is_number(name)) {
        return name_to_id(name);
    }

    ecs_entity_t e = find_as_alias(world, name);
    if (e) {
        return e;
    }    
    
    return ecs_lookup_child(world, 0, name);
}

ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);    

    if (is_number(name)) {
        return name_to_id(name);
    }   
    
    return find_child(world, 0, name);
}

ecs_entity_t ecs_lookup_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix,
    bool recursive)
{
    if (!path) {
        return 0;
    }

    if (!sep) {
        sep = ".";
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    if (path[0] == '.' && !path[1]) {
        return EcsThis;
    }

    ecs_entity_t e = find_as_alias(world, path);
    if (e) {
        return e;
    }      
    
    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr;
    ecs_entity_t cur;
    bool core_searched = false;

    if (!sep) {
        sep = ".";
    }

    parent = get_parent_from_path(world, parent, &path, prefix, true);

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
    if (!cur && recursive) {
        if (!core_searched) {
            if (parent) {
                parent = ecs_get_object_w_id(world, parent, EcsChildOf, 0);
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
    ecs_stage_t *stage = ecs_stage_from_world(&world);

    ecs_entity_t e = ecs_pair(EcsChildOf, scope);
    ecs_entities_t to_add = {
        .array = &e,
        .count = 1
    };

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    if (scope) {
        stage->scope_table = ecs_table_traverse_add(
            world, &world->store.root, &to_add, NULL);
    } else {
        stage->scope_table = &world->store.root;
    }

    return cur;
}

ecs_entity_t ecs_get_scope(
    const ecs_world_t *world)
{
    const ecs_stage_t *stage = ecs_stage_from_readonly_world(world);
    return stage->scope;
}

int32_t ecs_get_child_count(
    const ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    int32_t count = 0;

    ecs_id_record_t *r = ecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            count += ecs_table_count(tr->table);
        }
    }

    return count;
}

ecs_iter_t ecs_scope_iter_w_filter(
    ecs_world_t *iter_world,
    ecs_entity_t parent,
    ecs_filter_t *filter)
{
    ecs_assert(iter_world != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_world_t *world = (ecs_world_t*)ecs_get_world(iter_world);
    ecs_iter_t it = {
        .world = iter_world
    };

    ecs_id_record_t *r = ecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {
        it.iter.parent.tables = ecs_map_iter(r->table_index);
        it.table_count = ecs_map_count(r->table_index);
        if (filter) {
            it.iter.parent.filter = *filter;
        }
    }

    return it;
}

ecs_iter_t ecs_scope_iter(
    ecs_world_t *iter_world,
    ecs_entity_t parent)
{
    return ecs_scope_iter_w_filter(iter_world, parent, NULL);
}

bool ecs_scope_next(
    ecs_iter_t *it)
{
    ecs_scope_iter_t *iter = &it->iter.parent;
    ecs_map_iter_t *tables = &iter->tables;
    ecs_filter_t filter = iter->filter;
    ecs_table_record_t *tr;
    while ((tr = ecs_map_next(tables, ecs_table_record_t, NULL))) {
        ecs_table_t *table = tr->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        iter->index ++;

        ecs_data_t *data = ecs_table_get_data(table);
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

        iter->table.table = table;
        it->table = &iter->table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);

        return true;
    }

    return false;    
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

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
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!sep) {
        sep = ".";
    }    

    if (!path) {
        if (!entity) {
            entity = ecs_new_id(world);
        }

        if (parent) {
            ecs_add_pair(world, entity, EcsChildOf, entity);
        }

        return entity;
    }

    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr = path;

    parent = get_parent_from_path(world, parent, &path, prefix, entity == 0);

    ecs_entity_t cur = parent;

    while ((ptr = path_elem(ptr, buff, sep))) {
        ecs_entity_t e = ecs_lookup_child(world, cur, buff);
        if (!e) {
            char *name = ecs_os_strdup(buff);

            /* If this is the last entity in the path, use the provided id */
            if (entity && !path_elem(ptr, buff, sep)) {
                e = entity;
            }

            if (!e) {
                e = ecs_new_id(world);
            }
            
            ecs_set(world, e, EcsName, {
                .value = name,
                .alloc_value = name
            });

            ecs_os_free(name);

            if (cur) {
                ecs_add_pair(world, e, EcsChildOf, cur);
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
    if (!sep) {
        sep = ".";
    }

    return ecs_add_path_w_sep(world, 0, parent, path, sep, prefix);
}

void ecs_use(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = ecs_stage_from_world(&world);
    ecs_assert(stage->scope == 0 , ECS_INVALID_PARAMETER, NULL);
    ecs_assert(find_as_alias(world, name) == 0, ECS_ALREADY_DEFINED, NULL);
    (void)stage;
    
    ecs_alias_t *al = ecs_vector_add(&world->aliases, ecs_alias_t);
    al->name = ecs_os_strdup(name);
    al->entity = entity;
}
