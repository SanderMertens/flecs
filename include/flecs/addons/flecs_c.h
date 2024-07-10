/**
 * @file addons/flecs_c.h
 * @brief Extends the core API with convenience macros for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup flecs_c Macro API
 * @ingroup c
 * Convenience macro's for C API
 *
 * @{
 */

/**
 * @defgroup flecs_c_creation Creation macro's
 * Convenience macro's for creating entities, components and observers
 *
 * @{
 */

/* Use for declaring entity, tag, prefab / any other entity identifier */
#define ECS_DECLARE(id)\
    ecs_entity_t id, ecs_id(id)

/** Forward declare an entity. */
#define ECS_ENTITY_DECLARE ECS_DECLARE

/** Define a forward declared entity.
 *
 * Example:
 *
 * @code
 * ECS_ENTITY_DEFINE(world, MyEntity, Position, Velocity);
 * @endcode
 */
#define ECS_ENTITY_DEFINE(world, id_, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.id = id_; \
        desc.name = #id_; \
        desc.add_expr = #__VA_ARGS__; \
        id_ = ecs_entity_init(world, &desc); \
        ecs_id(id_) = id_; \
        ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, "failed to create entity %s", #id_); \
    } \
    (void)id_; \
    (void)ecs_id(id_)

/** Declare & define an entity.
 *
 * Example:
 *
 * @code
 * ECS_ENTITY(world, MyEntity, Position, Velocity);
 * @endcode
 */
#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__)

/** Forward declare a tag. */
#define ECS_TAG_DECLARE ECS_DECLARE

/** Define a forward declared tag.
 *
 * Example:
 *
 * @code
 * ECS_TAG_DEFINE(world, MyTag);
 * @endcode
 */
#define ECS_TAG_DEFINE(world, id) ECS_ENTITY_DEFINE(world, id, 0)

/** Declare & define a tag.
 *
 * Example:
 *
 * @code
 * ECS_TAG(world, MyTag);
 * @endcode
 */
#define ECS_TAG(world, id) ECS_ENTITY(world, id, 0)

/** Forward declare a prefab. */
#define ECS_PREFAB_DECLARE ECS_DECLARE

/** Define a forward declared prefab.
 *
 * Example:
 *
 * @code
 * ECS_PREFAB_DEFINE(world, MyPrefab, Position, Velocity);
 * @endcode
 */
#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)

/** Declare & define a prefab.
 *
 * Example:
 *
 * @code
 * ECS_PREFAB(world, MyPrefab, Position, Velocity);
 * @endcode
 */
#define ECS_PREFAB(world, id, ...) ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/** Forward declare a component. */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward declared component.
 *
 * Example:
 *
 * @code
 * ECS_COMPONENT_DEFINE(world, Position);
 * @endcode
 */
#define ECS_COMPONENT_DEFINE(world, id_) \
    {\
        ecs_component_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.use_low_id = true; \
        edesc.name = #id_; \
        edesc.symbol = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.type.size = ECS_SIZEOF(id_); \
        desc.type.alignment = ECS_ALIGNOF(id_); \
        ecs_id(id_) = ecs_component_init(world, &desc);\
    }\
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, "failed to create component %s", #id_)

/** Declare & define a component.
 *
 * Example:
 *
 * @code
 * ECS_COMPONENT(world, Position);
 * @endcode
 */
#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/* Forward declare an observer. */
#define ECS_OBSERVER_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward declared observer.
 *
 * Example:
 *
 * @code
 * ECS_OBSERVER_DEFINE(world, AddPosition, EcsOnAdd, Position);
 * @endcode
 */
#define ECS_OBSERVER_DEFINE(world, id_, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.name = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.callback = id_;\
        desc.query.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id_) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, "failed to create observer %s", #id_);\
    }

/** Declare & define an observer.
 *
 * Example:
 *
 * @code
 * ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);
 * @endcode
 */
#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id

/* Forward declare a query. */
#define ECS_QUERY_DECLARE(name)         ecs_query_t* name

/** Define a forward declared observer.
 *
 * Example:
 *
 * @code
 * ECS_QUERY_DEFINE(world, AddPosition, Position);
 * @endcode
 */
#define ECS_QUERY_DEFINE(world, name_, ...)\
    {\
        ecs_query_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.name = #name_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.expr = #__VA_ARGS__;\
        name_ = ecs_query_init(world, &desc);\
        ecs_assert(name_ != NULL, ECS_INVALID_PARAMETER, "failed to create query %s", #name_);\
    }

/** Declare & define an observer.
 *
 * Example:
 *
 * @code
 * ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);
 * @endcode
 */
#define ECS_QUERY(world, name, ...)\
    ecs_query_t* name = NULL; \
    ECS_QUERY_DEFINE(world, name, __VA_ARGS__);\
    (void)name

/** Shorthand for creating an entity with ecs_entity_init().
 *
 * Example:
 *
 * @code
 * ecs_entity(world, {
 *   .name = "MyEntity"
 * });
 * @endcode
 */
#define ecs_entity(world, ...)\
    ecs_entity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )

/** Shorthand for creating a component with ecs_component_init().
 *
 * Example:
 *
 * @code
 * ecs_component(world, {
 *   .type.size = 4,
 *   .type.alignment = 4
 * });
 * @endcode
 */
#define ecs_component(world, ...)\
    ecs_component_init(world, &(ecs_component_desc_t) __VA_ARGS__ )

/** Shorthand for creating a component from a type.
 *
 * Example:
 *
 * @code
 * ecs_component_t(world, Position);
 * @endcode
 */
#define ecs_component_t(world, T)\
    ecs_component_init(world, &(ecs_component_desc_t) { \
        .entity = ecs_entity(world, { \
            .name = #T, \
            .symbol = #T, \
            .use_low_id = true \
        }), \
        .type.size = ECS_SIZEOF(T), \
        .type.alignment = ECS_ALIGNOF(T) \
    })

/** Shorthand for creating a query with ecs_query_cache_init.
 *
 * Example:
 *   ecs_query(world, {
 *     .terms = {{ ecs_id(Position) }}
 *   });
 */
#define ecs_query(world, ...)\
    ecs_query_init(world, &(ecs_query_desc_t) __VA_ARGS__ )

/** Shorthand for creating an observer with ecs_observer_init().
 *
 * Example:
 *
 * @code
 * ecs_observer(world, {
 *   .terms = {{ ecs_id(Position) }},
 *   .events = { EcsOnAdd },
 *   .callback = AddPosition
 * });
 * @endcode
 */
#define ecs_observer(world, ...)\
    ecs_observer_init(world, &(ecs_observer_desc_t) __VA_ARGS__ )

/** @} */

/**
 * @defgroup flecs_c_type_safe Type Safe API
 * Macro's that wrap around core functions to provide a "type safe" API in C
 *
 * @{
 */

/**
 * @defgroup flecs_c_entities Entity API
 * @{
 */

/**
 * @defgroup flecs_c_creation_deletion Creation & Deletion
 * @{
 */

#define ecs_new_w(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, first, second)\
    ecs_new_w_id(world, ecs_pair(first, second))

#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

/** @} */

/**
 * @defgroup flecs_c_adding_removing Adding & Removing
 * @{
 */

#define ecs_add(world, entity, T)\
    ecs_add_id(world, entity, ecs_id(T))

#define ecs_add_pair(world, subject, first, second)\
    ecs_add_id(world, subject, ecs_pair(first, second))


#define ecs_remove(world, entity, T)\
    ecs_remove_id(world, entity, ecs_id(T))

#define ecs_remove_pair(world, subject, first, second)\
    ecs_remove_id(world, subject, ecs_pair(first, second))


#define ecs_auto_override(world, entity, T)\
    ecs_auto_override_id(world, entity, ecs_id(T))

#define ecs_auto_override_pair(world, subject, first, second)\
    ecs_auto_override_id(world, subject, ecs_pair(first, second))

/** @} */

/**
 * @defgroup flecs_c_getting_setting Getting & Setting
 * @{
 */

/* insert */
#define ecs_insert(world, ...)\
    ecs_entity(world, { .set = ecs_values(__VA_ARGS__)})

/* set */

#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), ptr)

#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

#define ecs_set_pair(world, subject, First, second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(ecs_id(First), second),\
        sizeof(First), &(First)__VA_ARGS__)

#define ecs_set_pair_second(world, subject, first, Second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(first, ecs_id(Second)),\
        sizeof(Second), &(Second)__VA_ARGS__)

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_AUTO_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

/* emplace */

#define ecs_emplace(world, entity, T, is_new)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T), is_new)))

#define ecs_emplace_pair(world, entity, First, second, is_new)\
    (ECS_CAST(First*, ecs_emplace_id(world, entity, ecs_pair_t(First, second), is_new)))

/* get */

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, First, second)\
    (ECS_CAST(const First*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_pair_second(world, subject, first, Second)\
    (ECS_CAST(const Second*, ecs_get_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

/* get_mut */

#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

#define ecs_get_mut_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_mut_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_get_mut_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

/* ensure */

#define ecs_ensure(world, entity, T)\
    (ECS_CAST(T*, ecs_ensure_id(world, entity, ecs_id(T))))

#define ecs_ensure_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_ensure_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_ensure_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_ensure_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_ensure(world, entity, T)\
    (ECS_CAST(T*, ecs_ensure_id(world, entity, ecs_id(T))))

#define ecs_ensure_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_ensure_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_ensure_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_ensure_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

/* modified */

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, first, second)\
    ecs_modified_id(world, subject, ecs_pair(first, second))

/* record */

#define ecs_record_get(world, record, T)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, ecs_id(T))))

#define ecs_record_has(world, record, T)\
    (ecs_record_has_id(world, record, ecs_id(T)))

#define ecs_record_get_pair(world, record, First, second)\
    (ECS_CAST(const First*, ecs_record_get_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_get_pair_second(world, record, first, Second)\
    (ECS_CAST(const Second*, ecs_record_get_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_record_ensure(world, record, T)\
    (ECS_CAST(T*, ecs_record_ensure_id(world, record, ecs_id(T))))

#define ecs_record_ensure_pair(world, record, First, second)\
    (ECS_CAST(First*, ecs_record_ensure_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_ensure_pair_second(world, record, first, Second)\
    (ECS_CAST(Second*, ecs_record_ensure_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(const T*, ecs_ref_get_id(world, ref, ecs_id(T))))

/** @} */

/**
 * @defgroup flecs_c_singletons Singletons
 * @{
 */

#define ecs_singleton_add(world, comp)\
    ecs_add(world, ecs_id(comp), comp)

#define ecs_singleton_remove(world, comp)\
    ecs_remove(world, ecs_id(comp), comp)

#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

#define ecs_singleton_set_ptr(world, comp, ptr)\
    ecs_set_ptr(world, ecs_id(comp), comp, ptr)

#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

#define ecs_singleton_ensure(world, comp)\
    ecs_ensure(world, ecs_id(comp), comp)

#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)

/** @} */

/**
 * @defgroup flecs_c_has Has, Owns, Shares
 * @{
 */

#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

#define ecs_has_pair(world, entity, first, second)\
    ecs_has_id(world, entity, ecs_pair(first, second))

#define ecs_owns_pair(world, entity, first, second)\
    ecs_owns_id(world, entity, ecs_pair(first, second))

#define ecs_owns(world, entity, T)\
    ecs_owns_id(world, entity, ecs_id(T))

#define ecs_shares_id(world, entity, id)\
    (ecs_search_relation(world, ecs_get_table(world, entity), 0, ecs_id(id), \
        EcsIsA, 1, 0, 0, 0, 0) != -1)

#define ecs_shares_pair(world, entity, first, second)\
    (ecs_shares_id(world, entity, ecs_pair(first, second)))

#define ecs_shares(world, entity, T)\
    (ecs_shares_id(world, entity, ecs_id(T)))

#define ecs_get_target_for(world, entity, rel, T)\
    ecs_get_target_for_id(world, entity, rel, ecs_id(T))

/** @} */

/**
 * @defgroup flecs_c_enable_disable Enabling & Disabling
 * @{
 */

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_id(world, entity, ecs_id(T), enable)

#define ecs_is_enabled(world, entity, T)\
    ecs_is_enabled_id(world, entity, ecs_id(T))

#define ecs_enable_pair(world, entity, First, second, enable)\
    ecs_enable_id(world, entity, ecs_pair(ecs_id(First), second), enable)

#define ecs_is_enabled_pair(world, entity, First, second)\
    ecs_is_enabled_id(world, entity, ecs_pair(ecs_id(First), second))

/** @} */

/**
 * @defgroup flecs_c_entity_names Entity Names
 * @{
 */

#define ecs_lookup_from(world, parent, path)\
    ecs_lookup_path_w_sep(world, parent, path, ".", NULL, true)

#define ecs_get_path_from(world, parent, child)\
    ecs_get_path_w_sep(world, parent, child, ".", NULL)

#define ecs_get_path(world, child)\
    ecs_get_path_w_sep(world, 0, child, ".", NULL)

#define ecs_get_path_buf(world, child, buf)\
    ecs_get_path_w_sep_buf(world, 0, child, ".", NULL, buf)

#define ecs_new_from_path(world, parent, path)\
    ecs_new_from_path_w_sep(world, parent, path, ".", NULL)

#define ecs_add_path(world, entity, parent, path)\
    ecs_add_path_w_sep(world, entity, parent, path, ".", NULL)

#define ecs_add_fullpath(world, entity, path)\
    ecs_add_path_w_sep(world, entity, 0, path, ".", NULL)

/** @} */

/** @} */

/**
 * @defgroup flecs_c_components Component API
 * @{
 */

#define ecs_set_hooks(world, T, ...)\
    ecs_set_hooks_id(world, ecs_id(T), &(ecs_type_hooks_t)__VA_ARGS__)

#define ecs_get_hooks(world, T)\
    ecs_get_hooks_id(world, ecs_id(T));

/** Declare a constructor.
 * Example:
 *
 * @code
 * ECS_CTOR(MyType, ptr, { ptr->value = NULL; });
 * @endcode
 */
#define ECS_CTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, ctor, var, __VA_ARGS__)

/** Declare a destructor.
 * Example:
 *
 * @code
 * ECS_DTOR(MyType, ptr, { free(ptr->value); });
 * @endcode
 */
#define ECS_DTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, dtor, var, __VA_ARGS__)

/** Declare a copy action.
 * Example:
 *
 * @code
 * ECS_COPY(MyType, dst, src, { dst->value = strdup(src->value); });
 * @endcode
 */
#define ECS_COPY(type, dst_var, src_var, ...)\
    ECS_COPY_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare a move action.
 * Example:
 *
 * @code
 * ECS_MOVE(MyType, dst, src, { dst->value = src->value; src->value = 0; });
 * @endcode
 */
#define ECS_MOVE(type, dst_var, src_var, ...)\
    ECS_MOVE_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare component hooks.
 * Example:
 *
 * @code
 * ECS_ON_SET(MyType, ptr, { printf("%d\n", ptr->value); });
 * @endcode
 */
#define ECS_ON_ADD(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_add(type), ptr, __VA_ARGS__)
#define ECS_ON_REMOVE(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_remove(type), ptr, __VA_ARGS__)
#define ECS_ON_SET(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_set(type), ptr, __VA_ARGS__)

/* Map from typename to function name of component lifecycle action */
#define ecs_ctor(type) type##_ctor
#define ecs_dtor(type) type##_dtor
#define ecs_copy(type) type##_copy
#define ecs_move(type) type##_move
#define ecs_on_set(type) type##_on_set
#define ecs_on_add(type) type##_on_add
#define ecs_on_remove(type) type##_on_remove

/** @} */

/**
 * @defgroup flecs_c_ids Id API
 * @{
 */

#define ecs_count(world, type)\
    ecs_count_id(world, ecs_id(type))

/** @} */

/**
 * @defgroup flecs_c_iterators Iterator API
 * @{
 */

#define ecs_field(it, T, index)\
    (ECS_CAST(T*, ecs_field_w_size(it, sizeof(T), index)))

/** @} */

/**
 * @defgroup flecs_c_tables Table API
 * @{
 */

#define ecs_table_get(world, table, T, offset)\
    (ECS_CAST(T*, ecs_table_get_id(world, table, ecs_id(T), offset)))

#define ecs_table_get_pair(world, table, First, second, offset)\
    (ECS_CAST(First*, ecs_table_get_id(world, table, ecs_pair(ecs_id(First), second), offset)))

#define ecs_table_get_pair_second(world, table, first, Second, offset)\
    (ECS_CAST(Second*, ecs_table_get_id(world, table, ecs_pair(first, ecs_id(Second)), offset)))

/** @} */

/**
 * @defgroup flecs_c_values Value API
 * @{
 */

/** Convenience macro for creating compound literal id array */
#define ecs_ids(...) (ecs_id_t[]){ __VA_ARGS__, 0 }

/** Convenience macro for creating compound literal values array */
#define ecs_values(...) (ecs_value_t[]){ __VA_ARGS__, {0, 0}}

/** Convenience macro for creating compound literal value */
#define ecs_value_ptr(T, ptr) ((ecs_value_t){ecs_id(T), ptr})

/** Convenience macro for creating compound literal pair value */
#define ecs_value_pair(R, t, ...) ((ecs_value_t){ecs_pair_t(R, t), &(R)__VA_ARGS__})

/** Convenience macro for creating compound literal pair value */
#define ecs_value_pair_2nd(r, T, ...) ((ecs_value_t){ecs_pair(r, ecs_id(T)), &(T)__VA_ARGS__})

/** Convenience macro for creating heap allocated value */
#define ecs_value_new_t(world, T) ecs_value_new(world, ecs_id(T))

/** Convenience macro for creating compound literal value literal */
#define ecs_value(T, ...) ((ecs_value_t){ecs_id(T), &(T)__VA_ARGS__})

/** @} */

/** @} */

/**
 * @defgroup flecs_c_table_sorting Table sorting
 * Convenience macro's for sorting tables.
 *
 * @{
 */
#define ecs_sort_table(id) ecs_id(id##_sort_table)

#define ecs_compare(id) ecs_id(id##_compare_fn)

/* Declare efficient table sorting operation that uses provided compare function.
 * For best results use LTO or make the function body visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions, use it to declare static
 *   or exported functions.
 * Parameters of the comparison function:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Parameters of the sort functions:
 *   ecs_world_t *world
 *   ecs_table_t *table
 *   ecs_entity_t *entities
 *   void *ptr
 *   int32_t elem_size
 *   int32_t lo
 *   int32_t hi
 *   ecs_order_by_action_t order_by - Pointer to the original comparison function. You are not supposed to use it.
 * Example:
 *
 * @code
 * int CompareMyType(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; }
 * ECS_SORT_TABLE_WITH_COMPARE(MyType, MyCustomCompare, CompareMyType)
 * @endcode
 */
#define ECS_SORT_TABLE_WITH_COMPARE(id, op_name, compare_fn, ...) \
    static int32_t ECS_CONCAT(op_name, _partition)( \
        ecs_world_t *world, \
        ecs_table_t *table, \
        ecs_entity_t *entities, \
        void *ptr, \
        int32_t elem_size, \
        int32_t lo, \
        int32_t hi, \
        ecs_order_by_action_t order_by) \
    { \
        (void)(order_by); \
        int32_t p = (hi + lo) / 2; \
        void *pivot = ECS_ELEM(ptr, elem_size, p); \
        ecs_entity_t pivot_e = entities[p]; \
        int32_t i = lo - 1, j = hi + 1; \
        void *el; \
    repeat: \
        { \
            do { \
                i ++; \
                el = ECS_ELEM(ptr, elem_size, i); \
            } while ( compare_fn(entities[i], el, pivot_e, pivot) < 0); \
            do { \
                j --; \
                el = ECS_ELEM(ptr, elem_size, j); \
            } while ( compare_fn(entities[j], el, pivot_e, pivot) > 0); \
            if (i >= j) { \
                return j; \
            } \
            ecs_table_swap_rows(world, table, i, j); \
            if (p == i) { \
                pivot = ECS_ELEM(ptr, elem_size, j); \
                pivot_e = entities[j]; \
            } else if (p == j) { \
                pivot = ECS_ELEM(ptr, elem_size, i); \
                pivot_e = entities[i]; \
            } \
            goto repeat; \
        } \
    } \
    __VA_ARGS__ void op_name( \
        ecs_world_t *world, \
        ecs_table_t *table, \
        ecs_entity_t *entities, \
        void *ptr, \
        int32_t size, \
        int32_t lo, \
        int32_t hi, \
        ecs_order_by_action_t order_by) \
    { \
        if ((hi - lo) < 1)  { \
            return; \
        } \
        int32_t p = ECS_CONCAT(op_name, _partition)(world, table, entities, ptr, size, lo, hi, order_by); \
        op_name(world, table, entities, ptr, size, lo, p, order_by); \
        op_name(world, table, entities, ptr, size, p + 1, hi, order_by); \
    }

/* Declare efficient table sorting operation that uses default component comparison operator.
 * For best results use LTO or make the comparison operator visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions, use it to declare static
 *   or exported functions.
 * Example:
 *
 * @code
 * ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 * ECS_SORT_TABLE(MyType)
 * @endcode
 */
#define ECS_SORT_TABLE(id, ...) \
    ECS_SORT_TABLE_WITH_COMPARE(id, ecs_sort_table(id), ecs_compare(id), __VA_ARGS__)

/* Declare component comparison operations.
 * Parameters:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Example:
 *
 * @code
 * ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 * @endcode
 */
#define ECS_COMPARE(id, ...) \
    int ecs_compare(id)(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { \
        __VA_ARGS__ \
    }

/** @} */

/**
 * @defgroup flecs_c_misc Misc
 * Misc convenience macro's.
 *
 * @{
 */

#define ecs_isa(e)       ecs_pair(EcsIsA, e)
#define ecs_childof(e)   ecs_pair(EcsChildOf, e)
#define ecs_dependson(e) ecs_pair(EcsDependsOn, e)
#define ecs_with(e)      ecs_pair(EcsWith, e)

#define ecs_each(world, id) ecs_each_id(world, ecs_id(id))
#define ecs_each_pair(world, r, t) ecs_each_id(world, ecs_pair(r, t))
#define ecs_each_pair_t(world, R, t) ecs_each_id(world, ecs_pair(ecs_id(R), t))

/** @} */

/** @} */

#endif // FLECS_C_
