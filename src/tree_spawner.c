/**
 * @file tree_spawner.c
 * @brief Data structure used to speed up the creation of hierarchies.
 */

#include "private_api.h"

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

static
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
ecs_type_t flecs_prefab_spawner_build_type(
    ecs_world_t *world,
    ecs_entity_t child,
    ecs_table_t *table,
    int32_t depth)
{
    ecs_type_t dst = {0};
    ecs_type_t *src = &table->type;

    int32_t i, count = src->count;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = src->array[i];
        ecs_table_record_t *tr = &table->_->records[i];
        ecs_component_record_t *cr = tr->hdr.cr;

        if (cr->flags & 
            (EcsIdOnInstantiateDontInherit|EcsIdOnInstantiateInherit)) 
        {
            continue;
        }

        if (id & ECS_AUTO_OVERRIDE) {
            /* If AUTO_OVERRIDE flag is set, add component to instances. This 
             * allows instances to end up with an owned component, even if the
             * component has the (OnInstantiate, Inherit) trait.
             * Additionally, this also allows for adding components to instances
             * that aren't copyable, since a prefab can have a component with
             * AUTO_OVERRIDE flag, but not have the actual component (which then
             * isn't copied). */
            flecs_type_add(world, &dst, id & ~ECS_AUTO_OVERRIDE);
            continue;
        }

        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        if (rel == EcsIsA) {
            /* If prefab child has IsA relationships, they will be inherited 
             * through the (IsA, prefab_child) relationship (added below). */
            continue;
        }

        if (rel == EcsParentDepth) {
            /* Replace depth value with the normalized depth for spawner. */
            id = ecs_value_pair(EcsParentDepth, depth);
        }

        flecs_type_add(world, &dst, id);
    }

    flecs_type_add(world, &dst, ecs_isa(child));

    return dst;
}

static
void flecs_prefab_spawner_build_from_cr(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_vec_t *spawner,
    int32_t parent_index,
    int32_t depth)
{
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
            NULL, spawner, ecs_tree_spawner_child_t);
        elem->parent_index = parent_index;
        elem->child_name = NULL;
        elem->child = (uint32_t)child;

        if (table->flags & EcsTableHasName) {
            elem->child_name = ecs_get_name(world, child);
        }

        ecs_type_t type = flecs_prefab_spawner_build_type(
            world, child, table, depth);
        elem->table = flecs_table_find_or_create(world, &type);
        ecs_assert(elem->table != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_type_free(world, &type);

        /* Make sure table doesn't get freed by shrink() */
        flecs_table_keep(elem->table);

        if (!(r->row & EcsEntityIsTraversable)) {
            continue;
        }

        ecs_component_record_t *child_cr = flecs_components_get(
            world, ecs_childof(child));
        if (!child_cr) {
            continue;
        }

        flecs_prefab_spawner_build_from_cr(
            world, child_cr, spawner, ecs_vec_count(spawner), depth + 1);
    }
}

static
void flecs_spawner_transpose_depth(
    ecs_world_t *world,
    EcsTreeSpawner *spawner,
    ecs_vec_t *dst,
    int32_t depth)
{
    ecs_vec_t *src = &spawner->data[0].children;

    int32_t i, count = ecs_vec_count(src);
    ecs_vec_set_count_t(NULL, dst, ecs_tree_spawner_child_t, count);

    for (i = 0; i < count; i ++) {
        ecs_tree_spawner_child_t *src_elem = ecs_vec_get_t(
            src, ecs_tree_spawner_child_t, i);
        ecs_tree_spawner_child_t *dst_elem = ecs_vec_get_t(
            dst, ecs_tree_spawner_child_t, i);

        dst_elem->child_name = src_elem->child_name;
        dst_elem->parent_index = src_elem->parent_index;
        dst_elem->child = src_elem->child;
        
        /* Get depth for source element at depth 0 */
        int32_t src_depth = flecs_relation_depth(
            world, EcsChildOf, src_elem->table);

        /* Get table for correct depth */
        ecs_id_t depth_pair = ecs_value_pair(EcsParentDepth, src_depth + depth);
        ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;

        dst_elem->table = flecs_table_traverse_add(
            world, src_elem->table, &depth_pair, &diff);
        
        flecs_table_keep(dst_elem->table);
    }
}

EcsTreeSpawner* flecs_prefab_spawner_build(
    ecs_world_t *world,
    ecs_entity_t base)
{    
    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(base));
    if (!cr) {
        return NULL;
    }

    ecs_vec_t spawner;
    ecs_vec_init_t(NULL, &spawner, ecs_tree_spawner_child_t, 0);
    flecs_prefab_spawner_build_from_cr(world, cr, &spawner, 0, 1);

    base = flecs_entities_get_alive(world, base);
    EcsTreeSpawner *ts = ecs_ensure(world, base, EcsTreeSpawner);
    ts->data[0].children = spawner;

    /* Initialize remaining vectors */
    int32_t i;
    for (i = 1; i < FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE; i ++) {
        ecs_vec_init_t(
            NULL, &ts->data[i].children, ecs_tree_spawner_child_t, 0);
    }

    return ts;
}

void flecs_spawner_instantiate(
    ecs_world_t *world,
    EcsTreeSpawner *spawner,
    ecs_entity_t instance)
{
    ecs_record_t *r_instance = flecs_entities_get(world, instance);
    int32_t depth = flecs_relation_depth(world, EcsChildOf, r_instance->table);
    int32_t i, child_count = ecs_vec_count(&spawner->data[0].children);

    bool is_prefab = r_instance->table->flags & EcsTableIsPrefab;
    
    /* Use cached spawner for depth if available. */
    ecs_vec_t *vec, tmp_vec;
    if (depth < FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE) {
        vec = &spawner->data[depth].children;
    } else {
        vec = &tmp_vec;
        ecs_vec_init_t(NULL, vec, ecs_tree_spawner_child_t, 0);
    }

    if (depth && ecs_vec_count(vec) != child_count) {
        /* Vector for depth is not yet initialized, create it. */
        flecs_spawner_transpose_depth(world, spawner, vec, depth);
    }

    ecs_tree_spawner_child_t *spawn_children = ecs_vec_first(vec);
    ecs_vec_set_min_count_t(&world->allocator, &world->allocators.tree_spawner, 
        ecs_entity_t, child_count + 1);
    ecs_entity_t *parents = ecs_vec_first(&world->allocators.tree_spawner);
    parents[0] = instance;

    ecs_component_record_t *cr = NULL;
    ecs_entity_t old_parent = 0;

    ecs_assert(ecs_vec_count(vec) == child_count, ECS_INTERNAL_ERROR, NULL);

    for (i = 0; i < child_count; i ++) {
        ecs_entity_t entity = parents[i + 1] = flecs_new_id(world);
        ecs_tree_spawner_child_t *spawn_child = &spawn_children[i];
        ecs_table_t *table = spawn_child->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (is_prefab) {
            ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
            ecs_id_t id = EcsPrefab;
            table = flecs_table_traverse_add(world, table, &id, &diff);

            if (spawn_child->child_name) {
                id = ecs_pair_t(EcsIdentifier, EcsName);
                table = flecs_table_traverse_add(world, table, &id, &diff);
            }
        }

        ecs_record_t *r = flecs_entities_get(world, entity);
        ecs_flags32_t flags = table->flags & EcsTableAddEdgeFlags;

        ecs_table_diff_t table_diff = { 
            .added = table->type,
            .added_flags = flags
        };

        int32_t row = flecs_table_append(world, table, entity, true, true);
        r->table = table;
        r->row = (uint32_t)row;

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

        if (is_prefab && spawn_child->child_name) {
            ecs_set_name(world, entity, spawn_child->child_name);
        }

        flecs_add_non_fragmenting_child_w_records(world, parent, entity, cr, r);

        ecs_entity_t base_child = spawn_child->child;
        ecs_record_t *spawn_r = flecs_entities_get_any(
            world, spawn_child->child);
        ecs_assert(spawn_r != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_table_range_t base_range = {
            .table = spawn_r->table,
            .offset = 0,
            .count = 1 };
        flecs_instantiate_sparse(world, &base_range, &base_child, 
            r->table, &entity, ECS_RECORD_TO_ROW(r->row));

        if (spawn_r->row & EcsEntityHasDontFragment) {
            flecs_instantiate_dont_fragment(
                world, spawn_child->child, entity);
        }
    }

    if (vec == &tmp_vec) {
        ecs_vec_fini_t(NULL, vec, ecs_tree_spawner_child_t);
    }
}

void flecs_bootstrap_spawner(
    ecs_world_t *world)
{
    flecs_type_info_init(world, EcsTreeSpawner, {
        .ctor = flecs_default_ctor,
        .copy = ecs_copy(EcsTreeSpawner),
        .move = ecs_move(EcsTreeSpawner),
        .dtor = ecs_dtor(EcsTreeSpawner)
    });

    ecs_add_pair(world, ecs_id(EcsTreeSpawner), 
        EcsOnInstantiate, EcsDontInherit);
}
