/**
 * @file addons/stats/memory.c
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(ecs_entity_index_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_index_memory_t);
ECS_COMPONENT_DECLARE(ecs_query_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_memory_t);
ECS_COMPONENT_DECLARE(ecs_table_memory_t);
ECS_COMPONENT_DECLARE(ecs_commands_memory_t);
ECS_COMPONENT_DECLARE(EcsWorldMemory);

/* Helper function to calculate memory usage of an ecs_map_t */
static
ecs_size_t ecs_map_memory_get(
    const ecs_map_t *map,
    ecs_size_t element_size)
{
    ecs_size_t result = 0;
    if (map && map->bucket_count > 0) {
        result += map->bucket_count * ECS_SIZEOF(ecs_bucket_t);
        result += ecs_map_count(map) * ECS_SIZEOF(ecs_bucket_entry_t);
        result += ecs_map_count(map) * element_size;
    }
    return result;
}

ecs_entity_index_memory_t ecs_entity_index_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_entity_index_t *index = &world->store.entity_index;
    ecs_entity_index_memory_t result = {0};
    
    /* Get counts */
    result.alive_count = index->alive_count - 1; /* Subtract 1 as index 0 is */
                                                  /* reserved */
    result.not_alive_count = ecs_vec_count(&index->dense) - index->alive_count;
    
    /* Dense array memory: stores uint64_t entity ids */
    ecs_size_t dense_size = ecs_vec_size(&index->dense);
    ecs_size_t dense_count = ecs_vec_count(&index->dense);
    result.bytes_alive = result.alive_count * ECS_SIZEOF(uint64_t);
    result.bytes_not_alive = result.not_alive_count * ECS_SIZEOF(uint64_t);
    result.bytes_unused = (dense_size - dense_count) * ECS_SIZEOF(uint64_t);
    
    /* Add pages vector memory (stores pointers to pages) */
    ecs_size_t pages_size = ecs_vec_size(&index->pages);
    ecs_size_t pages_count = ecs_vec_count(&index->pages);
    result.bytes_unused += (pages_size - pages_count) * 
        ECS_SIZEOF(ecs_entity_index_page_t*);
    
    /* Add allocated pages memory */
    ecs_entity_index_page_t **pages = ecs_vec_first(&index->pages);
    for (int32_t i = 0; i < pages_count; i++) {
        if (pages[i]) {
            result.bytes_alive += ECS_SIZEOF(ecs_entity_index_page_t);
        }
    }
    
error:
    return result;
}

ecs_component_index_memory_t ecs_component_index_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_component_index_memory_t result = {0};
    
    /* Count component records in low range (direct array) */
    int32_t i;
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i++) {
        ecs_component_record_t *cr = world->id_index_lo[i];
        if (!cr) continue;
        
        result.count++;
        result.bytes_component_record += ECS_SIZEOF(ecs_component_record_t);
        
        /* Calculate table cache memory (ecs_table_cache_t) */
        /* Table cache contains: ecs_map_t index + ecs_table_cache_list_t tables */
        ecs_table_cache_t *cache = &cr->cache;
        result.bytes_table_cache += ECS_SIZEOF(ecs_table_cache_t);
        
        /* Map memory: buckets + entries (approximate) */
        ecs_map_t *map = &cache->index;
        result.bytes_table_cache += ecs_map_memory_get(map, 0);
        result.bytes_table_cache += 
            cache->tables.count * ECS_SIZEOF(ecs_table_record_t);
        
        /* Calculate pair record memory if present */
        if (cr->pair) {
            ecs_pair_record_t *pair = cr->pair;
            
            /* Name index memory (hashmap for ChildOf pairs) */
            if (pair->name_index) {
                result.bytes_name_index += ECS_SIZEOF(ecs_hashmap_t);
                /* Hashmap uses internal map for storage */
                ecs_map_t *hm_map = &pair->name_index->impl;
                result.bytes_name_index += ecs_map_memory_get(hm_map, 0);
            }
            
            /* Ordered children vector */
            result.bytes_ordered_children += 
                ecs_vec_size(&pair->ordered_children) * ECS_SIZEOF(ecs_entity_t);
            
            /* Reachable cache */
            ecs_reachable_cache_t *reachable = &pair->reachable;
            result.bytes_reachable_cache += 
                ecs_vec_size(&reachable->ids) * ECS_SIZEOF(ecs_reachable_elem_t);
        }
    }
    
    /* Count component records in high range (stored in map) */
    ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
    while (ecs_map_next(&it)) {
        ecs_component_record_t *cr = ecs_map_ptr(&it);
        if (!cr) continue;
        
        result.count++;
        result.bytes_component_record += ECS_SIZEOF(ecs_component_record_t);
        
        /* Calculate table cache memory */
        ecs_table_cache_t *cache = &cr->cache;
        result.bytes_table_cache += ECS_SIZEOF(ecs_table_cache_t);
        
        /* Map memory: buckets + entries (approximate) */
        ecs_map_t *map = &cache->index;
        result.bytes_table_cache += ecs_map_memory_get(map, 0);
        result.bytes_table_cache += cache->tables.count * ECS_SIZEOF(ecs_table_record_t);
        
        /* Calculate pair record memory if present */
        if (cr->pair) {
            ecs_pair_record_t *pair = cr->pair;
            
            /* Name index memory */
            if (pair->name_index) {
                result.bytes_name_index += ECS_SIZEOF(ecs_hashmap_t);
                /* Hashmap uses internal map for storage */
                ecs_map_t *hm_map = &pair->name_index->impl;
                result.bytes_name_index += ecs_map_memory_get(hm_map, 0);
            }
            
            /* Ordered children vector */
            result.bytes_ordered_children += 
                ecs_vec_size(&pair->ordered_children) * ECS_SIZEOF(ecs_entity_t);
            
            /* Reachable cache */
            ecs_reachable_cache_t *reachable = 
                &pair->reachable;
            result.bytes_reachable_cache += 
                ecs_vec_size(&reachable->ids) * ECS_SIZEOF(ecs_reachable_elem_t);
        }
    }
    
    /* Add memory for the index arrays themselves */
    result.bytes_component_record += 
        FLECS_HI_ID_RECORD_ID * ECS_SIZEOF(ecs_component_record_t*); /* Low range array */
    
    /* High range map memory */
    const ecs_map_t *hi_map = &world->id_index_hi;
    result.bytes_component_record += ecs_map_memory_get(hi_map, 0);
    
error:
    return result;
}

ecs_query_memory_t ecs_query_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_query_memory_t result = (ecs_query_memory_t){0};

    ecs_iter_t it = ecs_each_pair(world, ecs_id(EcsPoly), EcsQuery);
    while (ecs_iter_next(&it)) {
        EcsPoly *queries = ecs_field(&it, EcsPoly, 0);

        for (int32_t i = 0; i < it.count; i++) {
            ecs_query_t *query = queries[i].poly;
            flecs_poly_assert(query, ecs_query_t);
            
            ecs_query_impl_t *impl = flecs_query_impl(query);
            result.count++;

            result.bytes_query += ECS_SIZEOF(ecs_query_impl_t);
            
            if (impl->vars) {
                result.bytes_misc += 
                    impl->var_size * ECS_SIZEOF(ecs_query_var_t);
            }
            
            /* TODO: Variable name indices */
            // result.bytes_query += ecs_map_memory_get(&impl->tvar_index.impl, 0);
            // result.bytes_query += ecs_map_memory_get(&impl->evar_index.impl, 0);
            
            if (impl->src_vars) {
                result.bytes_query += 
                    query->field_count * ECS_SIZEOF(ecs_var_id_t);
            }
            
            if (impl->ops) {
                result.bytes_query += 
                    impl->op_count * ECS_SIZEOF(ecs_query_op_t);
            }
            
            if (impl->tokens) {
                result.bytes_misc += impl->tokens_len;
            }

            if (impl->monitor) {
                result.bytes_misc += query->field_count * ECS_SIZEOF(int32_t);
            }
            
            /* Query cache memory */
            if (impl->cache) {
                ecs_query_cache_t *cache = impl->cache;
                result.bytes_cache += ECS_SIZEOF(ecs_query_cache_t);
                result.bytes_cache += 
                    ecs_map_memory_get(&cache->tables, 
                        ECS_SIZEOF(ecs_query_cache_table_t));
                result.bytes_group_by += 
                    ecs_map_memory_get(&cache->groups, 
                        ECS_SIZEOF(ecs_query_cache_group_t*));
                
                ecs_size_t cache_elem_size = flecs_query_cache_elem_size(cache);
                ecs_query_cache_group_t *cur = cache->first_group;
                do {
                    result.bytes_group_by += 
                        ECS_SIZEOF(ecs_query_cache_group_t);

                    int32_t group_table_count = ecs_vec_count(&cur->tables);
                    result.bytes_cache += group_table_count * cache_elem_size;
                    
                    if (!(cache->query->flags & EcsQueryTrivialCache)) {
                        for (int32_t j = 0; j < group_table_count; j++) {
                            ecs_query_cache_match_t *match = 
                                ecs_vec_get(&cur->tables, cache_elem_size, j);
                            ecs_size_t elem_arrays = 0;
                            elem_arrays += query->field_count * ECS_SIZEOF(ecs_id_t);
                            elem_arrays += query->field_count * ECS_SIZEOF(ecs_entity_t);
                            elem_arrays += query->field_count * ECS_SIZEOF(ecs_table_t*);
                            elem_arrays += query->field_count * ECS_SIZEOF(int32_t);
                            result.bytes_cache += elem_arrays;

                            if (match->wildcard_matches) {
                                int32_t wildcard_count = 
                                    ecs_vec_size(match->wildcard_matches);
                                result.bytes_cache += 
                                    (cache_elem_size + elem_arrays) * wildcard_count;
                            }
                        }
                    }
                    
                    cur = cur->next;
                } while (cur && cur != cache->first_group);
                
                result.bytes_cache += 
                    ecs_vec_size(&cache->table_slices) * 
                        ECS_SIZEOF(ecs_table_range_t);

                if (cache->sources) {
                    result.bytes_cache += 
                        query->field_count * ECS_SIZEOF(ecs_entity_t);
                }

                if (cache->field_map) {
                    result.bytes_cache += query->field_count * ECS_SIZEOF(int8_t);
                }
            }
        }   
    }

error:
    return result;
}

ecs_component_memory_t ecs_component_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_component_memory_t result = {0};
    
    /* Iterate through all tables to calculate table component memory */
    const ecs_sparse_t *tables = &world->store.tables;
    int32_t i, table_count = flecs_sparse_count(tables);
    
    for (i = 0; i < table_count; i++) {
        ecs_table_t *table = flecs_sparse_get_dense_t(tables, ecs_table_t, i);
        if (!table || !table->column_count) {
            continue;
        }
        
        ecs_data_t *data = &table->data;
        int32_t entity_count = data->count;
        int32_t entity_size = data->size;
        result.count += entity_count;
        
        /* Calculate memory for each column */
        for (int32_t col = 0; col < table->column_count; col++) {
            ecs_column_t *column = &data->columns[col];
            if (!column->ti) continue;
            
            ecs_size_t component_size = column->ti->size;
            ecs_size_t used_bytes = entity_count * component_size;
            ecs_size_t allocated_bytes = entity_size * component_size;
            
            result.bytes_table_components += used_bytes;
            result.bytes_table_components_unused += allocated_bytes - used_bytes;
        }
        
        /* Check for bitset columns (toggled components) */
        if (table->_ && table->_->bs_columns) {
            ecs_bitset_t *bitsets = table->_->bs_columns;
            int32_t bitset_count = table->_->bs_count;
            for (int32_t b = 0; b < bitset_count; b++) {
                ecs_bitset_t *bs = &bitsets[b];
                if (bs->data) {
                    result.bytes_table_bitset += bs->size;
                }
            }
        }
        
        /* Add entity storage (entities array) */
        if (data->entities) {
            result.bytes_table_components += entity_count * ECS_SIZEOF(ecs_entity_t);
            result.bytes_table_components_unused += 
                (entity_size - entity_count) * ECS_SIZEOF(ecs_entity_t);
        }
    }
    
    /* Iterate through component records for sparse components */
    /* Low id range (0 to FLECS_HI_ID_RECORD_ID) */
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i++) {
        ecs_component_record_t *cr = world->id_index_lo[i];
        if (!cr || !cr->sparse) continue;
        
        ecs_sparse_t *sparse = (ecs_sparse_t*)cr->sparse;
        if (cr->type_info && cr->type_info->size > 0) {
            result.bytes_sparse_components += 
                flecs_sparse_count(sparse) * cr->type_info->size;
            result.bytes_sparse_components_unused += 
                (ecs_vec_size(&sparse->dense) - flecs_sparse_count(sparse)) * cr->type_info->size;
            result.bytes_sparse_dense_array += 
                ecs_vec_size(&sparse->dense) * ECS_SIZEOF(uint64_t);
        }
    }
    
    /* High id range (stored in map) */
    ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
    while (ecs_map_next(&it)) {
        ecs_component_record_t *cr = ecs_map_ptr(&it);
        if (!cr || !cr->sparse) continue;
        
        ecs_sparse_t *sparse = (ecs_sparse_t*)cr->sparse;
        if (cr->type_info && cr->type_info->size > 0) {
            result.bytes_sparse_components += 
                flecs_sparse_count(sparse) * cr->type_info->size;
            result.bytes_sparse_components_unused += 
                (ecs_vec_size(&sparse->dense) - flecs_sparse_count(sparse)) * cr->type_info->size;
            result.bytes_sparse_dense_array += 
                ecs_vec_size(&sparse->dense) * ECS_SIZEOF(uint64_t);
        }
    }
    
    /* Add builtin component memory (approximate) */
    /* This includes memory for builtin entities and their components */
    result.bytes_builtin = FLECS_HI_COMPONENT_ID * ECS_SIZEOF(ecs_component_record_t*);
    
error:
    return result;
}

ecs_table_memory_t ecs_table_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_memory_t result = {0};
    const ecs_sparse_t *tables = &world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);

    result.count = count;

    for (i = 0; i < count; i++) {
        ecs_table_t *table = flecs_sparse_get_dense_t(tables, ecs_table_t, i);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
        
        int32_t entity_count = table->data.count;
        int32_t entity_size = table->data.size;
        int32_t type_count = table->type.count;
        int32_t column_count = table->column_count;
        
        result.column_count += column_count;
        
        /* Count empty tables */
        if (entity_count == 0) {
            result.empty_count++;
        }
        
        /* Populate entity count histogram using power-of-2 buckets */
        if (entity_count == 0) {
            result.entity_count_histogram[0]++;  /* Bucket 0: 0 entities */
        } else if (entity_count == 1) {
            result.entity_count_histogram[1]++;  /* Bucket 1: 1 entity */
        } else if (entity_count <= ECS_TABLE_MEMORY_HISTOGRAM_MAX_COUNT) {
            /* Find the appropriate power-of-2 bucket */
            /* Buckets: 2-3, 4-7, 8-15, 16-31, ..., 32768-65535 */
            int32_t bucket_index = 2;
            int32_t bucket_min = 2;
            int32_t bucket_max = 3;
            
            while (bucket_max < entity_count && bucket_index < ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT - 2) {
                bucket_index++;
                bucket_min = bucket_max + 1;
                bucket_max = bucket_min * 2 - 1;
            }
            
            result.entity_count_histogram[bucket_index]++;
        } else {
            /* Bucket for >65535 entities */
            result.entity_count_histogram[ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT - 1]++;
        }

        result.bytes_table += ECS_SIZEOF(ecs_table_t);
        if (table->_) {
            result.bytes_table += ECS_SIZEOF(ecs_table__t);
        }
        
        result.bytes_type += type_count * ECS_SIZEOF(ecs_id_t);
    
        result.bytes_entities += entity_count * ECS_SIZEOF(ecs_entity_t);
        result.bytes_entities_unused += 
            (entity_size - entity_count) * ECS_SIZEOF(ecs_entity_t);
        
        if (table->data.overrides) {
            result.bytes_overrides += ECS_SIZEOF(ecs_table_overrides_t);
            result.bytes_overrides += column_count * ECS_SIZEOF(ecs_ref_t);
        }
        
        result.bytes_columns += column_count * ECS_SIZEOF(ecs_column_t);

        if (table->_) {
            result.bytes_table_records += 
                table->_->record_count * ECS_SIZEOF(ecs_table_record_t);
            result.bytes_column_map += 
                (type_count + column_count) * ECS_SIZEOF(int16_t);
            if (table->component_map) {
                result.bytes_component_map += 
                    FLECS_HI_COMPONENT_ID * ECS_SIZEOF(int16_t);
            }
        }

        if (table->dirty_state) {
            result.bytes_dirty_state += column_count * ECS_SIZEOF(int32_t);
        }
        
        ecs_graph_edges_t *add_edges = &table->node.add;
        ecs_graph_edges_t *remove_edges = &table->node.remove;
        
        /* Add edges memory */
        if (add_edges->lo) {
            result.bytes_edges += 
                FLECS_HI_COMPONENT_ID * ECS_SIZEOF(ecs_graph_edge_t);
        }

        if (add_edges->hi) {
            result.bytes_edges += ECS_SIZEOF(ecs_map_t);
            result.bytes_edges += 
                ecs_map_memory_get(add_edges->hi, ECS_SIZEOF(ecs_graph_edge_t));
        }
        
        /* Remove edges memory */
        if (remove_edges->lo) {
            result.bytes_edges += FLECS_HI_COMPONENT_ID * ECS_SIZEOF(ecs_graph_edge_t);
        }
        if (remove_edges->hi) {
            result.bytes_edges += ECS_SIZEOF(ecs_map_t);
            result.bytes_edges += 
                ecs_map_memory_get(remove_edges->hi, ECS_SIZEOF(ecs_graph_edge_t));
        }
    }

error:
    return result;
}

ecs_commands_memory_t ecs_commands_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_commands_memory_t result = {0};
    
    /* Access stages array from world */
    int32_t stage_count = world->stage_count;
    ecs_stage_t **stages = world->stages;
    
    /* Iterate through all stages to collect command memory usage */
    for (int32_t i = 0; i < stage_count; i++) {
        ecs_stage_t *stage = stages[i];
        if (!stage || !stage->cmd) {
            continue;
        }
        
        ecs_commands_t *cmd = stage->cmd;
        
        /* Calculate queue memory (ecs_vec_t) */
        result.bytes_queue += ecs_vec_size(&cmd->queue);
        
        /* Calculate entries memory (ecs_sparse_t) */
        ecs_sparse_t *entries = &cmd->entries;
        result.bytes_entries += ecs_vec_size(&entries->dense) * ECS_SIZEOF(uint64_t);
        result.bytes_entries += ecs_vec_size(&entries->pages) * ECS_SIZEOF(void*);
        
        /* Calculate stack memory (ecs_stack_t) */
        ecs_stack_t *stack = &cmd->stack;
        ecs_stack_page_t *page = stack->first;
        while (page) {
            result.bytes_stack += FLECS_STACK_PAGE_OFFSET + FLECS_STACK_PAGE_SIZE;
            page = page->next;
        }
        
        /* Also check the two command stacks in the stage */
        for (int32_t j = 0; j < 2; j++) {
            ecs_commands_t *cmd_stack = &stage->cmd_stack[j];
            
            /* Queue memory */
            result.bytes_queue += ecs_vec_size(&cmd_stack->queue);
            
            /* Entries memory */
            ecs_sparse_t *stack_entries = &cmd_stack->entries;
            result.bytes_entries += ecs_vec_size(&stack_entries->dense) * ECS_SIZEOF(uint64_t);
            result.bytes_entries += ecs_vec_size(&stack_entries->pages) * ECS_SIZEOF(void*);
            
            /* Stack memory */
            ecs_stack_t *cmd_stack_stack = &cmd_stack->stack;
            ecs_stack_page_t *stack_page = cmd_stack_stack->first;
            while (stack_page) {
                result.bytes_stack += FLECS_STACK_PAGE_OFFSET + FLECS_STACK_PAGE_SIZE;
                stack_page = stack_page->next;
            }
        }
    }

error:
    return result;
}

static
int flecs_world_memory_serialize(
    const ecs_serializer_t *s, 
    const void *ptr)
{
    (void)ptr; /* Unused, we compute values from world */
    const ecs_world_t *world = s->world;

    flecs_poly_assert(world, ecs_world_t);

    EcsWorldMemory value;
    
    value.entity_index = ecs_entity_index_memory_get(world);
    value.component_index = ecs_component_index_memory_get(world);
    value.query = ecs_query_memory_get(world);
    value.component = ecs_component_memory_get(world);
    value.table = ecs_table_memory_get(world);
    value.commands = ecs_commands_memory_get(world);
    
    /* Use component IDs directly */
    s->member(s, "entity_index");
    s->value(s, ecs_id(ecs_entity_index_memory_t), &value.entity_index);
    s->member(s, "component_index");
    s->value(s, ecs_id(ecs_component_index_memory_t), &value.component_index);
    s->member(s, "query");
    s->value(s, ecs_id(ecs_query_memory_t), &value.query);
    s->member(s, "component");
    s->value(s, ecs_id(ecs_component_memory_t), &value.component);
    s->member(s, "table");
    s->value(s, ecs_id(ecs_table_memory_t), &value.table);
    s->member(s, "commands");
    s->value(s, ecs_id(ecs_commands_memory_t), &value.commands);
    
    return 0;
}

void flecs_stats_memory_register_reflection(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsWorldMemory);

#ifdef FLECS_META
    /* Register struct reflection for memory types */
    ecs_id(ecs_entity_index_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "entity_index_memory_t" }),
        .members = {
            { .name = "alive_count", .type = ecs_id(ecs_i32_t) },
            { .name = "not_alive_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_alive", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_not_alive", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_unused", .type = ecs_id(ecs_i32_t), .unit = EcsBytes }
        }
    });

    ecs_id(ecs_component_index_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "component_index_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_component_record", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_table_cache", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_name_index", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_ordered_children", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_reachable_cache", .type = ecs_id(ecs_i32_t), .unit = EcsBytes }
        }
    });

    ecs_id(ecs_query_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "query_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "cached_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_query", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_cache", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_group_by", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_ordered_by", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_plan", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_terms", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_misc", .type = ecs_id(ecs_i32_t), .unit = EcsBytes }
        }
    });

    ecs_id(ecs_component_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "component_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_table_components", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_table_components_unused", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_table_bitset", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_sparse_components", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_sparse_components_unused", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_sparse_dense_array", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_builtin", .type = ecs_id(ecs_i32_t), .unit = EcsBytes }
        }
    });

    ecs_id(ecs_table_memory_t) = ecs_struct(world, {
        .entity = ecs_id(ecs_table_memory_t),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "empty_count", .type = ecs_id(ecs_i32_t) },
            { .name = "column_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_table", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_type", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_entities", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_entities_unused", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_overrides", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_columns", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_table_records", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_column_map", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_component_map", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_dirty_state", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_edges", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "entity_count_histogram", .type = ecs_id(ecs_i32_t), .count = ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT }
        }
    });

    ecs_id(ecs_commands_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "commands_memory_t" }),
        .members = {
            { .name = "bytes_queue", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_entries", .type = ecs_id(ecs_i32_t), .unit = EcsBytes },
            { .name = "bytes_stack", .type = ecs_id(ecs_i32_t), .unit = EcsBytes }
        }
    });

    /* Register EcsWorldMemory as opaque type that computes on demand */
    ecs_entity_t world_memory_struct = ecs_struct(world, {
        .members = {
            { .name = "entity_index", .type = ecs_id(ecs_entity_index_memory_t) },
            { .name = "component_index", .type = ecs_id(ecs_component_index_memory_t) },
            { .name = "query", .type = ecs_id(ecs_query_memory_t) },
            { .name = "component", .type = ecs_id(ecs_component_memory_t) },
            { .name = "table", .type = ecs_id(ecs_table_memory_t) },
            { .name = "commands", .type = ecs_id(ecs_commands_memory_t) }
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(EcsWorldMemory),
        .type.as_type = world_memory_struct,
        .type.serialize = flecs_world_memory_serialize
    });
#endif
}

#endif
