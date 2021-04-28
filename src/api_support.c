#include "private_api.h"

static
bool ids_compare(
    const ecs_vector_t *ids_1,
    const ecs_vector_t *ids_2)
{
    int32_t i, count = ecs_vector_count(ids_1);
    if (count != ecs_vector_count(ids_2)) {
        return false;
    }

    ecs_id_t *arr_1 = ecs_vector_first(ids_1, ecs_id_t);
    ecs_id_t *arr_2 = ecs_vector_first(ids_2, ecs_id_t);

    for (i = 0; i < count; i ++) {
        if (arr_1[i] != arr_2[i]) {
            return false;
        }
    }

    return true;
}

static
ecs_vector_t* sort_and_dedup(
    ecs_vector_t *result)
{
    /* Sort vector */
    ecs_vector_sort(result, ecs_id_t, ecs_entity_compare_qsort);

    /* Ensure vector doesn't contain duplicates */
    ecs_id_t *ids = ecs_vector_first(result, ecs_id_t);
    int32_t i, offset = 0, count = ecs_vector_count(result);

    for (i = 0; i < count; i ++) {
        if (i && ids[i] == ids[i - 1]) {
            offset ++;
        }

        if (i + offset >= count) {
            break;
        }

        ids[i] = ids[i + offset];
    }

    ecs_vector_set_count(&result, ecs_id_t, i - offset);

    return result;
}

/** Parse callback that adds type to type identifier for ecs_new_type */
static
ecs_vector_t* expr_to_ids(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
#ifdef FLECS_PARSER    
    ecs_vector_t *result = NULL;
    const char *ptr = expr;
    ecs_term_t term = {0};

    if (!ptr) {
        return NULL;
    }

    while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) {
        if (term.name) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "column names not supported in type expression");
            goto error;
        }

        if (term.oper != EcsAnd && term.oper != EcsAndFrom) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "operator other than AND not supported in type expression");
            goto error;
        }

        if (ecs_term_finalize(world, name, expr, &term)) {
            goto error;
        }

        if (term.args[0].entity == 0) {
            /* Empty term */
            goto done;
        }

        if (term.args[0].set != EcsDefaultSet) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "source modifiers not supported for type expressions");
            goto error;
        }

        if (term.args[0].entity != EcsThis) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "subject other than this not supported in type expression");
            goto error;
        }

        if (term.oper == EcsAndFrom) {
            term.role = ECS_AND;
        }

        ecs_id_t* elem = ecs_vector_add(&result, ecs_id_t);
        *elem = term.id | term.role;

        ecs_term_fini(&term);
    }

    result = sort_and_dedup(result);

done:
    return result;
error:
    ecs_term_fini(&term);
    ecs_vector_free(result);
    return NULL;
#else
    (void)world;
    (void)name;
    (void)expr;
    ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
    return NULL;
#endif    
}

/* Create normalized type. A normalized type resolves all elements with an
 * AND flag and appends them to the resulting type, where the default type
 * maintains the original type hierarchy. */
static
ecs_vector_t* ids_to_normalized_ids(
    ecs_world_t *world,
    ecs_vector_t *ids)
{
    ecs_vector_t *result = NULL;

    ecs_entity_t *array = ecs_vector_first(ids, ecs_id_t);
    int32_t i, count = ecs_vector_count(ids);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (ECS_HAS_ROLE(e, AND)) {
            ecs_entity_t entity = ECS_PAIR_OBJECT(e);

            const EcsType *type_ptr = ecs_get(world, entity, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INVALID_PARAMETER, 
                "flag must be applied to type");

            ecs_vector_each(type_ptr->normalized, ecs_id_t, c_ptr, {
                ecs_entity_t *el = ecs_vector_add(&result, ecs_id_t);
                *el = *c_ptr;
            })
        } else {
            ecs_entity_t *el = ecs_vector_add(&result, ecs_id_t);
            *el = e;
        }   
    }

    return sort_and_dedup(result);
}

static
ecs_table_t* table_from_ids(
    ecs_world_t *world,
    ecs_vector_t *ids)
{
    ecs_entities_t ids_array = ecs_type_to_entities(ids);
    ecs_table_t *result = ecs_table_find_or_create(world, &ids_array);
    return result;
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
    if (!name) {
        return;
    }
    
    const char *e_name = ecs_name_from_symbol(world, name);

    EcsName *name_ptr = ecs_get_mut(world, e, EcsName, NULL);
    name_ptr->value = e_name;

    if (name_ptr->symbol) {
        ecs_os_free(name_ptr->symbol);
    }

    name_ptr->symbol = ecs_os_strdup(name);
}

ecs_entity_t ecs_lookup_w_id(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name)
{
    if (e) {
        /* If explicit id was provided but it does not exist in the world, make
         * sure it has the proper scope. This can happen when an entity was 
         * defined in another world. */
        if (!ecs_exists(world, e)) {
            ecs_entity_t scope = world->stage.scope;
            if (scope) {
                ecs_add_pair(world, e, EcsChildOf, scope);
            }
        }

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
    ecs_vector_t *ids = expr_to_ids(world, NULL, expr);
    if (!ids) {
        return NULL;
    }

    ecs_vector_t *normalized_ids = ids_to_normalized_ids(world, ids);
    ecs_vector_free(ids);

    ecs_table_t *table = table_from_ids(world, normalized_ids);
    ecs_vector_free(normalized_ids);

    return table->type;
}

ecs_table_t* ecs_table_from_str(
    ecs_world_t *world,
    const char *expr)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);

    ecs_vector_t *ids = expr_to_ids(world, NULL, expr);
    if (!ids) {
        return NULL;
    }

    ecs_table_t *result = table_from_ids(world, ids);
    ecs_vector_free(ids);

    return result;
}

static
int add_expr_to_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const char *expr)
{
    if (!expr || !expr[0] || !strcmp(expr, "0")) {
        return 0;
    }

    ecs_vector_t *ids = expr_to_ids(world, name, expr);
    if (!ids) {
        return -1;
    }

    ecs_vector_t *normalized = ids_to_normalized_ids(world, ids);
    ecs_add_type(world, entity, normalized);
    
    ecs_vector_free(ids);
    ecs_vector_free(normalized);   

    return 0;
}

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const char *expr)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t result = ecs_lookup_w_id(world, entity, name);
    if (!result) {
        result = ecs_new(world, 0);
        ecs_set_symbol(world, result, name);
    }

    if (add_expr_to_entity(world, result, name, expr)) {
        ecs_delete(world, result);
        return 0;
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
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_from_world(&world);

    bool is_readonly = world->is_readonly;
    bool found = false;

    /* If world is in progress component may be registered, but only when not
     * in multithreading mode. */
    if (is_readonly) {
        ecs_assert(ecs_get_stage_count(world) <= 1, 
            ECS_INVALID_WHILE_ITERATING, NULL);

        /* Component creation should not be deferred */
        world->is_readonly = false;
    }

    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new_component_id(world);
        found = true;
    }

    if (found) {
        ecs_set_symbol(world, result, name);
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

    if (is_readonly) {
        world->is_readonly = true;
    }

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, result, EcsComponent), ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_from_world(&world);

    ecs_entity_t result = ecs_lookup_w_id(world, e, name);
    if (!result) {
        result = ecs_new_entity(world, 0, name, NULL);
    }

    ecs_vector_t *ids = expr_to_ids(world, name, expr);
    ecs_vector_t *normalized = ids_to_normalized_ids(world, ids);
    ecs_assert(normalized != NULL, ECS_INTERNAL_ERROR, NULL);

    bool added = false;
    EcsType *type = ecs_get_mut(world, result, EcsType, &added);
    if (added) {
        ecs_table_t *table_type = table_from_ids(world, ids);
        ecs_table_t *table_normalized = table_from_ids(world, normalized);
        type->type = table_type->type;
        type->normalized = table_normalized->type;

        /* This will allow the type to show up in debug tools */
        ecs_map_set(world->type_handles, (uintptr_t)table_type->type, &result);        
    } else {
        if (!ids_compare(type->type, ids)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (!ids_compare(type->normalized, normalized)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    ecs_vector_free(ids);
    ecs_vector_free(normalized);    

    return result;
}
