/**
 * @file addons/script/ir/to_str.c
 * @brief Script IR listing.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

const char* flecs_script_ir_op_name(
    ecs_script_ir_op_kind_t kind)
{
    switch(kind) {
    case EcsIrEnd: return "End";
    case EcsIrJump: return "Jump";
    case EcsIrStmt: return "Stmt";
    case EcsIrStmtBlock: return "StmtBlock";
    case EcsIrMark: return "Mark";
    case EcsIrAnnotClear: return "AnnotClear";
    case EcsIrScopeEnter: return "ScopeEnter";
    case EcsIrScopeLeave: return "ScopeLeave";
    case EcsIrEntityEnter: return "EntityEnter";
    case EcsIrEntityLeave: return "EntityLeave";
    case EcsIrWithEnter: return "WithEnter";
    case EcsIrWithBody: return "WithBody";
    case EcsIrWithLeave: return "WithLeave";
    case EcsIrWithTag: return "WithTag";
    case EcsIrWithComponentBegin: return "WithComponentBegin";
    case EcsIrWithComponentEnd: return "WithComponentEnd";
    case EcsIrPairScopeEnter: return "PairScopeEnter";
    case EcsIrPairScopeLeave: return "PairScopeLeave";
    case EcsIrIfEnter: return "IfEnter";
    case EcsIrIfLeave: return "IfLeave";
    case EcsIrForEnter: return "ForEnter";
    case EcsIrForNext: return "ForNext";
    case EcsIrForLeave: return "ForLeave";
    case EcsIrContinue: return "Continue";
    case EcsIrTryEnter: return "TryEnter";
    case EcsIrTryLeave: return "TryLeave";
    case EcsIrAwaitStart: return "AwaitStart";
    case EcsIrAwaitLaunch: return "AwaitLaunch";
    case EcsIrAwaitPoll: return "AwaitPoll";
    case EcsIrTag: return "Tag";
    case EcsIrComponentBegin: return "ComponentBegin";
    case EcsIrComponentEnd: return "ComponentEnd";
    case EcsIrComponentVisitor: return "ComponentVisitor";
    case EcsIrUsing: return "Using";
    case EcsIrModule: return "Module";
    case EcsIrAnnot: return "Annot";
    case EcsIrTemplate: return "Template";
    case EcsIrMutCheck: return "MutCheck";
    case EcsIrConstEnd: return "ConstEnd";
    case EcsIrConstCached: return "ConstCached";
    case EcsIrExprBegin: return "ExprBegin";
    case EcsIrExprEnd: return "ExprEnd";
    case EcsIrLoadConst: return "LoadConst";
    case EcsIrLoadVar: return "LoadVar";
    case EcsIrLoadGlobal: return "LoadGlobal";
    case EcsIrLoadSymbol: return "LoadSymbol";
    case EcsIrLookup: return "Lookup";
    case EcsIrMember: return "Member";
    case EcsIrSwizzle: return "Swizzle";
    case EcsIrElement: return "Element";
    case EcsIrComponentGet: return "ComponentGet";
    case EcsIrHas: return "Has";
    case EcsIrUnary: return "Unary";
    case EcsIrBinary: return "Binary";
    case EcsIrBinaryI64: return "BinaryI64";
    case EcsIrBinaryI32: return "BinaryI32";
    case EcsIrBinaryF64: return "BinaryF64";
    case EcsIrBinaryF32: return "BinaryF32";
    case EcsIrJumpIfFalse: return "JumpIfFalse";
    case EcsIrJumpIfTrue: return "JumpIfTrue";
    case EcsIrCast: return "Cast";
    case EcsIrCastNumber: return "CastNumber";
    case EcsIrCall: return "Call";
    case EcsIrInitAlloc: return "InitAlloc";
    case EcsIrInitConst: return "InitConst";
    case EcsIrInitAssign: return "InitAssign";
    case EcsIrInitOp: return "InitOp";
    case EcsIrInitSwizzle: return "InitSwizzle";
    case EcsIrDynBegin: return "DynBegin";
    case EcsIrDynPush: return "DynPush";
    case EcsIrDynPop: return "DynPop";
    case EcsIrDynNext: return "DynNext";
    case EcsIrDynMember: return "DynMember";
    case EcsIrDynKey: return "DynKey";
    case EcsIrDynSet: return "DynSet";
    case EcsIrDynSwizzle: return "DynSwizzle";
    case EcsIrDynEnd: return "DynEnd";
    case EcsIrStrBegin: return "StrBegin";
    case EcsIrStrFrag: return "StrFrag";
    case EcsIrStrValue: return "StrValue";
    case EcsIrStrCast: return "StrCast";
    case EcsIrStrFormat: return "StrFormat";
    case EcsIrStrEnd: return "StrEnd";
    case EcsIrMatchCmp: return "MatchCmp";
    case EcsIrMatchFail: return "MatchFail";
    case EcsIrRange: return "Range";
    case EcsIrNew: return "New";
    case EcsIrScript: return "Script";
    case EcsIrToBool: return "ToBool";
    case EcsIrReturn: return "Return";
    case EcsIrOpKindLast: break;
    }
    return "?";
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

static void flecs_irs_type_to_buf(
    const ecs_script_impl_t *script,
    const ecs_expr_node_t *node,
    ecs_strbuf_t *buf)
{
    if (!node || !node->type) {
        return;
    }
    const char *name = ecs_get_name(script->pub.world, node->type);
    if (name) {
        ecs_strbuf_append(buf, " (%s)", name);
    }
}

void flecs_script_ir_to_buf(
    const ecs_script_impl_t *script,
    const ecs_script_ir_t *ir,
    ecs_strbuf_t *buf)
{
    ecs_world_t *world = script->pub.world;
    const ecs_script_ir_op_t *ops = ecs_vec_first(&ir->ops);
    const ecs_script_ir_entry_t *entries = ecs_vec_first(&ir->entries);
    int32_t i, count = ecs_vec_count(&ir->ops);
    int32_t e, entry_count = ecs_vec_count(&ir->entries);

    for (i = 0; i < count; i ++) {
        const ecs_script_ir_op_t *op = &ops[i];

        for (e = 0; e < entry_count; e ++) {
            if (entries[e].pc == i) {
                const char *kind = "root";
                if (entries[e].kind == EcsIrEntryTemplate) {
                    kind = "template";
                } else if (entries[e].kind == EcsIrEntryEntity) {
                    kind = "entity";
                } else if (entries[e].kind == EcsIrEntryFunction) {
                    kind = "fn";
                }
                ecs_strbuf_append(buf, "%s%s block %d (regs=%d, fors=%d)\n",
                    i ? "\n" : "", kind, e, entries[e].reg_count,
                    entries[e].for_count);
            }
        }

        ecs_strbuf_append(buf, "%4d  %-18s", i, flecs_script_ir_op_name(op->kind));

        switch((ecs_script_ir_op_kind_t)op->kind) {
        case EcsIrJump:
            ecs_strbuf_append(buf, "-> %d", op->a);
            break;
        case EcsIrConstCached:
            ecs_strbuf_append(buf, "slot=%d else-> %d", op->a, op->b);
            break;
        case EcsIrStmtBlock:
            ecs_strbuf_append(buf, "count=%d input=0x%llx skip-> %d",
                op->c, (unsigned long long)op->imm.u64, op->b);
            break;
        case EcsIrStmt:
            ecs_strbuf_append(buf, "input=0x%llx skip-> %d",
                (unsigned long long)op->imm.u64, op->b);
            if (op->flags & EcsIrStmtAlways) {
                ecs_strbuf_appendstr(buf, " always");
            }
            if (op->flags & EcsIrStmtSkip) {
                ecs_strbuf_appendstr(buf, " skipped");
            }
            if (op->c != -1) {
                const ecs_script_region_t *region = ecs_vec_get_t(
                    &script->regions, ecs_script_region_t, op->c);
                ecs_strbuf_append(buf, " marks=[scopes:%d fors:%d]",
                    region->scope_count, region->for_count);
            }
            break;
        case EcsIrMark: {
            const ecs_script_region_t *region = ecs_vec_get_t(
                &script->regions, ecs_script_region_t, op->c);
            ecs_strbuf_append(buf, "[scopes:%d fors:%d]",
                region->scope_count, region->for_count);
            break;
        }
        case EcsIrScopeEnter:
            ecs_strbuf_append(buf, "slot=%d", op->a);
            if (op->b >= 0) {
                ecs_strbuf_append(buf, " vars=%d", op->b);
            }
            if (op->flags & EcsIrScopeEntity) {
                ecs_strbuf_appendstr(buf, " entity");
            }
            if (op->c != -1) {
                const ecs_id_t *ids = ecs_vec_get_t(
                    &ir->components, ecs_id_t, op->c);
                ecs_strbuf_appendstr(buf, " add=[");
                int32_t c, cc = (int32_t)ids[0];
                for (c = 0; c < cc; c ++) {
                    char *str = ecs_id_str(world, ids[c + 1]);
                    ecs_strbuf_append(buf, "%s%s", c ? ", " : "", str);
                    ecs_os_free(str);
                }
                ecs_strbuf_appendstr(buf, "]");
            }
            break;
        case EcsIrEntityEnter: {
            const ecs_script_entity_t *node = op->node;
            if (node->name && !node->name_expr) {
                ecs_strbuf_append(buf, "'%s'", node->name);
            } else if (op->a != -1) {
                ecs_strbuf_append(buf, "name=r%d", op->a);
            } else {
                ecs_strbuf_appendstr(buf, "anonymous");
            }
            if (node->kind) {
                ecs_strbuf_append(buf, " kind='%s'", node->kind);
            }
            if (node->symbol != -1) {
                ecs_strbuf_append(buf, " sym=%d", node->symbol);
            }
            break;
        }
        case EcsIrWithTag:
        case EcsIrTag:
        case EcsIrPairScopeEnter:
            flecs_irs_id_to_buf(script, ir, op->a, buf);
            break;
        case EcsIrComponentVisitor:
            flecs_irs_id_to_buf(script, ir, op->a, buf);
            ecs_strbuf_append(buf, " done-> %d", op->b);
            break;
        case EcsIrComponentBegin:
        case EcsIrWithComponentBegin:
            flecs_irs_id_to_buf(script, ir, op->a, buf);
            if (op->b != -1) {
                ecs_strbuf_append(buf, " -> r%d", op->b);
            }
            if (op->flags & EcsIrComponentPartial) {
                ecs_strbuf_appendstr(buf, " partial");
            }
            break;
        case EcsIrComponentEnd:
        case EcsIrWithComponentEnd:
            flecs_irs_id_to_buf(script, ir, op->a, buf);
            if (op->b != -1) {
                ecs_strbuf_append(buf, " <- r%d", op->b);
                if (op->b != op->c) {
                    ecs_strbuf_append(buf, " (tmp r%d)", op->c);
                }
            }
            break;
        case EcsIrIfEnter:
            ecs_strbuf_append(buf, "else-> %d", op->b);
            break;
        case EcsIrForEnter:
            if (op->b != -1) {
                ecs_strbuf_append(buf, "range r%d..r%d", op->a, op->b);
            } else {
                ecs_strbuf_append(buf, "in r%d", op->a);
            }
            break;
        case EcsIrForNext:
            ecs_strbuf_append(buf, "end-> %d", op->a);
            break;
        case EcsIrTryEnter:
            ecs_strbuf_append(buf, "catches=%d end-> %d", op->b, op->c);
            break;
        case EcsIrAwaitStart:
            ecs_strbuf_append(buf, "resume-> %d", op->b);
            break;
        case EcsIrAwaitLaunch:
            if (op->a != -1) {
                ecs_strbuf_append(buf, "args=r%d +%d", op->a, op->b);
            }
            break;
        case EcsIrUsing:
            ecs_strbuf_append(buf, "'%s'",
                ((const ecs_script_using_t*)op->node)->name);
            break;
        case EcsIrModule:
            ecs_strbuf_append(buf, "'%s'",
                ((const ecs_script_module_t*)op->node)->name);
            break;
        case EcsIrAnnot:
            ecs_strbuf_append(buf, "@%s",
                ((const ecs_script_annot_t*)op->node)->name);
            break;
        case EcsIrTemplate:
            ecs_strbuf_append(buf, "'%s'",
                ((const ecs_script_template_node_t*)op->node)->name);
            break;
        case EcsIrConstEnd:
            ecs_strbuf_append(buf, "r%d -> '%s'", op->a,
                ((const ecs_script_var_node_t*)op->node)->name);
            break;
        case EcsIrExprBegin:
            ecs_strbuf_append(buf, "r%d +%d", op->a, op->b);
            break;
        case EcsIrLoadConst: {
            const ecs_expr_node_t *node = op->node;
            char *str = ecs_ptr_to_expr(world, node->type, op->imm.ptr);
            ecs_strbuf_append(buf, "r%d <- %s", op->a, str);
            ecs_os_free(str);
            flecs_irs_type_to_buf(script, node, buf);
            break;
        }
        case EcsIrLoadVar:
            ecs_strbuf_append(buf, "r%d <- sp %d '%s'", op->a, op->b,
                ((const ecs_expr_variable_t*)op->node)->name);
            break;
        case EcsIrLoadGlobal: {
            char *path = ecs_get_path(world, op->imm.entity);
            ecs_strbuf_append(buf, "r%d <- %s", op->a, path);
            ecs_os_free(path);
            break;
        }
        case EcsIrLoadSymbol:
            ecs_strbuf_append(buf, "r%d <- sym %d '%s'", op->a, op->b,
                ((const ecs_expr_identifier_t*)op->node)->value);
            break;
        case EcsIrLookup:
            ecs_strbuf_append(buf, "r%d <- '%s'", op->a, op->imm.str);
            break;
        case EcsIrMember:
            ecs_strbuf_append(buf, "r%d <- r%d +%d", op->a, op->b, op->c);
            flecs_irs_type_to_buf(script, op->node, buf);
            break;
        case EcsIrSwizzle:
            ecs_strbuf_append(buf, "r%d <- r%d.%s", op->a, op->b,
                ((const ecs_expr_swizzle_t*)op->node)->name);
            break;
        case EcsIrElement:
            ecs_strbuf_append(buf, "r%d <- r%d[r%d]", op->a, op->b, op->c);
            break;
        case EcsIrComponentGet: {
            char *path = ecs_get_path(world, op->imm.entity);
            ecs_strbuf_append(buf, "r%d <- r%d[%s]", op->a, op->b, path);
            ecs_os_free(path);
            break;
        }
        case EcsIrHas: {
            char *str = ecs_id_str(world, op->imm.id);
            ecs_strbuf_append(buf, "r%d <- r%d has %s", op->a, op->b, str);
            ecs_os_free(str);
            break;
        }
        case EcsIrUnary:
            ecs_strbuf_append(buf, "r%d <- %s r%d", op->a, flecs_token_str(
                ((const ecs_expr_unary_t*)op->node)->operator), op->b);
            break;
        case EcsIrBinary:
        case EcsIrBinaryI64:
        case EcsIrBinaryI32:
        case EcsIrBinaryF64:
        case EcsIrBinaryF32:
            ecs_strbuf_append(buf, "r%d <- r%d %s r%d", op->a, op->b,
                flecs_token_str((ecs_token_kind_t)(op->flags & 0x7fff)),
                op->c);
            flecs_irs_type_to_buf(script, op->node, buf);
            break;
        case EcsIrJumpIfFalse:
        case EcsIrJumpIfTrue:
            ecs_strbuf_append(buf, "r%d -> %d (r%d)", op->a, op->b, op->c);
            break;
        case EcsIrCast:
        case EcsIrCastNumber:
            ecs_strbuf_append(buf, "r%d <- r%d", op->a, op->b);
            flecs_irs_type_to_buf(script, op->node, buf);
            break;
        case EcsIrCall:
            ecs_strbuf_append(buf, "r%d <- %s(r%d +%d)", op->a,
                ((const ecs_expr_function_t*)op->node)->function_name,
                op->b, op->c);
            break;
        case EcsIrInitAlloc:
            ecs_strbuf_append(buf, "r%d", op->a);
            flecs_irs_type_to_buf(script, op->node, buf);
            break;
        case EcsIrInitConst: {
            const ecs_expr_node_t *node = op->node;
            char *str = ecs_ptr_to_expr(world, node->type, op->imm.ptr);
            ecs_strbuf_append(buf, "r%d +%d <- %s", op->a, op->b, str);
            ecs_os_free(str);
            flecs_irs_type_to_buf(script, node, buf);
            break;
        }
        case EcsIrInitAssign:
            ecs_strbuf_append(buf, "r%d +%d <- r%d", op->a, op->b, op->c);
            break;
        case EcsIrInitOp:
            ecs_strbuf_append(buf, "r%d +%d %s r%d", op->a, op->b,
                flecs_token_str((ecs_token_kind_t)op->flags), op->c);
            break;
        case EcsIrInitSwizzle:
            ecs_strbuf_append(buf, "r%d +%d <- r%d", op->a, op->b, op->c);
            break;
        case EcsIrDynBegin:
        case EcsIrDynEnd:
        case EcsIrDynKey:
        case EcsIrDynSet:
        case EcsIrDynSwizzle:
        case EcsIrStrValue:
        case EcsIrStrCast:
        case EcsIrMatchFail:
        case EcsIrToBool:
        case EcsIrReturn:
            ecs_strbuf_append(buf, "r%d", op->a);
            break;
        case EcsIrDynMember:
            ecs_strbuf_append(buf, "'%s'", op->imm.str);
            break;
        case EcsIrStrFrag:
            ecs_strbuf_append(buf, "\"%s\"", op->imm.str);
            break;
        case EcsIrStrFormat:
            ecs_strbuf_append(buf, "r%d", op->a);
            if (op->b != -1) {
                ecs_strbuf_append(buf, " width=r%d", op->b);
            }
            if (op->c != -1) {
                ecs_strbuf_append(buf, " precision=r%d", op->c);
            }
            break;
        case EcsIrStrBegin:
            break;
        case EcsIrStrEnd:
            ecs_strbuf_append(buf, "r%d", op->a);
            break;
        case EcsIrMatchCmp:
            ecs_strbuf_append(buf, "r%d == r%d else-> %d", op->a, op->b, op->c);
            break;
        case EcsIrRange:
            ecs_strbuf_append(buf, "r%d <- r%d..r%d", op->a, op->b, op->c);
            break;
        case EcsIrNew:
            ecs_strbuf_append(buf, "r%d <- block %d", op->a, op->b);
            break;
        case EcsIrScript:
            ecs_strbuf_append(buf, "r%d", op->a);
            break;
        case EcsIrEnd:
        case EcsIrAnnotClear:
        case EcsIrScopeLeave:
        case EcsIrEntityLeave:
        case EcsIrWithEnter:
        case EcsIrWithBody:
        case EcsIrWithLeave:
        case EcsIrPairScopeLeave:
        case EcsIrIfLeave:
        case EcsIrForLeave:
        case EcsIrContinue:
        case EcsIrTryLeave:
        case EcsIrAwaitPoll:
        case EcsIrMutCheck:
        case EcsIrExprEnd:
        case EcsIrDynPush:
        case EcsIrDynPop:
        case EcsIrDynNext:
        case EcsIrOpKindLast:
            break;
        }

        ecs_strbuf_appendch(buf, '\n');
    }
}

#endif
