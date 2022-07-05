#include "private_api.h"
#include <stdio.h>

/* Marker object used to differentiate a component vs. a tag edge */
static ecs_table_diff_t ecs_table_edge_is_component;

/* Id sequence (type) utilities */

static
uint64_t flecs_type_hash(const void *ptr) {
    const ecs_type_t *type = ptr;
    ecs_id_t *ids = type->array;
    int32_t count = type->count;
    return flecs_hash(ids, count * ECS_SIZEOF(ecs_id_t));
}

static
int flecs_type_compare(const void *ptr_1, const void *ptr_2) {
    const ecs_type_t *type_1 = ptr_1;
    const ecs_type_t *type_2 = ptr_2;

    int32_t count_1 = type_1->count;
    int32_t count_2 = type_2->count;

    if (count_1 != count_2) {
        return (count_1 > count_2) - (count_1 < count_2);
    }

    const ecs_id_t *ids_1 = type_1->array;
    const ecs_id_t *ids_2 = type_2->array;
    int result = 0;
    
    int32_t i;
    for (i = 0; !result && (i < count_1); i ++) {
        ecs_id_t id_1 = ids_1[i];
        ecs_id_t id_2 = ids_2[i];
        result = (id_1 > id_2) - (id_1 < id_2);
    }

    return result;
}

void flecs_table_hashmap_init(ecs_hashmap_t *hm) {
    flecs_hashmap_init(hm, ecs_type_t, ecs_table_t*, 
        flecs_type_hash, flecs_type_compare);
}

/* Find location where to insert id into type */
static
int flecs_type_find_insert(
    const ecs_type_t *type,
    int32_t offset,
    ecs_id_t to_add)
{
    ecs_id_t *array = type->array;
    int32_t i, count = type->count;

    for (i = offset; i < count; i ++) {
        ecs_id_t id = array[i];
        if (id == to_add) {
            return -1;
        }
        if (id > to_add) {
            return i;
        }
    }
    return i;
}

/* Find location of id in type */
static
int flecs_type_find(
    const ecs_type_t *type,
    ecs_id_t id)
{
    ecs_id_t *array = type->array;
    int32_t i, count = type->count;

    for (i = 0; i < count; i ++) {
        ecs_id_t cur = array[i];
        if (ecs_id_match(cur, id)) {
            return i;
        }
        if (cur > id) {
            return -1;
        }
    }

    return -1;
}

/* Count number of matching ids */
static
int flecs_type_count_matches(
    const ecs_type_t *type,
    ecs_id_t wildcard,
    int32_t offset)
{
    ecs_id_t *array = type->array;
    int32_t i = offset, count = type->count;

    for (; i < count; i ++) {
        ecs_id_t cur = array[i];
        if (!ecs_id_match(cur, wildcard)) {
            break;
        }
    }

    return i - offset;
}

/* Create type from source type with id */
static
int flecs_type_new_with(
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t with)
{
    ecs_id_t *src_array = src->array;
    int32_t at = flecs_type_find_insert(src, 0, with);
    if (at == -1) {
        return -1;
    }

    int32_t dst_count = src->count + 1;
    ecs_id_t *dst_array = ecs_os_malloc_n(ecs_id_t, dst_count);
    dst->count = dst_count;
    dst->array = dst_array;

    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t remain = src->count - at;
    if (remain) {
        ecs_os_memcpy_n(&dst_array[at + 1], &src_array[at], ecs_id_t, remain);
    }

    dst_array[at] = with;

    return 0;
}

/* Create type from source type without ids matching wildcard */
static
int flecs_type_new_filtered(
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t wildcard,
    int32_t at)
{
    *dst = flecs_type_copy(src);
    ecs_id_t *dst_array = dst->array;
    ecs_id_t *src_array = src->array;
    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t i = at + 1, w = at, count = src->count;
    for (; i < count; i ++) {
        ecs_id_t id = src_array[i];
        if (!ecs_id_match(id, wildcard)) {
            dst_array[w] = id;
            w ++;
        }
    }

    dst->count = w;
    if (w != count) {
        dst->array = ecs_os_realloc_n(dst->array, ecs_id_t, w);
    }

    return 0;
}

/* Create type from source type without id */
static
int flecs_type_new_without(
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t without)
{
    ecs_id_t *src_array = src->array;
    int32_t count = 1, at = flecs_type_find(src, without);
    if (at == -1) {
        return -1;
    }

    int32_t src_count = src->count;
    if (src_count == 1) {
        dst->array = NULL;
        dst->count = 0;
        return 0;
    }

    if (ecs_id_is_wildcard(without)) {
        if (ECS_HAS_ROLE(without, PAIR)) {
            ecs_entity_t r = ECS_PAIR_FIRST(without);
            ecs_entity_t o = ECS_PAIR_SECOND(without);
            if (r == EcsWildcard && o != EcsWildcard) {
                return flecs_type_new_filtered(dst, src, without, at);
            }
        }
        count += flecs_type_count_matches(src, without, at + 1);
    }

    int32_t dst_count = src_count - count;
    dst->count = dst_count;
    if (!dst_count) {
        dst->array = NULL;
        return 0;
    }

    ecs_id_t *dst_array = ecs_os_malloc_n(ecs_id_t, dst_count);
    dst->array = dst_array;

    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t remain = dst_count - at;
    if (remain) {
        ecs_os_memcpy_n(
            &dst_array[at], &src_array[at + count], ecs_id_t, remain);
    }

    return 0;
}

/* Copy type */
ecs_type_t flecs_type_copy(
    const ecs_type_t *src)
{
    int32_t src_count = src->count;
    return (ecs_type_t) {
        .array = ecs_os_memdup_n(src->array, ecs_id_t, src_count),
        .count = src_count
    };
}

/* Free type */
void flecs_type_free(
    ecs_type_t *type)
{
    ecs_os_free(type->array);
}

/* Add to type */
static
void flecs_type_add(
    ecs_type_t *type,
    ecs_id_t add)
{
    ecs_type_t new_type;
    int res = flecs_type_new_with(&new_type, type, add);
    if (res != -1) {
        flecs_type_free(type);
        type->array = new_type.array;
        type->count = new_type.count;
    }
}

/* Graph edge utilities */

static
void table_diff_free(
    ecs_table_diff_t *diff) 
{
    ecs_os_free(diff->added.array);
    ecs_os_free(diff->removed.array);
    ecs_os_free(diff->on_set.array);
    ecs_os_free(diff->un_set.array);
    ecs_os_free(diff);
}

static
ecs_graph_edge_t* graph_edge_new(
    ecs_world_t *world)
{
    ecs_graph_edge_t *result = (ecs_graph_edge_t*)world->store.first_free;
    if (result) {
        world->store.first_free = result->hdr.next;
        ecs_os_zeromem(result);
    } else {
        result = ecs_os_calloc_t(ecs_graph_edge_t);
    }
    return result;
}

static
void graph_edge_free(
    ecs_world_t *world,
    ecs_graph_edge_t *edge)
{
    if (world->flags & EcsWorldFini) {
        ecs_os_free(edge);
    } else {
        edge->hdr.next = world->store.first_free;
        world->store.first_free = &edge->hdr;
    }
}

static
ecs_graph_edge_t* ensure_hi_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    if (!ecs_map_is_initialized(&edges->hi)) {
        ecs_map_init(&edges->hi, ecs_graph_edge_t*, 1);
    }

    ecs_graph_edge_t **ep = ecs_map_ensure(&edges->hi, ecs_graph_edge_t*, id);
    ecs_graph_edge_t *edge = ep[0];
    if (edge) {
        return edge;
    }

    if (id < ECS_HI_COMPONENT_ID) {
        edge = &edges->lo[id];
    } else {
        edge = graph_edge_new(world);
    }

    ep[0] = edge;
    return edge;
}

static
ecs_graph_edge_t* ensure_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    ecs_graph_edge_t *edge;
    
    if (id < ECS_HI_COMPONENT_ID) {
        if (!edges->lo) {
            edges->lo = ecs_os_calloc_n(ecs_graph_edge_t, ECS_HI_COMPONENT_ID);
        }
        edge = &edges->lo[id];
    } else {
        if (!ecs_map_is_initialized(&edges->hi)) {
            ecs_map_init(&edges->hi, ecs_graph_edge_t*, 1);
        }
        edge = ensure_hi_edge(world, edges, id);
    }

    return edge;
}

static
void disconnect_edge(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == id, ECS_INTERNAL_ERROR, NULL);
    (void)id;

    /* Remove backref from destination table */
    ecs_graph_edge_hdr_t *next = edge->hdr.next;
    ecs_graph_edge_hdr_t *prev = edge->hdr.prev;

    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }

    /* Remove data associated with edge */
    ecs_table_diff_t *diff = edge->diff;
    if (diff && diff != &ecs_table_edge_is_component) {
        table_diff_free(diff);
    }

    /* If edge id is low, clear it from fast lookup array */
    if (id < ECS_HI_COMPONENT_ID) {
        ecs_os_memset_t(edge, 0, ecs_graph_edge_t);
    } else {
        graph_edge_free(world, edge);
    }
}

static
void remove_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edges != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_map_is_initialized(&edges->hi), ECS_INTERNAL_ERROR, NULL);
    disconnect_edge(world, id, edge);
    ecs_map_remove(&edges->hi, id);
}

static
void init_edges(
    ecs_graph_edges_t *edges)
{
    edges->lo = NULL;
    ecs_os_zeromem(&edges->hi);
}

static
void init_node(
    ecs_graph_node_t *node)
{
    init_edges(&node->add);
    init_edges(&node->remove);
}

bool flecs_table_records_update_empty(
    ecs_table_t *table)
{
    bool result = false;
    bool is_empty = ecs_table_count(table) == 0;

    int32_t i, count = table->record_count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &table->records[i];
        ecs_table_cache_t *cache = tr->hdr.cache;
        result |= ecs_table_cache_set_empty(cache, table, is_empty);
    }

    return result;
}

static
void init_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *prev)
{
    table->type_info = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->lock = 0;
    table->refcount = 1;
    table->generation = 0;

    init_node(&table->node);

    flecs_table_init(world, table, prev);
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_type_t *type,
    flecs_hashmap_result_t table_elem,
    ecs_table_t *prev)
{
    ecs_table_t *result = flecs_sparse_add(&world->store.tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    result->id = flecs_sparse_last_id(&world->store.tables);
    result->type = *type;

    if (ecs_should_log_2()) {
        char *expr = ecs_type_str(world, &result->type);
        ecs_dbg_2(
            "#[green]table#[normal] [%s] #[green]created#[reset] with id %d", 
            expr, result->id);
        ecs_os_free(expr);
    }

    ecs_log_push_2();

    /* Store table in table hashmap */
    *(ecs_table_t**)table_elem.value = result;

    /* Set keyvalue to one that has the same lifecycle as the table */
    *(ecs_type_t*)table_elem.key = result->type;

    init_table(world, result, prev);

    flecs_notify_queries(world, &(ecs_query_event_t) {
        .kind = EcsQueryTableMatch,
        .table = result
    });

    /* Update counters */
    world->info.table_count ++;
    world->info.table_record_count += result->record_count;
    world->info.table_storage_count += result->storage_count;
    world->info.empty_table_count ++;
    world->info.table_create_total ++;
    
    if (!result->storage_count) {
        world->info.tag_table_count ++;
    } else {
        world->info.trivial_table_count += !(result->flags & EcsTableIsComplex);
    }

    ecs_log_pop_2();

    return result;
}

static
ecs_table_t* find_or_create(
    ecs_world_t *world,
    ecs_type_t *type,
    bool own_type,
    ecs_table_t *prev)
{    
    ecs_poly_assert(world, ecs_world_t);   

    int32_t id_count = type->count;
    if (!id_count) {
        return &world->store.root;
    }

    ecs_table_t *table;
    flecs_hashmap_result_t elem = flecs_hashmap_ensure(
        &world->store.table_map, type, ecs_table_t*);
    if ((table = *(ecs_table_t**)elem.value)) {
        if (own_type) {
            flecs_type_free(type);
        }
        return table;
    }

    /* If we get here, table needs to be created which is only allowed when the
     * application is not currently in progress */
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);

    /* If we get here, the table has not been found, so create it. */
    if (own_type) {
        return create_table(world, type, elem, prev);
    }

    ecs_type_t copy = flecs_type_copy(type);
    return create_table(world, &copy, elem, prev);
}

static
void ids_append(
    ecs_type_t *ids,
    ecs_id_t id)
{
    ids->array = ecs_os_realloc_n(ids->array, ecs_id_t, ids->count + 1);
    ids->array[ids->count ++] = id;   
}

static
void diff_insert_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *base_diff,
    ecs_type_t *append_to,
    ecs_type_t *append_from,
    ecs_id_t add)
{
    ecs_entity_t base = ecs_pair_second(world, add);
    ecs_table_t *base_table = ecs_get_table(world, base);
    if (!base_table) {
        return;
    }

    ecs_type_t base_type = base_table->type;
    ecs_table_t *table_wo_base = base_table;

    /* If the table does not have a component from the base, it should
     * emit an OnSet event */
    ecs_id_t *ids = base_type.array;
    int32_t j, i, count = base_type.count;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* The base has an IsA relation. Find table without the base, which
             * gives us the list of ids the current base inherits and doesn't
             * override. This saves us from having to recursively check for each
             * base in the hierarchy whether the component is overridden. */
            table_wo_base = flecs_table_traverse_remove(
                world, table_wo_base, &id, base_diff);

            /* Because we removed, the ids are stored in un_set vs. on_set */
            for (j = 0; j < append_from->count; j ++) {
                ecs_id_t base_id = append_from->array[j];
                /* We still have to make sure the id isn't overridden by the
                 * current table */
                if (ecs_search(world, table, base_id, NULL) == -1) {
                    ids_append(append_to, base_id);
                }
            }

            continue;
        }

        /* Identifiers are not inherited */
        if (ECS_HAS_RELATION(id, ecs_id(EcsIdentifier))) {
            continue;
        }

        if (!ecs_get_typeid(world, id)) {
            continue;
        }

        if (ecs_search(world, table, id, NULL) == -1) {
            ids_append(append_to, id);
        }
    }
}

static
void diff_insert_added_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    ecs_table_diff_t base_diff;
    diff_insert_isa(world, table, &base_diff, &diff->on_set, 
        &base_diff.un_set, id);
}

static
void diff_insert_removed_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    ecs_table_diff_t base_diff;
    diff_insert_isa(world, table, &base_diff, &diff->un_set, 
        &base_diff.un_set, id);
}

static
void diff_insert_added(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    diff->added.array[diff->added.count ++] = id;

    if (ECS_HAS_RELATION(id, EcsIsA)) {
        diff_insert_added_isa(world, table, diff, id);
    }
}

static
void diff_insert_removed(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    diff->removed.array[diff->removed.count ++] = id;

    if (ECS_HAS_RELATION(id, EcsIsA)) {
        /* Removing an IsA relation also "removes" all components from the
         * instance. Any id from base that's not overridden should be UnSet. */
        diff_insert_removed_isa(world, table, diff, id);
        return;
    }

    if (table->flags & EcsTableHasIsA) {
        if (!ecs_get_typeid(world, id)) {
            /* Do nothing if id is not a component */
            return;
        }

        /* If next table has a base and component is removed, check if
         * the removed component was an override. Removed overrides reexpose the
         * base component, thus "changing" the value which requires an OnSet. */
        if (ecs_search_relation(world, table, 0, id, EcsIsA,
            1, -1, 0, 0, 0, 0) != -1)
        {
            ids_append(&diff->on_set, id);
            return;
        }
    }

    if (ecs_get_typeid(world, id) != 0) {
        ids_append(&diff->un_set, id);
    }
}

static
void compute_table_diff(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_table_t *next,
    ecs_graph_edge_t *edge,
    ecs_id_t id)
{
    if (node == next) {
        return;
    }

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_id_record_t *idr = flecs_id_record_get(world, ecs_pair(
            ECS_PAIR_FIRST(id), EcsWildcard));
        if (idr->flags & EcsIdUnion) {
            id = ecs_pair(EcsUnion, ECS_PAIR_FIRST(id));
        }
    }

    ecs_type_t node_type = node->type;
    ecs_type_t next_type = next->type;

    ecs_id_t *ids_node = node_type.array;
    ecs_id_t *ids_next = next_type.array;
    int32_t i_node = 0, node_count = node_type.count;
    int32_t i_next = 0, next_count = next_type.count;
    int32_t added_count = 0;
    int32_t removed_count = 0;
    bool trivial_edge = !ECS_HAS_RELATION(id, EcsIsA) && 
        !(node->flags & EcsTableHasIsA) && !(next->flags & EcsTableHasIsA);

    /* First do a scan to see how big the diff is, so we don't have to realloc
     * or alloc more memory than required. */
    for (; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        bool added = id_next < id_node;
        bool removed = id_node < id_next;

        trivial_edge &= !added || id_next == id;
        trivial_edge &= !removed || id_node == id;

        added_count += added;
        removed_count += removed;

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    added_count += next_count - i_next;
    removed_count += node_count - i_node;

    trivial_edge &= (added_count + removed_count) <= 1 && 
        !ecs_id_is_wildcard(id);

    if (trivial_edge) {
        /* If edge is trivial there's no need to create a diff element for it.
         * Store whether the id is a tag or not, so that we can still tell 
         * whether an UnSet handler should be called or not. */
        if (node->storage_table != next->storage_table) {
            edge->diff = &ecs_table_edge_is_component;
        }
        return;
    }

    ecs_table_diff_t *diff = ecs_os_calloc_t(ecs_table_diff_t);
    edge->diff = diff;
    if (added_count) {
        diff->added.array = ecs_os_malloc_n(ecs_id_t, added_count);
        diff->added.count = 0;
    }
    if (removed_count) {
        diff->removed.array = ecs_os_malloc_n(ecs_id_t, removed_count);
        diff->removed.count = 0;
    }

    for (i_node = 0, i_next = 0; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        if (id_next < id_node) {
            diff_insert_added(world, node, diff, id_next);
        } else if (id_node < id_next) {
            diff_insert_removed(world, next, diff, id_node);
        }

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    for (; i_next < next_count; i_next ++) {
        diff_insert_added(world, node, diff, ids_next[i_next]);
    }
    for (; i_node < node_count; i_node ++) {
        diff_insert_removed(world, next, diff, ids_node[i_node]);
    }

    ecs_assert(diff->added.count == added_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(diff->removed.count == removed_count, ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_add_overrides_for_base(
    ecs_world_t *world,
    ecs_type_t *dst_type,
    ecs_id_t pair)
{
    ecs_entity_t base = ecs_pair_second(world, pair);
    ecs_assert(base != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *base_table = ecs_get_table(world, base);
    if (!base_table) {
        return;
    }
    
    ecs_id_t *ids = base_table->type.array;

    ecs_flags32_t flags = base_table->flags;
    if (flags & EcsTableHasOverrides) {
        int32_t i, count = base_table->type.count;
        for (i = 0; i < count; i ++) {
            ecs_id_t id = ids[i];
            if (ECS_HAS_ROLE(id, OVERRIDE)) {
                flecs_type_add(dst_type, id & ECS_COMPONENT_MASK);
            }
        }
    }

    if (flags & EcsTableHasIsA) {
        const ecs_table_record_t *tr = flecs_id_record_get_table(
            world->idr_isa_wildcard, base_table);
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t i = tr->column, end = i + tr->count;
        for (; i != end; i ++) {
            flecs_add_overrides_for_base(world, dst_type, ids[i]);
        }
    }
}

static
void flecs_add_with_property(
    ecs_world_t *world,
    ecs_id_record_t *idr_with_wildcard,
    ecs_type_t *dst_type,
    ecs_entity_t r,
    ecs_entity_t o)
{
    r = ecs_get_alive(world, r);

    /* Check if component/relation has With pairs, which contain ids
     * that need to be added to the table. */
    ecs_table_t *table = ecs_get_table(world, r);
    if (!table) {
        return;
    }
    
    const ecs_table_record_t *tr = flecs_id_record_get_table(
        idr_with_wildcard, table);
    if (tr) {
        int32_t i = tr->column, end = i + tr->count;
        ecs_id_t *ids = table->type.array;

        for (; i < end; i ++) {
            ecs_id_t id = ids[i];
            ecs_assert(ECS_PAIR_FIRST(id) == EcsWith, ECS_INTERNAL_ERROR, NULL);
            ecs_id_t ra = ECS_PAIR_SECOND(id);
            ecs_id_t a = ra;
            if (o) {
                a = ecs_pair(ra, o);
            }

            flecs_type_add(dst_type, a);

            flecs_add_with_property(world, idr_with_wildcard, dst_type, ra, o);
        }
    }

}

static
ecs_table_t* flecs_find_table_with(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t with)
{    
    ecs_ensure_id(world, with);
    
    ecs_id_record_t *idr = NULL;
    ecs_entity_t r = 0, o = 0;
    if (ECS_HAS_ROLE(with, PAIR)) {
        r = ECS_PAIR_FIRST(with);
        o = ECS_PAIR_SECOND(with);
        idr = flecs_id_record_ensure(world, ecs_pair(r, EcsWildcard));
        if (idr->flags & EcsIdUnion) {
            ecs_type_t dst_type;
            ecs_id_t union_id = ecs_pair(EcsUnion, r);
            int res = flecs_type_new_with(&dst_type, &node->type, union_id);
            if (res == -1) {
                return node;
            }
            return find_or_create(world, &dst_type, true, node);
        } else if (idr->flags & EcsIdExclusive) {
            /* Relationship is exclusive, check if table already has it */
            const ecs_table_record_t *tr = flecs_id_record_get_table(idr, node);
            if (tr) {
                /* Table already has an instance of the relationship, create
                    * a new id sequence with the existing id replaced */
                ecs_type_t dst_type = flecs_type_copy(&node->type);
                dst_type.array[tr->column] = with;
                return find_or_create(world, &dst_type, true, node);
            }
        }
    } else {
        idr = flecs_id_record_ensure(world, with);
        r = with;
    }

    /* Create sequence with new id */
    ecs_type_t dst_type;
    int res = flecs_type_new_with(&dst_type, &node->type, with);
    if (res == -1) {
        return node; /* Current table already has id */
    }

    if (r == EcsIsA) {
        /* If adding a prefab, check if prefab has overrides */
        flecs_add_overrides_for_base(world, &dst_type, with);
    }

    if (idr->flags & EcsIdWith) {
        ecs_id_record_t *idr_with_wildcard = flecs_id_record_get(world,
            ecs_pair(EcsWith, EcsWildcard));
        /* If id has With property, add targets to type */
        flecs_add_with_property(world, idr_with_wildcard, &dst_type, r, o);
    }

    return find_or_create(world, &dst_type, true, node);
}

static
ecs_table_t* flecs_find_table_without(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t without)
{
    if (ECS_HAS_ROLE(without, PAIR)) {
        ecs_entity_t r = 0;
        ecs_id_record_t *idr = NULL;
        r = ECS_PAIR_FIRST(without);
        idr = flecs_id_record_get(world, ecs_pair(r, EcsWildcard));
        if (idr && idr->flags & EcsIdUnion) {
            without = ecs_pair(EcsUnion, r);
        }
    }

    /* Create sequence with new id */
    ecs_type_t dst_type;
    int res = flecs_type_new_without(&dst_type, &node->type, without);
    if (res == -1) {
        return node; /* Current table does not have id */
    }

    return find_or_create(world, &dst_type, true, node);
}

static
void init_edge(
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->hdr.next == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->hdr.prev == NULL, ECS_INTERNAL_ERROR, NULL);
    
    edge->from = table;
    edge->to = to;
    edge->id = id;
}

static
void flecs_init_edge_for_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    init_edge(table, edge, id, to);

    ensure_hi_edge(world, &table->node.add, id);

    if (table != to) {
        /* Add edges are appended to refs.next */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *next = to_refs->next;
        
        to_refs->next = &edge->hdr;
        edge->hdr.prev = to_refs;

        edge->hdr.next = next;
        if (next) {
            next->prev = &edge->hdr;
        }

        compute_table_diff(world, table, to, edge, id);
    }
}

static
void flecs_init_edge_for_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    init_edge(table, edge, id, to);

    ensure_hi_edge(world, &table->node.remove, id);

    if (table != to) {
        /* Remove edges are appended to refs.prev */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *prev = to_refs->prev;

        to_refs->prev = &edge->hdr;
        edge->hdr.next = to_refs;

        edge->hdr.prev = prev;
        if (prev) {
            prev->next = &edge->hdr;
        }

        compute_table_diff(world, table, to, edge, id);
    }
}

static
ecs_table_t* flecs_create_edge_for_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_graph_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *to = flecs_find_table_without(world, node, id);
    
    flecs_init_edge_for_remove(world, node, edge, id, to);

    return to;   
}

static
ecs_table_t* flecs_create_edge_for_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_graph_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *to = flecs_find_table_with(world, node, id);

    flecs_init_edge_for_add(world, node, edge, id, to);

    return to;
}

static
void populate_diff( 
    ecs_graph_edge_t *edge,
    ecs_id_t *add_ptr,
    ecs_id_t *remove_ptr,
    ecs_table_diff_t *out)
{
    if (out) {
        ecs_table_diff_t *diff = edge->diff;

        if (diff && diff != &ecs_table_edge_is_component) {
            *out = *diff;
        } else {
            out->on_set.count = 0;

            if (add_ptr) {
                out->added.array = add_ptr;
                out->added.count = 1;
            } else {
                out->added.count = 0;
            }

            if (remove_ptr) {
                out->removed.array = remove_ptr;
                out->removed.count = 1;
                if (diff == &ecs_table_edge_is_component) {
                    out->un_set.array = remove_ptr;
                    out->un_set.count = 1;
                } else {
                    out->un_set.count = 0;
                }
            } else {
                out->removed.count = 0;
                out->un_set.count = 0;
            }
        }
    }
}

ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    ecs_poly_assert(world, ecs_world_t);

    node = node ? node : &world->store.root;

    /* Removing 0 from an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = ensure_edge(world, &node->node.remove, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_create_edge_for_remove(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(edge, NULL, id_ptr, diff);

    return to;
error:
    return NULL;
}

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    ecs_poly_assert(world, ecs_world_t);

    node = node ? node : &world->store.root;

    /* Adding 0 to an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = ensure_edge(world, &node->node.add, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_create_edge_for_add(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(edge, id_ptr, NULL, diff);

    return to;
error:
    return NULL;
}

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    ecs_type_t *type)
{
    ecs_poly_assert(world, ecs_world_t);
    return find_or_create(world, type, false, NULL);
}

void flecs_init_root_table(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);

    world->store.root.type = (ecs_type_t){0};

    init_table(world, &world->store.root, NULL);

    /* Ensure table indices start at 1, as 0 is reserved for the root */
    uint64_t new_id = flecs_sparse_new_id(&world->store.tables);
    ecs_assert(new_id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)new_id;
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_poly_assert(world, ecs_world_t);

    ecs_log_push_1();

    ecs_map_iter_t it;
    ecs_graph_node_t *table_node = &table->node;
    ecs_graph_edges_t *node_add = &table_node->add;
    ecs_graph_edges_t *node_remove = &table_node->remove;
    ecs_map_t *add_hi = &node_add->hi;
    ecs_map_t *remove_hi = &node_remove->hi;
    ecs_graph_edge_hdr_t *node_refs = &table_node->refs;
    ecs_graph_edge_t *edge;
    uint64_t key;

    /* Cleanup outgoing edges */
    it = ecs_map_iter(add_hi);
    while ((edge = ecs_map_next_ptr(&it, ecs_graph_edge_t*, &key))) {
        disconnect_edge(world, key, edge);
    }

    it = ecs_map_iter(remove_hi);
    while ((edge = ecs_map_next_ptr(&it, ecs_graph_edge_t*, &key))) {
        disconnect_edge(world, key, edge);
    }

    /* Cleanup incoming add edges */
    ecs_graph_edge_hdr_t *next, *cur = node_refs->next;
    if (cur) {
        do {
            edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->next;
            remove_edge(world, &edge->from->node.add, edge->id, edge);
        } while ((cur = next));
    }

    /* Cleanup incoming remove edges */
    cur = node_refs->prev;
    if (cur) {
        do {
            edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->prev;
            remove_edge(world, &edge->from->node.remove, edge->id, edge);
        } while ((cur = next));
    }

    ecs_os_free(node_add->lo);
    ecs_os_free(node_remove->lo);
    ecs_map_fini(add_hi);
    ecs_map_fini(remove_hi);
    table_node->add.lo = NULL;
    table_node->remove.lo = NULL;

    ecs_log_pop_1();
}

/* Public convenience functions for traversing table graph */
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_add(world, table, &id, NULL);
}

ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_remove(world, table, &id, NULL);
}
