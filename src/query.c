#include "private_api.h"

#ifdef FLECS_SYSTEMS_H
#include "modules/system/system.h"
#endif

static
void activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active);

static
ecs_entity_t components_contains(
    const ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    ecs_vector_each(table_type, ecs_entity_t, c_ptr, {
        ecs_entity_t entity = *c_ptr;

        if (ECS_HAS_RELATION(entity, EcsChildOf)) {
            entity = ECS_PAIR_OBJECT(entity);

            ecs_record_t *record = ecs_eis_get(world, entity);
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
        ecs_record_t *record = ecs_eis_get(world, entity);
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
        if (ECS_HAS_RELATION(array[i], EcsChildOf)) {
            ecs_type_t c_type = ecs_get_type(world, ecs_pair_object(world, array[i]));
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
        } else if (!(array[i] & ECS_ROLE_MASK)) {
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
bool has_auto_activation(
    ecs_query_t *q)
{
    /* Only a basic query with no additional features does table activation */
    return !(q->flags & EcsQueryNoActivation);
}

static
void order_ranked_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_table) {
        ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);       

        /* Recompute the table indices by first resetting all indices, and then
         * re-adding them one by one. */
        if (has_auto_activation(query)) { 
            ecs_map_iter_t it = ecs_map_iter(query->table_indices);
            ecs_table_indices_t *ti;
            while ((ti = ecs_map_next(&it, ecs_table_indices_t, NULL))) {
                /* If table is registered, it must have at least one index */
                int32_t count = ti->count;
                ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
                (void)count;

                /* Only active tables are reordered, so don't reset inactive 
                 * tables */
                if (ti->indices[0] >= 0) {
                    ti->count = 0;
                }
            }
        }

        /* Re-register monitors after tables have been reordered. This will update
         * the table administration with the new matched_table ids, so that when a
         * monitor is executed we can quickly find the right matched_table. */
        if (query->flags & EcsQueryMonitor) { 
            ecs_vector_each(query->tables, ecs_matched_table_t, table, {        
                ecs_table_notify(world, table->iter_data.table, &(ecs_table_event_t){
                    .kind = EcsTableQueryMatch,
                    .query = query,
                    .matched_table_index = table_i
                });
            });
        }

        /* Update table index */
        if (has_auto_activation(query)) {
            ecs_vector_each(query->tables, ecs_matched_table_t, table, {  
                ecs_table_indices_t *ti = ecs_map_get(query->table_indices, 
                    ecs_table_indices_t, table->iter_data.table->id);

                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                ti->indices[ti->count] = table_i;
                ti->count ++;
            });
        }
    }
    
    query->match_count ++;
    query->needs_reorder = false;
}

static
void group_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_matched_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (query->group_table) {
        ecs_assert(table->iter_data.table != NULL, ECS_INTERNAL_ERROR, NULL);
        table->rank = query->group_table(
            world, query->rank_on_component, table->iter_data.table->type);
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
        if (op == EcsOperAnd || op == EcsOperNot || op == EcsOperOptional) {
            component = column->is.component;
        } else if (op == EcsOperAll) {
            component = column->is.component & ECS_COMPONENT_MASK;
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
                world, table_type, component, EcsChildOf);

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

typedef struct trait_offset_t {
    int32_t index;
    int32_t count;
} trait_offset_t;

/* Get index for specified trait. Take into account that a trait can be matched
 * multiple times per table, by keeping an offset of the last found index */
static
int32_t get_trait_index(
    ecs_type_t table_type,
    ecs_entity_t component,
    int32_t column_index,
    trait_offset_t *trait_offsets,
    int32_t count)
{
    int32_t result;

    /* The count variable keeps track of the number of times a trait has been
     * matched with the current table. Compare the count to check if the index
     * was already resolved for this iteration */
    if (trait_offsets[column_index].count == count) {
        /* If it was resolved, return the last stored index. Subtract one as the
         * index is offset by one, to ensure we're not getting stuck on the same
         * index. */
        result = trait_offsets[column_index].index - 1;
    } else {
        /* First time for this iteration that the trait index is resolved, look
         * it up in the type. */
        result = ecs_type_pair_index_of(table_type, 
            trait_offsets[column_index].index, component);
        trait_offsets[column_index].index = result + 1;
        trait_offsets[column_index].count = count;
    }
    
    return result;
}

static
int32_t get_component_index(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_type_t table_type,
    ecs_entity_t *component_out,
    int32_t column_index,
    ecs_sig_oper_kind_t op,
    trait_offset_t *trait_offsets,
    int32_t count)
{
    int32_t result = 0;
    ecs_entity_t component = *component_out;

    if (component) {
        /* If requested component is a case, find the corresponding switch to
         * lookup in the table */
        if (ECS_HAS_ROLE(component, CASE)) {
            result = ecs_table_switch_from_case(
                world, table, component);
            ecs_assert(result != -1, ECS_INTERNAL_ERROR, NULL);

            result += table->sw_column_offset;
        } else
        if (ECS_HAS_ROLE(component, PAIR)) { 
            /* If only the lo part of the trait identifier is set, interpret it
             * as the trait to match. This will match any instance of the trait
             * on the entity and in a signature looks like "PAIR | MyTrait". */
            if (!ECS_PAIR_RELATION(component)) {
                ecs_assert(trait_offsets != NULL, 
                    ECS_INTERNAL_ERROR, NULL);

                /* Strip the PAIR role */
                component &= ECS_COMPONENT_MASK;

                /* Get index of trait. Start looking from the last trait index
                 * as this may not be the first instance of the trait. */
                result = get_trait_index(
                    table_type, component, column_index, trait_offsets, count);
                
                if (result != -1) {
                    /* If component of current column is a trait, get the actual 
                     * trait type for the table, so the system can see which 
                     * component the trait was applied to */   
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
                /* If trait does have the hi part of the identifier set, this is
                 * a fully qualified trait identifier. In a signature this looks
                 * like "PAIR | MyTrait > Comp". */
                ecs_entity_t lo = ecs_entity_t_lo(component);
                if (lo == EcsWildcard) {
                    ecs_assert(trait_offsets != NULL, ECS_INTERNAL_ERROR, NULL);

                    /* Get id for the trait to lookup by taking the trait from
                     * the high 32 bits, move it to the low 32 bits, and reapply
                     * the PAIR mask. */
                    component = ECS_PAIR_RELATION(component);

                    /* If the low part of the identifier is the wildcard entity,
                     * this column is requesting the component to which the 
                     * trait is applied. First, find the component identifier */
                    result = get_trait_index(table_type, component, 
                        column_index, trait_offsets, count);

                    /* Type must have the trait, otherwise table would not have
                     * matched */
                    ecs_assert(result != -1, ECS_INTERNAL_ERROR, NULL);

                    /* Get component id at returned index */
                    ecs_entity_t *trait = ecs_vector_get(
                        table_type, ecs_entity_t, result);
                    ecs_assert(trait != NULL, ECS_INTERNAL_ERROR, NULL);

                    /* Get the lower part of the trait id. This is the component
                     * we're looking for. */
                    component = ecs_entity_t_lo(*trait);
                    *component_out = component;

                    /* Now lookup the component as usual */
                }

                /* If the low part is a regular entity (component), then
                 * this query exactly matches a single trait instance. In
                 * this case we can simply do a lookup of the trait 
                 * identifier in the table type. */
                result = ecs_type_index_of(table_type, component);
            }
        } else {
            /* Get column index for component */
            result = ecs_type_index_of(table_type, component);
        }

        /* If column is found, add one to the index, as column zero in
        * a table is reserved for entity id's */
        if (result != -1) {
            result ++;
        }

        /* Check if component is a tag. If it is, set table_data to
        * zero, so that a system won't try to access the data */
        if (!ECS_HAS_ROLE(component, CASE) && 
            !ECS_HAS_ROLE(component, SWITCH)) 
        {            
            component = ecs_get_typeid(world, component);

            if (component) {
                const EcsComponent *data = ecs_get(
                    world, component, EcsComponent);

                if (!data || !data->size) {
                    result = 0;
                }
            } else {
                result = 0;
            }
        }        

        /* ecs_table_column_offset may return -1 if the component comes
         * from a prefab. If so, the component will be resolved as a
         * reference (see below) */           
    }

    if (op == EcsOperAll) {
        result = 0;
    } else if (op == EcsOperOptional) {
        /* If table doesn't have the field, mark it as no data */
        if (!ecs_type_has_id(world, table_type, component)) {
            result = 0;
        }
    }

    return result;
}

static
ecs_vector_t* add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_type_t table_type,
    ecs_vector_t *references,
    ecs_entity_t component,
    ecs_entity_t entity,
    ecs_sig_from_kind_t from)
{
    const EcsComponent *c_info = ecs_get(world, component, EcsComponent);
    
    ecs_entity_t e;
    ecs_ref_t *ref = ecs_vector_add(&references, ecs_ref_t);

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
                ecs_get_ref_w_id(
                    world, ref, e, component);
                ecs_set_watch(world, e);                     
            }

            query->flags |= EcsQueryHasRefs;
        }
    }

    return references;
}

static
ecs_entity_t is_column_trait(
    ecs_sig_column_t *column)
{
    ecs_sig_from_kind_t from_kind = column->from_kind;
    ecs_sig_oper_kind_t oper_kind = column->oper_kind;

    /* For now traits are only supported on owned columns */
    if (from_kind == EcsFromOwned && oper_kind == EcsOperAnd) {
        ecs_entity_t c = column->is.component;
        if (ECS_HAS_ROLE(c, PAIR)) {
            if (!ECS_PAIR_RELATION(c)) {
                return c;
            } else
            if (ECS_PAIR_OBJECT(c) == EcsWildcard) {
                return ECS_PAIR_RELATION(c);
            }
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

    trait &= ECS_COMPONENT_MASK;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_ROLE(e, PAIR)) {
            if (ECS_PAIR_RELATION(e) == trait) {
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
    const ecs_query_t *query,
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

static
ecs_type_t get_column_type(
    ecs_world_t *world,
    ecs_sig_oper_kind_t oper_kind,
    ecs_entity_t component)
{
    if (oper_kind == EcsOperAll) {
        const EcsType *type = ecs_get(world, component, EcsType);
        ecs_assert(type != NULL, ECS_INVALID_PARAMETER, NULL);
        return type->normalized;
    } else {
        return ecs_type_from_id(world, component);
    }    
}

/** Add table to system, compute offsets for system components in table it */
static
void add_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
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
    trait_offset_t *trait_offsets = NULL;
    if (trait_count) {
        trait_offsets = ecs_os_calloc(
            ECS_SIZEOF(trait_offset_t) * column_count);
    }

    /* From here we recurse */
    int32_t *table_indices = NULL;
    int32_t table_indices_count = 0;
    int32_t matched_table_index = 0;
    ecs_matched_table_t table_data;
    ecs_vector_t *references = NULL;

add_trait:
    table_data = (ecs_matched_table_t){ .iter_data.table = table };
    if (table) {
        table_type = table->type;
    }

    /* If grouping is enabled for query, assign the group rank to the table */
    group_table(world, query, &table_data);

    if (column_count) {
        /* Array that contains the system column to table column mapping */
        table_data.iter_data.columns = ecs_os_malloc(ECS_SIZEOF(int32_t) * column_count);
        ecs_assert(table_data.iter_data.columns != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Store the components of the matched table. In the case of OR expressions,
        * components may differ per matched table. */
        table_data.iter_data.components = ecs_os_malloc(ECS_SIZEOF(ecs_entity_t) * column_count);
        ecs_assert(table_data.iter_data.components != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Also cache types, so no lookup is needed while iterating */
        table_data.iter_data.types = ecs_os_malloc(ECS_SIZEOF(ecs_type_t) * column_count);
        ecs_assert(table_data.iter_data.types != NULL, ECS_OUT_OF_MEMORY, NULL);        
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

        table_data.iter_data.columns[c] = 0;

        /* Get actual component and component source for current column */
        get_comp_and_src(world, query, table_type, column, op, from, &component, 
            &entity);

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromParent) and is not just a handle */
        if (!entity && from != EcsFromEmpty) {
            int32_t index = get_component_index(world, table, table_type, 
                &component, c, op, trait_offsets, trait_cur + 1);

            if (index == -1) {
                if (from == EcsFromOwned && op == EcsOperOptional) {
                    index = 0;
                }
            } else {
                if (from == EcsFromShared && op == EcsOperOptional) {
                    index = 0;
                }
            }

            table_data.iter_data.columns[c] = index;

            /* If the column is a case, we should only iterate the entities in
             * the column for this specific case. Add a sparse column with the
             * case id so we can find the correct entities when iterating */
            if (ECS_HAS_ROLE(component, CASE)) {
                ecs_sparse_column_t *sc = ecs_vector_add(
                    &table_data.sparse_columns, ecs_sparse_column_t);
                sc->signature_column_index = c;
                sc->sw_case = component & ECS_COMPONENT_MASK;
                sc->sw_column = NULL;
            }

            /* If table has a disabled bitmask for components, check if there is
             * a disabled column for the queried for component. If so, cache it
             * in a vector as the iterator will need to skip the entity when the
             * component is disabled. */
            if (index && (table && table->flags & EcsTableHasDisabled)) {
                ecs_entity_t bs_id = 
                    (component & ECS_COMPONENT_MASK) | ECS_DISABLED;
                int32_t bs_index = ecs_type_index_of(table->type, bs_id);
                if (bs_index != -1) {
                    ecs_bitset_column_t *elem = ecs_vector_add(
                        &table_data.bitset_columns, ecs_bitset_column_t);
                    elem->column_index = bs_index;
                    elem->bs_column = NULL;
                }
            }
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
        if ((entity || table_data.iter_data.columns[c] == -1 || from == EcsCascade)) {
            references = add_ref(world, query, table_type, references, 
                component, entity, from);
            table_data.iter_data.columns[c] = -ecs_vector_count(references);
        }

        table_data.iter_data.components[c] = component;
        table_data.iter_data.types[c] = get_column_type(world, op, component);
    }

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */

    ecs_matched_table_t *table_elem;
    if (table && has_auto_activation(query)) {
        table_elem = ecs_vector_add(&query->empty_tables, 
            ecs_matched_table_t);

        /* Store table index */
        matched_table_index = ecs_vector_count(query->empty_tables);
        table_indices_count ++;
        table_indices = ecs_os_realloc(
            table_indices, table_indices_count * ECS_SIZEOF(int32_t));
        table_indices[table_indices_count - 1] = -matched_table_index;

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
        table_elem = ecs_vector_add(&query->tables, ecs_matched_table_t);

        /* If query doesn't automatically activates/inactivates tables, we can 
         * get the count to determine the current table index. */
        matched_table_index = ecs_vector_count(query->tables) - 1;
        ecs_assert(matched_table_index >= 0, ECS_INTERNAL_ERROR, NULL);
    }

    if (references) {
        ecs_size_t ref_size = ECS_SIZEOF(ecs_ref_t) * ecs_vector_count(references);
        table_data.iter_data.references = ecs_os_malloc(ref_size);
        ecs_os_memcpy(table_data.iter_data.references, 
            ecs_vector_first(references, ecs_ref_t), ref_size);
        ecs_vector_free(references);
        references = NULL;
    }

    *table_elem = table_data;

    /* Use tail recursion when adding table for multiple traits */
    trait_cur ++;
    if (trait_cur < trait_count) {
        goto add_trait;
    }

    /* Register table indices before sending out the match signal. This signal
     * can cause table activation, and table indices are needed for that. */
    if (table_indices) {
        ecs_table_indices_t *ti = ecs_map_ensure(
            query->table_indices, ecs_table_indices_t, table->id);
        if (ti->indices) {
            ecs_os_free(ti->indices);
        }
        ti->indices = table_indices;
        ti->count = table_indices_count;
    }

    if (table && !(query->flags & EcsQueryIsSubquery)) {
        ecs_table_notify(world, table, &(ecs_table_event_t){
            .kind = EcsTableQueryMatch,
            .query = query,
            .matched_table_index = matched_table_index
        });
    } else if (table && ecs_table_count(table)) {
        activate_table(world, query, table, true);
    }

    if (trait_offsets) {
        ecs_os_free(trait_offsets);
    }
}

static
bool match_column(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_sig_from_kind_t from_kind,
    ecs_entity_t component,
    ecs_entity_t source,
    ecs_match_failure_t *failure_info)
{
    if (from_kind == EcsFromAny) {
        failure_info->reason = EcsMatchFromSelf;
        return ecs_type_has_id(world, type, component);
        
    } else if (from_kind == EcsFromOwned) {
        failure_info->reason = EcsMatchFromOwned;
        return ecs_type_owns_id(world, type, component, true);

    } else if (from_kind == EcsFromShared) {
        failure_info->reason = EcsMatchFromShared;
        return !ecs_type_owns_id(world, type, component, true) &&
            ecs_type_owns_id(world, type, component, false);

    } else if (from_kind == EcsFromParent) {
        failure_info->reason = EcsMatchFromContainer;
        return ecs_find_in_type(world, type, component, EcsChildOf) != 0;

    } else if (from_kind == EcsFromEntity) {
        failure_info->reason = EcsMatchFromEntity;
        ecs_type_t source_type = ecs_get_type(world, source);
        return ecs_type_has_id(world, source_type, component);
    } else {
        return true;
    }
}

/* Match table with system */
bool ecs_query_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_query_t *query,
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
    if (!(query->flags & EcsQueryMatchDisabled) && ecs_type_owns_id(
        world, table_type, EcsDisabled, true))
    {
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    /* Don't match prefab entities */
    if (!(query->flags & EcsQueryMatchPrefab) && ecs_type_owns_id(
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

        } else if (oper_kind == EcsOperOr || oper_kind == EcsOperAll) {
            bool match_all = oper_kind == EcsOperAll;
            if (match_all) {
                const EcsType *type_ptr = ecs_get(world, elem->is.component, EcsType);
                type = type_ptr->normalized;
            } else {
                type = elem->is.type;
            }

            if (from_kind == EcsFromAny) {
                if (!ecs_type_contains(
                    world, table_type, type, match_all, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    return false;
                }
            } else if (from_kind == EcsFromOwned) {
                if (!ecs_type_contains(
                    world, table_type, type, match_all, false))
                {
                    failure_info->reason = EcsMatchOrFromOwned;
                    return false;
                }
            } else if (from_kind == EcsFromShared) {
                if (ecs_type_contains(
                        world, table_type, type, match_all, false) ||
                    !ecs_type_contains(
                        world, table_type, type, match_all, true))
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
                    world, table_type, type, NULL, match_all))
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
    int32_t i, count = ecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(
            world->store.tables, ecs_table_t, i);

        if (ecs_query_match(world, table, query, NULL)) {
            add_table(world, query, table);
        }
    }

    order_ranked_tables(world, query);
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

        ecs_table_swap(world, table, data, i, j);

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
    ecs_data_t *data = ecs_table_get_data(table);
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
    const void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
    bool shared;
} sort_helper_t;

static
const void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->elem_size >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->row >= 0, ECS_INTERNAL_ERROR, NULL);
    if (helper->shared) {
        return helper->ptr;
    } else {
        return ELEM(helper->ptr, helper->elem_size, helper->row);
    }
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
    ecs_query_t *query,
    int32_t start,
    int32_t end)
{
    ecs_world_t *world = query->world;
    ecs_entity_t component = query->sort_on_component;
    ecs_compare_action_t compare = query->compare;

    /* Fetch data from all matched tables */
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    sort_helper_t *helper = ecs_os_malloc((end - start) * ECS_SIZEOF(sort_helper_t));

    int i, to_sort = 0;
    for (i = start; i < end; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->iter_data.table;
        ecs_data_t *data = ecs_table_get_data(table);
        ecs_vector_t *entities;
        if (!data || !(entities = data->entities) || !ecs_table_count(table)) {
            continue;
        }

        int32_t index = ecs_type_index_of(table->type, component);
        if (index != -1) {
            ecs_column_t *column = &data->columns[index];
            int16_t size = column->size;
            int16_t align = column->alignment;
            helper[to_sort].ptr = ecs_vector_first_t(column->data, size, align);
            helper[to_sort].elem_size = size;
            helper[to_sort].shared = false;
        } else if (component) {
            /* Find component in prefab */
            ecs_entity_t base = ecs_find_entity_in_prefabs(
                world, 0, table->type, component, 0);
            
            /* If a base was not found, the query should not have allowed using
             * the component for sorting */
            ecs_assert(base != 0, ECS_INTERNAL_ERROR, NULL);

            const EcsComponent *cptr = ecs_get(world, component, EcsComponent);
            ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);

            helper[to_sort].ptr = ecs_get_w_id(world, base, component);
            helper[to_sort].elem_size = cptr->size;
            helper[to_sort].shared = true;
        } else {
            helper[to_sort].ptr = NULL;
            helper[to_sort].elem_size = 0;
            helper[to_sort].shared = false;
        }

        helper[to_sort].table = table_data;
        helper[to_sort].entities = ecs_vector_first(entities, ecs_entity_t);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table);
        to_sort ++;      
    }

    ecs_table_slice_t *cur = NULL;

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

            const void *ptr1 = ptr_from_helper(&helper[min]);
            const void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
                e1 = e_from_helper(&helper[min]);
            }
        }

        sort_helper_t *cur_helper = &helper[min];
        if (!cur || cur->table != cur_helper->table) {
            cur = ecs_vector_add(&query->table_slices, ecs_table_slice_t);
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
    ecs_query_t *query)
{
    /* Clean previous sorted tables */
    ecs_vector_free(query->table_slices);
    query->table_slices = NULL;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_matched_table_t *table = NULL;

    int32_t start = 0, rank = 0;
    for (i = 0; i < count; i ++) {
        table = &tables[i];
        if (rank != table->rank) {
            if (start != i) {
                build_sorted_table_range(query, start, i);
                start = i;
            }
            rank = table->rank;
        }
    }

    if (start != i) {
        build_sorted_table_range(query, start, i);
    }
}

static
bool tables_dirty(
    ecs_query_t *query)
{
    if (query->needs_reorder) {
        order_ranked_tables(query->world, query);
    }

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, 
        ecs_matched_table_t);
    bool is_dirty = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->iter_data.table;

        if (!table_data->monitor) {
            table_data->monitor = ecs_table_get_monitor(table);
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            is_dirty = is_dirty || (dirty_state[t] != table_data->monitor[t]);
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
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->iter_data.table;

        if (!table_data->monitor) {
            /* If one table doesn't have a monitor, none of the tables will have
             * a monitor, so early out. */
            return;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            table_data->monitor[t] = dirty_state[t];
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
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);
    bool tables_sorted = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->iter_data.table;

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!table_data->monitor) {
            table_data->monitor = ecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);

        is_dirty = is_dirty || (dirty_state[0] != table_data->monitor[0]);

        int32_t index = -1;
        if (sort_on_component) {
            /* Get index of sorted component. We only care if the component we're
            * sorting on has changed or if entities have been added / re(moved) */
            index = ecs_type_index_of(table->type, sort_on_component);
            if (index != -1) {
                ecs_assert(index < ecs_vector_count(table->type), ECS_INTERNAL_ERROR, NULL); 
                is_dirty = is_dirty || (dirty_state[index + 1] != table_data->monitor[index + 1]);
            } else {
                /* Table does not contain component which means the sorted
                 * component is shared. Table does not need to be sorted */
                continue;
            }
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
        build_sorted_tables(query);
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
        ecs_sig_inout_kind_t inout = column->inout_kind;

        if (inout != EcsIn) {
            query->flags |= EcsQueryHasOutColumns;
        }

        if (op == EcsOperOptional) {
            query->flags |= EcsQueryHasOptional;
        }        

        if (!(query->flags & EcsQueryMatchDisabled)) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsDisabled,
                 * signal that disabled entities should be matched. By default,
                 * disabled entities are not matched. */
                if (ecs_type_owns_id(
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
                if (ecs_type_owns_id(
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
            ecs_set_watch(world, column->source);
        }
    }

    query->flags |= (ecs_flags32_t)(has_refs(&query->sig) * EcsQueryHasRefs);
    query->flags |= (ecs_flags32_t)(has_traits(&query->sig) * EcsQueryHasTraits);

    if (!(query->flags & EcsQueryIsSubquery)) {
        register_monitors(world, query);
    }
}

static
bool match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (ecs_query_match(world, table, query, NULL)) {
        add_table(world, query, table);
        return true;
    }
    return false;
}

/* Move table from empty to non-empty list, or vice versa */
static
int32_t move_table(
    ecs_query_t *query,
    ecs_table_t *table,
    int32_t index,
    ecs_vector_t **dst_array,
    ecs_vector_t *src_array,
    bool activate)
{
    (void)table;

    int32_t new_index = 0;
    int32_t last_src_index = ecs_vector_count(src_array) - 1;
    ecs_assert(last_src_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_matched_table_t *mt = ecs_vector_last(src_array, ecs_matched_table_t);
    
    /* The last table of the source array will be moved to the location of the
     * table to move, do some bookkeeping to keep things consistent. */
    if (last_src_index) {
        ecs_table_indices_t *ti = ecs_map_get(query->table_indices, 
            ecs_table_indices_t, mt->iter_data.table->id);

        int i, count = ti->count;
        for (i = 0; i < count; i ++) {
            int32_t old_index = ti->indices[i];
            if (activate) {
                if (old_index >= 0) {
                    /* old_index should be negative if activate is true, since
                     * we're moving from the empty list to the non-empty list. 
                     * However, if the last table in the source array is also
                     * the table being moved, this can happen. */
                    ecs_assert(table == mt->iter_data.table, 
                        ECS_INTERNAL_ERROR, NULL);
                    continue;
                }
                /* If activate is true, src = the empty list, and index should
                 * be negative. */
                old_index = old_index * -1 - 1; /* Normalize */
            }

            /* Ensure to update correct index, as there can be more than one */
            if (old_index == last_src_index) {
                if (activate) {
                    ti->indices[i] = index * -1 - 1;
                } else {
                    ti->indices[i] = index;
                }
                break;
            }
        }

        /* If the src array contains tables, there must be a table that will get
         * moved. */
        ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);
    } else {
        /* If last_src_index is 0, the table to move was the only table in the
         * src array, so no other administration needs to be updated. */
    }

    /* Actually move the table. Only move from src to dst if we have a
     * dst_array, otherwise just remove it from src. */
    if (dst_array) {
        new_index = ecs_vector_count(*dst_array);
        ecs_vector_move_index(dst_array, src_array, ecs_matched_table_t, index);

        /* Make sure table is where we expect it */
        mt = ecs_vector_last(*dst_array, ecs_matched_table_t);
        ecs_assert(mt->iter_data.table == table, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vector_count(*dst_array) == (new_index + 1), 
            ECS_INTERNAL_ERROR, NULL);  
    } else {
        ecs_vector_remove_index(src_array, ecs_matched_table_t, index);
    }

    /* Ensure that src array has now one element less */
    ecs_assert(ecs_vector_count(src_array) == last_src_index, 
        ECS_INTERNAL_ERROR, NULL);

    /* Return new index for table */
    if (activate) {
        /* Table is now active, index is positive */
        return new_index;
    } else {
        /* Table is now inactive, index is negative */
        return new_index * -1 - 1;
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
static
void activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active)
{    
    ecs_vector_t *src_array, *dst_array;
    int32_t activated = 0;
    int32_t prev_dst_count = 0;
    (void)world;
    (void)prev_dst_count; /* Only used when built with systems module */

    if (active) {
        src_array = query->empty_tables;
        dst_array = query->tables;
        prev_dst_count = ecs_vector_count(dst_array);
    } else {
        src_array = query->tables;
        dst_array = query->empty_tables;
    }

    ecs_table_indices_t *ti = ecs_map_get(
        query->table_indices, ecs_table_indices_t, table->id);

    if (ti) {
        int32_t i, count = ti->count;
        for (i = 0; i < count; i ++) {
            int32_t index = ti->indices[i];

            if (index < 0) {
                if (!active) {
                    /* If table is already inactive, no need to move */
                    continue;
                }
                index = index * -1 - 1;
            } else {
                if (active) {
                    /* If table is already active, no need to move */
                    continue;
                }
            }

            ecs_matched_table_t *mt = ecs_vector_get(
                src_array, ecs_matched_table_t, index);
            ecs_assert(mt->iter_data.table == table, ECS_INTERNAL_ERROR, NULL);
            (void)mt;
            
            activated ++;

            ti->indices[i] = move_table(
                query, table, index, &dst_array, src_array, active);
        }

        if (activated) {
            /* Activate system if registered with query */
#ifdef FLECS_SYSTEMS_H
            if (query->system) {
                int32_t dst_count = ecs_vector_count(dst_array);
                if (active) {
                    if (!prev_dst_count && dst_count) {
                        ecs_system_activate(world, query->system, true, NULL);
                    }
                } else if (ecs_vector_count(src_array) == 0) {
                    ecs_system_activate(world, query->system, false, NULL);
                }
            }
#endif
        }

        if (active)  {
            query->tables = dst_array;
        } else {
            query->empty_tables = dst_array;
        }
    }
    
    if (!activated) {
        /* Received an activate event for a table we're not matched with. This
         * can only happen if this is a subquery */
        ecs_assert((query->flags & EcsQueryIsSubquery) != 0, 
            ECS_INTERNAL_ERROR, NULL);
        return;
    }

    /* Signal query it needs to reorder tables. Doing this in place could slow
     * down scenario's where a large number of tables is matched with an ordered
     * query. Since each table would trigger the activate signal, there would be
     * as many sorts as added tables, vs. only one when ordering happens when an
     * iterator is obtained. */
    query->needs_reorder = true;
}

static
void add_subquery(
    ecs_world_t *world, 
    ecs_query_t *parent, 
    ecs_query_t *subquery) 
{
    ecs_query_t **elem = ecs_vector_add(&parent->subqueries, ecs_query_t*);
    *elem = subquery;

    /* Iterate matched tables, match them with subquery */
    ecs_matched_table_t *tables = ecs_vector_first(parent->tables, ecs_matched_table_t);
    int32_t i, count = ecs_vector_count(parent->tables);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        match_table(world, subquery, table->iter_data.table);
        activate_table(world, subquery, table->iter_data.table, true);
    }

    /* Do the same for inactive tables */
    tables = ecs_vector_first(parent->empty_tables, ecs_matched_table_t);
    count = ecs_vector_count(parent->empty_tables);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        match_table(world, subquery, table->iter_data.table);
    }    
}

static
void notify_subqueries(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    if (query->subqueries) {
        ecs_query_t **queries = ecs_vector_first(query->subqueries, ecs_query_t*);
        int32_t i, count = ecs_vector_count(query->subqueries);

        ecs_query_event_t sub_event = *event;
        sub_event.parent_query = query;

        for (i = 0; i < count; i ++) {
            ecs_query_t *sub = queries[i];
            ecs_query_notify(world, sub, &sub_event);
        }
    }
}

static
void free_matched_table(
    ecs_matched_table_t *table)
{
    ecs_os_free(table->iter_data.columns);
    ecs_os_free(table->iter_data.components);
    ecs_os_free((ecs_vector_t**)table->iter_data.types);
    ecs_os_free(table->iter_data.references);
    ecs_os_free(table->sparse_columns);
    ecs_os_free(table->bitset_columns);
    ecs_os_free(table->monitor);
}

/** Check if a table was matched with the system */
static
ecs_table_indices_t* get_table_indices(
    ecs_query_t *query,
    ecs_table_t *table)
{
    return ecs_map_get(query->table_indices, ecs_table_indices_t, table->id);
}

static
void resolve_cascade_container(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_indices_t *ti,
    ecs_type_t table_type)
{
    int32_t i, count = ti->count;
    for (i = 0; i < count; i ++) {
        int32_t table_data_index = ti->indices[i];
        ecs_matched_table_t *table_data;

        if (table_data_index >= 0) {
            table_data = ecs_vector_get(
                query->tables, ecs_matched_table_t, table_data_index);            
        } else {
            table_data = ecs_vector_get(
                query->empty_tables, ecs_matched_table_t, 
                    -1 * table_data_index - 1);
        }
        
        ecs_assert(table_data->iter_data.references != 0, ECS_INTERNAL_ERROR, NULL);

        /* Obtain reference index */
        int32_t *column_indices = table_data->iter_data.columns;
        int32_t column = query->cascade_by - 1;
        int32_t ref_index = -column_indices[column] - 1;

        /* Obtain pointer to the reference data */
        ecs_ref_t *references = table_data->iter_data.references;
        ecs_ref_t *ref = &references[ref_index];
        ecs_assert(ref->component == get_cascade_component(query), 
            ECS_INTERNAL_ERROR, NULL);

        /* Resolve container entity */
        ecs_entity_t container = ecs_find_in_type(
            world, table_type, ref->component, EcsChildOf);

        /* If container was found, update the reference */
        if (container) {
            references[ref_index].entity = container;
            ecs_get_ref_w_id(
                world, &references[ref_index], container, 
                ref->component);
        } else {
            references[ref_index].entity = 0;
        }
    }
}

/* Remove table */
static
void remove_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_vector_t *tables,
    int32_t index,
    bool empty)
{
    ecs_matched_table_t *mt = ecs_vector_get(
        tables, ecs_matched_table_t, index);
    if (!mt) {
        /* Query was notified of a table it doesn't match with, this can only
         * happen if query is a subquery. */
        ecs_assert(query->flags & EcsQueryIsSubquery, ECS_INTERNAL_ERROR, NULL);
        return;
    }
    
    ecs_assert(mt->iter_data.table == table, ECS_INTERNAL_ERROR, NULL);
    (void)table;

    /* Free table before moving, as the move will cause another table to occupy
     * the memory of mt */
    free_matched_table(mt);  
    move_table(query, mt->iter_data.table, index, NULL, tables, empty);
}

static
void unmatch_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_table_indices_t *ti)
{
    if (!ti) {
        ti = get_table_indices(query, table);
        if (!ti) {
            return;
        }
    }

    int32_t i, count = ti->count;
    for (i = 0; i < count; i ++) {
        int32_t index = ti->indices[i];
        if (index < 0) {
            index = index * -1 - 1;
            remove_table(query, table, query->empty_tables, index, true);
        } else {
            remove_table(query, table, query->tables, index, false);
        }
    }

    ecs_os_free(ti->indices);
    ecs_map_remove(query->table_indices, table->id);
}

static
void rematch_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_table_indices_t *match = get_table_indices(query, table);

    if (ecs_query_match(world, table, query, NULL)) {
        /* If the table matches, and it is not currently matched, add */
        if (match == NULL) {
            add_table(world, query, table);

        /* If table still matches and has cascade column, reevaluate the
         * sources of references. This may have changed in case 
         * components were added/removed to container entities */ 
        } else if (query->cascade_by) {
            resolve_cascade_container(
                world, query, match, table->type);

        /* If query has optional columns, it is possible that a column that
         * previously had data no longer has data, or vice versa. Do a full
         * rematch to make sure data is consistent. */
        } else if (query->flags & EcsQueryHasOptional) {
            unmatch_table(query, table, match);
            if (!(query->flags & EcsQueryIsSubquery)) {
                ecs_table_notify(world, table, &(ecs_table_event_t){
                    .kind = EcsTableQueryUnmatch,
                    .query = query
                }); 
            }
            add_table(world, query, table);
        }
    } else {
        /* Table no longer matches, remove */
        if (match != NULL) {
            unmatch_table(query, table, match);
            if (!(query->flags & EcsQueryIsSubquery)) {
                ecs_table_notify(world, table, &(ecs_table_event_t){
                    .kind = EcsTableQueryUnmatch,
                    .query = query
                });
            }
            notify_subqueries(world, query, &(ecs_query_event_t){
                .kind = EcsQueryTableUnmatch,
                .table = table
            });
        }
    }
}

/* Rematch system with tables after a change happened to a watched entity */
static
void rematch_tables(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_t *parent_query)
{
    if (parent_query) {
        ecs_matched_table_t *tables = ecs_vector_first(parent_query->tables, ecs_matched_table_t);
        int32_t i, count = ecs_vector_count(parent_query->tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i].iter_data.table;
            rematch_table(world, query, table);
        }

        tables = ecs_vector_first(parent_query->empty_tables, ecs_matched_table_t);
        count = ecs_vector_count(parent_query->empty_tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i].iter_data.table;
            rematch_table(world, query, table);
        }        
    } else {
        ecs_sparse_t *tables = world->store.tables;
        int32_t i, count = ecs_sparse_count(tables);

        for (i = 0; i < count; i ++) {
            /* Is the system currently matched with the table? */
            ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
            rematch_table(world, query, table);
        }
    }

    group_tables(world, query);
    order_ranked_tables(world, query);

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    if (query->system) {
        if (ecs_sig_check_constraints(world, &query->sig)) {
            ecs_remove_id(world, query->system, EcsDisabledIntern);
        } else {
            ecs_add_id(world, query->system, EcsDisabledIntern);
        }
    }
}

static
void remove_subquery(
    ecs_query_t *parent, 
    ecs_query_t *sub)
{
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(sub != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->subqueries != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(parent->subqueries);
    ecs_query_t **sq = ecs_vector_first(parent->subqueries, ecs_query_t*);

    for (i = 0; i < count; i ++) {
        if (sq[i] == sub) {
            break;
        }
    }

    ecs_vector_remove_index(parent->subqueries, ecs_query_t*, i);
}

/* -- Private API -- */

void ecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    bool notify = true;

    switch(event->kind) {
    case EcsQueryTableMatch:
        /* Creation of new table */
        if (match_table(world, query, event->table)) {
            if (query->subqueries) {
                notify_subqueries(world, query, event);
            }
        }
        notify = false;
        break;
    case EcsQueryTableUnmatch:
        /* Deletion of table */
        unmatch_table(query, event->table, NULL);
        break;
    case EcsQueryTableRematch:
        /* Rematch tables of query */
        rematch_tables(world, query, event->parent_query);
        break;        
    case EcsQueryTableEmpty:
        /* Table is empty, deactivate */
        activate_table(world, query, event->table, false);
        break;
    case EcsQueryTableNonEmpty:
        /* Table is non-empty, activate */
        activate_table(world, query, event->table, true);
        break;
    case EcsQueryOrphan:
        ecs_assert(query->flags & EcsQueryIsSubquery, ECS_INTERNAL_ERROR, NULL);
        query->flags |= EcsQueryIsOrphaned;
        query->parent = NULL;
        break;
    }

    if (notify) {
        notify_subqueries(world, query, event);
    }
}

/* -- Public API -- */

static
ecs_query_t* ecs_query_new_w_sig_intern(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig,
    bool is_subquery)
{
    ecs_query_t *result = ecs_os_calloc(sizeof(ecs_query_t));
    result->world = world;
    result->sig = *sig;
    result->table_indices = ecs_map_new(ecs_table_indices_t, 0);
    result->tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->empty_tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->system = system;
    result->prev_match_count = -1;

    if (is_subquery) {
        result->flags |= EcsQueryIsSubquery;
    }

    process_signature(world, result);

    ecs_trace_2("query #[green]%s#[reset] created with expression #[red]%s", 
        query_name(world, result), result->sig.expr);

    ecs_log_push();

    if (!is_subquery) {
        /* Register query with world */
        ecs_query_t **elem = ecs_vector_add(&world->queries, ecs_query_t*);
        *elem = result;

        if (result->flags & EcsQueryNeedsTables) {
            if (system) {
                if (ecs_has_id(world, system, EcsMonitor)) {
                    result->flags |= EcsQueryMonitor;
                }
                
                if (ecs_has_id(world, system, EcsOnSet)) {
                    result->flags |= EcsQueryOnSet;
                }

                if (ecs_has_id(world, system, EcsUnSet)) {
                    result->flags |= EcsQueryUnSet;
                }  
            }      

            match_tables(world, result);
        } else {
            /* Add stub table that resolves references (if any) so everything is
            * preprocessed when the query is evaluated. */
            add_table(world, result, NULL);
        }
    }

    if (result->cascade_by) {
        result->group_table = rank_by_depth;
    }

    ecs_log_pop();

    /* Make sure application can't try to free sig resources */
    *sig = (ecs_sig_t){ 0 };

    return result;
}

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig)
{
    return ecs_query_new_w_sig_intern(world, system, sig, false);
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    ecs_sig_t sig = { 0 };
    ecs_sig_init(world, NULL, expr, &sig);
    return ecs_query_new_w_sig(world, 0, &sig);
}

ecs_query_t* ecs_subquery_new(
    ecs_world_t *world,
    ecs_query_t *parent,
    const char *expr)
{
    ecs_sig_t sig = { 0 };
    ecs_sig_init(world, NULL, expr, &sig);
    ecs_query_t *result = ecs_query_new_w_sig_intern(world, 0, &sig, true);
    result->parent = parent;
    add_subquery(world, parent, result);
    return result;
}

ecs_sig_t* ecs_query_get_sig(
    ecs_query_t *query)
{
    return &query->sig;
}

void ecs_query_free(
    ecs_query_t *query)
{
    ecs_world_t *world = query->world;

    if ((query->flags & EcsQueryIsSubquery) &&
        !(query->flags & EcsQueryIsOrphaned))
    {
        remove_subquery(query->parent, query);
    }

    notify_subqueries(world, query, &(ecs_query_event_t){
        .kind = EcsQueryOrphan
    });

    ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
        if (!(query->flags & EcsQueryIsSubquery)) {
            ecs_table_notify(world, table->iter_data.table, &(ecs_table_event_t){
                .kind = EcsTableQueryUnmatch,
                .query = query
            });
        }    
        free_matched_table(table);
    });

    ecs_vector_each(query->tables, ecs_matched_table_t, table, {
        if (!(query->flags & EcsQueryIsSubquery)) {
            ecs_table_notify(world, table->iter_data.table, &(ecs_table_event_t){
                .kind = EcsTableQueryUnmatch,
                .query = query
            });
        }
        free_matched_table(table);
    });

    ecs_map_iter_t it = ecs_map_iter(query->table_indices);
    ecs_table_indices_t *ti;
    while ((ti = ecs_map_next(&it, ecs_table_indices_t, NULL))) {
        ecs_os_free(ti->indices);
    }
    ecs_map_free(query->table_indices);

    ecs_vector_free(query->subqueries);
    ecs_vector_free(query->tables);
    ecs_vector_free(query->empty_tables);
    ecs_vector_free(query->table_slices);
    ecs_sig_deinit(&query->sig);

    /* Find query in vector */
    if (!(query->flags & EcsQueryIsSubquery) && world->queries) {
        int32_t index = -1;
        ecs_vector_each(world->queries, ecs_query_t*, q_ptr, {
            if (*q_ptr == query) {
                index = q_ptr_i;
            }
        });

        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        ecs_vector_remove_index(world->queries, ecs_query_t*, index);
    }

    ecs_os_free(query);
}

/* Create query iterator */
ecs_iter_t ecs_query_iter_page(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world = query->world;

    if (query->needs_reorder) {
        order_ranked_tables(world, query);
    }
    
    sort_tables(world, query);

    if (!world->is_readonly && query->flags & EcsQueryHasRefs) {
        ecs_eval_component_monitors(world);
    }

    tables_reset_dirty(query);

    int32_t table_count;
    if (query->table_slices) {
        table_count = ecs_vector_count(query->table_slices);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_query_iter_t it = {
        .page_iter = {
            .offset = offset,
            .limit = limit,
            .remaining = limit
        },
        .index = 0,
    };

    return (ecs_iter_t){
        .world = world,
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
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count)
{
    (void)world;
    
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);
    
    ecs_matched_table_t *table_data = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_index);
    ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = table_data->iter_data.table;
    ecs_data_t *data = ecs_table_get_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_entity_t *entity_buffer = ecs_vector_first(data->entities, ecs_entity_t);  
    it->entities = &entity_buffer[row];

    it->world = NULL;
    it->query = query;
    it->column_count = ecs_vector_count(query->sig.columns);
    it->table_count = 1;
    it->inactive_table_count = 0;
    it->table_columns = data->columns;
    it->table = &table_data->iter_data;
    it->offset = row;
    it->count = count;
    it->total_count = count;
}

static
int ecs_page_iter_next(
    ecs_page_iter_t *it,
    ecs_page_cursor_t *cur)
{
    int32_t offset = it->offset;
    int32_t limit = it->limit;
    if (!(offset || limit)) {
        return cur->count == 0;
    }

    int32_t count = cur->count;
    int32_t remaining = it->remaining;

    if (offset) {
        if (offset > count) {
            /* No entities to iterate in current table */
            it->offset -= count;
            return 1;
        } else {
            cur->first += offset;
            count = cur->count -= offset;
            it->offset = 0;
        }
    }

    if (remaining) {
        if (remaining > count) {
            it->remaining -= count;
        } else {
            count = cur->count = remaining;
            it->remaining = 0;
        }
    } else if (limit) {
        /* Limit hit: no more entities left to iterate */
        return -1;
    }

    return count == 0;
}

static
int find_smallest_column(
    ecs_table_t *table,
    ecs_matched_table_t *table_data,
    ecs_vector_t *sparse_columns)
{
    ecs_sparse_column_t *sparse_column_array = 
        ecs_vector_first(sparse_columns, ecs_sparse_column_t);
    int32_t i, count = ecs_vector_count(sparse_columns);
    int32_t min = INT_MAX, index = 0;

    for (i = 0; i < count; i ++) {
        /* The array with sparse queries for the matched table */
        ecs_sparse_column_t *sparse_column = &sparse_column_array[i];

        /* Pointer to the switch column struct of the table */
        ecs_sw_column_t *sc = sparse_column->sw_column;

        /* If the sparse column pointer hadn't been retrieved yet, do it now */
        if (!sc) {
            /* Get the table column index from the signature column index */
            int32_t table_column_index = table_data->iter_data.columns[
                sparse_column->signature_column_index];

            /* Translate the table column index to switch column index */
            table_column_index -= table->sw_column_offset;
            ecs_assert(table_column_index >= 1, ECS_INTERNAL_ERROR, NULL);

            /* Get the sparse column */
            ecs_data_t *data = ecs_table_get_data(table);
            sc = sparse_column->sw_column = 
                &data->sw_columns[table_column_index - 1];
        }

        /* Find the smallest column */
        ecs_switch_t *sw = sc->data;
        int32_t case_count = ecs_switch_case_count(sw, sparse_column->sw_case);
        if (case_count < min) {
            min = case_count;
            index = i + 1;
        }
    }

    return index;
}

static
int sparse_column_next(
    ecs_table_t *table,
    ecs_matched_table_t *matched_table,
    ecs_vector_t *sparse_columns,
    ecs_query_iter_t *iter,
    ecs_page_cursor_t *cur)
{
    bool first_iteration = false;
    int32_t sparse_smallest;

    if (!(sparse_smallest = iter->sparse_smallest)) {
        sparse_smallest = iter->sparse_smallest = find_smallest_column(
            table, matched_table, sparse_columns);
        first_iteration = true;
    }

    sparse_smallest -= 1;

    ecs_sparse_column_t *columns = ecs_vector_first(
        sparse_columns, ecs_sparse_column_t);
    ecs_sparse_column_t *column = &columns[sparse_smallest];
    ecs_switch_t *sw, *sw_smallest = column->sw_column->data;
    ecs_entity_t case_smallest = column->sw_case;

    /* Find next entity to iterate in sparse column */
    int32_t first;
    if (first_iteration) {
        first = ecs_switch_first(sw_smallest, case_smallest);
    } else {
        first = ecs_switch_next(sw_smallest, iter->sparse_first);
    }

    if (first == -1) {
        goto done;
    }    

    /* Check if entity matches with other sparse columns, if any */
    int32_t i, count = ecs_vector_count(sparse_columns);
    do {
        for (i = 0; i < count; i ++) {
            if (i == sparse_smallest) {
                /* Already validated this one */
                continue;
            }

            column = &columns[i];
            sw = column->sw_column->data;

            if (ecs_switch_get(sw, first) != column->sw_case) {
                first = ecs_switch_next(sw_smallest, first);
                if (first == -1) {
                    goto done;
                }
            }
        }
    } while (i != count);

    cur->first = iter->sparse_first = first;
    cur->count = 1;

    return 0;
done:
    /* Iterated all elements in the sparse list, we should move to the
     * next matched table. */
    iter->sparse_smallest = 0;
    iter->sparse_first = 0;

    return -1;
}

#define BS_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

static
int bitset_column_next(
    ecs_table_t *table,
    ecs_vector_t *bitset_columns,
    ecs_query_iter_t *iter,
    ecs_page_cursor_t *cur)
{
    /* Precomputed single-bit test */
    static const uint64_t bitmask[64] = {
    (uint64_t)1 << 0, (uint64_t)1 << 1, (uint64_t)1 << 2, (uint64_t)1 << 3,
    (uint64_t)1 << 4, (uint64_t)1 << 5, (uint64_t)1 << 6, (uint64_t)1 << 7,
    (uint64_t)1 << 8, (uint64_t)1 << 9, (uint64_t)1 << 10, (uint64_t)1 << 11,
    (uint64_t)1 << 12, (uint64_t)1 << 13, (uint64_t)1 << 14, (uint64_t)1 << 15,
    (uint64_t)1 << 16, (uint64_t)1 << 17, (uint64_t)1 << 18, (uint64_t)1 << 19,
    (uint64_t)1 << 20, (uint64_t)1 << 21, (uint64_t)1 << 22, (uint64_t)1 << 23,
    (uint64_t)1 << 24, (uint64_t)1 << 25, (uint64_t)1 << 26, (uint64_t)1 << 27,  
    (uint64_t)1 << 28, (uint64_t)1 << 29, (uint64_t)1 << 30, (uint64_t)1 << 31,
    (uint64_t)1 << 32, (uint64_t)1 << 33, (uint64_t)1 << 34, (uint64_t)1 << 35,  
    (uint64_t)1 << 36, (uint64_t)1 << 37, (uint64_t)1 << 38, (uint64_t)1 << 39,
    (uint64_t)1 << 40, (uint64_t)1 << 41, (uint64_t)1 << 42, (uint64_t)1 << 43,
    (uint64_t)1 << 44, (uint64_t)1 << 45, (uint64_t)1 << 46, (uint64_t)1 << 47,  
    (uint64_t)1 << 48, (uint64_t)1 << 49, (uint64_t)1 << 50, (uint64_t)1 << 51,
    (uint64_t)1 << 52, (uint64_t)1 << 53, (uint64_t)1 << 54, (uint64_t)1 << 55,  
    (uint64_t)1 << 56, (uint64_t)1 << 57, (uint64_t)1 << 58, (uint64_t)1 << 59,
    (uint64_t)1 << 60, (uint64_t)1 << 61, (uint64_t)1 << 62, (uint64_t)1 << 63
    };

    /* Precomputed test to verify if remainder of block is set (or not) */
    static const uint64_t bitmask_remain[64] = {
    BS_MAX, BS_MAX - (BS_MAX >> 63), BS_MAX - (BS_MAX >> 62),
    BS_MAX - (BS_MAX >> 61), BS_MAX - (BS_MAX >> 60), BS_MAX - (BS_MAX >> 59),
    BS_MAX - (BS_MAX >> 58), BS_MAX - (BS_MAX >> 57), BS_MAX - (BS_MAX >> 56), 
    BS_MAX - (BS_MAX >> 55), BS_MAX - (BS_MAX >> 54), BS_MAX - (BS_MAX >> 53), 
    BS_MAX - (BS_MAX >> 52), BS_MAX - (BS_MAX >> 51), BS_MAX - (BS_MAX >> 50), 
    BS_MAX - (BS_MAX >> 49), BS_MAX - (BS_MAX >> 48), BS_MAX - (BS_MAX >> 47), 
    BS_MAX - (BS_MAX >> 46), BS_MAX - (BS_MAX >> 45), BS_MAX - (BS_MAX >> 44), 
    BS_MAX - (BS_MAX >> 43), BS_MAX - (BS_MAX >> 42), BS_MAX - (BS_MAX >> 41), 
    BS_MAX - (BS_MAX >> 40), BS_MAX - (BS_MAX >> 39), BS_MAX - (BS_MAX >> 38), 
    BS_MAX - (BS_MAX >> 37), BS_MAX - (BS_MAX >> 36), BS_MAX - (BS_MAX >> 35), 
    BS_MAX - (BS_MAX >> 34), BS_MAX - (BS_MAX >> 33), BS_MAX - (BS_MAX >> 32), 
    BS_MAX - (BS_MAX >> 31), BS_MAX - (BS_MAX >> 30), BS_MAX - (BS_MAX >> 29), 
    BS_MAX - (BS_MAX >> 28), BS_MAX - (BS_MAX >> 27), BS_MAX - (BS_MAX >> 26), 
    BS_MAX - (BS_MAX >> 25), BS_MAX - (BS_MAX >> 24), BS_MAX - (BS_MAX >> 23), 
    BS_MAX - (BS_MAX >> 22), BS_MAX - (BS_MAX >> 21), BS_MAX - (BS_MAX >> 20), 
    BS_MAX - (BS_MAX >> 19), BS_MAX - (BS_MAX >> 18), BS_MAX - (BS_MAX >> 17), 
    BS_MAX - (BS_MAX >> 16), BS_MAX - (BS_MAX >> 15), BS_MAX - (BS_MAX >> 14), 
    BS_MAX - (BS_MAX >> 13), BS_MAX - (BS_MAX >> 12), BS_MAX - (BS_MAX >> 11), 
    BS_MAX - (BS_MAX >> 10), BS_MAX - (BS_MAX >> 9), BS_MAX - (BS_MAX >> 8), 
    BS_MAX - (BS_MAX >> 7), BS_MAX - (BS_MAX >> 6), BS_MAX - (BS_MAX >> 5), 
    BS_MAX - (BS_MAX >> 4), BS_MAX - (BS_MAX >> 3), BS_MAX - (BS_MAX >> 2),
    BS_MAX - (BS_MAX >> 1)
    };

    int32_t i, count = ecs_vector_count(bitset_columns);
    ecs_bitset_column_t *columns = ecs_vector_first(
        bitset_columns, ecs_bitset_column_t);
    int32_t bs_offset = table->bs_column_offset;

    int32_t first = iter->bitset_first;
    int32_t last = 0;

    for (i = 0; i < count; i ++) {
        ecs_bitset_column_t *column = &columns[i];
        ecs_bs_column_t *bs_column = columns[i].bs_column;

        if (!bs_column) {
            ecs_data_t *data = table->data;
            int32_t index = column->column_index;
            ecs_assert((index - bs_offset >= 0), ECS_INTERNAL_ERROR, NULL);
            bs_column = &data->bs_columns[index - bs_offset];
            columns[i].bs_column = bs_column;
        }
        
        ecs_bitset_t *bs = &bs_column->data;
        int32_t bs_elem_count = bs->count;
        int32_t bs_block = first >> 6;
        int32_t bs_block_count = ((bs_elem_count - 1) >> 6) + 1;

        if (bs_block >= bs_block_count) {
            goto done;
        }

        uint64_t *data = bs->data;
        int32_t bs_start = first & 0x3F;

        /* Step 1: find the first non-empty block */
        uint64_t v = data[bs_block];
        uint64_t remain = bitmask_remain[bs_start];
        while (!(v & remain)) {
            /* If no elements are remaining, move to next block */
            if ((++bs_block) >= bs_block_count) {
                /* No non-empty blocks left */
                goto done;
            }

            bs_start = 0;
            remain = BS_MAX; /* Test the full block */
            v = data[bs_block];
        }

        /* Step 2: find the first non-empty element in the block */
        while (!(v & bitmask[bs_start])) {
            bs_start ++;

            /* Block was not empty, so bs_start must be smaller than 64 */
            ecs_assert(bs_start < 64, ECS_INTERNAL_ERROR, NULL);
        }
        
        /* Step 3: Find number of contiguous enabled elements after start */
        int32_t bs_end = bs_start, bs_block_end = bs_block;
        
        remain = bitmask_remain[bs_end];
        while ((v & remain) == remain) {
            bs_end = 0;
            bs_block_end ++;

            if (bs_block_end == bs_block_count) {
                break;
            }

            v = data[bs_block_end];
            remain = BS_MAX; /* Test the full block */
        }

        /* Step 4: find remainder of enabled elements in current block */
        if (bs_block_end != bs_block_count) {
            while ((v & bitmask[bs_end])) {
                bs_end ++;
            }
        }

        /* Block was not 100% occupied, so bs_start must be smaller than 64 */
        ecs_assert(bs_end < 64, ECS_INTERNAL_ERROR, NULL);

        /* Step 5: translate to element start/end and make sure that each column
         * range is a subset of the previous one. */
        first = bs_block * 64 + bs_start;
        int32_t cur_last = bs_block_end * 64 + bs_end;
        
        /* No enabled elements found in table */
        if (first == cur_last) {
            goto done;
        }

        /* If multiple bitsets are evaluated, make sure each subsequent range
         * is equal or a subset of the previous range */
        if (i) {
            /* If the first element of a subsequent bitset is larger than the
             * previous last value, start over. */
            if (first >= last) {
                i = -1;
                continue;
            }

            /* Make sure the last element of the range doesn't exceed the last
             * element of the previous range. */
            if (cur_last > last) {
                cur_last = last;
            }
        }
        
        last = cur_last;
        int32_t elem_count = last - first;

        /* Make sure last element doesn't exceed total number of elements in 
         * the table */
        if (elem_count > bs_elem_count) {
            elem_count = bs_elem_count;
        }
        
        cur->first = first;
        cur->count = elem_count;
        iter->bitset_first = first;
    }
    
    /* Keep track of last processed element for iteration */ 
    iter->bitset_first = last;

    return 0;
done:
    return -1;
}

static
void mark_columns_dirty(
    ecs_query_t *query,
    ecs_matched_table_t *table_data)
{
    ecs_table_t *table = table_data->iter_data.table;

    if (table && table->dirty_state) {
        int32_t i, count = ecs_vector_count(query->sig.columns);
        ecs_sig_column_t *columns = ecs_vector_first(
            query->sig.columns, ecs_sig_column_t);

        for (i = 0; i < count; i ++) {
            if (columns[i].inout_kind != EcsIn) {
                int32_t table_column = table_data->iter_data.columns[i];
                if (table_column > 0) {
                    table->dirty_state[table_column] ++;
                }
            }
        }
    }
}

/* Return next table */
bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_query_iter_t *iter = &it->iter.query;
    ecs_page_iter_t *piter = &iter->page_iter;
    ecs_query_t *query = it->query;
    ecs_world_t *world = query->world;
    (void)world;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_table_slice_t *slice = ecs_vector_first(
        query->table_slices, ecs_table_slice_t);
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);

    ecs_assert(!slice || query->compare, ECS_INTERNAL_ERROR, NULL);
    
    ecs_page_cursor_t cur;
    int32_t table_count = it->table_count;
    int32_t prev_count = it->total_count;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table_data = slice ? slice[i].table : &tables[i];
        ecs_table_t *table = table_data->iter_data.table;
        ecs_data_t *data = NULL;

        iter->index = i + 1;
        
        if (table) {
            ecs_vector_t *bitset_columns = table_data->bitset_columns;
            ecs_vector_t *sparse_columns = table_data->sparse_columns;
            data = ecs_table_get_data(table);
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
            it->table_columns = data->columns;
            
            if (slice) {
                cur.first = slice[i].start_row;
                cur.count = slice[i].count;                
            } else {
                cur.first = 0;
                cur.count = ecs_table_count(table);
            }

            if (cur.count) {
                if (bitset_columns) {
            
                    if (bitset_column_next(table, bitset_columns, iter, 
                        &cur) == -1) 
                    {
                        /* No more enabled components for table */
                        continue; 
                    } else {
                        iter->index = i;
                    }
                }

                if (sparse_columns) {
                    if (sparse_column_next(table, table_data,
                        sparse_columns, iter, &cur) == -1)
                    {
                        /* No more elements in sparse column */
                        continue;    
                    } else {
                        iter->index = i;
                    }
                }

                int ret = ecs_page_iter_next(piter, &cur);
                if (ret < 0) {
                    return false;
                } else if (ret > 0) {
                    continue;
                }
            } else {
                continue;
            }

            ecs_entity_t *entity_buffer = ecs_vector_first(
                data->entities, ecs_entity_t); 
            it->entities = &entity_buffer[cur.first];
            it->offset = cur.first;
            it->count = cur.count;
            it->total_count = cur.count;
        }

        it->table = &table_data->iter_data;
        it->frame_offset += prev_count;

        if (query->flags & EcsQueryHasOutColumns) {
            if (table) {
                mark_columns_dirty(query, table_data);
            }
        }

        return true;
    }

    return false;
}

bool ecs_query_next_w_filter(
    ecs_iter_t *iter,
    const ecs_filter_t *filter)
{
    ecs_table_t *table;

    do {
        if (!ecs_query_next(iter)) {
            return false;
        }
        table = iter->table->table;
    } while (filter && !ecs_table_match_filter(iter->world, table, filter));
    
    return true;
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
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->sort_on_component = sort_component;
    query->compare = compare;

    ecs_vector_free(query->table_slices);
    query->table_slices = NULL;

    sort_tables(world, query);    

    if (!query->table_slices) {
        build_sorted_tables(query);
    }
}

void ecs_query_group_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_rank_type_action_t group_table_action)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->rank_on_component = sort_component;
    query->group_table = group_table_action;

    group_tables(world, query);

    order_ranked_tables(world, query);

    build_sorted_tables(query);
}

bool ecs_query_changed(
    ecs_query_t *query)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);
    return tables_dirty(query);
}

bool ecs_query_orphaned(
    ecs_query_t *query)
{
    return query->flags & EcsQueryIsOrphaned;
}
