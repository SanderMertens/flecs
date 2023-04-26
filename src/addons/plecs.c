/**
 * @file addons/plecs.c
 * @brief Plecs addon.
 */

#include "flecs.h"

#ifdef FLECS_PLECS

ECS_COMPONENT_DECLARE(EcsScript);

#include "../private_api.h"
#include <ctype.h>

#define TOK_NEWLINE '\n'
#define TOK_USING "using"
#define TOK_MODULE "module"
#define TOK_WITH "with"
#define TOK_CONST "const"
#define TOK_PROP "prop"
#define TOK_ASSEMBLY "assembly"

#define STACK_MAX_SIZE (64)

typedef struct {
    ecs_value_t value;
    bool owned;
} plecs_with_value_t;

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
    plecs_with_value_t with_value_frames[STACK_MAX_SIZE];
    int32_t using_frames[STACK_MAX_SIZE];
    int32_t sp;
    int32_t with_frame;
    int32_t using_frame;
    ecs_entity_t global_with;
    ecs_entity_t assembly;
    const char *assembly_start, *assembly_stop;

    char *annot[STACK_MAX_SIZE];
    int32_t annot_count;

    ecs_vars_t vars;
    char var_name[256];
    ecs_entity_t var_type;

    bool with_stmt;
    bool scope_assign_stmt;
    bool assign_stmt;
    bool assembly_stmt;
    bool assembly_instance;
    bool isa_stmt;
    bool decl_stmt;
    bool decl_type;
    bool var_stmt;
    bool var_is_prop;
    bool is_module;

    int32_t errors;
} plecs_state_t;

static
int flecs_plecs_parse(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_vars_t *vars,
    ecs_entity_t script,
    ecs_entity_t instance);

static void flecs_dtor_script(EcsScript *ptr) {
    ecs_os_free(ptr->script);
    ecs_vec_fini_t(NULL, &ptr->using_, ecs_entity_t);

    int i, count = ptr->prop_defaults.count;
    ecs_value_t *values = ptr->prop_defaults.array;
    for (i = 0; i < count; i ++) {
        ecs_value_free(ptr->world, values[i].type, values[i].ptr);
    }

    ecs_vec_fini_t(NULL, &ptr->prop_defaults, ecs_value_t);
}

static
ECS_MOVE(EcsScript, dst, src, {
    flecs_dtor_script(dst);
    dst->using_ = src->using_;
    dst->prop_defaults = src->prop_defaults;
    dst->script = src->script;
    dst->world = src->world;
    ecs_os_zeromem(&src->using_);
    ecs_os_zeromem(&src->prop_defaults);
    src->script = NULL;
    src->world = NULL;
})

static
ECS_DTOR(EcsScript, ptr, {
    flecs_dtor_script(ptr);
})

/* Assembly ctor to initialize with default property values */
static
void flecs_assembly_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_entity_t assembly = ti->component;
    const EcsStruct *st = ecs_get(world, assembly, EcsStruct);

    if (!st) {
        ecs_err("assembly '%s' is not a struct, cannot construct", ti->name);
        return;
    }

    const EcsScript *script = ecs_get(world, assembly, EcsScript);
    if (!script) {
        ecs_err("assembly '%s' is not a script, cannot construct", ti->name);
        return;
    }

    if (st->members.count != script->prop_defaults.count) {
        ecs_err("number of props (%d) of assembly '%s' does not match members"
            " (%d), cannot construct", script->prop_defaults.count, 
                ti->name, st->members.count);
        return;
    }

    const ecs_member_t *members = st->members.array;
    int32_t i, m, member_count = st->members.count;
    ecs_value_t *values = script->prop_defaults.array;
    for (m = 0; m < member_count; m ++) {
        const ecs_member_t *member = &members[m];
        ecs_value_t *value = &values[m];
        const ecs_type_info_t *mti = ecs_get_type_info(world, member->type);
        if (!mti) {
            ecs_err("failed to get type info for prop '%s' of assembly '%s'",
                member->name, ti->name);
            return;
        }

        for (i = 0; i < count; i ++) {
            void *el = ECS_ELEM(ptr, ti->size, i);
            ecs_value_copy_w_type_info(world, mti, 
                ECS_OFFSET(el, member->offset), value->ptr);
        }
    }
}

/* Assembly on_set handler to update contents for new property values */
static
void flecs_assembly_on_set(
    ecs_iter_t *it)
{
    if (it->table->flags & EcsTableIsPrefab) {
        /* Don't instantiate assemblies for prefabs */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t assembly = ecs_field_id(it, 1);
    const char *name = ecs_get_name(world, assembly);
    ecs_record_t *r = ecs_record_find(world, assembly);

    const EcsComponent *ct = ecs_record_get(world, r, EcsComponent);
    ecs_get(world, assembly, EcsComponent);
    if (!ct) {
        ecs_err("assembly '%s' is not a component", name);
        return;
    }

    const EcsStruct *st = ecs_record_get(world, r, EcsStruct);
    if (!st) {
        ecs_err("assembly '%s' is not a struct", name);
        return;
    }

    const EcsScript *script = ecs_record_get(world, r, EcsScript);
    if (!script) {
        ecs_err("assembly '%s' is missing a script", name);
        return;
    }

    void *data = ecs_field_w_size(it, flecs_ito(size_t, ct->size), 1);

    int32_t i, m;
    for (i = 0; i < it->count; i ++) {
        /* Create variables to hold assembly properties */
        ecs_vars_t vars = {0};
        ecs_vars_init(world, &vars);

        /* Populate properties from assembly members */
        const ecs_member_t *members = st->members.array;
        for (m = 0; m < st->members.count; m ++) {
            const ecs_member_t *member = &members[m];

            ecs_value_t v = {0}; /* Prevent allocating value */
            ecs_expr_var_t *var = ecs_vars_declare_w_value(
                &vars, member->name, &v);
            if (var == NULL) {
                ecs_err("could not create prop '%s' for assembly '%s'", 
                    member->name, name);
                break;
            }

            /* Assign assembly property from assembly instance */
            var->value.type = member->type;
            var->value.ptr = ECS_OFFSET(data, member->offset);
            var->owned = false;
        }

        /* Update script with new code/properties */
        ecs_entity_t instance = it->entities[i];
        ecs_script_update(world, assembly, instance, script->script, &vars);
        ecs_vars_fini(&vars);

        if (ecs_record_has_id(world, r, EcsFlatten)) {
            ecs_flatten(it->real_world, ecs_childof(instance), NULL);
        }

        data = ECS_OFFSET(data, ct->size);
    }
}

/* Delete contents of assembly instance */
static
void flecs_assembly_on_remove(
    ecs_iter_t *it)
{
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t instance = it->entities[i];
        ecs_script_clear(it->world, 0, instance);
    }
}

/* Set default property values on assembly Script component */
static
int flecs_assembly_init_defaults(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t assembly,
    EcsScript *script,
    plecs_state_t *state)
{
    const EcsStruct *st = ecs_get(world, assembly, EcsStruct);
    int32_t i, count = st->members.count;
    const ecs_member_t *members = st->members.array;

    ecs_vec_init_t(NULL, &script->prop_defaults, ecs_value_t, count);

    for (i = 0; i < count; i ++) {
        const ecs_member_t *member = &members[i];
        ecs_expr_var_t *var = ecs_vars_lookup(&state->vars, member->name);
        if (!var) {
            char *assembly_name = ecs_get_fullpath(world, assembly);
            ecs_parser_error(name, expr, ptr - expr, 
                "missing property '%s' for assembly '%s'", 
                    member->name, assembly_name);
            ecs_os_free(assembly_name);
            return -1;
        }

        if (member->type != var->value.type) {
            char *assembly_name = ecs_get_fullpath(world, assembly);
            ecs_parser_error(name, expr, ptr - expr, 
                "property '%s' for assembly '%s' has mismatching type", 
                    member->name, assembly_name);
            ecs_os_free(assembly_name);
            return -1;
        }

        ecs_value_t *pv = ecs_vec_append_t(NULL, 
            &script->prop_defaults, ecs_value_t);
        pv->type = member->type;
        pv->ptr = var->value.ptr;
        var->owned = false; /* Transfer ownership */
    }

    return 0;
}

/* Create new assembly */
static
int flecs_assembly_create(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t assembly,
    char *script_code,
    plecs_state_t *state)
{
    const EcsStruct *st = ecs_get(world, assembly, EcsStruct);
    if (!st || !st->members.count) {
        char *assembly_name = ecs_get_fullpath(world, assembly);
        ecs_parser_error(name, expr, ptr - expr, 
            "assembly '%s' has no properties", assembly_name);
        ecs_os_free(assembly_name);
        ecs_os_free(script_code);
        return -1;
    }

    ecs_add_id(world, assembly, EcsAlwaysOverride);

    EcsScript *script = ecs_get_mut(world, assembly, EcsScript);
    flecs_dtor_script(script);
    script->world = world;
    script->script = script_code;
    ecs_vec_reset_t(NULL, &script->using_, ecs_entity_t);

    ecs_entity_t scope = ecs_get_scope(world);
    if (scope && (scope = ecs_get_target(world, scope, EcsChildOf, 0))) {
        ecs_vec_append_t(NULL, &script->using_, ecs_entity_t)[0] = scope;
    }

    int i, count = state->using_frame;
    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(NULL, &script->using_, ecs_entity_t)[0] = 
            state->using[i];
    }

    if (flecs_assembly_init_defaults(
        world, name, expr, ptr, assembly, script, state)) 
    {
        return -1;    
    }

    ecs_modified(world, assembly, EcsScript);

    ecs_set_hooks_id(world, assembly, &(ecs_type_hooks_t) {
        .ctor = flecs_assembly_ctor,
        .on_set = flecs_assembly_on_set,
        .on_remove = flecs_assembly_on_remove,
        .ctx = world
    });

    return 0;
}

/* Parser */

static
bool plecs_is_newline_comment(
    const char *ptr)
{
    if (ptr[0] == '/' && ptr[1] == '/') {
        return true;
    }
    return false;
}

static
const char* plecs_parse_fluff(
    const char *ptr)
{
    do {
        /* Skip whitespaces before checking for a comment */
        ptr = ecs_parse_ws(ptr);

        /* Newline comment, skip until newline character */
        if (plecs_is_newline_comment(ptr)) {
            ptr += 2;

            while (ptr[0] && ptr[0] != TOK_NEWLINE) {
                ptr ++;
            }
        }

        /* If a newline character is found, skip it */
        if (ptr[0] == TOK_NEWLINE) {
            ptr ++;
        }

    } while (isspace(ptr[0]) || plecs_is_newline_comment(ptr));

    return ptr;
}

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
                oneof = ecs_get_target(world, rel, EcsOneOf, 0);
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
static
ecs_entity_t plecs_lookup_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx)
{
    return plecs_lookup(world, path, ctx, 0, false);
}

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

    ecs_entity_t e = 0;
    bool is_anonymous = !ecs_os_strcmp(path, "_");
    bool is_new = false;
    if (is_anonymous) {
        path = NULL;
        e = ecs_new_id(world);
        is_new = true;
    }

    if (!e) {
        e = plecs_lookup(world, path, state, rel, is_subject);
    }

    if (!e) {
        is_new = true;
        if (rel && flecs_get_oneof(world, rel)) {
            /* If relationship has oneof and entity was not found, don't proceed
             * with creating an entity as this can cause asserts later on */
            char *relstr = ecs_get_fullpath(world, rel);
            ecs_parser_error(state->name, 0, 0, 
                "invalid identifier '%s' for relationship '%s'", path, relstr);
            ecs_os_free(relstr);
            return 0;
        }

        ecs_entity_t prev_scope = 0;
        ecs_entity_t prev_with = 0;
        if (!is_subject) {
            /* Don't apply scope/with for non-subject entities */
            prev_scope = ecs_set_scope(world, 0);
            prev_with = ecs_set_with(world, 0);
        }

        e = ecs_add_path(world, e, 0, path);
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

        if (prev_scope) {
            ecs_set_scope(world, prev_scope);
        }
        if (prev_with) {
            ecs_set_with(world, prev_with);
        }
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

    if (is_new) {
        if (state->assembly && !state->assembly_instance) {
            ecs_add_id(world, e, EcsPrefab);
        }

        if (state->global_with) {
            ecs_add_id(world, e, state->global_with);
        }
    }

    return e;
}

static
bool plecs_pred_is_subj(
    ecs_term_t *term,
    plecs_state_t *state)
{
    if (term->src.name != NULL) {
        return false;
    }
    if (term->second.name != NULL) {
        return false;
    }
    if (ecs_term_match_0(term)) {
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
    if (state->decl_type) {
        return false;
    }

    return true;
}

/* Set masks aren't useful in plecs, so translate them back to entity names */
static
const char* plecs_set_mask_to_name(
    ecs_flags32_t flags) 
{
    flags &= EcsTraverseFlags;
    if (flags == EcsSelf) {
        return "self";
    } else if (flags == EcsUp) {
        return "up";
    } else if (flags == EcsDown) {
        return "down";
    } else if (flags == EcsCascade || flags == (EcsUp|EcsCascade)) {
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
    annot = (char*)ecs_parse_ws(annot);
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

    const char *pred_name = term->first.name;
    const char *subj_name = term->src.name;
    const char *obj_name = term->second.name;

    if (!subj_name) {
        subj_name = plecs_set_mask_to_name(term->src.flags);
    }
    if (!obj_name) {
        obj_name = plecs_set_mask_to_name(term->second.flags);
    }

    if (!ecs_term_id_is_set(&term->first)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    if (state->assign_stmt && !ecs_term_match_this(term)) {
        ecs_parser_error(name, expr, column, 
            "invalid statement in assign statement");
        return -1;
    }

    bool pred_as_subj = plecs_pred_is_subj(term, state);
    ecs_entity_t pred = plecs_ensure_entity(world, state, pred_name, 0, pred_as_subj); 
    ecs_entity_t subj = plecs_ensure_entity(world, state, subj_name, pred, true);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->second)) {
        obj = plecs_ensure_entity(world, state, obj_name, pred,
            !state->assign_stmt && !state->with_stmt);
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

    if (state->isa_stmt) {
        pred = ecs_pair(EcsIsA, pred);
    }

    if (subj == EcsVariable) {
        subj = pred;
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

    } else {
        if (subj) {
            int32_t i, frame_count = state->with_frames[state->sp];
            for (i = 0; i < frame_count; i ++) {
                ecs_id_t id = state->with[i];
                plecs_with_value_t *v = &state->with_value_frames[i];
                if (v->value.type) {
                    void *ptr = ecs_get_mut_id(world, subj, id);
                    ecs_value_copy(world, v->value.type, ptr, v->value.ptr);
                    ecs_modified_id(world, subj, id);
                } else {
                    ecs_add_id(world, subj, id);
                }
            }
        }
    }

    /* If an id was provided by itself, add default scope type to it */
    ecs_entity_t default_scope_type = state->default_scope_type[state->sp];
    if (pred_as_subj && default_scope_type) {
        ecs_add_id(world, subj, default_scope_type);
    }

    /* If annotations preceded the statement, append */
    if (!state->decl_type && state->annot_count) {
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
const char* plecs_parse_assign_var_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state,
    ecs_expr_var_t *var)
{
    ecs_value_t value = {0};

    if (state->last_assign_id) {
        value.type = state->last_assign_id;
        value.ptr = ecs_value_new(world, state->last_assign_id);
        if (!var && state->assembly_instance) {
            var = ecs_vars_lookup(&state->vars, state->var_name);
        }
    }

    ptr = ecs_parse_expr(world, ptr, &value, 
        &(ecs_parse_expr_desc_t){
            .name = name,
            .expr = expr,
            .lookup_action = plecs_lookup_action,
            .lookup_ctx = state,
            .vars = &state->vars
        });
    if (!ptr) {
        if (state->last_assign_id) {
            ecs_value_free(world, value.type, value.ptr);
        }
        goto error;
    }

    if (var) {
        bool ignore = state->var_is_prop && state->assembly_instance;
        if (!ignore) {
            if (var->value.ptr) {
                ecs_value_free(world, var->value.type, var->value.ptr);
                var->value.ptr = value.ptr;
                var->value.type = value.type;
            }
        } else {
            ecs_value_free(world, value.type, value.ptr);
        }
    } else {
        var = ecs_vars_declare_w_value(
            &state->vars, state->var_name, &value);
        if (!var) {
            goto error;
        }
    }

    state->var_is_prop = false;
    return ptr;
error:
    return NULL;
}

static
const char* plecs_parse_assign_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state,
    ecs_expr_var_t *var) 
{
    (void)world;
    
    if (state->var_stmt) {
        return plecs_parse_assign_var_expr(world, name, expr, ptr, state, var);
    }

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

    if (assign_to == EcsVariable) {
        assign_to = type;
    }

    void *value_ptr = ecs_get_mut_id(world, assign_to, assign_id);

    ptr = ecs_parse_expr(world, ptr, &(ecs_value_t){type, value_ptr}, 
        &(ecs_parse_expr_desc_t){
            .name = name,
            .expr = expr,
            .lookup_action = plecs_lookup_action,
            .lookup_ctx = state,
            .vars = &state->vars
        });
    if (!ptr) {
        return NULL;
    }

    ecs_modified_id(world, assign_to, assign_id);

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
    if (!state->assign_to) {
        if (!state->last_subject) {
            ecs_parser_error(name, expr, ptr - expr, 
                "missing entity to assign to");
            return NULL;
        }
        state->assign_to = state->last_subject;
    }

    if (state->assign_stmt) {
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid assign statement in assign statement");
        return NULL;
    }

    state->assign_stmt = true;
    
    /* Assignment without a preceding component */
    if (ptr[0] == '{') {
        ecs_entity_t type = 0;

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
const char* plecs_parse_assign_with_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    int32_t with_frame = state->with_frame - 1;
    if (with_frame < 0) {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing type in with value");
        return NULL;
    }

    ecs_id_t id = state->with[with_frame];
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    const ecs_type_info_t *ti = idr->type_info;
    if (!ti) {
        char *typename = ecs_id_str(world, id);
        ecs_parser_error(name, expr, ptr - expr, 
            "id '%s' in with value is not a type", typename);
        ecs_os_free(typename);
        return NULL;
    }

    plecs_with_value_t *v = &state->with_value_frames[with_frame];
    v->value.type = ti->component;
    v->value.ptr = ecs_value_new(world, ti->component);
    v->owned = true;
    if (!v->value.ptr) {
        char *typename = ecs_id_str(world, id);
        ecs_parser_error(name, expr, ptr - expr, 
            "failed to create value for '%s'", typename);
        ecs_os_free(typename);
        return NULL;
    }

    ptr = ecs_parse_expr(world, ptr, &v->value,
        &(ecs_parse_expr_desc_t){
            .name = name,
            .expr = expr,
            .lookup_action = plecs_lookup_action,
            .lookup_ctx = state,
            .vars = &state->vars
        });
    if (!ptr) {
        return NULL;
    }

    return ptr;
}

static
const char* plecs_parse_assign_with_var(
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    ecs_assert(ptr[0] == '$', ECS_INTERNAL_ERROR, NULL);
    ecs_assert(state->with_stmt, ECS_INTERNAL_ERROR, NULL);

    char var_name[ECS_MAX_TOKEN_SIZE];
    const char *tmp = ptr;
    ptr = ecs_parse_token(name, expr, ptr + 1, var_name, 0);
    if (!ptr) {
        ecs_parser_error(name, expr, tmp - expr, 
            "unresolved variable '%s'", var_name);
        return NULL;
    }

    ecs_expr_var_t *var = ecs_vars_lookup(&state->vars, var_name);
    if (!var) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unresolved variable '%s'", var_name);
        return NULL;
    }

    int32_t with_frame = state->with_frame;
    state->with[with_frame] = var->value.type;
    state->with_value_frames[with_frame].value = var->value;
    state->with_value_frames[with_frame].owned = false;
    state->with_frame ++;

    return ptr;
}

static
const char* plecs_parse_var_as_component(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    ecs_assert(ptr[0] == '$', ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!state->var_stmt, ECS_INTERNAL_ERROR, NULL);
    char var_name[ECS_MAX_TOKEN_SIZE];
    const char *tmp = ptr;
    ptr = ecs_parse_token(name, expr, ptr + 1, var_name, 0);
    if (!ptr) {
        ecs_parser_error(name, expr, tmp - expr, 
            "unresolved variable '%s'", var_name);
        return NULL;
    }

    ecs_expr_var_t *var = ecs_vars_lookup(&state->vars, var_name);
    if (!var) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unresolved variable '%s'", var_name);
        return NULL;
    }

    if (!state->assign_to) {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing lvalue for variable assignment '%s'", var_name);
        return NULL;
    }

    /* Use type of variable as component */
    ecs_entity_t type = var->value.type;
    ecs_entity_t assign_to = state->assign_to;
    if (!assign_to) {
        assign_to = state->last_subject;
    }

    void *dst = ecs_get_mut_id(world, assign_to, type);
    if (!dst) {
        char *type_name = ecs_get_fullpath(world, type);
        ecs_parser_error(name, expr, ptr - expr, 
            "failed to obtain component for type '%s' of variable '%s'",    
                type_name, var_name);
        ecs_os_free(type_name);
        return NULL;
    }

    if (ecs_value_copy(world, type, dst, var->value.ptr)) {
        char *type_name = ecs_get_fullpath(world, type);
        ecs_parser_error(name, expr, ptr - expr, 
            "failed to copy value for variable '%s' of type '%s'",    
                var_name, type_name);
        ecs_os_free(type_name);
        return NULL;
    }

    ecs_modified_id(world, assign_to, type);

    return ptr;
}

static
void plecs_push_using(
    ecs_entity_t scope,
    plecs_state_t *state)
{
    for (int i = 0; i < state->using_frame; i ++) {
        if (state->using[i] == scope) {
            return;
        }
    }

    state->using[state->using_frame ++] = scope;
}

static
const char* plecs_parse_using_stmt(
    ecs_world_t *world,
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

    char using_path[ECS_MAX_TOKEN_SIZE];
    const char *tmp = ptr + 1;
    ptr = ecs_parse_token(name, expr, ptr + 5, using_path, 0);
    if (!ptr) {
        ecs_parser_error(name, expr, tmp - expr, 
            "expected identifier for using statement");
        return NULL;
    }

    ecs_size_t len = ecs_os_strlen(using_path);
    if (!len) {
        ecs_parser_error(name, expr, tmp - expr, 
            "missing identifier for using statement");
        return NULL;
    }

    /* Lookahead as * is not matched by parse_token */
    if (ptr[0] == '*') {
        using_path[len] = '*';
        using_path[len + 1] = '\0';
        len ++;
        ptr ++;
    }

    ecs_entity_t scope;
    if (len > 2 && !ecs_os_strcmp(&using_path[len - 2], ".*")) {
        using_path[len - 2] = '\0';
        scope = ecs_lookup_fullpath(world, using_path);
        if (!scope) {
            ecs_parser_error(name, expr, ptr - expr,
                "unresolved identifier '%s' in using statement", using_path);
            return NULL;
        }

        /* Add each child of the scope to using stack */
        ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ 
            .id = ecs_childof(scope) });
        while (ecs_term_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                plecs_push_using(it.entities[i], state);
            }
        }
    } else {
        scope = plecs_ensure_entity(world, state, using_path, 0, false);
        if (!scope) {
            ecs_parser_error(name, expr, ptr - expr,
                "unresolved identifier '%s' in using statement", using_path);
            return NULL;
        }

        plecs_push_using(scope, state);
    }

    state->using_frames[state->sp] = state->using_frame;
    return ptr;
}

static
const char* plecs_parse_module_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    const char *expr_start = ecs_parse_ws_eol(expr);
    if (expr_start != ptr) {
        ecs_parser_error(name, expr, ptr - expr, 
            "module must be first statement of script");
        return NULL;
    }

    char module_path[ECS_MAX_TOKEN_SIZE];
    const char *tmp = ptr + 1;
    ptr = ecs_parse_token(name, expr, ptr + 6, module_path, 0);
    if (!ptr) {
        ecs_parser_error(name, expr, tmp - expr, 
            "expected identifier for module statement");
        return NULL;
    }

    ecs_component_desc_t desc = {0};
    desc.entity = ecs_entity(world, { .name = module_path });
    ecs_entity_t module = ecs_module_init(world, NULL, &desc);
    if (!module) {
        return NULL;
    }

    state->is_module = true;
    state->sp ++;
    state->scope[state->sp] = module;
    ecs_set_scope(world, module);
    return ptr;
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
const char* plecs_parse_assembly_stmt(
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

    state->assembly_stmt = true;
    return ptr + 9;
}

static
const char* plecs_parse_var_type(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state,
    ecs_entity_t *type_out)
{
    char prop_type_name[ECS_MAX_TOKEN_SIZE];
    const char *tmp = ptr + 1;
    ptr = ecs_parse_token(name, expr, ptr + 1, prop_type_name, 0);
    if (!ptr) {
        ecs_parser_error(name, expr, tmp - expr, 
            "expected type for prop declaration");
        return NULL;
    }

    ecs_entity_t prop_type = plecs_lookup(world, prop_type_name, state, 0, false);
    if (!prop_type) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unresolved property type '%s'", prop_type_name);
        return NULL;
    }

    *type_out = prop_type;

    return ptr;
}

static
const char* plecs_parse_const_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    ptr = ecs_parse_token(name, expr, ptr + 5, state->var_name, 0);
    if (!ptr) {
        return NULL;
    }

    ptr = ecs_parse_ws(ptr);

    if (ptr[0] == ':') {
        ptr = plecs_parse_var_type(
            world, name, expr, ptr, state, &state->last_assign_id);
        if (!ptr) {
            return NULL;
        }

        ptr = ecs_parse_ws(ptr);
    }

    if (ptr[0] != '=') {
        ecs_parser_error(name, expr, ptr - expr, 
            "expected '=' after const declaration");
        return NULL;
    }

    state->var_stmt = true;
    return ptr + 1;
}

static
const char* plecs_parse_prop_stmt(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    plecs_state_t *state)
{
    char prop_name[ECS_MAX_TOKEN_SIZE];
    ptr = ecs_parse_token(name, expr, ptr + 5, prop_name, 0);
    if (!ptr) {
        return NULL;
    }

    ptr = ecs_parse_ws(ptr);

    if (ptr[0] != ':') {
        ecs_parser_error(name, expr, ptr - expr, 
            "expected ':' after prop declaration");
        return NULL;
    }

    ecs_entity_t prop_type;
    ptr = plecs_parse_var_type(world, name, expr, ptr, state, &prop_type);
    if (!ptr) {
        return NULL;
    }

    ecs_entity_t assembly = state->assembly;
    if (!assembly) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unexpected prop '%s' outside of assembly", prop_name);
        return NULL;
    }

    if (!state->assembly_instance) {
        ecs_entity_t prop_member = ecs_entity(world, {
            .name = prop_name,
            .add = { ecs_childof(assembly) }
        });

        if (!prop_member) {
            return NULL;
        }

        ecs_set(world, prop_member, EcsMember, {
            .type = prop_type
        });
    }

    if (ptr[0] != '=') {
        ecs_parser_error(name, expr, ptr - expr, 
            "expected '=' after prop type");
        return NULL;
    }

    ecs_os_strcpy(state->var_name, prop_name);
    state->last_assign_id = prop_type;
    state->var_stmt = true;
    state->var_is_prop = true;

    return plecs_parse_fluff(ptr + 1);
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
            ecs_entity_t def_type_src = ecs_get_target_for_id(world, scope, 
                0, ecs_pair(EcsDefaultChildComponent, EcsWildcard));

            if (def_type_src) {
                default_scope_type = ecs_get_target(
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

        if (state->assembly_stmt) {
            if (state->assembly) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "invalid nested assembly");
                return NULL;
            }
            state->assembly = scope;
            state->assembly_stmt = false;
            state->assembly_start = ptr;
        }
    } else {
        state->scope[state->sp] = state->scope[state->sp - 1];
        state->default_scope_type[state->sp] = 
            state->default_scope_type[state->sp - 1];
    }

    state->using_frames[state->sp] = state->using_frame;
    state->with_frames[state->sp] = state->with_frame;
    state->with_stmt = false;

    ecs_vars_push(&state->vars);

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

    ecs_entity_t cur = state->scope[state->sp], assembly = state->assembly;
    if (state->sp && (cur == state->scope[state->sp - 1])) {
        /* Previous scope is also from the assembly, not found the end yet */
        cur = 0;
    }
    if (cur && cur == assembly) {
        ecs_size_t assembly_len = flecs_ito(ecs_size_t, ptr - state->assembly_start);
        if (assembly_len) {
            assembly_len --;
            char *script = ecs_os_malloc_n(char, assembly_len + 1);
            ecs_os_memcpy(script, state->assembly_start, assembly_len);
            script[assembly_len] = '\0';
            state->assembly = 0;
            state->assembly_start = NULL;

            if (flecs_assembly_create(world, name, expr, ptr, assembly, script, state)) {
                return NULL;
            }
        } else {
            ecs_parser_error(name, expr, ptr - expr, "empty assembly");
            return NULL;
        }
    }

    state->scope[state->sp] = 0;
    state->default_scope_type[state->sp] = 0;
    state->sp --;

    if (state->sp < 0) {
        ecs_parser_error(name, expr, ptr - expr, "invalid } without a {");
        return NULL;
    }

    ecs_id_t id = state->scope[state->sp];
    if (!id || ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_set_with(world, id);
    }

    if (!id || !ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_set_scope(world, id);
    }

    int32_t i, prev_with = state->with_frames[state->sp];
    for (i = prev_with; i < state->with_frame; i ++) {
        plecs_with_value_t *v = &state->with_value_frames[i];
        if (!v->owned) {
            continue;
        }
        if (v->value.type) {
            ecs_value_free(world, v->value.type, v->value.ptr);
            v->value.type = 0;
            v->value.ptr = NULL;
            v->owned = false;
        }
    }

    state->with_frame = state->with_frames[state->sp];
    state->using_frame = state->using_frames[state->sp];
    state->last_subject = 0;
    state->assign_stmt = false;

    ecs_vars_pop(&state->vars);

    return plecs_parse_fluff(ptr + 1);
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
    ecs_entity_t decl_id = 0;
    if (state->decl_stmt) {
        decl_id = state->last_predicate;
    }

    ptr = ecs_parse_term(world, name, expr, ptr, &term);
    if (!ptr) {
        return NULL;
    }

    if (flecs_isident(ptr[0])) {
        state->decl_type = true;
    }

    if (!ecs_term_is_initialized(&term)) {
        ecs_parser_error(name, expr, ptr - expr, "expected identifier");
        return NULL; /* No term found */
    }

    if (plecs_create_term(world, &term, name, expr, (ptr - expr), state)) {
        ecs_term_fini(&term);
        return NULL; /* Failed to create term */
    }

    if (decl_id && state->last_subject) {
        ecs_add_id(world, state->last_subject, decl_id);
    }

    state->decl_type = false;

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

        ptr = plecs_parse_fluff(ptr);
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
    state->decl_stmt = false;
    state->var_stmt = false;
    state->last_subject = 0;
    state->last_predicate = 0;
    state->last_object = 0;
    state->assign_to = 0;
    state->last_assign_id = 0;

    plecs_clear_annotations(state);

    ptr = plecs_parse_fluff(ptr);

    char ch = ptr[0];

    if (!ch) {
        goto done;
    } else if (ch == '{') {
        ptr = plecs_parse_fluff(ptr + 1);
        goto scope_open;
    } else if (ch == '}') {
        goto scope_close;
    } else if (ch == '-') {
        ptr = plecs_parse_fluff(ptr + 1);
        state->assign_to = ecs_get_scope(world);
        state->scope_assign_stmt = true;
        goto assign_stmt;
    } else if (ch == '@') {
        ptr = plecs_parse_annotation(name, expr, ptr, state);
        if (!ptr) goto error;
        goto term_expr;
    } else if (!ecs_os_strncmp(ptr, TOK_USING " ", 5)) {
        ptr = plecs_parse_using_stmt(world, name, expr, ptr, state);
        if (!ptr) goto error;
        goto done;
    } else if (!ecs_os_strncmp(ptr, TOK_MODULE " ", 6)) {
        ptr = plecs_parse_module_stmt(world, name, expr, ptr, state);
        if (!ptr) goto error;
        goto done;
    } else if (!ecs_os_strncmp(ptr, TOK_WITH " ", 5)) {
        ptr = plecs_parse_with_stmt(name, expr, ptr, state);
        if (!ptr) goto error;
        goto term_expr;
    } else if (!ecs_os_strncmp(ptr, TOK_CONST " ", 6)) {
        ptr = plecs_parse_const_stmt(world, name, expr, ptr, state);
        if (!ptr) goto error;
        goto assign_expr;
    } else if (!ecs_os_strncmp(ptr, TOK_ASSEMBLY " ", 9)) {
        ptr = plecs_parse_assembly_stmt(name, expr, ptr, state);
        if (!ptr) goto error;
        goto decl_stmt;
    } else if (!ecs_os_strncmp(ptr, TOK_PROP " ", 5)) {
        ptr = plecs_parse_prop_stmt(world, name, expr, ptr, state);
        if (!ptr) goto error;
        goto assign_expr;
    } else {
        goto term_expr;
    }

term_expr:
    if (!ptr[0]) {
        goto done;
    }

    if (ptr[0] == '$' && !isspace(ptr[1])) {
        if (state->with_stmt) {
            ptr = plecs_parse_assign_with_var(name, expr, ptr, state);
            if (!ptr) {
                return NULL;
            }
        } else if (!state->var_stmt) {
            goto assign_var_as_component;
        }
    } else if (!(ptr = plecs_parse_plecs_term(world, name, ptr, ptr, state))) {
        goto error;
    }

    const char *tptr = ecs_parse_ws(ptr);
    if (flecs_isident(tptr[0])) {
        if (state->decl_stmt) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected ' ' in declaration statement");
            goto error;
        }
        ptr = tptr;
        goto decl_stmt;
    }

next_term:
    ptr = plecs_parse_fluff(ptr);

    if (ptr[0] == ':' && ptr[1] == '-') {
        ptr = plecs_parse_fluff(ptr + 2);
        goto assign_stmt;
    } else if (ptr[0] == ':') {
        ptr = plecs_parse_fluff(ptr + 1);
        goto inherit_stmt;
    } else if (ptr[0] == ',') {
        ptr = plecs_parse_fluff(ptr + 1);
        goto term_expr;
    } else if (ptr[0] == '{') {
        if (state->assign_stmt) {
            goto assign_expr;
        } else if (state->with_stmt && !isspace(ptr[-1])) {
            /* If this is a { in a with statement which directly follows a
                * non-whitespace character, the with id has a value */
            ptr = plecs_parse_assign_with_stmt(world, name, expr, ptr, state);
            if (!ptr) {
                goto error;
            }

            goto next_term;
        } else {
            ptr = plecs_parse_fluff(ptr + 1);
            goto scope_open;
        }
    }

    state->assign_stmt = false;
    goto done;

decl_stmt:
    state->decl_stmt = true;
    goto term_expr;

inherit_stmt:
    ptr = plecs_parse_inherit_stmt(name, expr, ptr, state);
    if (!ptr) goto error;

    /* Expect base identifier */
    goto term_expr;

assign_stmt:
    ptr = plecs_parse_assign_stmt(world, name, expr, ptr, state);
    if (!ptr) goto error;

    ptr = plecs_parse_fluff(ptr);

    /* Assignment without a preceding component */
    if (ptr[0] == '{') {
        goto assign_expr;
    }

    /* Expect component identifiers */
    goto term_expr;

assign_expr:
    ptr = plecs_parse_assign_expr(world, name, expr, ptr, state, NULL);
    if (!ptr) goto error;

    ptr = plecs_parse_fluff(ptr);
    if (ptr[0] == ',') {
        ptr ++;
        goto term_expr;
    } else if (ptr[0] == '{') {
        if (state->var_stmt) {
            ecs_expr_var_t *var = ecs_vars_lookup(&state->vars, state->var_name);
            if (var && var->value.type == ecs_id(ecs_entity_t)) {
                ecs_assert(var->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                /* The code contained an entity{...} variable assignment, use
                 * the assigned entity id as type for parsing the expression */
                state->last_assign_id = *(ecs_entity_t*)var->value.ptr;
                ptr = plecs_parse_assign_expr(world, name, expr, ptr, state, var);
                goto done;
            }
        }
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected '{' after assignment");
        goto error;
    }

    state->assign_stmt = false;
    state->assign_to = 0;
    goto done;

assign_var_as_component: {
    ptr = plecs_parse_var_as_component(world, name, expr, ptr, state);
    if (!ptr) {
        goto error;
    }
    state->assign_stmt = false;
    state->assign_to = 0;
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

static
int flecs_plecs_parse(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_vars_t *vars,
    ecs_entity_t script,
    ecs_entity_t instance)
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

    if (ECS_IS_PAIR(prev_with) && ECS_PAIR_FIRST(prev_with) == EcsChildOf) {
        ecs_set_scope(world, ECS_PAIR_SECOND(prev_with));
        state.scope[0] = ecs_pair_second(world, prev_with);
    } else {
        state.global_with = prev_with;
    }

    ecs_vars_init(world, &state.vars);

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        if (!s) {
            ecs_err("%s: provided script entity is not a script", name);
            goto error;
        }
        if (s && ecs_has(world, script, EcsStruct)) {
            state.assembly = script;
            state.assembly_instance = true;

            if (s->using_.count) {
                ecs_os_memcpy_n(state.using, s->using_.array, 
                    ecs_entity_t, s->using_.count);
                state.using_frame = s->using_.count;
                state.using_frames[0] = s->using_.count;
            }

            if (instance) {
                ecs_set_scope(world, instance);
            }
        }
    }

    if (vars) {
        state.vars.root.parent = vars->cur;
    }

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

    if (state.is_module) {
        state.sp --;
    }

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

    ecs_vars_fini(&state.vars);

    return 0;
error:
    ecs_vars_fini(&state.vars);
    ecs_set_scope(world, state.scope[0]);
    ecs_set_with(world, prev_with);
    ecs_term_fini(&term);
    return -1;
}

int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    return flecs_plecs_parse(world, name, expr, NULL, 0, 0);
}

static
char* flecs_load_from_file(
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

    return content;
error:
    ecs_os_free(content);
    return NULL;
}

int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename) 
{
    char *script = flecs_load_from_file(filename);
    if (!script) {
        return -1;
    }

    int result = ecs_plecs_from_str(world, filename, script);
    ecs_os_free(script);
    return result;
}

static
ecs_id_t flecs_script_tag(
    ecs_entity_t script,
    ecs_entity_t instance)
{
    if (!instance) {
        return ecs_pair_t(EcsScript, script);
    } else {
        return ecs_pair(EcsChildOf, instance);
    }
}

void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    ecs_delete_with(world, flecs_script_tag(script, instance));
}

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *script,
    ecs_vars_t *vars)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);

    int result = 0;
    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        ecs_assert(ecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_script_clear(world, e, instance);

    EcsScript *s = ecs_get_mut(world, e, EcsScript);
    if (!s->script || ecs_os_strcmp(s->script, script)) {
        s->script = ecs_os_strdup(script);
        ecs_modified(world, e, EcsScript);
    }

    ecs_entity_t prev = ecs_set_with(world, flecs_script_tag(e, instance));
    if (flecs_plecs_parse(world, ecs_get_name(world, e), script, vars, e, instance)) {
        ecs_delete_with(world, ecs_pair_t(EcsScript, e));
        result = -1;
    }
    ecs_set_with(world, prev);

    if (is_defer) {
        flecs_resume_readonly(real_world, &srs);
    }

    return result;
}

ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc)
{
    const char *script = NULL;
    ecs_entity_t e = desc->entity;
    
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!e) {
        if (desc->filename) {
            e = ecs_new_from_path_w_sep(world, 0, desc->filename, "/", NULL);
        } else {
            e = ecs_new_id(world);
        }
    }

    script = desc->str;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            goto error;
        }
    }

    if (ecs_script_update(world, e, 0, script, NULL)) {
        goto error;
    }

    if (script != desc->str) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free((char*)script);
    }

    return e;
error:
    if (script != desc->str) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free((char*)script);
    }
    if (!desc->entity) {
        ecs_delete(world, e);
    }
    return 0;
}

void FlecsScriptImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScript);
    ECS_IMPORT(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsScript);

    ecs_set_hooks(world, EcsScript, {
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsScript),
        .dtor = ecs_dtor(EcsScript)
    });

    ecs_add_id(world, ecs_id(EcsScript), EcsTag);

    ecs_struct(world, {
        .entity = ecs_id(EcsScript),
        .members = {
            { .name = "using", .type = ecs_vector(world, { 
                    .entity = ecs_entity(world, { .name = "UsingVector" }),
                    .type = ecs_id(ecs_entity_t)
                }),
                .count = 0
            },
            { .name = "script", .type = ecs_id(ecs_string_t), .count = 0 }
        }
    });
}

#endif
