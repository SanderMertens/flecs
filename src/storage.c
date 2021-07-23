#include "private_api.h"

static
void reserve(
    ecs_world_t *world,
    ecs_storage_t *storage,
    const ecs_type_info_t *info,
    ecs_vector_t *vec,
    ecs_size_t size,
    ecs_size_t alignment,    
    int32_t min_size)
{
    int32_t count = ecs_vector_count(vec);
    if (min_size <= ecs_vector_size(vec)) {
        return;
    }

    ecs_move_ctor_t move;
    if (info && (move = info->lifecycle.ctor_move_dtor)) {
        /* Create new vector */
        ecs_vector_t *new_vec = ecs_vector_new_t(size, alignment, min_size);
        ecs_vector_set_count_t(&new_vec, size, alignment, count);

        void *old_buf = ecs_vector_first_t(vec, size, alignment);
        void *new_buf = ecs_vector_first_t(new_vec, size, alignment);

        move(world, info->component, &info->lifecycle,
            new_buf, old_buf, ecs_to_size_t(size), count, info->lifecycle.ctx);

        ecs_vector_free(vec);

        storage->impl = new_vec;
    } else {
        ecs_vector_set_size_t((ecs_vector_t**)&storage->impl, 
            size, alignment, min_size);
    }
}

static
void add_n(
    ecs_world_t *world,
    ecs_storage_t *storage,
    const ecs_type_info_t *info,
    ecs_vector_t *vec,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t to_add)
{
    reserve(world, storage, info, vec, size, alignment, 
        ecs_vector_count(vec) + to_add);
}

static
void add(
    ecs_world_t *world,
    ecs_storage_t *storage,
    const ecs_type_info_t *info,
    ecs_vector_t *vec,
    ecs_size_t size,
    ecs_size_t alignment)
{
    add_n(world, storage, info, vec, size, alignment, 1);
}

void ecs_storage_init(
    ecs_storage_t *storage)
{
    (void)storage;
    return;
}

void ecs_storage_fini(
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->fini != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->fini(storage);
    } else {
        ecs_vector_free(storage->impl);  
    }
}

void* ecs_storage_push(
    ecs_world_t *world,
    ecs_storage_t *storage,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    void *ptr;
    
    if (plugin) {
        ecs_assert(plugin->push != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        ptr = plugin->push(world, storage, id);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        const ecs_type_info_t *info = storage->info;

        add(world, storage, info, impl, size, alignment);

        ptr = ecs_vector_add_t((ecs_vector_t**)&storage->impl, size, alignment);

        ecs_ctor_t ctor;
        if (info && (ctor = info->lifecycle.ctor)) {
            ctor(world, info->component, &id, ptr, ecs_to_size_t(size), 
                1, info->lifecycle.ctx);
        }
    }

    return ptr;
}

void ecs_storage_push_n(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t count,
    const uint64_t *ids,
    const ecs_storage_t *src_storage,
    int32_t src_index)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->push_n != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->push_n(world, storage, count, ids, src_storage, src_index);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        const ecs_type_info_t *info = storage->info;

        add_n(world, storage, info, impl, size, alignment, count);

        void *ptr = ecs_vector_addn_t((ecs_vector_t**)&storage->impl, 
            size, alignment, count);

        if (src_storage) {
            ecs_assert(src_storage->plugin == storage->plugin, 
                ECS_INVALID_PARAMETER, NULL);

            ecs_vector_t *src_impl = src_storage->impl;
            ecs_assert(src_impl != NULL, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(ecs_vector_count(src_impl) >= src_index + count, 
                ECS_INVALID_PARAMETER, NULL);

            void *src_ptr = ecs_vector_get_t(
                src_impl, size, alignment, src_index);
            ecs_assert(src_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
            src_ptr = ECS_OFFSET(src_ptr, size * src_index);

            ecs_copy_ctor_t copy;
            if (info && (copy = info->lifecycle.copy_ctor)) {
                copy(world, info->component, &info->lifecycle, ids, ptr, 
                    src_ptr, ecs_to_size_t(size), count, info->lifecycle.ctx);
            }
        } else {
            ecs_ctor_t ctor;
            if (info && (ctor = info->lifecycle.ctor)) {
                ctor(world, info->component, ids, ptr, ecs_to_size_t(size),
                    count, info->lifecycle.ctx);
            }
        }
    }
}

void* ecs_storage_emplace(
    ecs_world_t *world,
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    void *ptr;
    
    if (plugin) {
        ecs_assert(plugin->emplace != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        ptr = plugin->emplace(world, storage);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        add(world, storage, storage->info, impl, size, alignment);

        ptr = ecs_vector_add_t((ecs_vector_t**)&storage->impl, size, alignment);
    }

    return ptr;
}

void ecs_storage_reserve(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_size_t size)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->reserve != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->reserve(world, storage, size);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        add(world, storage, storage->info, impl, size, alignment);
    }
}

void ecs_storage_grow(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_size_t count)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->grow != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->grow(world, storage, count);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        add_n(world, storage, storage->info, impl, size, alignment, count);
    }
}

void ecs_storage_erase(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->erase != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->erase(world, storage, index, id);
    } else {
        ecs_size_t size = storage->size;
        ecs_size_t align = storage->alignment;
        ecs_vector_t *vec = storage->impl;
        void *dst = ecs_vector_get_t(vec, size, align, index);
        void *src = ecs_vector_last_t(vec, size, align);
        
        const ecs_type_info_t *info = storage->info;
        ecs_move_ctor_t move;
        if (info && (move = info->lifecycle.ctor_move_dtor)) {
            move(world, info->component, &info->lifecycle,
                dst, src, ecs_to_size_t(size), 1, info->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst, src, size);
        }

        ecs_vector_remove_last(vec);
    }
}

void ecs_storage_pop(
    ecs_world_t *world,
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->erase != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->pop(world, storage);
    } else {
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        const ecs_type_info_t *info = storage->info;
        ecs_dtor_t dtor;
        if (info && (dtor = info->lifecycle.dtor)) {
            void *ptr = ecs_vector_last_t(storage->impl, size, alignment);
            dtor(world, info->component, ptr, ecs_to_size_t(size), 1,
                info->lifecycle.ctx);
        }

        ecs_vector_remove_last(storage->impl);
    }
}

void ecs_storage_swap(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index_a,
    int32_t index_b)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->swap != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->swap(world, storage, index_a, index_b);
    } else {
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        void *ptr = ecs_vector_first_t(storage->impl, size, alignment);

        void *tmp = ecs_os_alloca(size);
        void *el_a = ECS_OFFSET(ptr, size * index_a);
        void *el_b = ECS_OFFSET(ptr, size * index_b);

        ecs_os_memcpy(tmp, el_a, size);
        ecs_os_memcpy(el_a, el_b, size);
        ecs_os_memcpy(el_b, tmp, size);
    }    
}

void ecs_storage_merge(
    ecs_world_t *world,
    ecs_storage_t *dst,
    ecs_storage_t *src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = dst->plugin;
    
    if (plugin) {
        ecs_assert(plugin->merge != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->merge(world, dst, src);
    } else {
        ecs_vector_t *impl_dst = dst->impl;
        ecs_vector_t *impl_src = src->impl;
        ecs_size_t size = dst->size;
        ecs_size_t alignment = dst->alignment;
        int32_t dst_count = ecs_vector_count(impl_dst);

        if (!dst_count) {
            if (impl_dst) {
                ecs_vector_free(impl_dst);
            }

            src->impl = impl_src;

        } else {
            const ecs_type_info_t *info = dst->info;
            int32_t src_count = ecs_vector_count(impl_src);

            add_n(world, dst, info, impl_dst, 
                size, alignment, src_count);

            void *dst_ptr = ecs_vector_first_t(impl_dst, size, alignment);
            void *src_ptr = ecs_vector_first_t(impl_src, size, alignment);
            dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);

            /* Move values from src to dst */
            ecs_move_ctor_t move;
            if (info && (move = info->lifecycle.move_ctor)) {
                move(world, info->component, &info->lifecycle,
                    dst_ptr, src_ptr, ecs_to_size_t(size), src_count, 
                    info->lifecycle.ctx);
            } else {
                ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
            }

            ecs_vector_free(impl_src);
            src->impl = NULL;
        }
    }    
}

void ecs_storage_clear(
    ecs_world_t *world,
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->clear != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->clear(world, storage);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_dtor_t dtor;
        if (info && (dtor = info->lifecycle.dtor)) {
            ecs_vector_t *impl = storage->impl;
            ecs_size_t size = storage->size;
            ecs_size_t alignment = storage->alignment;
            void *ptr = ecs_vector_first_t(impl, size, alignment);
            int32_t count = ecs_vector_count(impl);
            dtor(world, info->component, ptr, 
                ecs_to_size_t(size), count, info->lifecycle.ctx);
        } else {
            ecs_vector_clear(storage->impl);
        }
    }    
}

void ecs_storage_destruct(
    ecs_world_t *world,
    ecs_storage_t *storage)
{
    ecs_storage_clear(world, storage);
    ecs_storage_fini(storage);
}

void* ecs_storage_get(
    const ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->get != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        return plugin->get(storage, index, id);
    } else {
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        return ecs_vector_get_t(storage->impl, size, alignment, index);
    }  
}

bool ecs_storage_has(
    const ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->has != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        return plugin->has(storage, index, id);
    } else {
        return ecs_vector_count(storage->impl) < index;
    }  
}

int32_t ecs_storage_count(
    const ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->count != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        return plugin->count(storage);
    } else {
        return ecs_vector_count(storage->impl);
    } 
}

void ecs_storage_ctor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->ctor != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->ctor(world, storage, index, id);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_ctor_t ctor;
        if (info && (ctor = info->lifecycle.ctor)) {
            ecs_size_t size = storage->size;
            ecs_size_t alignment = storage->alignment;
            void *ptr = ecs_vector_get_t(storage->impl, size, alignment, index);
            ctor(world, info->component, &id, 
                ptr, size, 1, info->lifecycle.ctx);
        }
    }     
}

void ecs_storage_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;    

    if (plugin) {
        ecs_assert(plugin->dtor != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->dtor(world, storage, index);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_dtor_t dtor;
        if (info && (dtor = info->lifecycle.dtor)) {
            ecs_size_t size = storage->size;
            ecs_size_t alignment = storage->alignment;
            void *ptr = ecs_vector_get_t(storage->impl, size, alignment, index);
            dtor(world, info->component, ptr, size, 1, info->lifecycle.ctx);
        }
    }     
}

void ecs_storage_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    const void *src)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->copy != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->copy(world, storage, index, src);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        void *ptr = ecs_vector_get_t(storage->impl, size, alignment, index);

        ecs_copy_t copy;
        if (info && (copy = info->lifecycle.copy)) {
            copy(world, info->component, ptr, src, size, 1, 
                info->lifecycle.ctx);
        } else {
            memcpy(ptr, src, size);
        }
    }     
}

void ecs_storage_ctor_move_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    void *src)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;

    if (plugin) {
        ecs_assert(plugin->ctor_move_dtor != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->ctor_move_dtor(world, storage, index, src);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        void *dst = ecs_vector_get_t(storage->impl, size, alignment, index);

        ecs_move_ctor_t move;
        if (info && (move = info->lifecycle.ctor_move_dtor)) {
            move(world, info->component, &info->lifecycle,
                dst, src, size, 1, info->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst, src, size);
        }
    }     
}

void ecs_storage_ctor_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id,
    const void *src)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->ctor_copy != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        plugin->ctor_copy(world, storage, index, id, src);
    } else {
        const ecs_type_info_t *info = storage->info;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;

        void *dst = ecs_vector_get_t(storage->impl, size, alignment, index);

        ecs_copy_ctor_t copy;
        if (info && (copy = info->lifecycle.copy_ctor)) {
            copy(world, info->component, &info->lifecycle, &id,
                dst, src, size, 1, info->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst, src, size);
        }
    }
}

ecs_storage_iter_t ecs_storage_iter(
    const ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->iter != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        return plugin->iter(storage);
    } else {
        ecs_vector_t *impl = storage->impl;
        ecs_size_t size = storage->size;
        ecs_size_t alignment = storage->alignment;
        return (ecs_storage_iter_t){
            .data = ecs_vector_first_t(impl, size, alignment),
            .count = ecs_vector_count(impl)
        };
    } 
}

bool ecs_storage_next(
    const ecs_storage_t *storage,
    ecs_storage_iter_t *iter)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_storage_plugin_t *plugin = storage->plugin;
    
    if (plugin) {
        ecs_assert(plugin->next != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
        return plugin->next(storage, iter);
    } else {
        /* Should not modify vector while iterating */
        ecs_assert(iter->data == ecs_vector_first_t(storage->impl, 
            storage->size, storage->alignment), ECS_INVALID_OPERATION, NULL);
        ecs_assert(iter->count == ecs_vector_count(storage->impl), 
            ECS_INVALID_OPERATION, NULL);

        if (iter->offset == iter->count) {
            return false;
        }

        iter->offset ++;

        return true;
    } 
}
