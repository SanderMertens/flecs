#include "flecs.h"

#ifdef FLECS_PLECS

#include "../private_api.h"

#define TOK_NEWLINE '\n'

static
ecs_entity_t ensure_entity(
    ecs_world_t *world,
    const char *path)
{
    ecs_entity_t e = ecs_lookup_fullpath(world, path);
    if (!e) {
        e = ecs_new_from_path(world, 0, path);
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
    }

    return e;
}

static
int create_term(
    ecs_world_t *world, 
    ecs_term_t *term,
    const char *name,
    const char *expr,
    int32_t column)
{
    if (!ecs_term_id_is_set(&term->pred)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    if (!ecs_term_id_is_set(&term->args[0])) {
        ecs_parser_error(name, expr, column, "missing subject in expression");
        return -1;
    }

    ecs_entity_t pred = ensure_entity(world, term->pred.name);
    ecs_entity_t subj = ensure_entity(world, term->args[0].name);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->args[1])) {
        obj = ensure_entity(world, term->args[1].name);
    }

    if (!obj) {
        ecs_add_id(world, subj, pred);
    } else {
        ecs_add_pair(world, subj, pred, obj);
    }

    return 0;
}

int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *expr) 
{
    const char *ptr = expr;
    ecs_term_t term = {0};

    if (!expr) {
        return 0;
    }

    while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) {
        if (!ecs_term_is_initialized(&term)) {
            break;
        }
        
        if (create_term(world, &term, name, expr, (int32_t)(ptr - expr))) {
            return -1;
        }

        ecs_term_fini(&term);

        if (ptr[0] == TOK_NEWLINE) {
            ptr ++;
            expr = ptr;
        }
    }

    return 0;
}

int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename) 
{
    FILE* file;
    char* content = NULL;
    int32_t bytes;
    size_t size;

    /* Open file for reading */
    ecs_os_fopen(&file, filename, "r");
    if (!file) {
        ecs_err("%s (%s)", ecs_os_strerror(errno), filename);
        goto error;
    }

    /* Determine file size */
    fseek(file, 0 , SEEK_END);
    bytes = (int32_t)ftell(file);
    if (bytes == -1) {
        goto error;
    }
    rewind(file);

    /* Load contents in memory */
    content = ecs_os_malloc(bytes + 1);
    size = (size_t)bytes;
    if (!(size = fread(content, 1, size, file)) && bytes) {
        ecs_err("%s: read zero bytes instead of %d", filename, size);
        ecs_os_free(content);
        content = NULL;
        goto error;
    } else {
        content[size] = '\0';
    }

    fclose(file);

    int result = ecs_plecs_from_str(world, filename, content);
    ecs_os_free(content);
    return result;
error:
    ecs_os_free(content);
    return -1;
}

#endif
