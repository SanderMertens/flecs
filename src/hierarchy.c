
#include "private_api.h"

#define ECS_NAME_BUFFER_LENGTH (64)

static
bool path_append(
    const ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t cur = 0;
    char buff[22];
    const char *name;

    if (ecs_is_valid(world, child)) {
        cur = ecs_get_object(world, child, EcsChildOf, 0);
        if (cur) {
            if (cur != parent && cur != EcsFlecsCore) {
                path_append(world, parent, cur, sep, prefix, buf);
                ecs_strbuf_appendstr(buf, sep);
            }
        } else if (prefix) {
            ecs_strbuf_appendstr(buf, prefix);
        }

        name = ecs_get_name(world, child);
        if (!name) {
            ecs_os_sprintf(buff, "%u", (uint32_t)child);
            name = buff;
        }        
    } else {
        ecs_os_sprintf(buff, "%u", (uint32_t)child);
        name = buff;
    }

    ecs_strbuf_appendstr(buf, name);

    return cur != 0;
}

static
ecs_string_t get_string_key(
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_assert(!length || length == ecs_os_strlen(name), 
        ECS_INTERNAL_ERROR, NULL);

    if (!length) {
        length = ecs_os_strlen(name);
    }

    ecs_assert(!hash || hash == flecs_hash(name, length),
        ECS_INTERNAL_ERROR, NULL);

    if (!hash) {
        hash = flecs_hash(name, length);
    }

    return  (ecs_string_t) {
        .value = (char*)name,
        .length = length,
        .hash = hash
    };
}

static
ecs_entity_t find_by_name(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_string_t key = get_string_key(name, length, hash);

    ecs_entity_t *e = flecs_hashmap_get(*map, &key, ecs_entity_t);

    if (!e) {
        return 0;
    }

    return *e;
}

static
void register_by_name(
    ecs_hashmap_t *map,
    ecs_entity_t entity,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_string_t key = get_string_key(name, length, hash);
    
    ecs_entity_t existing = find_by_name(map, name, key.length, key.hash);
    if (existing) {
        if (existing != entity) {
            ecs_abort(ECS_ALREADY_DEFINED, 
                "conflicting entity registered with name '%s'", name);
        }
    } else {
        key.value = ecs_os_strdup(key.value);
    }

    flecs_hashmap_result_t hmr = flecs_hashmap_ensure(
        *map, &key, ecs_entity_t);

    *((ecs_entity_t*)hmr.value) = entity;
}

static
bool is_number(
    const char *name)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (!isdigit(name[0])) {
        return false;
    }

    ecs_size_t i, length = ecs_os_strlen(name);
    for (i = 1; i < length; i ++) {
        char ch = name[i];

        if (!isdigit(ch)) {
            break;
        }
    }

    return i >= length;
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
ecs_entity_t get_builtin(
    const char *name)
{
    if (name[0] == '.' && name[1] == '\0') {
        return EcsThis;
    } else if (name[0] == '*' && name[1] == '\0') {
        return EcsWildcard;
    }

    return 0;
}

static
ecs_entity_t find_child_in_table(
    const ecs_table_t *table,
    const char *name)
{
    /* If table doesn't have names, then don't bother */
    int32_t name_index = ecs_type_index_of(table->type, 0,
        ecs_pair(ecs_id(EcsIdentifier), EcsName));
    if (name_index == -1) {
        return 0;
    }

    const ecs_data_t *data = &table->storage;
    ecs_assert(data->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(data->entities);
    if (!count) {
        return 0;
    }

    ecs_column_t *column = &data->columns[name_index];
    EcsIdentifier *names = ecs_vector_first(column->data, EcsIdentifier);

    if (is_number(name)) {
        return name_to_id(name);
    }

    for (i = 0; i < count; i ++) {
        const char *cur_name = names[i].value;
        if (cur_name && !strcmp(cur_name, name)) {
            return *ecs_vector_get(data->entities, ecs_entity_t, i);
        }
    }

    return 0;
}

static
bool is_sep(
    const char **ptr,
    const char *sep)
{
    ecs_size_t len = ecs_os_strlen(sep);

    if (!ecs_os_strncmp(*ptr, sep, len)) {
        *ptr += len;
        return true;
    } else {
        return false;
    }
}

static
const char* path_elem(
    const char *path,
    const char *sep,
    int32_t *len)
{
    const char *ptr;
    char ch;
    int32_t template_nesting = 0;
    int32_t count = 0;

    for (ptr = path; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            template_nesting ++;
        } else if (ch == '>') {
            template_nesting --;
        }

        ecs_assert(template_nesting >= 0, ECS_INVALID_PARAMETER, path);

        if (!template_nesting && is_sep(&ptr, sep)) {
            break;
        }

        count ++;
    }

    if (len) {
        *len = count;
    }

    if (count) {
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

static
void on_set_symbol(ecs_iter_t *it) {
    EcsIdentifier *n = ecs_term(it, EcsIdentifier, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        register_by_name(
            &world->symbols, e, n[i].value, n[i].length, n[i].hash);
    }
}

static
uint64_t string_hash(
    const void *ptr)
{
    const ecs_string_t *str = ptr;
    ecs_assert(str->hash != 0, ECS_INVALID_PARAMETER, NULL);
    return str->hash;
}

static
int string_compare(
    const void *ptr1, 
    const void *ptr2)
{
    const ecs_string_t *str1 = ptr1;
    const ecs_string_t *str2 = ptr2;
    ecs_size_t len1 = str1->length;
    ecs_size_t len2 = str2->length;
    if (len1 != len2) {
        return (len1 > len2) - (len1 < len2);
    }

    return ecs_os_memcmp(str1->value, str2->value, len1);
}

ecs_hashmap_t flecs_string_hashmap_new(void) {
    return flecs_hashmap_new(ecs_string_t, ecs_entity_t, 
        string_hash, 
        string_compare);
}

void flecs_bootstrap_hierarchy(ecs_world_t *world) {
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(ecs_id(EcsIdentifier), EcsSymbol)},
        .callback = on_set_symbol,
        .events = {EcsOnSet}
    });
}


/* Public functions */

char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
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
        path_append(world, parent, child, sep, prefix, &buf);
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

    ecs_id_record_t *r = flecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
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

    ecs_entity_t e = get_builtin(name);
    if (e) {
        return e;
    }

    if (is_number(name)) {
        return name_to_id(name);
    }

    e = find_by_name(&world->aliases, name, 0, 0);
    if (e) {
        return e;
    }    
    
    return ecs_lookup_child(world, 0, name);
}

ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name,
    bool lookup_as_path)
{   
    if (!name) {
        return 0;
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = find_by_name(&world->symbols, name, 0, 0);
    if (e) {
        return e;
    }

    if (lookup_as_path) {
        return ecs_lookup_fullpath(world, name);
    }

    return 0;
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

    ecs_entity_t e = get_builtin(path);
    if (e) {
        return e;
    }

    e = find_by_name(&world->aliases, path, 0, 0);
    if (e) {
        return e;
    }      

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr, *ptr_start;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;
    ecs_entity_t cur;
    bool core_searched = false;

    if (!sep) {
        sep = ".";
    }

    parent = get_parent_from_path(world, parent, &path, prefix, true);

retry:
    cur = parent;
    ptr_start = ptr = path;

    while ((ptr = path_elem(ptr, sep, &len))) {
        if (len < size) {
            ecs_os_memcpy(elem, ptr_start, len);
        } else {
            if (size == ECS_NAME_BUFFER_LENGTH) {
                elem = NULL;
            }

            elem = ecs_os_realloc(elem, len + 1);
            ecs_os_memcpy(elem, ptr_start, len);
            size = len + 1;
        }

        elem[len] = '\0';
        ptr_start = ptr;

        cur = ecs_lookup_child(world, cur, elem);
        if (!cur) {
            goto tail;
        }
    }

tail:
    if (!cur && recursive) {
        if (!core_searched) {
            if (parent) {
                parent = ecs_get_object(world, parent, EcsChildOf, 0);
            } else {
                parent = EcsFlecsCore;
                core_searched = true;
            }
            goto retry;
        }
    }

    if (elem != buff) {
        ecs_os_free(elem);
    }

    return cur;
}

ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    if (scope) {
        stage->scope_table = flecs_table_traverse_add(
            world, &world->store.root, ecs_pair(EcsChildOf, scope), NULL);
    } else {
        stage->scope_table = &world->store.root;
    }

    return cur;
}

ecs_entity_t ecs_get_scope(
    const ecs_world_t *world)
{
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->scope;
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

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr = path;
    const char *ptr_start = path;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;

    parent = get_parent_from_path(world, parent, &path, prefix, entity == 0);

    ecs_entity_t cur = parent;

    char *name = NULL;

    while ((ptr = path_elem(ptr, sep, &len))) {
        if (len < size) {
            ecs_os_memcpy(elem, ptr_start, len);
        } else {
            if (size == ECS_NAME_BUFFER_LENGTH) {
                elem = NULL;
            }

            elem = ecs_os_realloc(elem, len + 1);
            ecs_os_memcpy(elem, ptr_start, len);
            size = len + 1;          
        }

        elem[len] = '\0';
        ptr_start = ptr;

        ecs_entity_t e = ecs_lookup_child(world, cur, elem);
        if (!e) {
            if (name) {
                ecs_os_free(name);
            }

            name = ecs_os_strdup(elem);

            /* If this is the last entity in the path, use the provided id */
            if (entity && !path_elem(ptr, sep, NULL)) {
                e = entity;
            }

            if (!e) {
                e = ecs_new_id(world);
            }

            ecs_set_name(world, e, name);

            if (cur) {
                ecs_add_pair(world, e, EcsChildOf, cur);
            }
        }

        cur = e;
    }

    if (entity && (cur != entity)) {
        if (name) {
            ecs_os_free(name);
        }

        name = ecs_os_strdup(elem);

        ecs_set_name(world, entity, name);
    }

    if (name) {
        ecs_os_free(name);
    }

    if (elem != buff) {
        ecs_os_free(elem);
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
    register_by_name(&world->aliases, entity, name, 0, 0);
}
