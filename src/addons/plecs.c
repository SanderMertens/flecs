#include "flecs.h"

#ifdef FLECS_PLECS

#include "../private_api.h"

#define TOK_NEWLINE '\n'
#define TOK_WITH "with"
#define TOK_USING "using"

#define STACK_MAX_SIZE (64)

typedef struct {
    ecs_entity_t last_predicate;
    ecs_entity_t last_subject;
    ecs_entity_t last_object;
    ecs_id_t last_assign_id;
    ecs_entity_t assign_to;
    ecs_entity_t scope[STACK_MAX_SIZE];
    ecs_entity_t with[STACK_MAX_SIZE];
    ecs_entity_t using[STACK_MAX_SIZE];
    int32_t with_frames[STACK_MAX_SIZE];
    int32_t using_frames[STACK_MAX_SIZE];
    int32_t sp;
    int32_t with_frame;
    int32_t using_frame;
    bool with_clause;
    bool using_clause;
    bool assignment;
    bool isa_clause;
} plecs_state_t;

static
ecs_entity_t plecs_lookup(
    const ecs_world_t *world,
    const char *path,
    plecs_state_t *state,
    bool is_subject)
{
    ecs_entity_t e = 0;

    int using_scope = state->using_frame - 1;
    for (; using_scope >= 0; using_scope--) {
        e = ecs_lookup_path_w_sep(
            world, state->using[using_scope], path, NULL, NULL, false);
        if (e) {
            break;
        }
    }

    if (!e) {
        e = ecs_lookup_path_w_sep(world, 0, path, NULL, NULL, !is_subject);
    }

    return e;
}

/* Lookup action used for deserializing entity refs in component values */
static
ecs_entity_t plecs_lookup_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx)
{
    return plecs_lookup(world, path, ctx, false);
}

static
ecs_entity_t ensure_entity(
    ecs_world_t *world,
    plecs_state_t *state,
    const char *path,
    bool is_subject)
{
    if (!path) {
        return 0;
    }

    ecs_entity_t e = plecs_lookup(world, path, state, is_subject);
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
                ecs_add_pair(world, e, EcsChildOf, scope);
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
    if (state->assignment) {
        return false;
    }
    if (state->isa_clause) {
        return false;
    }
    if (state->using_clause) {
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
    state->last_assign_id = 0;

    if (!ecs_term_id_is_set(&term->pred)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    if (state->assignment && term->args[0].entity != EcsThis) {
        ecs_parser_error(name, expr, column, "invalid statement in assignment");
        return -1;
    }

    bool pred_as_subj = pred_is_subj(term, state);

    ecs_entity_t pred = ensure_entity(world, state, term->pred.name, pred_as_subj); 
    ecs_entity_t subj = ensure_entity(world, state, term->args[0].name, true);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->args[1])) {
        obj = ensure_entity(world, state, term->args[1].name, true);
    }

    if (state->assignment || state->isa_clause) {
        subj = state->assign_to;
    }

    if (state->isa_clause && obj) {
        ecs_parser_error(name, expr, column, 
            "invalid object in inheritance statement");
        return -1;
    }

    if (state->using_clause && (obj || subj)) {
        ecs_parser_error(name, expr, column, 
            "invalid predicate/object in using statement");
        return -1;
    }

    if (state->isa_clause) {
        pred = ecs_pair(EcsIsA, pred);
    }

    if (subj) {
        if (!obj) {
            ecs_add_id(world, subj, pred);
            state->last_assign_id = pred;
        } else {
            ecs_add_pair(world, subj, pred, obj);
            state->last_object = obj;
            state->last_assign_id = ecs_pair(pred, obj);
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
    
    } else if (state->using_clause) {
        ecs_assert(pred != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(obj == 0, ECS_INTERNAL_ERROR, NULL);

        state->using[state->using_frame ++] = pred;
        state->using_frames[state->sp] = state->using_frame;

    /* If this is not a with/using clause, add with frames to subject */
    } else {
        if (subj) {
            int32_t i, frame_count = state->with_frames[state->sp];
            for (i = 0; i < frame_count; i ++) {
                ecs_add_id(world, subj, state->with[i]);
            }
        }
    }

    if (state->isa_clause) {
        state->last_assign_id = 0;
        state->isa_clause = false;
    }

    return 0;
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

    state->using_clause = false;

    do {
        stmt_parsed = false;

        ptr = ecs_parse_fluff(ptr);

        /* Inheritance (IsA shorthand) statement */
        if (ptr[0] == ':') {
            if (state->isa_clause) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "cannot nest inheritance");
                return NULL;
            }

            if (!state->last_subject) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "missing entity to assign inheritance to");
                return NULL;
            }
            
            state->isa_clause = true;
            state->assign_to = state->last_subject;
            stmt_parsed = true;

            ptr = ecs_parse_fluff(ptr + 1);
        }

        /* Assignment statement */
        if (ptr[0] == '=') {
            if (state->isa_clause) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "missing base for inheritance statement");
                return NULL;
            }

            ecs_id_t assign_id = state->last_assign_id;
            if (state->assignment || assign_id) {
                /* Component value assignment */
#ifndef FLECS_EXPR
                ecs_parser_error(name, expr, ptr - expr,
                    "cannot parse component value, missing FLECS_EXPR addon");
                return NULL;
#else
                ecs_entity_t assign_to = state->assign_to;
                if (!assign_to) {
                    assign_to = state->last_subject;
                }

                if (!assign_to) {
                    ecs_parser_error(name, expr, ptr - expr, 
                        "missing entity to assign to");
                    return NULL;
                }

                ecs_entity_t type = ecs_get_typeid(world, assign_id);
                if (!type) {
                    char *id_str = ecs_id_str(world, assign_id);
                    ecs_parser_error(name, expr, ptr - expr, 
                        "cannot assign to non-component id '%s'", id_str);
                    ecs_os_free(id_str);
                    return NULL;
                }

                void *value_ptr = ecs_get_mut_id(
                    world, assign_to, assign_id, NULL);

                ptr = ecs_parse_expr(world, ptr + 1, type, value_ptr, 
                    &(ecs_expr_desc_t) {
                        .name = name,
                        .expr = expr,
                        .lookup_action = plecs_lookup_action,
                        .lookup_ctx = state
                    });
                if (!ptr) {
                    return NULL;
                }

                ecs_modified_id(world, assign_to, assign_id);
#endif
            } else {
                /* Component scope (add components to entity) */
                if (!state->last_subject) {
                    ecs_parser_error(name, expr, ptr - expr, 
                        "missing entity to assign to");
                    return NULL;
                }

                state->assignment = true;
                state->assign_to = state->last_subject;

                ptr = ecs_parse_fluff(ptr + 1);
                if (ptr[0] != '{') {
                    ecs_parser_error(name, expr, ptr - expr, 
                        "expected '{' after assignment");
                    return NULL;
                }

                ptr = ecs_parse_fluff(ptr + 1);
                stmt_parsed = true;
            }
        }

        /* Using statement */
        if (!ecs_os_strncmp(ptr, TOK_USING " ", 5)) {
            if (state->isa_clause || state->assignment) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid usage of using keyword");
                return NULL;
            }

            /* Add following expressions to using list */
            state->using_clause = true;
            ptr = ecs_parse_fluff(ptr + 5);
            stmt_parsed = true;
        }

        /* With statement */
        if (!ecs_os_strncmp(ptr, TOK_WITH " ", 5)) {
            if (state->isa_clause) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid with after inheritance");
                return NULL;
            }

            if (state->assignment) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid with in assignment");
                return NULL;
            }

            /* Add following expressions to with list */
            state->with_clause = true;
            ptr = ecs_parse_fluff(ptr + 5);
            stmt_parsed = true;
        }

        /* With / ChildOf scope */
        if (ptr[0] == '{') {
            if (state->isa_clause) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "missing base for inheritance");
                return NULL;
            }

            if (state->assignment) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid scope in assignment");
                return NULL;
            }

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
            if (state->isa_clause) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid '}' after inheritance statement");
                return NULL;
            }

            if (state->assignment) {
                state->assignment = false;
                stmt_parsed = true;
            } else {
                state->scope[state->sp] = 0;
                state->sp --;

                ecs_id_t id = state->scope[state->sp];
                if (!id || ECS_HAS_ROLE(id, PAIR)) {
                    ecs_set_with(world, id);
                }

                if (!id || !ECS_HAS_ROLE(id, PAIR)) {
                    ecs_set_scope(world, id);
                }

                state->with_frame = state->with_frames[state->sp];
                state->using_frame = state->using_frames[state->sp];
                state->last_subject = 0;
                stmt_parsed = true;
            }

            ptr = ecs_parse_fluff(ptr + 1);
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
