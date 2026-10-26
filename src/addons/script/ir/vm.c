/**
 * @file addons/script/ir/vm.c
 * @brief Script IR virtual machine.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"
#include <inttypes.h>

#define flecs_ir_error(vm, node, ...)\
    flecs_script_eval_error_(&(vm)->v, (const ecs_script_node_t*)(node), __VA_ARGS__)

#define flecs_ir_expr_error(vm, node, ...)\
    flecs_expr_visit_error_(&(vm)->v.base.script->pub, node, __VA_ARGS__)

#define flecs_ir_reg(vm, index) (&(vm)->regs[(vm)->reg_base + (index)])

#ifdef FLECS_SCRIPT_IR_PROFILE
int64_t flecs_ir_profile_ops[EcsIrOpKindLast];
int64_t flecs_ir_profile_stats[EcsIrProfileLast];
#define flecs_ir_prof(kind) flecs_ir_profile_stats[kind] ++
#else
#define flecs_ir_prof(kind)
#endif

typedef struct flecs_ir_owned_t {
    void *ptr;
    const ecs_type_info_t *ti;
    int32_t reg;
} flecs_ir_owned_t;

#define FLECS_IR_SCRATCH_SIZE (64 * 1024)

static void flecs_ir_regs_ensure(
    ecs_script_ir_vm_t *vm,
    int32_t count)
{
    if (count <= vm->reg_count) {
        return;
    }

    int32_t new_count = count < 16 ? 16 : count;
    vm->regs = ecs_os_realloc_n(vm->regs, ecs_script_ir_reg_t, new_count);
    ecs_os_memset(&vm->regs[vm->reg_count], 0,
        (new_count - vm->reg_count) * ECS_SIZEOF(ecs_script_ir_reg_t));
    vm->reg_count = new_count;
}

static void* flecs_ir_arena_alloc(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_arena_t *arena,
    int32_t size,
    int32_t align)
{
    if (!arena->data) {
        int32_t initial = vm->can_suspend ? 4096 : FLECS_IR_SCRATCH_SIZE;
        arena->size = size > initial ? size : initial;
        arena->data = ecs_os_malloc(arena->size);
    }

    int32_t top = (arena->top + (align - 1)) & ~(align - 1);
    if (top + size <= arena->size) {
        arena->top = top + size;
        return arena->data + top;
    }

    void *ptr = ecs_os_malloc(size);
    ecs_vec_append_t(NULL, &arena->heap, void*)[0] = ptr;
    return ptr;
}

static void flecs_ir_arena_restore(
    ecs_script_ir_arena_t *arena,
    int32_t top,
    int32_t heap_count)
{
    void **heap = arena->heap.array;
    for (int32_t i = heap_count; i < arena->heap.count; i ++) {
        ecs_os_free(heap[i]);
    }

    arena->heap.count = heap_count;
    arena->top = top;
}

static void flecs_ir_arena_init(
    ecs_script_ir_arena_t *arena)
{
    arena->data = NULL;
    arena->size = 0;
    arena->top = 0;
    ecs_vec_init_t(NULL, &arena->heap, void*, 0);
}

static void flecs_ir_arena_fini(
    ecs_script_ir_arena_t *arena)
{
    ecs_vec_fini_t(NULL, &arena->heap, void*);
    ecs_os_free(arena->data);
    arena->data = NULL;
}

static inline ecs_script_var_t* flecs_ir_var_declare(
    ecs_script_ir_vm_t *vm)
{
    ecs_script_vars_t *vars = vm->v.vars;
    ecs_script_var_t *var;
    if (vars->vars.count < vars->vars.size) {
        var = &((ecs_script_var_t*)vars->vars.array)[vars->vars.count ++];
    } else {
        var = ecs_vec_append_t(vars->allocator, &vars->vars, ecs_script_var_t);
    }

    var->name = NULL;
    var->value.ptr = NULL;
    var->value.type = 0;
    var->type_info = NULL;
    var->sp = vars->vars.count + vars->sp - 1;
    var->is_const = false;
    var->owned = false;
    return var;
}

static void* flecs_ir_var_alloc(
    ecs_script_ir_vm_t *vm,
    const ecs_type_info_t *ti)
{
    void *ptr = flecs_ir_arena_alloc(
        vm, &vm->vscratch, ti->size, ti->alignment);
    if (ti->hooks.ctor) {
        ti->hooks.ctor(ptr, 1, ti);
    } else {
        ecs_os_memset(ptr, 0, ti->size);
    }

    return ptr;
}

static void* flecs_ir_reg_alloc_w_init(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_reg_t *reg,
    const ecs_type_info_t *ti,
    bool init)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    void *ptr = flecs_ir_arena_alloc(
        vm, &vm->scratch, ti->size, ti->alignment);
    if (init) {
        if (ti->hooks.ctor) {
            ti->hooks.ctor(ptr, 1, ti);
        } else {
            ecs_os_memset(ptr, 0, ti->size);
        }
    }

    if (ti->hooks.dtor) {
        flecs_ir_owned_t *o = ecs_vec_append_t(
            NULL, &vm->owned, flecs_ir_owned_t);
        o->ptr = ptr;
        o->ti = ti;
        o->reg = (int32_t)(reg - vm->regs);
    }

    reg->value.ptr = ptr;
    reg->value.type = ti->component;
    reg->ti = ti;
    reg->storage = ptr;
    reg->owned = true;
    return ptr;
}

static void* flecs_ir_reg_alloc(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_reg_t *reg,
    const ecs_type_info_t *ti)
{
    return flecs_ir_reg_alloc_w_init(vm, reg, ti, true);
}

static void* flecs_ir_reg_alloc_raw(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_reg_t *reg,
    const ecs_type_info_t *ti)
{
    return flecs_ir_reg_alloc_w_init(vm, reg, ti, false);
}

static void* flecs_ir_reg_out(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_script_ir_reg_t *reg)
{
    const ecs_expr_node_t *node = op->node;
    if (op->flags & EcsIrInPlace) {
        ecs_assert(reg->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        reg->value.type = node->type;
        reg->ti = node->type_info;
        return reg->value.ptr;
    }

    return flecs_ir_reg_alloc(vm, reg, node->type_info);
}

static inline void* flecs_ir_reg_out_raw(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_script_ir_reg_t *reg)
{
    const ecs_expr_node_t *node = op->node;
    const ecs_type_info_t *ti = node->type_info;
    if (op->flags & EcsIrInPlace) {
        ecs_assert(reg->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        reg->value.type = node->type;
        reg->ti = ti;
        return reg->value.ptr;
    }

    if (!ti->hooks.dtor && ti->size <= 16) {
        int32_t top = (vm->scratch.top + 15) & ~15;
        if (top + 16 <= vm->scratch.size) {
            void *out = vm->scratch.data + top;
            vm->scratch.top = top + 16;
            reg->value.ptr = out;
            reg->value.type = node->type;
            reg->ti = ti;
            reg->storage = out;
            reg->owned = true;
            return out;
        }
    }

    return flecs_ir_reg_alloc_raw(vm, reg, ti);
}

static void flecs_ir_reg_borrow(
    ecs_script_ir_reg_t *reg,
    const ecs_expr_node_t *node,
    ecs_entity_t type,
    void *ptr)
{
    reg->value.type = type;
    reg->value.ptr = ptr;
    reg->ti = node->type_info;
    reg->owned = false;
}

static ecs_script_ir_frame_t* flecs_ir_frame_at(
    const ecs_script_ir_vm_t *vm,
    int32_t frame_index)
{
    uint32_t index = flecs_ito(uint32_t, frame_index);
    return &vm->frames[index / ECS_SCRIPT_IR_FRAME_CHUNK_SIZE]
        [index % ECS_SCRIPT_IR_FRAME_CHUNK_SIZE];
}

static ecs_script_ir_frame_t* flecs_ir_frame_push(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_kind_t kind,
    int32_t pc)
{
    ecs_assert(vm->frame_count < ECS_SCRIPT_IR_MAX_FRAMES,
        ECS_INTERNAL_ERROR, NULL);
    uint32_t chunk = (uint32_t)vm->frame_count / ECS_SCRIPT_IR_FRAME_CHUNK_SIZE;
    if (!vm->frames[chunk]) {
        vm->frames[chunk] = ecs_os_malloc_n(
            ecs_script_ir_frame_t, ECS_SCRIPT_IR_FRAME_CHUNK_SIZE);
    }

    ecs_script_ir_frame_t *frame = flecs_ir_frame_at(vm, vm->frame_count ++);
    frame->kind = (int16_t)kind;
    frame->state = 0;
    frame->pc = pc;
    return frame;
}

static ecs_script_ir_frame_t* flecs_ir_frame_top(
    ecs_script_ir_vm_t *vm)
{
    ecs_assert(vm->frame_count > 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_ir_frame_at(vm, vm->frame_count - 1);
}

static void flecs_ir_expr_release(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    int32_t owned_count = vm->owned.count;
    if (owned_count > frame->u.expr.owned_count) {
        flecs_ir_owned_t *owned = ecs_vec_first(&vm->owned);
        int32_t i;
        for (i = owned_count - 1; i >= frame->u.expr.owned_count; i --) {
            ecs_script_ir_reg_t *reg = &vm->regs[owned[i].reg];
            if (reg->owned && reg->storage == owned[i].ptr) {
                flecs_type_info_dtor(owned[i].ptr, 1, owned[i].ti);
                reg->owned = false;
            }
        }

        ecs_vec_set_count_t(NULL, &vm->owned, flecs_ir_owned_t,
            frame->u.expr.owned_count);
    }

    flecs_ir_arena_restore(&vm->scratch,
        frame->u.expr.scratch_top, frame->u.expr.heap_count);

    int32_t strbuf_count = vm->strbufs.count;
    if (strbuf_count > frame->u.expr.strbuf_count) {
        while (strbuf_count > frame->u.expr.strbuf_count) {
            ecs_strbuf_reset(ecs_vec_get_t(
                &vm->strbufs, ecs_strbuf_t, strbuf_count - 1));
            strbuf_count --;
        }
        ecs_vec_set_count_t(NULL, &vm->strbufs, ecs_strbuf_t, strbuf_count);
    }

    if (vm->cursors.count != frame->u.expr.cursor_count) {
        ecs_vec_set_count_t(NULL, &vm->cursors, ecs_meta_cursor_t,
            frame->u.expr.cursor_count);
    }
}

static void flecs_ir_expr_begin(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    int32_t pc)
{
    ecs_script_ir_frame_t *frame = flecs_ir_frame_push(vm, EcsIrFrameExpr, pc);
    frame->u.expr.scratch_top = vm->scratch.top;
    frame->u.expr.owned_count = vm->owned.count;
    frame->u.expr.heap_count = vm->scratch.heap.count;
    frame->u.expr.strbuf_count = vm->strbufs.count;
    frame->u.expr.cursor_count = vm->cursors.count;
    if (op->node && ((const ecs_script_node_t*)op->node)->kind == EcsAstConst) {
        frame->state = 1;
    }
}

static void flecs_ir_expr_end(
    ecs_script_ir_vm_t *vm)
{
    ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
    ecs_assert(frame->kind == EcsIrFrameExpr, ECS_INTERNAL_ERROR, NULL);
    flecs_ir_expr_release(vm, frame);
    vm->frame_count --;
}

static void flecs_ir_vars_truncate(
    ecs_script_ir_vm_t *vm,
    int32_t count,
    int32_t vscratch_top,
    int32_t vheap_count)
{
    ecs_script_vars_t *vars = vm->v.vars;
    int32_t i, cur = vars->vars.count;
    if (cur > count) {
        ecs_script_var_t *array = vars->vars.array;
        for (i = count; i < cur; i ++) {
            ecs_script_var_t *var = &array[i];
            if (var->owned && var->value.ptr && var->type_info &&
                var->type_info->hooks.dtor)
            {
                flecs_type_info_dtor(var->value.ptr, 1, var->type_info);
            }
        }

        vars->vars.count = count;
    }

    flecs_ir_arena_restore(&vm->vscratch, vscratch_top, vheap_count);
}

static void flecs_ir_scope_leave(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    if (frame->u.scope.var_count != -1) {
        flecs_ir_vars_truncate(vm, frame->u.scope.var_count,
            frame->u.scope.vscratch_top, frame->u.scope.vheap_count);
    }

    v->parent = frame->u.scope.state.parent;
    v->scope_slot = frame->u.scope.state.scope_slot;
}

static void flecs_ir_for_leave(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    flecs_script_for_state_t *state = &frame->u.for_.state;
    flecs_ir_vars_truncate(vm, state->var_base,
        frame->u.for_.vscratch_top, frame->u.for_.vheap_count);
    vm->v.for_slot = state->for_slot;
    vm->v.force = state->force;
}

static void flecs_ir_block_leave(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    if (frame->u.block.vars) {
        flecs_ir_vars_truncate(vm, 0, 0, 0);
        vm->v.vars = ecs_script_vars_pop(vm->v.vars);
        frame->u.block.vars = false;
    }
}

static void flecs_ir_frame_leave(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    switch(frame->kind) {
    case EcsIrFrameScope:
        flecs_ir_scope_leave(vm, frame);
        break;
    case EcsIrFrameEntity:
        flecs_script_eval_entity_leave(v, &frame->u.entity);
        break;
    case EcsIrFrameWith:
        flecs_script_eval_with_leave(v, &frame->u.with);
        break;
    case EcsIrFramePairScope:
        flecs_script_eval_pair_scope_leave(v, &frame->u.pair_scope);
        break;
    case EcsIrFrameIf:
        v->force = frame->u.if_.force;
        break;
    case EcsIrFrameFor:
        flecs_ir_for_leave(vm, frame);
        break;
    case EcsIrFrameTry:
    case EcsIrFrameWhile:
        break;
    case EcsIrFrameBlock:
        flecs_ir_block_leave(vm, frame);
        break;
    case EcsIrFrameExpr:
        if (frame->state == 1) {
            const ecs_script_ir_op_t *op = &vm->ops[frame->pc];
            const ecs_script_var_node_t *node = op->node;
            flecs_ir_error(vm, node,
                "failed to evaluate expression for const variable '%s'",
                node->name);
        }
        flecs_ir_expr_release(vm, frame);
        break;
    }
}

static void flecs_ir_mark(
    ecs_script_ir_vm_t *vm,
    int32_t index);

static void flecs_ir_set_dirty(
    ecs_script_ir_vm_t *vm)
{
    if (vm->dirty) {
        return;
    }

    vm->dirty = true;
    int32_t i, count = vm->pending_marks.count;
    const int32_t *marks = vm->pending_marks.array;
    for (i = 0; i < count; i ++) {
        flecs_ir_mark(vm, marks[i]);
    }

    vm->pending_marks.count = 0;
}

static inline bool flecs_ir_stmt_runs(
    const ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_script_eval_visitor_t *v = &vm->v;
    return !(op->flags & EcsIrStmtSkip) && (v->force ||
        (op->flags & EcsIrStmtAlways) || (op->imm.u64 & v->input) ||
        (v->internal && (v->internal &
            ((const ecs_script_node_t*)op->node)->internal)));
}

static inline void flecs_ir_stmt_skip(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    flecs_ir_prof(EcsIrProfileStmtSkipped);
    if (op->c != -1) {
        const ecs_script_region_t *region = ecs_vec_get_t(
            &vm->v.base.script->regions, ecs_script_region_t, op->c);
        if (vm->dirty || region->for_count) {
            flecs_ir_mark(vm, op->c);
        } else {
            ecs_vec_append_t(NULL, &vm->pending_marks, int32_t)[0] = op->c;
        }
    }
}

static void flecs_ir_mark(
    ecs_script_ir_vm_t *vm,
    int32_t index)
{
    if (index < 0) {
        const ecs_script_ir_op_t *block = &vm->ops[-index - 1];
        for (int32_t pc = -index; pc < block->b; pc = vm->ops[pc].b) {
            int32_t marks = vm->ops[pc].c;
            if (marks != -1) {
                flecs_ir_mark(vm, marks);
            }
        }

        return;
    }

    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_region_t *region = ecs_vec_get_t(
        &v->base.script->regions, ecs_script_region_t, index);
    flecs_script_state_mark(v->state, region, v->visit);
}

static ecs_entity_t flecs_ir_lookup_name(
    ecs_script_ir_vm_t *vm,
    const char *name)
{
    return flecs_script_lookup_name(&vm->v, name);
}

static const char* flecs_ir_name(
    ecs_script_ir_vm_t *vm,
    int32_t reg)
{
    return reg != -1 ? *(char**)flecs_ir_reg(vm, reg)->value.ptr : NULL;
}

static inline int flecs_ir_id_get(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const void *node,
    ecs_id_t *out)
{
    if (!desc->dynamic) {
        *out = desc->eval;
        return 0;
    }

    if (desc->first_reg == -1 && desc->first_sp == -1 &&
        desc->index_reg == -1)
    {
        ecs_entity_t first = desc->first_symbol != -1
            ? flecs_script_symbol_entity(&vm->v, desc->first_symbol)
            : desc->first_eval;
        if (first && first != EcsWildcard && first != EcsAny) {
            if (!desc->has_second) {
                *out = desc->flag | first;
                return 0;
            }

            if (desc->second_reg == -1 && desc->second_sp == -1) {
                ecs_entity_t second = desc->second_symbol != -1
                    ? flecs_script_symbol_entity(&vm->v, desc->second_symbol)
                    : desc->second_eval;
                if (second && second != EcsWildcard && second != EcsAny) {
                    *out = desc->flag | ecs_pair(first, second);
                    return 0;
                }
            }
        }
    }

    return flecs_script_eval_id_resolve(&vm->v, node, desc->id,
        flecs_ir_name(vm, desc->first_reg), flecs_ir_name(vm, desc->second_reg),
        desc->index_reg != -1
            ? *(int32_t*)flecs_ir_reg(vm, desc->index_reg)->value.ptr : 0,
        out);
}

static const ecs_script_ir_id_t* flecs_ir_id_desc(
    ecs_script_ir_vm_t *vm,
    int32_t index)
{
    return ecs_vec_get_t(&vm->ir->ids, ecs_script_ir_id_t, index);
}

static void flecs_ir_id_cache(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    ecs_id_t id)
{
    ecs_script_ir_id_t *mut = ECS_CONST_CAST(ecs_script_ir_id_t*, desc);
    mut->cache_id = id;
    mut->cache_visitor = !ECS_IS_PAIR(id) &&
        ecs_has(vm->v.world, id, EcsScriptVisitor);
    ecs_component_record_t *cr = flecs_components_get(vm->v.world, id);
    mut->cache_ti = cr ? cr->type_info : NULL;
}

static const ecs_type_info_t* flecs_ir_id_type_info(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const void *node,
    ecs_id_t id)
{
    if (desc->ti) {
        return desc->ti;
    }

    if (desc->cache_id != id) {
        flecs_ir_id_cache(vm, desc, id);
    }

    if (desc->cache_ti) {
        return desc->cache_ti;
    }

    return flecs_script_get_type_info(&vm->v, node, id);
}

static int flecs_ir_tag(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, op->node, &id)) {
        return -1;
    }

    flecs_ir_prof(EcsIrProfileTagAdd);
    return flecs_script_eval_tag_id(&vm->v, op->node, id, desc->resolved);
}

static int flecs_ir_component_visitor(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    bool *handled)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;
    *handled = false;

    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    const EcsScriptVisitor *visitor = NULL;
    if (op->flags & EcsIrVisitorStatic) {
        visitor = ecs_get(v->world, id, EcsScriptVisitor);
    } else if (!ECS_IS_PAIR(id)) {
        if (node->id.interface && !ecs_is_alive(v->world, id)) {
            return 0;
        }

        if (desc->cache_id != id) {
            flecs_ir_id_cache(vm, desc, id);
        }

        if (desc->cache_visitor) {
            visitor = ecs_get(v->world, id, EcsScriptVisitor);
        }
    }

    if (!visitor) {
        return 0;
    }

    v->base.nodes[0] = ECS_CONST_CAST(ecs_script_node_t*, (const void*)node);
    v->base.depth = 1;
    int result = flecs_script_eval_component_visit(v, node, id, visitor);
    v->base.depth = 0;
    *handled = true;
    return result;
}

static int flecs_ir_component_begin(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_id_t *id_out)
{
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;

    if (node->id.interface && !node->id.index_expr) {
        if (flecs_script_eval_interface_id(&vm->v, node, &node->id, id_out)) {
            return -1;
        }
    } else if (flecs_ir_id_get(vm, desc, node, id_out)) {
        return -1;
    }

    return flecs_script_eval_component_check(
        &vm->v, node, *id_out, desc->resolved);
}

static int flecs_ir_init_type_check(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const ecs_script_component_t *node,
    const ecs_type_info_t *ti)
{
    if (!desc->ti && node->expr && ti->component != node->expr->type &&
        (node->expr->kind == EcsExprInitializer ||
         node->expr->kind == EcsExprEmptyInitializer) &&
        !flecs_struct_is_derived_from(vm->v.world, ti->component,
            node->expr->type))
    {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        return -1;
    }

    return 0;
}

static int flecs_ir_component_alloc(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;

    ecs_id_t id;
    if (flecs_ir_component_begin(vm, op, &id)) {
        return -1;
    }

    const ecs_type_info_t *ti = flecs_ir_id_type_info(vm, desc, node, id);
    if (!ti) {
        return -1;
    }

    if (flecs_ir_init_type_check(vm, desc, node, ti)) {
        return -1;
    }

    ecs_script_ir_reg_t *tmp = flecs_ir_reg(vm, op->b);
    void *ptr = flecs_ir_reg_alloc(vm, tmp, ti);

    if (node->id.interface && v->id_ref && v->id_ref->value) {
        if (flecs_script_template_ref_apply(v->world, v->id_ref, ti, ptr)) {
            return -1;
        }
    } else if (op->flags & EcsIrComponentPartial) {
        ecs_entity_t src = flecs_script_get_src(v->world, v->entity->eval, id);
        const void *existing = ecs_get_id(v->world, src, id);
        if (existing) {
            ecs_ptr_copy_w_type_info(v->world, ti, ptr, existing);
        }
    }

    return 0;
}

static int flecs_ir_value_to(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_reg_t *src,
    ecs_entity_t dst_type,
    void *dst_ptr,
    const ecs_type_info_t *dst_ti)
{
    ecs_world_t *world = vm->v.world;
    if (src->value.ptr == dst_ptr) {
        return 0;
    }

    if (src->value.type == dst_type) {
        if (src->owned) {
            flecs_type_info_move_dtor(dst_ptr, src->value.ptr, 1, dst_ti);
            src->owned = false;
        } else {
            flecs_type_info_copy(dst_ptr, src->value.ptr, 1, dst_ti);
        }

        return 0;
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst_type, dst_ptr);
    if (ecs_meta_set_value(&cur, &src->value)) {
        return -1;
    }

    return 0;
}

static int flecs_ir_component_end(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;

    ecs_id_t id;
    if (op->b == -1) {
        if (flecs_ir_component_begin(vm, op, &id)) {
            return -1;
        }

        return flecs_script_eval_component_add(v, node, id);
    }

    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    ecs_script_ir_reg_t *tmp = flecs_ir_reg(vm, op->c);
    const ecs_type_info_t *ti = tmp->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    void *value = tmp->storage;
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);

    {
        ecs_script_ir_reg_t *val = flecs_ir_reg(vm, op->b);
        if (flecs_ir_value_to(vm, val, ti->component, value, ti)) {
            flecs_ir_expr_error(vm, node->expr, "failed to write to output");
            return -1;
        }
    }

    ecs_entity_t src = flecs_script_get_src(v->world, v->entity->eval, id);
    flecs_ir_prof(EcsIrProfileComponentSet);
    if (desc->needs_set || (!desc->ti && ti->hooks.on_replace)) {
        ecs_set_id(v->world, src, id, flecs_itosize(ti->size), value);
    } else {
        void *dst = ecs_ensure_id(v->world, src, id, flecs_itosize(ti->size));
        flecs_type_info_move_dtor(dst, value, 1, ti);
        tmp->owned = false;
        ecs_modified_id(v->world, src, id);
    }

    flecs_script_track_component(v, desc->component_slot, id);
    return 0;
}

static int flecs_ir_with_tag(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_id_t id;
    if (flecs_ir_id_get(vm, flecs_ir_id_desc(vm, op->a), op->node, &id)) {
        return -1;
    }

    return flecs_script_eval_with_tag_id(&vm->v, op->node, id);
}

static int flecs_ir_with_component_begin(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;
    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    const ecs_type_info_t *ti = desc->ti;
    if (!ti) {
        ti = flecs_script_get_type_info(v, node, id);
    }

    ecs_value_t *value = flecs_script_with_append(v, ti);
    value->type = id;
    value->ptr = NULL;

    if (op->b == -1) {
        return 0;
    }

    if (!ti) {
        return -1;
    }

    if (flecs_ir_init_type_check(vm, desc, node, ti)) {
        return -1;
    }

    value->ptr = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
    flecs_type_info_ctor(value->ptr, 1, ti);

    if (node->id.interface && flecs_script_template_ref_apply(
        v->world, v->id_ref, ti, value->ptr))
    {
        return -1;
    }

    ecs_script_ir_reg_t *tmp = flecs_ir_reg(vm, op->b);
    tmp->value.type = ti->component;
    tmp->value.ptr = value->ptr;
    tmp->ti = ti;
    tmp->owned = false;
    return 0;
}

static int flecs_ir_with_component_end(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_component_t *node = op->node;
    ecs_script_ir_reg_t *tmp = flecs_ir_reg(vm, op->c);
    const ecs_type_info_t *ti = tmp->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_value_t *with_value = &ecs_vec_last_t(
        &v->r->with, ecs_script_with_value_t)->value;

    ecs_script_ir_reg_t *val = flecs_ir_reg(vm, op->b);
    if (flecs_ir_value_to(vm, val, ti->component, with_value->ptr, ti)) {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        return -1;
    }

    return 0;
}

static int flecs_ir_reg_to_i32(
    ecs_script_ir_vm_t *vm,
    int32_t index,
    int32_t *out)
{
    ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, index);
    if (reg->value.type == ecs_id(ecs_i32_t)) {
        *out = *(int32_t*)reg->value.ptr;
        return 0;
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        vm->v.world, ecs_id(ecs_i32_t), out);
    return ecs_meta_set_value(&cur, &reg->value);
}

static int flecs_ir_for_enter(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    flecs_script_for_state_t *state)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_for_t *node = ECS_CONST_CAST(
        ecs_script_for_t*, (const void*)op->node);
    const ecs_value_t *collection = NULL;

    flecs_script_for_prepare(v, node, state);

    if (node->expr) {
        collection = &flecs_ir_reg(vm, op->a)->value;
    } else if (flecs_ir_reg_to_i32(vm, op->a, &state->from)) {
        flecs_ir_expr_error(vm, node->from, "failed to write to output");
        return -1;
    } else if (flecs_ir_reg_to_i32(vm, op->b, &state->count)) {
        flecs_ir_expr_error(vm, node->to, "failed to write to output");
        return -1;
    }

    if (flecs_script_for_init(v, node, state, collection)) {
        return -1;
    }

    state->vars = v->vars;
    state->var_base = v->vars->vars.count;

    int32_t i;
    for (i = 0; i < node->loop_var_count; i ++) {
        ecs_script_var_t *var = flecs_ir_var_declare(vm);
        var->value.type = state->var_types[i];
        if (i != state->elem_var || state->kind == FlecsScriptForRange) {
            const ecs_type_info_t *ti = ecs_get_type_info(
                v->world, var->value.type);
            ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
            var->value.ptr = flecs_ir_var_alloc(vm, ti);
            var->type_info = ti;
            var->owned = true;
        }
    }

    v->for_slot = node->for_slot;
    v->force = true;
    return 0;
}

static int flecs_ir_const_end(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_var_node_t *node = op->node;
    ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);

    ecs_entity_t type = node->eval_type;
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = op->imm.ptr;
    if (!ti) {
        ti = ecs_get_type_info(v->world, type);
    }

    if (!ti) {
        frame->state = 0;
        flecs_ir_error(vm, node,
            "invalid type %s for variable '%s'",
            flecs_errstr(ecs_get_path(v->world, type)), node->name);
        return -1;
    }

    ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);

    if (op->b >= 0 && op->b < ecs_vec_count(&v->state->computed)) {
        ecs_script_computed_t *slot = ecs_vec_get_t(
            &v->state->computed, ecs_script_computed_t, op->b);
        const void *src = reg->value.ptr;
        void *tmp = NULL;
        if (reg->value.type != type) {
            tmp = flecs_ir_var_alloc(vm, ti);
            if (flecs_ir_value_to(vm, reg, type, tmp, ti)) {
                frame->state = 0;
                flecs_ir_expr_error(vm, node->expr,
                    "failed to write to output");
                return -1;
            }

            src = tmp;
        }

        flecs_script_computed_store(v, slot, op->b, ti, src);
        if (tmp && ti->hooks.dtor) {
            flecs_type_info_dtor(tmp, 1, ti);
        }

        frame->state = 0;
        flecs_script_const_var_init(
            flecs_ir_var_declare(vm), ti, type, slot->ptr, false);
        return 0;
    }

    void *ptr = flecs_ir_var_alloc(vm, ti);
    if (reg->value.type == type && !ti->hooks.copy && !ti->hooks.move) {
        if (ti->size == 8) {
            ecs_os_memcpy(ptr, reg->value.ptr, 8);
        } else if (ti->size == 4) {
            ecs_os_memcpy(ptr, reg->value.ptr, 4);
        } else if (ti->size == 1) {
            *(uint8_t*)ptr = *(uint8_t*)reg->value.ptr;
        } else {
            ecs_os_memcpy(ptr, reg->value.ptr, ti->size);
        }
    } else if (flecs_ir_value_to(vm, reg, type, ptr, ti)) {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        if (ti->hooks.dtor) {
            flecs_type_info_dtor(ptr, 1, ti);
        }

        return -1;
    }

    frame->state = 0;
    flecs_script_const_var_init(flecs_ir_var_declare(vm), ti, type, ptr, true);
    return 0;
}

static int flecs_ir_annot(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_annot_t *node = op->node;
    if (op->flags & EcsIrAnnotNoTarget) {
        flecs_ir_error(vm, node,
            "missing target for @%s annotation", node->name);
        return -1;
    }

    if (op->flags & EcsIrAnnotBadTarget) {
        flecs_ir_error(vm, node,
            "target of @%s annotation must be an entity or template",
            node->name);
        return -1;
    }

    ecs_vec_append_t(v->r->allocator, &v->r->annot,
        ecs_script_annot_t*)[0] = ECS_CONST_CAST(
            ecs_script_annot_t*, node);
    return 0;
}

static int flecs_ir_load_symbol(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_entity_t entity)
{
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    if (entity && (op->flags & EcsIrIdDirect)) {
        *(ecs_entity_t*)flecs_ir_reg_out_raw(vm, op, dst) = entity;
        return 0;
    }

    flecs_ir_reg_out(vm, op, dst);
    return flecs_expr_identifier_set(
        &vm->v.base.script->pub, op->node, entity, &dst->value);
}

static int flecs_ir_assign(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_assign_t *node = ECS_CONST_CAST(
        ecs_script_assign_t*, (const void*)op->node);
    ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
    const ecs_type_info_t *ti = ecs_get_type_info(v->world, node->eval_type);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    const void *src = reg->value.ptr;
    void *tmp = NULL;
    if (reg->value.type != node->eval_type) {
        tmp = ecs_ptr_new_w_type_info(v->world, ti);
        if (flecs_ir_value_to(vm, reg, node->eval_type, tmp, ti)) {
            flecs_ir_expr_error(vm, node->expr,
                "failed to evaluate expression for assignment to '%s'",
                node->name);
            ecs_ptr_free_w_type_info(v->world, ti, tmp);
            return -1;
        }

        src = tmp;
    }

    int result = flecs_script_assign_value(v, node, src);
    if (tmp) {
        ecs_ptr_free_w_type_info(v->world, ti, tmp);
    }

    return result;
}

static int flecs_ir_load_var(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_variable_t *node = op->node;
    const ecs_script_vars_t *vars = vm->v.vars;
    const ecs_script_var_t *var = NULL;
    if (vars) {
        int32_t sp = op->b;
        if (sp >= vars->sp && (sp - vars->sp) < vars->vars.count) {
            var = &((ecs_script_var_t*)vars->vars.array)[sp - vars->sp];
        } else {
            var = ecs_script_vars_from_sp(vars, sp);
        }
    }

    if (!var || !var->value.ptr) {
        flecs_expr_var_check(&vm->v.base.script->pub, node, var);
        return -1;
    }

    ecs_assert(var->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), &node->node,
        var->value.type, var->value.ptr);
    return 0;
}

static int flecs_ir_load_global(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *node = op->node;
    ecs_id_t component = (op->flags & EcsIrGlobalMut)
        ? ecs_id(EcsScriptMutVar)
        : ecs_id(EcsScriptConstVar);
    const EcsScriptConstVar *ptr = ecs_get_id(
        vm->v.world, op->imm.entity, component);
    ecs_value_t value;
    if (ptr) {
        value = ptr->value;
    } else {
        value = flecs_script_global_var_get(vm->v.world, op->imm.entity, NULL);
    }

    ecs_assert(value.type == node->type, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), node, value.type, value.ptr);
    return 0;
}

static int flecs_ir_element(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_element_t *node = op->node;
    ecs_type_kind_t kind = EcsArrayType;
    if (op->flags & EcsIrElementMap) {
        kind = EcsMapType;
    } else if (op->flags & EcsIrElementVector) {
        kind = EcsVectorType;
    }

    void *ptr;
    if (flecs_expr_element_get(&vm->v.base.script->pub, node, kind,
        &flecs_ir_reg(vm, op->b)->value, &flecs_ir_reg(vm, op->c)->value,
        &ptr))
    {
        return -1;
    }

    flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), &node->node,
        node->node.type, ptr);
    return 0;
}

static int flecs_ir_component_get(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_element_t *node = op->node;
    const void *ptr = flecs_expr_component_get(&vm->v.base.script->pub,
        &vm->v, node, *(ecs_entity_t*)flecs_ir_reg(vm, op->b)->value.ptr,
        op->imm.entity);
    if (!ptr) {
        return -1;
    }

    flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), &node->node,
        node->node.type, ECS_CONST_CAST(void*, ptr));
    return 0;
}

static int flecs_ir_has(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_entity_t entity = *(ecs_entity_t*)flecs_ir_reg(vm, op->b)->value.ptr;
    bool *out = flecs_ir_reg_out_raw(vm, op, dst);
    *out = flecs_expr_has_eval(
        &vm->v.base.script->pub, &vm->v, op->node, entity);
    return 0;
}

static int flecs_ir_binary(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_binary_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->b);
    ecs_script_ir_reg_t *right = flecs_ir_reg(vm, op->c);
    ecs_token_kind_t operator =
        (ecs_token_kind_t)(op->flags & ~EcsIrBinaryInPlace);
    void *out;
    if (op->flags & EcsIrBinaryInPlace) {
        out = dst->value.ptr;
        dst->value.type = node->node.type;
        dst->ti = node->node.type_info;
    } else {
        out = flecs_ir_reg_alloc_raw(vm, dst, node->node.type_info);
    }

    return flecs_expr_binary_eval(&vm->v.base.script->pub, node,
        &left->value, &right->value, out, operator);
}

#define FLECS_IR_INTEGER_OPS(T)\
    case EcsTokMod:\
        if (!r) {\
            flecs_ir_expr_error(vm, node, "division by zero");\
            return -1;\
        }\
        *(T*)out = (T)(l % r); return 0;\
    case EcsTokBitwiseAnd: *(T*)out = (T)(l & r); return 0;\
    case EcsTokBitwiseOr: *(T*)out = (T)(l | r); return 0;\
    case EcsTokShiftLeft: *(T*)out = (T)(l << r); return 0;\
    case EcsTokShiftRight: *(T*)out = (T)(l >> r); return 0;

#define FLECS_IR_FLOAT_OPS(T)

#define FLECS_IR_BINARY_TYPED(NAME, T, W, CMP_ONLY_FLOAT, INT_OPS)\
static int NAME(\
    ecs_script_ir_vm_t *vm,\
    const ecs_script_ir_op_t *op)\
{\
    const ecs_expr_binary_t *node = op->node;\
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);\
    W l = *(T*)flecs_ir_reg(vm, op->b)->value.ptr;\
    W r = *(T*)flecs_ir_reg(vm, op->c)->value.ptr;\
    ecs_token_kind_t operator =\
        (ecs_token_kind_t)(op->flags & ~EcsIrBinaryInPlace);\
    void *out;\
    if (op->flags & EcsIrBinaryInPlace) {\
        out = dst->value.ptr;\
        dst->value.type = node->node.type;\
        dst->ti = node->node.type_info;\
    } else {\
        int32_t top = (vm->scratch.top + 7) & ~7;\
        if (top + 8 <= vm->scratch.size) {\
            out = vm->scratch.data + top;\
            vm->scratch.top = top + 8;\
            dst->value.ptr = out;\
            dst->value.type = node->node.type;\
            dst->ti = node->node.type_info;\
            dst->storage = out;\
            dst->owned = true;\
        } else {\
            out = flecs_ir_reg_alloc_raw(vm, dst, node->node.type_info);\
        }\
    }\
    switch(operator) {\
    INT_OPS(T)\
    case EcsTokAdd: *(T*)out = (T)(l + r); return 0;\
    case EcsTokSub: *(T*)out = (T)(l - r); return 0;\
    case EcsTokMul: *(T*)out = (T)(l * r); return 0;\
    case EcsTokDiv:\
        if (CMP_ONLY_FLOAT ? ECS_EQZERO(r) : (r == 0)) {\
            flecs_ir_expr_error(vm, node, "division by zero");\
            return -1;\
        }\
        *(T*)out = (T)(l / r); return 0;\
    case EcsTokEq: *(bool*)out = l == r; return 0;\
    case EcsTokNeq: *(bool*)out = l != r; return 0;\
    case EcsTokGt: *(bool*)out = l > r; return 0;\
    case EcsTokGtEq: *(bool*)out = l >= r; return 0;\
    case EcsTokLt: *(bool*)out = l < r; return 0;\
    case EcsTokLtEq: *(bool*)out = l <= r; return 0;\
    default: break;\
    }\
    ecs_value_t out_value = { .type = node->node.type, .ptr = out };\
    return flecs_value_binary(&vm->v.base.script->pub, &node->node,\
        &flecs_ir_reg(vm, op->b)->value, &flecs_ir_reg(vm, op->c)->value,\
        &out_value, operator);\
}

FLECS_IR_BINARY_TYPED(flecs_ir_binary_i64, int64_t, int64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_i32, int32_t, int32_t, false, FLECS_IR_INTEGER_OPS)
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
FLECS_IR_BINARY_TYPED(flecs_ir_binary_f64, double, double, true, FLECS_IR_FLOAT_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_f32, float, float, true, FLECS_IR_FLOAT_OPS)
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
#pragma GCC diagnostic pop
#endif
FLECS_IR_BINARY_TYPED(flecs_ir_binary_i8, int8_t, int64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_i16, int16_t, int64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_u8, uint8_t, uint64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_u16, uint16_t, uint64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_u32, uint32_t, uint64_t, false, FLECS_IR_INTEGER_OPS)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_u64, uint64_t, uint64_t, false, FLECS_IR_INTEGER_OPS)

#undef FLECS_IR_BINARY_TYPED
#undef FLECS_IR_INTEGER_OPS
#undef FLECS_IR_FLOAT_OPS

static int flecs_ir_cast_number(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    const ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
    void *out = flecs_ir_reg_out_raw(vm, op, dst);
    op->imm.cast(out, src->value.ptr);
    return 0;
}

static int flecs_ir_cast(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_cast_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
    flecs_ir_reg_out(vm, op, dst);
    ecs_expr_value_t expr = {
        .value = src->value,
        .type_info = src->ti,
        .owned = src->owned
    };
    if (flecs_value_copy_to(vm->v.world, &dst->value, &expr)) {
        flecs_ir_expr_error(vm, node, "failed to cast value");
        return -1;
    }

    return 0;
}

static int flecs_ir_template_ref(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_template_ref_t *ref = flecs_ir_reg_out(vm, op, dst);
    ecs_entity_t tmpl = op->imm.entity;
    void *value = NULL;
    if (op->flags & EcsIrTemplateRefVar) {
        const ecs_script_template_ref_t *src_ref =
            flecs_ir_reg(vm, op->b)->value.ptr;
        value = flecs_expr_template_ref_value(
            &v->base.script->pub, op->node, src_ref);
        if (!value) {
            return -1;
        }

        tmpl = src_ref->type;
    } else if (op->b != -1) {
        ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
        ecs_assert(src->ti != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(src->ti->component == tmpl, ECS_INTERNAL_ERROR, NULL);
        value = ecs_ptr_new(v->world, tmpl);
        ecs_ptr_copy_w_type_info(v->world, src->ti, value, src->value.ptr);
    }

    flecs_script_template_ref_set(v->world, ref, tmpl, value);
    return 0;
}

static int flecs_ir_call(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_function_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    flecs_ir_reg_out(vm, op, dst);

    int32_t i, count = op->c;
    ecs_value_t *argv = NULL;
    if (count) {
        int32_t size = count * ECS_SIZEOF(ecs_value_t);
        int32_t top = (vm->scratch.top + 7) & ~7;
        if (top + size <= vm->scratch.size) {
            argv = (ecs_value_t*)(void*)(vm->scratch.data + top);
            vm->scratch.top = top + size;
        } else {
            argv = flecs_ir_arena_alloc(vm, &vm->scratch, size, 8);
        }

        for (i = 0; i < count; i ++) {
            argv[i] = flecs_ir_reg(vm, op->b + i)->value;
        }
    }

    return flecs_expr_function_call(&vm->v.base.script->pub, node,
        (op->flags & EcsIrCallMethod) ? count - 1 : count, argv, &dst->value);
}

static inline int flecs_ir_init_check(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    const void *error_node,
    ecs_script_ir_reg_t *dst,
    const ecs_type_info_t *ti)
{
    return flecs_expr_initializer_check(&vm->v.base.script->pub, error_node,
        ti->component, flecs_ito(uintptr_t, op->b), ti->size,
        dst->ti ? flecs_uto(uintptr_t, dst->ti->size) : UINTPTR_MAX);
}

static int flecs_ir_init_const(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    const ecs_type_info_t *ti = node->type_info;
    if (flecs_ir_init_check(vm, op, node, dst, ti)) {
        return -1;
    }

    void *ptr = ECS_OFFSET(dst->value.ptr, op->b);
    if (!ti->hooks.copy) {
        ecs_os_memcpy(ptr, op->imm.ptr, ti->size);
    } else {
        ecs_ptr_copy_w_type_info(vm->v.world, ti, ptr, op->imm.ptr);
    }

    return 0;
}

static int flecs_ir_init_assign(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->c);
    const ecs_type_info_t *ti = node->type_info;
    if (flecs_ir_init_check(vm, op, op->imm.ptr, dst, ti)) {
        return -1;
    }

    void *ptr = ECS_OFFSET(dst->value.ptr, op->b);
    if (src->owned) {
        if (!ti->hooks.move_dtor) {
            ecs_os_memcpy(ptr, src->value.ptr, ti->size);
        } else {
            flecs_type_info_move_dtor(ptr, src->value.ptr, 1, ti);
        }

        src->owned = false;
    } else {
        if (!ti->hooks.copy) {
            ecs_os_memcpy(ptr, src->value.ptr, ti->size);
        } else {
            flecs_type_info_copy(ptr, src->value.ptr, 1, ti);
        }
    }

    return 0;
}

static int flecs_ir_init_op(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->c);
    const ecs_type_info_t *ti = node->type_info;
    if (flecs_ir_init_check(vm, op, op->imm.ptr, dst, ti)) {
        return -1;
    }

    ecs_value_t out = {
        .type = node->type,
        .ptr = ECS_OFFSET(dst->value.ptr, op->b)
    };
    return flecs_value_binary(&vm->v.base.script->pub, op->imm.ptr,
        NULL, &src->value, &out, (ecs_token_kind_t)op->flags);
}

static int flecs_ir_init_swizzle(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_swizzle_t *swizzle = op->imm.ptr;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->c);
    ecs_size_t size = swizzle->elem_size;
    uintptr_t dst_size = dst->ti
        ? flecs_uto(uintptr_t, dst->ti->size)
        : UINTPTR_MAX;
    int32_t s;
    for (s = 0; s < swizzle->count; s ++) {
        uintptr_t offset = flecs_ito(uintptr_t, op->b) + swizzle->dst[s];
        if (flecs_expr_initializer_check(&vm->v.base.script->pub, op->node,
            swizzle->node.type, offset, size, dst_size))
        {
            return -1;
        }

        ecs_os_memcpy(ECS_OFFSET(dst->value.ptr, offset),
            ECS_OFFSET(left->value.ptr, swizzle->src[s]), size);
    }

    return 0;
}

static ecs_meta_cursor_t* flecs_ir_cursor(
    ecs_script_ir_vm_t *vm)
{
    ecs_assert(ecs_vec_count(&vm->cursors) > 0, ECS_INTERNAL_ERROR, NULL);
    return ecs_vec_last_t(&vm->cursors, ecs_meta_cursor_t);
}

static int flecs_ir_str_format(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *expr = op->node;
    const ecs_expr_format_t *format = op->imm.ptr;
    ecs_script_ir_reg_t *value = flecs_ir_reg(vm, op->a);
    int32_t width = 0, precision = -1;
    if (op->b != -1) {
        width = *(int32_t*)flecs_ir_reg(vm, op->b)->value.ptr;
    }

    if (op->c != -1) {
        precision = *(int32_t*)flecs_ir_reg(vm, op->c)->value.ptr;
    }

    ecs_strbuf_t *buf = ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t);
    return flecs_expr_format_value(&vm->v.base.script->pub, expr,
        &value->value, format, width, precision, buf);
}

static int flecs_ir_range(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_range_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    void *out = flecs_ir_reg_out(vm, op, dst);
    return flecs_expr_range_eval(vm->v.world, node->node.type,
        *(int32_t*)flecs_ir_reg(vm, op->b)->value.ptr,
        *(int32_t*)flecs_ir_reg(vm, op->c)->value.ptr, out);
}

static int flecs_ir_match_cmp(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_match_t *node = op->node;
    ecs_script_ir_reg_t *value = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *compare = flecs_ir_reg(vm, op->b);
    bool result = false;
    ecs_value_t out = { .type = ecs_id(ecs_bool_t), .ptr = &result };
    if (flecs_value_binary(&vm->v.base.script->pub, &node->node,
        &value->value, &compare->value, &out, EcsTokEq))
    {
        return -1;
    }

    vm->cond = result;
    return 0;
}

static int flecs_ir_script(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    void *out = flecs_ir_reg_out(vm, op, dst);
    *(ecs_entity_t*)out = flecs_expr_script_new(vm->v.world, op->imm.ptr);
    dst->value.type = ecs_id(ecs_entity_t);
    return 0;
}

#ifdef FLECS_SCRIPT_ASYNC
static int flecs_ir_await_launch(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_expr_function_t *call = flecs_script_await_call(
        &vm->v, op->node, op->imm.ptr);
    if (!call) {
        return -1;
    }

    int32_t i, count = op->b;
    ecs_value_t *argv = count ? ecs_os_alloca_n(ecs_value_t, count) : NULL;
    for (i = 0; i < count; i ++) {
        argv[i] = flecs_ir_reg(vm, op->a + i)->value;
    }

    flecs_script_await_launch(vm->v.world, &vm->async, call, argv);
    return 0;
}

static int flecs_ir_await_poll(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    bool *suspended)
{
    ecs_script_future_t *future = NULL;
    int result = flecs_script_await_poll(&vm->v, &vm->async, op->node, &future);
    *suspended = result == 1;
    if (result) {
        return result == 1 ? 0 : -1;
    }

    if (op->flags & EcsIrAwaitVar) {
        ecs_script_eval_visitor_t *v = &vm->v;
        const ecs_value_t *value = &future->value;
        ecs_script_var_t *var = ecs_script_vars_declare(v->vars, NULL);
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, value->type);
        var->value.type = value->type;
        var->value.ptr = flecs_ir_var_alloc(vm, ti);
        var->type_info = ti;
        var->owned = true;
        ecs_ptr_copy(v->world, value->type, var->value.ptr, value->ptr);
    }

    ecs_script_future_release(future);
    return result;
}

static bool flecs_ir_try_catch(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    if (frame->kind != EcsIrFrameTry || frame->state != 1) {
        return false;
    }

    const ecs_script_ir_op_t *op = &vm->ops[frame->pc];
    int32_t i = flecs_script_catch(&vm->v, &vm->async, op->node);
    if (i == -1) {
        return false;
    }

    frame->state = 2;
    vm->pc = ecs_vec_get_t(&vm->ir->catches, int32_t, op->a + i)[0];
    return true;
}
#endif

static bool flecs_ir_unwind(
    ecs_script_ir_vm_t *vm,
    int32_t base)
{
    while (vm->frame_count > base) {
        ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
#ifdef FLECS_SCRIPT_ASYNC
        if (flecs_ir_try_catch(vm, frame)) {
            return true;
        }
#endif
        if (frame->kind == EcsIrFrameBlock) {
            return false;
        }

        flecs_ir_frame_leave(vm, frame);
        vm->frame_count --;
    }
    return false;
}

static bool flecs_ir_continue(
    ecs_script_ir_vm_t *vm)
{
    int32_t i;
    for (i = vm->frame_count - 1; i >= 0; i --) {
        int16_t kind = flecs_ir_frame_at(vm, i)->kind;
        if (kind == EcsIrFrameFor || kind == EcsIrFrameWhile) {
            break;
        }

        if (kind == EcsIrFrameBlock) {
            return false;
        }
    }

    if (i < 0) {
        return false;
    }

    while (vm->frame_count > i + 1) {
        ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
        flecs_ir_frame_leave(vm, frame);
        vm->frame_count --;
    }

    vm->pc = flecs_ir_frame_at(vm, i)->pc;
    return true;
}

static flecs_script_run_status_t flecs_ir_exec(
    ecs_script_ir_vm_t *vm,
    int32_t base);

static int flecs_ir_run_nested(
    ecs_script_ir_vm_t *vm,
    int32_t entry_index,
    ecs_entity_t *out)
{
    const ecs_script_ir_entry_t *entries = ecs_vec_first(&vm->ir->entries);
    const ecs_script_ir_entry_t *current = &entries[vm->entry];
    const ecs_script_ir_entry_t *entry = &entries[entry_index];

    int32_t base = vm->frame_count;
    ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
        vm, EcsIrFrameBlock, vm->pc);
    frame->u.block.reg_base = vm->reg_base;
    frame->u.block.entry = vm->entry;
    frame->u.block.vars = false;

    vm->reg_base += current->reg_count;
    flecs_ir_regs_ensure(vm, vm->reg_base + entry->reg_count);
    vm->entry = entry_index;
    vm->pc = entry->pc;
    vm->last_entity = 0;

    flecs_script_run_status_t status = flecs_ir_exec(vm, base);
    ecs_assert(vm->frame_count == base, ECS_INTERNAL_ERROR, NULL);
    if (status != FlecsScriptRunDone) {
        return -1;
    }

    *out = vm->last_entity;
    return 0;
}

static void flecs_ir_block_pop(
    ecs_script_ir_vm_t *vm)
{
    ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
    ecs_assert(frame->kind == EcsIrFrameBlock, ECS_INTERNAL_ERROR, NULL);
    flecs_ir_block_leave(vm, frame);
    vm->pc = frame->pc;
    vm->reg_base = frame->u.block.reg_base;
    vm->entry = frame->u.block.entry;
    vm->frame_count --;
}

static flecs_script_run_status_t flecs_ir_exec(
    ecs_script_ir_vm_t *vm,
    int32_t base)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_op_t *ops = vm->ops;

    while (vm->frame_count > base) {
        int32_t pc = vm->pc ++;
        const ecs_script_ir_op_t *op = &ops[pc];
        int res = 0;
#ifdef FLECS_SCRIPT_IR_PROFILE
        flecs_ir_profile_ops[op->kind] ++;
#endif

        switch((ecs_script_ir_op_kind_t)op->kind) {
        case EcsIrEnd:
        case EcsIrReturn: {
            if (op->kind == EcsIrReturn) {
                ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
                ecs_value_t *result = vm->result;
                if (result) {
                    if (flecs_ir_value_to(vm, reg, result->type, result->ptr,
                        ecs_get_type_info(v->world, result->type)))
                    {
                        flecs_ir_expr_error(vm,
                            ((const ecs_script_function_node_t*)op->node)
                                ->return_expr,
                            "failed to write to output");
                        res = -1;
                        break;
                    }
                }

                flecs_ir_expr_end(vm);
            }

            while (vm->frame_count > base + 1) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
                flecs_ir_frame_leave(vm, frame);
                vm->frame_count --;
            }
            flecs_ir_block_pop(vm);
            return FlecsScriptRunDone;
        }
        case EcsIrJump:
            vm->pc = op->a;
            break;
        case EcsIrStmtBlock:
            if (v->script_entity && !ecs_is_alive(v->world, v->script_entity)) {
                flecs_ir_unwind(vm, base);
                flecs_ir_block_pop(vm);
                return FlecsScriptRunError;
            }
            if (!flecs_ir_stmt_runs(vm, op)) {
                if (vm->dirty) {
                    flecs_ir_mark(vm, -pc - 1);
                } else {
                    ecs_vec_append_t(NULL, &vm->pending_marks, int32_t)[0] = -pc - 1;
                }
#ifdef FLECS_SCRIPT_IR_PROFILE
                flecs_ir_profile_stats[EcsIrProfileStmtSkipped] += op->c;
#endif
                vm->pc = op->b;
            }

            break;
        case EcsIrStmt: {
            if (v->script_entity && !ecs_is_alive(v->world, v->script_entity)) {
                flecs_ir_unwind(vm, base);
                flecs_ir_block_pop(vm);
                return FlecsScriptRunError;
            }

            if (flecs_ir_stmt_runs(vm, op)) {
                flecs_ir_prof(EcsIrProfileStmtRun);
                break;
            }

            if (op->flags & EcsIrStmtCached) {
                vm->pc = op->b;
                break;
            }

            flecs_ir_stmt_skip(vm, op);
            int32_t next = op->b;
            for (;;) {
                const ecs_script_ir_op_t *next_op = &ops[next];
                if (next_op->kind != EcsIrStmt) {
                    vm->pc = next;
                    break;
                }

                if (flecs_ir_stmt_runs(vm, next_op)) {
                    flecs_ir_prof(EcsIrProfileStmtRun);
                    vm->pc = next + 1;
                    break;
                }

                if (next_op->flags & EcsIrStmtCached) {
                    vm->pc = next_op->b;
                    break;
                }

                flecs_ir_stmt_skip(vm, next_op);
                next = next_op->b;
            }

            break;
        }
        case EcsIrMark:
            flecs_ir_mark(vm, op->c);
            break;
        case EcsIrAnnotClear:
            ecs_vec_clear(&v->r->annot);
            break;
        case EcsIrScopeEnter: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameScope, pc);
            frame->u.scope.state.parent = v->parent;
            frame->u.scope.state.scope_slot = v->scope_slot;
            frame->u.scope.var_count = -1;
            v->scope_slot = op->a;
            if (op->a >= 0 &&
                op->a < ecs_vec_count(&v->state->scope_slots))
            {
                ecs_vec_get_t(&v->state->scope_slots, int32_t, op->a)[0] = v->visit;
            }

            if (op->b > 0) {
                frame->u.scope.var_count = v->vars->vars.count;
                frame->u.scope.vscratch_top = vm->vscratch.top;
                frame->u.scope.vheap_count = vm->vscratch.heap.count;
            }

            if (v->entity && op->c &&
                (v->force || v->entity->created))
            {
                ecs_entity_t src = v->entity->eval;
                if (src != EcsVariable) {
                    const ecs_id_t *ids = op->imm.ptr;
                    int32_t i, count = op->c;
                    ecs_table_t *table = ecs_get_table(v->world, src);
                    bool missing = table == NULL;
                    for (i = 0; !missing && i < count; i ++) {
                        if (ecs_search(v->world, table, ids[i], NULL) == -1) {
                            missing = true;
                        }
                    }

                    if (missing) {
                        flecs_ir_prof(EcsIrProfileBatchAdd);
                        flecs_script_scope_add_ids(v, src,
                            ECS_CONST_CAST(ecs_script_scope_t*, op->node),
                            ids, count);
                    } else {
                        flecs_ir_prof(EcsIrProfileBatchSkip);
                    }
                }
            }

            if (op->flags & EcsIrScopeEntity) {
                ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);
                v->parent = v->entity->eval;
            }

            break;
        }

        case EcsIrLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == op->a, ECS_INTERNAL_ERROR, NULL);
            if (frame->kind == EcsIrFrameEntity) {
                vm->last_entity = frame->u.entity.eval;
            }

            flecs_ir_frame_leave(vm, frame);
            vm->frame_count --;
            break;
        }
        case EcsIrEntityEnter: {
            flecs_script_entity_state_t state;
            res = flecs_script_eval_entity_enter_w_name(v, ECS_CONST_CAST(
                ecs_script_entity_t*, (const void*)op->node),
                op->a != -1 ? &flecs_ir_reg(vm, op->a)->value : NULL, &state);
#ifdef FLECS_SCRIPT_IR_PROFILE
            if (op->a == -1) {
                flecs_ir_prof(state.created ?
                    EcsIrProfileEntityCreate : EcsIrProfileEntityReuse);
            }
#endif
            if (op->flags & EcsIrEndsExpr) {
                flecs_ir_expr_end(vm);
            }

            if (!res) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                    vm, EcsIrFrameEntity, pc);
                frame->u.entity = state;
                v->entity = &frame->u.entity;
            }

            break;
        }

        case EcsIrWithEnter: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameWith, pc);
            frame->u.with.with_count = ecs_vec_count(&v->r->with);
            frame->u.with.cursor = flecs_stack_get_cursor(&v->r->stack);
            frame->u.with.is_with_scope = v->is_with_scope;
            frame->u.with.force = v->force;
            break;
        }
        case EcsIrWithBody: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            v->is_with_scope = true;
            v->force = flecs_script_eval_force(
                v, op->node, frame->u.with.force);
            break;
        }

        case EcsIrWithTag:
            res = flecs_ir_with_tag(vm, op);
            break;
        case EcsIrWithComponentBegin:
            res = flecs_ir_with_component_begin(vm, op);
            break;
        case EcsIrWithComponentEnd:
            res = flecs_ir_with_component_end(vm, op);
            break;
        case EcsIrPairScopeEnter: {
            const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
            flecs_script_pair_scope_state_t state;
            res = flecs_script_eval_pair_scope_enter(v, op->node,
                flecs_ir_name(vm, desc->first_reg),
                flecs_ir_name(vm, desc->second_reg), &state);
            if (op->flags & EcsIrEndsExpr) {
                flecs_ir_expr_end(vm);
            }

            if (!res) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                    vm, EcsIrFramePairScope, pc);
                frame->u.pair_scope = state;
            }

            break;
        }

        case EcsIrIfEnter: {
            flecs_ir_set_dirty(vm);
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameIf, pc);
            frame->u.if_.force = v->force;
            v->force = flecs_script_eval_force(v, op->node, v->force);
            if (!vm->cond) {
                vm->pc = op->b;
            }

            break;
        }

        case EcsIrForEnter: {
            flecs_ir_set_dirty(vm);
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameFor, vm->pc);
            frame->u.for_.vscratch_top = vm->vscratch.top;
            frame->u.for_.vheap_count = vm->vscratch.heap.count;
            res = flecs_ir_for_enter(vm, op, &frame->u.for_.state);
            if (res) {
                vm->frame_count --;
            }

            break;
        }
        case EcsIrForNext: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameFor, ECS_INTERNAL_ERROR, NULL);
            flecs_script_for_state_t *fs = &frame->u.for_.state;
            if (fs->kind == FlecsScriptForRange && fs->index_var == -1) {
                int32_t value = fs->from + fs->index;
                if (value >= fs->count) {
                    vm->pc = op->a;
                } else {
                    *(int32_t*)((ecs_script_var_t*)fs->vars->vars.array)
                        [fs->var_base + fs->elem_var].value.ptr = value;
                    fs->index ++;
                }
            } else if (!flecs_script_for_next(fs)) {
                vm->pc = op->a;
            }

            break;
        }

        case EcsIrContinue:
            if (!flecs_ir_continue(vm)) {
                flecs_ir_error(vm, op->node,
                    "continue is only allowed inside a loop");
                res = -1;
            }
            break;
        case EcsIrWhileEnter:
            flecs_ir_frame_push(vm, EcsIrFrameWhile, vm->pc);
            break;
        case EcsIrWhileNext:
            if (!vm->cond) {
                vm->pc = op->a;
            }
            break;
        case EcsIrAssign:
            res = flecs_ir_assign(vm, op);
            break;
        case EcsIrTryEnter: {
#ifdef FLECS_SCRIPT_ASYNC
            if (vm->can_suspend && base == 0) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                    vm, EcsIrFrameTry, pc);
                frame->state = 1;
                break;
            }
#endif
            flecs_ir_error(vm, op->node,
                "try requires resumable script execution");
            res = -1;
            break;
        }

        case EcsIrAwaitStart:
#ifdef FLECS_SCRIPT_ASYNC
            if (!vm->can_suspend || base != 0) {
                flecs_ir_error(vm, op->node,
                    "await requires resumable script execution");
                res = -1;
                break;
            }
            if (vm->async.future) {
                vm->pc = op->b;
            }

            break;
#else
            flecs_ir_error(vm, op->node,
                "await requires resumable script execution");
            res = -1;
            break;
#endif
        case EcsIrAwaitLaunch:
#ifdef FLECS_SCRIPT_ASYNC
            res = flecs_ir_await_launch(vm, op);
#else
            res = -1;
#endif
            break;
        case EcsIrAwaitPoll: {
#ifdef FLECS_SCRIPT_ASYNC
            bool suspended = false;
            res = flecs_ir_await_poll(vm, op, &suspended);
            if (suspended) {
                vm->pc = pc;
                return FlecsScriptRunSuspended;
            }
#else
            res = -1;
#endif
            break;
        }
        case EcsIrTag:
            res = flecs_ir_tag(vm, op);
            break;
        case EcsIrComponentBegin:
            res = flecs_ir_component_alloc(vm, op);
            break;
        case EcsIrComponentEnd:
            res = flecs_ir_component_end(vm, op);
            break;
        case EcsIrComponentVisitor: {
            bool handled = false;
            res = flecs_ir_component_visitor(vm, op, &handled);
            if (!res && handled) {
                vm->pc = op->b;
            }

            break;
        }
        case EcsIrUsing:
        case EcsIrModule:
        case EcsIrTemplate:
        case EcsIrMutCheck:
        case EcsIrAsync:
            res = flecs_script_eval_node(&v->base,
                ECS_CONST_CAST(ecs_script_node_t*, (const void*)op->node));
            break;
        case EcsIrAnnot:
            res = flecs_ir_annot(vm, op);
            break;
        case EcsIrConstEnd:
            res = flecs_ir_const_end(vm, op);
            break;
        case EcsIrConstCached: {
            ecs_script_computed_t *slot = op->a < ecs_vec_count(&v->state->computed)
                ? ecs_vec_get_t(&v->state->computed, ecs_script_computed_t, op->a)
                : NULL;
            if (!slot || !slot->valid) {
                vm->pc = op->b;
                break;
            }

            flecs_script_const_var_init(flecs_ir_var_declare(vm), slot->ti,
                ((const ecs_script_var_node_t*)op->node)->eval_type,
                slot->ptr, false);
            break;
        }
        case EcsIrExprBegin:
            flecs_ir_expr_begin(vm, op, pc);
            break;
        case EcsIrExprEnd:
            flecs_ir_expr_end(vm);
            break;
        case EcsIrLoadConst: {
            const ecs_expr_node_t *node = op->node;
            flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), node, node->type,
                op->imm.ptr);
            break;
        }
        case EcsIrLoadVar:
            res = flecs_ir_load_var(vm, op);
            break;
        case EcsIrLoadGlobal:
            res = flecs_ir_load_global(vm, op);
            break;
        case EcsIrLoadSymbol:
            res = flecs_ir_load_symbol(vm, op,
                flecs_script_symbol_entity(v, op->b));
            break;
        case EcsIrLookup:
            res = flecs_ir_load_symbol(vm, op,
                flecs_ir_lookup_name(vm, op->imm.str));
            break;
        case EcsIrMember: {
            const ecs_expr_node_t *node = op->node;
            ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
            flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), node, node->type,
                ECS_OFFSET(src->value.ptr, op->c));
            break;
        }
        case EcsIrSwizzle: {
            const ecs_expr_swizzle_t *node = op->node;
            ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
            ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
            void *out = flecs_ir_reg_out_raw(vm, op, dst);
            ecs_size_t size = node->elem_size;
            int32_t i;
            for (i = 0; i < node->count; i ++) {
                ecs_os_memcpy(ECS_OFFSET(out, i * size),
                    ECS_OFFSET(src->value.ptr, node->src[i]), size);
            }

            break;
        }
        case EcsIrElement:
            res = flecs_ir_element(vm, op);
            break;
        case EcsIrComponentGet:
            res = flecs_ir_component_get(vm, op);
            break;
        case EcsIrHas:
            res = flecs_ir_has(vm, op);
            break;
        case EcsIrUnary: {
            const ecs_expr_unary_t *node = op->node;
            ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
            ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
            flecs_ir_reg_out(vm, op, dst);
            res = flecs_value_unary(&v->base.script->pub, &src->value,
                &dst->value, node->operator);
            break;
        }
        case EcsIrBinary:
            res = flecs_ir_binary(vm, op);
            break;
        case EcsIrBinaryI8:
            res = flecs_ir_binary_i8(vm, op);
            break;
        case EcsIrBinaryI16:
            res = flecs_ir_binary_i16(vm, op);
            break;
        case EcsIrBinaryI32:
            res = flecs_ir_binary_i32(vm, op);
            break;
        case EcsIrBinaryI64:
            res = flecs_ir_binary_i64(vm, op);
            break;
        case EcsIrBinaryU8:
            res = flecs_ir_binary_u8(vm, op);
            break;
        case EcsIrBinaryU16:
            res = flecs_ir_binary_u16(vm, op);
            break;
        case EcsIrBinaryU32:
            res = flecs_ir_binary_u32(vm, op);
            break;
        case EcsIrBinaryU64:
            res = flecs_ir_binary_u64(vm, op);
            break;
        case EcsIrBinaryF32:
            res = flecs_ir_binary_f32(vm, op);
            break;
        case EcsIrBinaryF64:
            res = flecs_ir_binary_f64(vm, op);
            break;
        case EcsIrJumpIfFalse:
        case EcsIrJumpIfTrue: {
            ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->a);
            bool value = *(bool*)src->value.ptr;
            if (value == (op->kind == EcsIrJumpIfTrue)) {
                ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->c);
                bool *out = flecs_ir_reg_out_raw(vm, op, dst);
                *out = value;
                vm->pc = op->b;
            }

            break;
        }
        case EcsIrCast:
            res = flecs_ir_cast(vm, op);
            break;
        case EcsIrCastNumber:
            res = flecs_ir_cast_number(vm, op);
            break;
        case EcsIrCall:
            res = flecs_ir_call(vm, op);
            break;
        case EcsIrInitAlloc: {
            const ecs_expr_node_t *node = op->node;
            flecs_ir_reg_alloc(vm, flecs_ir_reg(vm, op->a), node->type_info);
            break;
        }
        case EcsIrInitConst:
            res = flecs_ir_init_const(vm, op);
            break;
        case EcsIrInitAssign:
            res = flecs_ir_init_assign(vm, op);
            break;
        case EcsIrInitOp:
            res = flecs_ir_init_op(vm, op);
            break;
        case EcsIrInitSwizzle:
            res = flecs_ir_init_swizzle(vm, op);
            break;
        case EcsIrDynBegin: {
            const ecs_expr_node_t *node = op->node;
            ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
            ecs_meta_cursor_t *cur = ecs_vec_append_t(
                NULL, &vm->cursors, ecs_meta_cursor_t);
            *cur = ecs_meta_cursor(v->world, node->type, dst->value.ptr);
            break;
        }
        case EcsIrDynPush:
            res = ecs_meta_push(flecs_ir_cursor(vm));
            break;
        case EcsIrDynPop:
            res = ecs_meta_pop(flecs_ir_cursor(vm));
            break;
        case EcsIrDynNext:
            res = ecs_meta_next(flecs_ir_cursor(vm));
            break;
        case EcsIrDynMember:
            res = ecs_meta_member(flecs_ir_cursor(vm), op->imm.str);
            break;
        case EcsIrDynKey:
            res = ecs_meta_key(flecs_ir_cursor(vm),
                &flecs_ir_reg(vm, op->a)->value);
            break;
        case EcsIrDynSet:
            res = ecs_meta_set_value(flecs_ir_cursor(vm),
                &flecs_ir_reg(vm, op->a)->value);
            break;
        case EcsIrDynSwizzle:
            res = flecs_expr_swizzle_set(flecs_ir_cursor(vm), op->imm.ptr,
                flecs_ir_reg(vm, op->a)->value.ptr);
            break;
        case EcsIrDynEnd:
            ecs_vec_remove_last(&vm->cursors);
            break;
        case EcsIrStrBegin: {
            ecs_strbuf_t *buf = ecs_vec_append_t(
                NULL, &vm->strbufs, ecs_strbuf_t);
            *buf = ECS_STRBUF_INIT;
            break;
        }
        case EcsIrStrFrag:
            ecs_strbuf_appendstr(
                ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t), op->imm.str);
            break;
        case EcsIrStrValue: {
            ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
            ecs_assert(reg->value.type == ecs_id(ecs_string_t),
                ECS_INTERNAL_ERROR, NULL);
            const char *str = *(char**)reg->value.ptr;
            if (str) {
                ecs_strbuf_appendstr(
                    ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t), str);
            }

            break;
        }
        case EcsIrStrCast: {
            const void *ptr = flecs_ir_reg(vm, op->a)->value.ptr;
            ecs_strbuf_t *buf = ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t);
            union { int64_t i; uint64_t u; double f; } value;
            if (op->flags == EcsExprNumChar) {
                ecs_strbuf_append(buf, "%c", *(const char*)ptr);
                break;
            }

            op->imm.cast(&value, ptr);
            if (op->flags == EcsExprNumSigned) {
                ecs_strbuf_appendint(buf, value.i);
            } else if (op->flags == EcsExprNumUnsigned) {
                ecs_strbuf_append(buf, "%" PRIu64, value.u);
            } else {
                ecs_strbuf_append(buf, "%f", value.f);
            }

            break;
        }
        case EcsIrStrFormat:
            res = flecs_ir_str_format(vm, op);
            break;
        case EcsIrStrEnd: {
            ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
            ecs_strbuf_t *buf = ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t);
            char *str = ecs_strbuf_get(buf);
            ecs_vec_remove_last(&vm->strbufs);
            char **out = flecs_ir_reg_out(vm, op, dst);
            if (op->flags & EcsIrInPlace) {
                ecs_os_free(*out);
            }

            *out = str;
            break;
        }
        case EcsIrMatchCmp:
            res = flecs_ir_match_cmp(vm, op);
            if (!res && !vm->cond) {
                vm->pc = op->c;
            }

            break;
        case EcsIrMatchFail: {
            ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
            char *str = ecs_ptr_to_str(v->world, reg->value.type, reg->value.ptr);
            flecs_ir_expr_error(vm, op->node,
                "match value '%s' not handled by case", str);
            ecs_os_free(str);
            res = -1;
            break;
        }
        case EcsIrRange:
            res = flecs_ir_range(vm, op);
            break;
        case EcsIrNew: {
            ecs_entity_t result = 0;
            res = flecs_ir_run_nested(vm, op->b, &result);
            if (!res) {
                ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
                ecs_entity_t *out = flecs_ir_reg_out_raw(vm, op, dst);
                *out = result;
                dst->value.type = ecs_id(ecs_entity_t);
            }

            break;
        }
        case EcsIrScript:
            res = flecs_ir_script(vm, op);
            break;
        case EcsIrTemplateRef:
            res = flecs_ir_template_ref(vm, op);
            break;
        case EcsIrTemplateRefValue: {
            const ecs_expr_node_t *node = op->node;
            ecs_script_template_ref_t *ref = flecs_ir_reg(vm, op->b)->value.ptr;
            flecs_ir_reg_borrow(flecs_ir_reg(vm, op->a), node, node->type,
                ref->value);
            break;
        }
        case EcsIrToBool: {
            ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
            if (reg->value.type == ecs_id(ecs_bool_t)) {
                vm->cond = *(bool*)reg->value.ptr;
            } else {
                res = flecs_script_value_to_bool(
                    v, op->node, &reg->value, &vm->cond);
            }

            break;
        }
        case EcsIrOpKindLast:
            ecs_abort(ECS_INTERNAL_ERROR, "corrupt IR instruction");
        }

        if (res) {
            if (!flecs_ir_unwind(vm, base)) {
#ifdef FLECS_SCRIPT_ASYNC
                if (vm->async.thrown && base == 0) {
                    flecs_script_report_throw(&vm->v, &vm->async);
                }
#endif
                flecs_ir_block_pop(vm);
                return FlecsScriptRunError;
            }
        }
    }

    return FlecsScriptRunDone;
}

static void flecs_ir_vm_reset(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_t *ir)
{
    vm->ir = ir;
    vm->ops = ir ? ecs_vec_first(&ir->ops) : NULL;
    vm->pc = 0;
    vm->entry = 0;
    vm->reg_base = 0;
    vm->scratch.top = 0;
    vm->vscratch.top = 0;
    vm->frame_count = 0;
    vm->last_entity = 0;
    vm->result = NULL;
    vm->cond = false;
    vm->dirty = false;
    vm->can_suspend = false;
#ifdef FLECS_SCRIPT_ASYNC
    ecs_os_zeromem(&vm->async);
#endif
}

static void flecs_ir_vm_setup(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_t *ir)
{
    vm->regs = NULL;
    vm->reg_count = 0;
    ecs_os_memset(vm->frames, 0, sizeof(vm->frames));
    flecs_ir_arena_init(&vm->scratch);
    flecs_ir_arena_init(&vm->vscratch);
    ecs_vec_init_t(NULL, &vm->owned, flecs_ir_owned_t, 0);
    ecs_vec_init_t(NULL, &vm->strbufs, ecs_strbuf_t, 0);
    ecs_vec_init_t(NULL, &vm->cursors, ecs_meta_cursor_t, 0);
    ecs_vec_init_t(NULL, &vm->pending_marks, int32_t, 0);
    flecs_ir_vm_reset(vm, ir);
}

static void flecs_ir_vm_clear(
    ecs_script_ir_vm_t *vm)
{
    flecs_script_ir_vm_abandon(vm);
    int32_t i, count = vm->strbufs.count;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_reset(ecs_vec_get_t(&vm->strbufs, ecs_strbuf_t, i));
    }

    vm->strbufs.count = 0;
    vm->cursors.count = 0;
    vm->pending_marks.count = 0;
    flecs_ir_arena_restore(&vm->scratch, 0, 0);
    flecs_ir_arena_restore(&vm->vscratch, 0, 0);
    vm->owned.count = 0;
#ifdef FLECS_SCRIPT_ASYNC
    flecs_script_async_fini(&vm->async);
#endif
}

static void flecs_ir_vm_teardown(
    ecs_script_ir_vm_t *vm)
{
    flecs_ir_vm_clear(vm);
    int32_t i;
    for (i = 0; i < (int32_t)(sizeof(vm->frames) / sizeof(vm->frames[0])); i ++) {
        ecs_os_free(vm->frames[i]);
    }

    ecs_vec_fini_t(NULL, &vm->strbufs, ecs_strbuf_t);
    ecs_vec_fini_t(NULL, &vm->cursors, ecs_meta_cursor_t);
    ecs_vec_fini_t(NULL, &vm->pending_marks, int32_t);
    flecs_ir_arena_fini(&vm->scratch);
    flecs_ir_arena_fini(&vm->vscratch);
    ecs_vec_fini_t(NULL, &vm->owned, flecs_ir_owned_t);
    ecs_os_free(vm->regs);
    vm->regs = NULL;
    vm->reg_count = 0;
}

static ecs_script_ir_vm_t* flecs_ir_vm_acquire(
    ecs_script_runtime_t *r,
    ecs_script_ir_t *ir)
{
    ecs_script_ir_vm_t *vm;
    if (r && r->ir_vms.count) {
        vm = ((ecs_script_ir_vm_t**)r->ir_vms.array)[-- r->ir_vms.count];
        flecs_ir_vm_reset(vm, ir);
    } else {
        vm = ecs_os_malloc_t(ecs_script_ir_vm_t);
        flecs_ir_vm_setup(vm, ir);
    }

    return vm;
}

static void flecs_ir_vm_release(
    ecs_script_runtime_t *r,
    ecs_script_ir_vm_t *vm)
{
    flecs_ir_vm_clear(vm);
    if (r) {
        ecs_vec_append_t(NULL, &r->ir_vms, ecs_script_ir_vm_t*)[0] = vm;
    } else {
        flecs_ir_vm_teardown(vm);
        ecs_os_free(vm);
    }
}

void flecs_script_ir_vm_pool_fini(
    ecs_script_runtime_t *r)
{
    int32_t i, count = r->ir_vms.count;
    ecs_script_ir_vm_t **vms = r->ir_vms.array;
    for (i = 0; i < count; i ++) {
        flecs_ir_vm_teardown(vms[i]);
        ecs_os_free(vms[i]);
    }

    ecs_vec_fini_t(NULL, &r->ir_vms, ecs_script_ir_vm_t*);
}

void flecs_script_ir_vm_init(
    ecs_script_ir_vm_t *vm)
{
    flecs_ir_vm_setup(vm, flecs_script_ir_ensure(vm->v.base.script));
}

void flecs_script_ir_vm_abandon(
    ecs_script_ir_vm_t *vm)
{
    while (vm->frame_count) {
        ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
        if (frame->kind == EcsIrFrameExpr) {
            frame->state = 0;
        }

        flecs_ir_frame_leave(vm, frame);
        vm->frame_count --;
    }
}

void flecs_script_ir_vm_fini(
    ecs_script_ir_vm_t *vm,
    const ecs_script_eval_desc_t *desc)
{
    flecs_ir_vm_teardown(vm);
    flecs_script_eval_visit_fini(&vm->v, desc);
}

flecs_script_run_status_t flecs_script_ir_vm_run(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_entry_t *entry)
{
    if (!vm->ir || !entry) {
        return FlecsScriptRunError;
    }

    flecs_ir_prof(EcsIrProfileRun);
    if (!vm->frame_count) {
        const ecs_script_ir_entry_t *entries = ecs_vec_first(&vm->ir->entries);
        ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
            vm, EcsIrFrameBlock, -1);
        frame->u.block.reg_base = 0;
        frame->u.block.entry = 0;
        frame->u.block.vars = true;
        vm->v.vars = flecs_script_vars_push(
            vm->v.vars, &vm->v.r->stack, vm->v.r->allocator);
        vm->entry = (int32_t)(entry - entries);
        vm->reg_base = 0;
        vm->pc = entry->pc;
        vm->last_entity = 0;
        vm->pending_marks.count = 0;
        flecs_ir_regs_ensure(vm, entry->reg_count);
    }

    return flecs_ir_exec(vm, 0);
}

flecs_script_run_status_t flecs_script_ir_vm_resume(
    ecs_script_ir_vm_t *vm)
{
    ecs_assert(vm->frame_count > 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_ir_exec(vm, 0);
}

const char* flecs_script_ir_vm_stmt_pos(
    const ecs_script_ir_vm_t *vm)
{
    if (!vm->frame_count || !vm->ops) {
        return NULL;
    }

    const ecs_script_ir_op_t *op = &vm->ops[vm->pc];
    if (op->kind == EcsIrAwaitPoll && op->node) {
        return ((const ecs_script_node_t*)op->node)->pos;
    }

    return NULL;
}

int flecs_script_ir_eval(
    ecs_script_eval_visitor_t *v,
    ecs_script_impl_t *script,
    const ecs_script_node_t *node)
{
    ecs_script_ir_t *ir = flecs_script_ir_ensure(script);
    ecs_script_ir_vm_t *vm = flecs_ir_vm_acquire(v->r, ir);
    vm->v = *v;

    const ecs_script_ir_entry_t *entry = ir
        ? flecs_script_ir_entry(ir, node) : NULL;
    flecs_script_run_status_t status = flecs_script_ir_vm_run(vm, entry);
    int result = -1;
    if (status == FlecsScriptRunDone) {
        flecs_script_eval_cleanup(&vm->v, vm->dirty);
        result = 0;
    }

    flecs_ir_vm_release(v->r, vm);
    return result;
}

void flecs_script_ir_call_function(
    const ecs_function_ctx_t *ctx,
    ecs_script_user_function_t *uf,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    ecs_script_impl_t *impl = flecs_script_impl(uf->script);
    bool failed = false;

    ecs_script_ir_t *ir = flecs_script_ir_ensure(impl);
    const ecs_script_ir_entry_t *entry = flecs_script_ir_entry(ir, uf->node);
    if (!entry) {
        ecs_err("script function has no compiled body");
        flecs_script_runtime_get(world)->error = true;
        return;
    }

    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    ecs_script_eval_desc_t desc = {
        .runtime = flecs_script_runtime_acquire_call(runtime)
    };
    ecs_script_ir_vm_t *vm = flecs_ir_vm_acquire(desc.runtime, ir);
    flecs_script_eval_visit_init(impl, &vm->v, &desc);
    ecs_script_eval_visitor_t *v = &vm->v;
    flecs_script_user_function_bind(v, uf, argc, argv, false);

    vm->result = result;
    if (flecs_script_ir_vm_run(vm, entry) != FlecsScriptRunDone) {
        failed = true;
    }

    v->vars = ecs_script_vars_pop(v->vars);
    flecs_ir_vm_release(desc.runtime, vm);
    flecs_script_runtime_release_call(runtime, desc.runtime);

    if (failed) {
        flecs_script_runtime_get(world)->error = true;
    }
}

#endif

#ifdef FLECS_SCRIPT_IR_PROFILE
void ecs_script_ir_profile_reset(void)
{
    ecs_os_memset_n(flecs_ir_profile_ops, 0, int64_t, EcsIrOpKindLast);
    ecs_os_memset_n(flecs_ir_profile_stats, 0, int64_t, EcsIrProfileLast);
}

static const char* flecs_ir_profile_stat_name(int32_t i) {
    switch(i) {
    case EcsIrProfileRun: return "vm runs";
    case EcsIrProfileStmtRun: return "statements run";
    case EcsIrProfileStmtSkipped: return "statements skipped";
    case EcsIrProfileBatchAdd: return "batch adds executed";
    case EcsIrProfileBatchSkip: return "batch adds skipped";
    case EcsIrProfileEntityCreate: return "entities created";
    case EcsIrProfileEntityReuse: return "entities reused";
    case EcsIrProfileComponentSet: return "components set";
    case EcsIrProfileTagAdd: return "tags added";
    default: return "?";
    }
}

char* ecs_script_ir_profile_str(void)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t i;
    int64_t total = 0;
    for (i = 0; i < EcsIrOpKindLast; i ++) {
        total += flecs_ir_profile_ops[i];
    }

    ecs_strbuf_append(&buf, "instructions: %lld\n", (long long)total);
    for (i = 0; i < EcsIrOpKindLast; i ++) {
        if (flecs_ir_profile_ops[i]) {
            ecs_strbuf_append(&buf, "  %-20s %10lld  %5.1f%%\n",
                flecs_script_ir_op_name((ecs_script_ir_op_kind_t)i),
                (long long)flecs_ir_profile_ops[i],
                100.0 * (double)flecs_ir_profile_ops[i] / (double)total);
        }
    }

    for (i = 0; i < EcsIrProfileLast; i ++) {
        ecs_strbuf_append(&buf, "%-22s %10lld\n", flecs_ir_profile_stat_name(i),
            (long long)flecs_ir_profile_stats[i]);
    }

    return ecs_strbuf_get(&buf);
}
#endif
