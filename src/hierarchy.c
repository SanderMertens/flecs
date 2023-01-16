/**
 * @file hierarchy.c
 * @brief API for entity paths and name lookups.
 */

#include "private_api.h"
#include <ctype.h>

#define ECS_NAME_BUFFER_LENGTH (64)

static
bool flecs_path_append(
    const ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(sep[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t cur = 0;
    const char *name = NULL;
    ecs_size_t name_len = 0;

    if (child && ecs_is_alive(world, child)) {
        cur = ecs_get_target(world, child, EcsChildOf, 0);
        if (cur) {
            ecs_assert(cur != child, ECS_CYCLE_DETECTED, NULL);
            if (cur != parent && (cur != EcsFlecsCore || prefix != NULL)) {
                flecs_path_append(world, parent, cur, sep, prefix, buf);
                if (!sep[1]) {
                    ecs_strbuf_appendch(buf, sep[0]);
                } else {
                    ecs_strbuf_appendstr(buf, sep);
                }
            }
        } else if (prefix && prefix[0]) {
            if (!prefix[1]) {
                ecs_strbuf_appendch(buf, prefix[0]);
            } else {
                ecs_strbuf_appendstr(buf, prefix);
            }
        }

        const EcsIdentifier *id = ecs_get_pair(
            world, child, EcsIdentifier, EcsName);
        if (id) {
            name = id->value;
            name_len = id->length;
        }      
    }

    if (name) {
        ecs_strbuf_appendstrn(buf, name, name_len);
    } else {
        ecs_strbuf_appendint(buf, flecs_uto(int64_t, (uint32_t)child));
    }

    return cur != 0;
}

static
bool flecs_is_string_number(
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
ecs_entity_t flecs_name_to_id(
    const ecs_world_t *world,
    const char *name)
{
    int64_t result = atoll(name);
    ecs_assert(result >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t alive = ecs_get_alive(world, (ecs_entity_t)result);
    if (alive) {
        return alive;
    } else {
        if ((uint32_t)result == (uint64_t)result) {
            return (ecs_entity_t)result;
        } else {
            return 0;
        }
    }
}

static
ecs_entity_t flecs_get_builtin(
    const char *name)
{
    if (name[0] == '.' && name[1] == '\0') {
        return EcsThis;
    } else if (name[0] == '*' && name[1] == '\0') {
        return EcsWildcard;
    } else if (name[0] == '_' && name[1] == '\0') {
        return EcsAny;
    }

    return 0;
}

static
bool flecs_is_sep(
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
const char* flecs_path_elem(
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

        ecs_check(template_nesting >= 0, ECS_INVALID_PARAMETER, path);

        if (!template_nesting && flecs_is_sep(&ptr, sep)) {
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
error:
    return NULL;
}

static
bool flecs_is_root_path(
    const char *path,
    const char *prefix)
{
    if (prefix) {
        return !ecs_os_strncmp(path, prefix, ecs_os_strlen(prefix));
    } else {
        return false;
    }
}

static
ecs_entity_t flecs_get_parent_from_path(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char **path_ptr,
    const char *prefix,
    bool new_entity)
{
    bool start_from_root = false;
    const char *path = *path_ptr;

    if (flecs_is_root_path(path, prefix)) {
        path += ecs_os_strlen(prefix);
        parent = 0;
        start_from_root = true;
    }

    if (!start_from_root && !parent && new_entity) {
        parent = ecs_get_scope(world);
    }

    *path_ptr = path;

    return parent;
}

static
void flecs_on_set_symbol(ecs_iter_t *it) {
    EcsIdentifier *n = ecs_field(it, EcsIdentifier, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        flecs_name_index_ensure(
            &world->symbols, e, n[i].value, n[i].length, n[i].hash);
    }
}

void flecs_bootstrap_hierarchy(ecs_world_t *world) {
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_childof(EcsFlecsInternals)}}),
        .filter.terms[0] = {
            .id = ecs_pair(ecs_id(EcsIdentifier), EcsSymbol), 
            .src.flags = EcsSelf 
        },
        .callback = flecs_on_set_symbol,
        .events = {EcsOnSet},
        .yield_existing = true
    });
}


/* Public functions */

void ecs_get_path_w_sep_buf(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(buf != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    if (child == EcsWildcard) {
        ecs_strbuf_appendch(buf, '*');
        return;
    }
    if (child == EcsAny) {
        ecs_strbuf_appendch(buf, '_');
        return;
    }

    if (!sep) {
        sep = ".";
    }

    if (!child || parent != child) {
        flecs_path_append(world, parent, child, sep, prefix, buf);
    } else {
        ecs_strbuf_appendstrn(buf, "", 0);
    }

error:
    return;
}

char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_get_path_w_sep_buf(world, parent, child, sep, prefix, &buf);
    return ecs_strbuf_get(&buf);
}

ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    if (flecs_is_string_number(name)) {
        ecs_entity_t result = flecs_name_to_id(world, name);
        if (result && ecs_is_alive(world, result)) {
            if (parent && !ecs_has_pair(world, result, EcsChildOf, parent)) {
                return 0;
            }
            return result;
        }
    }

    ecs_id_t pair = ecs_childof(parent);
    ecs_hashmap_t *index = flecs_id_name_index_get(world, pair);
    if (index) {
        return flecs_name_index_find(index, name, 0, 0);
    } else {
        return 0;
    }
error:
    return 0;
}

ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = flecs_get_builtin(name);
    if (e) {
        return e;
    }

    if (flecs_is_string_number(name)) {
        return flecs_name_to_id(world, name);
    }

    e = flecs_name_index_find(&world->aliases, name, 0, 0);
    if (e) {
        return e;
    }    
    
    return ecs_lookup_child(world, 0, name);
error:
    return 0;
}

ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name,
    bool lookup_as_path)
{   
    if (!name) {
        return 0;
    }

    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = flecs_name_index_find(&world->symbols, name, 0, 0);
    if (e) {
        return e;
    }

    if (lookup_as_path) {
        return ecs_lookup_fullpath(world, name);
    }

error:
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

    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_world_t *stage = world;
    world = ecs_get_world(world);

    ecs_entity_t e = flecs_get_builtin(path);
    if (e) {
        return e;
    }

    e = flecs_name_index_find(&world->aliases, path, 0, 0);
    if (e) {
        return e;
    }

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr, *ptr_start;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;
    ecs_entity_t cur;
    bool lookup_path_search = false;

    ecs_entity_t *lookup_path = ecs_get_lookup_path(stage);
    ecs_entity_t *lookup_path_cur = lookup_path;
    while (lookup_path_cur && *lookup_path_cur) {
        lookup_path_cur ++;
    }

    if (!sep) {
        sep = ".";
    }

    parent = flecs_get_parent_from_path(stage, parent, &path, prefix, true);

    if (!sep[0]) {
        return ecs_lookup_child(world, parent, path);
    }

retry:
    cur = parent;
    ptr_start = ptr = path;

    while ((ptr = flecs_path_elem(ptr, sep, &len))) {
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
        if (!lookup_path_search) {
            if (parent) {
                parent = ecs_get_target(world, parent, EcsChildOf, 0);
                goto retry;
            } else {
                lookup_path_search = true;
            }
        }

        if (lookup_path_search) {
            if (lookup_path_cur != lookup_path) {
                lookup_path_cur --;
                parent = lookup_path_cur[0];
                goto retry;
            }
        }
    }

    if (elem != buff) {
        ecs_os_free(elem);
    }

    return cur;
error:
    return 0;
}

ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    return cur;
error:
    return 0;
}

ecs_entity_t ecs_get_scope(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->scope;
error:
    return 0;
}

ecs_entity_t* ecs_set_lookup_path(
    ecs_world_t *world,
    const ecs_entity_t *lookup_path)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_entity_t *cur = stage->lookup_path;
    stage->lookup_path = (ecs_entity_t*)lookup_path;

    return cur;
error:
    return NULL;
}

ecs_entity_t* ecs_get_lookup_path(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->lookup_path;
error:
    return NULL;
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    ecs_poly_assert(world, ecs_world_t);
    const char *old_prefix = world->info.name_prefix;
    world->info.name_prefix = prefix;
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
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

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

    bool root_path = flecs_is_root_path(path, prefix);
    parent = flecs_get_parent_from_path(world, parent, &path, prefix, !entity);

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr = path;
    const char *ptr_start = path;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;

    ecs_entity_t cur = parent;

    char *name = NULL;

    if (sep[0]) {
        while ((ptr = flecs_path_elem(ptr, sep, &len))) {
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
                bool last_elem = false;
                if (!flecs_path_elem(ptr, sep, NULL)) {
                    e = entity;
                    last_elem = true;
                }

                if (!e) {
                    if (last_elem) {
                        ecs_entity_t prev = ecs_set_scope(world, 0);
                        e = ecs_new(world, 0);
                        ecs_set_scope(world, prev);
                    } else {
                        e = ecs_new_id(world);
                    }
                }

                if (cur) {
                    ecs_add_pair(world, e, EcsChildOf, cur);
                } else if (last_elem && root_path) {
                    ecs_remove_pair(world, e, EcsChildOf, EcsWildcard);
                }

                ecs_set_name(world, e, name);
            }

            cur = e;
        }

        if (entity && (cur != entity)) {
            ecs_throw(ECS_ALREADY_DEFINED, name);
        }

        if (name) {
            ecs_os_free(name);
        }

        if (elem != buff) {
            ecs_os_free(elem);
        }
    } else {
        if (parent) {
            ecs_add_pair(world, entity, EcsChildOf, parent);
        }
        ecs_set_name(world, entity, path);
    }

    return cur;
error:
    return 0;
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
