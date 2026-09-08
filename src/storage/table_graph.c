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

ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src);

/* Id sequence (type) utilities */

static uint64_t flecs_type_hash(const void *ptr) {
    const ecs_type_t *type = ptr;
    ecs_id_t *ids = type->array;
    int32_t count = type->count;
    return flecs_hash(ids, count * ECS_SIZEOF(ecs_id_t));
}

static int flecs_type_compare(const void *ptr_1, const void *ptr_2) {
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

void flecs_table_hashmap_init(
    ecs_world_t *world, 
    ecs_hashmap_t *hm) 
{
    flecs_hashmap_init(hm, ecs_type_t, ecs_table_t*, 
        flecs_type_hash, flecs_type_compare, &world->allocator);
}

/* Find location where to insert id into type */
static int flecs_type_find_insert(
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
static int flecs_type_find(
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

static int flecs_type_new_with(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t with)
{
    int32_t at = flecs_type_find_insert(src, 0, with);
    if (at == -1) {
        return -1;
    }

    int32_t count = src->count;
    ecs_id_t *array;
    if (dst == src) {
        array = flecs_wrealloc_n(world, ecs_id_t, count + 1, count, src->array);
        ecs_os_memmove_n(array + at + 1, array + at, ecs_id_t, count - at);
    } else {
        array = flecs_walloc_n(world, ecs_id_t, count + 1);
        if (at) {
            ecs_os_memcpy_n(array, src->array, ecs_id_t, at);
        }
        if (count != at) {
            ecs_os_memcpy_n(array + at + 1, src->array + at, ecs_id_t, count - at);
        }
    }
    array[at] = with;
    *dst = (ecs_type_t){ .array = array, .count = count + 1 };
    return 0;
}

static int flecs_type_new_without(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t without)
{
    int32_t at = flecs_type_find(src, without);
    if (at == -1) {
        return -1;
    }
    ecs_assert(src->array != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t count = src->count, removed = 1;
    bool wildcard = ecs_id_is_wildcard(without);
    if (wildcard) {
        for (int32_t i = at + 1; i < count; i ++) {
            removed += ecs_id_match(src->array[i], without);
        }
    }

    int32_t dst_count = count - removed;
    ecs_assert(dst_count >= at, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t *array = src->array;
    if (dst != src) {
        array = dst_count ? flecs_walloc_n(world, ecs_id_t, dst_count) : NULL;
        if (at && array) {
            ecs_os_memcpy_n(array, src->array, ecs_id_t, at);
        }
    }
    if (dst_count > at) {
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);
        if (wildcard) {
            int32_t w = at;
            for (int32_t i = at + 1; i < count; i ++) {
                ecs_id_t id = src->array[i];
                if (!ecs_id_match(id, without)) {
                    array[w ++] = id;
                }
            }
        } else {
            ecs_os_memmove_n(array + at, src->array + at + 1,
                ecs_id_t, dst_count - at);
        }
    }
    if (dst == src) {
        if (dst_count) {
            array = flecs_wrealloc_n(world, ecs_id_t, dst_count, count, array);
        } else {
            flecs_wfree_n(world, ecs_id_t, count, array);
            array = NULL;
        }
    }
    *dst = (ecs_type_t){ .array = array, .count = dst_count };
    return 0;
}

/* Copy type */
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

/* Free type */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type)
{
    int32_t count = type->count;
    if (count) {
        flecs_wfree_n(world, ecs_id_t, type->count, type->array);
    }
}

void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add)
{
    flecs_type_new_with(world, type, type, add);
}

static void flecs_type_remove(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove)
{
    flecs_type_new_without(world, type, type, remove);
}

void flecs_type_remove_ignoring_generation(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove)
{
    ecs_assert(!ecs_id_is_wildcard(remove), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!ECS_IS_PAIR(remove), ECS_INVALID_PARAMETER, NULL);
    for (int32_t i = 0; i < type->count; i ++) {
        if ((uint32_t)type->array[i] == (uint32_t)remove) {
            flecs_type_remove(world, type, type->array[i]);
            return;
        }
    }
}

/* Graph edge utilities */

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

void flecs_table_diff_builder_fini(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_fini_t(a, &builder->added, ecs_id_t);
    ecs_vec_fini_t(a, &builder->removed, ecs_id_t);
}

void flecs_table_diff_builder_clear(
    ecs_table_diff_builder_t *builder)
{
    ecs_vec_clear(&builder->added);
    ecs_vec_clear(&builder->removed);
}

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

static void flecs_table_diff_build_add_type_to_vec(
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

static ecs_table_diff_t* flecs_table_diff_new(
    ecs_world_t *world,
    int32_t added,
    int32_t removed)
{
    ecs_table_diff_t *diff = flecs_wcalloc(world,
        ECS_SIZEOF(ecs_table_diff_t) + (added + removed) * ECS_SIZEOF(ecs_id_t));
    ecs_id_t *ids = (ecs_id_t*)(diff + 1);
    diff->added = (ecs_type_t){ .array = added ? ids : NULL, .count = added };
    diff->removed = (ecs_type_t){
        .array = removed ? ids + added : NULL, .count = removed };
    return diff;
}

static void flecs_table_diff_free(
    ecs_world_t *world,
    ecs_table_diff_t *diff)
{
    flecs_wfree(world, ECS_SIZEOF(ecs_table_diff_t) +
        (diff->added.count + diff->removed.count) * ECS_SIZEOF(ecs_id_t), diff);
}

static ecs_graph_edge_t* flecs_table_ensure_hi_edge(
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

static ecs_graph_edge_t* flecs_table_ensure_edge(
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
        edge = edges->hi ? ecs_map_get_ptr(edges->hi, id) : NULL;
        if (!edge) {
            edge = flecs_table_ensure_hi_edge(world, edges, id);
        }
    }

    return edge;
}

static void flecs_table_disconnect_edge(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == id, ECS_INTERNAL_ERROR, NULL);
    (void)id;

    if (edge->next) {
        edge->next->prev = edge->prev;
    }
    if (edge->prev) {
        *edge->prev = edge->next;
    }

    /* Remove data associated with edge */
    ecs_table_diff_t *diff = edge->diff;
    if (diff) {
        flecs_table_diff_free(world, diff);
    }

    /* If edge id is low, clear it from fast lookup array */
    if (id < FLECS_HI_COMPONENT_ID) {
        ecs_os_memset_t(edge, 0, ecs_graph_edge_t);
    } else {
        flecs_bfree(&world->allocators.graph_edge, edge);
    }
}

static void flecs_init_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *prev)
{
    table->flags = EcsTableEmpty;
    table->dirty_state = NULL;
    table->_->lock = 0;
    table->_->generation = 0;

    ecs_os_zeromem(&table->node);

    flecs_table_init(world, table, prev);
}

static ecs_table_t *flecs_table_new(
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

    /* Store table in table hashmap */
    *(ecs_table_t**)table_elem.value = result;

    /* Set keyvalue to one that has the same lifecycle as the table */
    *(ecs_type_t*)table_elem.key = result->type;
    result->_->hash = table_elem.hash;

    flecs_init_table(world, result, prev);

    /* Update counters */
    world->info.table_count ++;
    world->info.table_create_total ++;

    ecs_log_pop_2();

    ecs_os_perf_trace_pop("flecs.table.create");

    return result;
}

static ecs_table_t* flecs_table_ensure(
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

    /* If we get here, table needs to be created which is only allowed when the
     * application is not currently in progress */
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);

    /* If we get here, the table has not been found, so create it. */
    if (own_type) {
        return flecs_table_new(world, type, elem, prev);
    }

    ecs_type_t copy = flecs_type_copy(world, type);
    return flecs_table_new(world, &copy, elem, prev);
}

static bool flecs_id_is_alive(
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

static ecs_flags32_t flecs_table_diff_ids(
    ecs_world_t *world,
    const ecs_type_t *src,
    const ecs_type_t *dst,
    ecs_type_t *result)
{
    ecs_flags32_t flags = 0;
    int32_t count = 0, j = 0;
    for (int32_t i = 0; i < dst->count; i ++) {
        ecs_id_t id = dst->array[i];
        while (j < src->count && src->array[j] < id) {
            j ++;
        }
        if (j < src->count && src->array[j] == id) {
            continue;
        }
        if (result->array) {
            result->array[count] = id;
        } else {
            flags |= flecs_id_flags_get(world, id);
        }
        count ++;
    }
    result->count = count;
    return flags;
}

static void flecs_compute_table_diff(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_table_t *next,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    bool is_remove)
{
    if (!flecs_id_is_alive(world, id)) {
        return;
    }

    ecs_component_record_t *cr = flecs_components_ensure(world, id);
    if (cr->flags & EcsIdDontFragment) {
        ecs_table_diff_t *diff = flecs_table_diff_new(world, !is_remove, is_remove);
        if (is_remove) {
            diff->removed.array[0] = id;
            diff->removed_flags = EcsTableHasDontFragment|EcsTableHasSparse;
        } else {
            diff->added.array[0] = id;
            diff->added_flags = EcsTableHasDontFragment|EcsTableHasSparse;
        }
        edge->diff = diff;
        ecs_vec_append_t(&world->allocator,
            &cr->dont_fragment_tables, uint64_t)[0] = node->id;
        return;
    }

    ecs_type_t added = {0}, removed = {0};
    ecs_flags32_t added_flags = flecs_table_diff_ids(
        world, &node->type, &next->type, &added) & EcsTableAddEdgeFlags;
    ecs_flags32_t removed_flags = flecs_table_diff_ids(
        world, &next->type, &node->type, &removed) & EcsTableRemoveEdgeFlags;
    bool childof = ECS_HAS_RELATION(id, EcsChildOf);
    if (!ECS_HAS_RELATION(id, EcsIsA) && !childof &&
        !ecs_id_is_wildcard(id) && added.count + removed.count <= 1 &&
        !(added_flags | removed_flags))
    {
        return;
    }

    ecs_table_diff_t *diff = flecs_table_diff_new(world, added.count, removed.count);
    flecs_table_diff_ids(world, &node->type, &next->type, &diff->added);
    flecs_table_diff_ids(world, &next->type, &node->type, &diff->removed);
    diff->added_flags = added_flags;
    diff->removed_flags = removed_flags;
    if (childof) {
        if (added.count) {
            diff->added_flags |= EcsTableEdgeReparent;
        } else if (removed.count) {
            diff->removed_flags |= EcsTableEdgeReparent;
        }
    }
    edge->diff = diff;
}

static void flecs_add_overrides_for_base(
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

                    /* Add flag to base table. Cheaper to do here vs adding an
                     * observer for (OnAdd, AUTO_OVERRIDE|*) during table
                     * creation. */
                    base_table->flags |= EcsTableOverrideDontFragment;
                }
            } else {
#ifdef FLECS_PREFAB
                ecs_table_record_t *tr = &base_table->_->records[i];
                if (ECS_ID_ON_INSTANTIATE(tr->hdr.cr->flags) == EcsOverride) {
                    to_add = id;
                }
#endif
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

static void flecs_add_with_property(
    ecs_world_t *world,
    ecs_component_record_t *cr_with_wildcard,
    ecs_type_t *dst_type,
    ecs_entity_t r,
    ecs_entity_t o)
{
    r = ecs_get_alive(world, r);

    /* Check if component/relationship has With pairs, which contain ids
     * that need to be added to the table. */
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
            ecs_id_t a = ecs_get_alive(world, ra);
            if (o) {
                a = ecs_pair(ra, o);
            }

            ecs_id_t check_id = ECS_IS_PAIR(a)
                ? ecs_pair(ECS_PAIR_FIRST(a), EcsWildcard)
                : a;
            ecs_component_record_t *a_cr = flecs_components_ensure(world, check_id);
            if (!(a_cr->flags & EcsIdDontFragment)) {
                flecs_type_add(world, dst_type, a);
            }
            flecs_add_with_property(world, cr_with_wildcard, dst_type, ra, o);
        }
    }
}

static ecs_table_t* flecs_find_table_with(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t with)
{
    ecs_make_alive_id(world, with);

    ecs_component_record_t *cr = NULL;
    ecs_entity_t r = 0, o = 0;
    ecs_type_t dst_type;
    bool replaced = false;

    if (ECS_IS_PAIR(with)) {
        r = ECS_PAIR_FIRST(with);
        o = ECS_PAIR_SECOND(with);
        cr = flecs_components_ensure(world, ecs_pair(r, EcsWildcard));
        if (cr->flags & EcsIdExclusive) {
            /* Relationship is exclusive, check if table already has it */
            const ecs_table_record_t *tr = flecs_component_get_table(cr, node);
            if (tr) {
                /* Table already has an instance of the relationship, create
                 * a new id sequence with the existing id replaced */
                dst_type = flecs_type_copy(world, &node->type);
                ecs_assert(dst_type.array != NULL, ECS_INTERNAL_ERROR, NULL);
                dst_type.array[tr->index] = with;
                replaced = true;
            }
        }
    } else {
        cr = flecs_components_ensure(world, with);
        r = with;
    }

    if (!replaced) {
        if (cr->flags & EcsIdDontFragment) {
            /* Component doesn't fragment tables */
            node->flags |= EcsTableHasDontFragment;
            return node;
        }

        /* Create sequence with new id */
        int res = flecs_type_new_with(world, &dst_type, &node->type, with);
        if (res == -1) {
            return node; /* Current table already has id */
        }
    }

    if (r == EcsIsA) {
        /* If adding a prefab, check if prefab has overrides */
        flecs_add_overrides_for_base(world, &dst_type, with);
#ifdef FLECS_PREFAB
    } else if (r == EcsChildOf) {
        o = ecs_get_alive(world, o);
        if (ecs_has_id(world, o, EcsPrefab)) {
            flecs_type_add(world, &dst_type, EcsPrefab);
        }
#endif
    }

    if (cr->flags & EcsIdWith) {
        ecs_component_record_t *cr_with_wildcard = flecs_components_get(world,
            ecs_pair(EcsWith, EcsWildcard));
        /* If id has With property, add targets to type */
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

static ecs_table_t* flecs_find_table_without(
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
                /* Component doesn't fragment tables */
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

    /* Create sequence with new id */
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

static ecs_table_t* flecs_table_create_edge(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edges_t *edges,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    bool remove)
{
    ecs_table_t *to = remove
        ? flecs_find_table_without(world, table, id)
        : flecs_find_table_with(world, table, id);
    edge->from = edges;
    edge->to = to;
    edge->id = id;

    if (id < FLECS_HI_COMPONENT_ID) {
        flecs_table_ensure_hi_edge(world, edges, id);
    }

    if (table != to || (table->flags & EcsTableHasDontFragment)) {
        ecs_graph_edge_t **incoming = &to->node.incoming[remove];
        edge->next = *incoming;
        edge->prev = incoming;
        if (edge->next) {
            edge->next->prev = &edge->next;
        }
        *incoming = edge;
        flecs_compute_table_diff(world, table, to, edge, id, remove);
    }
    return to;
}

ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);


    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = flecs_table_ensure_edge(world, &node->node.remove, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_table_create_edge(world, node, &node->node.remove, edge, id, true);
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

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);


    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_graph_edge_t *edge = flecs_table_ensure_edge(world, &node->node.add, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = flecs_table_create_edge(world, node, &node->node.add, edge, id, false);
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

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    ecs_type_t *type)
{
    flecs_poly_assert(world, ecs_world_t);
    return flecs_table_ensure(world, type, false, NULL);
}

void flecs_init_root_table(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);

    world->store.root.type = (ecs_type_t){0};
    world->store.root._ = flecs_calloc_t(&world->allocator, ecs_table__t);
    flecs_init_table(world, &world->store.root, NULL);

    /* Ensure table indices start at 1, as 0 is reserved for the root */
    uint64_t new_id = flecs_sparse_new_id(&world->store.tables);
    ecs_assert(new_id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)new_id;
}

void flecs_table_edges_add_flags(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_flags32_t flags)
{
    if ((flags == EcsTableHasOnAdd) || (flags == EcsTableHasUpNotify)) {
        for (ecs_graph_edge_t *edge = table->node.incoming[0];
            edge; edge = edge->next)
        {
            if ((id == EcsAny) || ecs_id_match(edge->id, id)) {
                if (!edge->diff) {
                    edge->diff = flecs_table_diff_new(world, 1, 0);
                    edge->diff->added.array[0] = edge->id;
                }
                edge->diff->added_flags |= flags;
            }
        }
    }

    /* Add flags to outgoing matching remove edges */
    if ((flags == EcsTableHasOnRemove) || (flags == EcsTableHasUpNotify)) {
        ecs_map_iter_t it = ecs_map_iter(table->node.remove.hi);
        while (ecs_map_next(&it)) {
            ecs_id_t edge_id = ecs_map_key(&it);
            if ((id == EcsAny) || ecs_id_match(edge_id, id)) {
                ecs_graph_edge_t *edge = ecs_map_ptr(&it);
                if (!edge->diff) {
                    edge->diff = flecs_table_diff_new(world, 0, 1);
                    edge->diff->removed.array[0] = edge->id;
                }
                edge->diff->removed_flags |= flags;
            }
        }
    }
}

static void flecs_table_clear_edge_set(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_graph_edge_t **incoming)
{
    ecs_map_iter_t it = ecs_map_iter(edges->hi);
    while (ecs_map_next(&it)) {
        flecs_table_disconnect_edge(world, ecs_map_key(&it), ecs_map_ptr(&it));
    }
    while (*incoming) {
        ecs_graph_edge_t *edge = *incoming;
        ecs_map_remove(edge->from->hi, edge->id);
        flecs_table_disconnect_edge(world, edge->id, edge);
    }
    flecs_bfree(&world->allocators.graph_edge_lo, edges->lo);
    ecs_map_fini(edges->hi);
    flecs_free_t(&world->allocator, ecs_map_t, edges->hi);
    ecs_os_zeromem(edges);
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_table_clear_edge_set(world, &table->node.add, &table->node.incoming[0]);
    flecs_table_clear_edge_set(world, &table->node.remove, &table->node.incoming[1]);
}

void flecs_table_clear_edges_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component)
{
    ecs_graph_edges_t *sets[2] = { &table->node.add, &table->node.remove };
    for (int32_t i = 0; i < 2; i ++) {
        ecs_graph_edges_t *edges = sets[i];
        ecs_graph_edge_t *edge = edges->hi
            ? ecs_map_get_ptr(edges->hi, component) : NULL;
        if (edge) {
            ecs_map_remove(edges->hi, component);
            flecs_table_disconnect_edge(world, component, edge);
        }
    }
}

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

/* Public convenience functions for traversing table graph */
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
