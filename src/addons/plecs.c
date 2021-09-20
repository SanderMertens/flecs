#include "flecs.h"

#ifdef FLECS_PLECS

#include "../private_api.h"

#define TOK_NEWLINE '\n'
#define TOK_WITH "with"

#define STACK_MAX_SIZE (64)

typedef struct {
    ecs_entity_t last_predicate;
    ecs_entity_t last_subject;
    ecs_entity_t last_object;
    ecs_entity_t scope[STACK_MAX_SIZE];
    ecs_entity_t with[STACK_MAX_SIZE];
    int32_t with_frames[STACK_MAX_SIZE];
    int32_t sp;
    int32_t with_frame;
    bool with_clause;
} plecs_state_t;

static
ecs_entity_t ensure_entity(
    ecs_world_t *world,
    const char *path,
    bool is_subject)
{
    if (!path) {
        return 0;
    }

    ecs_entity_t e = ecs_lookup_path_w_sep(world, 0, path, NULL, NULL, true);
    if (!e) {
        if (!is_subject) {
            /* If this is not a subject create an existing empty id, which 
             * ensures that scope & with are not applied */
            e = ecs_new_id(world);
        }

        e = ecs_add_path(world, e, 0, path);
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
    } else {
        /* If entity exists, make sure it gets the right scope and with */
        if (is_subject) {
            ecs_entity_t scope = ecs_get_scope(world);
            if (scope) {
                ecs_add_id(world, e, scope);
            }

            ecs_entity_t with = ecs_get_with(world);
            if (with) {
                ecs_add_id(world, e, with);
            }
        }
    }

    return e;
}

static
bool pred_is_subj(
    ecs_term_t *term,
    plecs_state_t *state)
{
    if (term->args[0].name != NULL) {
        return false;
    }
    if (term->args[1].name != NULL) {
        return false;
    }
    if (term->args[0].set.mask == EcsNothing) {
        return false;
    }
    if (state->with_clause) {
        return false;
    }
    return true;
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
    state->last_subject = 0;
    state->last_predicate = 0;
    state->last_object = 0;

    if (!ecs_term_id_is_set(&term->pred)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    bool pred_as_subj = pred_is_subj(term, state);
    ecs_entity_t pred = ensure_entity(world, term->pred.name, pred_as_subj); 
    ecs_entity_t subj = ensure_entity(world, term->args[0].name, true);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->args[1])) {
        obj = ensure_entity(world, term->args[1].name, true);
    }

    if (subj) {
        if (!obj) {
            ecs_add_id(world, subj, pred);
        } else {
            ecs_add_pair(world, subj, pred, obj);
            state->last_object = obj;
        }
        state->last_predicate = pred;
        state->last_subject = subj;
    } else {
        if (!obj) {
            /* If no subject or object were provided, use predicate as subj 
             * unless the expression explictly excluded the subject */
            if (pred_as_subj) {
                state->last_subject = pred;
                subj = pred;
            } else {
                state->last_predicate = pred;
            }
        } else {
            state->last_predicate = pred;
            state->last_object = obj;
        }
    }

    /* If this is a with clause (the list of entities between 'with' and scope
     * open), add subject to the array of with frames */
    if (state->with_clause) {
        ecs_assert(pred != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_id_t id;

        if (obj) {
            id = ecs_pair(pred, obj);
        } else {
            id = pred;
        }

        state->with[state->with_frame ++] = id;

    /* If this is not a with clause, add with frames to subject */
    } else {
        if (subj) {
            int32_t i, frame_count = state->with_frames[state->sp];
            for (i = 0; i < frame_count; i ++) {
                ecs_add_id(world, subj, state->with[i]);
            }
        }
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

    bool stmt_parsed;

    do {
        stmt_parsed = false;

        ptr = skip_fluff(ptr);

        if (!ecs_os_strncmp(ptr, TOK_WITH " ", 5)) {
            /* Add following expressions to with list */
            state->with_clause = true;
            ptr = skip_fluff(ptr + 5);
            stmt_parsed = true;
        }

        if (ptr[0] == '{') {
            state->sp ++;

            ecs_entity_t scope = 0;

            if (!state->with_clause) {
                if (state->last_subject) {
                    scope = state->last_subject;
                    ecs_set_scope(world, state->last_subject);
                } else {
                    if (state->last_object) {
                        scope = ecs_pair(
                            state->last_predicate, state->last_object);
                        ecs_set_with(world, scope);
                    } else {
                        scope = ecs_pair(EcsChildOf, state->last_predicate);
                        ecs_set_scope(world, state->last_predicate);
                    }
                }
                state->scope[state->sp] = scope;
            } else {
                state->scope[state->sp] = state->scope[state->sp - 1];
            }

            state->with_frames[state->sp] = state->with_frame;
            state->with_clause = false;

            ptr ++;
            stmt_parsed = true;
        }

        while (ptr[0] == '}') {
            state->scope[state->sp] = 0;

            state->sp --;
            ptr = skip_fluff(ptr + 1);
            ecs_id_t id = state->scope[state->sp];
            if (!id || ECS_HAS_ROLE(id, PAIR)) {
                ecs_set_with(world, id);
            }

            if (!id || !ECS_HAS_ROLE(id, PAIR)) {
                ecs_set_scope(world, id);
            }

            state->with_frame = state->with_frames[state->sp];
            stmt_parsed = true;
        }

    } while (stmt_parsed);

    return ptr;
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
    ecs_entity_t prev_with = ecs_set_with(world, 0);

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
    ecs_set_with(world, prev_with);

    if (state.sp != 0) {
        ecs_parser_error(name, expr, 0, "missing end of scope");
        goto error;
    }

    return 0;
error:
    ecs_set_scope(world, state.scope[0]);
    ecs_set_with(world, prev_with);
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
