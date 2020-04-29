#include "flecs_private.h"

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int parse_type_action(
    ecs_world_t *world,
    const char *system_id,
    const char *sig,
    int column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *entity_id,
    const char *source_id,
    void *data)
{
    ecs_vector_t **array = data;
    (void)source_id;
    (void)inout_kind;

    if (strcmp(entity_id, "0")) {
        ecs_entity_t entity = 0;

        if (from_kind != EcsFromSelf) {
            ecs_parser_error(system_id, sig, column, 
                "source modifiers not supported for type expressions");
            return -1;
        }

        entity = ecs_lookup(world, entity_id);
        if (!entity) {
            ecs_parser_error(system_id, sig, column, 
                "unresolved identifier '%s'", entity_id);
            return -1;
        }

        if (oper_kind == EcsOperAnd) {
            ecs_entity_t* e_ptr = ecs_vector_add(array, ecs_entity_t);
            *e_ptr = entity | flags;
        } else {
            /* Only AND and OR operators are supported for type expressions */
            ecs_parser_error(system_id, sig, column, 
                "invalid operator for type expression");
            return -1;
        }
    }

    return 0;
}

static
EcsTypeComponent type_from_vec(
    ecs_world_t *world,
    ecs_vector_t *vec)
{
    EcsTypeComponent result = {0, 0};
    ecs_entity_t *array = ecs_vector_first(vec);
    int32_t i, count = ecs_vector_count(vec);

    for (i = 0; i < count; i ++) {
        ecs_entity_t entity = array[i];

        ecs_type_t entity_type = ecs_type_find(world, &entity, 1);
        ecs_type_t resolved_type = ecs_type_from_entity(world, entity);
        assert(resolved_type != 0);

        result.type = ecs_type_merge_intern(
            world, &world->stage, result.type, entity_type, 0);

        result.normalized = ecs_type_merge_intern(
            world, &world->stage, result.normalized, resolved_type, 0);
    }

    return result;
}

static
EcsTypeComponent type_from_expr(
    ecs_world_t *world,
    const char *id,
    const char *expr)
{
    if (expr) {
        ecs_vector_t *vec = ecs_vector_new(ecs_entity_t, 1);
        ecs_parse_expr(world, expr, parse_type_action, id, &vec);
        EcsTypeComponent result = type_from_vec(world, vec);
        ecs_vector_free(vec);
        return result;
    } else {
        return (EcsTypeComponent){0, 0};
    }
}

static
ecs_entity_t lookup(
    ecs_world_t *world,
    const char *name,
    ecs_type_t type)
{
    ecs_entity_t result = ecs_lookup(world, name);
    if (result) {
        ecs_type_t entity_type = ecs_get_type(world, result);
        if (!ecs_type_contains(world, entity_type, type, true, false)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }
    return result;
}


/* -- Public functions -- */

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    EcsTypeComponent type = type_from_expr(world, name, expr);

    ecs_entity_t result = lookup(world, name, type.normalized);
    if (!result) {
        result = ecs_new_w_type(world, type.normalized);
        ecs_set(world, result, EcsName, {name});
    }

    return result;
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    EcsTypeComponent type = type_from_expr(world, name, expr);

    ecs_entity_t result = lookup(world, name, type.normalized);
    if (!result) {
        result = ecs_new_w_type(world, type.normalized);
        ecs_add(world, result, EcsPrefab);
        ecs_set(world, result, EcsName, {name});
    } else {
        if (!ecs_has(world, result, EcsPrefab)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    const char *name,
    size_t size)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t result = lookup(world, name, ecs_type(EcsComponent));
    if (!result) {
        result = ecs_set(world, 0, EcsName, {name});
        ecs_set(world, result, EcsComponent, {.size = size});
    } else {
        EcsComponent *ptr = ecs_get_ptr(world, result, EcsComponent);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, name);
        if (ptr->size != size) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);  
    EcsTypeComponent type = type_from_expr(world, name, expr);
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsTypeComponent));
    if (!result) {
        result = ecs_set(world, result, EcsName, {name});
        ecs_set(world, result, EcsTypeComponent, {
            .type = type.type, .normalized = type.normalized
        });        

        /* This will allow the type to show up in debug tools */
        ecs_map_set(world->type_handles, (uintptr_t)type.type, &result);
    } else {
        EcsTypeComponent *ptr = ecs_get_ptr(world, result, EcsTypeComponent);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->type != type.type || 
            ptr->normalized != type.normalized) 
        {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    const char *name,
    ecs_system_kind_t kind,
    char *signature,
    ecs_iter_action_t action)
{
    assert(world->magic == ECS_WORLD_MAGIC);  
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsSystem));
    if (!result) {
        result = ecs_set(world, result, EcsName, {name});
        ecs_set(world, result, EcsSystem, {
            .kind = kind,
            .action = action,
            .signature = signature
        });
    } else {
        EcsSystem *ptr = ecs_get_ptr(world, result, EcsSystem);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->kind != kind) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (strcmp(ptr->signature, signature)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->action != action) {
            ptr->action = action;
        }
    }

    return result;
}

ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    const char *name,
    ecs_trigger_kind_t kind,
    ecs_entity_t component,
    ecs_iter_action_t action,
    void *ctx)
{
    assert(world->magic == ECS_WORLD_MAGIC);  
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsTrigger));
    if (!result) {
        result = ecs_set(world, result, EcsName, {name});
        ecs_set(world, result, EcsTrigger, {
            .kind = kind,
            .action = action,
            .component = component,
            .ctx = ctx
        });
    } else {
        EcsTrigger *ptr = ecs_get_ptr(world, result, EcsTrigger);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->kind != kind) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->component != component) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->action != action) {
            ptr->action = action;
        }
    }

    return result;
}
