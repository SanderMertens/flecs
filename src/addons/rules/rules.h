 /**
 * @file addons/rules/rules.h
 * @brief Internal types and functions for rules addon.
 */

#include "../../private_api.h"

#ifdef FLECS_RULES

#define EcsRuleMaxVarCount      (64)
#define EcsVarNone              ((uint32_t)-1)
#define EcsThisName             "This"

typedef uint32_t ecs_var_id_t;
typedef ecs_flags64_t ecs_write_flags_t;

/* -- Variable types -- */
typedef enum {
    EcsVarEntity,
    EcsVarTable,
    EcsVarAny
} ecs_var_kind_t;

typedef struct {
    ecs_var_kind_t kind;
    ecs_var_id_t id;
    const char *name;
    const char *label; /* for debugging */
    ecs_var_id_t table_id; /* id to table variable, if any */
} ecs_rule_var_t;

/* -- Op types -- */
typedef enum {
    EcsRuleAnd,
    EcsRuleEach,
    EcsRuleDown,
    EcsRuleStore,
    EcsRuleUnion,
    EcsRuleEnd,
    EcsRuleNot,
    EcsRuleOption,
    EcsRuleSetVars,
    EcsRuleSetThis,
    EcsRuleContain,
    EcsRulePairEq,
    EcsRuleSetCond,
    EcsRuleJmpCondFalse,
    EcsRuleJmpNotSet,
    EcsRuleYield
} ecs_rule_op_kind_t;

/* Op flags to indicate if reference is entity or variable */
#define EcsRuleIsEntity  (1 << 0)
#define EcsRuleIsVar     (1 << 1)
#define EcsRuleIsSelf    (1 << 6)

/* Op flags used to shift EcsRuleIsEntity and EcsRuleIsVar */
#define EcsRuleSrc     0
#define EcsRuleFirst   2
#define EcsRuleSecond  4

/* Match flags used for special match instructions */
#define EcsRuleMatchAny    (1 << 0)
#define EcsRuleMatchAnySrc (1 << 1)
#define EcsRulePairSame    (1 << 2)

/* References to variable or entity (resolved during evaluation) */
typedef union {
    ecs_var_id_t var;
    ecs_entity_t entity;
} ecs_rule_ref_t;

typedef struct {
    uint16_t kind;             /* Instruction kind */
    int16_t prev;              /* Backtracking label (no data) */
    int16_t next;              /* Forwarding label */
    int16_t other;
    ecs_flags8_t flags;        /* Flags storing whether 1st/2nd are variables */
    ecs_flags8_t match_flags;  /* Flags that modify matching behavior */
    int8_t field_index;        /* Query field corresponding with operation */
    ecs_rule_ref_t src;
    ecs_rule_ref_t first;
    ecs_rule_ref_t second;
    ecs_flags64_t written;     /* Bitset with variables written by op */
    ecs_entity_t trav;
} ecs_rule_op_t;

 /* And context */
typedef struct {
    ecs_id_record_t *idr;
    ecs_table_cache_iter_t it;
    int32_t remaining;

    ecs_table_t *table;
    ecs_vector_t *tables;
    int32_t index;
} ecs_rule_and_ctx_t;

 /* Each context */
typedef struct {
    int32_t row;
} ecs_rule_each_ctx_t;

 /* Each context */
typedef struct {
    bool redo_latch;
} ecs_rule_option_ctx_t;

 /* Down context */
typedef struct {
    ecs_id_t id;
    ecs_vector_t *tables;
    int32_t index;
} ecs_rule_down_ctx_t;

 /* TravDown context */
typedef struct {
    ecs_table_range_t root;
} ecs_rule_trav_down_ctx_t;

/* End context */
typedef struct {
    int32_t lbl;
} ecs_rule_ctrlflow_ctx_t;

/* Condition context */
typedef struct {
    bool cond;
} ecs_rule_cond_ctx_t;

typedef struct ecs_rule_op_ctx_t {
    int32_t sp;
    union {
        ecs_rule_and_ctx_t and;
        ecs_rule_each_ctx_t each;
        ecs_rule_option_ctx_t option;
        ecs_rule_down_ctx_t down;
        ecs_rule_trav_down_ctx_t trav_down;
        ecs_rule_ctrlflow_ctx_t ctrlflow;
        ecs_rule_cond_ctx_t cond;
    } is;
} ecs_rule_op_ctx_t;

/* Rule compiler state */
typedef struct {
    ecs_vector_t *ops;
    ecs_write_flags_t written; /* Bitmask to check which variables have been written */
    ecs_write_flags_t cond_written; /* Track conditional writes (optional operators) */
    int32_t lbl_union;
    int32_t lbl_not;
    int32_t lbl_option;
    int32_t lbl_cond_eval;
} ecs_rule_compile_ctx_t;

/* Rule compiler state */
typedef struct {
    const ecs_rule_t *rule;
    ecs_iter_t *it;
    ecs_rule_iter_t *rit;
    ecs_rule_op_ctx_t *ctx;
    int32_t op_index;
    int32_t prev_index;
    int32_t jump;
    ecs_stage_t *stage;
} ecs_rule_run_ctx_t;

typedef struct {
    ecs_rule_var_t var;
    const char *name;
} ecs_rule_var_cache_t;

struct ecs_rule_t {
    ecs_header_t hdr;
    ecs_filter_t filter;

    /* Variables */
    ecs_rule_var_t *vars;
    int32_t var_count;
    int32_t var_pub_count; /* Number of public variables */
    int32_t var_size;
    ecs_hashmap_t tvar_index;
    ecs_hashmap_t evar_index;
    ecs_rule_var_cache_t vars_cache; /* For trivial rules with only This variables */
    char **var_names; /* Array with variable names for iterator */
    int32_t *src_vars; /* Array with ids to source variables for fields */

    /* Operations */
    ecs_rule_op_t *ops;
    int32_t op_count;

    /* Mixins */
    ecs_world_t *world;
    ecs_iterable_t iterable;
    ecs_poly_dtor_t dtor;
};

ecs_flags16_t flecs_rule_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind);

bool flecs_ref_is_written(
    const ecs_rule_t *rule,
    const ecs_rule_op_t *op,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written);

bool flecs_rule_is_written(
    uint32_t var_id,
    uint64_t written);

void flecs_rule_discover_vars(
    ecs_stage_t *stage,
    ecs_rule_t *rule);

void flecs_rule_compile(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_rule_t *rule);

#endif
