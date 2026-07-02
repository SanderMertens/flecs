/**
 * @file addons/cpp/sparse_query.hpp
 * @brief Direct iteration of sparse component storages.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_sparse_queries Sparse queries
 * @ingroup cpp_core
 * Direct iteration of sparse component storages.
 *
 * @{
 */

/** Query that iterates sparse component storages directly.
 * This type is returned by world::query() when all components in the template
 * argument list have the flecs::dont_fragment trait. Iteration bypasses the
 * query engine: the smallest sparse storage is iterated, and the other
 * storages are probed for each entity.
 *
 * Structural changes (add/remove/delete) are not allowed while iterating
 * unless they are deferred.
 */
template <typename ... Components>
struct sparse_query {
    static_assert(_::is_sparse_query<Components...>::value,
        "all sparse_query components must have the dont_fragment trait");

    explicit sparse_query(flecs::world_t *world)
        : world_(ECS_CONST_CAST(flecs::world_t*, ecs_get_world(world)))
        , ids_{ _::type<Components>::id(world)... } { }

    /** Iterate the query.
     * The function signature must match one of:
     *  - func(flecs::entity e, Components& ...)
     *  - func(Components& ...)
     *
     * @param func The callback function.
     */
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
            if (!(... && (ptrs[Is] = _::field_at_sparse(sparse[Is],
                sizeof(remove_reference_t<Components>), e, Is != lead))))
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

    flecs::world_t *world_;
    flecs::id_t ids_[sizeof...(Components)];
};

/** @} */

}
