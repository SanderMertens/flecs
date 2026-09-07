/**
 * @file addons/script/ir/ir.h
 * @brief Script IR (intermediate representation) runtime.
 */

#ifndef FLECS_SCRIPT_IR_H
#define FLECS_SCRIPT_IR_H

typedef enum ecs_script_ir_op_kind_t {
    EcsIrEnd,
    EcsIrJump,
    EcsIrStmt,
    EcsIrStmtBlock,
    EcsIrMark,
    EcsIrAnnotClear,
    EcsIrScopeEnter,
    EcsIrScopeLeave,
    EcsIrEntityEnter,
    EcsIrEntityLeave,
    EcsIrWithEnter,
    EcsIrWithBody,
    EcsIrWithLeave,
    EcsIrWithTag,
    EcsIrWithComponentBegin,
    EcsIrWithComponentEnd,
    EcsIrPairScopeEnter,
    EcsIrPairScopeLeave,
    EcsIrIfEnter,
    EcsIrIfLeave,
    EcsIrForEnter,
    EcsIrForNext,
    EcsIrForLeave,
    EcsIrContinue,
    EcsIrTryEnter,
    EcsIrTryLeave,
    EcsIrAwaitStart,
    EcsIrAwaitLaunch,
    EcsIrAwaitPoll,
    EcsIrTag,
    EcsIrComponentBegin,
    EcsIrComponentEnd,
    EcsIrComponentVisitor,
    EcsIrUsing,
    EcsIrModule,
    EcsIrAnnot,
    EcsIrTemplate,
    EcsIrMutCheck,
    EcsIrConstEnd,
    EcsIrConstCached,
    EcsIrExprBegin,
    EcsIrExprEnd,
    EcsIrLoadConst,
    EcsIrLoadVar,
    EcsIrLoadGlobal,
    EcsIrLoadSymbol,
    EcsIrLookup,
    EcsIrMember,
    EcsIrSwizzle,
    EcsIrElement,
    EcsIrComponentGet,
    EcsIrHas,
    EcsIrUnary,
    EcsIrBinary,
    EcsIrBinaryI64,
    EcsIrBinaryI32,
    EcsIrBinaryF64,
    EcsIrBinaryF32,
    EcsIrJumpIfFalse,
    EcsIrJumpIfTrue,
    EcsIrCast,
    EcsIrCastNumber,
    EcsIrCall,
    EcsIrInitAlloc,
    EcsIrInitConst,
    EcsIrInitAssign,
    EcsIrInitOp,
    EcsIrInitSwizzle,
    EcsIrDynBegin,
    EcsIrDynPush,
    EcsIrDynPop,
    EcsIrDynNext,
    EcsIrDynMember,
    EcsIrDynKey,
    EcsIrDynSet,
    EcsIrDynSwizzle,
    EcsIrDynEnd,
    EcsIrStrBegin,
    EcsIrStrFrag,
    EcsIrStrValue,
    EcsIrStrCast,
    EcsIrStrFormat,
    EcsIrStrEnd,
    EcsIrMatchCmp,
    EcsIrMatchFail,
    EcsIrRange,
    EcsIrNew,
    EcsIrScript,
    EcsIrToBool,
    EcsIrReturn,
    EcsIrOpKindLast
} ecs_script_ir_op_kind_t;

#define EcsIrStmtAlways      (1u << 0)
#define EcsIrStmtSkip        (1u << 1)
#define EcsIrStmtCached      (1u << 2)
#define EcsIrScopeEntity     (1u << 0)
#define EcsIrIdDirect        (1u << 1)
#define EcsIrInPlace         (1u << 0)
#define EcsIrCallMethod      (1u << 1)
#define EcsIrAwaitMethod     (1u << 0)
#define EcsIrAwaitVar        (1u << 1)
#define EcsIrAnnotNoTarget   (1u << 0)
#define EcsIrAnnotBadTarget  (1u << 1)
#define EcsIrComponentInterface (1u << 1)
#define EcsIrComponentPartial   (1u << 2)
#define EcsIrElementMap      (1u << 0)
#define EcsIrElementVector   (1u << 1)

typedef enum ecs_script_ir_profile_stat_t {
    EcsIrProfileRun,
    EcsIrProfileStmtRun,
    EcsIrProfileStmtSkipped,
    EcsIrProfileBatchAdd,
    EcsIrProfileBatchSkip,
    EcsIrProfileEntityCreate,
    EcsIrProfileEntityReuse,
    EcsIrProfileComponentSet,
    EcsIrProfileTagAdd,
    EcsIrProfileLast
} ecs_script_ir_profile_stat_t;

const char* flecs_script_ir_op_name(
    ecs_script_ir_op_kind_t kind);

typedef enum ecs_script_ir_num_class_t {
    EcsIrNumNone,
    EcsIrNumSigned,
    EcsIrNumUnsigned,
    EcsIrNumFloat
} ecs_script_ir_num_class_t;

typedef struct ecs_script_ir_op_t {
    uint16_t kind;
    uint16_t flags;
    int32_t a;
    int32_t b;
    int32_t c;
    union {
        int64_t i64;
        uint64_t u64;
        double f64;
        ecs_id_t id;
        ecs_entity_t entity;
        void *ptr;
        const char *str;
    } imm;
    const void *node;
} ecs_script_ir_op_t;

typedef struct ecs_script_ir_id_t {
    ecs_id_t eval;
    ecs_id_t flag;
    ecs_entity_t first_eval;
    ecs_entity_t second_eval;
    ecs_entity_t interface;
    const char *first;
    const char *second;
    const ecs_type_info_t *ti;
    int32_t first_symbol;
    int32_t second_symbol;
    int32_t first_sp;
    int32_t second_sp;
    int32_t first_reg;
    int32_t second_reg;
    int32_t value_sp;
    int32_t component_slot;
    ecs_id_t cache_id;
    const ecs_type_info_t *cache_ti;
    bool cache_visitor;
    bool resolved;
    bool dynamic;
    bool has_second;
    bool needs_set;
    bool visitor;
} ecs_script_ir_id_t;

typedef enum ecs_script_ir_entry_kind_t {
    EcsIrEntryRoot,
    EcsIrEntryTemplate,
    EcsIrEntryEntity,
    EcsIrEntryFunction
} ecs_script_ir_entry_kind_t;

typedef struct ecs_script_ir_entry_t {
    const void *node;
    ecs_script_ir_entry_kind_t kind;
    int32_t pc;
    int32_t reg_count;
} ecs_script_ir_entry_t;

struct ecs_script_ir_t {
    ecs_vec_t ops;
    ecs_vec_t ids;
    ecs_vec_t catches;
    ecs_vec_t entries;
    ecs_map_t entry_index;
    int32_t root_entry;
};

typedef struct ecs_script_ir_reg_t {
    ecs_value_t value;
    const ecs_type_info_t *ti;
    void *storage;
    bool owned;
} ecs_script_ir_reg_t;

typedef enum ecs_script_ir_frame_kind_t {
    EcsIrFrameBlock,
    EcsIrFrameScope,
    EcsIrFrameEntity,
    EcsIrFrameWith,
    EcsIrFramePairScope,
    EcsIrFrameIf,
    EcsIrFrameFor,
    EcsIrFrameTry,
    EcsIrFrameExpr
} ecs_script_ir_frame_kind_t;

typedef struct ecs_script_ir_frame_t {
    int16_t kind;
    int16_t state;
    int32_t pc;
    union {
        struct {
            flecs_script_scope_state_t state;
            int32_t var_count;
            int32_t vscratch_top;
            int32_t vheap_count;
        } scope;
        flecs_script_entity_state_t entity;
        flecs_script_with_state_t with;
        flecs_script_pair_scope_state_t pair_scope;
        struct {
            bool force;
        } if_;
        struct {
            flecs_script_for_state_t state;
            int32_t var_count;
            int32_t vscratch_top;
            int32_t vheap_count;
            int32_t var_index[3];
        } for_;
        struct {
            int32_t scratch_top;
            int32_t owned_count;
            int32_t heap_count;
            int32_t strbuf_count;
            int32_t cursor_count;
        } expr;
        struct {
            int32_t pc;
            int32_t reg_base;
            int32_t entry;
            bool vars;
        } block;
    } u;
} ecs_script_ir_frame_t;

#define ECS_SCRIPT_IR_MAX_FRAMES (ECS_SCRIPT_VISIT_MAX_DEPTH * 3)
#define ECS_SCRIPT_IR_FRAME_CHUNK_SIZE (16)

typedef struct ecs_script_ir_arena_t {
    char *data;
    int32_t size;
    int32_t top;
    ecs_vec_t heap;
} ecs_script_ir_arena_t;

typedef struct ecs_script_ir_vm_t {
    ecs_script_eval_visitor_t v;
    const ecs_script_ir_t *ir;
    const ecs_script_ir_op_t *ops;
    int32_t pc;
    int32_t entry;
    int32_t reg_base;
    ecs_script_ir_reg_t *regs;
    int32_t reg_count;
    ecs_script_ir_arena_t scratch;
    ecs_script_ir_arena_t vscratch;
    ecs_vec_t owned;
    ecs_script_ir_frame_t *frames[(ECS_SCRIPT_IR_MAX_FRAMES +
        ECS_SCRIPT_IR_FRAME_CHUNK_SIZE - 1) / ECS_SCRIPT_IR_FRAME_CHUNK_SIZE];
    int32_t frame_count;
    ecs_vec_t strbufs;
    ecs_vec_t cursors;
    ecs_vec_t pending_marks;
    ecs_entity_t last_entity;
    bool cond;
    bool dirty;
    bool can_suspend;
#ifdef FLECS_SCRIPT_ASYNC
    flecs_script_async_state_t async;
#endif
} ecs_script_ir_vm_t;

ecs_script_ir_t* flecs_script_ir_compile(
    ecs_script_impl_t *script);

void flecs_script_ir_free(
    ecs_script_ir_t *ir);

ecs_script_ir_t* flecs_script_ir_ensure(
    ecs_script_impl_t *script);

const ecs_script_ir_entry_t* flecs_script_ir_entry(
    const ecs_script_ir_t *ir,
    const void *node);

void flecs_script_ir_vm_init(
    ecs_script_ir_vm_t *vm);

void flecs_script_ir_vm_fini(
    ecs_script_ir_vm_t *vm,
    const ecs_script_eval_desc_t *desc);

ecs_script_ir_vm_t* flecs_script_ir_vm_new(
    ecs_script_impl_t *script,
    const ecs_script_eval_desc_t *desc);

void flecs_script_ir_vm_free(
    ecs_script_ir_vm_t *vm,
    const ecs_script_eval_desc_t *desc);

void flecs_script_ir_vm_pool_fini(
    ecs_script_runtime_t *r);

void flecs_script_ir_vm_abandon(
    ecs_script_ir_vm_t *vm);

flecs_script_run_status_t flecs_script_ir_vm_run(
    ecs_script_ir_vm_t *vm,
    const ecs_script_ir_entry_t *entry);

flecs_script_run_status_t flecs_script_ir_vm_resume(
    ecs_script_ir_vm_t *vm);

const char* flecs_script_ir_vm_stmt_pos(
    const ecs_script_ir_vm_t *vm);

int flecs_script_ir_eval_root(
    ecs_script_eval_visitor_t *v,
    ecs_script_impl_t *script);

int flecs_script_ir_eval_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_impl_t *script,
    ecs_script_template_t *template);

void flecs_script_ir_call_function(
    const ecs_function_ctx_t *ctx,
    ecs_script_user_function_t *uf,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

void flecs_script_ir_to_buf(
    const ecs_script_impl_t *script,
    const ecs_script_ir_t *ir,
    ecs_strbuf_t *buf);

#endif
