/**
 * @file flecs_c.h
 * @brief Extends the core API with convenience functions/macros for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup create_macros Macros that help with creation of ECS objects.
 * @{
 */

/* Use for declaring entity, tag, prefab / any other entity identifier */
#define ECS_DECLARE(id)\
    ecs_entity_t id, ecs_id(id)

#define ECS_ENTITY_DEFINE(world, id, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.entity = id; \
        desc.name = #id; \
        desc.add_expr = #__VA_ARGS__; \
        id = ecs_entity_init(world, &desc); \
        ecs_id(id) = id; \
        ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL); \
    } \
    (void)id; \
    (void)ecs_id(id);

#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__);

#define ECS_TAG_DEFINE(world, id)         ECS_ENTITY_DEFINE(world, id, 0)
#define ECS_TAG(world, id)                ECS_ENTITY(world, id, 0)

#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)
#define ECS_PREFAB(world, id, ...)        ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/* Use for declaring component identifiers */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)
#define ECS_COMPONENT_DEFINE(world, id) \
    {\
        ecs_component_desc_t desc = {0}; \
        desc.entity.entity = ecs_id(id); \
        desc.entity.name = #id; \
        desc.entity.symbol = #id; \
        desc.type.size = ECS_SIZEOF(id); \
        desc.type.alignment = ECS_ALIGNOF(id); \
        ecs_id(id) = ecs_component_init(world, &desc);\
        ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/* Use for declaring observer and system identifiers */
#define ECS_SYSTEM_DECLARE(id)         ecs_entity_t ecs_id(id)

/* Observers */
#define ECS_OBSERVER_DEFINE(world, id, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        desc.entity.entity = ecs_id(id); \
        desc.entity.name = #id;\
        desc.callback = id;\
        desc.filter.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/** @} */

/**
 * @defgroup sorting_macros Convenience macros that help with component comparison and sorting
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
 * @defgroup function_macros Convenience macros that wrap ECS operations
 * @{
 */


/* -- World API -- */

#define ecs_set_hooks(world, T, ...)\
    ecs_set_hooks_id(world, ecs_id(T), &(ecs_type_hooks_t)__VA_ARGS__)

#define ecs_get_hooks(world, T)\
    ecs_get_hooks_id(world, ecs_id(T));

/* -- New -- */

#define ecs_new(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, relation, object)\
    ecs_new_w_id(world, ecs_pair(relation, object))

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

#define ecs_set_pair_second(world, subject, relation, object, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(relation, ecs_id(object)),\
        sizeof(object), &(object)__VA_ARGS__)

#define ecs_set_pair_object ecs_set_pair_second

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

/* -- Emplace -- */

#define ecs_emplace(world, entity, T)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T))))

/* -- Get -- */

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, R, object)\
    (ECS_CAST(const R*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(R), object))))

#define ecs_get_pair_second(world, subject, relation, O)\
    (ECS_CAST(const O*, ecs_get_id(world, subject,\
        ecs_pair(relation, ecs_id(O)))))

#define ecs_get_pair_object ecs_get_pair_second

/* -- Get from record -- */

#define ecs_record_get(world, record, T)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, ecs_id(T))))

#define ecs_record_get_pair(world, record, R, object)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, \
        ecs_pair(ecs_id(R), object))))

#define ecs_record_get_pair_second(world, record, relation, O)\
    (ECS_CAST(const O*, ecs_record_get_id(world, record,\
        ecs_pair(relation, ecs_id(O)))))

#define ecs_record_get_pair_object ecs_record_get_pair_second

/* -- Get mut from record -- */

#define ecs_record_get_mut(world, record, T)\
    (ECS_CAST(T*, ecs_record_get_mut_id(world, record, ecs_id(T))))

#define ecs_record_get_mut_pair(world, record, R, object)\
    (ECS_CAST(T*, ecs_record_get_mut_id(world, record, \
        ecs_pair(ecs_id(R), object))))

#define ecs_record_get_mut_pair_second(world, record, relation, O)\
    (ECS_CAST(O*, ecs_record_get_mut_id(world, record,\
        ecs_pair(relation, ecs_id(O)))))

#define ecs_record_get_mut_pair_object ecs_record_get_mut_pair_second

/* -- Ref -- */

#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(const T*, ecs_ref_get_id(world, ref, ecs_id(T))))

/* -- Get mut & Modified -- */

#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

#define ecs_get_mut_pair(world, subject, relation, object)\
    (ECS_CAST(relation*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(relation), object))))

#define ecs_get_mut_pair_second(world, subject, relation, object)\
    (ECS_CAST(object*, ecs_get_mut_id(world, subject,\
        ecs_pair(relation, ecs_id(object)))))

#define ecs_get_mut_pair_object ecs_get_mut_pair_second

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, relation, object)\
    ecs_modified_id(world, subject, ecs_pair(relation, object))


/* -- Singletons -- */

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


/* -- Has, Owns & Shares -- */

#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

#define ecs_has_pair(world, entity, relation, object)\
    ecs_has_id(world, entity, ecs_pair(relation, object))

#define ecs_owns_id(world, entity, id)\
    (ecs_search(world, ecs_get_table(world, entity), id, 0) != -1)

#define ecs_owns_pair(world, entity, relation, object)\
    ecs_owns_id(world, entity, ecs_pair(relation, object))

#define ecs_owns(world, entity, T)\
    ecs_owns_id(world, entity, ecs_id(T))

#define ecs_shares_id(world, entity, id)\
    (ecs_search_relation(world, ecs_get_table(world, entity), 0, ecs_id(id), \
        EcsIsA, 1, 0, 0, 0, 0) != -1)

#define ecs_shares_pair(world, entity, relation, object)\
    (ecs_shares_id(world, entity, ecs_pair(relation, object)))

#define ecs_shares(world, entity, T)\
    (ecs_shares_id(world, entity, ecs_id(T)))


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
 * @defgroup utilities Utility macros for commonly used operations
 * @{
 */

#define ecs_isa(e)       ecs_pair(EcsIsA, e)
#define ecs_childof(e)   ecs_pair(EcsChildOf, e)
#define ecs_dependson(e) ecs_pair(EcsDependsOn, e)

/** @} */

/**
 * @defgroup temporary_macros Temp macros for easing the transition to v3
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

/** Declare component hooks
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

#define ecs_query_new(world, q_expr)\
    ecs_query_init(world, &(ecs_query_desc_t){\
        .filter.expr = q_expr\
    })

#define ecs_rule_new(world, q_expr)\
    ecs_rule_init(world, &(ecs_filter_desc_t){\
        .expr = q_expr\
    })

/** @} */


#endif // FLECS_C_
