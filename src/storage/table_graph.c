/**
 * @file storage/table_graph.c
 * @brief Data structure to speed up table transitions.
 * 
 * The table graph is used to speed up finding tables in add/remove operations.
 * For example, if component C is added to an entity in table [A, B], the entity
 * must be moved to table [A, B, C]. The graph speeds this process up with an
 * edge for component C that connects [A, B] to [A, B, C].
 */

#include "../private_api.h"

/* Compute hash for a type based on its component id array. */
static
uint64_t flecs_type_hash(const void *ptr) {
    const ecs_type_t *type = ptr;
    ecs_id_t *ids = type->array;
    int32_t count = type->count;
    return flecs_hash(ids, count * ECS_SIZEOF(ecs_id_t));
}

/* Compare two types lexicographically by their component id arrays. */
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

/* Initialize hashmap for looking up tables by type. */
void flecs_table_hashmap_init(
    ecs_world_t *world,
    ecs_hashmap_t *hm)
{
    flecs_hashmap_init(hm, ecs_type_t, ecs_table_t*, 
        flecs_type_hash, flecs_type_compare, &world->allocator);
}

/* Find insertion index for an id in a sorted type array. */
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

/* Find index of a matching id in a type array. */
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
        if (!ECS_IS_PAIR(id) && (cur > id)) {
            return -1;
        }
    }

    return -1;
}

/* Find index of an id in a type, comparing only lower 32 bits (ignoring generation). */
static
int flecs_type_find_ignoring_generation(
    const ecs_type_t *type,
    ecs_id_t id)
{
    ecs_id_t *array = type->array;
    int32_t i, count = type->count;

    for (i = 0; i < count; i ++) {
        if ((uint32_t)array[i] == (uint32_t)id) {
            return i;
        }
    }

    return -1;
}

/* Count consecutive ids matching a wildcard starting at the given offset. */
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

/* Create a new type by inserting an id into a sorted source type. */
static
int flecs_type_new_with(
    ecs_world_t *world,
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
    ecs_id_t *dst_array = flecs_walloc_n(world, ecs_id_t, dst_count);
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

/* Create a new type by filtering out ids matching a wildcard. */
static
int flecs_type_new_filtered(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t wildcard,
    int32_t at)
{
    *dst = flecs_type_copy(world, src);
    ecs_id_t *dst_array = dst->array;
    ecs_id_t *src_array = src->array;
    if (at) {
        ecs_assert(dst_array != NULL, ECS_INTERNAL_ERROR, NULL);
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
        if (w) {
            dst->array = flecs_wrealloc_n(world, ecs_id_t, w, count, dst->array);
        } else {
            flecs_wfree_n(world, ecs_id_t, count, dst->array);
            dst->array = NULL;
        }
    }

    return 0;
}

/* Create a new type by removing an id from a source type. */
static
int flecs_type_new_without(
    ecs_world_t *world,
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
        if (ECS_IS_PAIR(without)) {
            ecs_entity_t r = ECS_PAIR_FIRST(without);
            ecs_entity_t o = ECS_PAIR_SECOND(without);
            if (r == EcsWildcard && o != EcsWildcard) {
                return flecs_type_new_filtered(world, dst, src, without, at);
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

    ecs_id_t *dst_array = flecs_walloc_n(world, ecs_id_t, dst_count);
    dst->array = dst_array;

    ecs_assert(dst_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_array != NULL, ECS_INTERNAL_ERROR, NULL);

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

/* Create a new type by removing an id, comparing only lower 32 bits. */
static
int flecs_type_new_without_ignoring_generation(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t without)
{
    ecs_assert(!ecs_id_is_wildcard(without), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!ECS_IS_PAIR(without), ECS_INVALID_PARAMETER, NULL);

    ecs_id_t *src_array = src->array;
    int32_t at = flecs_type_find_ignoring_generation(src, without);
    if (at == -1) {
        return -1;
    }

    int32_t src_count = src->count;
    if (src_count == 1) {
        dst->array = NULL;
        dst->count = 0;
        return 0;
    }

    int32_t dst_count = src_count - 1;
    ecs_id_t *dst_array = flecs_walloc_n(world, ecs_id_t, dst_count);
    dst->array = dst_array;
    dst->count = dst_count;

    ecs_assert(dst_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_array != NULL, ECS_INTERNAL_ERROR, NULL);

    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t remain = dst_count - at;
    if (remain) {
        ecs_os_memcpy_n(
            &dst_array[at], &src_array[at + 1], ecs_id_t, remain);
    }

    return 0;
}

/* Copy a type by duplicating its id array. */
ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src)
{
    int32_t src_count = src->count;
    if (!src_count) {
        return (ecs_type_t){ 0 };
    }

    ecs_id_t *ids = flecs_walloc_n(world, ecs_id_t, src_count);
    ecs_os_memcpy_n(ids, src->array, ecs_id_t, src_count);
    return (ecs_type_t) {
        .array = ids,
        .count = src_count
    };
}

/* Free the id array owned by a type. */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type)
{
    int32_t count = type->count;
    if (count) {
        flecs_wfree_n(world, ecs_id_t, type->count, type->array);
    }
}

/* Add an id to an existing type, maintaining sorted order. */
void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add)
{
    ecs_type_t new_type;
    int res = flecs_type_new_with(world, &new_type, type, add);
    if (res != -1) {
        flecs_type_free(world, type);
        type->array = new_type.array;
        type->count = new_type.count;
    }
}

/* Remove an id from an existing type. */
void flecs_type_remove(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove)
{
    ecs_type_t new_type;
    int res = flecs_type_new_without(world, &new_type, type, remove);
    if (res != -1) {
        flecs_type_free(world, type);
        type->array = new_type.array;
        type->count = new_type.count;
    }
}

/* Remove from type while ignoring entity id generation. */
void flecs_type_remove_ignoring_generation(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove)
{
    ecs_type_t new_type;
    int res = flecs_type_new_without_ignoring_generation(
        world, &new_type, type, remove);
    if (res != -1) {
        flecs_type_free(world, type);
        type->array = new_type.array;
        type->count = new_type.count;
    }
}

/* Initialize a table diff builder with preallocated vectors. */
void flecs_table_diff_builder_init(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_init_t(a, &builder->added, ecs_id_t, 32);
    ecs_vec_init_t(a, &builder->removed, ecs_id_t, 32);
    builder->added_flags = 0;
    builder->removed_flags = 0;
}

/* Finalize a table diff builder and free its vectors. */
void flecs_table_diff_builder_fini(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_fini_t(a, &builder->added, ecs_id_t);
    ecs_vec_fini_t(a, &builder->removed, ecs_id_t);
}

/* Clear the added and removed vectors of a diff builder without freeing. */
void flecs_table_diff_builder_clear(
    ecs_table_diff_builder_t *builder)
{
    ecs_vec_clear(&builder->added);
    ecs_vec_clear(&builder->removed);
}

/* Build a type from a diff builder vector, copying elements from the given offset. */
static
void flecs_table_diff_build_type(
    ecs_world_t *world,
    ecs_vec_t *vec,
    ecs_type_t *type,
    int32_t offset)
{
    int32_t count = vec->count - offset;
    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);
    if (count) {
        type->array = flecs_wdup_n(world, ecs_id_t, count, 
            ECS_ELEM_T(vec->array, ecs_id_t, offset));
        type->count = count;
        ecs_vec_set_count_t(&world->allocator, vec, ecs_id_t, offset);
    }
}

/* Build a table diff from a diff builder, allocating copies of added/removed id arrays. */
void flecs_table_diff_build(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder,
    ecs_table_diff_t *diff,
    int32_t added_offset,
    int32_t removed_offset)
{
    flecs_table_diff_build_type(world, &builder->added, &diff->added, 
        added_offset);
    flecs_table_diff_build_type(world, &builder->removed, &diff->removed, 
        removed_offset);
    diff->added_flags = builder->added_flags;
    diff->removed_flags = builder->removed_flags;
}

/* Build a table diff without allocating, referencing builder arrays directly. */
void flecs_table_diff_build_noalloc(
    ecs_table_diff_builder_t *builder,
    ecs_table_diff_t *diff)
{
    diff->added = (ecs_type_t){
        .array = builder->added.array, .count = builder->added.count };
    diff->removed = (ecs_type_t){
        .array = builder->removed.array, .count = builder->removed.count };
    diff->added_flags = builder->added_flags;
    diff->removed_flags = builder->removed_flags;
}

/* Append a type's id array to a diff builder vector. */
static
void flecs_table_diff_build_add_type_to_vec(
    ecs_world_t *world,
    ecs_vec_t *vec,
    ecs_type_t *add)
{
    if (!add || !add->count) {
        return;
    }

    int32_t offset = vec->count;
    ecs_vec_grow_t(&world->allocator, vec, ecs_id_t, add->count);
    ecs_os_memcpy_n(ecs_vec_get_t(vec, ecs_id_t, offset),
        add->array, ecs_id_t, add->count);
}

/* Append a table diff's added/removed ids to a diff builder. */
void flecs_table_diff_build_append_table(
    ecs_world_t *world,
    ecs_table_diff_builder_t *dst,
    ecs_table_diff_t *src)
{
    flecs_table_diff_build_add_type_to_vec(world, &dst->added, &src->added);
    flecs_table_diff_build_add_type_to_vec(world, &dst->removed, &src->removed);
    dst->added_flags |= src->added_flags;
    dst->removed_flags |= src->removed_flags;
}

/* Free a table diff and its owned id arrays. */
static
void flecs_table_diff_free(
    ecs_world_t *world,
    ecs_table_diff_t *diff)
{
    flecs_wfree_n(world, ecs_id_t, diff->added.count, diff->added.array);
    flecs_wfree_n(world, ecs_id_t, diff->removed.count, diff->removed.array);
    flecs_bfree(&world->allocators.table_diff, diff);
}

/* Ensure a graph edge exists in the hi-id map, creating one if needed. */
static
ecs_graph_edge_t* flecs_table_ensure_hi_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    if (!edges->hi) {
        edges->hi = flecs_alloc_t(&world->allocator, ecs_map_t);
        ecs_map_init(edges->hi, &world->allocator);
    }

    ecs_graph_edge_t **r = ecs_map_ensure_ref(edges->hi, ecs_graph_edge_t, id);
    ecs_graph_edge_t *edge = r[0];
    if (edge) {
        return edge;
    }

    if (id < FLECS_HI_COMPONENT_ID) {
        edge = &edges->lo[id];
    } else {
        edge = flecs_bcalloc(&world->allocators.graph_edge);
    }

    r[0] = edge;
    return edge;
}

/* Ensure a graph edge exists, using the lo array for small ids or the hi map otherwise. */
static
ecs_graph_edge_t* flecs_table_ensure_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    ecs_graph_edge_t *edge;
    
    if (id < FLECS_HI_COMPONENT_ID) {
        if (!edges->lo) {
            edges->lo = flecs_bcalloc(&world->allocators.graph_edge_lo);
        }
        edge = &edges->lo[id];
    } else {
        edge = flecs_table_ensure_hi_edge(world, edges, id);
    }

    return edge;
}

/* Disconnect a graph edge by unlinking it and freeing its diff data. */
static
void flecs_table_disconnect_edge(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == id, ECS_INTERNAL_ERROR, NULL);
    (void)id;

    /* Unlink from destination table's incoming edge list */
    ecs_graph_edge_hdr_t *next = edge->hdr.next;
    ecs_graph_edge_hdr_t *prev = edge->hdr.prev;

    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }

    ecs_table_diff_t *diff = edge->diff;
    if (diff) {
        flecs_table_diff_free(world, diff);
    }

    /* Low ids live in the lo array; clear instead of freeing */
    if (id < FLECS_HI_COMPONENT_ID) {
        ecs_os_memset_t(edge, 0, ecs_graph_edge_t);
    } else {
        flecs_bfree(&world->allocators.graph_edge, edge);
    }
}

/* Remove a graph edge from the hi map and disconnect it. */
static
void flecs_table_remove_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edges != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edges->hi != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!edge->id) {
        return;
    }
    flecs_table_disconnect_edge(world, id, edge);
    ecs_map_remove(edges->hi, id);
}

/* Initialize graph edges struct to empty state. */
static
void flecs_table_init_edges(
    ecs_graph_edges_t *edges)
{
    edges->lo = NULL;
    edges->hi = NULL;
}

/* Initialize a graph node's add and remove edges. */
static
void flecs_table_init_node(
    ecs_graph_node_t *node)
{
    flecs_table_init_edges(&node->add);
    flecs_table_init_edges(&node->remove);
}

/* Initialize a table's flags, graph node, and component storage. */
static
void flecs_init_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *prev)
{
    table->flags = 0;
    table->dirty_state = NULL;
    table->_->lock = 0;
    table->_->generation = 0;

    flecs_table_init_node(&table->node);

    flecs_table_init(world, table, prev);
}

/* Allocate and initialize a new table in the world's sparse table set. */
static
ecs_table_t *flecs_table_new(
    ecs_world_t *world,
    ecs_type_t *type,
    flecs_hashmap_result_t table_elem,
    ecs_table_t *prev)
{
    flecs_check_exclusive_world_access_write(world);

    ecs_os_perf_trace_push("flecs.table.create");

    ecs_table_t *result = flecs_sparse_add_t(&world->store.tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
    result->_ = flecs_calloc_t(&world->allocator, ecs_table__t);
    ecs_assert(result->_ != NULL, ECS_INTERNAL_ERROR, NULL);

#ifdef FLECS_SANITIZE
    int32_t i, j, count = type->count;
    for (i = 0; i < count - 1; i ++) {
        if (type->array[i] >= type->array[i + 1]) {
            for (j = 0; j < count; j ++) {
                char *str = ecs_id_str(world, type->array[j]);
                if (i == j) {
                    ecs_err(" > %d: %s", j, str);
                } else {
                    ecs_err("   %d: %s", j, str);
                }
                ecs_os_free(str);
            }
            ecs_abort(ECS_CONSTRAINT_VIOLATED, "table type is not ordered");
        }
    }
#endif

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

    *(ecs_table_t**)table_elem.value = result;
    /* Point key to table-owned type so it persists with the table */
    *(ecs_type_t*)table_elem.key = result->type;
    result->_->hash = table_elem.hash;

    flecs_init_table(world, result, prev);

    world->info.table_count ++;
    world->info.table_create_total ++;

    ecs_log_pop_2();

    ecs_os_perf_trace_pop("flecs.table.create");

    return result;
}

/* Find an existing table for a type or create a new one. */
static
ecs_table_t* flecs_table_ensure(
    ecs_world_t *world,
    ecs_type_t *type,
    bool own_type,
    ecs_table_t *prev)
{    
    flecs_poly_assert(world, ecs_world_t);   

    int32_t id_count = type->count;
    if (!id_count) {
        return &world->store.root;
    }

    ecs_table_t *table;
    flecs_hashmap_result_t elem = flecs_hashmap_ensure(
        &world->store.table_map, type, ecs_table_t*);
    if ((table = *(ecs_table_t**)elem.value)) {
        if (own_type) {
            flecs_type_free(world, type);
        }
        return table;
    }

    /* Table not found; create it (not allowed during readonly/progress) */
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    if (own_type) {
        return flecs_table_new(world, type, elem, prev);
    }

    ecs_type_t copy = flecs_type_copy(world, type);
    return flecs_table_new(world, &copy, elem, prev);
}

/* Append an id to the added list of a diff builder. */
static
void flecs_diff_insert_added(
    ecs_world_t *world,
    ecs_table_diff_builder_t *diff,
    ecs_id_t id)
{
    ecs_vec_append_t(&world->allocator, &diff->added, ecs_id_t)[0] = id;
}

/* Append an id to the removed list of a diff builder. */
static
void flecs_diff_insert_removed(
    ecs_world_t *world,
    ecs_table_diff_builder_t *diff,
    ecs_id_t id)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_append_t(a, &diff->removed, ecs_id_t)[0] = id;
}

/* Check whether all entities referenced by an id are alive. */
static
bool flecs_id_is_alive(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        if (!flecs_entities_get_alive(world, ECS_PAIR_FIRST(id))) {
            return false;
        }
        if (!flecs_entities_get_alive(world, ECS_PAIR_SECOND(id))) {
            return false;
        }
        return true;
    } else {
        return flecs_entities_get_alive(world, id & ECS_COMPONENT_MASK) != 0;
    }
}

/* Compute the component diff between two tables and store it on the edge. */
static
void flecs_compute_table_diff(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_table_t *next,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    bool is_remove)
{
    ecs_type_t node_type = node->type;
    ecs_type_t next_type = next->type;
    bool childof = false;

    if (ECS_IS_PAIR(id)) { 
        childof = ECS_PAIR_FIRST(id) ==  EcsChildOf;
    }

    ecs_component_record_t *cr = NULL;

    if (!flecs_id_is_alive(world, id)) {
        return;
    }

    bool dont_fragment = false;
    if (id < FLECS_HI_COMPONENT_ID) {
        dont_fragment = (world->non_trivial_lookup[id] & 
            EcsNonTrivialIdNonFragmenting) != 0;
        if (dont_fragment) {
            cr = flecs_components_ensure(world, id);
        }
    } else {
        cr = flecs_components_ensure(world, id);
        dont_fragment = cr->flags & EcsIdDontFragment;
    }

    if (dont_fragment) {
        ecs_table_diff_t *diff = flecs_bcalloc(
            &world->allocators.table_diff);
        if (is_remove) {
            diff->removed.count = 1;
            diff->removed.array = flecs_wdup_n(world, ecs_id_t, 1, &id);
            diff->removed_flags = EcsTableHasDontFragment|EcsTableHasSparse;
        } else {
            diff->added.count = 1;
            diff->added.array = flecs_wdup_n(world, ecs_id_t, 1, &id);
            diff->added_flags = EcsTableHasDontFragment|EcsTableHasSparse;
        }
        edge->diff = diff;

        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_append_t(&world->allocator, 
            &cr->dont_fragment_tables, uint64_t)[0] = node->id;
        return;
    }

    ecs_id_t *ids_node = node_type.array;
    ecs_id_t *ids_next = next_type.array;
    int32_t i_node = 0, node_count = node_type.count;
    int32_t i_next = 0, next_count = next_type.count;
    int32_t added_count = 0;
    int32_t removed_count = 0;
    ecs_flags32_t added_flags = 0, removed_flags = 0;
    bool trivial_edge = !ECS_HAS_RELATION(id, EcsIsA) && !childof;

    /* First pass: count added/removed ids to size the diff */
    for (; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        bool added = id_next < id_node;
        bool removed = id_node < id_next;

        trivial_edge &= !added || id_next == id;
        trivial_edge &= !removed || id_node == id;

        if (added) {
            added_flags |= flecs_id_flags_get(world, id_next) & 
                EcsTableAddEdgeFlags;
            added_count ++;
        }

        if (removed) {
            removed_flags |= flecs_id_flags_get(world, id_node) & 
                EcsTableRemoveEdgeFlags;
            removed_count ++;
        }

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    for (; i_next < next_count; i_next ++) {
        added_flags |= flecs_id_flags_get(world, ids_next[i_next]) & 
            EcsTableAddEdgeFlags;
        added_count ++;
    }

    for (; i_node < node_count; i_node ++) {
        removed_flags |= flecs_id_flags_get(world, ids_node[i_node]) & 
            EcsTableRemoveEdgeFlags;
        removed_count ++;
    }

    trivial_edge &= (added_count + removed_count) <= 1 && 
        !ecs_id_is_wildcard(id) && !(added_flags|removed_flags);

    if (trivial_edge) {
        return; /* Single add/remove with no side effects needs no diff */
    }

    ecs_table_diff_builder_t *builder = &world->allocators.diff_builder;
    int32_t added_offset = builder->added.count;
    int32_t removed_offset = builder->removed.count;

    for (i_node = 0, i_next = 0; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        if (id_next < id_node) {
            flecs_diff_insert_added(world, builder, id_next);
        } else if (id_node < id_next) {
            flecs_diff_insert_removed(world, builder, id_node);
        }

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    for (; i_next < next_count; i_next ++) {
        flecs_diff_insert_added(world, builder, ids_next[i_next]);
    }

    for (; i_node < node_count; i_node ++) {
        flecs_diff_insert_removed(world, builder, ids_node[i_node]);
    }

    ecs_table_diff_t *diff = flecs_bcalloc(&world->allocators.table_diff);
    edge->diff = diff;
    flecs_table_diff_build(world, builder, diff, added_offset, removed_offset);
    diff->added_flags = added_flags;
    diff->removed_flags = removed_flags;

    if (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsChildOf) {
        if (added_count) {
            diff->added_flags |= EcsTableEdgeReparent;
        } else {
            ecs_assert(removed_count != 0, ECS_INTERNAL_ERROR, NULL);
            diff->removed_flags |= EcsTableEdgeReparent;
        }
    }

    ecs_assert(diff->added.count == added_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(diff->removed.count == removed_count, ECS_INTERNAL_ERROR, NULL);
}

/* Recursively add override components from a base entity to a destination type. */
static
void flecs_add_overrides_for_base(
    ecs_world_t *world,
    ecs_type_t *dst_type,
    ecs_id_t pair)
{
    ecs_entity_t base = ecs_pair_second(world, pair);
    ecs_assert(base != 0, ECS_INVALID_PARAMETER, 
        "target of IsA pair is not alive");
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
            ecs_id_t to_add = 0;
            if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
                to_add = id & ~ECS_AUTO_OVERRIDE;

                ecs_flags32_t cr_flags = flecs_component_get_flags(world, to_add);
                if (cr_flags & EcsIdDontFragment) {
                    to_add = 0;
                    base_table->flags |= EcsTableOverrideDontFragment;
                }
            } else {
                ecs_table_record_t *tr = &base_table->_->records[i];
                if (ECS_ID_ON_INSTANTIATE(tr->hdr.cr->flags) == EcsOverride) {
                    to_add = id;
                }
            }

            if (to_add) {
                ecs_id_t wc = ecs_pair(ECS_PAIR_FIRST(to_add), EcsWildcard);
                bool exclusive = false;
                if (ECS_IS_PAIR(to_add)) {
                    ecs_component_record_t *cr = flecs_components_get(world, wc);
                    if (cr) {
                        exclusive = (cr->flags & EcsIdExclusive) != 0;
                    }
                }
                if (!exclusive) {
                    flecs_type_add(world, dst_type, to_add);
                } else {
                    int32_t column = flecs_type_find(dst_type, wc);
                    if (column == -1) {
                        flecs_type_add(world, dst_type, to_add);
                    }
                }
            }
        }
    }

    if (flags & EcsTableHasIsA) {
        const ecs_table_record_t *tr = flecs_component_get_table(
            world->cr_isa_wildcard, base_table);
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t i = tr->index, end = i + tr->count;
        for (; i != end; i ++) {
            flecs_add_overrides_for_base(world, dst_type, ids[i]);
        }
    }
}

/* Recursively add ids required by the With relationship property. */
static
void flecs_add_with_property(
    ecs_world_t *world,
    ecs_component_record_t *cr_with_wildcard,
    ecs_type_t *dst_type,
    ecs_entity_t r,
    ecs_entity_t o)
{
    r = ecs_get_alive(world, r);

    /* Find With pairs on the relationship entity */
    ecs_table_t *table = ecs_get_table(world, r);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    
    const ecs_table_record_t *tr = flecs_component_get_table(
        cr_with_wildcard, table);
    if (tr) {
        int32_t i = tr->index, end = i + tr->count;
        ecs_id_t *ids = table->type.array;

        for (; i < end; i ++) {
            ecs_id_t id = ids[i];
            ecs_assert(ECS_PAIR_FIRST(id) == EcsWith, ECS_INTERNAL_ERROR, NULL);
            ecs_id_t ra = ECS_PAIR_SECOND(id);
            ecs_id_t a = ra;
            if (o) {
                a = ecs_pair(ra, o);
            }

            flecs_type_add(world, dst_type, a);
            flecs_add_with_property(world, cr_with_wildcard, dst_type, ra, o);
        }
    }
}

/* Find or create a table that includes the given id. */
static
ecs_table_t* flecs_find_table_with(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t with)
{
    ecs_make_alive_id(world, with);

    ecs_component_record_t *cr = NULL;
    ecs_entity_t r = 0, o = 0;

    if (ECS_IS_PAIR(with)) {
        r = ECS_PAIR_FIRST(with);
        o = ECS_PAIR_SECOND(with);
        cr = flecs_components_ensure(world, ecs_pair(r, EcsWildcard));
        if (cr->flags & EcsIdExclusive) {
            const ecs_table_record_t *tr = flecs_component_get_table(cr, node);
            if (tr) {
                /* Exclusive: replace existing target instead of adding */
                ecs_type_t dst_type = flecs_type_copy(world, &node->type);
                ecs_assert(dst_type.array != NULL, ECS_INTERNAL_ERROR, NULL);
                dst_type.array[tr->index] = with;
                return flecs_table_ensure(world, &dst_type, true, node);
            }
        }
    } else {
        cr = flecs_components_ensure(world, with);
        r = with;
    }

    if (cr->flags & EcsIdDontFragment) {
        node->flags |= EcsTableHasDontFragment;
        return node;
    }

    /* Create type with new id inserted */
    ecs_type_t dst_type;
    int res = flecs_type_new_with(world, &dst_type, &node->type, with);
    if (res == -1) {
        return node; /* Current table already has id */
    }

    if (r == EcsIsA) {
        flecs_add_overrides_for_base(world, &dst_type, with);
    } else if (r == EcsChildOf) {
        o = ecs_get_alive(world, o);
        if (ecs_has_id(world, o, EcsPrefab)) {
            flecs_type_add(world, &dst_type, EcsPrefab);
        }
    }

    if (cr->flags & EcsIdWith) {
        ecs_component_record_t *cr_with_wildcard = flecs_components_get(world,
            ecs_pair(EcsWith, EcsWildcard));
        flecs_add_with_property(world, cr_with_wildcard, &dst_type, r, o);
    }

    if (with == ecs_id(EcsParent)) {
        if (node->flags & EcsTableHasChildOf) {
            flecs_type_remove(world, &dst_type, 
                ecs_pair(EcsChildOf, EcsWildcard));
        }
    } else if (ECS_PAIR_FIRST(with) == EcsChildOf) {
        if (node->flags & EcsTableHasParent) {
            flecs_type_remove(world, &dst_type, ecs_id(EcsParent));
            flecs_type_remove(world, &dst_type, 
                ecs_pair(EcsParentDepth, EcsWildcard));
        }
    }

    return flecs_table_ensure(world, &dst_type, true, node);
}

/* Find or create a table that excludes the given id. */
static
ecs_table_t* flecs_find_table_without(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t without)
{
    ecs_component_record_t *cr = NULL;

    if (ECS_IS_PAIR(without)) {
        ecs_entity_t r = ECS_PAIR_FIRST(without);
        cr = flecs_components_get(world, ecs_pair(r, EcsWildcard));
        if (cr) {
            if (cr->flags & EcsIdDontFragment) {
                node->flags |= EcsTableHasDontFragment;
                return node;
            }
        }
    } else {
        cr = flecs_components_get(world, without);
        if (cr && cr->flags & EcsIdDontFragment) {
            node->flags |= EcsTableHasDontFragment;
            /* Component doesn't fragment tables */
            return node;
        }
    }

    /* Create type with id removed */
    ecs_type_t dst_type;
    int res = flecs_type_new_without(world, &dst_type, &node->type, without);
    if (res == -1) {
        return node; /* Current table does not have id */
    }

    if (without == ecs_id(EcsParent)) {
        flecs_type_remove(world, &dst_type, 
            ecs_pair(EcsParentDepth, EcsWildcard));
    }

    return flecs_table_ensure(world, &dst_type, true, node);
}

/* Initialize a graph edge with source table, destination, and id. */
static
void flecs_table_init_edge(
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

/* Initialize an add edge, link it to the destination's incoming refs, and compute the diff. */
static
void flecs_init_edge_for_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    flecs_table_init_edge(table, edge, id, to);

    flecs_table_ensure_hi_edge(world, &table->node.add, id);

    if ((table != to) || (table->flags & EcsTableHasDontFragment)) {
        /* Add edges are appended to refs.next */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *next = to_refs->next;
        
        to_refs->next = &edge->hdr;
        edge->hdr.prev = to_refs;

        edge->hdr.next = next;
        if (next) {
            next->prev = &edge->hdr;
        }

        flecs_compute_table_diff(world, table, to, edge, id, false);
    }
}

/* Initialize a remove edge, link it to the destination's incoming refs, and compute the diff. */
static
void flecs_init_edge_for_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    flecs_table_init_edge(table, edge, id, to);

    flecs_table_ensure_hi_edge(world, &table->node.remove, id);

    if ((table != to) || (table->flags & EcsTableHasDontFragment)) {
        /* Remove edges are appended to refs.prev */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *prev = to_refs->prev;

        to_refs->prev = &edge->hdr;
        edge->hdr.next = to_refs;

        edge->hdr.prev = prev;
        if (prev) {
            prev->next = &edge->hdr;
        }

        flecs_compute_table_diff(world, table, to, edge, id, true);
    }
}

/* Find the destination table for removing an id and create the remove edge. */
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

/* Find the destination table for adding an id and create the add edge. */
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

/* Traverse the table graph along a remove edge to find the destination table. */
ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Removing 0 from an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = flecs_table_ensure_edge(world, &node->node.remove, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_create_edge_for_remove(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (node != to || edge->diff) {
        if (edge->diff) {
            *diff = *edge->diff;
        } else {
            diff->added.count = 0;
            diff->removed.array = id_ptr;
            diff->removed.count = 1;
        }
    }

    return to;
error:
    return NULL;
}

/* Traverse the table graph along an add edge to find the destination table. */
ecs_table_t* flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Adding 0 to an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = flecs_table_ensure_edge(world, &node->node.add, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_create_edge_for_add(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (node != to || edge->diff) {
        if (edge->diff) {
            *diff = *edge->diff;
        } else {
            diff->added.array = id_ptr;
            diff->added.count = 1;
            diff->removed.count = 0;
        }
    }

    return to;
error:
    return NULL;
}

/* Find or create a table for the given component type. */
ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    ecs_type_t *type)
{
    flecs_poly_assert(world, ecs_world_t);
    return flecs_table_ensure(world, type, false, NULL);
}

/* Initialize the root table (the empty archetype). */
void flecs_init_root_table(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);

    world->store.root.type = (ecs_type_t){0};
    world->store.root._ = flecs_calloc_t(&world->allocator, ecs_table__t);
    flecs_init_table(world, &world->store.root, NULL);

    /* Reserve index 0 for the root table */
    uint64_t new_id = flecs_sparse_new_id(&world->store.tables);
    ecs_assert(new_id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)new_id;
}

/* Add observer flags to incoming add edges and outgoing remove edges for an id. */
void flecs_table_edges_add_flags(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_flags32_t flags)
{
    ecs_graph_node_t *table_node = &table->node;
    ecs_graph_edge_hdr_t *node_refs = &table_node->refs;

    /* Add flags to incoming matching add edges */
    if (flags == EcsTableHasOnAdd) {
        ecs_graph_edge_hdr_t *next, *cur = node_refs->next;
        if (cur) {
            do {
                ecs_graph_edge_t *edge = (ecs_graph_edge_t*)cur;
                if ((id == EcsAny) || ecs_id_match(edge->id, id)) {
                    if (!edge->diff) {
                        edge->diff = flecs_bcalloc(&world->allocators.table_diff);
                        edge->diff->added.array = flecs_walloc_t(world, ecs_id_t);
                        edge->diff->added.count = 1;
                        edge->diff->added.array[0] = edge->id;
                    }
                    edge->diff->added_flags |= EcsTableHasOnAdd;
                }
                next = cur->next;
            } while ((cur = next));
        }
    }

    /* Add flags to outgoing matching remove edges */
    if (flags == EcsTableHasOnRemove) {
        ecs_map_iter_t it = ecs_map_iter(table->node.remove.hi);
        while (ecs_map_next(&it)) {
            ecs_id_t edge_id = ecs_map_key(&it);
            if ((id == EcsAny) || ecs_id_match(edge_id, id)) {
                ecs_graph_edge_t *edge = ecs_map_ptr(&it);
                if (!edge->diff) {
                    edge->diff = flecs_bcalloc(&world->allocators.table_diff);
                    edge->diff->removed.array = flecs_walloc_t(world, ecs_id_t);
                    edge->diff->removed.count = 1;
                    edge->diff->removed.array[0] = edge->id;
                }
                edge->diff->removed_flags |= EcsTableHasOnRemove;
            }
        }
    }
}

/* Clean up all incoming and outgoing graph edges for a table. */
void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    flecs_poly_assert(world, ecs_world_t);

    ecs_log_push_1();

    ecs_map_iter_t it;
    ecs_graph_node_t *table_node = &table->node;
    ecs_graph_edges_t *node_add = &table_node->add;
    ecs_graph_edges_t *node_remove = &table_node->remove;
    ecs_map_t *add_hi = node_add->hi;
    ecs_map_t *remove_hi = node_remove->hi;
    ecs_graph_edge_hdr_t *node_refs = &table_node->refs;

    /* Cleanup outgoing edges */
    it = ecs_map_iter(add_hi);
    while (ecs_map_next(&it)) {
        flecs_table_disconnect_edge(world, ecs_map_key(&it), ecs_map_ptr(&it));
    }

    it = ecs_map_iter(remove_hi);
    while (ecs_map_next(&it)) {
        flecs_table_disconnect_edge(world, ecs_map_key(&it), ecs_map_ptr(&it));
    }

    /* Cleanup incoming add edges */
    ecs_graph_edge_hdr_t *next, *cur = node_refs->next;
    if (cur) {
        do {
            ecs_graph_edge_t *edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->next;
            flecs_table_remove_edge(world, &edge->from->node.add, edge->id, edge);
        } while ((cur = next));
    }

    /* Cleanup incoming remove edges */
    cur = node_refs->prev;
    if (cur) {
        do {
            ecs_graph_edge_t *edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->prev;
            flecs_table_remove_edge(world, &edge->from->node.remove, edge->id, edge);
        } while ((cur = next));
    }

    if (node_add->lo) {
        flecs_bfree(&world->allocators.graph_edge_lo, node_add->lo);
    }
    if (node_remove->lo) {
        flecs_bfree(&world->allocators.graph_edge_lo, node_remove->lo);
    }

    ecs_map_fini(add_hi);
    ecs_map_fini(remove_hi);
    flecs_free_t(&world->allocator, ecs_map_t, add_hi);
    flecs_free_t(&world->allocator, ecs_map_t, remove_hi);
    table_node->add.lo = NULL;
    table_node->remove.lo = NULL;
    table_node->add.hi = NULL;
    table_node->remove.hi = NULL;

    ecs_log_pop_1();
}

/* Clear add and remove graph edges for a specific component id. */
void flecs_table_clear_edges_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component)
{
    if (component < FLECS_HI_COMPONENT_ID) {
        if (table->node.add.lo) {
            ecs_graph_edge_t *add_edge = &table->node.add.lo[component];
            if (add_edge->id) {
                flecs_table_disconnect_edge(world, component, add_edge);
                ecs_map_remove(table->node.add.hi, component);
            }
        }

        if (table->node.remove.lo) {
            ecs_graph_edge_t *remove_edge = &table->node.remove.lo[component];
            if (remove_edge->id) {
                flecs_table_disconnect_edge(world, component, remove_edge);
                ecs_map_remove(table->node.remove.hi, component);
            }
        }
    } else {
        if (table->node.add.hi) {
            ecs_graph_edge_t *add_edge = ecs_map_get_ptr(
                table->node.add.hi, component);
            if (add_edge) {
                flecs_table_disconnect_edge(world, component, add_edge);
                ecs_map_remove(table->node.add.hi, component);
            }
        }

        if (table->node.remove.hi) {
            ecs_graph_edge_t *remove_edge = ecs_map_get_ptr(
                table->node.remove.hi, component);
            if (remove_edge) {
                flecs_table_disconnect_edge(world, component, remove_edge);
                ecs_map_remove(table->node.remove.hi, component);
            }
        }
    }
}

/* Find a table by adding an id via graph traversal and append the diff. */
ecs_table_t* flecs_find_table_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_builder_t *diff)
{
    ecs_table_diff_t temp_diff = ECS_TABLE_DIFF_INIT;
    table = flecs_table_traverse_add(world, table, &id, &temp_diff);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_table_diff_build_append_table(world, diff, &temp_diff);
    return table;
error:
    return NULL;
}

ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_table_diff_t diff;
    table = table ? table : &world->store.root;
    return flecs_table_traverse_add(world, table, &id, &diff);
}

ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_table_diff_t diff;
    table = table ? table : &world->store.root;
    return flecs_table_traverse_remove(world, table, &id, &diff);
}

ecs_table_t* ecs_table_find(
    ecs_world_t *world,
    const ecs_id_t *ids,
    int32_t id_count)
{
    ecs_type_t type = {
        .array = ECS_CONST_CAST(ecs_id_t*, ids),
        .count = id_count
    };
    return flecs_table_ensure(world, &type, false, NULL);
}
