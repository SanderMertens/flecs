/**
 * @file type_info.c
 * @brief Component metadata and lifecycle hooks.
 */

#include "private_api.h"

void flecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ti)
{
    ecs_os_memset(ptr, 0, ti->size * count);
}

static
void flecs_default_copy_ctor(void *dst_ptr, const void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->copy(dst_ptr, src_ptr, count, ti);
}

static
void flecs_default_move_ctor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void flecs_default_ctor_w_move_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->ctor(dst_ptr, count, ti);
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void flecs_default_move_ctor_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move_ctor(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
void flecs_default_move(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move(dst_ptr, src_ptr, count, ti);
}

static
void flecs_default_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    /* When there is no move, destruct the destination component & memcpy the
     * component to dst. The src component does not have to be destructed when
     * a component has a trivial move. */
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->dtor(dst_ptr, count, ti);
    ecs_os_memcpy(dst_ptr, src_ptr, flecs_uto(ecs_size_t, ti->size) * count);
}

static
void flecs_default_move_w_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *ti)
{
    /* If a component has a move, the move will take care of memcpying the data
     * and destroying any data in dst. Because this is not a trivial move, the
     * src component must also be destructed. */
    const ecs_type_hooks_t *cl = &ti->hooks;
    cl->move(dst_ptr, src_ptr, count, ti);
    cl->dtor(src_ptr, count, ti);
}

static
bool flecs_default_equals(const void *a_ptr, const void *b_ptr, const ecs_type_info_t* ti) {
    return ti->hooks.cmp(a_ptr, b_ptr, ti) == 0;
}

ECS_NORETURN static
void flecs_ctor_illegal(
    void * dst,
    int32_t count,
    const ecs_type_info_t *ti) {
    (void)dst; /* silence unused warning */
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid constructor for %s", ti->name);
}

ECS_NORETURN static
void flecs_dtor_illegal(
    void *dst,
    int32_t count,
    const ecs_type_info_t *ti) {
    (void)dst; /* silence unused warning */
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid destructor for %s", ti->name);
}

ECS_NORETURN static
void flecs_copy_illegal(
    void *dst,
    const void *src,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)dst; /* silence unused warning */
    (void)src; 
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy assignment for %s", ti->name);
}

ECS_NORETURN static
void flecs_move_illegal(
    void * dst,
    void * src,
    int32_t count,
    const ecs_type_info_t *ti) {
    (void)dst; /* silence unused warning */
    (void)src;
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid move assignment for %s", ti->name);
}

ECS_NORETURN static
void flecs_copy_ctor_illegal(
    void *dst,
    const void *src,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)dst; /* silence unused warning */
    (void)src;
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy construct for %s", ti->name);
}

ECS_NORETURN static
void flecs_move_ctor_illegal(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)dst; /* silence unused warning */
    (void)src;
    (void)count;
    ecs_abort(ECS_INVALID_OPERATION, "invalid move construct for %s", ti->name);
}

ECS_NORETURN static
int flecs_comp_illegal(
    const void *dst,
    const void *src,
    const ecs_type_info_t *ti)
{
    (void)dst; /* silence unused warning */
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "invalid compare hook for %s", ti->name);
}

ECS_NORETURN static
bool flecs_equals_illegal(
    const void *dst,
    const void *src,
    const ecs_type_info_t *ti)
{
    (void)dst; /* silence unused warning */
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "invalid equals hook for %s", ti->name);
}

void ecs_set_hooks_id(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_hooks_t *h)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, component), ECS_INVALID_PARAMETER, NULL);

    /* TODO: Refactor to enforce flags consistency: */
    ecs_flags32_t flags = h->flags;
    flags &= ~((ecs_flags32_t)ECS_TYPE_HOOKS);

    ecs_check(!(flags & ECS_TYPE_HOOK_CTOR_ILLEGAL && 
        h->ctor != NULL && 
        h->ctor != flecs_ctor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both ctor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_DTOR_ILLEGAL && 
        h->dtor != NULL && 
        h->dtor != flecs_dtor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both dtor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_COPY_ILLEGAL && 
        h->copy != NULL && 
        h->copy != flecs_copy_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both copy hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_MOVE_ILLEGAL && 
        h->move != NULL && 
        h->move != flecs_move_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both move hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL && 
        h->copy_ctor != NULL && 
        h->copy_ctor != flecs_copy_ctor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both copy ctor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL && 
        h->move_ctor != NULL && 
        h->move_ctor != flecs_move_ctor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both move ctor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL && 
        h->ctor_move_dtor != NULL && 
        h->ctor_move_dtor != flecs_move_ctor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both ctor move dtor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL && 
        h->move_dtor != NULL && 
        h->move_dtor != flecs_move_ctor_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both move dtor hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_CMP_ILLEGAL && 
        h->cmp != NULL && 
        h->cmp != flecs_comp_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both compare hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));

    ecs_check(!(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL && 
        h->equals != NULL && 
        h->equals != flecs_equals_illegal),
        ECS_INVALID_PARAMETER, "illegal call to set_hooks() for component '%s': "
            "cannot specify both equals hook and illegal flag",
                flecs_errstr(ecs_get_path(world, component)));


    flecs_stage_from_world(&world);

    /* Ensure that no tables have yet been created for the component */
    ecs_check( ecs_id_in_use(world, component) == false,
        ECS_ALREADY_IN_USE, ecs_get_name(world, component));
    ecs_check( ecs_id_in_use(world, ecs_pair(component, EcsWildcard)) == false,
        ECS_ALREADY_IN_USE, ecs_get_name(world, component));

    ecs_type_info_t *ti = flecs_type_info_ensure(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!ti->component || ti->component == component,
        ECS_INCONSISTENT_COMPONENT_ACTION, NULL);

    if (!ti->size) {
        const EcsComponent *component_ptr = ecs_get(
            world, component, EcsComponent);

        /* Cannot register lifecycle actions for things that aren't a component */
        ecs_check(component_ptr != NULL, ECS_INVALID_PARAMETER, 
            "illegal call to set_hooks() for '%s': component cannot be a tag/zero sized",
                flecs_errstr(ecs_get_path(world, component)));
        ecs_check(component_ptr->size != 0, ECS_INVALID_PARAMETER,
            "illegal call to set_hooks() for '%s': cannot register "
            " component cannot be a tag/zero sized",
                flecs_errstr(ecs_get_path(world, component)));

        ti->size = component_ptr->size;
        ti->alignment = component_ptr->alignment;
    }

    if (h->ctor) ti->hooks.ctor = h->ctor;
    if (h->dtor) ti->hooks.dtor = h->dtor;
    if (h->copy) ti->hooks.copy = h->copy;
    if (h->move) ti->hooks.move = h->move;
    if (h->copy_ctor) ti->hooks.copy_ctor = h->copy_ctor;
    if (h->move_ctor) ti->hooks.move_ctor = h->move_ctor;
    if (h->ctor_move_dtor) ti->hooks.ctor_move_dtor = h->ctor_move_dtor;
    if (h->move_dtor) ti->hooks.move_dtor = h->move_dtor;
    if (h->cmp) ti->hooks.cmp = h->cmp;
    if (h->equals) ti->hooks.equals = h->equals;

    if (h->on_add) ti->hooks.on_add = h->on_add;
    if (h->on_remove) ti->hooks.on_remove = h->on_remove;
    if (h->on_set) ti->hooks.on_set = h->on_set;
    if (h->on_replace) ti->hooks.on_replace = h->on_replace;

    if (h->ctx) ti->hooks.ctx = h->ctx;
    if (h->binding_ctx) ti->hooks.binding_ctx = h->binding_ctx;
    if (h->lifecycle_ctx) ti->hooks.lifecycle_ctx = h->lifecycle_ctx;
    if (h->ctx_free) ti->hooks.ctx_free = h->ctx_free;
    if (h->binding_ctx_free) ti->hooks.binding_ctx_free = h->binding_ctx_free;
    if (h->lifecycle_ctx_free) ti->hooks.lifecycle_ctx_free = h->lifecycle_ctx_free;

    /* If no constructor is set, invoking any of the other lifecycle actions
     * is not safe as they will potentially access uninitialized memory. For
     * ease of use, if no constructor is specified, set a default one that
     * initializes the component to 0. */
    if (!h->ctor && (h->dtor || h->copy || h->move)) {
        ti->hooks.ctor = flecs_default_ctor;   
    }

    /* Set default copy ctor, move ctor and merge */
    if (!h->copy_ctor && !(flags & ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL)) {
        if (h->copy) {
            ti->hooks.copy_ctor = flecs_default_copy_ctor;
        }
    }

    if (!h->move_ctor && !(flags & ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL)) {
        if (h->move) {
            ti->hooks.move_ctor = flecs_default_move_ctor;
        }
    }

    if (!h->ctor_move_dtor) {
        ecs_flags32_t illegal_check = 0;
        if (h->move) {
            illegal_check |= ECS_TYPE_HOOK_MOVE_ILLEGAL;
            if (h->dtor) {
                illegal_check |= ECS_TYPE_HOOK_DTOR_ILLEGAL;
                if (h->move_ctor) {
                    illegal_check |= ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL;
                    /* If an explicit move ctor has been set, use callback
                     * that uses the move ctor vs. using a ctor+move */
                    ti->hooks.ctor_move_dtor = flecs_default_move_ctor_w_dtor;
                } else {
                    illegal_check |= ECS_TYPE_HOOK_CTOR_ILLEGAL;
                    /* If no explicit move_ctor has been set, use
                     * combination of ctor + move + dtor */
                    ti->hooks.ctor_move_dtor = flecs_default_ctor_w_move_w_dtor;
                }
            } else {
                illegal_check |= ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL;
                /* If no dtor has been set, this is just a move ctor */
                ti->hooks.ctor_move_dtor = ti->hooks.move_ctor;
            }
        } else {
            /* If move is not set but move_ctor and dtor is, we can still set
             * ctor_move_dtor. */
            if (h->move_ctor) {
                illegal_check |= ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL;
                if (h->dtor) {
                    illegal_check |= ECS_TYPE_HOOK_DTOR_ILLEGAL;
                    ti->hooks.ctor_move_dtor = flecs_default_move_ctor_w_dtor;
                } else {
                    ti->hooks.ctor_move_dtor = ti->hooks.move_ctor;
                }
            }
        }
        if(flags & illegal_check) {
            flags |= ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL;
        }
    }

    if (!h->move_dtor) {
        ecs_flags32_t illegal_check = 0;
        if (h->move) {
            illegal_check |= ECS_TYPE_HOOK_MOVE_ILLEGAL;
            if (h->dtor) {
                illegal_check |= ECS_TYPE_HOOK_DTOR_ILLEGAL;
                ti->hooks.move_dtor = flecs_default_move_w_dtor;
            } else {
                ti->hooks.move_dtor = flecs_default_move;
            }
        } else {
            if (h->dtor) {
                illegal_check |= ECS_TYPE_HOOK_DTOR_ILLEGAL;
                ti->hooks.move_dtor = flecs_default_dtor;
            }
        }
        if(flags & illegal_check) {
            flags |= ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL;
        }
    }

    if(!h->cmp) {
        flags |= ECS_TYPE_HOOK_CMP_ILLEGAL;
    }

    if (!h->equals || h->equals == flecs_equals_illegal) {
        if(flags & ECS_TYPE_HOOK_CMP_ILLEGAL) {
            flags |= ECS_TYPE_HOOK_EQUALS_ILLEGAL;
        } else {
            ti->hooks.equals = flecs_default_equals;
            flags &= ~ECS_TYPE_HOOK_EQUALS_ILLEGAL;
        }
    }

    ti->hooks.flags = flags;

    if (ti->hooks.ctor) ti->hooks.flags |= ECS_TYPE_HOOK_CTOR;
    if (ti->hooks.dtor) ti->hooks.flags |= ECS_TYPE_HOOK_DTOR;
    if (ti->hooks.move) ti->hooks.flags |= ECS_TYPE_HOOK_MOVE;
    if (ti->hooks.move_ctor) ti->hooks.flags |= ECS_TYPE_HOOK_MOVE_CTOR;
    if (ti->hooks.ctor_move_dtor) ti->hooks.flags |= ECS_TYPE_HOOK_CTOR_MOVE_DTOR;
    if (ti->hooks.move_dtor) ti->hooks.flags |= ECS_TYPE_HOOK_MOVE_DTOR;
    if (ti->hooks.copy) ti->hooks.flags |= ECS_TYPE_HOOK_COPY;
    if (ti->hooks.copy_ctor) ti->hooks.flags |= ECS_TYPE_HOOK_COPY_CTOR;
    if (ti->hooks.cmp) ti->hooks.flags |= ECS_TYPE_HOOK_CMP;
    if (ti->hooks.equals) ti->hooks.flags |= ECS_TYPE_HOOK_EQUALS;

    if(flags & ECS_TYPE_HOOK_CTOR_ILLEGAL) ti->hooks.ctor = flecs_ctor_illegal;
    if(flags & ECS_TYPE_HOOK_DTOR_ILLEGAL) ti->hooks.dtor = flecs_dtor_illegal;
    if(flags & ECS_TYPE_HOOK_COPY_ILLEGAL) ti->hooks.copy = flecs_copy_illegal;
    if(flags & ECS_TYPE_HOOK_MOVE_ILLEGAL) ti->hooks.move = flecs_move_illegal;
    if(flags & ECS_TYPE_HOOK_CMP_ILLEGAL) ti->hooks.cmp = flecs_comp_illegal;
    if(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL) ti->hooks.equals = flecs_equals_illegal;

    if(flags & ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL) {
        ti->hooks.copy_ctor = flecs_copy_ctor_illegal;
    }

    if(ti->hooks.flags & ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL) {
        ti->hooks.move_ctor = flecs_move_ctor_illegal;
    }

    if(ti->hooks.flags & ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL) {
        ti->hooks.ctor_move_dtor = flecs_move_ctor_illegal;
    }

    if(ti->hooks.flags & ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL) {
        ti->hooks.move_dtor = flecs_move_ctor_illegal;
    }

    if (component < FLECS_HI_COMPONENT_ID) {
        if (ti->hooks.on_set || ti->hooks.copy || ti->hooks.move || ti->hooks.on_replace) {
            world->non_trivial_set[component] = true;
        }
    }

error:
    return;
}

static
void flecs_type_info_fini(
    ecs_type_info_t *ti)
{
    if (ti->hooks.ctx_free) {
        ti->hooks.ctx_free(ti->hooks.ctx);
    }
    if (ti->hooks.binding_ctx_free) {
        ti->hooks.binding_ctx_free(ti->hooks.binding_ctx);
    }
    if (ti->hooks.lifecycle_ctx_free) {
        ti->hooks.lifecycle_ctx_free(ti->hooks.lifecycle_ctx);
    }
    if (ti->name) {
        /* Safe to cast away const, world has ownership over string */
        ecs_os_free(ECS_CONST_CAST(char*, ti->name));
        ti->name = NULL;
    }

    ti->size = 0;
    ti->alignment = 0;
}

void flecs_fini_type_info(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(&world->type_info);
    while (ecs_map_next(&it)) {
        ecs_type_info_t *ti = ecs_map_ptr(&it);
        flecs_type_info_fini(ti);
        ecs_os_free(ti);
    }
    ecs_map_fini(&world->type_info);
}

const ecs_type_info_t* flecs_type_info_get(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    flecs_poly_assert(world, ecs_world_t);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ID_FLAGS_MASK), ECS_INTERNAL_ERROR, NULL);

    return ecs_map_get_deref(&world->type_info, ecs_type_info_t, component);
}

ecs_type_info_t* flecs_type_info_ensure(
    ecs_world_t *world,
    ecs_entity_t component)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    const ecs_type_info_t *ti = flecs_type_info_get(world, component);
    ecs_type_info_t *ti_mut = NULL;
    if (!ti) {
        ti_mut = ecs_map_ensure_alloc_t(
            &world->type_info, ecs_type_info_t, component);
        ecs_assert(ti_mut != NULL, ECS_INTERNAL_ERROR, NULL);
        ti_mut->component = component;
    } else {
        ti_mut = ECS_CONST_CAST(ecs_type_info_t*, ti);
    }

    if (!ti_mut->name) {
        const char *sym = ecs_get_symbol(world, component);
        if (sym) {
            ti_mut->name = ecs_os_strdup(sym);
        } else {
            const char *name = ecs_get_name(world, component);
            if (name) {
                ti_mut->name = ecs_os_strdup(name);
            }
        }
    }

    return ti_mut;
}

bool flecs_type_info_init_id(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_size_t size,
    ecs_size_t alignment,
    const ecs_type_hooks_t *li)
{
    bool changed = false;

    flecs_entities_ensure(world, component);

    ecs_type_info_t *ti = NULL;
    if (!size || !alignment) {
        ecs_assert(size == 0 && alignment == 0,
            ECS_INVALID_COMPONENT_SIZE, NULL);
        ecs_assert(li == NULL, ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_map_remove_free(&world->type_info, component);
    } else {
        ti = flecs_type_info_ensure(world, component);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        changed |= ti->size != size;
        changed |= ti->alignment != alignment;
        ti->size = size;
        ti->alignment = alignment;
        if (li) {
            ecs_set_hooks_id(world, component, li);
        }
    }

    /* Set type info for component record of component */
    ecs_component_record_t *cr = flecs_components_get(world, component);
    if (cr) {
        changed |= flecs_component_set_type_info(world, cr, ti);
    }

    bool is_tag = ecs_has_id(world, component, EcsPairIsTag);

    /* All id records with component as relationship inherit type info */
    cr = flecs_components_get(world, ecs_pair(component, EcsWildcard));
    if (cr) {
        while ((cr = flecs_component_first_next(cr))) {
            if (is_tag) {
                changed |= flecs_component_set_type_info(world, cr, NULL);
            } else if (ti) {
                changed |= flecs_component_set_type_info(world, cr, ti);
            } else if ((cr->type_info != NULL) &&
                (cr->type_info->component == component))
            {
                changed |= flecs_component_set_type_info(world, cr, NULL);
            }
        } 
    }

    /* All non-tag id records with component as object inherit type info,
     * if relationship doesn't have type info */
    cr = flecs_components_get(world, ecs_pair(EcsWildcard, component));
    if (cr) {
        while ((cr = flecs_component_second_next(cr))) {
            if (!(cr->flags & EcsIdPairIsTag) && !cr->type_info) {
                changed |= flecs_component_set_type_info(world, cr, ti);
            }
        }
    }

    return changed;
}

void flecs_type_info_free(
    ecs_world_t *world,
    ecs_entity_t component)
{
    flecs_poly_assert(world, ecs_world_t);

    if (world->flags & EcsWorldQuit) {
        /* If world is in the final teardown stages, cleanup policies are no
         * longer applied and it can't be guaranteed that a component is not
         * deleted before entities that use it. The remaining type info elements
         * will be deleted after the store is finalized. */
        return;
    }

    ecs_type_info_t *ti = ecs_map_get_deref(
        &world->type_info, ecs_type_info_t, component);
    if (ti) {
        flecs_type_info_fini(ti);
        ecs_map_remove_free(&world->type_info, component);
    }
}

ecs_size_t flecs_type_size(
    ecs_world_t *world, 
    ecs_entity_t type) 
{
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);
    return comp->size;
}

const ecs_type_hooks_t* ecs_get_hooks_id(
    const ecs_world_t *world,
    ecs_entity_t id)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    if (ti) {
        return &ti->hooks;
    }
    return NULL;
}

const ecs_type_info_t* flecs_determine_type_info_for_component(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (!ECS_IS_PAIR(id)) {
        if (!(id & ECS_ID_FLAGS_MASK)) {
            return flecs_type_info_get(world, id);
        }
    } else {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        rel = flecs_entities_get_alive(world, rel);
        ecs_assert(ecs_is_alive(world, rel), ECS_INTERNAL_ERROR, NULL);

        if (ecs_owns_id(world, rel, EcsPairIsTag)) {
            return NULL;
        }

        const ecs_type_info_t *ti = flecs_type_info_get(world, rel);
        if (ti) {
            return ti;
        }

        if (!ECS_IS_VALUE_PAIR(id)) {
            ecs_entity_t tgt = ecs_pair_second(world, id);
            if (tgt) {
                return flecs_type_info_get(world, tgt);
            }
        }
    }

    return NULL;
}

const ecs_type_info_t* ecs_get_type_info(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (cr) {
        return cr->type_info;
    } else {
        return flecs_determine_type_info_for_component(world, id);
    }
error:
    return NULL;
}
