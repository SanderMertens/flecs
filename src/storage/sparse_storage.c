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

                    if (flecs_sparse_has(cur->sparse, entity)) {
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

                    if (flecs_sparse_has(cur->sparse, entity)) {
                        return true;
                    }
                }
            }

            return false;
        }

        return false;
    } else {
        ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
        return flecs_sparse_has(cr->sparse, entity);
    }
}

void* flecs_component_sparse_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    ecs_entity_t entity)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_id_is_wildcard(cr->id)) {
        return flecs_sparse_get(cr->sparse, 0, entity);
    }

    /* Table should always be provided from context where wildcard is allowed */
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!(cr->flags & EcsIdDontFragment)) {
        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get the non-wildcard record */
        const ecs_table_record_t *ttr = &table->_->records[tr->index];
        cr = ttr->hdr.cr;
    } else {
        /* Find the target entity to replace the wildcard with */
        ecs_entity_t tgt = 0;
        if (cr->flags & EcsIdExclusive) {
            ecs_entity_t *tgt_ptr = 
                flecs_sparse_get_t(cr->sparse, ecs_entity_t, entity);
            if (!tgt_ptr) {
                return NULL;
            }

            tgt = *tgt_ptr;
        } else {
            ecs_type_t *type = flecs_sparse_get_t(
                cr->sparse, ecs_type_t, entity);
            if (!type) {
                return NULL;
            }

            tgt = type->array[0];
        }

        /* Find component record for the non-wildcard component */
        if (ECS_PAIR_FIRST(cr->id) == EcsWildcard) {
            cr = flecs_components_get(world, 
                ecs_pair(tgt, ECS_PAIR_SECOND(cr->id)));
        } else {
            /* Component record for (*, *) doesn't exist. */
            ecs_assert(ECS_PAIR_SECOND(cr->id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            cr = flecs_components_get(world, 
                ecs_pair(ECS_PAIR_FIRST(cr->id), tgt));
        }
    }

    return flecs_sparse_get(cr->sparse, 0, entity);
}

static
ecs_entity_t flecs_component_sparse_remove_intern(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    const ecs_type_info_t *ti = cr->type_info;

    if (!ti) {
        if (flecs_sparse_remove(cr->sparse, 0, entity)) {
            return entity;
        }
        return 0;
    }

    void *ptr = flecs_sparse_get(cr->sparse, 0, entity);
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

    flecs_sparse_remove(cr->sparse, 0, entity);

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
    if (!tgt) {
        /* It's possible that the target entity is cleaned up as part of the 
         * same entity that holds the relationship. If that's the case, the
         * relationship will get cleaned up later anyway so we can exit here. */
        ecs_assert(cr->flags & EcsIdMarkedForDelete, ECS_INTERNAL_ERROR, NULL);
        return;
    }

    ecs_type_t *type = flecs_sparse_get_t(
        parent->sparse, ecs_type_t, entity);
    if (!type) {
        return;
    }

    ecs_assert(type->count > 0, ECS_INTERNAL_ERROR, NULL);

    int32_t old_type_count = type->count;
    flecs_type_remove(world, type, tgt);
    ecs_assert(type->count != old_type_count, ECS_INTERNAL_ERROR, NULL);
    (void)old_type_count;

    if (!type->count) {
        flecs_sparse_remove(parent->sparse, 0, entity);
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

    flecs_sparse_remove(
        parent->sparse, ECS_SIZEOF(ecs_entity_t), entity);
}

void flecs_component_sparse_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_id_t id = cr->id;
    ecs_flags32_t flags = cr->flags;
    bool dont_fragment = flags & EcsIdDontFragment;

    /* If id is a wildcard, remove entity from all matching ids. */
    if (dont_fragment && ecs_id_is_wildcard(cr->id)) {
        /* A wildcard by itself can't be marked sparse so must be a pair. */
        ecs_assert(ECS_IS_PAIR(id), ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
            ECS_UNSUPPORTED,
            "remove(*, T) not supported for DontFragment targets");
        ecs_entity_t entity = ecs_table_entities(table)[row];
        ecs_component_record_t *cur = cr;
        while ((cur = flecs_component_first_next(cur))) {
            if (flecs_component_sparse_has(cur, entity)) {
                ecs_type_t type = {
                    .array = &cur->id,
                    .count = 1
                };

                flecs_emit(world, world, &(ecs_event_desc_t) {
                    .event = EcsOnRemove,
                    .ids = &type,
                    .table = table,
                    .other_table = NULL,
                    .offset = row,
                    .count = 1,
                    .observable = world
                });

                flecs_component_sparse_remove(world, cur, table, row);
            }
        }

        return;
    }

    ecs_entity_t entity = 
        flecs_component_sparse_remove_intern(world, cr, table, row);

    if (entity) {
        if (dont_fragment) {
            if (ECS_IS_PAIR(id)) {
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

static
void flecs_component_sparse_remove_all_id(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_id_t component_id = cr->id;

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
void flecs_component_sparse_remove_all_wildcard(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_id_t component_id = cr->id;
    
    if (!ECS_IS_PAIR(component_id)) {
        return;
    }

    if (cr->flags & EcsIdExclusive) {
        return;
    }

    ecs_sparse_t *sparse = cr->sparse;
    int32_t i, count = flecs_sparse_count(sparse);
    for (i = 0; i < count; i ++) {
        ecs_type_t *type = flecs_sparse_get_dense_t(sparse, ecs_type_t, i);
        flecs_type_free(world, type);
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
        flecs_component_sparse_remove_all_wildcard(world, cr);
    } else {
        flecs_component_sparse_remove_all_id(world, cr);
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

    ecs_type_t *type = flecs_sparse_ensure_t(
        parent->sparse, ecs_type_t, entity, NULL);
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
    ecs_entity_t tgt, *tgt_ptr = flecs_sparse_ensure_t(
        parent->sparse, ecs_entity_t, entity, NULL);
    ecs_assert(tgt_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(flecs_sparse_has(parent->sparse, entity), 
        ECS_INTERNAL_ERROR, NULL);

    if ((tgt = *tgt_ptr)) {
        ecs_component_record_t *other = flecs_components_get(world,
            ecs_pair(ECS_PAIR_FIRST(component_id), tgt));
        ecs_assert(other != NULL, ECS_INTERNAL_ERROR, NULL);
        if (other != cr) {
            ecs_type_t type = {
                .array = &other->id,
                .count = 1
            };

            flecs_emit(world, world, &(ecs_event_desc_t) {
                .event = EcsOnRemove,
                .ids = &type,
                .table = table,
                .other_table = NULL,
                .offset = row,
                .count = 1,
                .observable = world
            });

            flecs_component_sparse_remove_intern(world, other, table, row);
        }
    }

    ecs_assert(flecs_sparse_has(parent->sparse, entity), 
        ECS_INTERNAL_ERROR, NULL);

    *tgt_ptr = flecs_entities_get_alive(world, ECS_PAIR_SECOND(component_id));
}

static
void flecs_component_sparse_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component_id,
    void *ptr,
    const ecs_type_info_t *ti)
{
    const void *override_ptr = NULL;
    if (table->flags & EcsTableHasIsA) {
        ecs_entity_t base = 0;
        if (ecs_search_relation(world, table, 0, component_id, EcsIsA, EcsUp, 
            &base, NULL, NULL) != -1) 
        {
            override_ptr = ecs_get_id(world, base, component_id);
            ecs_assert(override_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        }
    }

    ecs_xtor_t ctor = ti->hooks.ctor;
    if (!override_ptr) {
        if (ctor) {
            ctor(ptr, 1, ti);
        }
    } else {
        ecs_copy_t copy = ti->hooks.copy_ctor;
        if (copy) {
            copy(ptr, override_ptr, 1, ti);
        } else {
            if (ctor) {
                ctor(ptr, 1, ti);
            }
            ecs_os_memcpy(ptr, override_ptr, ti->size);
        }
    }
}

void* flecs_component_sparse_insert(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];

    bool is_new = true;
    void *ptr = flecs_sparse_ensure(cr->sparse, 0, entity, &is_new);

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

    if (!ptr || !is_new) {
        return ptr;
    }

    const ecs_type_info_t *ti = cr->type_info;
    if (!ti) {
        return ptr;
    }

    flecs_component_sparse_override(world, table, component_id, ptr, ti);

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
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_ensure(cr->sparse, 0, entity, NULL);
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
