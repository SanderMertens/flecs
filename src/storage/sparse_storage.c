#include "../private_api.h"

bool flecs_component_sparse_has(
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t id = cdr->id;
    if (ecs_id_is_wildcard(id)) {
        if (ECS_IS_PAIR(id)) {
            if ((ECS_PAIR_SECOND(id) == EcsWildcard) && 
                (cdr->flags & EcsIdDontFragment)) 
            {
                ecs_component_record_t *cur = cdr;
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
                (cdr->flags & EcsIdMatchDontFragment)) 
            {
                ecs_component_record_t *cur = cdr;
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
        ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
        return flecs_sparse_has_any(cdr->sparse, entity);
    }
}

void* flecs_component_sparse_get(
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_sparse_get_any(cdr->sparse, 0, entity);
}

static
ecs_entity_t flecs_component_sparse_remove_intern(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    const ecs_type_info_t *ti = cdr->type_info;

    if (!ti) {
        if (flecs_sparse_remove_fast(cdr->sparse, 0, entity)) {
            return entity;
        }
        return 0;
    }

    void *ptr = flecs_sparse_get_any(cdr->sparse, 0, entity);
    if (!ptr) {
        return 0;
    }

    ecs_iter_action_t on_remove = ti->hooks.on_remove;
    if (on_remove) {
        const ecs_table_record_t *tr = NULL;
        if (!(cdr->flags & EcsIdDontFragment)) {
            tr = flecs_component_get_table(cdr, table);
        }

        flecs_invoke_hook(world, table, cdr, tr, 1, row, 
            entity, cdr->id, ti, EcsOnRemove, on_remove);
    }

    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        dtor(ptr, 1, ti);
    }

    flecs_sparse_remove_fast(cdr->sparse, 0, entity);

    return entity;
}

static
void flecs_component_sparse_dont_fragment_pair_remove(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cdr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!parent->sparse) {
        /* It's not the relationship that's non-fragmenting, but the target */
        return;
    }

    ecs_entity_t tgt = ecs_pair_second(world, cdr->id);
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
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cdr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_sparse_remove_fast(
        parent->sparse, ECS_SIZEOF(ecs_entity_t), entity);
}

void flecs_component_sparse_remove(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_entity_t entity = 
        flecs_component_sparse_remove_intern(world, cdr, table, row);

    if (entity) {
        ecs_flags32_t flags = cdr->flags;
        if (flags & EcsIdDontFragment) {
            if (ECS_IS_PAIR(cdr->id)) {
                if (flags & EcsIdExclusive) {
                    flecs_component_sparse_dont_fragment_exclusive_remove(
                        cdr, entity);
                } else {
                    flecs_component_sparse_dont_fragment_pair_remove(
                        world, cdr, entity);
                }
            }
        }
    }
}

static
void flecs_component_sparse_dont_fragment_pair_insert(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cdr->pair->parent;
    
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!parent->sparse) {
        /* It's not the relationship that's non-fragmenting, but the target */
        return;
    }

    ecs_type_t *type = flecs_sparse_ensure_fast_t(
        parent->sparse, ecs_type_t, entity);
    flecs_type_add(world, type, ecs_pair_second(world, cdr->id));
}

static
void flecs_component_sparse_dont_fragment_exclusive_insert(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row,
    ecs_entity_t entity)
{
    ecs_component_record_t *parent = cdr->pair->parent;
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t component_id = cdr->id;
    ecs_entity_t tgt, *tgt_ptr = flecs_sparse_ensure_fast_t(
        parent->sparse, ecs_entity_t, entity);
    if (tgt_ptr && (tgt = *tgt_ptr)) {
        ecs_component_record_t *other = flecs_components_get(world,
            ecs_pair(ECS_PAIR_FIRST(component_id), tgt));
        ecs_assert(other != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_component_sparse_remove(world, other, table, row);
    }

    *tgt_ptr = ECS_PAIR_SECOND(component_id);
}

void* flecs_component_sparse_insert(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cdr->sparse, 0, entity);

    ecs_id_t component_id = cdr->id;
    if (ECS_IS_PAIR(component_id)) {
        ecs_flags32_t flags = cdr->flags;
        if (flags & EcsIdDontFragment) {
            if (flags & EcsIdExclusive) {
                flecs_component_sparse_dont_fragment_exclusive_insert(
                    world, cdr, table, row, entity);
            } else {
                flecs_component_sparse_dont_fragment_pair_insert(
                    world, cdr, entity);
            }
        }
    }

    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cdr->type_info;
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
    if (!(cdr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cdr, table);
    }

    flecs_invoke_hook(world, table, cdr, tr, 1, row, 
        entity, component_id, ti, EcsOnAdd, on_add);

    return ptr;
}

void* flecs_component_sparse_emplace(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cdr->sparse, 0, entity);
    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cdr->type_info;
    if (!ti) {
        return ptr;
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (!on_add) {
        return ptr;
    }

    const ecs_table_record_t *tr = NULL;
    if (!(cdr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cdr, table);
    }

    flecs_invoke_hook(world, table, cdr, tr, 1, row, 
        entity, cdr->id, ti, EcsOnAdd, on_add);

    return ptr;
}
