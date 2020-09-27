#include "private_api.h"

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int parse_type_action(
    ecs_world_t *world,
    const char *name,
    const char *sig,
    int64_t column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t role,
    const char *entity_id,
    const char *source_id,
    const char *trait_id,
    const char *arg_name,
    void *data)
{
    ecs_vector_t **array = data;
    (void)source_id;
    (void)inout_kind;
    
    if (arg_name) {
        ecs_parser_error(name, sig, column, 
            "column names not supported in type expression");
        return -1;
    }

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

        if (trait_id) {
            ecs_entity_t trait = ecs_lookup_fullpath(world, trait_id);
            if (!trait) {
                ecs_parser_error(name, sig, column, 
                    "unresolved trait identifier '%s'", trait_id);
                return -1;
            }

            entity = ecs_entity_t_comb(entity, trait);
        }        

        if (oper_kind == EcsOperAnd) {
            ecs_entity_t* e_ptr = ecs_vector_add(array, ecs_entity_t);
            *e_ptr = entity | role;
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

    ecs_table_t *table = ecs_table_find_or_create(world, &entities);
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
        if (ECS_HAS_ROLE(e, AND)) {
            ecs_entity_t entity = e & ECS_COMPONENT_MASK;
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
            world, table, &normalized_array, NULL);

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
        ecs_parse_expr(world, name, expr, parse_type_action, &vec);
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
const char* ecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name)
{
    const char *prefix = world->name_prefix;
    if (type_name && prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(type_name, prefix, len) && 
           (isupper(type_name[len]) || type_name[len] == '_')) 
        {
            if (type_name[len] == '_') {
                return type_name + len + 1;
            } else {
                return type_name + len;
            }
        }
    }

    return type_name;
}

void ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name)
{
    const char *e_name = ecs_name_from_symbol(world, name);

    ecs_set(world, e, EcsName, { 
        .value = e_name, 
        .symbol = name 
    });
}

ecs_entity_t ecs_lookup_w_id(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name)
{
    if (e) {
        if (name) {
            /* Make sure name is the same */
            const char *existing = ecs_get_name(world, e);
            if (existing && strcmp(existing, name)) {
                ecs_abort(ECS_INCONSISTENT_NAME, name);
            }
            if (!existing) {
                ecs_set_symbol(world, e, name);
            }
        }
    }
    
    ecs_entity_t result = e;
    if (!result) {
        if (!name) {
            /* If neither an id nor name is specified, return 0 */
            return 0;
        }

        result = ecs_lookup(world, name);
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
    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new(world, 0);
        ecs_set_symbol(world, result, name);
    }
    
    EcsType type = type_from_expr(world, name, expr);
    ecs_add_type(world, result, type.normalized);

    return result;
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new(world, 0);
        ecs_set_symbol(world, result, name);
    }

    ecs_add_entity(world, result, EcsPrefab);

    EcsType type = type_from_expr(world, name, expr);
    ecs_add_type(world, result, type.normalized);

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

    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new_component_id(world);
        ecs_set_symbol(world, result, name);
    }

    /* ecs_new_component_id does not add the scope, so add it explicitly */
    ecs_entity_t scope = world->stage.scope;
    if (scope) {
        ecs_add_entity(world, result, ECS_CHILDOF | scope);
    }

    bool added = false;
    EcsComponent *ptr = ecs_get_mut(world, result, EcsComponent, &added);
    if (added) {
        ptr->size = ecs_from_size_t(size);
        ptr->alignment = ecs_from_size_t(alignment);
    } else {
        if (ptr->size != ecs_from_size_t(size)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }
        if (ptr->alignment != ecs_from_size_t(alignment)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }
    }

    ecs_modified(world, result, EcsComponent);

    if (e > world->stats.last_component_id && e < ECS_HI_COMPONENT_ID) {
        world->stats.last_component_id = e + 1;
    }

    return result;
}

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new_entity(world, 0, name, NULL);
    }
    
    EcsType type_parsed = type_from_expr(world, name, expr);

    bool added = false;
    EcsType *type = ecs_get_mut(world, result, EcsType, &added);
    if (added) {
        type->type = type_parsed.type;
        type->normalized = type_parsed.normalized;
    } else {
        if (type->type != type_parsed.type) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (type->normalized != type_parsed.normalized) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }     

    /* This will allow the type to show up in debug tools */
    ecs_map_set(world->type_handles, (uintptr_t)type_parsed.type, &result);

    return result;
}
