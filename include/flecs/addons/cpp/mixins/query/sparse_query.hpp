/** @file addons/cpp/mixins/query/sparse_query.hpp
 * @brief Direct iteration of sparse component storages. */

#pragma once

namespace flecs {

/** @defgroup cpp_queries Sparse queries
 * @ingroup cpp_core
 * Direct iteration of sparse component storages. @{ */

namespace _ {

inline void* field_at_sparse(
    const ecs_sparse_t *sparse, size_t size, uint64_t entity, bool checked)
{
    return flecs_sparse_get_w_check(sparse, static_cast<ecs_size_t>(size),
        entity, checked);
}

}

/** Query that iterates sparse component storages directly.
 * Returned by world::query() when all components have the dont_fragment
 * trait and don't declare the on_instantiate::inherit policy. */
template <typename ... Components>
struct sparse_query {
    static_assert(_::is_sparse_query<Components...>::value,
        "all sparse_query components must have the dont_fragment trait and "
        "must not declare the on_instantiate::inherit policy");

    explicit sparse_query(flecs::world_t *world)
        : world_(ECS_CONST_CAST(flecs::world_t*, ecs_get_world(world)))
        , ids_{ _::type<Components>::id(world)... }
    {
        assert_policies();
    }

    /** Iterate the query.
     * The function signature must match func(flecs::entity e, Components&...)
     * or func(Components&...). */
    template <typename Func>
    void each(Func&& func) const {
        each_impl(std::index_sequence_for<Components...>{}, func);
    }

    /** Return the number of entities matched by the query. */
    int32_t count() const {
        int32_t result = 0;
        each([&](Components&...) { result ++; });
        return result;
    }

    /** Convert to a regular flecs::query for the same components. */
    operator flecs::query<Components...>() const;

private:
    template <size_t ... Is, typename Func>
    void each_impl(std::index_sequence<Is...>, const Func& func) const {
        constexpr size_t n = sizeof...(Components);

        ecs_sparse_t *sparse[n] = { storage(ids_[Is])... };
        for (size_t f = 0; f < n; f ++) {
            if (!sparse[f]) {
                return;
            }
        }

        size_t lead = 0;
        for (size_t f = 1; f < n; f ++) {
            if (sparse[f]->count < sparse[lead]->count) {
                lead = f;
            }
        }

        const uint64_t *entities = flecs_sparse_ids(sparse[lead]);
        int32_t count = sparse[lead]->count - 1;

        for (int32_t i = 0; i < count; i ++) {
            uint64_t e = entities[i];
            void *ptrs[n];
            if (!(... && (ptrs[Is] = flecs_sparse_get_w_check(
                sparse[Is], ECS_SIZEOF(remove_reference_t<Components>), e, 
                Is != lead))))
            {
                continue;
            }

            const ecs_record_t *r = ecs_record_find(world_, e);
            if (flecs_table_flags(r->table) &
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))
            {
                continue;
            }

            if constexpr (std::is_invocable<
                Func, flecs::entity, Components&...>::value)
            {
                func(flecs::entity(world_, e),
                    (*static_cast<remove_reference_t<Components>*>(
                        ptrs[Is]))...);
            } else {
                func((*static_cast<remove_reference_t<Components>*>(
                    ptrs[Is]))...);
            }
        }
    }

    ecs_sparse_t* storage(flecs::id_t id) const {
        ecs_component_record_t *cr = flecs_components_get(world_, id);
        return cr ? flecs_component_get_sparse(cr) : nullptr;
    }

    void assert_policies() const {
        for (flecs::id_t id : ids_) {
            (void)id;
            ecs_assert(ecs_get_target(world_, id, flecs::OnInstantiate, 0) !=
                flecs::Inherit, ECS_INVALID_OPERATION,
                "sparse_query component has the OnInstantiate Inherit trait, "
                "which sparse queries cannot match; add the on_instantiate "
                "trait at compile time instead");
        }
    }

    flecs::world_t *world_;
    flecs::id_t ids_[sizeof...(Components)];
};

/** @} */

}
