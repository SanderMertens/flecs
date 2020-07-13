#include "flecs_private.h"

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int parse_type_action(
    ecs_world_t *world,
    const char *name,
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

        if (from_kind != EcsFromOwned) {
            if (!name) {
                return -1;
            }

            ecs_parser_error(name, sig, column, 
                "source modifiers not supported for type expressions");
            return -1;
        }

        entity = ecs_lookup_fullpath(world, entity_id);
        if (!entity) {
            if (!name) {
                return -1;
            }

            ecs_parser_error(name, sig, column, 
                "unresolved identifier '%s'", entity_id);
            return -1;
        }

        if (oper_kind == EcsOperAnd) {
            ecs_entity_t* e_ptr = ecs_vector_add(array, ecs_entity_t);
            *e_ptr = entity | flags;
        } else {
            if (!name) {
                return -1;
            }

            /* Only AND and OR operators are supported for type expressions */
            ecs_parser_error(name, sig, column, 
                "invalid operator for type expression");
            return -1;
        }
    }

    return 0;
}

static
EcsType type_from_vec(
    ecs_world_t *world,
    ecs_vector_t *vec)
{
    EcsType result = {0, 0};
    ecs_entity_t *array = ecs_vector_first(vec, ecs_entity_t);
    int32_t i, count = ecs_vector_count(vec);

    ecs_entities_t entities = {
        .array = array,
        .count = count
    };

    ecs_table_t *table = ecs_table_find_or_create(
        world, &world->stage, &entities);
    if (!table) {
        return (EcsType){ 0 };
    }    

    result.type = table->type;

    /* Create normalized type. A normalized type resolves all elements with an
     * AND flag and appends them to the resulting type, where the default type
     * maintains the original type hierarchy. */
    ecs_vector_t *normalized = NULL;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e & ECS_AND) {
            ecs_entity_t entity = e & ECS_ENTITY_MASK;
            const EcsType *type_ptr = ecs_get(world, entity, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INVALID_PARAMETER, 
                "flag must be applied to type");

            ecs_vector_each(type_ptr->normalized, ecs_entity_t, c_ptr, {
                ecs_entity_t *el = ecs_vector_add(&normalized, ecs_entity_t);
                *el = *c_ptr;
            })
        }       
    }

    /* Only get normalized type if it's different from the type */
    if (normalized) {
        ecs_entities_t normalized_array = ecs_type_to_entities(normalized);
        ecs_table_t *norm_table = ecs_table_traverse_add(
            world, &world->stage, table, &normalized_array, NULL);

        result.normalized = norm_table->type;

        ecs_vector_free(normalized);
    } else {
        result.normalized = result.type;
    }

    return result;
}

static
EcsType type_from_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    if (expr) {
        ecs_vector_t *vec = ecs_vector_new(ecs_entity_t, 1);
        ecs_parse_expr(world, expr, parse_type_action, name, &vec);
        EcsType result = type_from_vec(world, vec);
        ecs_vector_free(vec);
        return result;
    } else {
        return (EcsType){0, 0};
    }
}

/* If a name prefix is set with ecs_set_name_prefix, check if the entity name
 * has the prefix, and if so remove it. This enables using prefixed names in C
 * for components / systems while storing a canonical / language independent 
 * identifier. */
static
const char* get_entity_name(
    ecs_world_t *world,
    const char *type_name)
{
    const char *prefix = world->name_prefix;
    if (type_name && prefix) {
        size_t len = strlen(prefix);
        if (!strncmp(type_name, prefix, len) && (isupper(type_name[len]) || type_name[len] == '_')) {
            if (type_name[len] == '_') {
                return type_name + len + 1;
            } else {
                return type_name + len;
            }
        }
    }

    return type_name;
}

static
ecs_entity_t lookup(
    ecs_world_t *world,
    const char *name,
    ecs_type_t type)
{
    if (!name) {
        return 0;
    }
    
    ecs_entity_t result = ecs_lookup(world, name);
    if (result) {
        ecs_type_t entity_type = ecs_get_type(world, result);
        if (type && (!entity_type || !ecs_type_contains(world, entity_type, type, true, false))) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }
    
    return result;
}


/* -- Public functions -- */

ecs_type_t ecs_type_from_str(
    ecs_world_t *world,
    const char *expr)
{
    EcsType type = type_from_expr(world, NULL, expr);
    return type.normalized;
}

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = get_entity_name(world, name);

    ecs_entity_t result = lookup(world, name, type.normalized);
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_add_type(world, result, type.normalized);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
    }

    return result;
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = get_entity_name(world, name);

    ecs_entity_t result = lookup(world, name, type.normalized);
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_add_entity(world, result, EcsPrefab);
        ecs_add_type(world, result, type.normalized);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
    } else {
        if (!ecs_has_entity(world, result, EcsPrefab)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    const char *e_name = get_entity_name(world, name);

    ecs_entity_t result = e ? e : lookup(world, e_name, ecs_type(EcsComponent));
    if (!result || e) {
        ecs_stage_t *stage = ecs_get_stage(&world);
        result = e ? e : ++ world->stats.last_component_id;

        ecs_set(world, result, EcsComponent, {
            .size = size,
            .alignment = alignment
        });

        if (name) {
            ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        }

        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_entity(world, result, ECS_CHILDOF | scope);
        }
    } else {
        const EcsComponent *ptr = ecs_get(world, result, EcsComponent);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, name);
        if (ptr->size != size) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }
        if (ptr->alignment != alignment) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }        
    }

    return result;
}

ecs_entity_t ecs_new_module(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    if (!e) {
        char *module_path = ecs_module_path_from_c(name);
        e = ecs_new_from_fullpath(world, module_path);

        EcsName *name_ptr = ecs_get_mut(world, e, EcsName, NULL);
        name_ptr->symbol = name;

        ecs_os_free(module_path);
    }

    ecs_entity_t result = ecs_new_component(world, e, NULL, size, alignment);
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);

    /* Add module tag */
    ecs_add_entity(world, result, EcsModule);

    /* Add module to itself. This way we have all the module information stored
     * in a single contained entity that we can use for namespacing */
    ecs_set_ptr_w_entity(world, result, result, size, NULL);

    /* Set the current scope to the module */
    ecs_set_scope(world, result);

    return result;
}

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);  
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = get_entity_name(world, name);
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsType));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        ecs_set(world, result, EcsType, {
            .type = type.type, .normalized = type.normalized
        });        

        /* This will allow the type to show up in debug tools */
        ecs_map_set(world->type_handles, (uintptr_t)type.type, &result);
    } else {
        const EcsType *ptr = ecs_get(world, result, EcsType);
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
    ecs_entity_t e,
    const char *name,
    ecs_entity_t tag,
    const char *signature,
    ecs_iter_action_t action)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    const char *e_name = get_entity_name(world, name);
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsSignatureExpr));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        if (tag) {
            ecs_add_entity(world, result, tag);
        }

        ecs_set(world, result, EcsSignatureExpr, {signature});
        ecs_set(world, result, EcsIterAction, {action});
    } else {
        EcsSignatureExpr *ptr = ecs_get_mut(world, result, EcsSignatureExpr, NULL);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (strcmp(ptr->expr, signature)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t kind,
    const char *component_name,
    ecs_iter_action_t action)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t component = ecs_lookup_fullpath(world, component_name);
    ecs_assert(component != 0, ECS_INVALID_COMPONENT_ID, component_name);

    const char *e_name = get_entity_name(world, name);
    
    ecs_entity_t result = lookup(world, name, ecs_type(EcsTrigger));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        ecs_set(world, result, EcsTrigger, {
            .kind = kind,
            .action = action,
            .component = component,
            .ctx = NULL
        });
    } else {
        EcsTrigger *ptr = ecs_get_mut(world, result, EcsTrigger, NULL);
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

ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t result = ecs_new_type(world, e, name, expr);
    ecs_assert(ecs_get(world, result, EcsType) != NULL, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_add_entity(world, result, EcsPipeline);

    return result;
}
