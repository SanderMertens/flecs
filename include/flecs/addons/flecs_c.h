/**
 * @file flecs_c.h
 * @brief Extends the core API with convenience functions/macro's for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup create_macros Macro's that help with creation of ECS objects.
 * @{
 */

/* Use for declaring entity, tag, prefab / any other entity identifier */
#define ECS_DECLARE(id)\
    ecs_entity_t id;\
    ecs_entity_t ecs_id(id)

#define ECS_ENTITY_DEFINE(world, id, ...)\
    id = ecs_entity_init(world, &(ecs_entity_desc_t){\
        .name = #id,\
        .add_expr = #__VA_ARGS__\
    });\
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);\
    ecs_id(id) = id;\
    (void)id;\
    (void)ecs_id(id)

#define ECS_ENTITY(world, id, ...)\
    ecs_entity_t ecs_id(id);\
    ecs_entity_t ECS_ENTITY_DEFINE(world, id, __VA_ARGS__);

#define ECS_TAG_DEFINE(world, id)         ECS_ENTITY_DEFINE(world, id, 0)
#define ECS_TAG(world, id)                ECS_ENTITY(world, id, 0)

#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)
#define ECS_PREFAB(world, id, ...)        ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/* Use for declaring component identifiers */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)
#define ECS_COMPONENT_DEFINE(world, id)\
    ecs_id(id) = ecs_component_init(world, &(ecs_component_desc_t){\
        .entity = {\
            .entity = ecs_id(id),\
            .name = #id,\
            .symbol = #id\
        },\
        .size = sizeof(id),\
        .alignment = ECS_ALIGNOF(id)\
    });\
    ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL)

#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/* Use for declaring trigger, observer and system identifiers */
#define ECS_SYSTEM_DECLARE(id)         ecs_entity_t ecs_id(id)

/* Triggers */
#define ECS_TRIGGER_DEFINE(world, id, kind, component) \
    ecs_id(id) = ecs_trigger_init(world, &(ecs_trigger_desc_t){\
        .entity.name = #id,\
        .callback = id,\
        .expr = #component,\
        .events = {kind},\
    });\
    ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);

#define ECS_TRIGGER(world, id, kind, component) \
    ecs_entity_t ECS_TRIGGER_DEFINE(world, id, kind, component);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/* Observers */
#define ECS_OBSERVER_DEFINE(world, id, kind, ...)\
    ecs_id(id) = ecs_observer_init(world, &(ecs_observer_desc_t){\
        .entity.name = #id,\
        .callback = id,\
        .filter.expr = #__VA_ARGS__,\
        .events = {kind},\
    });\
    ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL)

#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/** @} */


/**
 * @defgroup function_macros Convenience macro's that wrap ECS operations
 * @{
 */


/* -- World API -- */

#define ecs_set_component_actions(world, T, ...)\
    ecs_set_component_actions_w_id(world, ecs_id(T), &(EcsComponentLifecycle)__VA_ARGS__)

/* -- New -- */

#define ecs_new(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, relation, object)\
    ecs_new_w_id(world, ecs_pair(relation, object))

#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

#define ecs_new_entity(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t) {\
        .name = n,\
    })

#define ecs_new_prefab(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t) {\
        .name = n,\
        .add = {EcsPrefab}\
    })

/* -- Add -- */

#define ecs_add(world, entity, T)\
    ecs_add_id(world, entity, ecs_id(T))

#define ecs_add_pair(world, subject, relation, object)\
    ecs_add_id(world, subject, ecs_pair(relation, object))


/* -- Remove -- */

#define ecs_remove(world, entity, T)\
    ecs_remove_id(world, entity, ecs_id(T))

#define ecs_remove_pair(world, subject, relation, object)\
    ecs_remove_id(world, subject, ecs_pair(relation, object))


/* -- Bulk remove/delete -- */

#define ecs_delete_children(world, parent)\
    ecs_delete_with(world, ecs_pair(EcsChildOf, parent))


/* -- Set -- */

#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), ptr)

#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

#define ecs_set_pair(world, subject, relation, object, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(ecs_id(relation), object),\
        sizeof(relation), &(relation)__VA_ARGS__)

#define ecs_set_pair_object(world, subject, relation, object, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(relation, ecs_id(object)),\
        sizeof(object), &(object)__VA_ARGS__)

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

/* -- Emplace -- */

#define ecs_emplace(world, entity, T)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T))))


/* -- Get -- */

#define ecs_get_ref(world, ref, entity, T)\
    (ECS_CAST(const T*, ecs_get_ref_w_id(world, ref, entity, ecs_id(T))))

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, relation, object)\
    (ECS_CAST(relation*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(relation), object))))

#define ecs_get_pair_object(world, subject, relation, object)\
    (ECS_CAST(object*, ecs_get_id(world, subject,\
        ecs_pair(relation, ecs_id(object)))))


/* -- Get mut & Modified -- */

#define ecs_get_mut(world, entity, T, is_added)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T), is_added)))

#define ecs_get_mut_pair(world, subject, relation, object, is_added)\
    (ECS_CAST(relation*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(relation), object), is_added)))

#define ecs_get_mut_pair_object(world, subject, relation, object, is_added)\
    (ECS_CAST(object*, ecs_get_mut_id(world, subject,\
        ecs_pair(relation, ecs_id(object)), is_added)))

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, relation, object)\
    ecs_modified_id(world, subject, ecs_pair(relation, object))


/* -- Singletons -- */

#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp, NULL)

#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)


/* -- Has, Owns & Shares -- */

#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

#define ecs_has_pair(world, entity, relation, object)\
    ecs_has_id(world, entity, ecs_pair(relation, object))

#define ecs_owns_id(world, entity, id)\
    ecs_type_has_id(world, ecs_get_type(world, entity), id, true)

#define ecs_owns_pair(world, entity, relation, object)\
    ecs_type_has_id(world, ecs_get_type(world, entity), ecs_pair(relation, object), true)

#define ecs_owns(world, entity, T)\
    ecs_type_has_id(world, ecs_get_type(world, entity), ecs_id(T), true)

#define ecs_shares_id(world, entity, id)\
    ecs_type_has_id(world, ecs_get_type(world, entity), id, false)

#define ecs_shares_pair(world, entity, relation, object)\
    ecs_type_has_id(world, ecs_get_type(world, entity), ecs_pair(relation, object), false)

#define ecs_shares(world, entity, T)\
    ecs_type_has_id(world, ecs_get_type(world, entity), ecs_id(T), false)


/* -- Enable / Disable component -- */

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_component_w_id(world, entity, ecs_id(T), enable)

#define ecs_is_component_enabled(world, entity, T)\
    ecs_is_component_enabled_w_id(world, entity, ecs_id(T))


/* -- Count -- */

#define ecs_count(world, type)\
    ecs_count_id(world, ecs_id(type))


/* -- Lookups & Paths -- */

#define ecs_lookup_path(world, parent, path)\
    ecs_lookup_path_w_sep(world, parent, path, ".", NULL, true)

#define ecs_lookup_fullpath(world, path)\
    ecs_lookup_path_w_sep(world, 0, path, ".", NULL, true)

#define ecs_get_path(world, parent, child)\
    ecs_get_path_w_sep(world, parent, child, ".", NULL)

#define ecs_get_fullpath(world, child)\
    ecs_get_path_w_sep(world, 0, child, ".", NULL)

#define ecs_get_fullpath_buf(world, child, buf)\
    ecs_get_path_w_sep_buf(world, 0, child, ".", NULL, buf)

#define ecs_new_from_path(world, parent, path)\
    ecs_new_from_path_w_sep(world, parent, path, ".", NULL)

#define ecs_new_from_fullpath(world, path)\
    ecs_new_from_path_w_sep(world, 0, path, ".", NULL)

#define ecs_add_path(world, entity, parent, path)\
    ecs_add_path_w_sep(world, entity, parent, path, ".", NULL)

#define ecs_add_fullpath(world, entity, path)\
    ecs_add_path_w_sep(world, entity, 0, path, ".", NULL)


/* -- Queries -- */

#define ecs_query_table_count(query)\
    ecs_vector_count(query->cache.tables)

#define ecs_query_empty_table_count(query)\
    ecs_vector_count(query->cache.empty_tables)

/* -- Iterators -- */

#define ecs_term_id(it, index)\
    ((it)->ids[(index) - 1])

#define ecs_term_source(it, index)\
    ((it)->subjects ? (it)->subjects[(index) - 1] : 0)

#define ecs_term_size(it, index)\
    ((index) == 0 ? sizeof(ecs_entity_t) : ECS_CAST(size_t, (it)->sizes[(index) - 1]))

#define ecs_term_is_owned(it, index)\
    ((it)->subjects == NULL || (it)->subjects[(index) - 1] == 0)

#define ecs_term(it, T, index)\
    (ECS_CAST(T*, ecs_term_w_size(it, sizeof(T), index)))

#define ecs_iter_column(it, T, index)\
    (ECS_CAST(T*, ecs_iter_column_w_size(it, sizeof(T), index)))

/** @} */

/**
 * @defgroup utilities Utility macro's for commonly used operations
 * @{
 */

#define ecs_childof(parent) ecs_pair(EcsChildOf, parent)

#define ecs_isa(base) ecs_pair(EcsIsA, base)

/** @} */

/**
 * @defgroup temporary_macros Temp macro's for easing the transition to v3
 * @{
 */

/** Declare a constructor.
 * Example:
 *   ECS_CTOR(MyType, ptr, { ptr->value = NULL; });
 */
#define ECS_CTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, ctor, var, __VA_ARGS__)

/** Declare a destructor.
 * Example:
 *   ECS_DTOR(MyType, ptr, { free(ptr->value); });
 */
#define ECS_DTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, dtor, var, __VA_ARGS__)

/** Declare a copy action.
 * Example:
 *   ECS_COPY(MyType, dst, src, { dst->value = strdup(src->value); });
 */
#define ECS_COPY(type, dst_var, src_var, ...)\
    ECS_COPY_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare a move action.
 * Example:
 *   ECS_MOVE(MyType, dst, src, { dst->value = src->value; src->value = 0; });
 */
#define ECS_MOVE(type, dst_var, src_var, ...)\
    ECS_MOVE_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare an on_set action.
 * Example:
 *   ECS_ON_SET(MyType, ptr, { printf("%d\n", ptr->value); });
 */
#define ECS_ON_SET(type, ptr, ...)\
    ECS_ON_SET_IMPL(type, ptr, __VA_ARGS__)

/* Map from typename to function name of component lifecycle action */
#define ecs_ctor(type) type##_ctor
#define ecs_dtor(type) type##_dtor
#define ecs_copy(type) type##_copy
#define ecs_move(type) type##_move
#define ecs_on_set(type) type##_on_set

#define ecs_query_new(world, q_expr)\
    ecs_query_init(world, &(ecs_query_desc_t){\
        .filter.expr = q_expr\
    })

#define ECS_TYPE(world, id, ...) \
    ecs_entity_t id = ecs_type_init(world, &(ecs_type_desc_t){\
        .entity.name = #id,\
        .ids_expr = #__VA_ARGS__\
    });\
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);\
    (void)id;

/** @} */


#endif // FLECS_C_
