/**
 * @file each.c
 * @brief Simple iterator for a single component id.
 */

#include "private_api.h"

static bool flecs_each_dont_fragment_match(
    const ecs_component_record_t *cr,
    ecs_id_t id)
{
    if (!(cr->flags & EcsIdDontFragment)) {
        return false;
    }

    if (ecs_id_is_wildcard(cr->id)) {
        return false;
    }

    if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
        return ECS_IS_PAIR(cr->id);
    }

    return true;
}

static ecs_component_record_t* flecs_each_dont_fragment_next(
    ecs_component_record_t *cr,
    ecs_id_t id)
{
    do {
        if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
            cr = cr->non_fragmenting.next;
        } else if (ECS_PAIR_FIRST(id) == EcsWildcard) {
            cr = cr->pair->second.next;
        } else {
            cr = cr->pair->first.next;
        }
    } while (cr && !flecs_each_dont_fragment_match(cr, id));

    return cr;
}

static ecs_component_record_t* flecs_each_dont_fragment_first(
    const ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_id_t id)
{
    if (!ecs_id_is_wildcard(id)) {
        if (cr->flags & EcsIdDontFragment) {
            return cr;
        }

        return NULL;
    }

    if (!ECS_IS_PAIR(id)) {
        return NULL;
    }

    if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
        cr = world->cr_non_fragmenting_head;
        if (cr && !flecs_each_dont_fragment_match(cr, id)) {
            cr = flecs_each_dont_fragment_next(cr, id);
        }

        return cr;
    }

    if (ECS_PAIR_FIRST(id) == EcsWildcard) {
        if (!(cr->flags & EcsIdMatchDontFragment)) {
            return NULL;
        }
    } else if (!(cr->flags & EcsIdDontFragment)) {
        return NULL;
    }

    return flecs_each_dont_fragment_next(cr, id);
}

static bool flecs_each_component_record(
    const ecs_world_t *world,
    ecs_iter_t *it,
    ecs_component_record_t *cr,
    ecs_id_t id)
{
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(id == cr->id, ECS_INTERNAL_ERROR, NULL);

    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    each_iter->ids = id;
    each_iter->sizes = 0;
    if (cr->type_info) {
        each_iter->sizes = cr->type_info->size;
    }

    each_iter->sources = 0;
    each_iter->trs = NULL;
    each_iter->cr = flecs_each_dont_fragment_first(world, cr, id);
    flecs_table_cache_iter((ecs_table_cache_t*)cr, &each_iter->it, EcsTableNotEmpty);

    if (cr->flags & EcsIdSparse) {
        it->row_fields = 1;
        it->ref_fields = 1;
    }

    return true;
error:
    return false;
}

ecs_iter_t ecs_each_id(
    const ecs_world_t *stage,
    ecs_id_t id)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_check_exclusive_world_access_write(world);

    ecs_iter_t it = {
        .world = ECS_CONST_CAST(ecs_world_t*, world),
        .stage = ECS_CONST_CAST(ecs_world_t*, stage),
        .field_count = 1,
        .next = ecs_each_next
    };

    ecs_table_cache_iter_t *cache_it = &it.priv_.iter.each.it;
    cache_it->elems = NULL;
    cache_it->remaining = 0;
    it.priv_.iter.each.cr = NULL;

    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return it;
    }

    if (!flecs_each_component_record(world, &it, cr, id)) {
        return (ecs_iter_t){0};
    }

    return it;
error:
    return (ecs_iter_t){0};
}

static bool flecs_each_next_dont_fragment(
    ecs_iter_t *it)
{
    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    ecs_component_record_t *cr;

    while ((cr = each_iter->cr)) {
        each_iter->cr = NULL;
        if (ecs_id_is_wildcard(each_iter->ids)) {
            each_iter->cr = flecs_each_dont_fragment_next(cr, each_iter->ids);
        }

        ecs_sparse_t *sparse = cr->sparse;
        if (!sparse) {
            continue;
        }

        int32_t count = flecs_sparse_count(sparse);
        if (!count) {
            continue;
        }

        each_iter->trs = NULL;
        each_iter->columns = -1;
        each_iter->sizes = 0;
        if (cr->type_info) {
            each_iter->sizes = cr->type_info->size;
        }

        it->table = NULL;
        it->offset = 0;
        it->count = count;
        it->entities = flecs_sparse_ids(sparse);
        it->ids = &cr->id;
        it->trs = &each_iter->trs;
        it->columns = &each_iter->columns;
        it->sources = &each_iter->sources;
        it->sizes = &each_iter->sizes;
        it->set_fields = 1;
        it->row_fields = 1;
        it->ref_fields = 1;
        return true;
    }

    return false;
}

bool ecs_each_next(
    ecs_iter_t *it)
{
    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    const ecs_table_cache_elem_t *elem = flecs_table_cache_next(
        &each_iter->it);
    it->flags |= EcsIterIsValid;
    if (elem) {
        ecs_table_t *table = elem->table;
        each_iter->trs = elem->tr;
        if ((it->row_fields & 1) && (elem->index != -1)) {
            each_iter->trs = &table->_->records[elem->index];
        }

        each_iter->columns = elem->column;
        it->table = table;
        it->count = ecs_table_count(table);
        it->entities = ecs_table_entities(table);
        if (elem->index != -1) {
            it->ids = &table->type.array[elem->index];
        } else {
            it->ids = NULL;
        }

        it->trs = &each_iter->trs;
        it->columns = &each_iter->columns;
        it->sources = &each_iter->sources;
        it->sizes = &each_iter->sizes;
        it->set_fields = 1;
        return true;
    } else {
        return flecs_each_next_dont_fragment(it);
    }
}

static bool flecs_children_next_ordered(
    ecs_iter_t *it)
{
    return ecs_children_next(it);
}

ecs_iter_t ecs_children_w_rel(
    const ecs_world_t *stage,
    ecs_entity_t relationship,
    ecs_entity_t parent)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_check_exclusive_world_access_read(world);

    ecs_iter_t it = {
        .world = ECS_CONST_CAST(ecs_world_t*, world),
        .stage = ECS_CONST_CAST(ecs_world_t*, stage),
        .field_count = 1,
        .next = ecs_children_next
    };

    if (parent == EcsWildcard || parent == EcsAny) {
        return (ecs_iter_t){0};
    }

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_pair(relationship, parent));
    if (!cr) {
        return (ecs_iter_t){0};
    }

    if (cr->flags & EcsIdOrderedChildren) {
        ecs_vec_t *v = &cr->pair->ordered_children;
        it.entities = ecs_vec_first_t(v, ecs_entity_t);
        it.count = ecs_vec_count(v);
        it.next = flecs_children_next_ordered;
        return it;
    } else if (cr->flags & EcsIdSparse) {
        it.entities = flecs_sparse_ids(cr->sparse);
        it.count = flecs_sparse_count(cr->sparse);
        it.next = flecs_children_next_ordered;
        return it;
    }

    return ecs_each_id(stage, ecs_pair(relationship, parent));
error:
    return (ecs_iter_t){0};
}

ecs_iter_t ecs_children(
    const ecs_world_t *stage,
    ecs_entity_t parent)
{
    return ecs_children_w_rel(stage, EcsChildOf, parent);
}

bool ecs_children_next(
    ecs_iter_t *it)
{
    if (it->next == NULL) {
        return false;
    }

    if (it->next == flecs_children_next_ordered) {
        if (!it->count) {
            return false;
        }

        it->next = NULL; /* Only return once with ordered children vector */

        return true;
    }

    return ecs_each_next(it);
}

int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!id) {
        return 0;
    }

    int32_t count = 0;
    ecs_iter_t it = ecs_each_id(world, id);
    while (ecs_each_next(&it)) {
        if (it.table) {
            count += it.count * it.priv_.iter.each.it.cur->count;
        } else {
            count += it.count;
        }
    }

    return count;
error:
    return 0;
}
