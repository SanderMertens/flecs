#include "private_api.h"

#ifdef FLECS_SYSTEMS_H
#include "modules/system/system.h"
#endif

static
ecs_entity_t components_contains(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    ecs_vector_each(table_type, ecs_entity_t, c_ptr, {
        ecs_entity_t entity = *c_ptr;

        if (entity & ECS_CHILDOF) {
            entity &= ECS_ENTITY_MASK;

            ecs_record_t *record = ecs_eis_get(&world->stage, entity);
            ecs_assert(record != 0, ECS_INTERNAL_ERROR, NULL);

            if (record->table) {
                ecs_entity_t component = ecs_type_contains(
                    world, record->table->type, type, match_all, true);

                if (component) {
                    if (entity_out) *entity_out = entity;
                    return component;
                }
            }
        }
    });

    return 0;
}

/* Get actual entity on which specified component is stored */
static
ecs_entity_t get_entity_for_component(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component)
{
    if (entity) {
        ecs_record_t *record = ecs_eis_get(&world->stage, entity);
        ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        if (record->table) {
            type = record->table->type;
        } else {
            type = NULL;
        }
    }

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == component) {
            return entity;
        }
    });

    return ecs_find_entity_in_prefabs(world, entity, type, component, 0);
}

#ifndef NDEBUG
static
ecs_entity_t get_cascade_component(
    ecs_query_t *query)
{
    ecs_sig_column_t *column = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    return column[query->cascade_by - 1].is.component;
}
#endif

static
int32_t rank_by_depth(
    ecs_world_t *world,
    ecs_entity_t rank_by_component,
    ecs_type_t type)
{
    int32_t result = 0;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    for (i = count - 1; i >= 0; i --) {
        if (array[i] & ECS_CHILDOF) {
            ecs_type_t c_type = ecs_get_type(world, array[i] & ECS_ENTITY_MASK);
            int32_t j, c_count = ecs_vector_count(c_type);
            ecs_entity_t *c_array = ecs_vector_first(c_type, ecs_entity_t);

            for (j = 0; j < c_count; j ++) {
                if (c_array[j] == rank_by_component) {
                    result ++;
                    result += rank_by_depth(world, rank_by_component, c_type);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }
        } else if (!(array[i] & ECS_TYPE_ROLE_MASK)) {
            /* No more parents after this */
            break;
        }
    }

    return result;
}

static
int table_compare(
    const void *t1,
    const void *t2)
{
    const ecs_matched_table_t *table_1 = t1;
    const ecs_matched_table_t *table_2 = t2;

    return table_1->rank - table_2->rank;
}

static
void order_ranked_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_table) {
        ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);       
    }

    /* Re-register monitors after tables have been reordered. This will update
     * the table administration with the new matched_table ids, so that when a
     * monitor is executed we can quickly find the right matched_table. */
    if (query->flags & EcsQueryMonitor) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            ecs_table_register_query(
                world, table->table, query, table_i);
        });
    }

    query->match_count ++; 
}

static
void group_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_matched_table_t *table)
{
    if (query->group_table) {
        table->rank = query->group_table(
            world, query->rank_on_component, table->table->type);
    } else {
        table->rank = 0;
    }
}

/* Rank all tables of query. Only necessary if a new ranking function was
 * provided or if a monitored entity set the component used for ranking. */
static
void group_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_table) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });

        ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });              
    }
}

static
bool has_auto_activation(
    ecs_query_t *q)
{
    /* Only a basic query with no additional features does table activation */
    return !(q->flags & EcsQueryNoActivation);
}

#ifndef NDEBUG

static
const char* query_name(
    ecs_world_t *world,
    ecs_query_t *q)
{
    if (q->system) {
        return ecs_get_name(world, q->system);
    } else {
        return q->sig.expr;
    }
}

#endif

static
void get_comp_and_src(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_type_t table_type,
    ecs_sig_column_t *column,
    ecs_sig_oper_kind_t op,
    ecs_sig_from_kind_t from,
    ecs_entity_t *component_out,
    ecs_entity_t *entity_out)
{
    ecs_entity_t component = 0, entity = 0;

    if (op == EcsOperNot) {
        entity = column->source;
    }

    /* Column that retrieves data from self or a fixed entity */
    if (from == EcsFromAny || from == EcsFromEntity || 
        from == EcsFromOwned || from == EcsFromShared) 
    {
        if (op == EcsOperAnd || op == EcsOperNot) {
            component = column->is.component;
        } else if (op == EcsOperOptional) {
            component = column->is.component;
        } else if (op == EcsOperOr) {
            component = ecs_type_contains(
                world, table_type, column->is.type, 
                false, true);
        }

        if (from == EcsFromEntity) {
            entity = column->source;
        }

    /* Column that just passes a handle to the system (no data) */
    } else if (from == EcsFromEmpty) {
        component = column->is.component;

    /* Column that retrieves data from a dynamic entity */
    } else if (from == EcsFromParent || from == EcsCascade) {
        if (op == EcsOperAnd ||
            op == EcsOperOptional)
        {
            component = column->is.component;
            entity = ecs_find_in_type(
                world, table_type, component, ECS_CHILDOF);

        } else if (op == EcsOperOr) {
            component = components_contains(
                world,
                table_type,
                column->is.type,
                &entity,
                false);
        }

    /* Column that retrieves data from a system */
    } else if (from == EcsFromSystem) {
        if (op == EcsOperAnd) {
            component = column->is.component;
        }

        entity = query->system;
    }

    *component_out = component;
    *entity_out = entity;
}

static
int32_t get_component_index(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t *component_out,
    int32_t column_index,
    ecs_sig_oper_kind_t op,
    int32_t *trait_index_offsets)
{
    int32_t result = 0;
    ecs_entity_t component = *component_out;

    if (component) {
        if (component & ECS_TRAIT) {
            ecs_assert(trait_index_offsets != NULL, ECS_INTERNAL_ERROR, NULL);

            component &= ECS_ENTITY_MASK;

            result = ecs_type_trait_index_of(table_type, 
                trait_index_offsets[column_index], component);

            if (result != -1) {
                trait_index_offsets[column_index] = result + 1;

                /* If component of current column is a trait, get the actual trait
                 * type for the table, so the system can see which component the
                 * trait was applied to */   
                ecs_entity_t *trait = ecs_vector_get(
                    table_type, ecs_entity_t, result);
                *component_out = *trait;

                /* Check if the trait is a tag or whether it has data */
                if (ecs_get(world, component, EcsComponent) == NULL) {
                    /* If trait has no data associated with it, use the
                     * component to which the trait has been added */
                    component = ecs_entity_t_lo(*trait);
                }
            }
        } else {
            /* Get column index for component */
            result = ecs_type_index_of(table_type, component);
        }

        /* If column is found, add one to the index, as column zero in
        * a table is reserved for entity id's */
        if (result != -1) {
            result ++;

            /* Check if component is a tag. If it is, set table_data to
            * zero, so that a system won't try to access the data */
            const EcsComponent *data = ecs_get(
                world, component, EcsComponent);

            if (!data || !data->size) {
                result = 0;
            }
        }
        
        /* ecs_table_column_offset may return -1 if the component comes
        * from a prefab. If so, the component will be resolved as a
        * reference (see below) */           
    }

    if (op == EcsOperOptional) {
        /* If table doesn't have the field, mark it as no data */
        if (!ecs_type_has_entity(
            world, table_type, component))
        {
            result = 0;
        }
    }

    return result;
}

static
void add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_type_t table_type,
    int32_t column_index,
    ecs_matched_table_t *table_data,
    ecs_entity_t component,
    ecs_entity_t entity,
    ecs_sig_from_kind_t from)
{
    const EcsComponent *c_info = ecs_get(world, component, EcsComponent);
    
    ecs_entity_t e;
    ecs_ref_t *ref = ecs_vector_add(
            &table_data->references, ecs_ref_t);
    
    table_data->columns[column_index] = -ecs_vector_count(table_data->references);

    /* Find the entity for the component */
    if (from == EcsFromEntity || from == EcsFromEmpty) {
        e = entity;
    } else if (from == EcsCascade) {
        e = entity;
    } else if (from == EcsFromSystem) {
        e = entity;
    } else {
        e = get_entity_for_component(
            world, entity, table_type, component);
    }

    if (from != EcsCascade) {
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
    }
    
    *ref = (ecs_ref_t){0};
    ref->entity = e;
    ref->component = component;

    if (ecs_has(world, component, EcsComponent)) {
        if (c_info->size && from != EcsFromEmpty) {
            if (e) {
                ecs_get_ref_w_entity(
                    world, ref, e, component);
                ecs_set_watch(world, &world->stage, e);                     
            }

            query->flags |= EcsQueryHasRefs;
        }
    }
}

static
ecs_entity_t is_column_trait(
    ecs_sig_column_t *column)
{
    ecs_sig_from_kind_t from_kind = column->from_kind;
    ecs_sig_oper_kind_t oper_kind = column->oper_kind;

    /* For now traits are only supported on owned columns */
    if (from_kind == EcsFromOwned && oper_kind == EcsOperAnd) {
        if (column->is.component & ECS_TRAIT) {
            return column->is.component;
        }
    }

    return 0;
}

static
int32_t type_trait_count(
    ecs_type_t type,
    ecs_entity_t trait)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t result = 0;

    trait &= ECS_ENTITY_MASK;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (ecs_entity_t_hi(e) == trait) {
                result ++;
            }
        }
    }

    return result;
}

/* For each trait that the query subscribes for, count the occurrences in the
 * table. Cardinality of subscribed for traits must be the same as in the table
 * or else the table won't match. */
static
int32_t count_traits(
    ecs_query_t *query,
    ecs_type_t type)
{
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, count = ecs_vector_count(query->sig.columns);
    int32_t first_count = 0, trait_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_entity_t trait = is_column_trait(&columns[i]);
        if (trait) {
            trait_count = type_trait_count(type, trait);
            if (!first_count) {
                first_count = trait_count;
            } else {
                if (first_count != trait_count) {
                    /* The traits that this query subscribed for occur in the
                     * table but don't have the same cardinality. Ignore the
                     * table. This could typically happen for empty tables along
                     * a path in the table graph. */
                    return -1;
                }
            }
        }
    }

    return first_count;
}

/** Add table to system, compute offsets for system components in table it */
static
void add_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_matched_table_t *table_data;
    ecs_type_t table_type = NULL;
    int32_t c, column_count = ecs_vector_count(query->sig.columns);

    if (table) {
        table_type = table->type;
    }

    int32_t trait_cur = 0, trait_count = count_traits(query, table_type);
    
    /* If the query has traits, we need to account for the fact that a table may
     * have multiple components to which the trait is applied, which means the
     * table has to be registered with the query multiple times, with different
     * table columns. If so, allocate a small array for each trait in which the
     * last added table index of the trait is stored, so that in the next 
     * iteration we can start the search from the correct offset type. */
    int32_t *trait_index_offsets = NULL;
    if (trait_count) {
        trait_index_offsets = ecs_os_calloc(ECS_SIZEOF(int32_t) * column_count);
    }


    /* From here we recurse */
add_trait:

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    if (table && has_auto_activation(query)) {
        table_type = table->type;
        table_data = ecs_vector_add(&query->empty_tables, ecs_matched_table_t);

        #ifndef NDEBUG
        char *type_expr = ecs_type_str(world, table->type);
        ecs_trace_2("query #[green]%s#[reset] matched with table #[green][%s]",
            query_name(world, query), type_expr);
        ecs_os_free(type_expr);
        #endif
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_data = ecs_vector_add(
            &query->tables, ecs_matched_table_t);
    }

    table_data->table = table;
    table_data->references = NULL;
    table_data->columns = NULL;
    table_data->components = NULL;
    table_data->monitor = NULL;

    /* If grouping is enabled for query, assign the group rank to the table */
    group_table(world, query, table_data);

    if (column_count) {
        /* Array that contains the system column to table column mapping */
        table_data->columns = ecs_os_malloc(ECS_SIZEOF(int32_t) * column_count);
        ecs_assert(table_data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Store the components of the matched table. In the case of OR expressions,
        * components may differ per matched table. */
        table_data->components = ecs_os_malloc(ECS_SIZEOF(ecs_entity_t) * column_count);
        ecs_assert(table_data->components != NULL, ECS_OUT_OF_MEMORY, NULL);
    }

    /* Walk columns parsed from the system signature */
    ecs_sig_column_t *columns = ecs_vector_first(
        query->sig.columns, ecs_sig_column_t);

    for (c = 0; c < column_count; c ++) {
        ecs_sig_column_t *column = &columns[c];
        ecs_entity_t entity = 0, component = 0;
        ecs_sig_oper_kind_t op = column->oper_kind;
        ecs_sig_from_kind_t from = column->from_kind;

        if (op == EcsOperNot) {
            from = EcsFromEmpty;
        }

        table_data->columns[c] = 0;

        /* Get actual component and component source for current column */
        get_comp_and_src(world, query, table_type, column, op, from, &component, 
            &entity);

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromParent) and is not just a handle */
        if (!entity && from != EcsFromEmpty) {
            int32_t index = get_component_index(
                world, table_type, &component, c, op, trait_index_offsets);
            
            table_data->columns[c] = index;
        }

        /* Check if a the component is a reference. If 'entity' is set, the
         * component must be resolved from another entity, which is the case
         * for FromEntity and FromContainer. 
         * 
         * If no entity is set but the component is not found in the table, it
         * must come from a prefab. This is guaranteed, as at this point it is
         * already validated that the table matches with the system.
         * 
         * If the column from is Cascade, there may not be an entity in case the
         * current table contains root entities. In that case, still add a
         * reference field. The application can, after the table has matched,
         * change the set of components, so that this column will turn into a
         * reference. Having the reference already linked to the system table
         * makes changing this administation easier when the change happens.
         */
        if ((entity || table_data->columns[c] == -1 || from == EcsCascade)) {
            add_ref(world, query, table_type, c, table_data, component, entity, 
                from);
        }

        table_data->components[c] = component;
    }

    if (table) {
        int32_t matched_table_index = 0;
        if (!has_auto_activation(query)) {
            /* If query doesn't automatically activates/inactivates tables, the
             * table has been added to query->tables, and we can get the count
             * to determine the current table index. */
            matched_table_index = ecs_vector_count(query->tables) - 1;
            ecs_assert(matched_table_index >= 0, ECS_INTERNAL_ERROR, NULL);
        }
        
        ecs_table_register_query(world, table, query, matched_table_index);
    }

    /* Use tail recursion when adding table for multiple traits */
    trait_cur ++;
    if (trait_cur < trait_count) {
        goto add_trait;
    }

    if (trait_index_offsets) {
        ecs_os_free(trait_index_offsets);
    }
}

static
bool match_column(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_sig_from_kind_t from_kind,
    ecs_entity_t component,
    ecs_entity_t source,
    ecs_match_failure_t *failure_info)
{
    if (from_kind == EcsFromAny) {
        failure_info->reason = EcsMatchFromSelf;
        return ecs_type_has_entity(world, type, component);
        
    } else if (from_kind == EcsFromOwned) {
        failure_info->reason = EcsMatchFromOwned;
        return ecs_type_owns_entity(world, type, component, true);

    } else if (from_kind == EcsFromShared) {
        failure_info->reason = EcsMatchFromShared;
        return !ecs_type_owns_entity(world, type, component, true) &&
            ecs_type_owns_entity(world, type, component, false);

    } else if (from_kind == EcsFromParent) {
        failure_info->reason = EcsMatchFromContainer;
        return ecs_find_in_type(world, type, component, ECS_CHILDOF) != 0;

    } else if (from_kind == EcsFromEntity) {
        failure_info->reason = EcsMatchFromEntity;
        ecs_type_t source_type = ecs_get_type(world, source);
        return ecs_type_has_entity(world, source_type, component);
    } else {
        return true;
    }
}

/* Match table with system */
bool ecs_query_match(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    ecs_match_failure_t *failure_info)
{
    /* Prevent having to add if not null checks everywhere */
    ecs_match_failure_t tmp_failure_info;
    if (!failure_info) {
        failure_info = &tmp_failure_info;
    }

    failure_info->reason = EcsMatchOk;
    failure_info->column = 0;

    if (!(query->flags & EcsQueryNeedsTables)) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t type, table_type = table->type;

    /* Don't match disabled entities */
    if (!(query->flags & EcsQueryMatchDisabled) && ecs_type_owns_entity(
        world, table_type, EcsDisabled, true))
    {
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    /* Don't match prefab entities */
    if (!(query->flags & EcsQueryMatchPrefab) && ecs_type_owns_entity(
        world, table_type, EcsPrefab, true))
    {
        failure_info->reason = EcsMatchEntityIsPrefab;
        return false;
    }

    /* Check if trait cardinality matches traits in query, if any */
    if (count_traits(query, table->type) == -1) {
        return false;
    }

    int32_t i, column_count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);

    for (i = 0; i < column_count; i ++) {
        ecs_sig_column_t *elem = &columns[i];
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        failure_info->column = i + 1;

        if (oper_kind == EcsOperAnd) {
            if (!match_column(
                world, table_type, from_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperNot) {
            if (match_column(
                world, table_type, from_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;

            if (from_kind == EcsFromAny) {
                if (!ecs_type_contains(
                    world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    return false;
                }
            } else if (from_kind == EcsFromOwned) {
                if (!ecs_type_contains(
                    world, table_type, type, false, false))
                {
                    failure_info->reason = EcsMatchOrFromOwned;
                    return false;
                }
            } else if (from_kind == EcsFromShared) {
                if (ecs_type_contains(
                        world, table_type, type, false, false) ||
                    !ecs_type_contains(
                        world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromShared;
                    return false;
                }                                
            } else if (from_kind == EcsFromParent) {
                if (!(table->flags & EcsTableHasParent)) {
                    failure_info->reason = EcsMatchOrFromContainer;
                    return false;
                }

                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    failure_info->reason = EcsMatchOrFromContainer;
                    return false;
                }
            }
        }
    }

    return true;
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int32_t i, count = ecs_sparse_count(world->stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(
            world->stage.tables, ecs_table_t, i);

        if (ecs_query_match(world, table, query, NULL)) {
            add_table(world, query, table);
        }
    }

    order_ranked_tables(world, query);
}

/** Get index of table in system's matched tables */
static
int32_t get_table_param_index(
    ecs_table_t *table,
    ecs_vector_t *tables)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

    return i;
}

/** Check if a table was matched with the system */
static
int32_t table_matched(
    ecs_vector_t *tables,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            return i;
        }
    }

    return -1;
}

static
void resolve_cascade_container(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t table_data_index,
    ecs_type_t table_type)
{
    ecs_matched_table_t *table_data = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_data_index);
    
    ecs_assert(table_data->references != 0, ECS_INTERNAL_ERROR, NULL);

    /* Obtain reference index */
    int32_t *column_indices = table_data->columns;
    int32_t column = query->cascade_by - 1;
    int32_t ref_index = -column_indices[column] - 1;

    /* Obtain pointer to the reference data */
    ecs_ref_t *references = ecs_vector_first(table_data->references, ecs_ref_t);
    ecs_assert(ref_index < ecs_vector_count(table_data->references), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_ref_t *ref = &references[ref_index];
    ecs_assert(ref->component == get_cascade_component(query), 
        ECS_INTERNAL_ERROR, NULL);

    /* Resolve container entity */
    ecs_entity_t container = ecs_find_in_type(
        world, table_type, ref->component, ECS_CHILDOF);    

    /* If container was found, update the reference */
    if (container) {
        references[ref_index].entity = container;
        ecs_get_ref_w_entity(
            world, &references[ref_index], container, 
            ref->component);
    } else {
        references[ref_index].entity = 0;
    }
}

#define ELEM(ptr, size, index) ECS_OFFSET(ptr, size * index)

static
int32_t qsort_partition(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,    
    int32_t elem_size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{
    int32_t p = (hi + lo) / 2;
    void *pivot = ELEM(ptr, elem_size, p);
    ecs_entity_t pivot_e = entities[p];
    int32_t i = lo - 1, j = hi + 1;
    void *el;    

repeat:
    {
        do {
            i ++;
            el = ELEM(ptr, elem_size, i);
        } while ( compare(entities[i], el, pivot_e, pivot) < 0);

        do {
            j --;
            el = ELEM(ptr, elem_size, j);
        } while ( compare(entities[j], el, pivot_e, pivot) > 0);

        if (i >= j) {
            return j;
        }

        ecs_table_swap(world, &world->stage, table, data, i, j);

        if (p == i) {
            pivot = ELEM(ptr, elem_size, j);
            pivot_e = entities[j];
        } else if (p == j) {
            pivot = ELEM(ptr, elem_size, i);
            pivot_e = entities[i];
        }

        goto repeat;
    }
}

static
void qsort_array(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,
    int32_t size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{   
    if ((hi - lo) < 1)  {
        return;
    }   

    int32_t p = qsort_partition(
        world, table, data, entities, ptr, size, lo, hi, compare);

    qsort_array(world, table, data, entities, ptr, size, lo, p, compare);

    qsort_array(world, table, data, entities, ptr, size, p + 1, hi, compare); 
}

static
void sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_index,
    ecs_compare_action_t compare)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data || !data->entities) {
        /* Nothing to sort */
        return;
    }

    int32_t count = ecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_column_t *column = &data->columns[column_index];
        size = column->size;
        ptr = ecs_vector_first_t(column->data, size, column->alignment);
    }

    qsort_array(world, table, data, entities, ptr, size, 0, count - 1, compare);
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_matched_table_t *table;
    ecs_entity_t *entities;
    void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
} sort_helper_t;

static
void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    return ELEM(helper->ptr, helper->elem_size, helper->row);
}

static
ecs_entity_t e_from_helper(
    sort_helper_t *helper)
{
    if (helper->row < helper->count) {
        return helper->entities[helper->row];
    } else {
        return 0;
    }
}

static
void build_sorted_table_range(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t start,
    int32_t end)
{
    ecs_entity_t component = query->sort_on_component;
    ecs_compare_action_t compare = query->compare;

    /* Fetch data from all matched tables */
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    sort_helper_t *helper = ecs_os_malloc((end - start) * ECS_SIZEOF(sort_helper_t));

    int i, to_sort = 0;
    for (i = start; i < end; i ++) {
        ecs_matched_table_t *table = &tables[i];
        ecs_data_t *data = ecs_table_get_data(world, table->table);
        if (!data || !data->entities || !ecs_table_count(table->table)) {
            continue;
        }

        int32_t index = ecs_type_index_of(table->table->type, component);
        if (index != -1) {
            ecs_column_t *column = &data->columns[index];
            int16_t size = column->size;
            int16_t align = column->alignment;
            helper[to_sort].ptr = ecs_vector_first_t(column->data, size, align);
            helper[to_sort].elem_size = size;
        }

        helper[to_sort].table = table;
        helper[to_sort].entities = ecs_vector_first(data->entities, ecs_entity_t);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table->table);
        to_sort ++;
    }

    ecs_table_range_t *cur = NULL;

    bool proceed;
    do {
        int32_t j, min = 0;
        proceed = true;

        ecs_entity_t e1;
        while (!(e1 = e_from_helper(&helper[min]))) {
            min ++;
            if (min == to_sort) {
                proceed = false;
                break;
            }
        }

        if (!proceed) {
            break;
        }

        for (j = min + 1; j < to_sort; j++) {
            ecs_entity_t e2 = e_from_helper(&helper[j]);
            if (!e2) {
                continue;
            }

            void *ptr1 = ptr_from_helper(&helper[min]);
            void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
            }
        }

        sort_helper_t *cur_helper = &helper[min];

        if (!cur || cur->table != cur_helper->table) {
            cur = ecs_vector_add(
                &query->table_ranges, ecs_table_range_t);
            ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);
            cur->table = cur_helper->table;
            cur->start_row = cur_helper->row;
            cur->count = 1;
        } else {
            cur->count ++;
        }

        cur_helper->row ++;
    } while (proceed);

    ecs_os_free(helper);
}

static
void build_sorted_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    /* Clean previous sorted tables */
    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_matched_table_t *table = NULL;

    int32_t start = 0, rank = 0;
    for (i = 0; i < count; i ++) {
        table = &tables[i];
        if (rank != table->rank) {
            if (start != i) {
                build_sorted_table_range(world, query, start, i);
                start = i;
            }
            rank = table->rank;
        }
    }

    if (start != i) {
        build_sorted_table_range(world, query, start, i);
    }
}

static
bool tables_dirty(
    ecs_query_t *query)
{
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    bool is_dirty = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            is_dirty = is_dirty || (dirty_state[t] != m_table->monitor[t]);
        }
    }

    is_dirty = is_dirty || (query->match_count != query->prev_match_count);

    return is_dirty;
}

static
void tables_reset_dirty(
    ecs_query_t *query)
{
    query->prev_match_count = query->match_count;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        if (!m_table->monitor) {
            /* If one table doesn't have a monitor, none of the tables will have
             * a monitor, so early out. */
            return;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            m_table->monitor[t] = dirty_state[t];
        }
    }
}

static
void sort_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_compare_action_t compare = query->compare;
    if (!compare) {
        return;
    }
    
    ecs_entity_t sort_on_component = query->sort_on_component;

    /* Iterate over active tables. Don't bother with inactive tables, since
     * they're empty */
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    bool tables_sorted = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);

        is_dirty = is_dirty || (dirty_state[0] != m_table->monitor[0]);

        int32_t index = -1;
        if (sort_on_component) {
            /* Get index of sorted component. We only care if the component we're
            * sorting on has changed or if entities have been added / re(moved) */
            index = ecs_type_index_of(table->type, sort_on_component);
            ecs_assert(index != -1, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(index < ecs_vector_count(table->type), ECS_INTERNAL_ERROR, NULL); 
            is_dirty = is_dirty || (dirty_state[index + 1] != m_table->monitor[index + 1]);
        }      
        
        /* Check both if entities have moved (element 0) or if the component
         * we're sorting on has changed (index + 1) */
        if (is_dirty) {
            /* Sort the table */
            sort_table(world, table, index, compare);
            tables_sorted = true;
        }
    }

    if (tables_sorted || query->match_count != query->prev_match_count) {
        build_sorted_tables(world, query);
        query->match_count ++; /* Increase version if tables changed */
    }
}

static
bool has_refs(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        ecs_sig_from_kind_t from_kind = columns[i].from_kind;

        if (columns[i].oper_kind == EcsOperNot && from_kind == EcsFromEmpty) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromId to
             * prevent resolving the ref */
            return true;
        } else if (from_kind != EcsFromAny && from_kind != EcsFromEmpty) {
            /* If the component is not from the entity being iterated over, and
             * the column is not just passing an id, it must be a reference to
             * another entity. */
            return true;
        }
    }

    return false;
}

static
bool has_traits(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        if (is_column_trait(&columns[i])) {
            return true;
        }
    }

    return false;    
}

static
void register_monitors(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_vector_each(query->sig.columns, ecs_sig_column_t, column, {
        /* If component is requested with CASCADE source register component as a
         * parent monitor. Parent monitors keep track of whether an entity moved
         * in the hierarchy, which potentially requires the query to reorder its
         * tables. 
         * Also register a regular component monitor for EcsCascade columns.
         * This ensures that when the component used in the CASCADE column
         * is added or removed tables are updated accordingly*/
        if (column->from_kind == EcsCascade) {
            if (column->oper_kind != EcsOperOr) {
                ecs_component_monitor_register(
                    &world->parent_monitors, column->is.component, query);

                ecs_component_monitor_register(
                    &world->component_monitors, column->is.component, query);
            } else {
                ecs_vector_each(column->is.type, ecs_entity_t, e_ptr, {
                    ecs_component_monitor_register(
                        &world->parent_monitors, *e_ptr, query);

                    ecs_component_monitor_register(
                        &world->component_monitors, *e_ptr, query);
                });
            }

        /* FromSelf also requires registering a monitor, as FromSelf columns can
         * be matched with prefabs. The only column kinds that do not require
         * registering a monitor are FromOwned and FromNothing. */
        } else if (column->from_kind == EcsFromAny || 
            column->from_kind == EcsFromShared ||
            column->from_kind == EcsFromEntity ||
            column->from_kind == EcsFromParent)
        {
            if (column->oper_kind != EcsOperOr) {
                ecs_component_monitor_register(
                    &world->component_monitors, column->is.component, query);
            } else {
                ecs_vector_each(column->is.type, ecs_entity_t, e_ptr, {
                    ecs_component_monitor_register(
                        &world->component_monitors, *e_ptr, query);
                });
            }
        }
    });
}

static
void process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int i, count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        ecs_sig_column_t *column = &columns[i];
        ecs_sig_oper_kind_t op = column->oper_kind; 
        ecs_sig_from_kind_t from = column->from_kind; 

        if (!(query->flags & EcsQueryMatchDisabled)) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsDisabled,
                 * signal that disabled entities should be matched. By default,
                 * disabled entities are not matched. */
                if (ecs_type_owns_entity(
                    world, column->is.type, EcsDisabled, true))
                {
                    query->flags |= EcsQueryMatchDisabled;
                }         
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == EcsDisabled) {
                    query->flags |= EcsQueryMatchDisabled;
                }
            }
        }

        if (!(query->flags & EcsQueryMatchPrefab)) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsPrefab,
                * signal that disabled entities should be matched. By default,
                * prefab entities are not matched. */
                if (ecs_type_owns_entity(
                    world, column->is.type, EcsPrefab, true))
                {
                    query->flags |= EcsQueryMatchPrefab;
                }            
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == EcsPrefab) {
                    query->flags |= EcsQueryMatchPrefab;
                }
            }
        }

        if (from == EcsFromAny || 
            from == EcsFromOwned ||
            from == EcsFromShared ||
            from == EcsFromParent) 
        {
            query->flags |= EcsQueryNeedsTables;
        }

        if (from == EcsCascade) {
            query->cascade_by = i + 1;
            query->rank_on_component = column->is.component;
        }

        if (from == EcsFromEntity) {
            ecs_assert(column->source != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_set_watch(world, &world->stage, column->source);
        }
    }

    query->flags |= (ecs_flags32_t)(has_refs(&query->sig) * EcsQueryHasRefs);
    query->flags |= (ecs_flags32_t)(has_traits(&query->sig) * EcsQueryHasTraits);

    register_monitors(world, query);
}


/* -- Private API -- */

void ecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (ecs_query_match(world, table, query, NULL)) {
        add_table(world, query, table);
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_query_activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active)
{
    ecs_vector_t *src_array, *dst_array;

    if (active) {
        src_array = query->empty_tables;
        dst_array = query->tables;
    } else {
        src_array = query->tables;
        dst_array = query->empty_tables;
    }

    int32_t i = get_table_param_index(table, src_array);
    int32_t src_count = ecs_vector_move_index(
        &dst_array, src_array, ecs_matched_table_t, i);

    if (active) {
        query->tables = dst_array;
    } else {
        query->empty_tables = dst_array;
    }

    /* Activate system if registered with query */
#ifdef FLECS_SYSTEMS_H
    if (query->system) {
        int32_t dst_count = ecs_vector_count(dst_array);
        if (active) {
            if (dst_count == 1) {
                ecs_system_activate(world, query->system, true);
            }
        } else if (src_count == 0) {
            ecs_system_activate(world, query->system, false);
        }
    }
#else
    (void)src_count;
#endif

    order_ranked_tables(world, query);
}

static
void free_matched_table(
    ecs_matched_table_t *table)
{
    ecs_os_free(table->columns);
    ecs_os_free(table->components);
    ecs_vector_free(table->references);
    ecs_os_free(table->monitor);
}

/* Remove table */
static
void remove_table(
    ecs_vector_t *tables,
    int32_t index)
{
    ecs_matched_table_t *table = ecs_vector_get(
        tables, ecs_matched_table_t, index);
    free_matched_table(table);
    ecs_vector_remove_index(tables, ecs_matched_table_t, index);
}

/* Rematch system with tables after a change happened to a watched entity */
void ecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_trace_1("rematch query %s", query_name(world, query));

    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        /* Is the system currently matched with the table? */
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        int32_t match = table_matched(query->tables, table);

        if (ecs_query_match(world, table, query, NULL)) {
            /* If the table matches, and it is not currently matched, add */
            if (match == -1) {
                if (table_matched(query->empty_tables, table) == -1) {
                    add_table(world, query, table);
                }

            /* If table still matches and has cascade column, reevaluate the
                * sources of references. This may have changed in case 
                * components were added/removed to container entities */ 
            } else if (query->cascade_by) {
                resolve_cascade_container(
                    world, query, match, table->type);
            }
        } else {
            /* If table no longer matches, remove it */
            if (match != -1) {
                remove_table(query->tables, match);
            } else {
                /* Make sure the table is removed if it was inactive */
                match = table_matched(
                    query->empty_tables, table);
                if (match != -1) {
                    remove_table(query->empty_tables, match);
                }
            }
        }
    }

    group_tables(world, query);

    order_ranked_tables(world, query);

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    if (query->system) {
        if (ecs_sig_check_constraints(world, &query->sig)) {
            ecs_remove_entity(world, query->system, EcsDisabledIntern);
        } else {
            ecs_add_entity(world, query->system, EcsDisabledIntern);
        }
    }
}


/* -- Public API -- */

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig)
{
    ecs_query_t *result = ecs_sparse_add(world->queries, ecs_query_t);
    memset(result, 0, sizeof(ecs_query_t));
    result->world = world;
    result->sig = *sig;
    result->tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->empty_tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->system = system;
    result->match_count = 0;
    result->prev_match_count = -1;
    result->flags = 0;

    process_signature(world, result);

    ecs_trace_2("query #[green]%s#[reset] created with expression #[red]%s", 
        query_name(world, result), result->sig.expr);

    ecs_log_push();

    if (result->flags & EcsQueryNeedsTables) {
        if (ecs_has_entity(world, system, EcsMonitor)) {
            result->flags |= EcsQueryMonitor;
        }
        
        if (ecs_has_entity(world, system, EcsOnSet)) {
            result->flags |= EcsQueryOnSet;
        }

        if (ecs_has_entity(world, system, EcsUnSet)) {
            result->flags |= EcsQueryUnSet;
        }        

        match_tables(world, result);
    } else {
        /* Add stub table that resolves references (if any) so everything is
         * preprocessed when the query is evaluated. */
        add_table(world, result, NULL);
    }

    if (result->cascade_by) {
        result->group_table = rank_by_depth;
    }

    ecs_log_pop();

    /* Make sure application can't try to free sig resources */
    *sig = (ecs_sig_t){ 0 };

    return result;
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    ecs_sig_t sig = { 0 };
    ecs_sig_init(world, NULL, expr, &sig);
    return ecs_query_new_w_sig(world, 0, &sig);
}

void ecs_query_free(
    ecs_query_t *query)
{
    ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
        free_matched_table(table);
    });

    ecs_vector_each(query->tables, ecs_matched_table_t, table, {
        free_matched_table(table);
    });

    ecs_vector_free(query->tables);
    ecs_vector_free(query->empty_tables);
    ecs_vector_free(query->table_ranges);
    ecs_sig_deinit(&query->sig);
}

/* Create query iterator */
ecs_iter_t ecs_query_iter_page(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);

    sort_tables(query->world, query);

    tables_reset_dirty(query);

    int32_t table_count;
    if (query->table_ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_query_iter_t it = {
        .query = query,
        .offset = offset,
        .limit = limit,
        .remaining = limit,
        .index = 0,
    };

    return (ecs_iter_t){
        .world = query->world,
        .query = query,
        .column_count = ecs_vector_count(query->sig.columns),
        .table_count = table_count,
        .inactive_table_count = ecs_vector_count(query->empty_tables),
        .iter.query = it
    };
}

ecs_iter_t ecs_query_iter(
    ecs_query_t *query)
{
    return ecs_query_iter_page(query, 0, 0);
}

void ecs_query_set_iter(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count)
{
    ecs_matched_table_t *table = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_index);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *world_table = table->table;
    ecs_data_t *table_data = ecs_table_get_staged_data(world, stage, world_table);
    ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities, ecs_entity_t);  
    it->entities = &entity_buffer[row];

    it->world = world;
    it->query = query;
    it->column_count = ecs_vector_count(query->sig.columns);
    it->table_count = 1;
    it->inactive_table_count = 0;
    it->table = world_table;
    it->table_columns = table_data->columns;
    it->columns = table->columns;
    it->components = table->components;
    it->references = ecs_vector_first(table->references, ecs_ref_t);
    it->offset = row;
    it->count = count;
    it->total_count = count;
}

/* Return next table */
bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_query_iter_t *iter = &it->iter.query;
    ecs_query_t *query = iter->query;
    ecs_world_t *world = it->world;

    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_table_range_t *ranges = ecs_vector_first(query->table_ranges, ecs_table_range_t);

    ecs_assert(!ranges || query->compare, ECS_INTERNAL_ERROR, NULL);
    
    int32_t table_count;
    if (ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_get_stage(&world);

    int32_t offset = iter->offset;
    int32_t limit = iter->limit;
    int32_t remaining = iter->remaining;
    int32_t prev_count = it->total_count;
    bool offset_limit = (offset | limit) != 0;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table;

        if (ranges) {
            table = ranges[i].table;
        } else {
            table = &tables[i];
        }

        ecs_table_t *world_table = table->table;
        ecs_data_t *table_data = NULL;
        int32_t first = 0, count = 0;

        if (world_table) {
            table_data = ecs_table_get_data(world, world_table);
            ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
            it->table_columns = table_data->columns;
            
            if (ranges) {
                first = ranges[i].start_row;
                count = ranges[i].count;
            } else {
                count = ecs_table_count(world_table);
            }
        }

        if (table_data) {
            if (offset_limit) {
                if (offset) {
                    if (offset > count) {
                        /* No entities to iterate in current table */
                        offset = iter->offset -= count;
                        continue;
                    } else {
                        first += offset;
                        count -= offset;
                        offset = iter->offset = 0;
                    }
                }

                if (remaining) {
                    if (remaining > count) {
                        remaining = iter->remaining -= count;
                    } else {
                        count = remaining;
                        remaining = iter->remaining = 0;
                    }
                } else if (limit) {
                    /* Limit hit: no more entities left to iterate */
                    return false;
                }
            }

            if (!count) {
                /* No entities to iterate in current table */
                continue;
            }

            ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities, ecs_entity_t); 
            it->entities = &entity_buffer[first];
            it->offset = first;
            it->count = count;
            it->total_count = count;
        }

        it->table = world_table;
        it->columns = table->columns;
        it->components = table->components;
        it->references = ecs_vector_first(table->references, ecs_ref_t);
        it->frame_offset += prev_count;

        /* Table is ready to be iterated, return it struct */
        iter->index = i + 1;

        return true;
    }

    return false;
}

bool ecs_query_next_worker(
    ecs_iter_t *it,
    int32_t current,
    int32_t total)
{
    int32_t per_worker, first, prev_offset = it->offset;

    do {
        if (!ecs_query_next(it)) {
            return false;
        }

        int32_t count = it->count;
        per_worker = count / total;
        first = per_worker * current;

        count -= per_worker * total;

        if (count) {
            if (current < count) {
                per_worker ++;
                first += current;
            } else {
                first += count;
            }
        }

        if (!per_worker && !(it->query->flags & EcsQueryNeedsTables)) {
            if (current == 0) {
                return true;
            } else {
                return false;
            }
        }
    } while (!per_worker);

    it->frame_offset -= prev_offset;
    it->count = per_worker;
    it->offset += first;
    it->entities = &it->entities[first];
    it->frame_offset += first;

    return true;
}

void ecs_query_order_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_compare_action_t compare)
{
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->sort_on_component = sort_component;
    query->compare = compare;

    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    sort_tables(world, query);    

    if (!query->table_ranges) {
        build_sorted_tables(world, query);
    }
}

void ecs_query_group_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_rank_type_action_t group_table_action)
{
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->rank_on_component = sort_component;
    query->group_table = group_table_action;

    group_tables(world, query);

    order_ranked_tables(world, query);

    build_sorted_tables(world, query);
}

bool ecs_query_changed(
    ecs_query_t *query)
{
    return tables_dirty(query);
}
