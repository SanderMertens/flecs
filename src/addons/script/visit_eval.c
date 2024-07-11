/**
 * @file addons/script/visit_eval.c
 * @brief Script evaluation visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

void flecs_script_eval_error_(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = flecs_vasprintf(fmt, args);
    va_end(args);

    if (node) {
        int32_t line = ecs_script_node_line_number(v->base.script, node);
        ecs_parser_error(v->base.script->pub.name, NULL, 0, "%d: %s", line, msg);
    } else {
        ecs_parser_error(v->base.script->pub.name, NULL, 0, "%s", msg);
    }

    ecs_os_free(msg);
}

static
ecs_value_t* flecs_script_with_append(
    ecs_allocator_t *a,
    ecs_script_eval_visitor_t *v)
{
    if (ecs_vec_count(&v->with)) {
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->type == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->ptr == NULL, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_vec_remove_last(&v->with);
    }

    ecs_vec_append_t(a, &v->with, ecs_value_t);
    ecs_value_t *last = ecs_vec_append_t(a, &v->with, ecs_value_t);
    ecs_os_memset_t(last, 0, ecs_value_t);
    return ecs_vec_get_t(&v->with, ecs_value_t, ecs_vec_count(&v->with) - 2);
}

static
void flecs_script_with_set_count(
    ecs_allocator_t *a,
    ecs_script_eval_visitor_t *v,
    int32_t count)
{
    if (count) {
        ecs_value_t *last = ecs_vec_get_t(&v->with, ecs_value_t, count);
        ecs_os_memset_t(last, 0, ecs_value_t);
        ecs_vec_set_count_t(a, &v->with, ecs_value_t, count + 1);
    } else {
        ecs_vec_set_count_t(a, &v->with, ecs_value_t, 0);
    }
}

static
ecs_value_t* flecs_script_with_last(
    ecs_script_eval_visitor_t *v)
{
    int32_t count = ecs_vec_count(&v->with);
    if (count) {
        return ecs_vec_get_t(&v->with, ecs_value_t, count - 2);
    }
    return NULL;
}

static
int32_t flecs_script_with_count(
    ecs_script_eval_visitor_t *v)
{
    if (ecs_vec_count(&v->with)) {
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->type == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vec_last_t(&v->with, ecs_value_t)->ptr == NULL, 
            ECS_INTERNAL_ERROR, NULL);
        return ecs_vec_count(&v->with) - 1;
    }
    return 0;
}

const ecs_type_info_t* flecs_script_get_type_info(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_ensure(v->world, id);
    if (!idr) {
        goto error;
    }

    if (!idr->type_info) {
        goto error;
    }

    return idr->type_info;
error:
    {
        char *idstr = ecs_id_str(v->world, id);
        flecs_script_eval_error(v, node, 
            "cannot set value of '%s': not a component", idstr);
        ecs_os_free(idstr);
    }
    return NULL;
}

ecs_entity_t flecs_script_find_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t from,
    const char *path)
{
    if (!path) {
        return 0;
    }

    if (path[0] == '$') {
        const ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, &path[1]);
        if (!var) {
            return 0;
        }

        if (var->value.type != ecs_id(ecs_entity_t)) {
            char *type_str = ecs_id_str(v->world, var->value.type);
            flecs_script_eval_error(v, NULL, 
                "variable '%s' must be of type entity, got '%s'", 
                    path, type_str);
            ecs_os_free(type_str);
            return 0;
        }

        if (var->value.ptr == NULL) {
            flecs_script_eval_error(v, NULL, 
                "variable '%s' is not initialized", path);
            return 0;
        }

        ecs_entity_t result = *(ecs_entity_t*)var->value.ptr;
        if (!result) {
            flecs_script_eval_error(v, NULL, 
                "variable '%s' contains invalid entity id (0)", path);
            return 0;
        }

        return result;
    }

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

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name)
{
    ecs_value_t *with = NULL;
    if (flecs_script_with_count(v)) {
        with = ecs_vec_first_t(&v->with, ecs_value_t);
    }

    if (name || with) {
        ecs_entity_desc_t desc = {0};
        desc.name = name;
        desc.parent = v->parent;
        desc.set = with;
        return ecs_entity_init(v->world, &desc);
    } else if (v->parent) {
        return ecs_new_w_pair(v->world, EcsChildOf, v->parent);
    } else {
        return ecs_new(v->world);
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
    void *node,
    ecs_script_id_t *id)
{
    ecs_entity_t second_from = 0;

    if (!id->first) {
        flecs_script_eval_error(v, node, 
            "invalid component/tag identifier");
        return -1;
    }

    if (v->template) {
        /* Can't resolve variables while preprocessing template scope */
        if (id->first[0] == '$') {
            return 0;
        }
        if (id->second && id->second[0] == '$') {
            return 0;
        }
    }

    ecs_entity_t first = flecs_script_find_entity(v, 0, id->first);
    if (!first) {
        if (id->first[0] == '$') {
            flecs_script_eval_error(v, node, 
                "unresolved variable '%s'", id->first);
            return -1;
        }

        flecs_script_eval_error(v, node,
            "unresolved identifier '%s'", id->first);
        return -1;
    } else if (id->second) {
        second_from = flecs_get_oneof(v->world, first);
    }

    if (id->second) {
        ecs_entity_t second = flecs_script_find_entity(
            v, second_from, id->second);
        if (!second) {
            if (id->second[0] == '$') {
                flecs_script_eval_error(v, node, 
                    "unresolved variable '%s'", id->second);
                return -1;
            }

            /* Tags/components must be created in advance for templates */
            if (v->template) {
                flecs_script_eval_error(v, node, 
                    "'%s' must be defined outside of template scope", 
                        id->second);
                return -1;
            }

            if (second_from) {
                char *parent_str = ecs_id_str(v->world, second_from);
                flecs_script_eval_error(v, node, "target '%s' not found in "
                    "parent '%s'", id->second, parent_str);
                ecs_os_free(parent_str);
                return -1;
            }

            flecs_script_eval_error(v, node,
                "unresolved identifier '%s'", id->second);
            return -1;
        }

        if (first == EcsAny || second == EcsAny) {
            flecs_script_eval_error(v, node,
                "cannot use anonymous entity as element of pair");
            return -1;
        }

        id->eval = id->flag | ecs_pair(first, second);
    } else {
        if (first == EcsAny) {
            flecs_script_eval_error(v, node,
                "cannot use anonymous entity as component or tag");
            return -1;
        }

        id->eval = id->flag | first;
    }

    return 0;
}

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    const char *expr,
    ecs_value_t *value)
{
    if (!value->type && expr[0] == '{') {
        expr ++;
    }

    ecs_script_expr_run_desc_t desc = {
        .name = v->base.script->pub.name,
        .expr = expr,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = v,
        .vars = v->vars
    };

    if (!ecs_script_expr_run(v->world, expr, value, &desc)) {
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
    int32_t prev_using_count = ecs_vec_count(&v->using);

    for (int i = v->base.depth - 2; i >= 0; i --) {
        if (v->base.nodes[i]->kind == EcsAstScope) {
            node->parent = (ecs_script_scope_t*)v->base.nodes[i];
            break;
        }
    }

    ecs_allocator_t *a = v->allocator;
    v->vars = flecs_script_vars_push(v->vars, &v->stack, a);

    if (scope_parent && (scope_parent->kind == EcsAstEntity)) {
        v->parent = ecs_script_node(entity, scope_parent)->eval;
    }

    int result = ecs_script_visit_scope(v, node);

    ecs_vec_set_count_t(a, &v->using, ecs_entity_t, prev_using_count);
    v->vars = ecs_script_vars_pop(v->vars);
    v->parent = prev_eval_parent;

    return result;
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
    bool is_slot = false;
    if (node->kind) {
        ecs_script_id_t id = {
            .first = node->kind
        };

        if (!ecs_os_strcmp(node->kind, "prefab")) {
            id.eval = EcsPrefab;
        } else if (!ecs_os_strcmp(node->kind, "slot")) {
            is_slot = true;
        } else if (flecs_script_eval_id(v, node, &id)) {
            return -1;
        }

        node->eval_kind = id.eval;
    } else {
        /* Inherit kind from parent kind's DefaultChildComponent, if it existst */
        ecs_script_scope_t *scope = ecs_script_current_scope(v);
        if (scope && scope->default_component_eval) {
            node->eval_kind = scope->default_component_eval;
        }
    }

    if (v->template) {
        return 0;
    }

    node->eval = flecs_script_create_entity(v, node->name);
    node->parent = v->entity;

    if (is_slot) {
        ecs_entity_t parent = ecs_get_target(
            v->world, node->eval, EcsChildOf, 0);
        if (!parent) {
            flecs_script_eval_error(v, node, 
                "slot entity must have a parent");
            return -1;
        }

        ecs_add_pair(v->world, node->eval, EcsSlotOf, parent);
    }

    const EcsDefaultChildComponent *default_comp = NULL;
    ecs_script_entity_t *old_entity = v->entity;
    v->entity = node;

    if (node->eval_kind) {
        ecs_add_id(v->world, node->eval, node->eval_kind);

        default_comp = 
            ecs_get(v->world, node->eval_kind, EcsDefaultChildComponent);
        if (default_comp) {
            if (!default_comp->component) {
                flecs_script_eval_error(v, node, "entity '%s' has kind '%s' "
                    "with uninitialized DefaultChildComponent",
                        node->name, node->kind);
                return -1;
            }

            node->scope->default_component_eval = default_comp->component;
        }
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

    if (node->eval_kind) {
        if (!node->kind_w_expr) {
            if (ecs_get_type_info(v->world, node->eval_kind) != NULL) {
                ecs_modified_id(v->world, node->eval, node->eval_kind);
            }
        }
    }

    v->entity = old_entity;

    return 0;
}

static
ecs_entity_t flecs_script_get_src(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (entity == EcsVariable) { // Singleton ($)
        if (ECS_IS_PAIR(id)) {
            return ecs_pair_first(v->world, id);
        } else {
            return id & ECS_COMPONENT_MASK;
        }
    }
    return entity;
}

static
int flecs_script_eval_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (v->template) {
        return 0;
    }

    if (!v->entity) {
        if (node->id.second) {
            flecs_script_eval_error(
                v, node, "missing entity for pair (%s, %s)",
                node->id.first, node->id.second);
        } else {
            flecs_script_eval_error(v, node, "missing entity for tag %s", 
                node->id.first);
        }
        return -1;
    }

    ecs_entity_t src = flecs_script_get_src(
        v, v->entity->eval, node->id.eval);
    ecs_add_id(v->world, src, node->id.eval);

    return 0;
}

static
int flecs_script_eval_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (v->template) {
        return 0;
    }

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

    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, node->id.eval);

    if (node->expr && node->expr[0]) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }

        const EcsType *type = ecs_get(v->world, ti->component, EcsType);
        if (type) {
            bool is_collection = false;

            switch(type->kind) {
            case EcsPrimitiveType:
            case EcsBitmaskType:
            case EcsEnumType:
            case EcsStructType:
            case EcsOpaqueType:
                break;
            case EcsArrayType:
            case EcsVectorType:
                is_collection = true;
                break;
            }

            if (node->is_collection != is_collection) {
                char *id_str = ecs_id_str(v->world, ti->component);
                if (node->is_collection && !is_collection) {
                    flecs_script_eval_error(v, node, 
                        "type %s is not a collection (use '%s: {...}')", 
                            id_str, id_str);
                } else {
                    flecs_script_eval_error(v, node, 
                        "type %s is a collection (use '%s: [...]')", 
                            id_str, id_str);
                }
                ecs_os_free(id_str);
                return -1;
            }
        }
 
        ecs_value_t value = {
            .ptr = ecs_ensure_id(v->world, src, node->id.eval),
            .type = ti->component
        };

        if (ecs_os_strcmp(node->expr, "{}")) {
            if (flecs_script_eval_expr(v, node->expr, &value)) {
                return -1;
            }
        }

        ecs_modified_id(v->world, src, node->id.eval);
    } else {
        ecs_add_id(v->world, src, node->id.eval);
    }

    return 0;
}

static
int flecs_script_eval_var_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    ecs_id_t var_id = var->value.type;

    if (var->value.ptr) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, var_id);
        if (!ti) {
            return -1;
        }

        ecs_value_t value = {
            .ptr = ecs_ensure_id(v->world, v->entity->eval, var_id),
            .type = var_id
        };

        ecs_value_copy_w_type_info(v->world, ti, value.ptr, var->value.ptr);

        ecs_modified_id(v->world, v->entity->eval, var_id);
    } else {
        ecs_add_id(v->world, v->entity->eval, var_id);
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

    ecs_script_scope_t *scope = ecs_script_current_scope(v);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope->node.kind == EcsAstScope, ECS_INTERNAL_ERROR, NULL);
    scope = scope->parent;

    if (!scope) {
        flecs_script_eval_error(v, node,
            "entity '%s' is in root scope which cannot have a default type",
                v->entity->name); 
        return -1;
    }

    ecs_id_t default_type = scope->default_component_eval;
    if (!default_type) {
        flecs_script_eval_error(v, node,
            "scope for entity '%s' does not have a default type",
                v->entity->name);
        return -1;
    }

    if (ecs_get_type_info(v->world, default_type) == NULL) {
        char *id_str = ecs_id_str(v->world, default_type);
        flecs_script_eval_error(v, node,
            "cannot use tag '%s' as default type in assignment",
                id_str);
        ecs_os_free(id_str);
        return -1;
    }

    ecs_value_t value = {
        .ptr = ecs_ensure_id(v->world, v->entity->eval, default_type),
        .type = default_type
    };

    if (flecs_script_eval_expr(v, node->expr, &value)) {
        return -1;
    }

    ecs_modified_id(v->world, v->entity->eval, default_type);

    return 0;
}

static
int flecs_script_eval_with_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    ecs_allocator_t *a = v->allocator;
    ecs_value_t *value = flecs_script_with_append(a, v);
    *value = var->value;

    return 0;
}

static
int flecs_script_eval_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = v->allocator;
    ecs_value_t *value = flecs_script_with_append(a, v);
    value->type = node->id.eval;
    value->ptr = NULL;

    return 0;
}

static
int flecs_script_eval_with_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = v->allocator;
    ecs_value_t *value = flecs_script_with_append(a, v);
    value->type = node->id.eval;
    value->ptr = NULL;

    if (node->expr && node->expr[0]) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }

        value->ptr = flecs_stack_alloc(&v->stack, ti->size, ti->alignment);
        value->type = ti->component; // Expression parser needs actual type

        if (flecs_script_eval_expr(v, node->expr, value)) {
            return -1;
        }

        value->type = node->id.eval; // Restore so we're adding actual id
    }

    return 0;
}

static
int flecs_script_eval_with(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node)
{
    ecs_allocator_t *a = v->allocator;
    int32_t prev_with_count = flecs_script_with_count(v);
    ecs_stack_cursor_t *prev_stack_cursor = flecs_stack_get_cursor(&v->stack);
    int result = 0;

    if (ecs_script_visit_scope(v, node->expressions)) {
        result = -1;
        goto error;
    }

    ecs_value_t *value = flecs_script_with_last(v);
    if (!value->ptr) {
        if (ecs_is_valid(v->world, value->type)) {
            node->scope->default_component_eval = value->type;
        }
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        result = -1;
        goto error;
    }

error:
    flecs_script_with_set_count(a, v, prev_with_count);
    flecs_stack_restore_cursor(&v->stack, prev_stack_cursor);
    return result;
}

static
int flecs_script_eval_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    ecs_allocator_t *a = v->allocator;
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
        ecs_entity_t from = ecs_lookup_path_w_sep(
            v->world, 0, node->name, NULL, NULL, false);
        if (!from) {
            from = ecs_entity(v->world, {
                .name = node->name,
                .root_sep = ""
            });

            if (!from) {
                return -1;
            }
        }

        ecs_vec_append_t(a, &v->using, ecs_entity_t)[0] = from;
    }

    return 0;
}

static
int flecs_script_eval_module(
    ecs_script_eval_visitor_t *v,
    ecs_script_module_t *node)
{
    ecs_entity_t m = flecs_script_create_entity(v, node->name);
    if (!m) {
        return -1;
    }

    ecs_add_id(v->world, m, EcsModule);

    v->module = m;
    v->parent = m;

    return 0;
}

static
int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "variable '%s' redeclared", node->name);
        return -1;
    }

    if (node->type) {
        ecs_entity_t type = flecs_script_find_entity(v, 0, node->type);
        if (!type) {
            flecs_script_eval_error(v, node,
                "unresolved type '%s' for const variable '%s'", 
                    node->type, node->name);
            return -1;
        }

        const ecs_type_info_t *ti = flecs_script_get_type_info(v, node, type);
        if (!ti) {
            flecs_script_eval_error(v, node,
                "failed to retrieve type info for '%s' for const variable '%s'", 
                    node->type, node->name);
            return -1;
        }

        var->value.ptr = flecs_stack_calloc(&v->stack, ti->size, ti->alignment);
        var->value.type = type;
        var->type_info = ti;

        if (flecs_script_eval_expr(v, node->expr, &var->value)) {
            flecs_script_eval_error(v, node,
                "failed to evaluate expression for const variable '%s'", 
                    node->name);
            return -1;
        }
    } else {
        /* We don't know the type yet, so we can't create a storage for it yet.
         * Run the expression first to deduce the type. */
        ecs_value_t value = {0};
        if (flecs_script_eval_expr(v, node->expr, &value)) {
            flecs_script_eval_error(v, node,
                "failed to evaluate expression for const variable '%s'", 
                    node->name);
            return -1;
        }

        ecs_assert(value.type != 0, ECS_INTERNAL_ERROR, NULL);
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, value.type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        var->value.ptr = flecs_stack_calloc(&v->stack, ti->size, ti->alignment);
        var->value.type = value.type;
        var->type_info = ti;

        ecs_value_copy_w_type_info(v->world, ti, var->value.ptr, value.ptr);
        ecs_value_fini_w_type_info(v->world, ti, value.ptr);
        flecs_free(&v->world->allocator, ti->size, value.ptr);
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
        first = flecs_script_create_entity(v, node->id.first);
        if (!first) {
            return -1;
        }
    }

    ecs_entity_t second = flecs_script_create_entity(v, node->id.second);
    if (!second) {
        return -1;
    }

    ecs_allocator_t *a = v->allocator;
    ecs_entity_t prev_first = v->with_relationship;
    ecs_entity_t prev_second = 0;
    int32_t prev_with_relationship_sp = v->with_relationship_sp;

    v->with_relationship = first;

    if (prev_first != first) {
        /* Append new element to with stack */
        ecs_value_t *value = flecs_script_with_append(a, v);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
        v->with_relationship_sp = flecs_script_with_count(v) - 1;
    } else {
        /* Get existing with element for current relationhip stack */
        ecs_value_t *value = ecs_vec_get_t(
            &v->with, ecs_value_t, v->with_relationship_sp);
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first, 
            ECS_INTERNAL_ERROR, NULL);
        prev_second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    if (prev_second) {
        ecs_value_t *value = ecs_vec_get_t(
            &v->with, ecs_value_t, v->with_relationship_sp);
        value->type = ecs_pair(first, prev_second);
    } else {
        flecs_script_with_set_count(a, v, v->with_relationship_sp);
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
    ecs_value_t condval = { .type = 0, .ptr = NULL };
    if (flecs_script_eval_expr(v, node->expr, &condval)) {
        return -1;
    }

    bool cond;
    if (condval.type == ecs_id(ecs_bool_t)) {
        cond = *(bool*)(condval.ptr);
    } else {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            v->world, condval.type, condval.ptr);
        cond = ecs_meta_get_bool(&cur);
    }

    ecs_value_free(v->world, condval.type, condval.ptr);

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

    ecs_allocator_t *a = v->allocator;
    ecs_vec_append_t(a, &v->annot, ecs_script_annot_t*)[0] = node;

    return 0;
}

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
    case EcsAstVarComponent:
        return flecs_script_eval_var_component(
            v, (ecs_script_var_component_t*)node);
    case EcsAstDefaultComponent:
        return flecs_script_eval_default_component(
            v, (ecs_script_default_component_t*)node);
    case EcsAstWithVar:
        return flecs_script_eval_with_var(
            v, (ecs_script_var_node_t*)node);
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
    case EcsAstModule:
        return flecs_script_eval_module(
            v, (ecs_script_module_t*)node);
    case EcsAstAnnotation:
        return flecs_script_eval_annot(
            v, (ecs_script_annot_t*)node);
    case EcsAstTemplate:
        return flecs_script_eval_template(
            v, (ecs_script_template_node_t*)node);
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
}

void flecs_script_eval_visit_init(
    ecs_script_impl_t *script,
    ecs_script_eval_visitor_t *v)
{
    *v = (ecs_script_eval_visitor_t){
        .base = {
            .script = script,
            .visit = (ecs_visit_action_t)flecs_script_eval_node
        },  
        .world = script->pub.world,
        .allocator = &script->allocator
    };

    flecs_stack_init(&v->stack);
    ecs_vec_init_t(v->allocator, &v->using, ecs_entity_t, 0);
    ecs_vec_init_t(v->allocator, &v->with, ecs_value_t, 0);
    ecs_vec_init_t(v->allocator, &v->annot, ecs_script_annot_t*, 0);

    /* Always include flecs.meta */
    ecs_vec_append_t(v->allocator, &v->using, ecs_entity_t)[0] = 
        ecs_lookup(v->world, "flecs.meta");
}

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v)
{
    ecs_vec_fini_t(v->allocator, &v->annot, ecs_script_annot_t*);
    ecs_vec_fini_t(v->allocator, &v->with, ecs_value_t);
    ecs_vec_fini_t(v->allocator, &v->using, ecs_entity_t);
    flecs_stack_fini(&v->stack);
}

int ecs_script_eval(
    ecs_script_t *script)
{
    ecs_script_eval_visitor_t v;
    ecs_script_impl_t *impl = flecs_script_impl(script);
    flecs_script_eval_visit_init(impl, &v);
    int result = ecs_script_visit(impl, &v, flecs_script_eval_node);
    flecs_script_eval_visit_fini(&v);
    return result;
}

#endif
