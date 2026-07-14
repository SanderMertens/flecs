/**
 * @file addons/prefab/prefab.c
 * @brief Prefab addon.
 */

#include "../../private_api.h"

#ifdef FLECS_PREFAB

static
void flecs_tree_spawner_release_tables(
    ecs_vec_t *v)
{
    int32_t i, count = ecs_vec_count(v);
    ecs_tree_spawner_child_t *elems = ecs_vec_first(v);
    for (i = 0; i < count; i ++) {
        ecs_tree_spawner_child_t *elem = &elems[i];
        flecs_table_release(elem->table);
    }
}

void EcsTreeSpawner_free(EcsTreeSpawner *ptr) {
    int32_t i;
    for (i = 0; i < FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE; i ++) {
        flecs_tree_spawner_release_tables(&ptr->data[i].children);
        ecs_vec_fini_t(NULL, &ptr->data[i].children, ecs_tree_spawner_child_t);
    }
}

static ECS_COPY(EcsTreeSpawner, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "TreeSpawner component cannot be copied");
})

static ECS_MOVE(EcsTreeSpawner, dst, src, {
    EcsTreeSpawner_free(dst);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsTreeSpawner, ptr, {
    EcsTreeSpawner_free(ptr);
})

static
void flecs_on_add_prefab(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    for (int32_t i = 0; i < it->count; i ++) {
        ecs_entity_t p = it->entities[i];

        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(p));
        if (cr && (cr->flags & EcsIdOrderedChildren)) {
            flecs_ordered_children_set_prefab(world, cr);
        }

        ecs_iter_t cit = ecs_children(world, p);
        while (ecs_children_next(&cit)) {
            for (int32_t j = 0; j < cit.count; j ++) {
                ecs_add_id(world, cit.entities[j], EcsPrefab);
            }
        }
    }
}

ecs_entity_t flecs_get_prefab_instance_child(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t prefab_child)
{
    ecs_map_val_t *index_ptr = ecs_map_get(
        &world->prefab_child_indices, prefab_child);
    if (!index_ptr) {
        return 0;
    }

    flecs_assert_entity_valid(world, prefab_child, "get_target");
    ecs_check(ecs_owns_id(world, prefab_child, EcsPrefab), 
        ECS_INVALID_OPERATION, 
        "cannot get target for '%s': entity is not/no longer a prefab",
        flecs_errstr(ecs_id_str(world, prefab_child)));

#ifdef FLECS_DEBUG
    ecs_entity_t prefab = ecs_get_parent(world, prefab_child);
    ecs_check(prefab != 0, ECS_INVALID_OPERATION,
        "cannot get target for '%s': entity has no parent",
        flecs_errstr(ecs_id_str(world, prefab_child)));

    ecs_check(ecs_owns_id(world, prefab, EcsPrefab), ECS_INVALID_OPERATION,
        "cannot get target for '%s': parent is not/no longer a prefab",
        flecs_errstr(ecs_id_str(world, prefab)),
        flecs_errstr_1(ecs_id_str(world, prefab_child)));

    ecs_check(ecs_has_pair(world, entity, EcsIsA, prefab),
        ECS_INVALID_OPERATION,
        "cannot get target for '%s': entity '%s' is not an instance of prefab '%s'",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)),
            flecs_errstr_2(ecs_id_str(world, prefab)));
#endif

    ecs_component_record_t *childof_cr = flecs_components_get(
        world, ecs_childof(entity));
    ecs_check(childof_cr != NULL, ECS_INVALID_OPERATION, 
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    ecs_vec_t *v = &childof_cr->pair->ordered_children;
    int32_t index = flecs_uto(int32_t, *index_ptr);
    ecs_check(ecs_vec_count(v) > index, ECS_INVALID_OPERATION,
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    ecs_entity_t tgt = ecs_vec_get_t(v, ecs_entity_t, index)[0];
    ecs_check(ecs_has_pair(world, tgt, EcsIsA, prefab_child), ECS_INVALID_OPERATION,
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    return tgt;
error:
    return 0;
}

void flecs_bootstrap_prefab(
    ecs_world_t *world)
{
    flecs_bootstrap_tag(world, EcsPrefab);

    ecs_add_pair(world, EcsPrefab, EcsOnInstantiate, EcsDontInherit);

    flecs_type_info_init(world, EcsTreeSpawner, {
        .ctor = flecs_default_ctor,
        .copy = ecs_copy(EcsTreeSpawner),
        .move = ecs_move(EcsTreeSpawner),
        .dtor = ecs_dtor(EcsTreeSpawner)
    });

    ecs_add_pair(world, ecs_id(EcsTreeSpawner), 
        EcsOnInstantiate, EcsDontInherit);

    /* Observer that ensures children of a prefab are also prefabs */
    ecs_observer(world, {
        .query.terms = {
            { .id = EcsPrefab },
        },
        .events = {EcsOnAdd},
        .callback = flecs_on_add_prefab,
        .global_observer = true
    });
}

#else

void flecs_bootstrap_prefab(
    ecs_world_t *world)
{
#ifdef FLECS_REST
    /* If the REST addon is enabled, just create an entity called Prefab. 
     * Without this the explorer tree query (which checks if an entity is a
     * Prefab) will fail to create. */
    ecs_entity(world, { .name = "flecs.core.Prefab" });
#endif
}

void flecs_fini_prefab(
    ecs_world_t *world)
{
    (void)world;
}

ecs_entity_t flecs_get_prefab_instance_child(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t prefab_child)
{
    (void)world;
    (void)entity;
    (void)prefab_child;
    return 0;
}

#endif
