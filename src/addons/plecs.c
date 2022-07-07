#include "flecs.h"

#ifdef FLECS_PLECS

#include "../private_api.h"
#include <stdio.h>
#include <ctype.h>

#define TOK_NEWLINE '\n'
#define TOK_WITH "with"
#define TOK_USING "using"

#define STACK_MAX_SIZE (64)

typedef struct {
    const char *name;
    const char *code;

    ecs_entity_t last_predicate;
    ecs_entity_t last_subject;
    ecs_entity_t last_object;

    ecs_id_t last_assign_id;
    ecs_entity_t assign_to;

    ecs_entity_t scope[STACK_MAX_SIZE];
    ecs_entity_t default_scope_type[STACK_MAX_SIZE];
    ecs_entity_t with[STACK_MAX_SIZE];
    ecs_entity_t using[STACK_MAX_SIZE];
    int32_t with_frames[STACK_MAX_SIZE];
    int32_t using_frames[STACK_MAX_SIZE];
    int32_t sp;
    int32_t with_frame;
    int32_t using_frame;

    char *annot[STACK_MAX_SIZE];
    int32_t annot_count;

    bool with_stmt;
    bool scope_assign_stmt;
    bool using_stmt;
    bool assign_stmt;
    bool isa_stmt;

    int32_t errors;
} plecs_state_t;

static
ecs_entity_t plecs_lookup(
    const ecs_world_t *world,
    const char *path,
    plecs_state_t *state,
    ecs_entity_t rel,
    bool is_subject)
{
    ecs_entity_t e = 0;

    if (!is_subject) {
        ecs_entity_t oneof = 0;
        if (rel) {
            if (ecs_has_id(world, rel, EcsOneOf)) {
                oneof = rel;
            } else {
                oneof = ecs_get_object(world, rel, EcsOneOf, 0);
            }
            if (oneof) {
                return ecs_lookup_path_w_sep(
                    world, oneof, path, NULL, NULL, false);
            }
        }
        int using_scope = state->using_frame - 1;
        for (; using_scope >= 0; using_scope--) {
            e = ecs_lookup_path_w_sep(
                world, state->using[using_scope], path, NULL, NULL, false);
            if (e) {
                break;
            }
        }
    }

    if (!e) {
        e = ecs_lookup_path_w_sep(world, 0, path, NULL, NULL, !is_subject);
    }

    return e;
}

/* Lookup action used for deserializing entity refs in component values */
#ifdef FLECS_EXPR
static
ecs_entity_t plecs_lookup_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx)
{
    return plecs_lookup(world, path, ctx, 0, false);
}
#endif

static
ecs_entity_t plecs_ensure_entity(
    ecs_world_t *world,
    plecs_state_t *state,
    const char *path,
    ecs_entity_t rel,
    bool is_subject)
{
    if (!path) {
        return 0;
    }

    ecs_entity_t e = plecs_lookup(world, path, state, rel, is_subject);
    if (!e) {
        if (rel && flecs_get_oneof(world, rel)) {
            /* If relationship has oneof and entity was not found, don't proceed
             * with creating an entity as this can cause asserts later on */
            char *relstr = ecs_get_fullpath(world, rel);
            ecs_parser_error(state->name, 0, 0, 
                "invalid identifier '%s' for relation '%s'", path, relstr);
            ecs_os_free(relstr);
            return 0;
        }

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
bool plecs_pred_is_subj(
    ecs_term_t *term,
    plecs_state_t *state)
{
    if (term->subj.name != NULL) {
        return false;
    }
    if (term->obj.name != NULL) {
        return false;
    }
    if (term->subj.set.mask == EcsNothing) {
        return false;
    }
    if (state->with_stmt) {
        return false;
    }
    if (state->assign_stmt) {
        return false;
    }
    if (state->isa_stmt) {
        return false;
    }
    if (state->using_stmt) {
        return false;
    }

    return true;
}

/* Set masks aren't useful in plecs, so translate them back to entity names */
static
const char* plecs_set_mask_to_name(
    ecs_flags32_t flags) 
{
    if (flags == EcsSelf) {
        return "self";
    } else if (flags == EcsAll) {
        return "all";
    } else if (flags == EcsSuperSet) {
        return "super";
    } else if (flags == EcsSubSet) {
        return "sub";
    } else if (flags == EcsCascade || flags == (EcsSuperSet|EcsCascade)) {
        return "cascade";
    } else if (flags == EcsParent) {
        return "parent";
    }
    return NULL;
}

static
char* plecs_trim_annot(
    char *annot)
{
    annot = (char*)ecs_parse_whitespace(annot);
    int32_t len = ecs_os_strlen(annot) - 1;
    while (isspace(annot[len]) && (len > 0)) {
        annot[len] = '\0';
        len --;
    }
    return annot;
}

static
void plecs_apply_annotations(
    ecs_world_t *world,
    ecs_entity_t subj,
    plecs_state_t *state)
{
    (void)world;
    (void)subj;
    (void)state;
#ifdef FLECS_DOC
    int32_t i = 0, count = state->annot_count;
    for (i = 0; i < count; i ++) {
        char *annot = state->annot[i];
        if (!ecs_os_strncmp(annot, "@brief ", 7)) {
            annot = plecs_trim_annot(annot + 7);
            ecs_doc_set_brief(world, subj, annot);
        } else if (!ecs_os_strncmp(annot, "@link ", 6)) {
            annot = plecs_trim_annot(annot + 6);
            ecs_doc_set_link(world, subj, annot);
        } else if (!ecs_os_strncmp(annot, "@name ", 6)) {
            annot = plecs_trim_annot(annot + 6);
            ecs_doc_set_name(world, subj, annot);
        } else if (!ecs_os_strncmp(annot, "@color ", 7)) {
            annot = plecs_trim_annot(annot + 7);
            ecs_doc_set_color(world, subj, annot);
        }
    }
#else
    ecs_warn("cannot apply annotations, doc addon is missing");
#endif
}

static
int plecs_create_term(
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

    const char *pred_name = term->pred.name;
    const char *subj_name = term->subj.name;
    const char *obj_name = term->obj.name;

    if (!subj_name) {
        subj_name = plecs_set_mask_to_name(term->subj.set.mask);
    }
    if (!obj_name) {
        obj_name = plecs_set_mask_to_name(term->obj.set.mask);
    }

    if (!ecs_term_id_is_set(&term->pred)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    if (state->assign_stmt && term->subj.entity != EcsThis) {
        ecs_parser_error(name, expr, column, 
            "invalid statement in assign statement");
        return -1;
    }

    bool pred_as_subj = plecs_pred_is_subj(term, state);

    ecs_entity_t pred = plecs_ensure_entity(world, state, pred_name, 0, pred_as_subj); 
    ecs_entity_t subj = plecs_ensure_entity(world, state, subj_name, pred, true);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->obj)) {
        obj = plecs_ensure_entity(world, state, obj_name, pred,
            state->assign_stmt == false);
        if (!obj) {
            return -1;
        }
    }

    if (state->assign_stmt || state->isa_stmt) {
        subj = state->assign_to;
    }

    if (state->isa_stmt && obj) {
        ecs_parser_error(name, expr, column, 
            "invalid object in inheritance statement");
        return -1;
    }

    if (state->using_stmt && (obj || subj)) {
        ecs_parser_error(name, expr, column, 
            "invalid predicate/object in using statement");
        return -1;
    }

    if (state->isa_stmt) {
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

        pred_as_subj = false;
    } else {
        if (!obj) {
            /* If no subject or object were provided, use predicate as subj 
             * unless the expression explictly excluded the subject */
            if (pred_as_subj) {
                state->last_subject = pred;
                subj = pred;
            } else {
                state->last_predicate = pred;
                pred_as_subj = false;
            }
        } else {
            state->last_predicate = pred;
            state->last_object = obj;
            pred_as_subj = false;
        }
    }

    /* If this is a with clause (the list of entities between 'with' and scope
     * open), add subject to the array of with frames */
    if (state->with_stmt) {
        ecs_assert(pred != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_id_t id;

        if (obj) {
            id = ecs_pair(pred, obj);
        } else {
            id = pred;
        }

        state->with[state->with_frame ++] = id;
    
    } else if (state->using_stmt) {
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

    /* If an id was provided by itself, add default scope type to it */
    ecs_entity_t default_scope_type = state->default_scope_type[state->sp];
    if (pred_as_subj && default_scope_type) {
        ecs_add_id(world, subj, default_scope_type);
    }

    /* If annotations preceded the statement, append */
    if (state->annot_count) {
        if (!subj) {
            ecs_parser_error(name, expr, column, 
                "missing subject for annotations");
            return -1;
        }

        plecs_apply_annotations(world, subj, state);
    }

    return 0;
}

static
const char* plecs_parse_inherit_stmt(
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    if (state->isa_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "cannot nest inheritance");
        return NULL;
    }

    if (!state->last_subject) {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing entity to assign inheritance to");
        return NULL;
    }
    
    state->isa_stmt = true;
    state->assign_to = state->last_subject;

    return ptr;
}

static
const char* plecs_parse_assign_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    (void)world;
    
    if (!state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr,
            "unexpected value outside of assignment statement");
        return NULL;
    }

    ecs_id_t assign_id = state->last_assign_id;
    if (!assign_id) {
        ecs_parser_error(name, expr, ptr - expr,
            "missing type for assignment statement");
        return NULL;
    }

#ifndef FLECS_EXPR
    ecs_parser_error(name, expr, ptr - expr,
        "cannot parse value, missing FLECS_EXPR addon");
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
            "invalid assignment, '%s' is not a type", id_str);
        ecs_os_free(id_str);
        return NULL;
    }

    void *value_ptr = ecs_get_mut_id(world, assign_to, assign_id);

    ptr = ecs_parse_expr(world, ptr, type, value_ptr, 
        &(ecs_parse_expr_desc_t){
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

    return ptr;
}

static
const char* plecs_parse_assign_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    (void)world;

    state->isa_stmt = false;

    /* Component scope (add components to entity) */
    if (!state->last_subject) {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing entity to assign to");
        return NULL;
    }

    if (state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid assign statement in assign statement");
        return NULL;
    }

    if (!state->scope_assign_stmt) {
        state->assign_to = state->last_subject;
    }

    state->assign_stmt = true;
    
    /* Assignment without a preceding component */
    if (ptr[0] == '{') {
        ecs_entity_t type = 0;

        if (state->scope_assign_stmt) {
            ecs_assert(state->assign_to == ecs_get_scope(world), 
                ECS_INTERNAL_ERROR, NULL);
        }

        /* If we're in a scope & last_subject is a type, assign to scope */
        if (ecs_get_scope(world) != 0) {
            type = ecs_get_typeid(world, state->last_subject);
            if (type != 0) {
                type = state->last_subject;
            }
        }

        /* If type hasn't been set yet, check if scope has default type */
        if (!type && !state->scope_assign_stmt) {
            type = state->default_scope_type[state->sp];
        }

        /* If no type has been found still, check if last with id is a type */
        if (!type && !state->scope_assign_stmt) {
            int32_t with_frame_count = state->with_frames[state->sp];
            if (with_frame_count) {
                type = state->with[with_frame_count - 1];
            }
        }

        if (!type) {
            ecs_parser_error(name, expr, ptr - expr, 
                "missing type for assignment");
            return NULL;
        }

        state->last_assign_id = type;
    }

    return ptr;
}

static
const char* plecs_parse_using_stmt(
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    if (state->isa_stmt || state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid usage of using keyword");
        return NULL;
    }

    /* Add following expressions to using list */
    state->using_stmt = true;

    return ptr + 5;
}

static
const char* plecs_parse_with_stmt(
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    if (state->isa_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid with after inheritance");
        return NULL;
    }

    if (state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid with in assign_stmt");
        return NULL;
    }

    /* Add following expressions to with list */
    state->with_stmt = true;
    return ptr + 5;
}

static
const char* plecs_parse_scope_open(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    state->isa_stmt = false;

    if (state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid scope in assign_stmt");
        return NULL;
    }

    state->sp ++;

    ecs_entity_t scope = 0;
    ecs_entity_t default_scope_type = 0;

    if (!state->with_stmt) {
        if (state->last_subject) {
            scope = state->last_subject;
            ecs_set_scope(world, state->last_subject);

            /* Check if scope has a default child component */
            ecs_entity_t def_type_src = ecs_get_object_for_id(world, scope, 
                0, ecs_pair(EcsDefaultChildComponent, EcsWildcard));

            if (def_type_src) {
                default_scope_type = ecs_get_object(
                    world, def_type_src, EcsDefaultChildComponent, 0);
            }
        } else {
            if (state->last_object) {
                scope = ecs_pair(
                    state->last_predicate, state->last_object);
                ecs_set_with(world, scope);
            } else {
                if (state->last_predicate) {
                    scope = ecs_pair(EcsChildOf, state->last_predicate);
                }
                ecs_set_scope(world, state->last_predicate);
            }
        }

        state->scope[state->sp] = scope;
        state->default_scope_type[state->sp] = default_scope_type;
    } else {
        state->scope[state->sp] = state->scope[state->sp - 1];
        state->default_scope_type[state->sp] = 
            state->default_scope_type[state->sp - 1];
    }

    state->using_frames[state->sp] = state->using_frame;
    state->with_frames[state->sp] = state->with_frame;
    state->with_stmt = false;

    return ptr;
}

static
const char* plecs_parse_scope_close(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state) 
{
    if (state->isa_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid '}' after inheritance statement");
        return NULL;
    }

    if (state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unfinished assignment before }");
        return NULL;
    }

    state->scope[state->sp] = 0;
    state->default_scope_type[state->sp] = 0;
    state->sp --;

    if (state->sp < 0) {
        ecs_parser_error(name, expr, ptr - expr, "invalid } without a {");
        return NULL;
    }

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
    state->assign_stmt = false;

    return ptr;
}

static
const char *plecs_parse_plecs_term(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    ecs_term_t term = {0};
    ecs_entity_t scope = ecs_get_scope(world);

    /* If first character is a (, this should be interpreted as an id assigned
     * to the current scope if:
     * - this is not already an assignment: "Foo = (Hello, World)" 
     * - this is in a scope
     */
    bool scope_assignment = (ptr[0] == '(') && !state->assign_stmt && scope != 0;

    ptr = ecs_parse_term(world, name, expr, ptr, &term);
    if (!ptr) {
        return NULL;
    }

    if (!ecs_term_is_initialized(&term)) {
        ecs_parser_error(name, expr, ptr - expr, "expected identifier");
        return NULL; /* No term found */
    }

    /* Lookahead to check if this is an implicit scope assignment (no parens) */
    if (ptr[0] == '=') {
        const char *tptr = ecs_parse_fluff(ptr + 1, NULL);
        if (tptr[0] == '{') {
            ecs_entity_t pred = plecs_lookup(
                world, term.pred.name, state, 0, false);
            ecs_entity_t obj = plecs_lookup(
                world, term.obj.name, state, pred, false);
            ecs_id_t id = 0;
            if (pred && obj) {
                id = ecs_pair(pred, obj);
            } else if (pred) {
                id = pred;
            }

            if (id && (ecs_get_typeid(world, id) != 0)) {
                scope_assignment = true;
            }
        } 
    }

    bool prev = state->assign_stmt;
    if (scope_assignment) {
        state->assign_stmt = true;
        state->assign_to = scope;
    }
    if (plecs_create_term(world, &term, name, expr, (ptr - expr), state)) {
        ecs_term_fini(&term);
        return NULL; /* Failed to create term */
    }
    if (scope_assignment) {
        state->last_subject = state->last_assign_id;
        state->scope_assign_stmt = true;
    }
    state->assign_stmt = prev;

    ecs_term_fini(&term);

    return ptr;
}

static
const char* plecs_parse_annotation(
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    do {
        if(state->annot_count >= STACK_MAX_SIZE) {
            ecs_parser_error(name, expr, ptr - expr, 
                "max number of annotations reached");
            return NULL;
        }

        char ch;
        const char *start = ptr;
        for (; (ch = *ptr) && ch != '\n'; ptr ++) { }

        int32_t len = (int32_t)(ptr - start);
        char *annot = ecs_os_malloc_n(char, len + 1);
        ecs_os_memcpy_n(annot, start, char, len);
        annot[len] = '\0';

        state->annot[state->annot_count] = annot;
        state->annot_count ++;

        ptr = ecs_parse_fluff(ptr, NULL);
    } while (ptr[0] == '@');

    return ptr;
}

static
void plecs_clear_annotations(
    plecs_state_t *state)
{
    int32_t i, count = state->annot_count;
    for (i = 0; i < count; i ++) {
        ecs_os_free(state->annot[i]);
    }
    state->annot_count = 0;
}

static
const char* plecs_parse_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    state->assign_stmt = false;
    state->scope_assign_stmt = false;
    state->isa_stmt = false;
    state->with_stmt = false;
    state->using_stmt = false;
    state->last_subject = 0;
    state->last_predicate = 0;
    state->last_object = 0;

    plecs_clear_annotations(state);

    ptr = ecs_parse_fluff(ptr, NULL);

    char ch = ptr[0];

    if (!ch) {
        goto done;
    } else if (ch == '{') {
        ptr = ecs_parse_fluff(ptr + 1, NULL);
        goto scope_open;
    } else if (ch == '}') {
        ptr = ecs_parse_fluff(ptr + 1, NULL);
        goto scope_close;
    } else if (ch == '(') {
        goto term_expr;
    } else if (ch == '@') {
        ptr = plecs_parse_annotation(name, expr, ptr, state);
        if (!ptr) goto error;
        goto term_expr;
    } else if (!ecs_os_strncmp(ptr, TOK_USING " ", 5)) {
        ptr = plecs_parse_using_stmt(name, expr, ptr, state);
        if (!ptr) goto error;
        goto term_expr;
    } else if (!ecs_os_strncmp(ptr, TOK_WITH " ", 5)) {
        ptr = plecs_parse_with_stmt(name, expr, ptr, state);
        if (!ptr) goto error;
        goto term_expr;
    } else {
        goto term_expr;
    }

term_expr:
    if (!ptr[0]) {
        goto done;
    }

    if (!(ptr = plecs_parse_plecs_term(world, name, ptr, ptr, state))) {
        goto error;
    }

    ptr = ecs_parse_fluff(ptr, NULL);

    if (ptr[0] == '{' && !isspace(ptr[-1])) {
        /* A '{' directly after an identifier (no whitespace) is a literal */
        goto assign_expr;
    }

    if (!state->using_stmt) {
        if (ptr[0] == ':') {
            ptr = ecs_parse_fluff(ptr + 1, NULL);
            goto inherit_stmt;
        } else if (ptr[0] == '=') {
            ptr = ecs_parse_fluff(ptr + 1, NULL);
            goto assign_stmt;
        } else if (ptr[0] == ',') {
            ptr = ecs_parse_fluff(ptr + 1, NULL);
            goto term_expr;
        } else if (ptr[0] == '{') {
            state->assign_stmt = false;
            ptr = ecs_parse_fluff(ptr + 1, NULL);
            goto scope_open;
        }
    }

    state->assign_stmt = false;
    goto done;

inherit_stmt:
    ptr = plecs_parse_inherit_stmt(name, expr, ptr, state);
    if (!ptr) goto error;

    /* Expect base identifier */
    goto term_expr;

assign_stmt:
    ptr = plecs_parse_assign_stmt(world, name, expr, ptr, state);
    if (!ptr) goto error;

    ptr = ecs_parse_fluff(ptr, NULL);

    /* Assignment without a preceding component */
    if (ptr[0] == '{') {
        goto assign_expr;
    }

    /* Expect component identifiers */
    goto term_expr;

assign_expr:
    ptr = plecs_parse_assign_expr(world, name, expr, ptr, state);
    if (!ptr) goto error;

    ptr = ecs_parse_fluff(ptr, NULL);
    if (ptr[0] == ',') {
        ptr ++;
        goto term_expr;
    } else if (ptr[0] == '{') {
        state->assign_stmt = false;
        ptr ++;
        goto scope_open;
    } else {
        state->assign_stmt = false;
        goto done;
    }

scope_open:
    ptr = plecs_parse_scope_open(world, name, expr, ptr, state);
    if (!ptr) goto error;
    goto done;

scope_close:
    ptr = plecs_parse_scope_close(world, name, expr, ptr, state);
    if (!ptr) goto error;
    goto done;

done:
    return ptr;
error:
    return NULL;
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

    state.scope[0] = 0;
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_entity_t prev_with = ecs_set_with(world, 0);

    do {
        expr = ptr = plecs_parse_stmt(world, name, expr, ptr, &state);
        if (!ptr) {
            goto error;
        }

        if (!ptr[0]) {
            break; /* End of expression */
        }
    } while (true);

    ecs_set_scope(world, prev_scope);
    ecs_set_with(world, prev_with);

    plecs_clear_annotations(&state);

    if (state.sp != 0) {
        ecs_parser_error(name, expr, 0, "missing end of scope");
        goto error;
    }

    if (state.assign_stmt) {
        ecs_parser_error(name, expr, 0, "unfinished assignment");
        goto error;
    }

    if (state.errors) {
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
