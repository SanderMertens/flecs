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

    if (add->count == 1) {
        ecs_vec_append_t(&world->allocator, vec, ecs_id_t)[0] = add->array[0];
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

static ecs_graph_edges_t* flecs_graph_node_edges(
    ecs_table_t *table,
    bool remove)
{
    return remove ? &table->node.remove : &table->node.add;
}

FLECS_FLATMAP_IMPL(flecs_graph_edges, ecs_graph_edges_t, ecs_graph_edge_t, id)

static void flecs_graph_incoming_add(
    ecs_world_t *world,
    ecs_graph_edge_t *edge,
    ecs_table_t *from,
    bool remove)
{
    ecs_vec_t *incoming = &edge->to->_->incoming[remove];
    ecs_vec_init_if_t(incoming, ecs_graph_incoming_t);
    edge->incoming = ecs_vec_count(incoming);
    ecs_vec_append_t(&world->allocator, incoming, ecs_graph_incoming_t)[0] =
        (ecs_graph_incoming_t){ .from = from, .id = edge->id };
}

static void flecs_graph_incoming_remove(
    ecs_table_t *to,
    int32_t index,
    bool remove)
{
    ecs_vec_t *incoming = &to->_->incoming[remove];
    ecs_graph_incoming_t *array = ecs_vec_first_t(
        incoming, ecs_graph_incoming_t);
    int32_t last = ecs_vec_count(incoming) - 1;
    ecs_assert(index <= last, ECS_INTERNAL_ERROR, NULL);

    if (index != last) {
        ecs_graph_incoming_t *moved = &array[index];
        *moved = array[last];
        ecs_graph_edge_t *edge = flecs_graph_edges_get(
            flecs_graph_node_edges(moved->from, remove), moved->id);
        ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->incoming == last, ECS_INTERNAL_ERROR, NULL);
        edge->incoming = index;
    }

    ecs_vec_remove_last(incoming);
}

static void flecs_graph_edge_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    bool remove)
{
    ecs_graph_edges_t *edges = flecs_graph_node_edges(table, remove);
    ecs_graph_edge_t *edge = flecs_graph_edges_get(edges, id);
    if (!edge) {
        return;
    }

    if (edge->incoming != -1) {
        flecs_graph_incoming_remove(edge->to, edge->incoming, remove);
    }

    if (edge->diff) {
        flecs_table_diff_free(world, edge->diff);
    }

    flecs_graph_edges_remove_elem(edges, edge);
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

static void flecs_add_with_property(
    ecs_world_t *world,
    ecs_component_record_t *cr_with_wildcard,
    ecs_type_t *dst_type,
    ecs_type_t *dont_fragment_type,
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
                if (dst_type) {
                    flecs_type_add(world, dst_type, a);
                }
            } else if (dont_fragment_type) {
                flecs_type_add(world, dont_fragment_type, a);
            }

            flecs_add_with_property(world, cr_with_wildcard, dst_type,
                dont_fragment_type, ra, o);
        }
    }
}

static ecs_flags32_t flecs_table_graph_id_flags(
    const ecs_component_record_t *cr,
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id) && cr->pair->parent) {
        return cr->pair->parent->flags;
    }

    return cr->flags;
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
    ecs_type_t dont_fragment = {0};
    if (!is_remove && (cr->flags & EcsIdWith)) {
        ecs_entity_t r = id, o = 0;
        if (ECS_IS_PAIR(id)) {
            r = ECS_PAIR_FIRST(id);
            o = ECS_PAIR_SECOND(id);
        }

        flecs_add_with_property(world, flecs_components_get(world,
            ecs_pair(EcsWith, EcsWildcard)), NULL, &dont_fragment, r, o);
    }

    if (flecs_table_graph_id_flags(cr, id) & EcsIdDontFragment) {
        ecs_assert(!is_remove && (cr->flags & EcsIdWith),
            ECS_INTERNAL_ERROR, NULL);
        ecs_vec_append_t(&world->allocator,
            &cr->dont_fragment_tables, uint64_t)[0] = node->id;
        flecs_type_add(world, &dont_fragment, id);
    }

    ecs_type_t added = {0}, removed = {0};
    ecs_flags32_t added_flags = flecs_table_diff_ids(
        world, &node->type, &next->type, &added) & EcsTableAddEdgeFlags;
    ecs_flags32_t removed_flags = flecs_table_diff_ids(
        world, &next->type, &node->type, &removed) & EcsTableRemoveEdgeFlags;

    if (ECS_HAS_RELATION(id, EcsChildOf)) {
        if (added.count) {
            added_flags |= EcsTableEdgeReparent;
        } else if (removed.count) {
            removed_flags |= EcsTableEdgeReparent;
        }
    } else if ((next->flags & EcsTableHasParent) &&
        (node->flags & EcsTableHasChildOf) &&
        !(next->flags & EcsTableHasChildOf))
    {
        removed_flags |= EcsTableEdgeReparent;
    }

    bool only_id = is_remove
        ? (!added.count && removed.count == 1 && !ecs_id_is_wildcard(id))
        : (!removed.count && added.count == 1);
    if (only_id && !dont_fragment.count) {
        edge->flags = is_remove ? removed_flags : added_flags;
        return;
    }

    ecs_table_diff_t *diff = flecs_table_diff_new(
        world, added.count + dont_fragment.count, removed.count);
    flecs_table_diff_ids(world, &node->type, &next->type, &diff->added);
    flecs_table_diff_ids(world, &next->type, &node->type, &diff->removed);
    if (dont_fragment.count) {
        ecs_os_memcpy_n(&diff->added.array[diff->added.count],
            dont_fragment.array, ecs_id_t, dont_fragment.count);
        diff->added.count += dont_fragment.count;
        added_flags |= EcsTableEdgeDontFragment|EcsTableHasSparse;
        flecs_type_free(world, &dont_fragment);
    }

    diff->added_flags = added_flags;
    diff->removed_flags = removed_flags;
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
            ecs_assert(cr->flags & EcsIdWith, ECS_INTERNAL_ERROR, NULL);
            dst_type = flecs_type_copy(world, &node->type);
        } else {
            /* Create sequence with new id */
            int res = flecs_type_new_with(world, &dst_type, &node->type, with);
            if (res == -1) {
                return node; /* Current table already has id */
            }
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
        flecs_add_with_property(
            world, cr_with_wildcard, &dst_type, NULL, r, o);
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

static ecs_graph_edge_t* flecs_table_create_edge(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    bool remove)
{
    ecs_table_t *to = remove
        ? flecs_find_table_without(world, table, id)
        : flecs_find_table_with(world, table, id);

    ecs_graph_edge_t result = { .id = id, .to = to, .incoming = -1 };
    if (table != to || (!remove && (flecs_table_graph_id_flags(
        flecs_components_ensure(world, id), id) & EcsIdDontFragment)))
    {
        flecs_compute_table_diff(world, table, to, &result, id, remove);
    }

    ecs_graph_edge_t *edge = flecs_graph_edges_ensure(
        &world->allocator, flecs_graph_node_edges(table, remove), id);
    if (edge->to) {
        if (result.diff) {
            flecs_table_diff_free(world, result.diff);
        }

        return edge;
    }

    *edge = result;

    if (table != to) {
        flecs_graph_incoming_add(world, edge, table, remove);
    }

    return edge;
}

static bool flecs_table_traverse_dont_fragment(
    ecs_world_t *world,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff,
    bool remove)
{
    ecs_id_t id = id_ptr[0];
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        if (remove) {
            *diff = (ecs_table_diff_t)ECS_TABLE_DIFF_INIT;
            return true;
        }

        ecs_make_alive_id(world, id);
        cr = flecs_components_ensure(world, id);
    }

    ecs_flags32_t flags = flecs_table_graph_id_flags(cr, id);
    if (!(flags & EcsIdDontFragment)) {
        return false;
    }

    if (remove) {
        *diff = (ecs_table_diff_t){
            .removed = { .array = id_ptr, .count = 1 },
            .removed_flags = EcsTableEdgeDontFragment|EcsTableHasSparse
        };
    } else {
        if (flags & EcsIdWith) {
            return false;
        }

        *diff = (ecs_table_diff_t){
            .added = { .array = id_ptr, .count = 1 },
            .added_flags = EcsTableEdgeDontFragment|EcsTableHasSparse
        };
    }

    return true;
}

static ecs_table_t* flecs_table_edge_traverse(
    const ecs_graph_edge_t *edge,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff,
    bool remove)
{
    ecs_table_t *to = edge->to;
    if (edge->diff) {
        *diff = *edge->diff;
    } else if (node != to) {
        if (remove) {
            diff->added.count = 0;
            diff->removed.array = id_ptr;
            diff->removed.count = 1;
            diff->removed_flags = edge->flags;
        } else {
            diff->added.array = id_ptr;
            diff->added.count = 1;
            diff->removed.count = 0;
            diff->added_flags = edge->flags;
        }
    }

    return to;
}

static FLECS_NOINLINE ecs_table_t* flecs_table_traverse_new_edge(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff,
    bool remove)
{
    if (flecs_table_traverse_dont_fragment(world, id_ptr, diff, remove)) {
        return node;
    }

    ecs_graph_edge_t *edge = flecs_table_create_edge(
        world, node, id_ptr[0], remove);
    ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_table_edge_traverse(edge, node, id_ptr, diff, remove);
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

    ecs_graph_edge_t *edge = flecs_graph_edges_get(
        &node->node.remove, id_ptr[0]);
    if (!edge) {
        return flecs_table_traverse_new_edge(world, node, id_ptr, diff, true);
    }

    return flecs_table_edge_traverse(edge, node, id_ptr, diff, true);
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

    ecs_graph_edge_t *edge = flecs_graph_edges_get(
        &node->node.add, id_ptr[0]);
    if (!edge) {
        return flecs_table_traverse_new_edge(world, node, id_ptr, diff, false);
    }

    return flecs_table_edge_traverse(edge, node, id_ptr, diff, false);
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

static bool flecs_type_has_match(
    const ecs_type_t *type,
    ecs_id_t id)
{
    int32_t i;
    for (i = 0; i < type->count; i ++) {
        if (ecs_id_match(type->array[i], id)) {
            return true;
        }
    }

    return false;
}

static void flecs_graph_edge_add_flags(
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_flags32_t flags,
    bool remove)
{
    ecs_table_diff_t *diff = edge->diff;
    if (!diff) {
        if ((id == EcsAny) || ecs_id_match(edge->id, id)) {
            edge->flags |= flags;
        }
    } else if (remove) {
        if ((id == EcsAny) || flecs_type_has_match(&diff->removed, id)) {
            diff->removed_flags |= flags;
        }
    } else {
        if ((id == EcsAny) || flecs_type_has_match(&diff->added, id)) {
            diff->added_flags |= flags;
        }
    }
}

void flecs_table_edges_add_flags(
    ecs_table_t *table,
    ecs_id_t id,
    ecs_flags32_t flags)
{
    if ((flags == EcsTableHasOnAdd) || (flags == EcsTableHasUpNotify)) {
        const ecs_vec_t *incoming = &table->_->incoming[0];
        const ecs_graph_incoming_t *array = ecs_vec_first_t(
            incoming, ecs_graph_incoming_t);
        int32_t i, count = ecs_vec_count(incoming);
        for (i = 0; i < count; i ++) {
            ecs_graph_edge_t *edge = flecs_graph_edges_get(
                &array[i].from->node.add, array[i].id);
            ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
            flecs_graph_edge_add_flags(edge, id, flags, false);
        }
    }

    if ((flags == EcsTableHasOnRemove) || (flags == EcsTableHasUpNotify)) {
        ecs_graph_edges_t *edges = &table->node.remove;
        int32_t i, size = flecs_flatmap_size(edges);
        for (i = 0; i < size; i ++) {
            ecs_graph_edge_t *edge = &edges->data[i];
            if (edge->id) {
                flecs_graph_edge_add_flags(edge, id, flags, true);
            }
        }
    }
}

static void flecs_table_clear_edge_set(
    ecs_world_t *world,
    ecs_table_t *table,
    bool remove)
{
    ecs_graph_edges_t *edges = flecs_graph_node_edges(table, remove);
    if (edges->data) {
        int32_t i, size = flecs_flatmap_size(edges);
        for (i = 0; i < size; i ++) {
            ecs_graph_edge_t *edge = &edges->data[i];
            if (!edge->id) {
                continue;
            }

            if (edge->incoming != -1) {
                flecs_graph_incoming_remove(edge->to, edge->incoming, remove);
            }

            if (edge->diff) {
                flecs_table_diff_free(world, edge->diff);
            }
        }

        flecs_graph_edges_fini(&world->allocator, edges);
    }

    ecs_vec_t *incoming = &table->_->incoming[remove];
    ecs_graph_incoming_t *array = ecs_vec_first_t(
        incoming, ecs_graph_incoming_t);
    int32_t i, count = ecs_vec_count(incoming);
    for (i = 0; i < count; i ++) {
        ecs_graph_edges_t *from = flecs_graph_node_edges(
            array[i].from, remove);
        ecs_graph_edge_t *edge = flecs_graph_edges_get(from, array[i].id);
        ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
        if (edge->diff) {
            flecs_table_diff_free(world, edge->diff);
        }

        flecs_graph_edges_remove_elem(from, edge);
    }

    ecs_vec_fini_t(&world->allocator, incoming, ecs_graph_incoming_t);
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_table_clear_edge_set(world, table, false);
    flecs_table_clear_edge_set(world, table, true);
}

void flecs_table_clear_edges_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component)
{
    flecs_graph_edge_remove(world, table, component, false);
    flecs_graph_edge_remove(world, table, component, true);
}

void flecs_table_clear_edges_for_base(
    ecs_world_t *world,
    ecs_entity_t base)
{
    ecs_id_t id = ecs_pair(EcsIsA, base);
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return;
    }

    ecs_table_cache_iter_t it;
    if (!flecs_table_cache_iter(
        &cr->cache, &it, EcsTableEmpty|EcsTableNotEmpty))
    {
        return;
    }

    const ecs_table_cache_elem_t *elem;
    while ((elem = flecs_table_cache_next(&it))) {
        ecs_table_t *table = elem->tr->hdr.table;

        ecs_vec_t *incoming = &table->_->incoming[0];
        int32_t i, count;
        for (i = ecs_vec_count(incoming) - 1; i >= 0; i --) {
            ecs_graph_incoming_t *in = ecs_vec_get_t(
                incoming, ecs_graph_incoming_t, i);
            if (in->id == id) {
                flecs_graph_edge_remove(world, in->from, id, false);
            }
        }

        if (!table->_->traversable_count) {
            continue;
        }

        const ecs_entity_t *entities = ecs_table_entities(table);
        count = ecs_table_count(table);
        for (i = 0; i < count; i ++) {
            ecs_record_t *r = flecs_entities_get(world, entities[i]);
            if (r->row & EcsEntityIsBase) {
                flecs_table_clear_edges_for_base(world, entities[i]);
            }
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
