#include "flecs.h"

#ifdef FLECS_PLECS

#include "../private_api.h"

#define TOK_NEWLINE '\n'

#define STACK_MAX_SIZE (32)

typedef struct {
    ecs_entity_t last_subject;
    ecs_entity_t scope[STACK_MAX_SIZE];
    int32_t sp;
} plecs_state_t;

static
ecs_entity_t ensure_entity(
    ecs_world_t *world,
    const char *path)
{
    if (!path) {
        return 0;
    }

    ecs_entity_t e = ecs_lookup_path_w_sep(world, 0, path, NULL, NULL, true);
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
    int64_t column,
    plecs_state_t *state)
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

    if (subj) {
        if (!obj) {
            ecs_add_id(world, subj, pred);
        } else {
            ecs_add_pair(world, subj, pred, obj);
        }
        state->last_subject = subj;
    } else {
        state->last_subject = pred;
    }

    return 0;
}

static
const char *plecs_skip_space(
    const char *ptr)
{
    while ((*ptr != '\n') && isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

static
bool is_newline_comment(
    const char *ptr)
{
    if (ptr[0] == '/' && ptr[1] == '/') {
        return true;
    }
    return false;
}

static 
const char* skip_fluff(
    const char *ptr) 
{
    do {
        /* Skip whitespaces before checking for a comment */
        ptr = plecs_skip_space(ptr);

        /* Newline comment, skip until newline character */
        if (is_newline_comment(ptr)) {
            ptr += 2;
            while (ptr[0] && ptr[0] != '\n') {
                ptr ++;
            }
        }

        /* If a newline character is found, skip it */
        if (ptr[0] == TOK_NEWLINE) {
            ptr ++;
        }

    } while (isspace(ptr[0]) || is_newline_comment(ptr));

    return ptr;
}

static
const char* parse_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    (void)world;
    (void)name;
    (void)expr;

    ptr = skip_fluff(ptr);

    if (ptr[0] == '{') {
        state->scope[++ state->sp] = state->last_subject;
        ptr ++;
        ecs_set_scope(world, state->last_subject);
    }

    while (ptr[0] == '}') {
        state->sp --;
        ptr = skip_fluff(ptr + 1);
        ecs_set_scope(world, state->scope[state->sp]);
    }

    return skip_fluff(ptr);
}

int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *expr) 
{
    const char *ptr = expr;
    ecs_term_t term = {0};
    plecs_state_t state = {0};

    if (!expr) {
        return 0;
    }

    state.scope[0] = ecs_get_scope(world);

    do {
        expr = ptr = parse_stmt(world, name, expr, ptr, &state);
        if (!ptr) {
            goto error;
        }

        if (!ptr[0]) {
            break; /* End of expression */
        }

        ptr = ecs_parse_term(world, name, expr, ptr, &term);
        if (!ptr) {
            goto error; /* Error occurred */
        }

        if (!ecs_term_is_initialized(&term)) {
            goto error; /* No term found */
        }

        if (create_term(world, &term, name, expr, (ptr - expr), &state)) {
            goto error; /* Failed to create term */
        }

        ecs_term_fini(&term);
    } while (true);

    ecs_set_scope(world, state.scope[0]);

    if (state.sp != 0) {
        ecs_parser_error(name, expr, 0, "missing end of scope");
        goto error;
    }

    return 0;
error:
    ecs_term_fini(&term);
    return -1;
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
