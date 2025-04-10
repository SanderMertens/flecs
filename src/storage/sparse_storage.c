#include "../private_api.h"

bool flecs_component_sparse_has(
    ecs_component_record_t *cr,
    ecs_entity_t entity)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t id = cr->id;
    if (ecs_id_is_wildcard(id)) {
        if (ECS_IS_PAIR(id)) {
            if ((ECS_PAIR_SECOND(id) == EcsWildcard) && 
                (cr->flags & EcsIdDontFragment)) 
            {
                ecs_component_record_t *cur = cr;
                while ((cur = flecs_component_first_next(cur))) {
                    if (!cur->sparse) {
                        continue;
                    }

                    if (flecs_sparse_has_any(cur->sparse, entity)) {
                        return true;
                    }
                }
            }

            if ((ECS_PAIR_FIRST(id) == EcsWildcard) && 
                (cr->flags & EcsIdMatchDontFragment)) 
            {
                ecs_component_record_t *cur = cr;
                while ((cur = flecs_component_second_next(cur))) {
                    if (!cur->sparse) {
                        continue;
                    }

                    if (flecs_sparse_has_any(cur->sparse, entity)) {
                        return true;
                    }
                }
            }

            return false;
        }

        return false;
    } else {
        ecs_assert(cr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
        return flecs_sparse_has_any(cr->sparse, entity);
    }
}

void* flecs_component_sparse_get(
    ecs_component_record_t *cr,
    ecs_entity_t entity)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_sparse_get_any(cr->sparse, 0, entity);
}

static
ecs_entity_t flecs_component_sparse_remove_intern(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    const ecs_type_info_t *ti = cr->type_info;

    if (!ti) {
        if (flecs_sparse_remove_fast(cr->sparse, 0, entity)) {
            return entity;
        }
        return 0;
    }

    void *ptr = flecs_sparse_get_any(cr->sparse, 0, entity);
    if (!ptr) {
        return 0;
    }

    ecs_iter_action_t on_remove = ti->hooks.on_remove;
    if (on_remove) {
        const ecs_table_record_t *tr = NULL;
        if (!(cr->flags & EcsIdDontFragment)) {
            tr = flecs_component_get_table(cr, table);
        }

        flecs_invoke_hook(world, table, cr, tr, 1, row, 
            &entity, cr->id, ti, EcsOnRemove, on_remove);
    }

    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        dtor(ptr, 1, ti);
    }

    flecs_sparse_remove_fast(cr->sparse, 0, entity);

    return entity;
}

static
void flecs_component_sparse_dont_fragment_pair_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!parent->sparse) {
        /* It's not the relationship that's non-fragmenting, but the target */
        return;
    }

    ecs_entity_t tgt = ecs_pair_second(world, cr->id);
    ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t *type = flecs_sparse_get_any_t(
        parent->sparse, ecs_type_t, entity);
    if (!type) {
        return;
    }

    ecs_assert(type->count > 0, ECS_INTERNAL_ERROR, NULL);

    flecs_type_remove(world, type, tgt);

    if (!type->count) {
        flecs_sparse_remove_fast(parent->sparse, 0, entity);
    }
}

static
void flecs_component_sparse_dont_fragment_exclusive_remove(
    ecs_component_record_t *cr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_sparse_remove_fast(
        parent->sparse, ECS_SIZEOF(ecs_entity_t), entity);
}

void flecs_component_sparse_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_entity_t entity = 
        flecs_component_sparse_remove_intern(world, cr, table, row);

    if (entity) {
        ecs_flags32_t flags = cr->flags;
        if (flags & EcsIdDontFragment) {
            if (ECS_IS_PAIR(cr->id)) {
                if (flags & EcsIdExclusive) {
                    flecs_component_sparse_dont_fragment_exclusive_remove(
                        cr, entity);
                } else {
                    flecs_component_sparse_dont_fragment_pair_remove(
                        world, cr, entity);
                }
            }
        }
    }
}

void flecs_component_sparse_remove_all(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_assert(cr->flags & EcsIdDontFragment, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t component_id = cr->id;
    if (ecs_id_is_wildcard(component_id)) {
        return;
    }

    ecs_sparse_t *sparse = cr->sparse;
    const ecs_entity_t *entities = flecs_sparse_ids(sparse);
    int32_t i, count = flecs_sparse_count(sparse);

    const ecs_type_info_t *ti = cr->type_info;
    if (ti) {
        ecs_iter_action_t on_remove = ti->hooks.on_remove;
        ecs_xtor_t dtor = ti->hooks.dtor;

        if (on_remove) {
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];
                ecs_record_t *r = flecs_entities_get(world, e);
                flecs_invoke_hook(world, r->table, cr, NULL, 1, 
                    ECS_RECORD_TO_ROW(r->row), &entities[i], component_id, ti, 
                    EcsOnRemove, on_remove);
            }
        }

        if (dtor) {
            for (i = 0; i < count; i ++) {
                void *ptr = flecs_sparse_get_dense(sparse, 0, i);
                dtor(ptr, 1, ti);
            }
        }
    }

    if (ECS_IS_PAIR(component_id)) {
        if (cr->flags & EcsIdExclusive) {
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];
                flecs_component_sparse_dont_fragment_exclusive_remove(cr, e);
            }
        } else {
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];
                flecs_component_sparse_dont_fragment_pair_remove(world, cr, e);
            }
        }
    }
}

static
void flecs_component_sparse_dont_fragment_pair_insert(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cr->pair->parent;
    
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!parent->sparse) {
        /* It's not the relationship that's non-fragmenting, but the target */
        return;
    }

    ecs_type_t *type = flecs_sparse_ensure_fast_t(
        parent->sparse, ecs_type_t, entity);
    flecs_type_add(world, type, ecs_pair_second(world, cr->id));
}

static
void flecs_component_sparse_dont_fragment_exclusive_insert(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t component_id = cr->id;
    ecs_entity_t tgt, *tgt_ptr = flecs_sparse_ensure_fast_t(
        parent->sparse, ecs_entity_t, entity);
    ecs_assert(tgt_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    if ((tgt = *tgt_ptr)) {
        ecs_component_record_t *other = flecs_components_get(world,
            ecs_pair(ECS_PAIR_FIRST(component_id), tgt));
        ecs_assert(other != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_component_sparse_remove(world, other, table, row);
    }

    *tgt_ptr = ECS_PAIR_SECOND(component_id);
}

void* flecs_component_sparse_insert(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cr->sparse, 0, entity);

    ecs_id_t component_id = cr->id;
    if (ECS_IS_PAIR(component_id)) {
        ecs_flags32_t flags = cr->flags;
        if (flags & EcsIdDontFragment) {
            if (flags & EcsIdExclusive) {
                flecs_component_sparse_dont_fragment_exclusive_insert(
                    world, cr, table, row, entity);
            } else {
                flecs_component_sparse_dont_fragment_pair_insert(
                    world, cr, entity);
            }
        }
    }

    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cr->type_info;
    if (!ti) {
        return ptr;
    }

    ecs_xtor_t ctor = ti->hooks.ctor;
    if (ctor) {
        ctor(ptr, 1, ti);
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (!on_add) {
        return ptr;
    }

    const ecs_table_record_t *tr = NULL;
    if (!(cr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cr, table);
    }

    flecs_invoke_hook(world, table, cr, tr, 1, row, 
        &entity, component_id, ti, EcsOnAdd, on_add);

    return ptr;
}

void* flecs_component_sparse_emplace(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cr->sparse, 0, entity);
    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cr->type_info;
    if (!ti) {
        return ptr;
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (!on_add) {
        return ptr;
    }

    const ecs_table_record_t *tr = NULL;
    if (!(cr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cr, table);
    }

    flecs_invoke_hook(world, table, cr, tr, 1, row, 
        &entity, cr->id, ti, EcsOnAdd, on_add);

    return ptr;
}
