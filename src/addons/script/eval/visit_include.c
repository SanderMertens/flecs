/**
 * @file addons/script/eval/visit_include.c
 * @brief Script include visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static bool flecs_script_include_has_parent_dir(
    const char *path)
{
    const char *p = path;
    while (p[0]) {
        if (p[0] == '.' && p[1] == '.') {
            char before = (p == path) ? '/' : p[-1];
            char after = p[2];
            if ((before == '/' || before == '\\') &&
                (after == '/' || after == '\\' || after == '\0'))
            {
                return true;
            }
        }
        p ++;
    }
    return false;
}

static char* flecs_script_include_resolve(
    const char *script_name,
    const char *include_path)
{
    const char *dir_end = NULL;
    if (script_name) {
        const char *p = script_name;
        while (p[0]) {
            if (p[0] == '/' || p[0] == '\\') {
                dir_end = p + 1;
            }
            p ++;
        }
    }

    if (dir_end) {
        ecs_size_t dir_len = flecs_ito(ecs_size_t, dir_end - script_name);
        ecs_size_t incl_len = ecs_os_strlen(include_path);
        char *result = ecs_os_malloc(dir_len + incl_len + 1);
        ecs_os_memcpy(result, script_name, dir_len);
        ecs_os_memcpy(result + dir_len, include_path, incl_len + 1);
        return result;
    } else {
        return ecs_os_strdup(include_path);
    }
}

static bool flecs_script_include_is_absolute(
    const char *path)
{
    if (path[0] == '/' || path[0] == '\\') {
        return true;
    }
    if (path[0] && path[1] == ':') {
        return true;
    }
    return false;
}

static int flecs_script_include_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_include_t *node)
{
    if (flecs_script_include_is_absolute(node->filename)) {
        flecs_script_eval_error(v, node,
            "include path '%s' must be relative", node->filename);
        return -1;
    }

    if (flecs_script_include_has_parent_dir(node->filename)) {
        flecs_script_eval_error(v, node,
            "include path '%s' must not contain '..'", node->filename);
        return -1;
    }

    bool is_managed = v->script_entity != 0;
    ecs_entity_t parent_script_entity = v->script_entity;

    const char *script_name = NULL;
    if (parent_script_entity) {
        const EcsScript *sc = ecs_get(v->world, parent_script_entity, EcsScript);
        if (sc && sc->filename) {
            script_name = sc->filename;
        }
    }
    if (!script_name) {
        script_name = v->base.script->pub.name;
    }

    const char *include_path = node->filename;
    char *with_ext = NULL;
    static const char ext[] = ".flecs";
    ecs_size_t fn_len = ecs_os_strlen(include_path);
    ecs_size_t ext_len = sizeof(ext) - 1;
    if (fn_len < ext_len || ecs_os_strcmp(
        include_path + fn_len - ext_len, ext))
    {
        with_ext = ecs_os_malloc(fn_len + ext_len + 1);
        ecs_os_memcpy(with_ext, include_path, fn_len);
        ecs_os_memcpy(with_ext + fn_len, ext, ext_len + 1);
        include_path = with_ext;
    }

    char *resolved = flecs_script_include_resolve(script_name, include_path);
    ecs_os_free(with_ext);

    int result = 0;

    if (is_managed) {
        ecs_entity_t existing = ecs_lookup_path_w_sep(
            v->world, 0, resolved, "/", NULL, false);
        if (existing && ecs_has(v->world, existing, EcsScript)) {
            goto done;
        }

        ecs_entity_t prev_scope = ecs_set_scope(v->world, 0);
        ecs_script_runtime_t *runtime = flecs_script_runtime_get(v->world);
        runtime->include_depth ++;
        ecs_entity_t e = ecs_script_init(v->world, &(ecs_script_desc_t){
            .filename = resolved
        });
        runtime->include_depth --;
        ecs_set_scope(v->world, prev_scope);

        if (!e) {
            flecs_script_eval_error(v, node,
                "failed to include managed script '%s'", resolved);
            result = -1;
            goto done;
        }

        const EcsScript *sc = ecs_get(v->world, e, EcsScript);
        if (sc && sc->error) {
            flecs_script_eval_error(v, node,
                "failed to include managed script '%s': %s",
                resolved, sc->error);
            result = -1;
        }
    } else {
        char *code = flecs_load_from_file(resolved);
        if (!code) {
            flecs_script_eval_error(v, node,
                "failed to load include '%s'", resolved);
            result = -1;
            goto done;
        }

        if (ecs_script_run(v->world, resolved, code, NULL)) {
            result = -1;
        }
        ecs_os_free(code);
    }

done:
    ecs_os_free(resolved);
    return result;
}

int flecs_script_eval_include(
    ecs_script_eval_visitor_t *v,
    ecs_script_include_t *node)
{
    ecs_script_scope_t *cur_scope = ecs_script_current_scope(v);
    if (cur_scope != v->base.script->root) {
        flecs_script_eval_error(v, node,
            "include is only allowed at the root scope");
        return -1;
    }

    return 0;
}

int flecs_script_visit_include(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        if (nodes[i]->kind != EcsAstInclude) {
            continue;
        }

        if (flecs_script_include_node(v, (ecs_script_include_t*)nodes[i])) {
            return -1;
        }
    }

    return 0;
}

#endif
