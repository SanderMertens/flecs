/**
 * @file addons/stats/memory.c
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(ecs_entities_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_index_memory_t);
ECS_COMPONENT_DECLARE(ecs_query_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_memory_t);
ECS_COMPONENT_DECLARE(ecs_table_memory_t);
ECS_COMPONENT_DECLARE(ecs_commands_memory_t);
ECS_COMPONENT_DECLARE(ecs_table_histogram_t);
ECS_COMPONENT_DECLARE(ecs_allocator_memory_t);
ECS_COMPONENT_DECLARE(EcsWorldMemory);

/* Helper function to calculate memory usage of an ecs_map_t */
static
ecs_size_t flecs_map_memory_get(
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

static
ecs_size_t flecs_ballocator_memory_get(
    const ecs_block_allocator_t *allocator)
{
    ecs_size_t result = 0;
    (void)allocator;

#ifndef FLECS_USE_OS_ALLOC
    ecs_block_allocator_chunk_header_t *chunk = allocator->head;

    while (chunk) {
        result += allocator->data_size;
        chunk = chunk->next;
    }
#endif

    return result;
}

static
ecs_size_t flecs_allocator_memory_get(
    const ecs_allocator_t *allocator)
{
    ecs_size_t result = 0;
    (void)allocator;
    
#ifndef FLECS_USE_OS_ALLOC
    int32_t i, count = flecs_sparse_count(&allocator->sizes);
    for (i = 0; i < count; i++) {
        ecs_block_allocator_t *ba = flecs_sparse_get_dense_t(
            &allocator->sizes, ecs_block_allocator_t, i);
        result += flecs_ballocator_memory_get(ba);
    }
    result += flecs_ballocator_memory_get(&allocator->chunks);
#endif

    return result;
}

static
void flecs_sparse_memory_get(
    const ecs_sparse_t *sparse,
    ecs_size_t element_size,
    ecs_size_t *used,
    ecs_size_t *unused,
    ecs_size_t *overhead)
{
    *used = 0;
    *unused = 0;
    *overhead = 0;

    if (!sparse) {
        return;
    }

    int32_t count = flecs_sparse_count(sparse);
    int32_t size = ecs_vec_size(&sparse->dense);
    
    *overhead = size * ECS_SIZEOF(uint64_t);

    int32_t i, pages_count = ecs_vec_count(&sparse->pages);
    ecs_sparse_page_t *pages = ecs_vec_first_t(
        &sparse->pages, ecs_sparse_page_t);
    int32_t page_count = 0;
    for (i = 0; i < pages_count; i++) {
        ecs_sparse_page_t *page = &pages[i];
        if (page->sparse) {
            page_count ++;
        }
    }

    *overhead += pages_count * ECS_SIZEOF(ecs_sparse_page_t);
    *overhead += page_count * ECS_SIZEOF(int32_t) * FLECS_SPARSE_PAGE_SIZE;

    int32_t total_size = page_count * element_size * FLECS_SPARSE_PAGE_SIZE;
    *used = count * element_size;
    *unused = total_size - (count * element_size);
}

ecs_entities_memory_t ecs_entity_index_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_entity_index_t *index = &world->store.entity_index;
    ecs_entities_memory_t result = {0};
    
    result.alive_count = index->alive_count - 1;
    result.not_alive_count = ecs_vec_count(&index->dense) - index->alive_count;
    
    /* Dense array */
    int32_t dense_size = ecs_vec_size(&index->dense);
    int32_t dense_count = ecs_vec_count(&index->dense);
    result.bytes_alive = result.alive_count * ECS_SIZEOF(uint64_t);
    result.bytes_not_alive = result.not_alive_count * ECS_SIZEOF(uint64_t);
    result.bytes_unused = (dense_size - dense_count) * ECS_SIZEOF(uint64_t);
    
    /* Pages vector memory */
    int32_t pages_size = ecs_vec_size(&index->pages);
    int32_t pages_count = ecs_vec_count(&index->pages);
    result.bytes_unused += (pages_size - pages_count) * 
        ECS_SIZEOF(ecs_entity_index_page_t*);
    
    /* Pages memory */
    ecs_entity_index_page_t **pages = ecs_vec_first(&index->pages);
    int32_t page_count = 0;
    for (int32_t i = 0; i < pages_count; i++) {
        if (pages[i]) {
            page_count ++;
        }
    }

    ecs_size_t alive_records = ECS_SIZEOF(ecs_record_t) * result.alive_count;
    ecs_size_t not_alive_records = 
        ECS_SIZEOF(ecs_record_t) * result.not_alive_count;
    ecs_size_t total_records = page_count * ECS_SIZEOF(ecs_entity_index_page_t);
    result.bytes_alive += alive_records;
    result.bytes_not_alive += not_alive_records;
    result.bytes_unused += total_records - alive_records - not_alive_records;

    {
        ecs_iter_t it = ecs_each_pair_t(world, EcsIdentifier, EcsWildcard);
        while (ecs_each_next(&it)) {
            EcsIdentifier *ptr = ecs_field(&it, EcsIdentifier, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result.bytes_names += ecs_os_strlen(ptr[i].value) + 1;
            }
        }
    }

#ifdef FLECS_DOC
    {
        ecs_iter_t it = ecs_each_pair_t(world, EcsDocDescription, EcsWildcard);
        while (ecs_each_next(&it)) {
            EcsDocDescription *ptr = ecs_field(&it, EcsDocDescription, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result.bytes_doc_names += ecs_os_strlen(ptr[i].value) + 1;
            }
        }
    }
#endif
    
error:
    return result;
}

static
void flecs_component_index_memory_record_get(
    const ecs_component_record_t *cr,
    ecs_component_index_memory_t *result)
{
    result->count++;

    result->bytes_component_record += ECS_SIZEOF(ecs_component_record_t);
        
    const ecs_table_cache_t *cache = &cr->cache;
    result->bytes_table_cache += ECS_SIZEOF(ecs_table_cache_t);
    
    const ecs_map_t *map = &cache->index;
    result->bytes_table_cache += flecs_map_memory_get(map, 0);
    
    if (cr->pair) {
        ecs_pair_record_t *pair = cr->pair;

        result->bytes_component_record += ECS_SIZEOF(ecs_pair_record_t);
        
        if (pair->name_index) {
            result->bytes_name_index += ECS_SIZEOF(ecs_hashmap_t);
            ecs_map_t *hm_map = &pair->name_index->impl;
            result->bytes_name_index += flecs_map_memory_get(hm_map, 0);
        }
        
        result->bytes_ordered_children += 
            ecs_vec_size(&pair->ordered_children) * ECS_SIZEOF(ecs_entity_t);
        
        const ecs_reachable_cache_t *reachable = &pair->reachable;
        result->bytes_reachable_cache += 
            ecs_vec_size(&reachable->ids) * ECS_SIZEOF(ecs_reachable_elem_t);
    }
}

ecs_component_index_memory_t ecs_component_index_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_component_index_memory_t result = {0};
    
    int32_t i;
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i++) {
        ecs_component_record_t *cr = world->id_index_lo[i];
        if (cr) {
            flecs_component_index_memory_record_get(cr, &result);
        }
    }
    
    ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
    while (ecs_map_next(&it)) {
        ecs_component_record_t *cr = ecs_map_ptr(&it);
        flecs_component_index_memory_record_get(cr, &result);
    }

    it = ecs_map_iter(&world->type_info);
    while (ecs_map_next(&it)) {
        result.bytes_type_info += ECS_SIZEOF(ecs_type_info_t);
    }
    
error:
    return result;
}

static
void flecs_query_memory_get(
    const ecs_query_t *query,
    ecs_query_memory_t *result)
{
    const ecs_query_impl_t *impl = flecs_query_impl(query);

    result->count++;
    result->bytes_query += ECS_SIZEOF(ecs_query_impl_t);

    result->bytes_terms += query->term_count * ECS_SIZEOF(ecs_term_t);
    
    if (impl->ops) {
        result->bytes_plan += 
            impl->op_count * ECS_SIZEOF(ecs_query_op_t);
    }

    if (impl->vars) {
        result->bytes_misc += 
            impl->var_size * ECS_SIZEOF(ecs_query_var_t);
    }

    if (impl->src_vars) {
        result->bytes_misc += 
            query->field_count * ECS_SIZEOF(ecs_var_id_t);
    }

    if (impl->tokens) {
        result->bytes_misc += impl->tokens_len;
    }

    if (impl->monitor) {
        result->bytes_misc += query->field_count * ECS_SIZEOF(int32_t);
    }
    
    /* Query cache memory */
    if (impl->cache) {
        ecs_query_cache_t *cache = impl->cache;

        result->cached_count++;

        result->bytes_cache += ECS_SIZEOF(ecs_query_cache_t);
        result->bytes_cache += 
            flecs_map_memory_get(&cache->tables, 
                ECS_SIZEOF(ecs_query_cache_table_t));
        result->bytes_group_by += 
            flecs_map_memory_get(&cache->groups, 
                ECS_SIZEOF(ecs_query_cache_group_t*));
        
        ecs_size_t cache_elem_size = flecs_query_cache_elem_size(cache);
        ecs_query_cache_group_t *cur = cache->first_group;
        do {
            result->bytes_group_by += 
                ECS_SIZEOF(ecs_query_cache_group_t);

            int32_t group_table_count = ecs_vec_count(&cur->tables);
            result->bytes_cache += group_table_count * cache_elem_size;
            
            if (!(cache->query->flags & EcsQueryTrivialCache)) {
                for (int32_t j = 0; j < group_table_count; j++) {
                    ecs_query_cache_match_t *match = 
                        ecs_vec_get(&cur->tables, cache_elem_size, j);
                    ecs_size_t elem_arrays = 0;
                    elem_arrays += query->field_count * ECS_SIZEOF(ecs_id_t);
                    elem_arrays += query->field_count * ECS_SIZEOF(ecs_entity_t);
                    elem_arrays += query->field_count * ECS_SIZEOF(ecs_table_t*);
                    elem_arrays += query->field_count * ECS_SIZEOF(int32_t);
                    result->bytes_cache += elem_arrays;

                    if (match->wildcard_matches) {
                        int32_t wildcard_count = 
                            ecs_vec_size(match->wildcard_matches);
                        result->bytes_cache += 
                            (cache_elem_size + elem_arrays) * wildcard_count;
                    }
                }
            }
            
            cur = cur->next;
        } while (cur && cur != cache->first_group);
        
        result->bytes_order_by += 
            ecs_vec_size(&cache->table_slices) * 
                ECS_SIZEOF(ecs_table_range_t);

        if (cache->sources) {
            result->bytes_cache += 
                query->field_count * ECS_SIZEOF(ecs_entity_t);
        }

        if (cache->field_map) {
            result->bytes_cache += query->field_count * ECS_SIZEOF(int8_t);
        }

        flecs_query_memory_get(cache->query, result);
    }
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
            flecs_query_memory_get(query, &result);
        }
    }

error:
    return result;
}

static
void flecs_component_memory_get_sparse(
    const ecs_component_record_t *cr,
    ecs_component_memory_t *result)
{
    ecs_sparse_t *sparse = cr->sparse;
    if (!sparse) {
        return;
    }

    ecs_size_t component_size = cr->type_info ? cr->type_info->size : 0;
    flecs_sparse_memory_get(sparse, component_size, 
        &result->bytes_sparse_components, 
        &result->bytes_sparse_components_unused, 
        &result->bytes_sparse_overhead);

    if (component_size) {
        result->instances += flecs_sparse_count(sparse);
    }
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

        int32_t count = ecs_table_count(table);
        int32_t size = ecs_table_size(table);
        ecs_column_t *columns = table->data.columns;

        int32_t col;
        for (col = 0; col < table->column_count; col++) {
            ecs_column_t *column = &columns[col];
            
            ecs_size_t component_size = column->ti->size;
            ecs_size_t used = count * component_size;
            ecs_size_t allocated = size * component_size;
            
            result.bytes_table_components += used;
            result.bytes_table_components_unused += allocated - used;

            if (table->flags & EcsTableHasBuiltins) {
                result.bytes_builtin += used;
            }
        }

        result.instances += count * table->column_count;

        if (table->_) {
            result.bytes_toggle_bitsets += 
                table->_->bs_count * ECS_SIZEOF(ecs_bitset_t);
            result.bytes_toggle_bitsets += 
                (count / 64) * ECS_SIZEOF(uint64_t) * table->_->bs_count;
        }
    }

    /* Sparse components */
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i++) {
        ecs_component_record_t *cr = world->id_index_lo[i];
        if (cr) {
            flecs_component_memory_get_sparse(cr, &result);
        }
    }
    
    ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
    while (ecs_map_next(&it)) {
        ecs_component_record_t *cr = ecs_map_ptr(&it);
        flecs_component_memory_get_sparse(cr, &result);
    }
    
error:
    return result;
}

static
void flecs_table_graph_edge_memory_get(
    ecs_graph_edge_t *edge,
    ecs_table_memory_t *result)
{
    result->bytes_edges += ECS_SIZEOF(ecs_graph_edge_t);
    if (edge->diff) {
        result->bytes_edges += ECS_SIZEOF(ecs_table_diff_t);
        result->bytes_edges += 
            edge->diff->added.count * ECS_SIZEOF(ecs_id_t);
        result->bytes_edges += 
            edge->diff->removed.count * ECS_SIZEOF(ecs_id_t);
    }
}

static
void flecs_table_graph_edges_memory_get(
    ecs_graph_edges_t *edges,
    ecs_table_memory_t *result)
{
    if (edges->lo) {
        int32_t i;
        for (i = 0; i < FLECS_HI_COMPONENT_ID; i++) {
            ecs_graph_edge_t *edge = &edges->lo[i];
            flecs_table_graph_edge_memory_get(edge, result);
        }
    }
    if (edges->hi) {
        result->bytes_edges += ECS_SIZEOF(ecs_map_t);
        result->bytes_edges += flecs_map_memory_get(edges->hi, 0);

        ecs_map_iter_t it = ecs_map_iter(edges->hi);
        while (ecs_map_next(&it)) {
            ecs_graph_edge_t *edge = ecs_map_ptr(&it);
            flecs_table_graph_edge_memory_get(edge, result);
        }
    }
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
        
        int32_t entity_count = ecs_table_count(table);
        int32_t entity_size = ecs_table_size(table);
        int32_t type_count = table->type.count;
        int32_t column_count = table->column_count;
        
        result.column_count += column_count;

        /* Populate entity count histogram using power-of-2 buckets */
        if (entity_count == 0) {
            result.empty_count++;
        }
        
        result.bytes_type += type_count * ECS_SIZEOF(ecs_id_t);

        result.bytes_entities += 
            entity_size * ECS_SIZEOF(ecs_entity_t);
        
        if (table->data.overrides) {
            result.bytes_overrides += ECS_SIZEOF(ecs_table_overrides_t);
            result.bytes_overrides += column_count * ECS_SIZEOF(ecs_ref_t);
        }
        
        result.bytes_columns += column_count * ECS_SIZEOF(ecs_column_t);

        if (table->_) {
            result.bytes_table += ECS_SIZEOF(ecs_table__t);
            result.bytes_table_records += 
                table->_->record_count * ECS_SIZEOF(ecs_table_record_t);
        }

        result.bytes_column_map += 
            (type_count + column_count) * ECS_SIZEOF(int16_t);
        if (table->component_map) {
            result.bytes_component_map += 
                FLECS_HI_COMPONENT_ID * ECS_SIZEOF(int16_t);
        }

        if (table->dirty_state) {
            result.bytes_dirty_state += 
                (column_count + 1) * ECS_SIZEOF(int32_t);
        }
        
        ecs_graph_edges_t *add_edges = &table->node.add;
        ecs_graph_edges_t *remove_edges = &table->node.remove;
        
        flecs_table_graph_edges_memory_get(add_edges, &result);
        flecs_table_graph_edges_memory_get(remove_edges, &result);
    }

error:
    return result;
}

ecs_table_histogram_t ecs_table_histogram_get(
    const ecs_world_t *world)
{
    ecs_table_histogram_t result = {0};

    const ecs_sparse_t *tables = &world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);
    for (i = 0; i < count; i++) {
        ecs_table_t *table = flecs_sparse_get_dense_t(tables, ecs_table_t, i);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
        
        int32_t entity_count = ecs_table_count(table);

        if (entity_count == 0) {
            result.entity_counts[0]++;  /* Bucket 0: 0 entities */
        } else if (entity_count == 1) {
            result.entity_counts[1]++;  /* Bucket 1: 1 entity */
        } else if (entity_count <= ECS_TABLE_MEMORY_HISTOGRAM_MAX_COUNT) {
            /* Find the appropriate power-of-2 bucket */
            /* Buckets: 2-3, 4-7, 8-15, 16-31, ..., 32768-65535 */
            int32_t bucket_index = 2;
            int32_t bucket_min = 2;
            int32_t bucket_max = 3;

            while ((bucket_max < entity_count) && 
                (bucket_index < (ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT - 2))) 
            {
                bucket_index++;
                bucket_min = bucket_max + 1;
                bucket_max = bucket_min * 2 - 1;
            }
            
            result.entity_counts[bucket_index]++;
        } else {
            result.entity_counts[ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT - 1]++;
        }
    }

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
        for (int32_t j = 0; j < 2; j++) {
            ecs_commands_t *cmd = &stage->cmd_stack[j];
            
            /* Calculate queue memory (ecs_vec_t) */
            result.bytes_queue += 
                ecs_vec_size(&cmd->queue) * ECS_SIZEOF(ecs_cmd_t);
            
            /* Calculate entries memory (ecs_sparse_t) */
            ecs_sparse_t *entries = &cmd->entries;
            flecs_sparse_memory_get(entries, 0, 
                &result.bytes_entries, 
                &result.bytes_entries, 
                &result.bytes_entries);
            
            /* Calculate stack memory (ecs_stack_t) */
            ecs_stack_t *stack = &cmd->stack;
            ecs_stack_page_t *page = stack->first;
            while (page) {
                result.bytes_stack += 
                    FLECS_STACK_PAGE_OFFSET + FLECS_STACK_PAGE_SIZE;
                page = page->next;
            }
        }
    }

error:
    return result;
}

ecs_allocator_memory_t ecs_allocator_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_allocator_memory_t result = {0};

    result.bytes_graph_edge = flecs_ballocator_memory_get(
        &world->allocators.graph_edge_lo);
    result.bytes_graph_edge += flecs_ballocator_memory_get(
        &world->allocators.graph_edge);
    result.bytes_component_record = flecs_ballocator_memory_get(
        &world->allocators.component_record);
    result.bytes_pair_record = flecs_ballocator_memory_get(
        &world->allocators.pair_record);
    result.bytes_table_diff = flecs_ballocator_memory_get(
        &world->allocators.table_diff);
    result.bytes_sparse_chunk = flecs_ballocator_memory_get(
        &world->allocators.sparse_chunk);
    result.bytes_hashmap = flecs_ballocator_memory_get(
        &world->allocators.hashmap);

    result.bytes_allocator = flecs_allocator_memory_get(&world->allocator);

    int32_t i, stage_count = world->stage_count;
    ecs_stage_t **stages = world->stages;
    for (i = 0; i < stage_count; i++) {
        ecs_stage_t *stage = stages[i];
        result.bytes_cmd_entry_chunk += flecs_ballocator_memory_get(
            &stage->allocators.cmd_entry_chunk);
        result.bytes_query_impl += flecs_ballocator_memory_get(
            &stage->allocators.query_impl);
        result.bytes_query_cache += flecs_ballocator_memory_get(
            &stage->allocators.query_cache);
    }

error:
    return result;
}

#ifdef FLECS_META
static
int flecs_world_memory_serialize(
    const ecs_serializer_t *s, 
    const void *ptr)
{
    (void)ptr; /* Unused, we compute values from world */
    const ecs_world_t *world = s->world;

    flecs_poly_assert(world, ecs_world_t);

    EcsWorldMemory value;

    ecs_time_t t = {0};
    ecs_time_measure(&t);
    
    value.entities = ecs_entity_index_memory_get(world);
    value.components = ecs_component_memory_get(world);
    value.component_index = ecs_component_index_memory_get(world);
    value.query = ecs_query_memory_get(world);
    value.table = ecs_table_memory_get(world);
    value.table_histogram = ecs_table_histogram_get(world);
    value.commands = ecs_commands_memory_get(world);
    value.allocators = ecs_allocator_memory_get(world);
    
    /* Use component IDs directly */
    s->member(s, "entities");
    s->value(s, ecs_id(ecs_entities_memory_t), &value.entities);
    s->member(s, "components");
    s->value(s, ecs_id(ecs_component_memory_t), &value.components);
    s->member(s, "component_index");
    s->value(s, ecs_id(ecs_component_index_memory_t), &value.component_index);
    s->member(s, "query");
    s->value(s, ecs_id(ecs_query_memory_t), &value.query);
    s->member(s, "table");
    s->value(s, ecs_id(ecs_table_memory_t), &value.table);
    s->member(s, "table_histogram");
    s->value(s, ecs_id(ecs_table_histogram_t), &value.table_histogram);
    s->member(s, "commands");
    s->value(s, ecs_id(ecs_commands_memory_t), &value.commands);
    s->member(s, "allocators");
    s->value(s, ecs_id(ecs_allocator_memory_t), &value.allocators);

    value.collection_time = ecs_time_measure(&t);
    
    return 0;
}

#endif

void flecs_stats_memory_register_reflection(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsWorldMemory);

#ifdef FLECS_META
    ecs_entity_t unit = 0;
    #ifdef FLECS_UNITS
    unit = EcsBytes;
    #endif

    /* Register struct reflection for memory types */
    ecs_id(ecs_entities_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "entity_index_memory_t" }),
        .members = {
            { .name = "alive_count", .type = ecs_id(ecs_i32_t) },
            { .name = "not_alive_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_alive", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_not_alive", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_unused", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_names", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_doc_names", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_component_index_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "component_index_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_component_record", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_cache", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_name_index", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_ordered_children", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_reachable_cache", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_type_info", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_query_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "query_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "cached_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_query", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_cache", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_group_by", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_order_by", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_plan", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_terms", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_misc", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_component_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "component_memory_t" }),
        .members = {
            { .name = "instances", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_table_components", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_components_unused", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_bitset", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_sparse_components", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_sparse_components_unused", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_sparse_overhead", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_builtin", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_table_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "table_memory_t" }),
        .members = {
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "empty_count", .type = ecs_id(ecs_i32_t) },
            { .name = "column_count", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_table", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_type", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_entities", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_overrides", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_columns", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_records", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_column_map", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_component_map", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_dirty_state", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_edges", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_table_histogram_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "table_histogram_t" }),
        .members = {
            { .name = "entity_counts", .type = ecs_id(ecs_i32_t), .count = ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT }
        }
    });

    ecs_id(ecs_commands_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "commands_memory_t" }),
        .members = {
            { .name = "bytes_queue", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_entries", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_stack", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_allocator_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "allocator_memory_t" }),
        .members = {
            { .name = "bytes_graph_edge", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_component_record", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_pair_record", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_diff", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_sparse_chunk", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_hashmap", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_allocator", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_cmd_entry_chunk", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_query_impl", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_query_cache", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    /* Register EcsWorldMemory as opaque type that computes on demand */
    ecs_entity_t world_memory_struct = ecs_struct(world, {
        .members = {
            { .name = "entities", .type = ecs_id(ecs_entities_memory_t) },
            { .name = "components", .type = ecs_id(ecs_component_memory_t) },
            { .name = "component_index", .type = ecs_id(ecs_component_index_memory_t) },
            { .name = "query", .type = ecs_id(ecs_query_memory_t) },
            { .name = "table", .type = ecs_id(ecs_table_memory_t) },
            { .name = "table_histogram", .type = ecs_id(ecs_table_histogram_t) },
            { .name = "commands", .type = ecs_id(ecs_commands_memory_t) },
            { .name = "allocators", .type = ecs_id(ecs_allocator_memory_t) },
            { .name = "collection_time", .type = ecs_id(ecs_f32_t) }
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
