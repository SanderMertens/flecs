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
    flecs_script_eval_error_(&(vm)->v, (ecs_script_node_t*)(node), __VA_ARGS__)

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

static void flecs_ir_scratch_init(
    ecs_script_ir_vm_t *vm,
    char **ptr,
    int32_t *capacity,
    int32_t size)
{
    int32_t initial = vm->can_suspend ? 4096 : FLECS_IR_SCRATCH_SIZE;
    *capacity = size > initial ? size : initial;
    *ptr = ecs_os_malloc(*capacity);
}

static void* flecs_ir_scratch_alloc(
    ecs_script_ir_vm_t *vm,
    int32_t size,
    int32_t align)
{
    if (!vm->scratch) {
        flecs_ir_scratch_init(vm, &vm->scratch, &vm->scratch_size, size);
    }
    int32_t top = (vm->scratch_top + (align - 1)) & ~(align - 1);
    if (top + size <= vm->scratch_size) {
        vm->scratch_top = top + size;
        return vm->scratch + top;
    }
    void *ptr = ecs_os_malloc(size);
    ecs_vec_append_t(NULL, &vm->heap, void*)[0] = ptr;
    return ptr;
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
    if (!vm->vscratch) {
        flecs_ir_scratch_init(vm, &vm->vscratch, &vm->vscratch_size, ti->size);
    }
    int32_t align = ti->alignment;
    int32_t top = (vm->vscratch_top + (align - 1)) & ~(align - 1);
    void *ptr;
    if (top + ti->size <= vm->vscratch_size) {
        vm->vscratch_top = top + ti->size;
        ptr = vm->vscratch + top;
    } else {
        ptr = ecs_os_malloc(ti->size);
        ecs_vec_append_t(NULL, &vm->vheap, void*)[0] = ptr;
    }
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
    void *ptr = flecs_ir_scratch_alloc(vm, ti->size, ti->alignment);
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
        int32_t top = (vm->scratch_top + 15) & ~15;
        if (top + 16 <= vm->scratch_size) {
            void *out = vm->scratch + top;
            vm->scratch_top = top + 16;
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
    uint32_t index)
{
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

    int32_t heap_count = vm->heap.count;
    if (heap_count > frame->u.expr.heap_count) {
        void **heap = ecs_vec_first(&vm->heap);
        int32_t i;
        for (i = frame->u.expr.heap_count; i < heap_count; i ++) {
            ecs_os_free(heap[i]);
        }
        ecs_vec_set_count_t(NULL, &vm->heap, void*, frame->u.expr.heap_count);
    }

    vm->scratch_top = frame->u.expr.scratch_top;

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
    frame->u.expr.scratch_top = vm->scratch_top;
    frame->u.expr.owned_count = vm->owned.count;
    frame->u.expr.heap_count = vm->heap.count;
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
    int32_t heap_count = vm->vheap.count;
    if (heap_count > vheap_count) {
        void **heap = vm->vheap.array;
        for (i = vheap_count; i < heap_count; i ++) {
            ecs_os_free(heap[i]);
        }
        vm->vheap.count = vheap_count;
    }
    vm->vscratch_top = vscratch_top;
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
    ecs_script_eval_visitor_t *v = &vm->v;
    flecs_script_for_state_t *state = &frame->u.for_.state;
    flecs_ir_vars_truncate(vm, frame->u.for_.var_count,
        frame->u.for_.vscratch_top, frame->u.for_.vheap_count);
    flecs_script_for_merge_slots(v, state->for_slot, v->for_slot);
    v->for_slot = state->for_slot;
    v->force = state->force;
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
        const int32_t *pcs = ecs_vec_get_t(
            &vm->ir->scope_stmts, int32_t, block->a + 1);
        int32_t i;
        for (i = 0; i < block->c; i ++) {
            int32_t marks = vm->ops[pcs[i]].c;
            if (marks != -1) {
                flecs_ir_mark(vm, marks);
            }
        }
        return;
    }
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_region_t *region = ecs_vec_get_t(
        &v->base.script->regions, ecs_script_region_t, index);
    int32_t i;

    if (v->scope_slots) {
        int32_t count = ecs_vec_count(v->scope_slots);
        int32_t *array = ecs_vec_first(v->scope_slots);
        for (i = 0; i < region->scope_count; i ++) {
            int32_t slot = region->scope_first + i;
            if (slot >= 0 && slot < count) {
                array[slot] = v->visit;
            }
        }
    }

    if (v->for_slots) {
        int32_t count = ecs_vec_count(v->for_slots);
        for (i = 0; i < region->for_count; i ++) {
            int32_t slot = region->for_first + i;
            if (slot >= 0 && slot < count) {
                flecs_script_for_slot_mark(ecs_vec_get_t(
                    v->for_slots, ecs_script_for_slot_t, slot), v->visit);
            }
        }
    }
}

static ecs_entity_t flecs_ir_lookup_name(
    ecs_script_ir_vm_t *vm,
    const char *name)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .vars = v->vars,
        .runtime = v->r,
        .disable_dynamic_variable_binding = true,
        .script_visitor = v
    };
    flecs_script_symbol_t symbol;
    if (flecs_script_symbol_lookup(script, &desc, 0, name,
        FlecsScriptLookupEntity | FlecsScriptLookupDynamic, &symbol))
    {
        return 0;
    }
    return symbol.entity;
}

static int flecs_ir_id_elem(
    ecs_script_ir_vm_t *vm,
    const void *node,
    int32_t reg,
    ecs_entity_t eval,
    int32_t slot,
    int32_t sp,
    ecs_entity_t *out)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_entity_t result = eval;
    if (reg != -1) {
        const char *name = *(char**)flecs_ir_reg(vm, reg)->value.ptr;
        if (!name) {
            flecs_ir_error(vm, node, "failed to evaluate entity name");
            return -1;
        }
        result = flecs_ir_lookup_name(vm, name);
    } else if (slot != -1) {
        result = flecs_script_symbol_entity(v, slot);
    } else if (sp != -1) {
        ecs_script_var_t *var = ecs_script_vars_from_sp(v->vars, sp);
        result = 0;
        if (var && var->value.type == ecs_id(ecs_entity_t) &&
            var->value.ptr)
        {
            result = *(ecs_entity_t*)var->value.ptr;
        }
    }

    if (result == EcsWildcard || result == EcsAny) {
        flecs_ir_error(vm, node, "cannot use wildcard entity as id element");
        return -1;
    }

    *out = result;
    return 0;
}

static int flecs_ir_id_resolve(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const void *node,
    ecs_id_t *out)
{
    if (!desc->dynamic) {
        *out = desc->eval;
        return 0;
    }

    ecs_entity_t first;
    if (flecs_ir_id_elem(vm, node, desc->first_reg, desc->first_eval,
        desc->first_symbol, desc->first_sp, &first))
    {
        return -1;
    }
    if (!first) {
        flecs_ir_error(vm, node, "unresolved identifier '%s'", desc->first);
        return -1;
    }

    if (desc->has_second) {
        ecs_entity_t second;
        if (flecs_ir_id_elem(vm, node, desc->second_reg, desc->second_eval,
            desc->second_symbol, desc->second_sp, &second))
        {
            return -1;
        }
        if (!second) {
            flecs_ir_error(vm, node,
                "unresolved identifier '%s'", desc->second);
            return -1;
        }
        *out = desc->flag | ecs_pair(first, second);
    } else {
        *out = desc->flag | first;
    }
    return 0;
}

static inline ecs_entity_t flecs_ir_symbol_entity(
    const ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    if (v->symbol_slots && slot >= v->symbol_offset) {
        return ((ecs_script_symbol_slot_t*)v->symbol_slots->array)
            [slot - v->symbol_offset].entity;
    }
    return ((ecs_script_symbol_slot_t*)v->base.script->symbol_slots.array)
        [slot].entity;
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
    if (desc->first_reg == -1 && desc->first_sp == -1) {
        ecs_entity_t first = desc->first_symbol != -1
            ? flecs_ir_symbol_entity(&vm->v, desc->first_symbol)
            : desc->first_eval;
        if (first && first != EcsWildcard && first != EcsAny) {
            if (!desc->has_second) {
                *out = desc->flag | first;
                return 0;
            }
            if (desc->second_reg == -1 && desc->second_sp == -1) {
                ecs_entity_t second = desc->second_symbol != -1
                    ? flecs_ir_symbol_entity(&vm->v, desc->second_symbol)
                    : desc->second_eval;
                if (second && second != EcsWildcard && second != EcsAny) {
                    *out = desc->flag | ecs_pair(first, second);
                    return 0;
                }
            }
        }
    }
    return flecs_ir_id_resolve(vm, desc, node, out);
}

static void flecs_ir_track(
    ecs_script_eval_visitor_t *v,
    int32_t component_slot,
    ecs_id_t component)
{
    flecs_script_entity_state_t *entity = v->entity;
    if (entity && entity->for_slot < 0 && entity->symbol >= 0 &&
        component_slot >= 0 && v->component_slots &&
        component_slot < ecs_vec_count(v->component_slots))
    {
        ecs_script_component_slot_t *slot = ecs_vec_get_t(
            v->component_slots, ecs_script_component_slot_t, component_slot);
        if (slot->component == component &&
            slot->entity_slot == entity->symbol)
        {
            slot->scope_slot = v->scope_slot;
            return;
        }
    }
    flecs_script_track_component(v, component_slot, component);
}

static const ecs_script_ir_id_t* flecs_ir_id_desc(
    ecs_script_ir_vm_t *vm,
    int32_t index)
{
    return ecs_vec_get_t(&vm->ir->ids, ecs_script_ir_id_t, index);
}

static int flecs_ir_default_ctor_check(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const void *node,
    ecs_id_t id)
{
    if (desc->resolved) {
        return 0;
    }
    if (!flecs_script_can_default_ctor(vm->v.world, id)) {
        if (desc->has_second) {
            flecs_ir_error(vm, node,
                "cannot add (%s, %s), type is not default constructible",
                desc->first, desc->second);
        } else {
            flecs_ir_error(vm, node,
                "cannot add %s, type is not default constructible",
                desc->first);
        }
        return -1;
    }
    return 0;
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
    return flecs_script_get_type_info(&vm->v, (void*)node, id);
}

static int flecs_ir_tag(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_tag_t *node = op->node;
    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    if (id == ecs_id(EcsParent)) {
        flecs_ir_error(vm, node,
            "Parent component cannot be added as tag (set to valid parent)");
        return -1;
    }

    if (flecs_ir_default_ctor_check(vm, desc, node, id)) {
        return -1;
    }

    ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);

    if (v->entity->eval_kind == ecs_id(EcsStruct) && ECS_IS_PAIR(id) &&
        ECS_PAIR_FIRST(id) == EcsIsA)
    {
        ecs_entity_t base = ecs_pair_second(v->world, id);
        if (!ecs_has(v->world, base, EcsStruct)) {
            flecs_ir_error(vm, node,
                "base '%s' of struct '%s' is not a struct type",
                desc->second, v->entity->node->name);
            return -1;
        }
        if (base == v->entity->eval) {
            flecs_ir_error(vm, node,
                "struct '%s' cannot inherit from itself",
                v->entity->node->name);
            return -1;
        }
        if (flecs_struct_is_derived_from(v->world, base, v->entity->eval)) {
            flecs_ir_error(vm, node,
                "inheritance cycle between struct '%s' and base '%s'",
                v->entity->node->name, desc->second);
            return -1;
        }
        int32_t i = 0;
        ecs_entity_t existing;
        while ((existing = ecs_get_target(
            v->world, v->entity->eval, EcsIsA, i ++)))
        {
            if (existing != base && ecs_has(v->world, existing, EcsStruct)) {
                flecs_ir_error(vm, node,
                    "struct '%s' cannot have multiple base types",
                    v->entity->node->name);
                return -1;
            }
        }
    }

    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, id);

    if (desc->value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, (void*)node, desc->value_sp, id);
        if (!var) {
            return -1;
        }
        const ecs_type_info_t *ti = flecs_ir_id_type_info(vm, desc, node, id);
        if (!ti) {
            return -1;
        }
        ecs_set_id(v->world, src, id, flecs_itosize(ti->size), var->value.ptr);
    } else {
        flecs_ir_prof(EcsIrProfileTagAdd);
        ecs_add_id(v->world, src, id);
    }

    flecs_ir_track(v, desc->component_slot, id);
    return 0;
}

static int flecs_ir_interface_check(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_id_t *desc,
    const void *node)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_entity_t tmpl;
    if (flecs_ir_id_elem(vm, node, desc->first_reg, desc->first_eval,
        desc->first_symbol, desc->first_sp, &tmpl))
    {
        return -1;
    }

    char *interface_str = ecs_get_path(v->world, desc->interface);
    if (!tmpl) {
        flecs_ir_error(vm, node,
            "template prop '%s' has no value: expected a template derived "
            "from '%s'", desc->first, interface_str);
        ecs_os_free(interface_str);
        return -1;
    }

    const EcsScript *script = ecs_get(v->world, tmpl, EcsScript);
    if (!script || !script->template_ ||
        !flecs_struct_is_derived_from(v->world, tmpl, desc->interface))
    {
        char *tmpl_str = ecs_get_path(v->world, tmpl);
        flecs_ir_error(vm, node,
            "'%s' passed to template prop '%s' is not a template derived "
            "from '%s'", tmpl_str, desc->first, interface_str);
        ecs_os_free(tmpl_str);
        ecs_os_free(interface_str);
        return -1;
    }

    ecs_os_free(interface_str);
    return 0;
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
    if (op->flags & 1) {
        visitor = ecs_get(v->world, id, EcsScriptVisitor);
    } else if (!ECS_IS_PAIR(id)) {
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

    ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);
    if (v->entity->eval == EcsVariable) {
        flecs_ir_error(vm, node,
            "cannot use '%s' in singleton scope", desc->first);
        return -1;
    }

    if (node->expr && node->expr->kind != EcsExprInitializer &&
        node->expr->kind != EcsExprEmptyInitializer)
    {
        flecs_ir_error(vm, node, "expected initializer for '%s'", desc->first);
        return -1;
    }

    ecs_script_visitor_ctx_t ctx = {
        .world = v->world,
        .entity = v->entity->eval,
        .kind = id,
        .initializer = (ecs_expr_initializer_t*)node->expr,
        .eval = v,
        .ctx = visitor->ctx
    };

    ecs_assert(visitor->visit != NULL, ECS_INVALID_OPERATION,
        "EcsScriptVisitor component without visit callback");

    v->base.nodes[0] = ECS_CONST_CAST(ecs_script_node_t*, (const void*)node);
    v->base.depth = 1;
    int result = visitor->visit(&ctx);
    v->base.depth = 0;
    *handled = true;
    return result;
}

static int flecs_ir_component_begin(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_id_t *id_out)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_component_t *node = op->node;

    if (desc->interface && flecs_ir_interface_check(vm, desc, node)) {
        return -1;
    }

    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    if (flecs_ir_default_ctor_check(vm, desc, node, id)) {
        return -1;
    }

    ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, id);
    if (src == id) {
        const EcsScript *tmpl = ecs_get(v->world, id, EcsScript);
        if (tmpl && tmpl->template_) {
            flecs_ir_error(vm, node,
                "cannot instantiate template '%s' on itself", desc->first);
            return -1;
        }
    }

    *id_out = id;
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

    if (!desc->ti && node->expr && ti->component != node->expr->type &&
        (node->expr->kind == EcsExprInitializer ||
         node->expr->kind == EcsExprEmptyInitializer) &&
        !flecs_struct_is_derived_from(v->world, ti->component,
            node->expr->type))
    {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        return -1;
    }

    ecs_script_ir_reg_t *tmp = flecs_ir_reg(vm, op->b);
    void *ptr = flecs_ir_reg_alloc(vm, tmp, ti);

    if (desc->value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, (void*)node, desc->value_sp, id);
        if (!var) {
            return -1;
        }
        ecs_ptr_copy_w_type_info(v->world, ti, ptr, var->value.ptr);
    } else if (op->flags & EcsIrComponentPartial) {
        ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, id);
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
            flecs_type_info_move(dst_ptr, src->value.ptr, 1, dst_ti);
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
        ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, id);
        if (desc->interface) {
            const ecs_type_info_t *ti = flecs_ir_id_type_info(
                vm, desc, node, id);
            if (!ti) {
                return -1;
            }
            void *value = ecs_os_alloca(ti->size);
            flecs_type_info_ctor(value, 1, ti);
            ecs_set_id(v->world, src, id, flecs_itosize(ti->size), value);
        } else {
            ecs_add_id(v->world, src, id);
        }
        flecs_ir_track(v, desc->component_slot, id);
        return 0;
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

    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, id);
    flecs_ir_prof(EcsIrProfileComponentSet);
    if (desc->needs_set || (!desc->ti && ti->hooks.on_replace)) {
        ecs_set_id(v->world, src, id, flecs_itosize(ti->size), value);
    } else {
        void *dst = ecs_ensure_id(v->world, src, id, flecs_itosize(ti->size));
        flecs_type_info_move_dtor(dst, value, 1, ti);
        tmp->owned = false;
        flecs_script_modified(v->world, src, id, ti);
    }

    flecs_ir_track(v, desc->component_slot, id);
    return 0;
}


static int flecs_ir_with_tag(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_tag_t *node = op->node;
    ecs_id_t id;
    if (flecs_ir_id_get(vm, desc, node, &id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    if (desc->value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, (void*)node, desc->value_sp, id);
        if (!var) {
            return -1;
        }
        const ecs_type_info_t *ti = flecs_ir_id_type_info(vm, desc, node, id);
        if (!ti) {
            return -1;
        }
        ecs_value_t *value = flecs_script_with_append(a, v, ti);
        value->type = id;
        value->ptr = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
        flecs_type_info_ctor(value->ptr, 1, ti);
        ecs_ptr_copy_w_type_info(v->world, ti, value->ptr, var->value.ptr);
        return 0;
    }

    ecs_value_t *value = flecs_script_with_append(a, v, NULL);
    value->type = id;
    value->ptr = NULL;
    return 0;
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

    ecs_allocator_t *a = &v->r->allocator;
    const ecs_type_info_t *ti = desc->ti;
    if (!ti) {
        ti = flecs_script_get_type_info(v, (void*)node, id);
    }

    ecs_value_t *value = flecs_script_with_append(a, v, ti);
    value->type = id;
    value->ptr = NULL;

    if (op->b == -1) {
        return 0;
    }

    if (!ti) {
        return -1;
    }

    if (!desc->ti && node->expr && ti->component != node->expr->type &&
        (node->expr->kind == EcsExprInitializer ||
         node->expr->kind == EcsExprEmptyInitializer) &&
        !flecs_struct_is_derived_from(v->world, ti->component,
            node->expr->type))
    {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        return -1;
    }

    value->ptr = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
    flecs_type_info_ctor(value->ptr, 1, ti);

    if (desc->value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, (void*)node, desc->value_sp, id);
        if (!var) {
            return -1;
        }
        ecs_ptr_copy_w_type_info(v->world, ti, value->ptr, var->value.ptr);
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
    ecs_value_t *with_value = ecs_vec_get_t(
        &v->r->with, ecs_value_t, flecs_script_with_count(v) - 1);

    ecs_script_ir_reg_t *val = flecs_ir_reg(vm, op->b);
    if (flecs_ir_value_to(vm, val, ti->component, with_value->ptr, ti)) {
        flecs_ir_expr_error(vm, node->expr, "failed to write to output");
        return -1;
    }
    return 0;
}

static int flecs_ir_entity_enter(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    flecs_script_entity_state_t *state)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_entity_t *node = ECS_CONST_CAST(
        ecs_script_entity_t*, (const void*)op->node);

    state->node = node;
    state->eval = 0;
    state->eval_kind = 0;
    state->prev_entity = v->entity;
    state->prev_is_with_scope = v->is_with_scope;
    state->prev_template_entity = v->template_entity;
    state->prev_force = v->force;
    state->symbol = -1;
    state->for_slot = -1;
    state->created = false;

    if (v->entity && v->entity->eval_kind && !node->kind &&
        !ecs_vec_count(&node->scope->stmts) && ecs_has(
            v->world, v->entity->eval_kind, EcsScriptVisitor))
    {
        const char *parent_kind = v->entity->node->kind;
        flecs_ir_error(vm, node,
            "invalid declaration for '%s', did you mean '%s %s(%s ...)'?",
                node->name ? node->name : "",
                parent_kind ? parent_kind : "struct",
                v->entity->node->name,
                node->name ? node->name : "");
        return -1;
    }

    if (node->kind) {
        state->eval_kind = node->eval_kind;
        if (node->kind_symbol != -1) {
            state->eval_kind = flecs_script_symbol_entity(
                v, node->kind_symbol);
        } else if (node->kind_sp != -1) {
            ecs_script_var_t *var = ecs_script_vars_from_sp(
                v->vars, node->kind_sp);
            if (var && var->value.ptr &&
                var->value.type == ecs_id(ecs_entity_t))
            {
                state->eval_kind = *(ecs_entity_t*)var->value.ptr;
            }
        }
        if (!state->eval_kind) {
            flecs_ir_error(vm, node, "unresolved identifier '%s'", node->kind);
            return -1;
        }
    }

    if (node->symbol != -1) {
        state->symbol = flecs_script_symbol_runtime_slot(v, node->symbol);
        state->eval = flecs_script_symbol_entity(v, node->symbol);
        if (state->eval && (!ecs_is_alive(v->world, state->eval) ||
            ecs_get_target(v->world, state->eval, EcsChildOf, 0) != v->parent))
        {
            state->eval = 0;
        }
    } else if (node->eval && ecs_is_alive(v->world, node->eval)) {
        state->eval = node->eval;
    }

    bool apply_with = state->eval != 0;
    if (op->a != -1) {
        const char *name = *(char**)flecs_ir_reg(vm, op->a)->value.ptr;
        if (!name) {
            flecs_ir_error(vm, node, "failed to evaluate entity name");
            return -1;
        }
        ecs_entity_t eval = flecs_script_create_entity(v, name);
        if (!eval) {
            return -1;
        }
        state->created = true;
        if (state->eval && state->eval != eval &&
            ecs_is_alive(v->world, state->eval))
        {
            ecs_delete(v->world, state->eval);
        }
        state->eval = eval;
    } else if (!state->eval) {
        flecs_ir_prof(EcsIrProfileEntityCreate);
        state->eval = flecs_script_create_entity(v, node->name);
        state->created = true;
    } else {
        flecs_ir_prof(EcsIrProfileEntityReuse);
    }
    if (apply_with && op->a == -1 && v->force) {
        flecs_script_apply_with(v, state->eval);
    }
    if (node->symbol != -1) {
        flecs_script_symbol_set(v, node->symbol, state->eval);
    }

    if (v->for_slot >= 0 && v->for_slots &&
        v->for_slot < ecs_vec_count(v->for_slots) &&
        !flecs_script_is_builtin(v->world, state->eval))
    {
        bool named = false;
        flecs_script_for_slot_track(v->world, ecs_vec_get_t(
            v->for_slots, ecs_script_for_slot_t, v->for_slot),
            state->eval, v->visit, &named);
        if (named) {
            state->for_slot = v->for_slot;
        }
    }

    if (v->body_template && (state->created || v->force)) {
        ecs_add_pair(
            v->world, state->eval, EcsScriptTemplate, v->body_template);
    }

    v->entity = state;

    if (state->eval_kind && (state->created || v->force ||
        (node->node.direct_input & v->input) ||
        (node->node.direct_internal & v->internal)))
    {
        if (state->eval_kind == state->eval) {
            const EcsScript *tmpl = ecs_get(
                v->world, state->eval_kind, EcsScript);
            if (tmpl && tmpl->template_) {
                flecs_ir_error(vm, node,
                    "cannot instantiate template '%s' on itself", node->kind);
                goto error;
            }
        }
        ecs_add_id(v->world, state->eval, state->eval_kind);
    }

    int32_t i, count = ecs_vec_count(&v->r->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = 0; i < count ; i ++) {
            if (flecs_script_apply_annot(v, node, state->eval, annots[i])) {
                goto error;
            }
        }
        ecs_vec_clear(&v->r->annot);
    }

    v->is_with_scope = false;
    v->template_entity = 0;
    v->force = state->prev_force ||
        ((node->node.direct_input & v->input) != 0) ||
        ((node->node.direct_internal & v->internal) != 0);
    return 0;
error:
    v->entity = state->prev_entity;
    v->force = state->prev_force;
    return -1;
}

static int flecs_ir_pair_scope_enter(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    flecs_script_pair_scope_state_t *state)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_script_ir_id_t *desc = flecs_ir_id_desc(vm, op->a);
    const ecs_script_pair_scope_t *node = op->node;

    ecs_entity_t first;
    if (flecs_ir_id_elem(vm, node, desc->first_reg, desc->first_eval,
        desc->first_symbol, desc->first_sp, &first))
    {
        return -1;
    }
    if (!first && desc->first_symbol != -1) {
        first = flecs_script_create_entity(v, desc->first);
        if (!first) {
            return -1;
        }
        flecs_script_symbol_set(v, desc->first_symbol, first);
    } else if (!first) {
        return -1;
    }

    ecs_entity_t second;
    if (flecs_ir_id_elem(vm, node, desc->second_reg, desc->second_eval,
        desc->second_symbol, desc->second_sp, &second))
    {
        return -1;
    }
    if (!second && desc->second_symbol != -1) {
        second = flecs_script_create_entity(v, desc->second);
        if (second) {
            flecs_script_symbol_set(v, desc->second_symbol, second);
        }
    }
    if (!second) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    state->with_relationship = v->with_relationship;
    state->second = 0;
    state->with_relationship_sp = v->with_relationship_sp;
    state->force = v->force;

    v->with_relationship = first;

    if (state->with_relationship != first) {
        ecs_value_t *value = flecs_script_with_append(a, v, NULL);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
        v->with_relationship_sp = flecs_script_with_count(v) - 1;
    } else {
        ecs_value_t *value = ecs_vec_get_t(
            &v->r->with, ecs_value_t, v->with_relationship_sp);
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first,
            ECS_INTERNAL_ERROR, NULL);
        state->second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
    }

    v->force = state->force ||
        ((node->node.direct_input & v->input) != 0) ||
        ((node->node.direct_internal & v->internal) != 0);
    return 0;
}

static int flecs_ir_reg_to_i32(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_reg_t *reg,
    int32_t *out)
{
    if (reg->value.type == ecs_id(ecs_i32_t)) {
        *out = *(int32_t*)reg->value.ptr;
        return 0;
    }
    ecs_meta_cursor_t cur = ecs_meta_cursor(
        vm->v.world, ecs_id(ecs_i32_t), out);
    return ecs_meta_set_value(&cur, &reg->value);
}

static ecs_script_var_t* flecs_ir_for_var(
    ecs_script_ir_vm_t *vm,
    ecs_entity_t type,
    bool alloc,
    int32_t *index)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_var_t *var = flecs_ir_var_declare(vm);
    *index = v->vars->vars.count - 1;
    var->value.type = type;
    if (alloc) {
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        var->value.ptr = flecs_ir_var_alloc(vm, ti);
        var->type_info = ti;
        var->owned = true;
    }
    return var;
}

static int flecs_ir_for_enter(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    flecs_script_for_state_t *state,
    int32_t *var_index)
{
    var_index[0] = var_index[1] = var_index[2] = -1;
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_for_t *node = ECS_CONST_CAST(
        ecs_script_for_t*, (const void*)op->node);

    ecs_os_zeromem(state);
    state->for_slot = v->for_slot;
    state->force = v->force;

    bool visited = v->scope_slots && node->scope->scope_slot >= 0 &&
        node->scope->scope_slot < ecs_vec_count(v->scope_slots) &&
        ecs_vec_get_t(v->scope_slots,
            int32_t, node->scope->scope_slot)[0] == v->visit;
    if (!visited && v->for_slots && node->for_slot >= 0 &&
        node->for_slot < ecs_vec_count(v->for_slots))
    {
        flecs_script_for_slot_clear(v->world, ecs_vec_get_t(
            v->for_slots, ecs_script_for_slot_t, node->for_slot), false);
    }

    ecs_entity_t key_type = 0;
    ecs_entity_t elem_type = ecs_id(ecs_i32_t);

    if (!node->expr) {
        int32_t from, to;
        if (flecs_ir_reg_to_i32(vm, flecs_ir_reg(vm, op->a), &from)) {
            flecs_ir_expr_error(vm, node->from, "failed to write to output");
            return -1;
        }
        if (flecs_ir_reg_to_i32(vm, flecs_ir_reg(vm, op->b), &to)) {
            flecs_ir_expr_error(vm, node->to, "failed to write to output");
            return -1;
        }
        state->kind = FlecsScriptForRange;
        state->from = from;
        state->count = to;
    } else {
        ecs_script_ir_reg_t *coll = flecs_ir_reg(vm, op->a);
        ecs_entity_t type = coll->value.type;
        if (flecs_script_for_collection_kind(v, node, type,
            &state->kind, &key_type, &elem_type))
        {
            return -1;
        }

        if (state->kind == FlecsScriptForArray) {
            const EcsArray *array = ecs_get(v->world, type, EcsArray);
            state->elems = coll->value.ptr;
            state->count = array->count;
        } else if (state->kind == FlecsScriptForVector) {
            ecs_vec_t *vec = coll->value.ptr;
            state->elems = ecs_vec_first(vec);
            state->count = ecs_vec_count(vec);
        } else {
            state->map_it = ecs_map_iter(coll->value.ptr);
        }
    }

    if (flecs_script_for_check_var_count(v, node, state->kind)) {
        return -1;
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(v->world, elem_type);
    if (!elem_ti) {
        flecs_ir_error(vm, node,
            "for loop element type is not a valid type");
        return -1;
    }

    state->elem_size = elem_ti->size;

    if (state->kind == FlecsScriptForMap && node->loop_var_count >= 2) {
        const ecs_type_info_t *key_ti = ecs_get_type_info(v->world, key_type);
        ecs_assert(key_ti != NULL, ECS_INTERNAL_ERROR, NULL);
        state->key_size = key_ti->size;
        state->key_var = flecs_ir_for_var(vm, key_type, true, &var_index[1]);
    }

    if (flecs_script_for_has_index_var(state->kind, node->loop_var_count)) {
        state->index_var = flecs_ir_for_var(
            vm, ecs_id(ecs_i32_t), true, &var_index[2]);
    }

    state->elem_var = flecs_ir_for_var(vm, elem_type,
        state->kind == FlecsScriptForRange, &var_index[0]);

    v->for_slot = node->for_slot;
    v->force = true;
    return 0;
}

static bool flecs_ir_for_next(
    ecs_script_ir_vm_t *vm,
    ecs_script_ir_frame_t *frame)
{
    flecs_script_for_state_t *state = &frame->u.for_.state;
    ecs_script_var_t *vars = vm->v.vars->vars.array;
    state->elem_var = &vars[frame->u.for_.var_index[0]];
    if (frame->u.for_.var_index[1] != -1) {
        state->key_var = &vars[frame->u.for_.var_index[1]];
    }
    if (frame->u.for_.var_index[2] != -1) {
        state->index_var = &vars[frame->u.for_.var_index[2]];
    }
    int32_t i = state->index;

    switch(state->kind) {
    case FlecsScriptForRange: {
        int32_t value = state->from + i;
        if (value >= state->count) {
            return false;
        }
        *(int32_t*)state->elem_var->value.ptr = value;
        break;
    }
    case FlecsScriptForArray:
    case FlecsScriptForVector:
        if (i >= state->count) {
            return false;
        }
        state->elem_var->value.ptr = ECS_OFFSET(
            state->elems, state->elem_size * i);
        state->elem_var->owned = false;
        break;
    case FlecsScriptForMap:
        if (!ecs_map_next(&state->map_it)) {
            return false;
        }
        if (state->key_var) {
            ecs_map_key_t key = ecs_map_key(&state->map_it);
            ecs_os_memcpy(state->key_var->value.ptr, &key, state->key_size);
        }
        if (state->elem_size > ECS_SIZEOF(ecs_map_val_t)) {
            state->elem_var->value.ptr = ecs_map_ptr(&state->map_it);
        } else {
            state->elem_var->value.ptr = &state->map_it.res[1];
        }
        state->elem_var->owned = false;
        break;
    }

    if (state->index_var) {
        *(int32_t*)state->index_var->value.ptr = i;
    }

    state->index = i + 1;
    return true;
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

    ecs_script_var_t *var = flecs_ir_var_declare(vm);
    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);

    if (op->b >= 0 && v->computed && op->b < v->computed_count) {
        ecs_script_computed_t *slot = &v->computed[op->b];
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
        var->is_const = true;
        var->type_info = ti;
        var->value.type = type;
        var->value.ptr = slot->ptr;
        var->owned = false;
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
    var->is_const = true;
    var->type_info = ti;
    var->value.type = type;
    var->value.ptr = ptr;
    var->owned = true;
    return 0;
}

static int flecs_ir_using(
    ecs_script_ir_vm_t *vm,
    const ecs_script_using_t *node)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_allocator_t *a = &v->r->allocator;
    int32_t len = ecs_os_strlen(node->name);
    bool wildcard = len > 2 && !ecs_os_strcmp(&node->name[len - 2], ".*");
    ecs_entity_t from = node->eval;
    ecs_assert(from != 0, ECS_INTERNAL_ERROR, NULL);

    if (wildcard) {
        ecs_iter_t it = ecs_children(v->world, from);
        while (ecs_children_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                ecs_vec_append_t(
                    a, &v->r->using, ecs_entity_t)[0] = it.entities[i];
            }
        }
    } else {
        ecs_vec_append_t(a, &v->r->using, ecs_entity_t)[0] = from;
    }
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
    ecs_vec_append_t(&v->r->allocator, &v->r->annot,
        ecs_script_annot_t*)[0] = ECS_CONST_CAST(
            ecs_script_annot_t*, node);
    return 0;
}

static int flecs_ir_to_bool(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
    if (reg->value.type == ecs_id(ecs_bool_t)) {
        vm->cond = *(bool*)reg->value.ptr;
        return 0;
    }

    const EcsType *type = ecs_get(v->world, reg->value.type, EcsType);
    if (!type || (type->kind != EcsPrimitiveType &&
        type->kind != EcsEnumType && type->kind != EcsBitmaskType))
    {
        char *type_str = ecs_get_path(v->world, reg->value.type);
        flecs_ir_error(vm, op->node,
            "value of type '%s' cannot be used as condition", type_str);
        ecs_os_free(type_str);
        return -1;
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        v->world, reg->value.type, reg->value.ptr);
    vm->cond = ecs_meta_get_bool(&cur);
    return 0;
}

static int flecs_ir_load_symbol(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_entity_t entity)
{
    const ecs_expr_identifier_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    if (!entity) {
        flecs_ir_expr_error(vm, node,
            "unresolved identifier '%s'", node->value);
        return -1;
    }

    if (op->flags & EcsIrIdDirect) {
        *(ecs_entity_t*)flecs_ir_reg_out_raw(vm, op, dst) = entity;
        return 0;
    }
    void *ptr = flecs_ir_reg_out(vm, op, dst);

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        vm->v.world, dst->value.type, ptr);
    if (ecs_meta_set_entity(&cur, entity)) {
        flecs_ir_expr_error(vm, node,
            "failed to assign identifier '%s'", node->value);
        return -1;
    }
    return 0;
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
    if (!var) {
        flecs_ir_expr_error(vm, node,
            "unresolved variable '%s'", node->name);
        return -1;
    }
    if (!var->value.ptr) {
        flecs_ir_expr_error(vm, node,
            "variable '%s' is not a compile time constant", node->name);
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
    ecs_id_t component = op->flags
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
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *expr = flecs_ir_reg(vm, op->b);
    ecs_script_ir_reg_t *index = flecs_ir_reg(vm, op->c);

    if (op->flags & EcsIrElementMap) {
        ecs_map_key_t key = 0;
        if (flecs_value_blit_u64(vm->v.world, &index->value, &key)) {
            return -1;
        }
        const ecs_map_t *map = expr->value.ptr;
        ecs_map_val_t *val = NULL;
        if (ecs_map_is_init(map)) {
            val = ecs_map_get(map, key);
        }
        if (!val) {
            char *key_str = ecs_ptr_to_str(
                vm->v.world, index->value.type, index->value.ptr);
            flecs_ir_expr_error(vm, node,
                "map does not contain key '%s'", key_str);
            ecs_os_free(key_str);
            return -1;
        }
        void *ptr = node->elem_size > ECS_SIZEOF(ecs_map_val_t)
            ? (void*)(uintptr_t)val[0]
            : (void*)val;
        flecs_ir_reg_borrow(dst, &node->node, node->node.type, ptr);
        return 0;
    }

    int64_t index_value = *(int64_t*)index->value.ptr;
    void *data = expr->value.ptr;
    int64_t elem_count = node->elem_count;
    if (op->flags & EcsIrElementVector) {
        data = ecs_vec_first(expr->value.ptr);
        if (!elem_count) {
            elem_count = ecs_vec_count(expr->value.ptr);
        }
    }

    if (index_value < 0 || index_value >= elem_count) {
        flecs_ir_expr_error(vm, node,
            "index %" PRId64 " is out of range for collection (count = %"
                PRId64 ")", index_value, elem_count);
        return -1;
    }

    flecs_ir_reg_borrow(dst, &node->node, node->node.type,
        ECS_OFFSET(data, node->elem_size * index_value));
    return 0;
}

static int flecs_ir_component_get(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_expr_element_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->b);
    ecs_entity_t entity = *(ecs_entity_t*)left->value.ptr;
    ecs_entity_t component = op->imm.entity;

    if (!entity || !ecs_is_alive(v->world, entity)) {
        char *cstr = ecs_get_path(v->world, component);
        flecs_ir_expr_error(vm, node,
            "cannot read component '%s' from invalid entity", cstr);
        ecs_os_free(cstr);
        return -1;
    }

    if (node->dyn_input && v->script_entity && !v->instance_template) {
        flecs_script_record_dyn_ref(v->base.script, entity, component,
            node->dyn_input, false);
    }

    const void *ptr = ecs_get_id(v->world, entity, component);
    if (!ptr) {
        char *estr = ecs_get_path(v->world, entity);
        char *cstr = ecs_get_path(v->world, component);
        flecs_ir_expr_error(vm, node,
            "entity '%s' does not have component '%s'", estr, cstr);
        ecs_os_free(estr);
        ecs_os_free(cstr);
        return -1;
    }

    flecs_ir_reg_borrow(dst, &node->node, node->node.type,
        ECS_CONST_CAST(void*, ptr));
    return 0;
}

static int flecs_ir_has(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_script_eval_visitor_t *v = &vm->v;
    const ecs_expr_has_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->b);
    ecs_entity_t entity = *(ecs_entity_t*)left->value.ptr;
    bool *out = flecs_ir_reg_out_raw(vm, op, dst);

    if (entity && ecs_is_alive(v->world, entity)) {
        if (node->dyn_input && v->script_entity && !v->instance_template) {
            flecs_script_record_dyn_ref(v->base.script, entity, op->imm.id,
                node->dyn_input, true);
        }
        *out = ecs_has_id(v->world, entity, op->imm.id);
    } else {
        *out = false;
    }
    return 0;
}

static bool flecs_ir_num_load(
    ecs_script_ir_num_class_t class,
    int32_t size,
    const void *ptr,
    int64_t *i,
    uint64_t *u,
    double *f)
{
    switch(class) {
    case EcsIrNumSigned:
        switch(size) {
        case 1: *i = *(const int8_t*)ptr; return true;
        case 2: *i = *(const int16_t*)ptr; return true;
        case 4: *i = *(const int32_t*)ptr; return true;
        case 8: *i = *(const int64_t*)ptr; return true;
        }
        return false;
    case EcsIrNumUnsigned:
        switch(size) {
        case 1: *u = *(const uint8_t*)ptr; return true;
        case 2: *u = *(const uint16_t*)ptr; return true;
        case 4: *u = *(const uint32_t*)ptr; return true;
        case 8: *u = *(const uint64_t*)ptr; return true;
        }
        return false;
    case EcsIrNumFloat:
        switch(size) {
        case 4: *f = (double)*(const float*)ptr; return true;
        case 8: *f = *(const double*)ptr; return true;
        }
        return false;
    case EcsIrNumNone:
        return false;
    }
    return false;
}

static void flecs_ir_num_store_i(
    ecs_script_ir_num_class_t class,
    int32_t size,
    void *ptr,
    int64_t value)
{
    switch(class) {
    case EcsIrNumSigned:
        switch(size) {
        case 1: *(int8_t*)ptr = (int8_t)value; return;
        case 2: *(int16_t*)ptr = (int16_t)value; return;
        case 4: *(int32_t*)ptr = (int32_t)value; return;
        case 8: *(int64_t*)ptr = value; return;
        }
        return;
    case EcsIrNumUnsigned:
        switch(size) {
        case 1: *(uint8_t*)ptr = (uint8_t)value; return;
        case 2: *(uint16_t*)ptr = (uint16_t)value; return;
        case 4: *(uint32_t*)ptr = (uint32_t)value; return;
        case 8: *(uint64_t*)ptr = (uint64_t)value; return;
        }
        return;
    case EcsIrNumFloat:
        switch(size) {
        case 4: *(float*)ptr = (float)value; return;
        case 8: *(double*)ptr = (double)value; return;
        }
        return;
    case EcsIrNumNone:
        return;
    }
}

static void flecs_ir_num_store_u(
    ecs_script_ir_num_class_t class,
    int32_t size,
    void *ptr,
    uint64_t value)
{
    switch(class) {
    case EcsIrNumSigned:
        switch(size) {
        case 1: *(int8_t*)ptr = (int8_t)value; return;
        case 2: *(int16_t*)ptr = (int16_t)value; return;
        case 4: *(int32_t*)ptr = (int32_t)value; return;
        case 8: *(int64_t*)ptr = (int64_t)value; return;
        }
        return;
    case EcsIrNumUnsigned:
        switch(size) {
        case 1: *(uint8_t*)ptr = (uint8_t)value; return;
        case 2: *(uint16_t*)ptr = (uint16_t)value; return;
        case 4: *(uint32_t*)ptr = (uint32_t)value; return;
        case 8: *(uint64_t*)ptr = value; return;
        }
        return;
    case EcsIrNumFloat:
        switch(size) {
        case 4: *(float*)ptr = (float)value; return;
        case 8: *(double*)ptr = (double)value; return;
        }
        return;
    case EcsIrNumNone:
        return;
    }
}

static void flecs_ir_num_store_f(
    ecs_script_ir_num_class_t class,
    int32_t size,
    void *ptr,
    double value)
{
    switch(class) {
    case EcsIrNumSigned:
        switch(size) {
        case 1: *(int8_t*)ptr = (int8_t)value; return;
        case 2: *(int16_t*)ptr = (int16_t)value; return;
        case 4: *(int32_t*)ptr = (int32_t)value; return;
        case 8: *(int64_t*)ptr = (int64_t)value; return;
        }
        return;
    case EcsIrNumUnsigned:
        switch(size) {
        case 1: *(uint8_t*)ptr = (uint8_t)value; return;
        case 2: *(uint16_t*)ptr = (uint16_t)value; return;
        case 4: *(uint32_t*)ptr = (uint32_t)value; return;
        case 8: *(uint64_t*)ptr = (uint64_t)value; return;
        }
        return;
    case EcsIrNumFloat:
        switch(size) {
        case 4: *(float*)ptr = (float)value; return;
        case 8: *(double*)ptr = value; return;
        }
        return;
    case EcsIrNumNone:
        return;
    }
}

#define FLECS_IR_NUM_LCLASS(packed) ((ecs_script_ir_num_class_t)((packed) & 0xff))
#define FLECS_IR_NUM_LSIZE(packed)  ((int32_t)(((packed) >> 8) & 0xff))
#define FLECS_IR_NUM_RCLASS(packed) ((ecs_script_ir_num_class_t)(((packed) >> 16) & 0xff))
#define FLECS_IR_NUM_RSIZE(packed)  ((int32_t)(((packed) >> 24) & 0xff))

static bool flecs_ir_binary_fast(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    const ecs_value_t *left,
    const ecs_value_t *right,
    void *out,
    ecs_token_kind_t operator)
{
    uint64_t packed = op->imm.u64;
    ecs_script_ir_num_class_t lclass = FLECS_IR_NUM_LCLASS(packed);
    int32_t lsize = FLECS_IR_NUM_LSIZE(packed);
    ecs_script_ir_num_class_t rclass = FLECS_IR_NUM_RCLASS(packed);
    int32_t rsize = FLECS_IR_NUM_RSIZE(packed);
    if (lclass == EcsIrNumNone || rclass == EcsIrNumNone) {
        return false;
    }

    int64_t li = 0, ri = 0;
    uint64_t lu = 0, ru = 0;
    double lf = 0, rf = 0;
    if (!flecs_ir_num_load(lclass, lsize, left->ptr, &li, &lu, &lf) ||
        !flecs_ir_num_load(lclass, lsize, right->ptr, &ri, &ru, &rf))
    {
        return false;
    }

    if (operator == EcsTokDiv || operator == EcsTokMod) {
        bool zero = false;
        if (lclass == EcsIrNumSigned) {
            zero = ri == 0;
        } else if (lclass == EcsIrNumUnsigned) {
            zero = ru == 0;
        } else {
            zero = ECS_EQZERO(rf);
        }
        if (zero) {
            flecs_ir_expr_error(vm, op->node, "division by zero");
            vm->cond = true;
            return true;
        }
    }

    bool cmp = false;
    switch(operator) {
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
        if (lclass == EcsIrNumSigned) {
            int64_t r = 0;
            switch(operator) {
            case EcsTokAdd: r = li + ri; break;
            case EcsTokSub: r = li - ri; break;
            case EcsTokMul: r = li * ri; break;
            default: r = li / ri; break;
            }
            flecs_ir_num_store_i(rclass, rsize, out, r);
        } else if (lclass == EcsIrNumUnsigned) {
            uint64_t r = 0;
            switch(operator) {
            case EcsTokAdd: r = lu + ru; break;
            case EcsTokSub: r = lu - ru; break;
            case EcsTokMul: r = lu * ru; break;
            default: r = lu / ru; break;
            }
            flecs_ir_num_store_u(rclass, rsize, out, r);
        } else {
            double r = 0;
            switch(operator) {
            case EcsTokAdd: r = lf + rf; break;
            case EcsTokSub: r = lf - rf; break;
            case EcsTokMul: r = lf * rf; break;
            default: r = lf / rf; break;
            }
            if (lsize == 4 && rsize == 4 && rclass == EcsIrNumFloat) {
                float lf32 = (float)lf, rf32 = (float)rf, r32 = 0;
                switch(operator) {
                case EcsTokAdd: r32 = lf32 + rf32; break;
                case EcsTokSub: r32 = lf32 - rf32; break;
                case EcsTokMul: r32 = lf32 * rf32; break;
                default: r32 = lf32 / rf32; break;
                }
                *(float*)out = r32;
            } else {
                flecs_ir_num_store_f(rclass, rsize, out, r);
            }
        }
        return true;
    case EcsTokMod:
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
        if (lclass == EcsIrNumSigned) {
            int64_t r = 0;
            switch(operator) {
            case EcsTokMod: r = li % ri; break;
            case EcsTokBitwiseAnd: r = li & ri; break;
            case EcsTokBitwiseOr: r = li | ri; break;
            case EcsTokShiftLeft: r = li << ri; break;
            default: r = li >> ri; break;
            }
            flecs_ir_num_store_i(rclass, rsize, out, r);
        } else if (lclass == EcsIrNumUnsigned) {
            uint64_t r = 0;
            switch(operator) {
            case EcsTokMod: r = lu % ru; break;
            case EcsTokBitwiseAnd: r = lu & ru; break;
            case EcsTokBitwiseOr: r = lu | ru; break;
            case EcsTokShiftLeft: r = lu << ru; break;
            default: r = lu >> ru; break;
            }
            flecs_ir_num_store_u(rclass, rsize, out, r);
        } else {
            return false;
        }
        return true;
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        if (lclass == EcsIrNumSigned) {
            switch(operator) {
            case EcsTokEq: cmp = li == ri; break;
            case EcsTokNeq: cmp = li != ri; break;
            case EcsTokGt: cmp = li > ri; break;
            case EcsTokGtEq: cmp = li >= ri; break;
            case EcsTokLt: cmp = li < ri; break;
            default: cmp = li <= ri; break;
            }
        } else if (lclass == EcsIrNumUnsigned) {
            switch(operator) {
            case EcsTokEq: cmp = lu == ru; break;
            case EcsTokNeq: cmp = lu != ru; break;
            case EcsTokGt: cmp = lu > ru; break;
            case EcsTokGtEq: cmp = lu >= ru; break;
            case EcsTokLt: cmp = lu < ru; break;
            default: cmp = lu <= ru; break;
            }
        } else {
            if (lsize == 4) {
                float a = (float)lf, b = (float)rf;
                switch(operator) {
                case EcsTokEq: cmp = a == b; break;
                case EcsTokNeq: cmp = a != b; break;
                case EcsTokGt: cmp = a > b; break;
                case EcsTokGtEq: cmp = a >= b; break;
                case EcsTokLt: cmp = a < b; break;
                default: cmp = a <= b; break;
                }
            } else {
                switch(operator) {
                case EcsTokEq: cmp = lf == rf; break;
                case EcsTokNeq: cmp = lf != rf; break;
                case EcsTokGt: cmp = lf > rf; break;
                case EcsTokGtEq: cmp = lf >= rf; break;
                case EcsTokLt: cmp = lf < rf; break;
                default: cmp = lf <= rf; break;
                }
            }
        }
        *(bool*)out = cmp;
        return true;
    default:
        return false;
    }
}

static int flecs_ir_binary(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_binary_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->b);
    ecs_script_ir_reg_t *right = flecs_ir_reg(vm, op->c);
    ecs_token_kind_t operator = (ecs_token_kind_t)(op->flags & 0x7fff);
    void *out;
    if (op->flags & 0x8000) {
        out = dst->value.ptr;
        dst->value.type = node->node.type;
        dst->ti = node->node.type_info;
    } else {
        out = flecs_ir_reg_alloc_raw(vm, dst, node->node.type_info);
    }

    const ecs_script_t *script = &vm->v.base.script->pub;
    if (!node->vector_count) {
        if (node->node.type != ecs_id(ecs_bool_t) ||
            left->value.type != ecs_id(ecs_bool_t))
        {
            vm->cond = false;
            if (flecs_ir_binary_fast(
                vm, op, &left->value, &right->value, out, operator))
            {
                return vm->cond ? -1 : 0;
            }
        }
        ecs_value_t out_value = { .type = node->node.type, .ptr = out };
        return flecs_value_binary(script, &node->node, &left->value,
            &right->value, &out_value, operator);
    }

    ecs_entity_t vector_type = node->vector_type;
    ecs_value_t left_value = { .ptr = left->value.ptr, .type = vector_type };
    ecs_value_t right_value = { .ptr = right->value.ptr, .type = vector_type };
    ecs_value_t out_value = { .ptr = out, .type = vector_type };
    ecs_size_t size = flecs_expr_storage_size(vector_type);
    ecs_size_t right_offset = 0;
    if (left->value.type == right->value.type) {
        right_offset = size;
    }
    int32_t i;
    for (i = 0; i < node->vector_count; i ++) {
        if (flecs_value_binary(script, &node->node, &left_value,
            &right_value, &out_value, operator))
        {
            return -1;
        }
        left_value.ptr = ECS_OFFSET(left_value.ptr, size);
        right_value.ptr = ECS_OFFSET(right_value.ptr, right_offset);
        out_value.ptr = ECS_OFFSET(out_value.ptr, size);
    }
    return 0;
}

#define FLECS_IR_BINARY_TYPED(NAME, T, CMP_ONLY_FLOAT)\
static int NAME(\
    ecs_script_ir_vm_t *vm,\
    const ecs_script_ir_op_t *op)\
{\
    const ecs_expr_binary_t *node = op->node;\
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);\
    T l = *(T*)flecs_ir_reg(vm, op->b)->value.ptr;\
    T r = *(T*)flecs_ir_reg(vm, op->c)->value.ptr;\
    ecs_token_kind_t operator = (ecs_token_kind_t)(op->flags & 0x7fff);\
    void *out;\
    if (op->flags & 0x8000) {\
        out = dst->value.ptr;\
        dst->value.type = node->node.type;\
        dst->ti = node->node.type_info;\
    } else {\
        int32_t top = (vm->scratch_top + 7) & ~7;\
        if (top + 8 <= vm->scratch_size) {\
            out = vm->scratch + top;\
            vm->scratch_top = top + 8;\
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
    case EcsTokAdd: *(T*)out = l + r; return 0;\
    case EcsTokSub: *(T*)out = l - r; return 0;\
    case EcsTokMul: *(T*)out = l * r; return 0;\
    case EcsTokDiv:\
        if (CMP_ONLY_FLOAT ? ECS_EQZERO(r) : (r == 0)) {\
            flecs_ir_expr_error(vm, node, "division by zero");\
            return -1;\
        }\
        *(T*)out = l / r; return 0;\
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

FLECS_IR_BINARY_TYPED(flecs_ir_binary_i64, int64_t, false)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_i32, int32_t, false)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_f64, double, true)
FLECS_IR_BINARY_TYPED(flecs_ir_binary_f32, float, true)

static int flecs_ir_cast_number(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_cast_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    ecs_script_ir_reg_t *src = flecs_ir_reg(vm, op->b);
    void *out = flecs_ir_reg_out_raw(vm, op, dst);

    uint64_t packed = op->imm.u64;
    const void *in = src->value.ptr;
#define FLECS_IR_PACK(lc, ls, rc, rs)\
    ((uint64_t)(lc) | ((uint64_t)(ls) << 8) | ((uint64_t)(rc) << 16) |\
        ((uint64_t)(rs) << 24))
    switch(packed) {
    case FLECS_IR_PACK(EcsIrNumSigned, 4, EcsIrNumSigned, 8):
        *(int64_t*)out = *(const int32_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumSigned, 8, EcsIrNumSigned, 4):
        *(int32_t*)out = (int32_t)*(const int64_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumSigned, 8, EcsIrNumFloat, 4):
        *(float*)out = (float)*(const int64_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumSigned, 8, EcsIrNumFloat, 8):
        *(double*)out = (double)*(const int64_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumSigned, 4, EcsIrNumFloat, 4):
        *(float*)out = (float)*(const int32_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumSigned, 4, EcsIrNumFloat, 8):
        *(double*)out = (double)*(const int32_t*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumFloat, 4, EcsIrNumFloat, 8):
        *(double*)out = (double)*(const float*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumFloat, 8, EcsIrNumFloat, 4):
        *(float*)out = (float)*(const double*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumFloat, 8, EcsIrNumSigned, 8):
        *(int64_t*)out = (int64_t)*(const double*)in; return 0;
    case FLECS_IR_PACK(EcsIrNumFloat, 4, EcsIrNumSigned, 8):
        *(int64_t*)out = (int64_t)*(const float*)in; return 0;
    default:
        break;
    }
    ecs_script_ir_num_class_t lclass = FLECS_IR_NUM_LCLASS(packed);
    int32_t lsize = FLECS_IR_NUM_LSIZE(packed);
    ecs_script_ir_num_class_t rclass = FLECS_IR_NUM_RCLASS(packed);
    int32_t rsize = FLECS_IR_NUM_RSIZE(packed);

    int64_t i = 0;
    uint64_t u = 0;
    double f = 0;
    if (!flecs_ir_num_load(lclass, lsize, in, &i, &u, &f)) {
        flecs_ir_expr_error(vm, node, "failed to cast value");
        return -1;
    }

    switch(lclass) {
    case EcsIrNumSigned:
        flecs_ir_num_store_i(rclass, rsize, out, i);
        break;
    case EcsIrNumUnsigned:
        flecs_ir_num_store_u(rclass, rsize, out, u);
        break;
    case EcsIrNumFloat:
        flecs_ir_num_store_f(rclass, rsize, out, f);
        break;
    case EcsIrNumNone:
        break;
    }
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

static int flecs_ir_call(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    ecs_expr_function_t *node = ECS_CONST_CAST(
        ecs_expr_function_t*, (const void*)op->node);
    const ecs_function_calldata_t *calldata = &node->calldata;
    bool method = (op->flags & EcsIrCallMethod) != 0;

    ecs_function_ctx_t call_ctx = {
        .world = vm->v.world,
        .function = calldata->function,
        .ctx = calldata->ctx
    };

#ifdef FLECS_SCRIPT_ASYNC
    if (calldata->async_callback) {
        flecs_ir_expr_error(vm, node,
            method ? "async method '%s' requires await"
                   : "async function '%s' requires await",
            node->function_name);
        return -1;
    }
#endif

    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    void *out = flecs_ir_reg_out(vm, op, dst);
    ecs_value_t out_value = { .type = node->node.type, .ptr = out };

    int32_t i, count = op->c;
    ecs_value_t *argv = NULL;
    if (count) {
        argv = ecs_os_alloca_n(ecs_value_t, count);
        for (i = 0; i < count; i ++) {
            argv[i] = flecs_ir_reg(vm, op->b + i)->value;
        }
    }

    int32_t argc = method ? count - 1 : count;
    int32_t elem_count = calldata->vector_elem_count;
    if (elem_count) {
        calldata->is.vector_callback(&call_ctx, argc, argv, &out_value,
            elem_count);
    } else {
        calldata->is.callback(&call_ctx, argc, argv, &out_value);
    }

    ecs_script_runtime_t *r = flecs_script_runtime_get(vm->v.world);
    if (r->error) {
        r->error = false;
        flecs_ir_expr_error(vm, node,
            method ? "error in script method '%s'"
                   : "error in script function '%s'",
            node->function_name);
        return -1;
    }

    return 0;
}

static inline int flecs_ir_init_check(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op,
    ecs_script_ir_reg_t *dst,
    const ecs_type_info_t *ti)
{
    if (dst->ti && (op->b + ti->size) <= dst->ti->size) {
        return 0;
    }
    uintptr_t offset = flecs_ito(uintptr_t, op->b);
    uintptr_t type_size = flecs_uto(uintptr_t, ti->size);
    uintptr_t dst_size = dst->ti
        ? flecs_uto(uintptr_t, dst->ti->size)
        : UINTPTR_MAX;
    if (offset > dst_size || type_size > (dst_size - offset)) {
        flecs_ir_expr_error(vm, op->imm.ptr ? op->imm.ptr : op->node,
            "initializer of type '%s' writes past end of target value",
            flecs_errstr(ecs_get_path(vm->v.world, ti->component)));
        return -1;
    }
    return 0;
}

static int flecs_ir_init_const(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_node_t *node = op->node;
    ecs_script_ir_reg_t *dst = flecs_ir_reg(vm, op->a);
    const ecs_type_info_t *ti = node->type_info;
    if (flecs_ir_init_check(vm, op, dst, ti)) {
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
    if (flecs_ir_init_check(vm, op, dst, ti)) {
        return -1;
    }
    void *ptr = ECS_OFFSET(dst->value.ptr, op->b);
    if (src->owned) {
        if (!ti->hooks.move) {
            ecs_os_memcpy(ptr, src->value.ptr, ti->size);
        } else {
            flecs_type_info_move(ptr, src->value.ptr, 1, ti);
        }
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
    if (flecs_ir_init_check(vm, op, dst, ti)) {
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
        if ((offset + flecs_uto(uintptr_t, size)) > dst_size) {
            flecs_ir_expr_error(vm, op->node,
                "initializer of type '%s' writes past end of target value",
                flecs_errstr(ecs_get_path(vm->v.world, swizzle->node.type)));
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

static int flecs_ir_dyn_swizzle(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_expr_swizzle_t *swizzle = op->imm.ptr;
    ecs_script_ir_reg_t *left = flecs_ir_reg(vm, op->a);
    ecs_meta_cursor_t *cur = flecs_ir_cursor(vm);
    int32_t s;
    for (s = 0; s < swizzle->count; s ++) {
        if (s) {
            if (ecs_meta_next(cur)) {
                return -1;
            }
        }
        ecs_value_t value = {
            .ptr = ECS_OFFSET(left->value.ptr, swizzle->src[s]),
            .type = swizzle->node.type
        };
        if (ecs_meta_set_value(cur, &value)) {
            return -1;
        }
    }
    return 0;
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
    int32_t from_value = *(int32_t*)flecs_ir_reg(vm, op->b)->value.ptr;
    int32_t to_value = *(int32_t*)flecs_ir_reg(vm, op->c)->value.ptr;

    ecs_meta_cursor_t cur = ecs_meta_cursor(vm->v.world, node->node.type, out);
    if (ecs_meta_push(&cur)) {
        return -1;
    }
    int32_t i, count = to_value - from_value;
    if (count < 0) {
        count = 0;
    }
    for (i = 0; i < count; i ++) {
        if (i) {
            if (ecs_meta_next(&cur)) {
                return -1;
            }
        }
        if (ecs_meta_set_int(&cur, from_value + i)) {
            return -1;
        }
    }
    if (ecs_meta_pop(&cur)) {
        return -1;
    }
    return 0;
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
    ecs_script_t *script = op->imm.ptr;
    ecs_world_t *world = vm->v.world;
    void *out = flecs_ir_reg_out(vm, op, dst);
    ecs_entity_t e = ecs_new(world);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    s->code = ecs_os_strdup(script->code);
    s->script = script;
    flecs_script_impl(script)->refcount ++;
    ecs_modified(world, e, EcsScript);
    *(ecs_entity_t*)out = e;
    dst->value.type = ecs_id(ecs_entity_t);
    return 0;
}

#ifdef FLECS_SCRIPT_ASYNC
static int flecs_ir_await_launch(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_op_t *op)
{
    const ecs_script_node_t *stmt = op->node;
    if (op->a == -1) {
        flecs_ir_error(vm, stmt,
            "await expression must be an async function call");
        return -1;
    }

    ecs_expr_function_t *call = op->imm.ptr;
    if (!call->calldata.async_callback) {
        flecs_ir_error(vm, stmt,
            "await expression must be an async function call");
        return -1;
    }

    int32_t i, count = op->b;
    ecs_value_t *argv = NULL;
    if (count) {
        argv = ecs_os_alloca_n(ecs_value_t, count);
        for (i = 0; i < count; i ++) {
            argv[i] = flecs_ir_reg(vm, op->a + i)->value;
        }
    }

    int32_t argc = (op->flags & EcsIrAwaitMethod) ? count - 1 : count;
    vm->async.future = flecs_script_future_start(vm->v.world, vm->async.entity,
        call->node.type, &call->calldata, argc, argv);
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
    if (frame->kind != EcsIrFrameTry || !vm->async.thrown || frame->state != 1) {
        return false;
    }

    const ecs_script_ir_op_t *op = &vm->ops[frame->pc];
    int32_t i = flecs_script_find_catch(&vm->v, &vm->async, op->node);
    if (i == -1) {
        return false;
    }

    flecs_script_throw_clear(&vm->async);
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
        if (flecs_ir_frame_at(vm, i)->kind == EcsIrFrameFor) {
            break;
        }
        if (flecs_ir_frame_at(vm, i)->kind == EcsIrFrameBlock) {
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
    frame->u.block.pc = vm->pc;
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
    vm->pc = frame->u.block.pc;
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
                ecs_value_t *result = v->type_visitor;
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
            const int32_t *index = ecs_vec_first(&vm->ir->scope_stmts);
            index += op->a + 1;
            for (;;) {
                int32_t next = *index ++;
                if (next < 0) {
                    vm->pc = -next - 1;
                    break;
                }
                const ecs_script_ir_op_t *next_op = &ops[next];
                if (next_op->kind == EcsIrStmtBlock) {
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
            if (v->scope_slots && op->a >= 0 &&
                op->a < ecs_vec_count(v->scope_slots))
            {
                ecs_vec_get_t(v->scope_slots, int32_t, op->a)[0] = v->visit;
            }
            if (op->b > 0) {
                frame->u.scope.var_count = v->vars->vars.count;
                frame->u.scope.vscratch_top = vm->vscratch_top;
                frame->u.scope.vheap_count = vm->vheap.count;
            }
            if (v->entity && op->c != -1 &&
                (v->force || v->entity->created))
            {
                ecs_entity_t src = v->entity->eval;
                if (src != EcsVariable) {
                    const ecs_id_t *ids = ecs_vec_get_t(
                        &vm->ir->components, ecs_id_t, op->c);
                    int32_t i, count = (int32_t)ids[0];
                    ecs_table_t *table = ecs_get_table(v->world, src);
                    bool missing = table == NULL;
                    for (i = 0; !missing && i < count; i ++) {
                        if (ecs_search(v->world, table, ids[i + 1], NULL) == -1) {
                            missing = true;
                        }
                    }
                    if (missing) {
                        flecs_ir_prof(EcsIrProfileBatchAdd);
                        flecs_add_ids(v->world, src, &ids[1], count);
                    } else {
                        flecs_ir_prof(EcsIrProfileBatchSkip);
                    }
                }
            }
            if (op->flags & EcsIrScopeEntity) {
                v->parent = v->entity->eval;
            }
            break;
        }
        case EcsIrScopeLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameScope, ECS_INTERNAL_ERROR, NULL);
            flecs_ir_scope_leave(vm, frame);
            vm->frame_count --;
            break;
        }
        case EcsIrEntityEnter: {
            flecs_script_entity_state_t state;
            res = flecs_ir_entity_enter(vm, op, &state);
            if (op->flags & 1) {
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
        case EcsIrEntityLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameEntity, ECS_INTERNAL_ERROR, NULL);
            vm->last_entity = frame->u.entity.eval;
            flecs_script_eval_entity_leave(v, &frame->u.entity);
            vm->frame_count --;
            break;
        }
        case EcsIrWithEnter: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameWith, pc);
            frame->u.with.with_count = flecs_script_with_count(v);
            frame->u.with.cursor = flecs_stack_get_cursor(&v->r->stack);
            frame->u.with.is_with_scope = v->is_with_scope;
            frame->u.with.force = v->force;
            break;
        }
        case EcsIrWithBody: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            const ecs_script_node_t *node = op->node;
            v->is_with_scope = true;
            v->force = frame->u.with.force ||
                ((node->direct_input & v->input) != 0) ||
                ((node->direct_internal & v->internal) != 0);
            break;
        }
        case EcsIrWithLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameWith, ECS_INTERNAL_ERROR, NULL);
            flecs_script_eval_with_leave(v, &frame->u.with);
            vm->frame_count --;
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
            flecs_script_pair_scope_state_t state;
            res = flecs_ir_pair_scope_enter(vm, op, &state);
            if (op->flags & 1) {
                flecs_ir_expr_end(vm);
            }
            if (!res) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                    vm, EcsIrFramePairScope, pc);
                frame->u.pair_scope = state;
            }
            break;
        }
        case EcsIrPairScopeLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFramePairScope,
                ECS_INTERNAL_ERROR, NULL);
            flecs_script_eval_pair_scope_leave(v, &frame->u.pair_scope);
            vm->frame_count --;
            break;
        }
        case EcsIrIfEnter: {
            flecs_ir_set_dirty(vm);
            ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                vm, EcsIrFrameIf, pc);
            const ecs_script_node_t *node = op->node;
            frame->u.if_.force = v->force;
            v->force = frame->u.if_.force ||
                ((node->direct_input & v->input) != 0) ||
                ((node->direct_internal & v->internal) != 0);
            if (!vm->cond) {
                vm->pc = op->b;
            }
            break;
        }
        case EcsIrIfLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameIf, ECS_INTERNAL_ERROR, NULL);
            v->force = frame->u.if_.force;
            vm->frame_count --;
            break;
        }
        case EcsIrForEnter: {
            flecs_ir_set_dirty(vm);
            flecs_script_for_state_t state;
            int32_t var_index[3];
            int32_t var_count = v->vars->vars.count;
            int32_t vscratch_top = vm->vscratch_top;
            int32_t vheap_count = vm->vheap.count;
            res = flecs_ir_for_enter(vm, op, &state, var_index);
            if (!res) {
                ecs_script_ir_frame_t *frame = flecs_ir_frame_push(
                    vm, EcsIrFrameFor, vm->pc);
                frame->u.for_.state = state;
                frame->u.for_.var_count = var_count;
                frame->u.for_.vscratch_top = vscratch_top;
                frame->u.for_.vheap_count = vheap_count;
                frame->u.for_.var_index[0] = var_index[0];
                frame->u.for_.var_index[1] = var_index[1];
                frame->u.for_.var_index[2] = var_index[2];
            } else {
                flecs_ir_vars_truncate(vm, var_count, vscratch_top, vheap_count);
            }
            break;
        }
        case EcsIrForNext: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameFor, ECS_INTERNAL_ERROR, NULL);
            flecs_script_for_state_t *fs = &frame->u.for_.state;
            if (fs->kind == FlecsScriptForRange && !fs->index_var) {
                int32_t value = fs->from + fs->index;
                if (value >= fs->count) {
                    vm->pc = op->a;
                } else {
                    ecs_script_var_t *elem = &((ecs_script_var_t*)
                        v->vars->vars.array)[frame->u.for_.var_index[0]];
                    *(int32_t*)elem->value.ptr = value;
                    fs->index ++;
                }
            } else if (!flecs_ir_for_next(vm, frame)) {
                vm->pc = op->a;
            }
            break;
        }
        case EcsIrForLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameFor, ECS_INTERNAL_ERROR, NULL);
            flecs_ir_for_leave(vm, frame);
            vm->frame_count --;
            break;
        }
        case EcsIrContinue:
            if (!flecs_ir_continue(vm)) {
                flecs_ir_error(vm, op->node,
                    "continue is only allowed inside a for loop");
                res = -1;
            }
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
        case EcsIrTryLeave: {
            ecs_script_ir_frame_t *frame = flecs_ir_frame_top(vm);
            ecs_assert(frame->kind == EcsIrFrameTry, ECS_INTERNAL_ERROR, NULL);
            vm->frame_count --;
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
            res = flecs_ir_using(vm, op->node);
            break;
        case EcsIrModule: {
            const ecs_script_module_t *node = op->node;
            ecs_assert(node->eval != 0, ECS_INTERNAL_ERROR, NULL);
            v->module = node->eval;
            v->parent = node->eval;
            break;
        }
        case EcsIrAnnot:
            res = flecs_ir_annot(vm, op);
            break;
        case EcsIrTemplate:
            ecs_vec_clear(&v->r->annot);
            res = flecs_script_template_update_vars(v,
                ECS_CONST_CAST(ecs_script_template_node_t*,
                    (const void*)op->node));
            break;
        case EcsIrMutCheck:
            if (!v->template_entity) {
                flecs_ir_error(vm, op->node,
                    "mut variables are only allowed in templates");
                res = -1;
            }
            break;
        case EcsIrConstEnd:
            res = flecs_ir_const_end(vm, op);
            break;
        case EcsIrConstCached: {
            ecs_script_computed_t *slot = v->computed &&
                op->a < v->computed_count ? &v->computed[op->a] : NULL;
            if (!slot || !slot->valid) {
                vm->pc = op->b;
                break;
            }
            const ecs_script_var_node_t *node = op->node;
            ecs_script_var_t *var = flecs_ir_var_declare(vm);
            var->is_const = true;
            var->type_info = slot->ti;
            var->value.type = node->eval_type;
            var->value.ptr = slot->ptr;
            var->owned = false;
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
        case EcsIrBinaryI64:
            res = flecs_ir_binary_i64(vm, op);
            break;
        case EcsIrBinaryI32:
            res = flecs_ir_binary_i32(vm, op);
            break;
        case EcsIrBinaryF64:
            res = flecs_ir_binary_f64(vm, op);
            break;
        case EcsIrBinaryF32:
            res = flecs_ir_binary_f32(vm, op);
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
            res = flecs_ir_dyn_swizzle(vm, op);
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
            ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
            ecs_strbuf_t *buf = ecs_vec_last_t(&vm->strbufs, ecs_strbuf_t);
            const void *ptr = reg->value.ptr;
            switch(op->flags) {
            case 1: {
                int64_t value = 0;
                switch(op->b) {
                case 1: value = *(const int8_t*)ptr; break;
                case 2: value = *(const int16_t*)ptr; break;
                case 4: value = *(const int32_t*)ptr; break;
                default: value = *(const int64_t*)ptr; break;
                }
                ecs_strbuf_appendint(buf, value);
                break;
            }
            case 2: {
                uint64_t value = 0;
                switch(op->b) {
                case 1: value = *(const uint8_t*)ptr; break;
                case 2: value = *(const uint16_t*)ptr; break;
                case 4: value = *(const uint32_t*)ptr; break;
                default: value = *(const uint64_t*)ptr; break;
                }
                ecs_strbuf_append(buf, "%" PRIu64, value);
                break;
            }
            case 3: {
                double value = op->b == 4
                    ? (double)*(const float*)ptr
                    : *(const double*)ptr;
                ecs_strbuf_append(buf, "%f", value);
                break;
            }
            default:
                ecs_strbuf_append(buf, "%c", *(const char*)ptr);
                break;
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
        case EcsIrToBool: {
            ecs_script_ir_reg_t *reg = flecs_ir_reg(vm, op->a);
            if (reg->value.type == ecs_id(ecs_bool_t)) {
                vm->cond = *(bool*)reg->value.ptr;
            } else {
                res = flecs_ir_to_bool(vm, op);
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
    vm->scratch_top = 0;
    vm->vscratch_top = 0;
    vm->frame_count = 0;
    vm->last_entity = 0;
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
    vm->scratch = NULL;
    vm->scratch_size = 0;
    ecs_vec_init_t(NULL, &vm->owned, flecs_ir_owned_t, 0);
    ecs_vec_init_t(NULL, &vm->heap, void*, 0);
    vm->vscratch = NULL;
    vm->vscratch_size = 0;
    ecs_vec_init_t(NULL, &vm->vheap, void*, 0);
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
    count = vm->heap.count;
    void **heap = vm->heap.array;
    for (i = 0; i < count; i ++) {
        ecs_os_free(heap[i]);
    }
    vm->heap.count = 0;
    vm->owned.count = 0;
    count = vm->vheap.count;
    heap = vm->vheap.array;
    for (i = 0; i < count; i ++) {
        ecs_os_free(heap[i]);
    }
    vm->vheap.count = 0;
#ifdef FLECS_SCRIPT_ASYNC
    if (vm->async.future) {
        ecs_script_future_release(vm->async.future);
        vm->async.future = NULL;
    }
    flecs_script_throw_clear(&vm->async);
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
    ecs_vec_fini_t(NULL, &vm->heap, void*);
    ecs_vec_fini_t(NULL, &vm->owned, flecs_ir_owned_t);
    ecs_os_free(vm->scratch);
    vm->scratch = NULL;
    ecs_vec_fini_t(NULL, &vm->vheap, void*);
    ecs_os_free(vm->vscratch);
    vm->vscratch = NULL;
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

static void flecs_ir_visit_init(
    ecs_script_impl_t *script,
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc)
{
    v->base.script = script;
    v->base.visit = NULL;
    v->base.prev = NULL;
    v->base.next = NULL;
    v->base.depth = 0;
    v->world = script->pub.world;
    v->r = desc ? desc->runtime : NULL;
    v->template = NULL;
    v->instance_template = NULL;
    v->template_entity = 0;
    v->body_template = 0;
    v->script_entity = 0;
    v->script_tag = 0;
    v->module = 0;
    v->parent = 0;
    v->entity = NULL;
    v->with_relationship = 0;
    v->with_relationship_sp = 0;
    v->is_with_scope = false;
    v->vars = NULL;
    v->type_visitor = NULL;
    v->symbol_slots = NULL;
    v->component_slots = &script->component_slots;
    v->scope_slots = &script->scope_slots;
    v->for_slots = &script->for_slots;
    v->input = UINT64_MAX;
    v->symbol_offset = 0;
    v->visit = 0;
    v->scope_slot = -1;
    v->for_slot = -1;
    v->force = true;

    if (!v->r) {
        v->r = ecs_script_runtime_new();
    }

    ecs_id_t tag = flecs_script_runtime_get(v->world)->current_tag;
    if (tag) {
        v->script_tag = tag;
        if (ECS_HAS_ID_FLAG(tag, PAIR)) {
            if (ECS_PAIR_FIRST(tag) == ecs_id(EcsScript)) {
                v->script_entity = ecs_pair_second(v->world, tag);
            }
        }
    }
}

ecs_script_ir_vm_t* flecs_script_ir_vm_new(
    ecs_script_impl_t *script,
    const ecs_script_eval_desc_t *desc)
{
    ecs_script_ir_vm_t *vm = flecs_ir_vm_acquire(
        desc ? desc->runtime : NULL, flecs_script_ir_ensure(script));
    flecs_ir_visit_init(script, &vm->v, desc);
    return vm;
}

void flecs_script_ir_vm_free(
    ecs_script_ir_vm_t *vm,
    const ecs_script_eval_desc_t *desc)
{
    ecs_script_runtime_t *r = desc ? desc->runtime : NULL;
    flecs_script_eval_visit_fini(&vm->v, desc);
    flecs_ir_vm_release(r, vm);
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
#ifdef FLECS_SCRIPT_ASYNC
    if (vm->async.future) {
        ecs_script_future_release(vm->async.future);
        vm->async.future = NULL;
    }
    flecs_script_throw_clear(&vm->async);
#endif
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
        frame->u.block.pc = -1;
        frame->u.block.reg_base = 0;
        frame->u.block.entry = 0;
        frame->u.block.vars = true;
        vm->v.vars = flecs_script_vars_push(
            vm->v.vars, &vm->v.r->stack, &vm->v.r->allocator);
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

void flecs_script_ir_cleanup(
    ecs_script_eval_visitor_t *v,
    const ecs_script_ir_t *ir,
    const ecs_script_ir_entry_t *entry)
{
    flecs_script_ir_cleanup_w_vm(v, ir, entry, true);
}

void flecs_script_ir_cleanup_w_vm(
    ecs_script_eval_visitor_t *v,
    const ecs_script_ir_t *ir,
    const ecs_script_ir_entry_t *entry,
    bool dirty)
{
    if (dirty || v->input == UINT64_MAX) {
        flecs_script_cleanup_slots(v);
    }

    if (!v->for_slots) {
        return;
    }

    int32_t slot_count = ecs_vec_count(v->for_slots);
    const ecs_script_ir_for_t *fors = ecs_vec_first(&ir->fors);
    int32_t i, end = entry->for_first + entry->for_count;
    for (i = entry->for_first; i < end; i ++) {
        const ecs_script_ir_for_t *f = &fors[i];
        if (f->for_slot < 0 || f->for_slot >= slot_count) {
            continue;
        }
        ecs_script_for_slot_t *slot = ecs_vec_get_t(
            v->for_slots, ecs_script_for_slot_t, f->for_slot);
        if (!flecs_script_scope_visited(v, f->scope_slot)) {
            flecs_script_for_slot_clear(v->world, slot, true);
        } else {
            flecs_script_for_slot_purge(v->world, slot, v->visit);
        }
    }
}

int flecs_script_ir_eval_root(
    ecs_script_eval_visitor_t *v,
    ecs_script_impl_t *script)
{
    ecs_script_ir_t *ir = flecs_script_ir_ensure(script);
    if (!ir) {
        return -1;
    }

    ecs_script_ir_vm_t *vm = flecs_ir_vm_acquire(v->r, ir);
    vm->v = *v;

    const ecs_script_ir_entry_t *entry = ecs_vec_get_t(
        &ir->entries, ecs_script_ir_entry_t, ir->root_entry);
    flecs_script_run_status_t status = flecs_script_ir_vm_run(vm, entry);
    int result = -1;
    if (status == FlecsScriptRunDone) {
        flecs_script_ir_cleanup_w_vm(&vm->v, ir, entry, vm->dirty);
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
    ecs_world_t *real_world = world;
    flecs_stage_from_world(&real_world);
    ecs_script_function_node_t *node = uf->node;
    ecs_script_impl_t *impl = flecs_script_impl(uf->script);
    bool failed = false;

    ecs_script_ir_t *ir = flecs_script_ir_ensure(impl);
    const ecs_script_ir_entry_t *entry = ir
        ? flecs_script_ir_entry(ir, node)
        : NULL;
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

    ecs_allocator_t *a = &v->r->allocator;
    int32_t using_count = ecs_vec_count(&uf->using);
    ecs_entity_t *using = ecs_vec_first(&uf->using);
    for (int32_t u = 0; u < using_count; u ++) {
        ecs_vec_append_t(a, &v->r->using, ecs_entity_t)[0] = using[u];
    }

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, a);

    int32_t i, param_count = ecs_vec_count(&node->params);
    for (i = 0; i < argc && i < param_count; i ++) {
        const ecs_type_info_t *ti = ecs_get_type_info(
            real_world, argv[i].type);
        ecs_script_var_t *var = ecs_script_vars_declare(v->vars, NULL);
        var->value.type = argv[i].type;
        var->type_info = ti;
        if (ti) {
            var->value.ptr = flecs_stack_calloc(
                &v->r->stack, ti->size, ti->alignment);
            flecs_type_info_ctor(var->value.ptr, 1, ti);
            ecs_ptr_copy_w_type_info(
                real_world, ti, var->value.ptr, argv[i].ptr);
            var->owned = true;
        } else {
            var->value.ptr = argv[i].ptr;
            var->owned = false;
        }
    }

    v->type_visitor = result;
    if (flecs_script_ir_vm_run(vm, entry) != FlecsScriptRunDone) {
        failed = true;
    }
    v->type_visitor = NULL;

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
