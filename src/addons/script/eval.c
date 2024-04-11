#include "flecs.h"
#include "script.h"

typedef struct ecs_script_eval_visitor_t {
    ecs_script_visit_t base;
    ecs_world_t *world;
    ecs_entity_t module;
    ecs_entity_t parent;
    ecs_script_entity_t *entity;
    ecs_vec_t using;
    ecs_vec_t with;
    ecs_vec_t annot;
    ecs_entity_t with_relationship;
    int32_t with_relationship_sp;
    ecs_script_vars_t *vars;
    ecs_stack_t stack;
} ecs_script_eval_visitor_t;

static
void flecs_script_eval_error_(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = ecs_vasprintf(fmt, args);
    va_end(args);

    int32_t line = ecs_script_node_line_number(v->base.script, node);
    ecs_parser_error(v->base.script->name, NULL, 0, "%d: %s", line, msg);
    ecs_os_free(msg);
}

#define flecs_script_eval_error(v, node, ...)\
    flecs_script_eval_error_(v, (ecs_script_node_t*)node, __VA_ARGS__)

static
ecs_entity_t flecs_script_find_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t from,
    const char *path)
{
    if (from) {
        return ecs_lookup_path_w_sep(v->world, from, path, NULL, NULL, false);
    } else {
        int32_t i, using_count = ecs_vec_count(&v->using);
        if (using_count) {
            ecs_entity_t *using = ecs_vec_first(&v->using);
            for (i = using_count - 1; i >= 0; i --) {
                ecs_entity_t e = ecs_lookup_path_w_sep(
                    v->world, using[i], path, NULL, NULL, false);
                if (e) {
                    return e;
                }
            }
        }

        return ecs_lookup_path_w_sep(
            v->world, v->parent, path, NULL, NULL, true);
    }
}

static
ecs_entity_t flecs_script_find_entity_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx)
{
    (void)world;
    ecs_script_eval_visitor_t *v = ctx;
    return flecs_script_find_entity(v, 0, path);
}

static
int flecs_script_eval_id(
    ecs_script_eval_visitor_t *v,
    ecs_script_id_t *id)
{
    ecs_entity_t second_from = 0;

    ecs_entity_t first = flecs_script_find_entity(v, 0, id->first);
    if (!first) {
        first = ecs_entity(v->world, {
            .name = id->first,
            .parent = v->module
        });
        
        if (!first) {
            return -1;
        }
    } else if (id->second) {
        second_from = flecs_get_oneof(v->world, first);
    }

    if (id->second) {
        ecs_entity_t second = flecs_script_find_entity(
            v, second_from, id->second);
        if (!second) {
            second = ecs_entity(v->world, {
                .name = id->second,
                .parent = v->module
            });

            if (!second) {
                return -1;
            }
        }

        id->eval = ecs_pair(first, second);
    } else {
        id->eval = first;
    }

    return 0;
}

static
int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    const char *expr,
    ecs_value_t *value)
{
    ecs_parse_expr_desc_t desc = {
        .name = v->base.script->name,
        .expr = expr,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = v,
    };

    if (!ecs_parse_expr(v->world, expr, value, &desc)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_eval_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node)
{
    ecs_script_node_t *scope_parent = ecs_script_parent_node(v);
    ecs_entity_t prev_eval_parent = v->parent;

    v->vars = ecs_script_vars_push(
        v->vars, &v->stack, &v->base.script->allocator);

    if (scope_parent && (scope_parent->kind == EcsAstEntity)) {
        v->parent = ecs_script_node(entity, scope_parent)->eval;
    }

    int result = ecs_script_visit_scope(v, node);

    v->vars = ecs_script_vars_pop(v->vars);
    v->parent = prev_eval_parent;

    return result;
}

static
ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name)
{
    if (name || ecs_vec_count(&v->with)) {
        ecs_entity_desc_t desc = {0};
        desc.name = name;
        desc.parent = v->parent;
        desc.set = ecs_vec_first_t(&v->with, ecs_value_t);
        return ecs_entity_init(v->world, &desc);
    } else if (v->parent) {
        return ecs_new_w_pair(v->world, EcsChildOf, v->parent);
    } else {
        return ecs_new(v->world);
    }
}

static
int flecs_script_apply_annot(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity,
    ecs_script_annot_t *node)
{
    if (!ecs_os_strcmp(node->name, "name")) {
        ecs_doc_set_name(v->world, entity, node->expr);
    } else
    if (!ecs_os_strcmp(node->name, "brief")) {
        ecs_doc_set_brief(v->world, entity, node->expr);
    } else 
    if (!ecs_os_strcmp(node->name, "detail")) {
        ecs_doc_set_detail(v->world, entity, node->expr);
    } else
    if (!ecs_os_strcmp(node->name, "link")) {
        ecs_doc_set_link(v->world, entity, node->expr);
    } else
    if (!ecs_os_strcmp(node->name, "color")) {
        ecs_doc_set_color(v->world, entity, node->expr);
    } else {
        flecs_script_eval_error(v, node, "unknown annotation '%s'",
            node->name);
        return -1;
    }
    
    return 0;
}

static
int flecs_script_eval_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node)
{
    if (node->kind) {
        ecs_script_id_t id = {
            .first = node->kind
        };

        if (flecs_script_eval_id(v, &id)) {
            return -1;
        }

        node->eval_kind = id.eval;
    }

    node->eval = flecs_script_create_entity(v, node->name);
    node->parent = v->entity;

    ecs_script_entity_t *old_entity = v->entity;
    v->entity = node;

    if (node->eval_kind) {
        ecs_add_id(v->world, node->eval, node->eval_kind);
    }

    int32_t i, count = ecs_vec_count(&v->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->annot);
        for (i = 0; i < count ; i ++) {
            flecs_script_apply_annot(v, node->eval, annots[i]);
        }
        ecs_vec_clear(&v->annot);
    }

    if (ecs_script_visit_node(v, node->scope)) {
        return -1;
    }

    v->entity = old_entity;

    return 0;
}

static
int flecs_script_eval_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (!v->entity) {
        if (node->id.second) {
            flecs_script_eval_error(v, node, "missing entity for pair (%s, %s)",
                node->id.first, node->id.second);
        } else {
            flecs_script_eval_error(v, node, "missing entity for tag %s", 
                node->id.first);
        }
        return -1;
    }

    if (flecs_script_eval_id(v, &node->id)) {
        return -1;
    }

    ecs_add_id(v->world, v->entity->eval, node->id.eval);

    return 0;
}

static
int flecs_script_eval_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (!v->entity) {
        if (node->id.second) {
            flecs_script_eval_error(v, node, "missing entity for pair (%s, %s)",
                node->id.first, node->id.second);
        } else {
            flecs_script_eval_error(v, node, "missing entity for component %s", 
                node->id.first);
        }
        return -1;
    }

    if (flecs_script_eval_id(v, &node->id)) {
        return -1;
    }

    if (node->expr && node->expr[0]) {
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, node->id.eval);
        if (!ti) {
            char *idstr = ecs_id_str(v->world, node->id.eval);
            flecs_script_eval_error(v, node, 
                "cannot set value of '%s': not a component", idstr);
            ecs_os_free(idstr);
            return -1;
        }

        ecs_value_t value = {
            .ptr = ecs_ensure_id(v->world, v->entity->eval, node->id.eval),
            .type = node->id.eval
        };

        if (flecs_script_eval_expr(v, node->expr, &value)) {
            return -1;
        }

        ecs_modified_id(v->world, v->entity->eval, node->id.eval);
    } else {
        ecs_add_id(v->world, v->entity->eval, node->id.eval);
    }

    return 0;
}

static
int flecs_script_eval_default_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_default_component_t *node)
{
    if (!v->entity) {
        flecs_script_eval_error(v, node, 
            "missing entity for default component");
        return -1;
    }

    ecs_script_entity_t *scope = v->entity->parent;
    if (!scope) {
        flecs_script_eval_error(v, node, 
            "invalid default component outside of entity scope");
        return -1;
    }

    if (!scope->eval_kind) {
        flecs_script_eval_error(v, node, 
            "invalid default component in scope for '%s' without kind "
                "(try 'KindType entity {')", scope->name);
        return -1;
    }

    const EcsDefaultChildComponent *cp = 
        ecs_get(v->world, scope->eval_kind, EcsDefaultChildComponent);
    if (!cp || !cp->component) {
        flecs_script_eval_error(v, node, 
            "kind '%s' of scope '%s' does not have a default component",
            scope->kind, scope->name);
        return -1;
    }

    ecs_value_t value = {
        .ptr = ecs_ensure_id(v->world, v->entity->eval, cp->component),
        .type = cp->component
    };

    if (flecs_script_eval_expr(v, node->expr, &value)) {
        return -1;
    }

    ecs_modified_id(v->world, v->entity->eval, cp->component);

    return 0;
}

static
int flecs_script_eval_with_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_value_t *value = ecs_vec_append_t(a, &v->with, ecs_value_t);
    *value = var->value;

    return 0;
}

static
int flecs_script_eval_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_value_t *value = ecs_vec_append_t(a, &v->with, ecs_value_t);
    value->type = node->id.eval;
    value->ptr = NULL;

    return 0;
}

static
int flecs_script_eval_with_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_value_t *value = ecs_vec_append_t(a, &v->with, ecs_value_t);
    value->type = node->id.eval;
    value->ptr = NULL;

    if (node->expr && node->expr[0]) {
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, node->id.eval);
        if (!ti) {
            char *idstr = ecs_id_str(v->world, node->id.eval);
            flecs_script_eval_error(v, node, 
                "cannot set value of '%s': not a component", idstr);
            ecs_os_free(idstr);
            return -1;
        }

        value->ptr = flecs_stack_alloc(&v->stack, ti->size, ti->alignment);

        if (flecs_script_eval_expr(v, node->expr, value)) {
            return -1;
        }
    }

    return 0;
}

static
int flecs_script_eval_with(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node)
{
    ecs_allocator_t *a = &v->base.script->allocator;
    int32_t prev_with_count = ecs_vec_count(&v->with);
    ecs_stack_cursor_t *prev_stack_cursor = flecs_stack_get_cursor(&v->stack);

    /* If with vector is not empty, remove empty element (see below) */
    if (prev_with_count) {
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->type == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->ptr == NULL, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_vec_remove_last(&v->with);
    }

    if (ecs_script_visit_scope(v, node->expressions)) {
        return -1;
    }

    /* Add empty element so we vector can be used with entity_desc_t::set */
    ecs_vec_append_t(a, &v->with, ecs_value_t)[0] = (ecs_value_t){0};

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    ecs_vec_set_count_t(a, &v->with, ecs_value_t, prev_with_count);
    flecs_stack_restore_cursor(&v->stack, prev_stack_cursor);

    return 0;
}

static
int flecs_script_eval_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    ecs_allocator_t *a = &v->base.script->allocator;
    int32_t len = ecs_os_strlen(node->name);

    if (len > 2 && !ecs_os_strcmp(&node->name[len - 2], ".*")) {
        char *path = flecs_strdup(a, node->name);
        path[len - 2] = '\0';

        ecs_entity_t from = ecs_lookup(v->world, path);
        if (!from) {
            flecs_script_eval_error(v, node, 
                "unresolved path '%s' in using statement", path);
            flecs_strfree(a, path);
            return -1;
        }

        /* Add each child of the scope to using stack */
        ecs_iter_t it = ecs_children(v->world, from);
        while (ecs_children_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                ecs_vec_append_t(
                    a, &v->using, ecs_entity_t)[0] = it.entities[i];
            }
        }

        flecs_strfree(a, path);
    } else {
        ecs_entity_t from = ecs_lookup(v->world, node->name);
        if (!from) {
            flecs_script_eval_error(v, node,
                "unresolved path '%s' in using statement", node->name);
            return -1;
        }

        ecs_vec_append_t(a, &v->using, ecs_entity_t)[0] = from;
    }

    return 0;
}

static
int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_entity_t type = flecs_script_find_entity(v, 0, node->type);
    if (!type) {
        flecs_script_eval_error(v, node,
            "unresolved type '%s' for const variable '%s'", 
                node->type, node->name);
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(v->world, type);
    if (!ti) {
        char *idstr = ecs_id_str(v->world, type);
        flecs_script_eval_error(v, node, 
            "cannot set value of '%s': not a component", idstr);
        ecs_os_free(idstr);
        return -1;
    }

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    var->value.type = type;
    var->value.ptr = flecs_stack_alloc(&v->stack, ti->size, ti->alignment);

    if (flecs_script_eval_expr(v, node->expr, &var->value)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_eval_pair_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node)
{
    ecs_entity_t first = flecs_script_find_entity(v, 0, node->id.first);
    if (!first) {
        flecs_script_eval_error(v, node,
            "unresolved relationship '%s' for pair scope", node->id.first);
        return -1;
    }

    ecs_entity_t second = flecs_script_create_entity(v, node->id.second);
    if (!second) {
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_entity_t prev_first = v->with_relationship;
    ecs_entity_t prev_second = 0;
    int32_t prev_with_relationship_sp = v->with_relationship_sp;

    v->with_relationship = first;

    if (prev_first != first) {
        /* Append new element to with stack */
        ecs_value_t *value = ecs_vec_append_t(a, &v->with, ecs_value_t);
        value->type = ecs_pair(first, second);
        v->with_relationship_sp = ecs_vec_count(&v->with) - 1;
    } else {
        /* Get existing with element for current relationhip stack */
        ecs_value_t *value = ecs_vec_get_t(
            &v->with, ecs_value_t, v->with_relationship_sp);
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first, 
            ECS_INTERNAL_ERROR, NULL);
        prev_second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    if (prev_second) {
        ecs_value_t *value = ecs_vec_get_t(
            &v->with, ecs_value_t, v->with_relationship_sp);
        value->type = ecs_pair(first, prev_second);
    }

    v->with_relationship = prev_first;
    v->with_relationship_sp = prev_with_relationship_sp;

    return 0;
}

static
int flecs_script_eval_if(
    ecs_script_eval_visitor_t *v,
    ecs_script_if_t *node)
{
    bool cond = false;
    ecs_value_t condval = { .type = ecs_id(ecs_bool_t), .ptr = &cond };

    if (flecs_script_eval_expr(v, node->expr, &condval)) {
        return -1;
    }

    if (flecs_script_eval_scope(v, cond ? node->if_true : node->if_false)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_eval_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_annot_t *node)
{
    if (!v->base.next) {
        flecs_script_eval_error(v, node,
            "annotation '%s' is not applied to anything", node->name);
        return -1;
    }

    ecs_script_node_kind_t kind = v->base.next->kind;
    if (kind != EcsAstEntity && kind != EcsAstAnnotation) {
        flecs_script_eval_error(v, node,
            "annotation must be applied to an entity");
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_vec_append_t(a, &v->annot, ecs_script_annot_t*)[0] = node;

    return 0;
}

static
int flecs_script_eval_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_eval_scope(
            v, (ecs_script_scope_t*)node);
    case EcsAstTag:
        return flecs_script_eval_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_eval_component(
            v, (ecs_script_component_t*)node);
    case EcsAstDefaultComponent:
        return flecs_script_eval_default_component(
            v, (ecs_script_default_component_t*)node);
    case EcsAstWithVar:
        return flecs_script_eval_with_var(
            v, (ecs_script_var_component_t*)node);
    case EcsAstWithTag:
        return flecs_script_eval_with_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstWithComponent:
        return flecs_script_eval_with_component(
            v, (ecs_script_component_t*)node);
    case EcsAstWith:
        return flecs_script_eval_with(
            v, (ecs_script_with_t*)node);
    case EcsAstUsing:
        return flecs_script_eval_using(
            v, (ecs_script_using_t*)node);
    case EcsAstAnnotation:
        return flecs_script_eval_annot(
            v, (ecs_script_annot_t*)node);
    case EcsAstAssembly:
        return 0;
    case EcsAstProp:
        return 0;
    case EcsAstConst:
        return flecs_script_eval_const(
            v, (ecs_script_var_node_t*)node);
    case EcsAstEntity:
        return flecs_script_eval_entity(
            v, (ecs_script_entity_t*)node);
    case EcsAstPairScope:
        return flecs_script_eval_pair_scope(
            v, (ecs_script_pair_scope_t*)node);
    case EcsAstIf:
        return flecs_script_eval_if(
            v, (ecs_script_if_t*)node);
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");

    return -1;
}

int ecs_script_eval(
    ecs_script_t *script)
{
    ecs_script_eval_visitor_t v = {
        .world = script->world
    };

    flecs_stack_init(&v.stack);
    ecs_vec_init_t(&script->allocator, &v.using, ecs_entity_t, 0);
    ecs_vec_init_t(&script->allocator, &v.with, ecs_value_t, 0);
    ecs_vec_init_t(&script->allocator, &v.annot, ecs_script_annot_t*, 0);
    int result = ecs_script_visit(script, &v, flecs_script_eval_node);
    ecs_vec_fini_t(&script->allocator, &v.annot, ecs_script_annot_t*);
    ecs_vec_fini_t(&script->allocator, &v.with, ecs_value_t);
    ecs_vec_fini_t(&script->allocator, &v.using, ecs_entity_t);
    flecs_stack_fini(&v.stack);

    return result;
}
