#include "private_api.h"

static
ecs_vector_t* sort_and_dedup(
    ecs_vector_t *result)
{
    /* Sort vector */
    ecs_vector_sort(result, ecs_id_t, flecs_entity_compare_qsort);

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

/** Parse callback that adds type to type identifier */
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

        if (term.args[0].set.mask != EcsDefaultSet) {
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
    ecs_ids_t ids_array = flecs_type_to_ids(ids);
    ecs_table_t *result = flecs_table_find_or_create(world, &ids_array);
    return result;
}

/* If a name prefix is set with ecs_set_name_prefix, check if the entity name
 * has the prefix, and if so remove it. This enables using prefixed names in C
 * for components / systems while storing a canonical / language independent 
 * identifier. */
const char* flecs_name_from_symbol(
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
