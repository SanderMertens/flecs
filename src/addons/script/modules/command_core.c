/**
 * @file addons/script/modules/command_core.c
 * @brief Commands for inspecting and modifying entities.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_SHELL
#include "command.h"

typedef struct {
    const char *name;
    ecs_entity_t entity;
} flecs_command_sort_elem_t;

static int flecs_command_sort_compare(
    const void *a,
    const void *b)
{
    return ecs_os_strcmp(((const flecs_command_sort_elem_t*)a)->name,
        ((const flecs_command_sort_elem_t*)b)->name);
}

static bool flecs_command_match(
    const char *str,
    const char *text,
    bool case_insensitive)
{
    if (!str) {
        return false;
    }

    return case_insensitive ? flecs_strstr_i(str, text) : !!strstr(str, text);
}

static ecs_entity_t flecs_command_scope_arg(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    int32_t index,
    bool *valid)
{
    *valid = true;
    if (argc <= index) {
        return flecs_command_scope(ctx->world);
    }

    ecs_entity_t scope = flecs_command_arg(argv, index, ecs_entity_t);
    if (scope && !flecs_command_check_alive(ctx, scope)) {
        *valid = false;
    }

    return scope;
}

static void flecs_command_help(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    const char *filter = argc ? flecs_command_arg(argv, 0, char*) : NULL;
    ecs_vec_t *out = result->ptr;

    ecs_vec_t children;
    ecs_vec_init_t(NULL, &children, ecs_entity_t, 0);
    flecs_command_children(world, ecs_get_parent(world, ctx->function),
        &children);

    int32_t i, count = 0, child_count = ecs_vec_count(&children);
    ecs_entity_t *entities = ecs_vec_first(&children);
    flecs_command_sort_elem_t *elems = child_count ?
        ecs_os_malloc_n(flecs_command_sort_elem_t, child_count) : NULL;
    for (i = 0; i < child_count; i ++) {
        const char *name = ecs_get_name(world, entities[i]);
        if (!name || !ecs_has(world, entities[i], EcsScriptFunction)) {
            continue;
        }

        if (filter && ecs_os_strcmp(name, filter)) {
            continue;
        }

        elems[count ++] = (flecs_command_sort_elem_t){ name, entities[i] };
    }

    if (count) {
        qsort(elems, (size_t)count, sizeof(flecs_command_sort_elem_t),
            flecs_command_sort_compare);
    }

    for (i = 0; i < count; i ++) {
        const EcsScriptFunction *f = ecs_get(
            world, elems[i].entity, EcsScriptFunction);
        ecs_command_info_t *info = ecs_vec_append_t(
            NULL, out, ecs_command_info_t);
        info->name = ecs_os_strdup(elems[i].name);
        info->brief = ecs_os_strdup(ecs_doc_get_brief(world, elems[i].entity));
        info->return_type = f->return_type;
        ecs_vec_init_t(NULL, &info->params, ecs_command_param_info_t, 0);

        const ecs_script_parameter_t *params = ecs_vec_first(&f->params);
        int32_t p, param_count = ecs_vec_count(&f->params);
        for (p = 0; p < param_count; p ++) {
            ecs_command_param_info_t *pi = ecs_vec_append_t(
                NULL, &info->params, ecs_command_param_info_t);
            pi->name = ecs_os_strdup(params[p].name);
            pi->type = params[p].type;
            pi->optional = params[p].optional;
            pi->brief = ecs_os_strdup(params[p].brief);
        }
    }

    if (filter && !count) {
        flecs_command_err(ctx, "unknown command '%s'", filter);
    }

    ecs_os_free(elems);
    ecs_vec_fini_t(NULL, &children, ecs_entity_t);
}

static void flecs_command_ls(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    bool valid;
    ecs_entity_t scope = flecs_command_scope_arg(ctx, argc, argv, 0, &valid);
    if (valid) {
        flecs_command_children(ctx->world, scope, result->ptr);
    }
}

static void flecs_command_tree_visit(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t depth,
    void *ctx)
{
    (void)world;
    ecs_command_tree_node_t *node = ecs_vec_append_t(
        NULL, ctx, ecs_command_tree_node_t);
    node->entity = entity;
    node->depth = depth;
}

static void flecs_command_tree(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    bool valid;
    ecs_entity_t scope = flecs_command_scope_arg(ctx, argc, argv, 0, &valid);
    if (valid) {
        flecs_command_descendants(ctx->world, scope, 0,
            flecs_command_tree_visit, result->ptr);
    }
}

static void flecs_command_cat(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    ecs_command_entity_info_t *info = result->ptr;
    info->entity = e;
    info->id = e;
    ecs_vec_init_if_t(&info->components, ecs_command_component_value_t);
    ecs_vec_init_if_t(&info->ids, ecs_id_t);

    const ecs_type_t *type = ecs_get_type(world, e);
    int32_t i, count = type ? type->count : 0;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = type->array[i];
        ecs_entity_t value_type = flecs_command_value_type(world, id);
        const void *ptr = value_type ? ecs_get_id(world, e, id) : NULL;

        if (ptr) {
            ecs_command_component_value_t *cv = ecs_vec_append_t(
                NULL, &info->components, ecs_command_component_value_t);
            cv->id = id;
            cv->value = ecs_value_init(world, value_type, ptr);
        } else {
            ecs_vec_append_t(NULL, &info->ids, ecs_id_t)[0] = id;
        }
    }
}

static ecs_entity_t flecs_command_type_arg(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t component)
{
    if (!flecs_command_check_alive(ctx, component)) {
        return 0;
    }

    ecs_entity_t type = ecs_get_typeid(ctx->world, component);
    if (!type || !ecs_get_type_info(ctx->world, type)) {
        flecs_command_err(ctx, "'%s' is not a type",
            flecs_errstr(ecs_get_path(ctx->world, component)));
        return 0;
    }

    return type;
}

static void flecs_command_schema(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t type = flecs_command_type_arg(
        ctx, flecs_command_arg(argv, 0, ecs_entity_t));
    if (!type) {
        return;
    }

    char *str = ecs_type_to_script(ctx->world, type);
    if (!str) {
        flecs_command_fail(ctx);
        return;
    }

    *(char**)result->ptr = str;
}

static void flecs_command_schema_value(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t type = flecs_command_type_arg(
        ctx, flecs_command_arg(argv, 0, ecs_entity_t));
    if (!type) {
        return;
    }

    *(ecs_value_t*)result->ptr = ecs_value_new(ctx->world, type);
}

static void flecs_command_cd(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    ecs_entity_t scope = argc ? flecs_command_arg(argv, 0, ecs_entity_t) : 0;
    if (scope && !flecs_command_check_alive(ctx, scope)) {
        return;
    }

    ecs_entity_t session = ecs_command_get_session(world);
    EcsCommandSession *s = ecs_get_mut(world, session, EcsCommandSession);
    s->scope = scope;
    *(ecs_entity_t*)result->ptr = scope;
}

static void flecs_command_pwd(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    *(ecs_entity_t*)result->ptr = flecs_command_scope(ctx->world);
}

static void flecs_command_clear(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    (void)result;
    ecs_world_t *world = ctx->world;
    ecs_entity_t session = ecs_command_get_session(world);
    EcsCommandSession *s = ecs_get_mut(world, session, EcsCommandSession);
    flecs_command_history_clear(s);
}

static bool flecs_command_check_id(
    const ecs_function_ctx_t *ctx,
    ecs_id_t id)
{
    if (!id || !ecs_id_is_valid(ctx->world, id)) {
        flecs_command_err(ctx, "invalid component '%s'",
            flecs_errstr(ecs_id_str(ctx->world, id)));
        return false;
    }

    return true;
}

static void flecs_command_get(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_id_t id = flecs_command_arg(argv, 1, ecs_id_t);
    if (!flecs_command_check_alive(ctx, e) || !flecs_command_check_id(ctx, id)) {
        return;
    }

    ecs_entity_t type = ecs_get_typeid(world, id);
    const void *ptr = type ? ecs_get_id(world, e, id) : NULL;
    if (!ptr) {
        if (!type) {
            flecs_command_err(ctx, "'%s' is not a component with a value",
                flecs_errstr(ecs_id_str(world, id)));
        } else {
            flecs_command_err(ctx, "entity '%s' does not have component '%s'",
                flecs_errstr(ecs_get_path(world, e)),
                flecs_errstr_1(ecs_id_str(world, id)));
        }
        return;
    }

    *(ecs_value_t*)result->ptr = ecs_value_init(world, type, ptr);
}

static void flecs_command_set(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_id_t id = flecs_command_arg(argv, 1, ecs_id_t);
    const char *expr = flecs_command_arg(argv, 2, char*);
    if (!flecs_command_check_alive(ctx, e) || !flecs_command_check_id(ctx, id)) {
        return;
    }

    ecs_value_t value;
    if (flecs_command_parse_component(ctx, e, id, expr, &value)) {
        return;
    }

    ecs_set_id(world, e, id,
        flecs_itosize(ecs_get_type_info(world, value.type)->size), value.ptr);
    *(ecs_value_t*)result->ptr = value;
}

static void flecs_command_add(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_id_t id = flecs_command_arg(argv, 1, ecs_id_t);
    if (!flecs_command_check_alive(ctx, e) || !flecs_command_check_id(ctx, id)) {
        return;
    }

    ecs_add_id(ctx->world, e, id);
    *(ecs_id_t*)result->ptr = id;
}

static void flecs_command_rm(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)result;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    if (argc > 1) {
        ecs_id_t id = flecs_command_arg(argv, 1, ecs_id_t);
        if (!flecs_command_check_id(ctx, id)) {
            return;
        }

        ecs_remove_id(ctx->world, e, id);
    } else {
        ecs_delete(ctx->world, e);
    }
}

static void flecs_command_new(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    ecs_entity_t scope = flecs_command_scope(world);
    const char *name = argc ? flecs_command_arg(argv, 0, char*) : NULL;
    ecs_entity_t e = 0;

    if (name && name[0]) {
        if (ecs_lookup_path_w_sep(world, scope, name, ".", NULL, false)) {
            flecs_command_err(ctx, "entity '%s' already exists", name);
            return;
        }

        e = ecs_entity(world, { .name = name, .parent = scope });
        if (!e) {
            flecs_command_fail(ctx);
            return;
        }
    } else if (scope) {
        e = ecs_new_w_pair(world, EcsChildOf, scope);
    } else {
        e = ecs_new(world);
    }

    *(ecs_entity_t*)result->ptr = e;
}

static void flecs_command_mv(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    const char *path = flecs_command_arg(argv, 1, char*);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    if (!path || !path[0]) {
        flecs_command_err(ctx, "invalid path for mv");
        return;
    }

    ecs_entity_t parent = 0;
    const char *name = strrchr(path, '.');
    if (name) {
        char *parent_path = flecs_asprintf("%.*s",
            flecs_ito(int32_t, name - path), path);
        parent = ecs_lookup_path_w_sep(
            world, 0, parent_path, ".", NULL, false);
        if (!parent) {
            ecs_err("parent '%s' does not exist", parent_path);
            ecs_os_free(parent_path);
            flecs_command_fail(ctx);
            return;
        }

        ecs_os_free(parent_path);
        name ++;
    } else {
        name = path;
    }

    if (!name[0]) {
        flecs_command_err(ctx, "invalid path '%s' for mv", path);
        return;
    }

    ecs_entity_t cur;
    for (cur = parent; cur; cur = ecs_get_parent(world, cur)) {
        if (cur == e) {
            flecs_command_err(ctx, "cannot move entity into itself");
            return;
        }
    }

    ecs_entity_t existing = ecs_lookup_child(world, parent, name);
    if (existing && existing != e) {
        flecs_command_err(ctx, "entity '%s' already exists", path);
        return;
    }

    if (ecs_get_parent(world, e) != parent) {
        if (ecs_has(world, e, EcsParent)) {
            if (parent) {
                ecs_set(world, e, EcsParent, { parent });
            } else {
                ecs_remove(world, e, EcsParent);
            }
        } else if (parent) {
            ecs_add_pair(world, e, EcsChildOf, parent);
        } else {
            ecs_remove_pair(world, e, EcsChildOf, EcsWildcard);
        }
    }

    ecs_set_name(world, e, name);
    *(ecs_entity_t*)result->ptr = e;
}

static void flecs_command_man(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    ecs_command_doc_info_t *info = result->ptr;
    info->entity = e;
    info->name = ecs_os_strdup(ecs_doc_get_name(world, e));
    info->brief = ecs_os_strdup(ecs_doc_get_brief(world, e));
    info->detail = ecs_os_strdup(ecs_doc_get_detail(world, e));
    info->link = ecs_os_strdup(ecs_doc_get_link(world, e));
    info->color = ecs_os_strdup(ecs_doc_get_color(world, e));
}

static void flecs_command_eval(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    const char *expr = flecs_command_arg(argv, 0, char*);
    ecs_value_t value = {0};
    if (flecs_command_parse_value(world, expr ? expr : "", &value, false)) {
        if (value.ptr) {
            ecs_ptr_free(world, value.type, value.ptr);
        }

        flecs_command_fail(ctx);
        return;
    }

    ecs_value_t *out = result->ptr;
    *out = ecs_value_new(world, value.type);
    ecs_ptr_move(world, value.type, out->ptr, value.ptr);
    ecs_ptr_free(world, value.type, value.ptr);
}

static void flecs_command_query(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    const char *expr = flecs_command_arg(argv, 0, char*);
    ecs_query_t *q = ecs_query(world, { .expr = expr });
    if (!q) {
        flecs_command_fail(ctx);
        return;
    }

    ecs_vec_t *rows = result->ptr;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        int32_t i, count = it.count ? it.count : 1;
        for (i = 0; i < count; i ++) {
            ecs_command_query_row_t *row = ecs_vec_append_t(
                NULL, rows, ecs_command_query_row_t);
            row->entity = it.count ? it.entities[i] : 0;
            ecs_vec_init_t(NULL, &row->fields,
                ecs_command_component_value_t, it.field_count);

            int8_t f;
            for (f = 0; f < it.field_count; f ++) {
                ecs_id_t id = ecs_field_id(&it, f);
                ecs_entity_t type = flecs_command_value_type(world, id);
                ecs_size_t size = it.sizes[f];
                if (!size || !ecs_field_is_set(&it, f) || !type) {
                    continue;
                }

                const void *ptr = ecs_field_w_size(&it, (size_t)size, f);
                if (!ptr) {
                    continue;
                }

                if (ecs_field_is_self(&it, f)) {
                    ptr = ECS_ELEM(ptr, size, i);
                }

                ecs_command_component_value_t *cv = ecs_vec_append_t(
                    NULL, &row->fields, ecs_command_component_value_t);
                cv->id = id;
                cv->value = ecs_value_init(world, type, ptr);
            }
        }
    }

    ecs_query_fini(q);
}

static void flecs_command_quit(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    (void)result;
    ecs_quit(ctx->world);
}

typedef struct {
    const char *text;
    bool case_insensitive;
    ecs_vec_t *out;
} flecs_command_search_t;

static void flecs_command_find_visit(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t depth,
    void *ctx)
{
    (void)depth;
    flecs_command_search_t *search = ctx;
    if (flecs_command_match(ecs_get_name(world, entity), search->text,
        search->case_insensitive))
    {
        ecs_vec_append_t(NULL, search->out, ecs_entity_t)[0] = entity;
    }
}

static void flecs_command_find(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    flecs_command_search_t search = {
        .text = flecs_command_arg(argv, 0, char*),
        .out = result->ptr
    };

    if (!search.text) {
        search.text = "";
    }

    flecs_command_descendants(ctx->world, flecs_command_scope(ctx->world), 0,
        flecs_command_find_visit, &search);
}

static void flecs_command_grep_append(
    flecs_command_search_t *search,
    ecs_entity_t entity,
    ecs_id_t id,
    char *text)
{
    ecs_command_grep_match_t *m = ecs_vec_append_t(
        NULL, search->out, ecs_command_grep_match_t);
    m->entity = entity;
    m->id = id;
    m->text = text;
}

static void flecs_command_grep_visit(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t depth,
    void *ctx)
{
    (void)depth;
    flecs_command_search_t *search = ctx;
    const char *name = ecs_get_name(world, entity);
    if (flecs_command_match(name, search->text, search->case_insensitive)) {
        flecs_command_grep_append(search, entity, 0, ecs_os_strdup(name));
    }

    const ecs_type_t *type = ecs_get_type(world, entity);
    int32_t i, count = type ? type->count : 0;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = type->array[i];
        if (ECS_IS_PAIR(id)) {
            ecs_entity_t first = ECS_PAIR_FIRST(id);
            if (first == ecs_id(EcsIdentifier) || first == EcsChildOf) {
                continue;
            }
        }

        char *str = ecs_id_str(world, id);
        if (flecs_command_match(str, search->text, search->case_insensitive)) {
            flecs_command_grep_append(search, entity, id, str);
            continue;
        }

        ecs_os_free(str);

        ecs_entity_t value_type = flecs_command_value_type(world, id);
        const void *ptr = value_type ? ecs_get_id(world, entity, id) : NULL;
        if (!ptr) {
            continue;
        }

        str = ecs_ptr_to_expr(world, value_type, ptr);
        if (flecs_command_match(str, search->text, search->case_insensitive)) {
            flecs_command_grep_append(search, entity, id, str);
        } else {
            ecs_os_free(str);
        }
    }
}

static void flecs_command_grep(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    bool valid;
    ecs_entity_t scope = flecs_command_scope_arg(ctx, argc, argv, 1, &valid);
    if (!valid) {
        return;
    }

    flecs_command_search_t search = {
        .text = flecs_command_arg(argv, 0, char*),
        .case_insensitive = ctx->ctx != NULL,
        .out = result->ptr
    };

    if (!search.text) {
        search.text = "";
    }

    flecs_command_descendants(ctx->world, scope, 0,
        flecs_command_grep_visit, &search);
}

void flecs_command_core_import(
    ecs_world_t *world)
{
    ecs_entity_t string_t = ecs_id(ecs_string_t);
    ecs_entity_t entity_t = ecs_id(ecs_entity_t);
    ecs_entity_t id_t = ecs_id(ecs_id_t);
    ecs_entity_t value_t = ecs_id(ecs_value_t);
    ecs_entity_t entity_vec = flecs_script_vector_type(world, entity_t);
    ecs_entity_t info_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_info_t));
    ecs_entity_t tree_node_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_tree_node_t));
    ecs_entity_t query_row_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_query_row_t));
    ecs_entity_t grep_match_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_grep_match_t));

    flecs_command_desc_t commands[] = {
        { "help", flecs_command_help, info_vec,
            "Commands with parameters and descriptions", {
            { "command", string_t, true, "Command to describe" }}},
        { "ls", flecs_command_ls, entity_vec, "Direct children", {
            { "scope", entity_t, true,
                "Scope to list (default: session scope)" }}},
        { "tree", flecs_command_tree, tree_node_vec,
            "Descendants in preorder", {
            { "scope", entity_t, true,
                "Scope to list (default: session scope)" }}},
        { "cat", flecs_command_cat, ecs_id(ecs_command_entity_info_t),
            "Id, components with values, and ids without values", {
            { "entity", entity_t, false, "Entity to show" }}},
        { "schema", flecs_command_schema, string_t, "Type as script", {
            { "component", entity_t, false, "Component or type" }}},
        { "schema_value", flecs_command_schema_value, value_t,
            "Default value", {
            { "component", entity_t, false, "Component or type" }}},
        { "cd", flecs_command_cd, entity_t,
            "Set the session scope, no argument goes to the root", {
            { "scope", entity_t, true, "New scope (default: root)" }}},
        { "pwd", flecs_command_pwd, entity_t, "Session scope" },
        { "clear", flecs_command_clear, 0, "Clear the session history" },
        { "get", flecs_command_get, value_t, "Get component value", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, false, "Component" }}},
        { "set", flecs_command_set, value_t,
            "Set component value, parsed into the current value", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, false, "Component" },
            { "value", string_t, false,
                "Value, partial initializers keep the other members" }}},
        { "add", flecs_command_add, id_t, "Add component", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, false, "Component, tag or pair" }}},
        { "rm", flecs_command_rm, 0,
            "Remove component, or delete entity if no component is provided", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, true, "Component, tag or pair" }}},
        { "new", flecs_command_new, entity_t,
            "Create entity in the session scope", {
            { "name", string_t, true, "Entity name" }}},
        { "mv", flecs_command_mv, entity_t, "Move or rename entity", {
            { "entity", entity_t, false, "Entity to move or rename" },
            { "path", string_t, false, "New full path" }}},
        { "man", flecs_command_man, ecs_id(ecs_command_doc_info_t),
            "Documentation (flecs.doc data)", {
            { "entity", entity_t, false, "Entity" }}},
        { "eval", flecs_command_eval, value_t, "Evaluate expression", {
            { "expr", string_t, false, "Expression" }}},
        { "query", flecs_command_query, query_row_vec, "Run query", {
            { "expr", string_t, false, "Query expression" }}},
        { "quit", flecs_command_quit, 0, "Quit the application" },
        { "find", flecs_command_find, entity_vec,
            "Find descendants of the session scope by name substring", {
            { "text", string_t, false, "Text to find in names" }}},
        { "grep", flecs_command_grep, grep_match_vec,
            "Search names, component names and values of descendants", {
            { "text", string_t, false, "Text to find" },
            { "scope", entity_t, true,
                "Scope to search (default: session scope)" }}},
        { "grep_i", flecs_command_grep, grep_match_vec,
            "Case insensitive grep", {
            { "text", string_t, false, "Text to find" },
            { "scope", entity_t, true,
                "Scope to search (default: session scope)" }}, (void*)1 },
    };

    flecs_command_register(world, commands,
        ECS_SIZEOF(commands) / ECS_SIZEOF(flecs_command_desc_t));
}

#endif
