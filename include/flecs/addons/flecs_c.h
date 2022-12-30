/**
 * @file addons/flecs_c.h
 * @brief Extends the core API with convenience macros for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup flecs_c Macro API
 * @brief Convenience macro's for C API
 * 
 * \ingroup c
 * @{
 */

/**
 * @defgroup flecs_c_creation Creation macro's
 * @brief Convenience macro's for creating entities, components and observers
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
 *   ECS_ENTITY_DEFINE(world, MyEntity, Position, Velocity);
 */
#define ECS_ENTITY_DEFINE(world, id_, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.id = id_; \
        desc.name = #id_; \
        desc.add_expr = #__VA_ARGS__; \
        id_ = ecs_entity_init(world, &desc); \
        ecs_id(id_) = id_; \
        ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, NULL); \
    } \
    (void)id_; \
    (void)ecs_id(id_);

/** Declare & define an entity.
 *
 * Example:
 *   ECS_ENTITY(world, MyEntity, Position, Velocity);
 */
#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__);

/** Forward declare a tag. */
#define ECS_TAG_DECLARE ECS_DECLARE

/** Define a forward declared tag.
 * 
 * Example:
 *   ECS_TAG_DEFINE(world, MyTag);
 */
#define ECS_TAG_DEFINE(world, id) ECS_ENTITY_DEFINE(world, id, 0)

/** Declare & define a tag.
 *
 * Example:
 *   ECS_TAG(world, MyTag);
 */
#define ECS_TAG(world, id) ECS_ENTITY(world, id, 0)

/** Forward declare a prefab. */
#define ECS_PREFAB_DECLARE ECS_DECLARE

/** Define a forward declared prefab.
 * 
 * Example:
 *   ECS_PREFAB_DEFINE(world, MyPrefab, Position, Velocity);
 */
#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)

/** Declare & define a prefab.
 *
 * Example:
 *   ECS_PREFAB(world, MyPrefab, Position, Velocity);
 */
#define ECS_PREFAB(world, id, ...) ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/** Forward declare a component. */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward declared component.
 * 
 * Example:
 *   ECS_COMPONENT_DEFINE(world, Position);
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
        ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

/** Declare & define a component.
 *
 * Example:
 *   ECS_COMPONENT(world, Position);
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
 *   ECS_OBSERVER_DEFINE(world, AddPosition, EcsOnAdd, Position);
 */
#define ECS_OBSERVER_DEFINE(world, id_, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.name = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.callback = id_;\
        desc.filter.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id_) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

/** Declare & define an observer.
 *
 * Example:
 *   ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);
 */
#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/** Shorthand for creating an entity with ecs_entity_init.
 *
 * Example:
 *   ecs_entity(world, {
 *     .name = "MyEntity"
 *   });
 */
#define ecs_entity(world, ...)\
    ecs_entity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )

/** Shorthand for creating a filter with ecs_filter_init.
 *
 * Example:
 *   ecs_filter(world, {
 *     .terms = {{ ecs_id(Position) }}
 *   });
 */
#define ecs_filter(world, ...)\
    ecs_filter_init(world, &(ecs_filter_desc_t) __VA_ARGS__ )

/** Shorthand for creating a query with ecs_query_init.
 *
 * Example:
 *   ecs_query(world, {
 *     .filter.terms = {{ ecs_id(Position) }}
 *   });
 */
#define ecs_query(world, ...)\
    ecs_query_init(world, &(ecs_query_desc_t) __VA_ARGS__ )

/** Shorthand for creating an observer with ecs_observer_init.
 *
 * Example:
 *   ecs_observer(world, {
 *     .filter.terms = {{ ecs_id(Position) }},
 *     .events = { EcsOnAdd },
 *     .callback = AddPosition
 *   });
 */
#define ecs_observer(world, ...)\
    ecs_observer_init(world, &(ecs_observer_desc_t) __VA_ARGS__ )

/** @} */

/**
 * @defgroup flecs_c_type_safe Type Safe API
 * @brief Macro's that wrap around core functions to provide a "type safe" API in C
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

#define ecs_new(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, first, second)\
    ecs_new_w_id(world, ecs_pair(first, second))

#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

#define ecs_new_entity(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t){\
        .name = n,\
    })

#define ecs_new_prefab(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t){\
        .name = n,\
        .add = {EcsPrefab}\
    })

#define ecs_delete_children(world, parent)\
    ecs_delete_with(world, ecs_pair(EcsChildOf, parent))

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


#define ecs_override(world, entity, T)\
    ecs_override_id(world, entity, ecs_id(T))

#define ecs_override_pair(world, subject, first, second)\
    ecs_override_id(world, subject, ecs_pair(first, second))

/** @} */

/**
 * @defgroup flecs_c_getting_setting Getting & Setting
 * @{
 */

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

#define ecs_set_pair_object ecs_set_pair_second

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

#define ecs_emplace(world, entity, T)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T))))

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, First, second)\
    (ECS_CAST(const First*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_pair_second(world, subject, first, Second)\
    (ECS_CAST(const Second*, ecs_get_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_get_pair_object ecs_get_pair_second

#define ecs_record_get(world, record, T)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, ecs_id(T))))

#define ecs_record_get_pair(world, record, First, second)\
    (ECS_CAST(const First*, ecs_record_get_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_get_pair_second(world, record, first, Second)\
    (ECS_CAST(const Second*, ecs_record_get_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_record_get_mut(world, record, T)\
    (ECS_CAST(T*, ecs_record_get_mut_id(world, record, ecs_id(T))))

#define ecs_record_get_mut_pair(world, record, First, second)\
    (ECS_CAST(First*, ecs_record_get_mut_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_get_mut_pair_second(world, record, first, Second)\
    (ECS_CAST(Second*, ecs_record_get_mut_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_record_get_mut_pair_object ecs_record_get_mut_pair_second

#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(const T*, ecs_ref_get_id(world, ref, ecs_id(T))))

#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

#define ecs_get_mut_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_mut_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_get_mut_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_get_mut_pair_object ecs_get_mut_pair_second

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, first, second)\
    ecs_modified_id(world, subject, ecs_pair(first, second))

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

#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp)

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

#define ecs_owns_id(world, entity, id)\
    (ecs_search(world, ecs_get_table(world, entity), id, 0) != -1)

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

/** @} */

/**
 * @defgroup flecs_c_enable_disable Enabling & Disabling
 * @{
 */

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_id(world, entity, ecs_id(T), enable)

#define ecs_is_enabled_component(world, entity, T)\
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

/** Declare component hooks.
 * Example:
 *   ECS_ON_SET(MyType, ptr, { printf("%d\n", ptr->value); });
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

#define ecs_value(T, ptr) ((ecs_value_t){ecs_id(T), ptr})
#define ecs_value_new_t(world, T) ecs_value_new(world, ecs_id(T))

/** @} */

/** @} */

/**
 * @defgroup flecs_c_table_sorting Table sorting
 * @brief Convenience macro's for sorting tables.
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
 *   int CompareMyType(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; }
 *   ECS_SORT_TABLE_WITH_COMPARE(MyType, MyCustomCompare, CompareMyType)
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
 *   ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 *   ECS_SORT_TABLE(MyType)
 */
#define ECS_SORT_TABLE(id, ...) \
    ECS_SORT_TABLE_WITH_COMPARE(id, ecs_sort_table(id), ecs_compare(id), __VA_ARGS__)

/* Declare component comparison operations.
 * Parameters:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Example:
 *   ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 */
#define ECS_COMPARE(id, ...) \
    int ecs_compare(id)(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { \
        __VA_ARGS__ \
    }

/** @} */

/**
 * @defgroup flecs_c_misc Misc
 * @brief Misc convenience macro's.
 * 
 * @{
 */

#define ecs_isa(e)       ecs_pair(EcsIsA, e)
#define ecs_childof(e)   ecs_pair(EcsChildOf, e)
#define ecs_dependson(e) ecs_pair(EcsDependsOn, e)

#define ecs_query_new(world, q_expr)\
    ecs_query_init(world, &(ecs_query_desc_t){\
        .filter.expr = q_expr\
    })

#define ecs_rule_new(world, q_expr)\
    ecs_rule_init(world, &(ecs_filter_desc_t){\
        .expr = q_expr\
    })

/** @} */

/** @} */

#endif // FLECS_C_
