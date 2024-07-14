 /**
 * @file queries/api.c
 * @brief User facing API for rules.
 */

#include "../private_api.h"
#include <ctype.h>

ecs_mixins_t ecs_query_t_mixins = {
    .type_name = "ecs_query_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_query_impl_t, pub.real_world),
        [EcsMixinEntity] = offsetof(ecs_query_impl_t, pub.entity),
        [EcsMixinDtor] = offsetof(ecs_query_impl_t, dtor)
    }
};

int32_t ecs_query_find_var(
    const ecs_query_t *q,
    const char *name)
{
    flecs_poly_assert(q, ecs_query_t);

    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_var_id_t var_id = flecs_query_find_var_id(impl, name, EcsVarEntity);
    if (var_id == EcsVarNone) {
        if (q->flags & EcsQueryMatchThis) {
            if (!ecs_os_strcmp(name, EcsThisName)) {
                var_id = 0;
            }
        }
        if (var_id == EcsVarNone) {
            return -1;
        }
    }
    return (int32_t)var_id;
}

const char* ecs_query_var_name(
    const ecs_query_t *q,
    int32_t var_id)
{
    flecs_poly_assert(q, ecs_query_t);

    if (var_id) {
        ecs_assert(var_id < flecs_query_impl(q)->var_count, 
            ECS_INVALID_PARAMETER, NULL);
        return flecs_query_impl(q)->vars[var_id].name;
    } else {
        return EcsThisName;
    }
}

bool ecs_query_var_is_entity(
    const ecs_query_t *q,
    int32_t var_id)
{
    flecs_poly_assert(q, ecs_query_t);

    return flecs_query_impl(q)->vars[var_id].kind == EcsVarEntity;
}

static
int flecs_query_set_caching_policy(
    ecs_query_impl_t *impl,
    const ecs_query_desc_t *desc)
{
    ecs_query_cache_kind_t kind = desc->cache_kind;

    /* If caching policy is default, try to pick a policy that does the right
     * thing in most cases. */
    if (kind == EcsQueryCacheDefault) {
        if (desc->entity || desc->group_by || desc->group_by_callback || 
            desc->order_by || desc->order_by_callback)
        {
            /* If the query is created with an entity handle (typically 
             * indicating that the query is named or belongs to a system) the
             * chance is very high that the query will be reused, so enable
             * caching. 
             * Additionally, if the query uses features that require a cache
             * such as group_by/order_by, also enable caching. */
            kind = EcsQueryCacheAuto;
        } else {
            /* Be conservative in other scenario's, as caching adds significant
             * overhead to the cost of query creation which doesn't offset the
             * benefit of faster iteration if it's only used once. */
            kind = EcsQueryCacheNone;
        }
    }

    /* Don't cache query, even if it has cacheable terms */
    if (kind == EcsQueryCacheNone) {
        impl->pub.cache_kind = EcsQueryCacheNone;
        if (desc->group_by || desc->order_by) {
            ecs_err("cannot create uncached query with group_by/order_by");
            return -1;
        }
        return 0;
    }

    /* Entire query must be cached */
    if (desc->cache_kind == EcsQueryCacheAll) {
        if (impl->pub.flags & EcsQueryIsCacheable) {
            impl->pub.cache_kind = EcsQueryCacheAll;
            return 0;
        } else {
            ecs_err("cannot enforce QueryCacheAll, "
                "query contains uncacheable terms");
            return -1;
        }
    }

    /* Only cache terms that are cacheable */
    if (kind == EcsQueryCacheAuto) {
        if (impl->pub.flags & EcsQueryIsCacheable) {
            /* If all terms of the query are cacheable, just set the policy to 
             * All which simplifies work for the compiler. */
            impl->pub.cache_kind = EcsQueryCacheAll;
        } else if (!(impl->pub.flags & EcsQueryHasCacheable)) {
            /* Same for when the query has no cacheable terms */
            impl->pub.cache_kind = EcsQueryCacheNone;
        } else {
            /* Part of the query is cacheable */
            impl->pub.cache_kind = EcsQueryCacheAuto;
        }
    }

    return 0;
}

static
int flecs_query_create_cache(
    ecs_query_impl_t *impl,
    ecs_query_desc_t *desc)
{
    ecs_query_t *q = &impl->pub;
    if (flecs_query_set_caching_policy(impl, desc)) {
        return -1;
    }

    if ((q->cache_kind != EcsQueryCacheNone) && !q->entity) {
        /* Cached queries need an entity handle for observer components */
        q->entity = ecs_new(q->world);
        desc->entity = q->entity;
    }

    if (q->cache_kind == EcsQueryCacheAll) {
        /* Create query cache for all terms */
        if (!flecs_query_cache_init(impl, desc)) {
            goto error;
        }
    } else if (q->cache_kind == EcsQueryCacheAuto) {
        /* Query is partially cached */
        ecs_query_desc_t cache_desc = *desc;
        ecs_os_memset_n(&cache_desc.terms, 0, ecs_term_t, FLECS_TERM_COUNT_MAX);
        cache_desc.expr = NULL;

        /* Maps field indices from cache to query */
        int8_t field_map[FLECS_TERM_COUNT_MAX];

        int32_t i, count = q->term_count, dst_count = 0, dst_field = 0;
        ecs_term_t *terms = q->terms;
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            if (term->flags_ & EcsTermIsCacheable) {
                cache_desc.terms[dst_count] = *term;
                field_map[dst_field] = flecs_ito(int8_t, term->field_index);
                dst_count ++;
                if (i) {
                    dst_field += term->field_index != term[-1].field_index;
                } else {
                    dst_field ++;
                }
            }
        }

        if (dst_count) {
            if (!flecs_query_cache_init(impl, &cache_desc)) {
                goto error;
            }

            impl->field_map = flecs_alloc_n(&impl->stage->allocator,
                int8_t, FLECS_TERM_COUNT_MAX);

            ecs_os_memcpy_n(impl->field_map, field_map, int8_t, dst_count);
        }
    }

    return 0;
error:
    return -1;
}

static
void flecs_query_fini(
    ecs_query_impl_t *impl)
{
    ecs_stage_t *stage = impl->stage;
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &stage->allocator;

    if (impl->ctx_free) {
        impl->ctx_free(impl->pub.ctx);
    }

    if (impl->binding_ctx_free) {
        impl->binding_ctx_free(impl->pub.binding_ctx);
    }

    if (impl->vars != &impl->vars_cache.var) {
        flecs_free(a, (ECS_SIZEOF(ecs_query_var_t) + ECS_SIZEOF(char*)) * 
            impl->var_size, impl->vars);
    }

    flecs_free_n(a, ecs_query_op_t, impl->op_count, impl->ops);
    flecs_free_n(a, ecs_var_id_t, impl->pub.field_count, impl->src_vars);
    flecs_free_n(a, int32_t, impl->pub.field_count, impl->monitor);
    flecs_free_n(a, int8_t, FLECS_TERM_COUNT_MAX, impl->field_map);
    flecs_name_index_fini(&impl->tvar_index);
    flecs_name_index_fini(&impl->evar_index);

    ecs_query_t *q = &impl->pub;
    int i, count = q->term_count;
    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &q->terms[i];
        if (!(term->flags_ & EcsTermKeepAlive)) {
            continue;
        }

        ecs_id_record_t *idr = flecs_id_record_get(q->real_world, term->id);
        if (idr) {
            if (!(q->world->flags & EcsWorldQuit)) {
                if (ecs_os_has_threading()) {
                    int32_t idr_keep_alive = ecs_os_adec(&idr->keep_alive);
                    ecs_assert(idr_keep_alive >= 0, ECS_INTERNAL_ERROR, NULL);
                    (void)idr_keep_alive;
                } else {
                    idr->keep_alive --;
                    ecs_assert(idr->keep_alive >= 0, ECS_INTERNAL_ERROR, NULL);
                }
            }
        }
    }

    if (impl->tokens) {
        flecs_free(&impl->stage->allocator, impl->tokens_len, impl->tokens);
    }

    if (impl->cache) {
        flecs_query_cache_fini(impl);
    }

    flecs_poly_fini(impl, ecs_query_t);
    flecs_bfree(&stage->allocators.query_impl, impl);
}

static
void flecs_query_poly_fini(void *ptr) {
    flecs_query_fini(ptr);
}

static
char* flecs_query_append_token(
    char *dst,
    const char *src)
{
    int32_t len = ecs_os_strlen(src);
    ecs_os_memcpy(dst, src, len + 1);
    return dst + len + 1;
}

static
void flecs_query_populate_tokens(
    ecs_query_impl_t *impl)
{
    ecs_query_t *q = &impl->pub;
    int32_t i, term_count = q->term_count;

    char *old_tokens = impl->tokens;
    int32_t old_tokens_len = impl->tokens_len;
    impl->tokens = NULL;
    impl->tokens_len = 0;
    
    /* Step 1: determine size of token buffer */
    int32_t len = 0;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &q->terms[i];
        
        if (term->first.name) {
            len += ecs_os_strlen(term->first.name) + 1;
        }
        if (term->second.name) {
            len += ecs_os_strlen(term->second.name) + 1;
        }
        if (term->src.name) {
            len += ecs_os_strlen(term->src.name) + 1;
        }
    }

    /* Step 2: reassign term tokens to buffer */
    if (len) {
        impl->tokens = flecs_alloc(&impl->stage->allocator, len);
        impl->tokens_len = flecs_ito(int16_t, len);
        char *token = impl->tokens, *next;

        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &q->terms[i];
            if (term->first.name) {
                next = flecs_query_append_token(token, term->first.name);
                term->first.name = token;
                token = next;
            }
            if (term->second.name) {
                next = flecs_query_append_token(token, term->second.name);
                term->second.name = token;
                token = next;
            }
            if (term->src.name) {
                next = flecs_query_append_token(token, term->src.name);
                term->src.name = token;
                token = next;
            }
        }
    }

    if (old_tokens) {
        flecs_free(&impl->stage->allocator, old_tokens_len, old_tokens);
    }
}

static
void flecs_query_add_self_ref(
    ecs_query_t *q)
{
    if (q->entity) {
        int32_t t, term_count = q->term_count;
        ecs_term_t *terms = q->terms;

        for (t = 0; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            if (ECS_TERM_REF_ID(&term->src) == q->entity) {
                ecs_add_id(q->world, q->entity, term->id);
            }
        }
    }
}

void ecs_query_fini(
    ecs_query_t *q)
{
    flecs_poly_assert(q, ecs_query_t);

    if (q->entity) {
        /* If query is associated with entity, use poly dtor path */
        ecs_delete(q->world, q->entity);
    } else {
        flecs_query_fini(flecs_query_impl(q));
    }
}

ecs_query_t* ecs_query_init(
    ecs_world_t *world, 
    const ecs_query_desc_t *const_desc)
{
    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_query_impl_t *result = flecs_bcalloc(&stage->allocators.query_impl);
    flecs_poly_init(result, ecs_query_t);
    
    ecs_query_desc_t desc = *const_desc;
    ecs_entity_t entity = const_desc->entity;

    if (entity) {
        /* Remove existing query if entity has one */
        bool deferred = false;
        if (ecs_is_deferred(world)) {
            deferred = true;
            /* Ensures that remove operation doesn't get applied after bind */
            ecs_defer_suspend(world);
        }
        ecs_remove_pair(world, entity, ecs_id(EcsPoly), EcsQuery);
        if (deferred) {
            ecs_defer_resume(world);
        }
    }

    /* Initialize the query */
    result->pub.entity = entity;
    result->pub.real_world = world;
    result->pub.world = world_arg;
    result->stage = stage;

    ecs_assert(flecs_poly_is(result->pub.real_world, ecs_world_t),
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_poly_is(result->stage, ecs_stage_t),
        ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_finalize_query(world, &result->pub, &desc)) {
        goto error;
    }

    /* If query terms have itself as source, add term ids to self */
    flecs_query_add_self_ref(&result->pub);

    /* Store remaining string tokens in terms (after entity lookups) in single
     * token buffer which simplifies memory management & reduces allocations. */
    flecs_query_populate_tokens(result);

    /* Initialize static context */
    result->pub.ctx = const_desc->ctx;
    result->pub.binding_ctx = const_desc->binding_ctx;
    result->ctx_free = const_desc->ctx_free;
    result->binding_ctx_free = const_desc->binding_ctx_free;
    result->dtor = flecs_query_poly_fini;
    result->cache = NULL;

    /* Initialize query cache if necessary */
    if (flecs_query_create_cache(result, &desc)) {
        goto error;
    }

    /* Compile query to operations */
    if (flecs_query_compile(world, stage, result)) {
        goto error;
    }

    /* Entity could've been set by finalize query if query is cached */
    entity = result->pub.entity;
    if (entity) {
        EcsPoly *poly = flecs_poly_bind(world, entity, ecs_query_t);
        poly->poly = result;
        flecs_poly_modified(world, entity, ecs_query_t);
    }

    return &result->pub;
error:
    result->pub.entity = 0;
    ecs_query_fini(&result->pub);
    return NULL;
}

bool ecs_query_has(
    ecs_query_t *q,
    ecs_entity_t entity,
    ecs_iter_t *it)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_check(q->flags & EcsQueryMatchThis, ECS_INVALID_PARAMETER, NULL);

    *it = ecs_query_iter(q->world, q);
    ecs_iter_set_var(it, 0, entity);
    return ecs_query_next(it);
error:
    return false;
}

bool ecs_query_has_table(
    ecs_query_t *q,
    ecs_table_t *table,
    ecs_iter_t *it)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_check(q->flags & EcsQueryMatchThis, ECS_INVALID_PARAMETER, NULL);

    *it = ecs_query_iter(q->world, q);
    ecs_iter_set_var_as_table(it, 0, table);
    return ecs_query_next(it);
error:
    return false;
}

bool ecs_query_has_range(
    ecs_query_t *q,
    ecs_table_range_t *range,
    ecs_iter_t *it)
{
    flecs_poly_assert(q, ecs_query_t);

    if (q->flags & EcsQueryMatchThis) {
        if (range->table) {
            if ((range->offset + range->count) > ecs_table_count(range->table)) {
                return false;
            }
        }
    }

    *it = ecs_query_iter(q->world, q);
    if (q->flags & EcsQueryMatchThis) {
        ecs_iter_set_var_as_range(it, 0, range);
    }

    return ecs_query_next(it);
}

ecs_query_count_t ecs_query_count(
    const ecs_query_t *q)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_query_count_t result = {0};

    if (!(q->flags & EcsQueryMatchThis)) {
        return result;
    }

    ecs_run_aperiodic(q->world, EcsAperiodicEmptyTables);

    ecs_query_impl_t *impl = flecs_query_impl(q);
    if (impl->cache && q->flags & EcsQueryIsCacheable) {
        result.results = flecs_query_cache_table_count(impl->cache);
        result.entities = flecs_query_cache_entity_count(impl->cache);
        result.tables = flecs_query_cache_table_count(impl->cache);
        result.empty_tables = flecs_query_cache_empty_table_count(impl->cache);
    } else {
        ecs_iter_t it = flecs_query_iter(q->world, q);
        it.flags |= EcsIterIsInstanced;
        it.flags |= EcsIterNoData;

        while (flecs_query_next_instanced(&it)) {
            result.results ++;
            result.entities += it.count;
            ecs_iter_skip(&it);
        }
    }

    return result;
}

bool ecs_query_is_true(
    const ecs_query_t *q)
{
    flecs_poly_assert(q, ecs_query_t);

    ecs_run_aperiodic(q->world, EcsAperiodicEmptyTables);

    ecs_query_impl_t *impl = flecs_query_impl(q);
    if (impl->cache && q->flags & EcsQueryIsCacheable) {
        return flecs_query_cache_table_count(impl->cache) != 0;
    } else {
        ecs_iter_t it = flecs_query_iter(q->world, q);
        return ecs_iter_is_true(&it);
    }
}

int32_t ecs_query_match_count(
    const ecs_query_t *q)
{
    flecs_poly_assert(q, ecs_query_t);

    ecs_query_impl_t *impl = flecs_query_impl(q);
    if (!impl->cache) {
        return 0;
    } else {
        return impl->cache->match_count;
    }
}
