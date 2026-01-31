/**
 * @file entity_name.c
 * @brief Functions for working with named entities.
 */

#include "private_api.h"

#define ECS_NAME_BUFFER_LENGTH (64)

static
bool flecs_path_append(
    const ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf,
    bool escape)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(sep[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t cur = 0;
    const char *name = NULL;
    ecs_size_t name_len = 0;

    if (child && ecs_is_alive(world, child)) {
        ecs_record_t *r = flecs_entities_get(world, child);
        ecs_assert(r != NULL, ECS_INVALID_OPERATION, NULL);
        ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
        bool hasName = r->table->flags & EcsTableHasName;

        if (hasName) {
            cur = ecs_get_target(world, child, EcsChildOf, 0);
            if (cur) {
                ecs_assert(cur != child, ECS_CYCLE_DETECTED, NULL);
                if (cur != parent && (cur != EcsFlecsCore || prefix != NULL)) {
                    flecs_path_append(world, parent, cur, sep, prefix, buf, escape);
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
    }

    if (name) {
        /* Check if we need to escape separator character */
        const char *sep_in_name = NULL;
        if (!sep[1]) {
            sep_in_name = strchr(name, sep[0]);
        }

        if (sep_in_name || escape) {
            const char *name_ptr;
            char ch;
            for (name_ptr = name; (ch = name_ptr[0]); name_ptr ++) {
                char esc[3];
                if (ch != sep[0]) {
                    if (escape) {
                        flecs_chresc(esc, ch, '\"');
                        ecs_strbuf_appendch(buf, esc[0]);
                        if (esc[1]) {
                            ecs_strbuf_appendch(buf, esc[1]);
                        }
                    } else {
                        ecs_strbuf_appendch(buf, ch);
                    }
                } else {
                    if (!escape) {
                        ecs_strbuf_appendch(buf, '\\');
                        ecs_strbuf_appendch(buf, sep[0]);
                    } else {
                        ecs_strbuf_appendlit(buf, "\\\\");
                        flecs_chresc(esc, ch, '\"');
                        ecs_strbuf_appendch(buf, esc[0]);
                        if (esc[1]) {
                            ecs_strbuf_appendch(buf, esc[1]);
                        }
                    }
                }
            }
        } else {
            ecs_strbuf_appendstrn(buf, name, name_len);
        }
    } else {
        ecs_strbuf_appendch(buf, '#');
        ecs_strbuf_appendint(buf, flecs_uto(int64_t, (uint32_t)child));
    }

    return cur != 0;
}

bool flecs_name_is_id(
    const char *name)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    if (name[0] == '#') {
        /* If name is not just digits it's not an id */
        const char *ptr;
        char ch;
        for (ptr = name + 1; (ch = ptr[0]); ptr ++) {
            if (!isdigit(ch)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

ecs_entity_t flecs_name_to_id(
    const char *name)
{
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name[0] == '#', ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t res = flecs_ito(uint64_t, atoll(name + 1));
    if (res >= UINT32_MAX) {
        return 0; /* Invalid id */
    }
    return res;
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
    } else if (name[0] == '$' && name[1] == '\0') {
        return EcsVariable;
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
    char **buffer_out,
    ecs_size_t *size_out)
{
    char *buffer = NULL;
    if (buffer_out) {
        buffer = *buffer_out;
    }

    const char *ptr;
    char ch;
    int32_t template_nesting = 0;
    int32_t pos = 0;
    ecs_size_t size = size_out ? *size_out : 0;

    for (ptr = path; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            template_nesting ++;
        } else if (ch == '>') {
            template_nesting --;
        } else if (ch == '\\') {
            ptr ++;
            if (buffer) {
                buffer[pos] = ptr[0];
            }
            pos ++;
            continue;
        }

        ecs_check(template_nesting >= 0, ECS_INVALID_PARAMETER, path);

        if (!template_nesting && flecs_is_sep(&ptr, sep)) {
            break;
        }

        if (buffer) {
            if (pos >= (size - 1)) {
                if (size == ECS_NAME_BUFFER_LENGTH) { /* stack buffer */
                    char *new_buffer = ecs_os_malloc(size * 2 + 1);
                    ecs_os_memcpy(new_buffer, buffer, size);
                    buffer = new_buffer;
                } else { /* heap buffer */
                    buffer = ecs_os_realloc(buffer, size * 2 + 1);
                }
                size *= 2;
            }

            buffer[pos] = ch;
        }

        pos ++;
    }

    if (buffer) {
        buffer[pos] = '\0';
        *buffer_out = buffer;
        *size_out = size;
    }

    if (pos || ptr[0]) {
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
    const char *sep,
    const char *prefix,
    bool new_entity,
    bool *error)
{
    ecs_assert(error != NULL, ECS_INTERNAL_ERROR, NULL);

    bool start_from_root = false;
    const char *path = *path_ptr;

    if (flecs_is_root_path(path, prefix)) {
        path += ecs_os_strlen(prefix);
        parent = 0;
        start_from_root = true;
    }

    if (path[0] == '#') {
        parent = flecs_name_to_id(path);
        if (!parent && ecs_os_strncmp(path, "#0", 2)) {
            *error = true;
            return 0;
        }

        path ++;
        while (path[0] && isdigit(path[0])) {
            path ++; /* Skip id part of path */
        }

        /* Skip next separator so that the returned path points to the next
         * name element. */
        ecs_size_t sep_len = ecs_os_strlen(sep);
        if (!ecs_os_strncmp(path, sep, ecs_os_strlen(sep))) {
            path += sep_len;
        }

        start_from_root = true;
    }
    
    if (!start_from_root && !parent && new_entity) {
        parent = ecs_get_scope(world);
    }

    *path_ptr = path;

    return parent;
}

static
void flecs_on_set_symbol(
    ecs_iter_t *it) 
{
    EcsIdentifier *n = ecs_field(it, EcsIdentifier, 0);
    ecs_world_t *world = it->real_world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        flecs_name_index_ensure(
            &world->symbols, e, n[i].value, n[i].length, n[i].hash);
    }
}

void flecs_bootstrap_entity_name(
    ecs_world_t *world) 
{
    ecs_observer(world, {
        .query.terms[0] = {
            .id = ecs_pair(ecs_id(EcsIdentifier), EcsSymbol)
        },
        .callback = flecs_on_set_symbol,
        .events = {EcsOnSet},
        .yield_existing = true,
        .global_observer = true
    });
}

void ecs_on_set(EcsIdentifier)(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->real_world;
    EcsIdentifier *ptr = ecs_field(it, EcsIdentifier, 0);

    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t evt = it->event;
    ecs_id_t evt_id = it->event_id;
    ecs_entity_t kind = ECS_PAIR_SECOND(evt_id); /* Name, Symbol, Alias */
    ecs_id_t pair = ecs_childof(0);
    ecs_hashmap_t *index = NULL;
    bool has_parent = false;
    EcsParent *parents = NULL;

    if (kind == EcsSymbol) {
        index = &world->symbols;
    } else if (kind == EcsAlias) {
        index = &world->aliases;
    } else if (kind == EcsName) {
        ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (it->table->flags & EcsTableHasParent) {
            has_parent = true;
            parents = ecs_table_get(world, it->table, EcsParent, it->offset);
        } else {
            ecs_search(world, it->table, ecs_childof(EcsWildcard), &pair);
            ecs_assert(pair != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_component_record_t *cr = flecs_components_get(world, pair);
            if (evt == EcsOnSet) {
                index = flecs_component_name_index_ensure(world, cr);
            } else {
                index = flecs_component_name_index_get(world, cr);
            }
        }
    }

    int i, count = it->count;

    for (i = 0; i < count; i ++) {
        EcsIdentifier *cur = &ptr[i];
        uint64_t hash;
        ecs_size_t len;
        const char *name = cur->value;

        if (kind == EcsName) {
            ecs_assert((world->flags & (EcsWorldInit|EcsWorldFini)) || 
                !(cur->index) ||
                !(it->table->flags & EcsTableHasBuiltins), 
                    ECS_INVALID_OPERATION, 
                    "cannot rename builtin entity to '%s'", name);
            ecs_assert((world->flags & (EcsWorldInit|EcsWorldFini)) ||
                (it->entities[i] != EcsFlecs),
                ECS_INVALID_OPERATION,
                    "cannot rename flecs root module");
            ecs_assert((world->flags & (EcsWorldInit|EcsWorldFini)) ||
                (it->entities[i] != EcsFlecsCore),
                ECS_INVALID_OPERATION,
                    "cannot rename flecs.core module");

            if (has_parent) {
                ecs_component_record_t *cr = flecs_components_get(
                    world, ecs_childof(parents[i].value));
                ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
                index = flecs_component_name_index_ensure(world, cr);
            }
        }

        if (cur->index && cur->index != index) {
            /* If index doesn't match up, the value must have been copied from
             * another entity, so reset index & cached index hash */
            cur->index = NULL;
            cur->index_hash = 0;
        }

        if (cur->value && (evt == EcsOnSet)) {
            len = cur->length = ecs_os_strlen(name);
            hash = cur->hash = flecs_hash(name, len);
        } else {
            len = cur->length = 0;
            hash = cur->hash = 0;
            cur->index = NULL;
        }

        if (index) {
            uint64_t index_hash = cur->index_hash;
            ecs_entity_t e = it->entities[i];

            if (hash != index_hash) {
                if (index_hash) {
                    flecs_name_index_remove(index, e, index_hash);
                }
                if (hash) {
                    flecs_name_index_ensure(index, e, name, len, hash);
                    cur->index_hash = hash;
                    cur->index = index;
                }
            } else {
                /* Name didn't change, but the string could have been 
                 * reallocated. Make sure name index points to correct string */
                flecs_name_index_update_name(index, e, hash, name);
            }
        }
    }
}

static
void flecs_reparent_name_index_intern(
    const ecs_entity_t *entities,
    ecs_hashmap_t *src_index,
    ecs_hashmap_t *dst_index,
    EcsIdentifier *names,
    int32_t count) 
{
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        EcsIdentifier *name = &names[i];

        ecs_assert(e != EcsFlecs, ECS_INVALID_OPERATION,
            "cannot reparent flecs root module");
        ecs_assert(e != EcsFlecsCore, ECS_INVALID_OPERATION,
            "cannot reparent flecs.core module");

        uint64_t index_hash = name->index_hash;
        if (index_hash) {
            flecs_name_index_remove(src_index, e, index_hash);
        }
    
        if (dst_index) {
            const char *name_str = name->value;
            if (name_str) {
                if (name->hash == 0) {
                    name->length = ecs_os_strlen(name_str);
                    name->hash = flecs_hash(name_str, name->length);
                }

                ecs_assert(name->hash != 0, ECS_INTERNAL_ERROR, NULL);
        
                flecs_name_index_ensure(
                    dst_index, e, name_str, name->length, name->hash);
                name->index = dst_index;
            }
        }
    }
}

void flecs_reparent_name_index(
    ecs_world_t *world,
    ecs_table_t *dst,
    ecs_table_t *src, 
    int32_t offset,
    int32_t count) 
{
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!(dst->flags & EcsTableHasName)) {
        /* If destination table doesn't have a name, we don't need to update the
         * name index. Even if the src table had a name, the on_remove hook for
         * EcsIdentifier will remove the entity from the index. */
        return;
    }

    if (!src) {
        src = &world->store.root;
    }

    ecs_hashmap_t *src_index = flecs_table_get_name_index(world, src);
    ecs_hashmap_t *dst_index = flecs_table_get_name_index(world, dst);
    ecs_assert(src_index != dst_index, ECS_INTERNAL_ERROR, NULL);
    if ((!src_index && !dst_index)) {
        return;
    }

    EcsIdentifier *names = ecs_table_get_pair(world, 
        dst, EcsIdentifier, EcsName, offset);
    ecs_assert(names != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_reparent_name_index_intern(&ecs_table_entities(dst)[offset],
        src_index, dst_index, names, count);

}

void flecs_unparent_name_index(
    ecs_world_t *world,
    ecs_table_t *src,
    ecs_table_t *dst,
    int32_t offset,
    int32_t count) 
{
    if (!(src->flags & EcsTableHasName)) {
        return;
    }

    if (!dst || !(dst->flags & EcsTableHasName)) {
        /* If destination table doesn't have a name, we don't need to update the
         * name index. Even if the src table had a name, the on_remove hook for
         * EcsIdentifier will remove the entity from the index. */
        return;
    }

    ecs_hashmap_t *src_index = flecs_table_get_name_index(world, src);
    ecs_hashmap_t *dst_index = dst ? flecs_table_get_name_index(world, dst) : NULL;
    ecs_assert(src_index != NULL, ECS_INTERNAL_ERROR, NULL);

    EcsIdentifier *names = ecs_table_get_pair(world, 
        src, EcsIdentifier, EcsName, offset);
    ecs_assert(names != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_reparent_name_index_intern(&ecs_table_entities(src)[offset],
        src_index, dst_index, names, count);
}

/* Public functions */

void ecs_get_path_w_sep_buf(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf,
    bool escape)
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
        flecs_path_append(world, parent, child, sep, prefix, buf, escape);
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
    ecs_get_path_w_sep_buf(world, parent, child, sep, prefix, &buf, false);
    return ecs_strbuf_get(&buf);
}

ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    if (flecs_name_is_id(name)) {
        ecs_entity_t result = flecs_name_to_id(name);
        if (result && ecs_is_alive(world, result)) {
            if (parent && !ecs_has_pair(world, result, EcsChildOf, parent)) {
                return 0;
            }
            return result;
        }
    }

    ecs_id_t pair = ecs_childof(parent);
    ecs_component_record_t *cr = flecs_components_get(world, pair);
    ecs_hashmap_t *index = NULL;
    if (cr) {
        index = flecs_component_name_index_get(world, cr);
    }
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
    const char *path)
{   
    return ecs_lookup_path_w_sep(world, 0, path, ".", NULL, true);
}

ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name,
    bool lookup_as_path,
    bool recursive)
{   
    if (!name) {
        return 0;
    }

    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = 0;
    if (lookup_as_path) {
        e = ecs_lookup_path_w_sep(world, 0, name, ".", NULL, recursive);
    }

    if (!e) {
        e = flecs_name_index_find(&world->symbols, name, 0, 0);
    }

    return e;
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
    ecs_check(!parent || ecs_is_valid(world, parent), 
        ECS_INVALID_PARAMETER, NULL);
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

    char buff[ECS_NAME_BUFFER_LENGTH], *elem = buff;
    const char *ptr;
    int32_t size = ECS_NAME_BUFFER_LENGTH;
    ecs_entity_t cur;
    bool lookup_path_search = false;

    const ecs_entity_t *lookup_path = ecs_get_lookup_path(stage);
    const ecs_entity_t *lookup_path_cur = lookup_path;
    while (lookup_path_cur && *lookup_path_cur) {
        lookup_path_cur ++;
    }

    if (!sep) {
        sep = ".";
    }

    bool error = false;
    parent = flecs_get_parent_from_path(
        stage, parent, &path, sep, prefix, true, &error);
    if (error) {
        return 0;
    }

    if (parent && !(parent = ecs_get_alive(world, parent))) {
        return 0;
    }

    if (!path[0]) {
        return parent;
    }

    if (!sep[0]) {
        return ecs_lookup_child(world, parent, path);
    }

retry:
    cur = parent;
    ptr = path;

    while ((ptr = flecs_path_elem(ptr, sep, &elem, &size))) {
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

    /* Safe: application owns lookup path */
    ecs_entity_t *cur = ECS_CONST_CAST(ecs_entity_t*, stage->lookup_path);
    stage->lookup_path = lookup_path;

    return cur;
error:
    return NULL;
}

ecs_entity_t* ecs_get_lookup_path(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    /* Safe: application owns lookup path */
    return ECS_CONST_CAST(ecs_entity_t*, stage->lookup_path);
error:
    return NULL;
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    flecs_poly_assert(world, ecs_world_t);
    const char *old_prefix = world->info.name_prefix;
    world->info.name_prefix = prefix;
    return old_prefix;
}

static
void flecs_add_path(
    ecs_world_t *world,
    bool defer_suspend,
    ecs_entity_t parent,
    ecs_entity_t entity,
    const char *name)
{
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (defer_suspend) {
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (parent) {
        ecs_add_pair(world, entity, EcsChildOf, parent);
    }

    ecs_assert(name[0] != '#', ECS_INVALID_PARAMETER, 
        "path should not contain identifier with #");

    ecs_set_name(world, entity, name);

    if (defer_suspend) {
        ecs_stage_t *stage = flecs_stage_from_world(&world);
        flecs_resume_readonly(real_world, &srs);
        flecs_defer_path(stage, parent, entity, name);
    }
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
    const ecs_world_t *real_world = world;
    if (flecs_poly_is(world, ecs_stage_t)) {
        real_world = ecs_get_world(world);
    }

    if (!sep) {
        sep = ".";
    }

    if (!path) {
        if (!entity) {
            entity = ecs_new(world);
        }

        if (parent) {
            ecs_add_pair(world, entity, EcsChildOf, entity);
        }

        return entity;
    }

    bool root_path = flecs_is_root_path(path, prefix);
    bool error = false;
    parent = flecs_get_parent_from_path(
        world, parent, &path, sep, prefix, !entity, &error);
    if (error) {
        /* Invalid id */
        ecs_err("invalid identifier: '%s'", path);
        return 0;
    }

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr = path;
    char *elem = buff;
    int32_t size = ECS_NAME_BUFFER_LENGTH;
    
    /* If we're in deferred/readonly mode suspend it, so that the name index is
     * immediately updated. Without this, we could create multiple entities for
     * the same name in a single command queue. */
    bool suspend_defer = ecs_is_deferred(world) &&
        !(real_world->flags & EcsWorldMultiThreaded);
        
    ecs_entity_t cur = parent;
    char *name = NULL;

    if (sep[0]) {
        while ((ptr = flecs_path_elem(ptr, sep, &elem, &size))) {
            ecs_entity_t e = ecs_lookup_child(world, cur, elem);
            if (!e) {
                if (name) {
                    ecs_os_free(name);
                }

                name = ecs_os_strdup(elem);

                /* If this is the last entity in the path, use the provided id */
                bool last_elem = false;
                if (!flecs_path_elem(ptr, sep, NULL, NULL)) {
                    e = entity;
                    last_elem = true;
                }

                if (!e) {
                    if (last_elem) {
                        ecs_entity_t prev = ecs_set_scope(world, 0);
                        e = ecs_entity(world, {0});
                        ecs_set_scope(world, prev);
                    } else {
                        e = ecs_new(world);
                    }
                }

                if (!cur && last_elem && root_path) {
                    ecs_remove_pair(world, e, EcsChildOf, EcsWildcard);
                }

                flecs_add_path(world, suspend_defer, cur, e, name);
            }

            cur = e;
        }

        if (entity && (cur != entity)) {
            ecs_throw(ECS_ALREADY_DEFINED, "cannot assign name '%s' to "
                "entity %u, name already used by entity '%s'", path, 
                    (uint32_t)cur, flecs_errstr(ecs_get_path(world, entity)));
        }

        if (name) {
            ecs_os_free(name);
        }

        if (elem != buff) {
            ecs_os_free(elem);
        }
    } else {
        flecs_add_path(world, suspend_defer, parent, entity, path);
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
    return ecs_add_path_w_sep(world, 0, parent, path, sep, prefix);
}

static
const char* flecs_get_identifier(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    const EcsIdentifier *ptr = ecs_get_pair(
        world, entity, EcsIdentifier, tag);

    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
error:
    return NULL;
}

const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return flecs_get_identifier(world, entity, EcsName);
}

const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ecs_get_world(world);
    if (ecs_owns_pair(world, entity, ecs_id(EcsIdentifier), EcsSymbol)) {
        return flecs_get_identifier(world, entity, EcsSymbol);
    } else {
        return NULL;
    }
}

ecs_entity_t flecs_set_identifier(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t tag,
    const char *name)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0 || name != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        entity = ecs_new(world);
    }

    if (!name) {
        ecs_remove_pair(world, entity, ecs_id(EcsIdentifier), tag);
        return entity;
    }

    EcsIdentifier *ptr = ecs_ensure_pair(world, entity, EcsIdentifier, tag);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (tag == EcsName) {
        /* Insert command after ensure, but before the name is potentially 
         * freed. Even though the name is a const char*, it is possible that the
         * application passed in the existing name of the entity which could 
         * still cause it to be freed. */
        flecs_defer_path(stage, 0, entity, name);
    }

    char *old = ptr->value;
    ptr->value = ecs_os_strdup(name);

    ecs_modified_pair(world, entity, ecs_id(EcsIdentifier), tag);

    /* Free old name after updating name index in on_set handler. */
    ecs_os_free(old);

    return entity;
error:
    return 0;
}

ecs_entity_t ecs_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    if (!entity) {
        return ecs_entity(world, {
            .name = name
        });
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    flecs_set_identifier(world, stage, entity, EcsName, name);

    return entity;
}

ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    return flecs_set_identifier(world, NULL, entity, EcsSymbol, name);
}

void ecs_set_alias(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    flecs_set_identifier(world, NULL, entity, EcsAlias, name);
}
