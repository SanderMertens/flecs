/**
 * @file addons/script/ir/to_str.c
 * @brief Script IR listing.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static const struct {
    const char *name;
    const char *operands[3];
} flecs_ir_listing[EcsIrOpKindLast] = {
    [EcsIrEnd] = {"End", {NULL}},
    [EcsIrLeave] = {"Leave", {"frame"}},
    [EcsIrJump] = {"Jump", {"target"}},
    [EcsIrStmt] = {"Stmt", {NULL, "skip", "region"}},
    [EcsIrStmtBlock] = {"StmtBlock", {NULL, "skip", "count"}},
    [EcsIrMark] = {"Mark", {NULL, NULL, "region"}},
    [EcsIrAnnotClear] = {"AnnotClear", {NULL}},
    [EcsIrScopeEnter] = {"ScopeEnter", {"slot", "vars", "add_count"}},
    [EcsIrEntityEnter] = {"EntityEnter", {"name"}},
    [EcsIrWithEnter] = {"WithEnter", {NULL}},
    [EcsIrWithBody] = {"WithBody", {NULL}},
    [EcsIrWithTag] = {"WithTag", {"id"}},
    [EcsIrWithComponentBegin] = {"WithComponentBegin", {"id", "dst"}},
    [EcsIrWithComponentEnd] = {"WithComponentEnd", {"id", "value", "tmp"}},
    [EcsIrPairScopeEnter] = {"PairScopeEnter", {"id"}},
    [EcsIrIfEnter] = {"IfEnter", {NULL, "else"}},
    [EcsIrForEnter] = {"ForEnter", {"from", "to"}},
    [EcsIrForNext] = {"ForNext", {"end"}},
    [EcsIrContinue] = {"Continue", {NULL}},
    [EcsIrTryEnter] = {"TryEnter", {"catches", "count", "end"}},
    [EcsIrAwaitStart] = {"AwaitStart", {NULL, "resume"}},
    [EcsIrAwaitLaunch] = {"AwaitLaunch", {"args", "count"}},
    [EcsIrAwaitPoll] = {"AwaitPoll", {NULL}},
    [EcsIrTag] = {"Tag", {"id"}},
    [EcsIrComponentBegin] = {"ComponentBegin", {"id", "dst"}},
    [EcsIrComponentEnd] = {"ComponentEnd", {"id", "value", "tmp"}},
    [EcsIrComponentVisitor] = {"ComponentVisitor", {"id", "done"}},
    [EcsIrUsing] = {"Using", {NULL}},
    [EcsIrModule] = {"Module", {NULL}},
    [EcsIrAnnot] = {"Annot", {NULL}},
    [EcsIrTemplate] = {"Template", {NULL}},
    [EcsIrMutCheck] = {"MutCheck", {NULL}},
    [EcsIrConstEnd] = {"ConstEnd", {"value"}},
    [EcsIrConstCached] = {"ConstCached", {"slot", "else"}},
    [EcsIrExprBegin] = {"ExprBegin", {"first", "count"}},
    [EcsIrExprEnd] = {"ExprEnd", {NULL}},
    [EcsIrLoadConst] = {"LoadConst", {"dst"}},
    [EcsIrLoadVar] = {"LoadVar", {"dst", "slot"}},
    [EcsIrLoadGlobal] = {"LoadGlobal", {"dst"}},
    [EcsIrLoadSymbol] = {"LoadSymbol", {"dst", "symbol"}},
    [EcsIrLookup] = {"Lookup", {"dst"}},
    [EcsIrMember] = {"Member", {"dst", "src", "offset"}},
    [EcsIrSwizzle] = {"Swizzle", {"dst", "src"}},
    [EcsIrElement] = {"Element", {"dst", "src", "index"}},
    [EcsIrComponentGet] = {"ComponentGet", {"dst", "entity"}},
    [EcsIrHas] = {"Has", {"dst", "entity"}},
    [EcsIrUnary] = {"Unary", {"dst", "src"}},
    [EcsIrBinary] = {"Binary", {"dst", "left", "right"}},
    [EcsIrBinaryI8] = {"BinaryI8", {"dst", "left", "right"}},
    [EcsIrBinaryI16] = {"BinaryI16", {"dst", "left", "right"}},
    [EcsIrBinaryU8] = {"BinaryU8", {"dst", "left", "right"}},
    [EcsIrBinaryU16] = {"BinaryU16", {"dst", "left", "right"}},
    [EcsIrBinaryU32] = {"BinaryU32", {"dst", "left", "right"}},
    [EcsIrBinaryU64] = {"BinaryU64", {"dst", "left", "right"}},
    [EcsIrBinaryI64] = {"BinaryI64", {"dst", "left", "right"}},
    [EcsIrBinaryI32] = {"BinaryI32", {"dst", "left", "right"}},
    [EcsIrBinaryF64] = {"BinaryF64", {"dst", "left", "right"}},
    [EcsIrBinaryF32] = {"BinaryF32", {"dst", "left", "right"}},
    [EcsIrJumpIfFalse] = {"JumpIfFalse", {"condition", "target", "result"}},
    [EcsIrJumpIfTrue] = {"JumpIfTrue", {"condition", "target", "result"}},
    [EcsIrCast] = {"Cast", {"dst", "src"}},
    [EcsIrCastNumber] = {"CastNumber", {"dst", "src"}},
    [EcsIrCall] = {"Call", {"dst", "args", "count"}},
    [EcsIrInitAlloc] = {"InitAlloc", {"dst"}},
    [EcsIrInitConst] = {"InitConst", {"dst", "offset"}},
    [EcsIrInitAssign] = {"InitAssign", {"dst", "offset", "src"}},
    [EcsIrInitOp] = {"InitOp", {"dst", "offset", "src"}},
    [EcsIrInitSwizzle] = {"InitSwizzle", {"dst", "offset", "src"}},
    [EcsIrDynBegin] = {"DynBegin", {"dst"}},
    [EcsIrDynPush] = {"DynPush", {"dst"}},
    [EcsIrDynPop] = {"DynPop", {"dst"}},
    [EcsIrDynNext] = {"DynNext", {"dst"}},
    [EcsIrDynMember] = {"DynMember", {NULL}},
    [EcsIrDynKey] = {"DynKey", {"key"}},
    [EcsIrDynSet] = {"DynSet", {"value"}},
    [EcsIrDynSwizzle] = {"DynSwizzle", {"value"}},
    [EcsIrDynEnd] = {"DynEnd", {"dst"}},
    [EcsIrStrBegin] = {"StrBegin", {NULL}},
    [EcsIrStrFrag] = {"StrFrag", {NULL}},
    [EcsIrStrValue] = {"StrValue", {"value"}},
    [EcsIrStrCast] = {"StrCast", {"value", "size"}},
    [EcsIrStrFormat] = {"StrFormat", {"value", "width", "precision"}},
    [EcsIrStrEnd] = {"StrEnd", {"dst"}},
    [EcsIrMatchCmp] = {"MatchCmp", {"value", "compare", "else"}},
    [EcsIrMatchFail] = {"MatchFail", {"value"}},
    [EcsIrRange] = {"Range", {"dst", "from", "to"}},
    [EcsIrNew] = {"New", {"dst", "block"}},
    [EcsIrScript] = {"Script", {"dst"}},
    [EcsIrToBool] = {"ToBool", {"value"}},
    [EcsIrReturn] = {"Return", {"value"}},
};

const char* flecs_script_ir_op_name(
    ecs_script_ir_op_kind_t kind)
{
    if (kind < 0 || kind >= EcsIrOpKindLast) {
        return "?";
    }
    return flecs_ir_listing[kind].name;
}

static void flecs_irs_id_to_buf(
    const ecs_script_impl_t *script,
    const ecs_script_ir_t *ir,
    int32_t index,
    ecs_strbuf_t *buf)
{
    const ecs_script_ir_id_t *id = ecs_vec_get_t(
        &ir->ids, ecs_script_ir_id_t, index);
    if (!id->dynamic && id->eval) {
        char *str = ecs_id_str(script->pub.world, id->eval);
        ecs_strbuf_appendstr(buf, str);
        ecs_os_free(str);
        return;
    }
    if (id->has_second) {
        ecs_strbuf_append(buf, "(%s, %s)", id->first, id->second);
    } else {
        ecs_strbuf_appendstr(buf, id->first);
    }
    if (id->first_symbol != -1) {
        ecs_strbuf_append(buf, " sym=%d", id->first_symbol);
    }
    if (id->first_sp != -1) {
        ecs_strbuf_append(buf, " sp=%d", id->first_sp);
    }
    if (id->first_reg != -1) {
        ecs_strbuf_append(buf, " r%d", id->first_reg);
    }
    if (id->second_symbol != -1) {
        ecs_strbuf_append(buf, " sym2=%d", id->second_symbol);
    }
    if (id->second_sp != -1) {
        ecs_strbuf_append(buf, " sp2=%d", id->second_sp);
    }
    if (id->second_reg != -1) {
        ecs_strbuf_append(buf, " r%d", id->second_reg);
    }
}

static void flecs_irs_payload_to_buf(
    const ecs_script_impl_t *script,
    const ecs_script_ir_op_t *op,
    ecs_strbuf_t *buf)
{
    ecs_world_t *world = script->pub.world;
    const ecs_expr_node_t *expr = op->node;
    switch (op->kind) {
    case EcsIrLoadConst:
    case EcsIrInitConst: {
        char *value = ecs_ptr_to_expr(world, expr->type, op->imm.ptr);
        ecs_strbuf_append(buf, " value=%s", value);
        ecs_os_free(value);
        break;
    }
    case EcsIrLoadGlobal:
    case EcsIrComponentGet:
    case EcsIrHas: {
        char *id = ecs_id_str(world, op->imm.id);
        ecs_strbuf_append(buf, " id=%s", id);
        ecs_os_free(id);
        break;
    }
    case EcsIrLookup:
    case EcsIrDynMember:
    case EcsIrStrFrag:
        ecs_strbuf_append(buf, " text=\"%s\"", op->imm.str);
        break;
    case EcsIrStmt:
    case EcsIrStmtBlock:
        ecs_strbuf_append(buf, " input=0x%llx",
            (unsigned long long)op->imm.u64);
        break;
    case EcsIrScopeEnter: {
        const ecs_id_t *ids = op->imm.ptr;
        for (int32_t i = 0; i < op->c; i ++) {
            char *id = ecs_id_str(world, ids[i]);
            ecs_strbuf_append(buf, "%s%s", i ? ", " : " add=", id);
            ecs_os_free(id);
        }
        break;
    }
    default:
        break;
    }
    if ((op->kind >= EcsIrBinary && op->kind <= EcsIrBinaryF32) ||
        op->kind == EcsIrInitOp || op->kind == EcsIrUnary)
    {
        ecs_token_kind_t token = op->kind == EcsIrUnary
            ? ((const ecs_expr_unary_t*)op->node)->operator
            : (ecs_token_kind_t)(op->flags & 0x7fff);
        ecs_strbuf_append(buf, " operator=%s", flecs_token_str(token));
    }
    if (op->node && op->kind != EcsIrJump && op->kind != EcsIrEnd) {
        const char *pos;
        if (op->kind >= EcsIrLoadConst && op->kind < EcsIrToBool) {
            pos = expr->pos;
            if (expr->type) {
                const char *type = ecs_get_name(world, expr->type);
                if (type) {
                    ecs_strbuf_appendlit(buf, " type=");
                    ecs_strbuf_appendstr(buf, type);
                }
            }
        } else {
            pos = ((const ecs_script_node_t*)op->node)->pos;
        }
        if (pos && op->kind == EcsIrStmt) {
            int32_t length = 0;
            while (length < 80 && pos[length] && pos[length] != '\n') {
                length ++;
            }
            ecs_strbuf_append(buf, " ; %.*s", length, pos);
        }
    }
}

void flecs_script_ir_to_buf(
    const ecs_script_impl_t *script,
    const ecs_script_ir_t *ir,
    ecs_strbuf_t *buf)
{
    const ecs_script_ir_op_t *ops = ecs_vec_first(&ir->ops);
    const ecs_script_ir_entry_t *entries = ecs_vec_first(&ir->entries);
    int32_t count = ecs_vec_count(&ir->ops);
    int32_t entry_count = ecs_vec_count(&ir->entries);

    for (int32_t i = 0; i < ecs_vec_count(&ir->ids); i ++) {
        ecs_strbuf_append(buf, "id %d: ", i);
        flecs_irs_id_to_buf(script, ir, i, buf);
        ecs_strbuf_appendch(buf, '\n');
    }
    for (int32_t i = 0; i < count; i ++) {
        const ecs_script_ir_op_t *op = &ops[i];
        for (int32_t e = 0; e < entry_count; e ++) {
            if (entries[e].pc == i) {
                static const char *const kinds[] = {
                    "root", "template", "entity", "fn"
                };
                ecs_strbuf_append(buf, "%s%s block %d (regs=%d)\n",
                    i ? "\n" : "", kinds[entries[e].kind], e,
                    entries[e].reg_count);
            }
        }
        ecs_strbuf_append(buf, "%4d  %-18s", i,
            flecs_script_ir_op_name(op->kind));
        const int32_t values[] = {op->a, op->b, op->c};
        for (int32_t a = 0; a < 3; a ++) {
            const char *label = flecs_ir_listing[op->kind].operands[a];
            if (label) {
                ecs_strbuf_appendch(buf, ' ');
                ecs_strbuf_appendstr(buf, label);
                ecs_strbuf_appendch(buf, '=');
                ecs_strbuf_appendint(buf, values[a]);
            }
        }
        if (op->flags) {
            ecs_strbuf_append(buf, " flags=0x%x", op->flags);
        }
        flecs_irs_payload_to_buf(script, op, buf);
        ecs_strbuf_appendch(buf, '\n');
    }
}

#endif
