/**
 * @file spawner.c
 * @brief TODO.
 */

#include "private_api.h"

void flecs_type_set_depth(
    ecs_type_t *type,
    int32_t depth)
{
    int32_t i, count = type->count;
    for (i = 0; i < count; i ++) {
        if (ECS_PAIR_FIRST(type->array[i]) == EcsParentDepth) {
            type->array[i] = ecs_value_pair(EcsParentDepth, depth);
            return;
        }
    }

    ecs_assert(false, ECS_INTERNAL_ERROR, "missing ParentDepth pair in type");
}

void flecs_build_prefab_spawner_from_cr(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_vec_t *spawner,
    int32_t parent_index,
    int32_t depth)
{
    ecs_allocator_t *a = &world->allocator;

    ecs_vec_t *children_vec = &cr->pair->ordered_children;
    int32_t i, count = ecs_vec_count(children_vec);
    ecs_entity_t *children = ecs_vec_first(children_vec);
    for (i = 0; i < count; i ++) {
        ecs_entity_t child = children[i];
        ecs_record_t *r = flecs_entities_get(world, child);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_table_t *table = r->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!(table->flags & EcsTableHasParent)) {
            continue;
        }

        ecs_tree_spawner_child_t *elem = ecs_vec_append_t(
            a, spawner, ecs_tree_spawner_child_t);
        elem->parent_index = parent_index;

        ecs_type_t type = flecs_type_copy(world, &table->type);
        flecs_type_remove(world, &type, EcsPrefab);
        flecs_type_remove(world, &type, ecs_pair_t(EcsIdentifier, EcsName));
        flecs_type_add(world, &type, ecs_isa(child));
        flecs_type_set_depth(&type, depth);

        elem->table = flecs_table_find_or_create(world, &type);
        ecs_assert(elem->table != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_type_free(world, &type);

        // printf("%d: %s\n", parent_index, ecs_table_str(world, elem->table));

        if (!(r->row & EcsEntityIsTraversable)) {
            continue;
        }

        ecs_component_record_t *child_cr = flecs_components_get(
            world, ecs_childof(child));
        if (!child_cr) {
            continue;
        }

        flecs_build_prefab_spawner_from_cr(
            world, child_cr, spawner, ecs_vec_count(spawner), depth + 1);
    }
}

const EcsTreeSpawner* flecs_build_prefab_spawner(
    ecs_world_t *world,
    ecs_entity_t base)
{    
    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(base));
    if (!cr) {
        return NULL;
    }

    ecs_vec_t spawner;
    ecs_vec_init_t(&world->allocator, &spawner, 
        ecs_tree_spawner_child_t, 0);

    flecs_build_prefab_spawner_from_cr(world, cr, &spawner, 0, 1);

    EcsTreeSpawner *ts = ecs_ensure(world, base, EcsTreeSpawner);
    ts->depth_offset = 0;
    ts->data[0].children = spawner;
    return ts;
}

void flecs_spawner_instantiate(
    ecs_world_t *world,
    const EcsTreeSpawner *spawner,
    ecs_entity_t instance)
{
    const ecs_vec_t *vec = &spawner->data[0].children;
    ecs_tree_spawner_child_t *spawn_children = ecs_vec_first(vec);
    int32_t i, child_count = ecs_vec_count(vec);

    ecs_vec_set_min_count_t(&world->allocator, &world->allocators.tree_spawner, 
        ecs_entity_t, child_count + 1);
    ecs_entity_t *parents = ecs_vec_first(&world->allocators.tree_spawner);
    parents[0] = instance;

    ecs_component_record_t *cr = NULL;
    ecs_entity_t old_parent = 0;

    for (i = 0; i < child_count; i ++) {
        ecs_entity_t entity = parents[i + 1] = flecs_new_id(world);
        ecs_tree_spawner_child_t *spawn_child = &spawn_children[i];
        ecs_table_t *table = spawn_child->table;

        ecs_record_t *r = flecs_entities_get(world, entity);
        ecs_flags32_t flags = table->flags & EcsTableAddEdgeFlags;

        ecs_table_diff_t table_diff = { 
            .added = table->type,
            .added_flags = flags
        };

        int32_t row = flecs_table_append(world, table, entity, true, true);
        r->table = table;
        r->row = row;

        flecs_actions_new(world, table, row, 1, &table_diff, 0, false, true);

        ecs_entity_t parent = parents[spawn_child->parent_index];
        if (parent != old_parent) {
            cr = flecs_components_ensure(world, ecs_childof(parent));
            old_parent = parent;
        }

        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

        int32_t parent_column = table->component_map[ecs_id(EcsParent)];
        EcsParent *parent_ptr = table->data.columns[parent_column - 1].data;
        parent_ptr = &parent_ptr[row];
        parent_ptr->value = parent;

        flecs_add_non_fragmenting_child_w_records(world, parent, entity, cr, r);
    }
}
