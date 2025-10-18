/**
 * @file addons/stats/memory.c
 */

#include "flecs.h"
#include "stats.h"
#include "../pipeline/pipeline.h"

#ifdef FLECS_REST
#include "../http/http.h"
#endif

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(ecs_entities_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_index_memory_t);
ECS_COMPONENT_DECLARE(ecs_query_memory_t);
ECS_COMPONENT_DECLARE(ecs_component_memory_t);
ECS_COMPONENT_DECLARE(ecs_table_memory_t);
ECS_COMPONENT_DECLARE(ecs_misc_memory_t);
ECS_COMPONENT_DECLARE(ecs_table_histogram_t);
ECS_COMPONENT_DECLARE(ecs_allocator_memory_t);
ECS_COMPONENT_DECLARE(EcsWorldMemory);

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
ecs_size_t flecs_hashmap_memory_get(
    const ecs_hashmap_t *name_index)
{
    const ecs_map_t *map = &name_index->impl;

    ecs_size_t key_size = name_index->key_size;
    ecs_size_t value_size = name_index->value_size;
    ecs_size_t result = flecs_map_memory_get(map, ECS_SIZEOF(ecs_hm_bucket_t));

    ecs_map_iter_t it = ecs_map_iter(map);
    while (ecs_map_next(&it)) {
        ecs_hm_bucket_t *bucket = ecs_map_ptr(&it);
        result += ecs_vec_size(&bucket->keys) * key_size;
        result += ecs_vec_size(&bucket->values) * value_size;
    }

    return result;
}

static
ecs_size_t flecs_sparse_memory_get(
    const ecs_sparse_t *sparse,
    ecs_size_t element_size)
{
    if (!sparse) {
        return 0;
    }

    ecs_size_t result = 0;
    
    result += ecs_vec_size(&sparse->dense) * ECS_SIZEOF(uint64_t);
    result += ecs_vec_size(&sparse->pages) * ECS_SIZEOF(ecs_sparse_page_t);

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

    result += page_count * ECS_SIZEOF(int32_t) * FLECS_SPARSE_PAGE_SIZE;
    result += page_count * element_size * FLECS_SPARSE_PAGE_SIZE;

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
    
    result += flecs_sparse_memory_get(&allocator->sizes, 
        ECS_SIZEOF(ecs_block_allocator_t));
#endif

    return result;
}

static
ecs_size_t flecs_stack_memory_get(
    const ecs_stack_t *stack)
{
    ecs_size_t result = 0;
    ecs_stack_page_t *page = stack->first;
    while (page) {
        result += 
            FLECS_STACK_PAGE_OFFSET + FLECS_STACK_PAGE_SIZE;
        page = page->next;
    }
    return result;
}

static
ecs_size_t flecs_identifier_memory_get(
    const ecs_world_t *world,
    ecs_entity_t tag)
{
    ecs_size_t result = 0;
    ecs_iter_t it = ecs_each_id(world, ecs_pair_t(EcsIdentifier, tag));
    while (ecs_each_next(&it)) {
        EcsIdentifier *ptr = ecs_field(&it, EcsIdentifier, 0);
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            if (ptr[i].value) {
                result += ecs_os_strlen(ptr[i].value) + 1;
            }
        }
    }
    return result;
}

#ifdef FLECS_DOC
static
ecs_size_t flecs_doc_string_memory_get(
    const ecs_world_t *world,
    ecs_entity_t tag)
{
    ecs_size_t result = 0;
    ecs_iter_t it = ecs_each_id(world, ecs_pair_t(EcsDocDescription, tag));
    while (ecs_each_next(&it)) {
        EcsDocDescription *ptr = ecs_field(&it, EcsDocDescription, 0);
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            if (ptr[i].value) {
                result += ecs_os_strlen(ptr[i].value) + 1;
            }
        }
    }
    return result;
}
#endif

ecs_entities_memory_t ecs_entity_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_entity_index_t *index = &world->store.entity_index;
    ecs_entities_memory_t result = {0};

    result.alive_count = index->alive_count - 1;
    result.not_alive_count = ecs_vec_count(&index->dense) - index->alive_count;

    result.bytes_entity_index = 
        ecs_vec_size(&index->dense) * ECS_SIZEOF(uint64_t);
    result.bytes_entity_index += ecs_vec_size(&index->pages) * 
        ECS_SIZEOF(ecs_entity_index_page_t*);

    ecs_entity_index_page_t **pages = ecs_vec_first(&index->pages);
    int32_t i, page_count = 0, pages_count = ecs_vec_count(&index->pages);
    for (i = 0; i < pages_count; i++) {
        if (pages[i]) {
            page_count ++;
        }
    }

    result.bytes_entity_index += page_count * 
        ECS_SIZEOF(ecs_entity_index_page_t);

    /* Entity names, symbols, aliases */
    result.bytes_names += flecs_identifier_memory_get(world, EcsName);
    result.bytes_names += flecs_identifier_memory_get(world, EcsSymbol);
    result.bytes_names += flecs_identifier_memory_get(world, EcsAlias);

#ifdef FLECS_DOC
    /* Doc strings */
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsName);
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsDocBrief);
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsDocDetail);
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsDocUuid);
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsDocLink);
    result.bytes_doc_strings += flecs_doc_string_memory_get(world, EcsDocColor);
#endif

error:
    return result;
}

void ecs_component_record_memory_get(
    const ecs_component_record_t *cr,
    ecs_component_index_memory_t *result)
{
    result->count++;

    result->bytes_component_record += ECS_SIZEOF(ecs_component_record_t);
        
    const ecs_table_cache_t *cache = &cr->cache;
    result->bytes_table_cache += ECS_SIZEOF(ecs_table_cache_t);
    
    const ecs_map_t *map = &cache->index;
    result->bytes_table_cache += flecs_map_memory_get(map, 0);

#ifdef FLECS_DEBUG_INFO
    if (cr->str) {
        result->bytes_component_record += ecs_os_strlen(cr->str);
    }
#endif

    if (cr->pair) {
        ecs_pair_record_t *pair = cr->pair;

        result->bytes_component_record += ECS_SIZEOF(ecs_pair_record_t);
        
        if (pair->name_index) {
            result->bytes_name_index += ECS_SIZEOF(ecs_hashmap_t);
            result->bytes_name_index += flecs_hashmap_memory_get(
                pair->name_index);

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
            ecs_component_record_memory_get(cr, &result);
        }
    }
    
    ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
    while (ecs_map_next(&it)) {
        ecs_component_record_t *cr = ecs_map_ptr(&it);
        ecs_component_record_memory_get(cr, &result);
    }
    
error:
    return result;
}

void ecs_query_memory_get(
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
                ECS_SIZEOF(ecs_query_cache_group_t));
        
        ecs_size_t cache_elem_size = flecs_query_cache_elem_size(cache);
        ecs_query_cache_group_t *cur = cache->first_group;
        do {
            result->bytes_cache += ecs_vec_size(&cur->tables) * cache_elem_size;

            if (!(cache->query->flags & EcsQueryTrivialCache)) {
                int32_t group_table_count = ecs_vec_count(&cur->tables);
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

        result->bytes_cache += flecs_ballocator_memory_get(&cache->allocators.ids);
        result->bytes_cache += flecs_ballocator_memory_get(&cache->allocators.monitors);
        result->bytes_cache += flecs_ballocator_memory_get(&cache->allocators.pointers);

        ecs_query_memory_get(cache->query, result);
        result->count --; /* Don't double count query */
    }
}

static
ecs_size_t flecs_query_total_memory_get(
    const ecs_query_t *query)
{
    ecs_query_memory_t memory = {0};
    ecs_query_memory_get(query, &memory);
    
    ecs_size_t result = 0;
    result += memory.bytes_query;
    result += memory.bytes_cache;
    result += memory.bytes_group_by;
    result += memory.bytes_order_by;
    result += memory.bytes_plan;
    result += memory.bytes_terms;
    result += memory.bytes_misc;
    return result;
}

ecs_query_memory_t ecs_queries_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_query_memory_t result = (ecs_query_memory_t){0};

    {
        ecs_iter_t it = ecs_each_pair(world, ecs_id(EcsPoly), EcsQuery);
        while (ecs_each_next(&it)) {
            EcsPoly *queries = ecs_field(&it, EcsPoly, 0);

            for (int32_t i = 0; i < it.count; i++) {
                ecs_query_t *query = queries[i].poly;
                if (!query) {
                    continue;
                }

                flecs_poly_assert(query, ecs_query_t);
                ecs_query_memory_get(query, &result);
            }
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
    result->bytes_sparse_components += flecs_sparse_memory_get(
        sparse, component_size);

    if (component_size) {
        result->instances += flecs_sparse_count(sparse);
    }
}

void ecs_table_component_memory_get(
    const ecs_table_t *table,
    ecs_component_memory_t *result)
{
    int32_t count = ecs_table_count(table);
    int32_t size = ecs_table_size(table);
    ecs_column_t *columns = table->data.columns;

    int32_t col;
    for (col = 0; col < table->column_count; col++) {
        ecs_column_t *column = &columns[col];
        
        ecs_size_t component_size = column->ti->size;
        ecs_size_t used = count * component_size;
        ecs_size_t allocated = size * component_size;
        
        result->bytes_table_components += used;
        result->bytes_table_components_unused += allocated - used;
    }

    result->instances += count * table->column_count;

    if (table->_) {
        result->bytes_toggle_bitsets += 
            table->_->bs_count * ECS_SIZEOF(ecs_bitset_t);
        result->bytes_toggle_bitsets += 
            (count / 64) * ECS_SIZEOF(uint64_t) * table->_->bs_count;
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
        ecs_table_component_memory_get(table, &result);
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
    const ecs_graph_edge_t *edge,
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
    const ecs_graph_edges_t *edges,
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

void ecs_table_memory_get(
    const ecs_table_t *table,
    ecs_table_memory_t *result)
{
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);

    result->bytes_table += ECS_SIZEOF(ecs_table_t);
    
    int32_t entity_count = ecs_table_count(table);
    int32_t entity_size = ecs_table_size(table);
    int32_t type_count = table->type.count;
    int32_t column_count = table->column_count;
    
    result->column_count += column_count;

    if (entity_count == 0) {
        result->empty_count++;
    }
    
    result->bytes_type += type_count * ECS_SIZEOF(ecs_id_t);

    result->bytes_entities += 
        entity_size * ECS_SIZEOF(ecs_entity_t);
    
    if (table->data.overrides) {
        result->bytes_overrides += ECS_SIZEOF(ecs_table_overrides_t);
        result->bytes_overrides += column_count * ECS_SIZEOF(ecs_ref_t);
    }
    
    result->bytes_columns += column_count * ECS_SIZEOF(ecs_column_t);

    if (table->_) {
        result->bytes_table += ECS_SIZEOF(ecs_table__t);
        result->bytes_table_records += 
            table->_->record_count * ECS_SIZEOF(ecs_table_record_t);
    }

    result->bytes_column_map += 
        (type_count + column_count) * ECS_SIZEOF(int16_t);
    if (table->component_map) {
        result->bytes_component_map += 
            FLECS_HI_COMPONENT_ID * ECS_SIZEOF(int16_t);
    }

    if (table->dirty_state) {
        result->bytes_dirty_state += 
            (column_count + 1) * ECS_SIZEOF(int32_t);
    }
    
    flecs_table_graph_edges_memory_get(&table->node.add, result);
    flecs_table_graph_edges_memory_get(&table->node.remove, result);
}

ecs_table_memory_t ecs_tables_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_memory_t result = {0};
    const ecs_sparse_t *tables = &world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);

    result.count = count;

    result.bytes_table += 
        flecs_sparse_memory_get(tables, ECS_SIZEOF(ecs_table_t));

    for (i = 0; i < count; i++) {
        ecs_table_t *table = flecs_sparse_get_dense_t(tables, ecs_table_t, i);
        ecs_table_memory_get(table, &result);
        /* Already accounted for by flecs_sparse_memory_get */
        result.bytes_table -= ECS_SIZEOF(ecs_table_t);
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

static
ecs_size_t flecs_event_record_memory_get(
    const ecs_event_record_t *er)
{
    ecs_size_t result = flecs_map_memory_get(&er->event_ids, 
        ECS_SIZEOF(ecs_event_id_record_t));

    ecs_map_iter_t mit = ecs_map_iter(&er->event_ids);
    while (ecs_map_next(&mit)) {
        ecs_event_id_record_t *eir = ecs_map_ptr(&mit);
        result += flecs_map_memory_get(&eir->self, 0);
        result += flecs_map_memory_get(&eir->self_up, 0);
        result += flecs_map_memory_get(&eir->up, 0);
    }

    return result;
}

static
ecs_size_t flecs_observer_memory_get(
    const ecs_observer_impl_t *o)
{
    ecs_size_t result = ECS_SIZEOF(ecs_observer_impl_t);
    result += ecs_vec_size(&o->children) * ECS_SIZEOF(void*);

    if (o->pub.query) {
        result += flecs_query_total_memory_get(o->pub.query);
    }

    if (o->not_query) {
        result += flecs_query_total_memory_get(o->not_query);
    }

    ecs_observer_impl_t **children = ecs_vec_first(&o->children);
    int32_t i, count = ecs_vec_count(&o->children);
    for (i = 0; i < count; i ++) {
        result += flecs_observer_memory_get(children[i]);
    }

    return result;
}

static
ecs_size_t flecs_observers_memory_get(
    const ecs_world_t *world)
{
    ecs_size_t result = 0;

    ecs_iter_t it = ecs_each_pair_t(world, EcsPoly, EcsObserver);
    while (ecs_each_next(&it)) {
        EcsPoly *p = ecs_field(&it, EcsPoly, 0);
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            ecs_observer_impl_t *o = p[i].poly;
            if (!o) {
                continue;
            }

            result += flecs_observer_memory_get(o);
        }
    }

    return result;
}

static
ecs_size_t flecs_observer_index_memory_get(
    const ecs_world_t *world)
{
    const ecs_observable_t *o = &world->observable;
    ecs_size_t result = 0;
    result += flecs_event_record_memory_get(&o->on_add);
    result += flecs_event_record_memory_get(&o->on_remove);
    result += flecs_event_record_memory_get(&o->on_set);
    result += flecs_event_record_memory_get(&o->on_wildcard);

    const ecs_sparse_t *events = &o->events;
    result += flecs_sparse_memory_get(events,
        ECS_SIZEOF(ecs_event_record_t));

    int32_t i, count = flecs_sparse_count(events);
    for (i = 0; i < count; i ++) {
        ecs_event_record_t *er = flecs_sparse_get_dense_t(
            events, ecs_event_record_t, i);
        result += flecs_event_record_memory_get(er);
    }

    return result;
}

static
void flecs_system_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    ecs_iter_t it = ecs_each_pair_t(world, EcsPoly, EcsSystem);
    while (ecs_each_next(&it)) {
        EcsPoly *p = ecs_field(&it, EcsPoly, 0);
        result->bytes_systems += it.count * ECS_SIZEOF(ecs_system_t);
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            ecs_system_t *s = p[i].poly;
            if (!s) {
                continue;
            }
            
            if (s->name) {
                result->bytes_systems += ecs_os_strlen(s->name);
            }
        }
    }
}

static
void flecs_pipeline_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    ecs_iter_t it = ecs_each(world, EcsPipeline);
    while (ecs_each_next(&it)) {
        EcsPipeline *p = ecs_field(&it, EcsPipeline, 0);
        result->bytes_pipelines += it.count * ECS_SIZEOF(ecs_pipeline_state_t);
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            ecs_pipeline_state_t *ps = p[i].state;
            if (!ps) {
                continue;
            }
            
            result->bytes_pipelines += ecs_vec_size(&ps->ops) *
                ECS_SIZEOF(ecs_pipeline_op_t);
            result->bytes_pipelines += ecs_vec_size(&ps->systems) *
                ECS_SIZEOF(ecs_entity_t);
            result->bytes_pipelines += ps->iter_count *
                ECS_SIZEOF(ecs_iter_t);
        }
    }
}

static
void flecs_rematch_monitor_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    ecs_map_iter_t it = ecs_map_iter(&world->monitors.monitors);
    while (ecs_map_next(&it)) {
        ecs_monitor_t *m = ecs_map_ptr(&it);
        result->bytes_rematch_monitor += ECS_SIZEOF(ecs_monitor_t);
        result->bytes_rematch_monitor += ecs_vec_size(&m->queries) *
            ECS_SIZEOF(void*);
    }
}

#ifdef FLECS_META
static
void flecs_reflection_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    {
        ecs_iter_t it = ecs_each(world, EcsStruct);
        while (ecs_each_next(&it)) {
            EcsStruct *s = ecs_field(&it, EcsStruct, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result->bytes_reflection += ecs_vec_size(&s[i].members) *
                    ECS_SIZEOF(ecs_member_t);

                int32_t m, mcount = ecs_vec_count(&s[i].members);
                ecs_member_t *members = ecs_vec_first(&s[i].members);
                for (m = 0; m < mcount; m ++) {
                    ecs_member_t *member = &members[m];
                    result->bytes_reflection += ecs_os_strlen(member->name);
                }
            }
        }
    }

    {
        ecs_iter_t it = ecs_each(world, EcsConstants);
        while (ecs_each_next(&it)) {
            EcsConstants *c = ecs_field(&it, EcsConstants, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result->bytes_reflection += flecs_map_memory_get(
                    c[i].constants, ECS_SIZEOF(ecs_enum_constant_t));
                result->bytes_reflection += 
                    ecs_vec_size(&c[i].ordered_constants) * 
                        ECS_SIZEOF(ecs_enum_constant_t);

                ecs_map_iter_t mit = ecs_map_iter(c[i].constants);
                while (ecs_map_next(&mit)) {
                    ecs_enum_constant_t *constant = ecs_map_ptr(&mit);
                    result->bytes_reflection += ecs_os_strlen(constant->name);   
                }
            }
        }
    }

    {
        ecs_iter_t it = ecs_each(world, EcsTypeSerializer);
        while (ecs_each_next(&it)) {
            EcsTypeSerializer *s = ecs_field(&it, EcsTypeSerializer, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                int32_t o, ocount = ecs_vec_count(&s[i].ops);
                ecs_meta_op_t *ops = ecs_vec_first(&s[i].ops);

                result->bytes_reflection += ecs_vec_size(&s[i].ops) *
                    ECS_SIZEOF(ecs_meta_op_t);

                for (o = 0; o < ocount; o ++) {
                    ecs_meta_op_t *op = &ops[o];
                    if (op->kind == EcsOpPushStruct) {
                        result->bytes_reflection += flecs_hashmap_memory_get(
                            op->is.members);
                    } else if (op->kind == EcsOpEnum || 
                        op->kind == EcsOpBitmask) 
                    {
                        result->bytes_reflection += flecs_map_memory_get(
                            op->is.constants, 0);
                    }
                }
            }
        }
    }
}
#endif

static
void flecs_stats_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    ecs_entity_t periods[] = {
        EcsPeriod1s, EcsPeriod1m, EcsPeriod1h, EcsPeriod1d, EcsPeriod1w};
    int32_t p, period_count = 5;

    result->bytes_stats += ECS_SIZEOF(ecs_world_stats_t) * ecs_count_id(world,
        ecs_pair_t(EcsWorldStats, EcsWildcard));

    for (p = 0; p < period_count; p ++) {
        ecs_iter_t it = ecs_each_id(world, 
            ecs_pair_t(EcsSystemStats, periods[p]));
        while (ecs_each_next(&it)) {
            EcsSystemStats *ss = ecs_field(&it, EcsSystemStats, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result->bytes_stats += flecs_map_memory_get(&ss[i].stats,
                    ECS_SIZEOF(ecs_system_stats_t));
            }
        }
    }
    for (p = 0; p < period_count; p ++) {
        ecs_iter_t it = ecs_each_id(world, 
            ecs_pair_t(EcsPipelineStats, periods[p]));
        while (ecs_each_next(&it)) {
            EcsPipelineStats *ps = ecs_field(&it, EcsPipelineStats, 0);
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                result->bytes_stats += flecs_map_memory_get(&ps[i].stats,
                    ECS_SIZEOF(ecs_pipeline_stats_t));

                ecs_map_iter_t pit = ecs_map_iter(&ps[i].stats);
                while (ecs_map_next(&pit)) {
                    ecs_pipeline_stats_t *stats = ecs_map_ptr(&pit);
                    result->bytes_stats += ecs_vec_size(&stats->systems) * 
                        ECS_SIZEOF(ecs_entity_t);
                    result->bytes_stats += ecs_vec_size(&stats->sync_points) * 
                        ECS_SIZEOF(ecs_sync_stats_t);
                }
            }
        }
    }
}

#ifdef FLECS_REST
static
void flecs_http_memory_get(
    ecs_http_server_t *srv,
    ecs_misc_memory_t *result)
{
    result->bytes_rest += ECS_SIZEOF(ecs_http_server_t);

    result->bytes_rest += flecs_sparse_memory_get(&srv->connections, 
        ECS_SIZEOF(ecs_http_connection_t));

    result->bytes_rest += flecs_sparse_memory_get(&srv->requests, 
        ECS_SIZEOF(ecs_http_request_t));

    result->bytes_rest += flecs_hashmap_memory_get(&srv->request_cache);

    ecs_map_iter_t it = ecs_map_iter(&srv->request_cache.impl);
    while (ecs_map_next(&it)) {
        ecs_hm_bucket_t *bucket = ecs_map_ptr(&it);
        int32_t i, count = ecs_vec_count(&bucket->values);
        ecs_http_request_key_t *keys = ecs_vec_first(&bucket->keys);
        ecs_http_request_entry_t *entries = ecs_vec_first(&bucket->values);
        for (i = count - 1; i >= 0; i --) {
            ecs_http_request_entry_t *entry = &entries[i];
            ecs_http_request_key_t *key = &keys[i];

            result->bytes_rest += key->count;
            if (entry->content) {
                result->bytes_rest += ecs_os_strlen(entry->content);
            }
        }
    }
}
#endif

#ifdef FLECS_REST
static
void flecs_rest_memory_get(
    const ecs_world_t *world,
    ecs_misc_memory_t *result)
{
    ecs_iter_t it = ecs_each(world, EcsRest);
    while (ecs_each_next(&it)) {
        EcsRest *r = ecs_field(&it, EcsRest, 0);
        
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            result->bytes_rest += ECS_SIZEOF(ecs_rest_ctx_t);
            if (!r[i].impl) {
                continue;
            }
            flecs_http_memory_get(r[i].impl->srv, result);
        }
    }
}
#endif

ecs_misc_memory_t ecs_misc_memory_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_misc_memory_t result = {0};
    
    int32_t stage_count = world->stage_count;
    ecs_stage_t **stages = world->stages;

    result.bytes_world = ECS_SIZEOF(ecs_world_t) + 
        ECS_SIZEOF(ecs_stage_t) * stage_count +
        ECS_SIZEOF(ecs_stage_t*) * stage_count;
    
    result.bytes_observers = flecs_observers_memory_get(world);
    result.bytes_observers = flecs_observer_index_memory_get(world);
    flecs_system_memory_get(world, &result);
    flecs_pipeline_memory_get(world, &result);
    flecs_rematch_monitor_memory_get(world, &result);

    #ifdef FLECS_META
        flecs_reflection_memory_get(world, &result);
    #endif
    flecs_stats_memory_get(world, &result);
    #ifdef FLECS_REST
        flecs_rest_memory_get(world, &result);
    #endif

    result.bytes_component_ids += 
        ecs_vec_size(&world->component_ids) * ECS_SIZEOF(ecs_entity_t);
    
    result.bytes_table_lookup += 
        flecs_hashmap_memory_get(&world->store.table_map);

    result.bytes_component_record_lookup += FLECS_HI_ID_RECORD_ID * 
        ECS_SIZEOF(ecs_component_record_t*);

    result.bytes_component_record_lookup += flecs_map_memory_get(
        &world->id_index_hi, 0);

    result.bytes_type_info += flecs_map_memory_get(
        &world->type_info, ECS_SIZEOF(ecs_type_info_t));

    /* Iterate through all stages to collect command memory usage */
    for (int32_t i = 0; i < stage_count; i++) {
        ecs_stage_t *stage = stages[i];
        for (int32_t j = 0; j < 2; j++) {
            ecs_commands_t *cmd = &stage->cmd_stack[j];
            
            /* Calculate queue memory (ecs_vec_t) */
            result.bytes_commands += 
                ecs_vec_size(&cmd->queue) * ECS_SIZEOF(ecs_cmd_t);
            
            /* Calculate entries memory (ecs_sparse_t) */
            ecs_sparse_t *entries = &cmd->entries;
            result.bytes_commands += flecs_sparse_memory_get(entries, 
                ECS_SIZEOF(ecs_cmd_entry_t));

            /* Calculate stack memory (ecs_stack_t) */
            result.bytes_commands += flecs_stack_memory_get(&cmd->stack);
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

    result.bytes_allocator = flecs_allocator_memory_get(&world->allocator);
    result.bytes_misc += ecs_vec_size(&world->allocators.diff_builder.added) *
        ECS_SIZEOF(ecs_id_t);

    result.bytes_misc += ecs_vec_size(&world->allocators.diff_builder.removed) *
        ECS_SIZEOF(ecs_id_t);
    result.bytes_misc += ecs_vec_size(&world->store.records) *
        ECS_SIZEOF(ecs_table_record_t);
    result.bytes_misc += ecs_vec_size(&world->store.marked_ids) *
        ECS_SIZEOF(ecs_marked_id_t);
    result.bytes_misc += ecs_vec_size(&world->store.deleted_components) *
        ECS_SIZEOF(ecs_entity_t);

    int32_t i, stage_count = world->stage_count;
    ecs_stage_t **stages = world->stages;
    for (i = 0; i < stage_count; i++) {
        ecs_stage_t *stage = stages[i];
        result.bytes_allocator += flecs_allocator_memory_get(&stage->allocator);
        result.bytes_cmd_entry_chunk += flecs_ballocator_memory_get(
            &stage->allocators.cmd_entry_chunk);
        result.bytes_query_impl += flecs_ballocator_memory_get(
            &stage->allocators.query_impl);
        result.bytes_query_cache += flecs_ballocator_memory_get(
            &stage->allocators.query_cache);
        result.bytes_misc += ecs_vec_size(&stage->variables) * 
            ECS_SIZEOF(ecs_query_var_t);
        result.bytes_misc += ecs_vec_size(&stage->operations) * 
            ECS_SIZEOF(ecs_query_op_t);
        result.bytes_stack_allocator += 
            flecs_stack_memory_get(&stage->allocators.iter_stack);
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
    ecs_os_zeromem(&value);

    ecs_time_t t = {0};
    ecs_time_measure(&t);
    
    value.entities = ecs_entity_memory_get(world);
    value.components = ecs_component_memory_get(world);
    value.component_index = ecs_component_index_memory_get(world);
    value.queries = ecs_queries_memory_get(world);
    value.tables = ecs_tables_memory_get(world);
    value.table_histogram = ecs_table_histogram_get(world);
    value.misc = ecs_misc_memory_get(world);
    value.allocators = ecs_allocator_memory_get(world);
    
    /* Use component IDs directly */
    s->member(s, "entities");
    s->value(s, ecs_id(ecs_entities_memory_t), &value.entities);
    s->member(s, "components");
    s->value(s, ecs_id(ecs_component_memory_t), &value.components);
    s->member(s, "component_index");
    s->value(s, ecs_id(ecs_component_index_memory_t), &value.component_index);
    s->member(s, "queries");
    s->value(s, ecs_id(ecs_query_memory_t), &value.queries);
    s->member(s, "tables");
    s->value(s, ecs_id(ecs_table_memory_t), &value.tables);
    s->member(s, "table_histogram");
    s->value(s, ecs_id(ecs_table_histogram_t), &value.table_histogram);
    s->member(s, "misc");
    s->value(s, ecs_id(ecs_misc_memory_t), &value.misc);
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
            { .name = "bytes_entity_index", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_names", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_doc_strings", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    ecs_id(ecs_component_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "component_memory_t" }),
        .members = {
            { .name = "instances", .type = ecs_id(ecs_i32_t) },
            { .name = "bytes_table_components", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_components_unused", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_toggle_bitset", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_sparse_components", .type = ecs_id(ecs_i32_t), .unit = unit }
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
            { .name = "bytes_reachable_cache", .type = ecs_id(ecs_i32_t), .unit = unit }
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

    ecs_id(ecs_misc_memory_t) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "misc_memory_t" }),
        .members = {
            { .name = "bytes_world", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_observers", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_systems", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_pipelines", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_table_lookup", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_component_record_lookup", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_type_info", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_commands", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_rematch_monitor", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_component_ids", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_reflection", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_stats", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_rest", .type = ecs_id(ecs_i32_t), .unit = unit },
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
            { .name = "bytes_allocator", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_stack_allocator", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_cmd_entry_chunk", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_query_impl", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_query_cache", .type = ecs_id(ecs_i32_t), .unit = unit },
            { .name = "bytes_misc", .type = ecs_id(ecs_i32_t), .unit = unit }
        }
    });

    /* Register EcsWorldMemory as opaque type that computes on demand */
    ecs_entity_t world_memory_struct = ecs_struct(world, {
        .members = {
            { .name = "entities", .type = ecs_id(ecs_entities_memory_t) },
            { .name = "components", .type = ecs_id(ecs_component_memory_t) },
            { .name = "component_index", .type = ecs_id(ecs_component_index_memory_t) },
            { .name = "queries", .type = ecs_id(ecs_query_memory_t) },
            { .name = "tables", .type = ecs_id(ecs_table_memory_t) },
            { .name = "table_histogram", .type = ecs_id(ecs_table_histogram_t) },
            { .name = "misc", .type = ecs_id(ecs_misc_memory_t) },
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

static
ecs_size_t flecs_get_memory_total(
    const ecs_world_t *world,
    const void *ptr,
    ecs_entity_t type)
{
    (void)world;
    (void)ptr;
    (void)type;

    if (!type) {
        ecs_warn("import flecs.stats module before calling ecs_memory_get");
        return 0;
    }

    ecs_size_t result = 0;
#ifdef FLECS_META
    const EcsTypeSerializer *s = ecs_get(world, type, EcsTypeSerializer);
    ecs_meta_op_t *ops = ecs_vec_first(&s->ops);
    int32_t i, count = ecs_vec_count(&s->ops);

    for (i = 0; i < count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        if (!op->name) {
            continue;
        }

        if (!ecs_os_strncmp(op->name, "bytes_", 6)) {
            result += *(ecs_size_t*)ECS_OFFSET(ptr, op->offset);
        }
    }
#else
    ecs_warn("FLECS_META addon required for ecs_memory_get");
#endif

    return result;
}

ecs_size_t ecs_memory_get(
    const ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    
    ecs_entities_memory_t entities = {0};
    ecs_component_memory_t component = {0};
    ecs_component_index_memory_t component_index = {0};
    ecs_query_memory_t query = {0};
    ecs_table_memory_t table = {0};
    ecs_misc_memory_t misc = {0};
    ecs_allocator_memory_t allocator = {0};

    entities = ecs_entity_memory_get(world);
    component = ecs_component_memory_get(world);
    component_index = ecs_component_index_memory_get(world);
    query = ecs_queries_memory_get(world);
    table = ecs_tables_memory_get(world);
    misc = ecs_misc_memory_get(world);
    allocator = ecs_allocator_memory_get(world);

    ecs_size_t result = 0;
    result += flecs_get_memory_total(world, &entities, 
        ecs_id(ecs_entities_memory_t));
    result += flecs_get_memory_total(world, &component, 
        ecs_id(ecs_component_memory_t));
    result += flecs_get_memory_total(world, &component_index, 
        ecs_id(ecs_component_index_memory_t));
    result += flecs_get_memory_total(world, &query, 
        ecs_id(ecs_query_memory_t));
    result += flecs_get_memory_total(world, &table, 
        ecs_id(ecs_table_memory_t));
    result += flecs_get_memory_total(world, &misc, 
        ecs_id(ecs_misc_memory_t));
    result += flecs_get_memory_total(world, &allocator, 
        ecs_id(ecs_allocator_memory_t));
    return result;
}

#endif
