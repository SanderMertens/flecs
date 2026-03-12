 /**
 * @file query/types.h
 * @brief Internal types for query variables, instructions, and execution contexts.
 */

#ifndef FLECS_QUERY_TYPES
#define FLECS_QUERY_TYPES

typedef struct ecs_query_impl_t ecs_query_impl_t;
typedef uint8_t ecs_var_id_t;
typedef int16_t ecs_query_lbl_t;
typedef ecs_flags64_t ecs_write_flags_t;

#define flecs_query_impl(query) (ECS_CONST_CAST(ecs_query_impl_t*, query))
#define EcsQueryMaxVarCount     (64)
#define EcsVarNone              ((ecs_var_id_t)-1)
#define EcsThisName             "this"

/* Variable storage kinds */
typedef enum {
    EcsVarEntity,          /* Stores a single entity id */
    EcsVarTable,           /* Stores a table (set of entities with same archetype) */
    EcsVarAny              /* Resolved to entity or table depending on context */
} ecs_var_kind_t;

typedef struct ecs_query_var_t {
    int8_t kind;           /* variable kind (EcsVarEntity or EcsVarTable) */
    bool anonymous;        /* variable is anonymous */
    ecs_var_id_t id;       /* variable id */
    ecs_var_id_t table_id; /* id to table variable, if any */
    ecs_var_id_t base_id;  /* id to base entity variable, for lookups */
    const char *name;      /* variable name */
    const char *lookup;    /* Lookup string for variable */
#ifdef FLECS_DEBUG
    const char *label;     /* for debugging */
#endif
} ecs_query_var_t;

/* Placeholder values for queries with only $this variable */
extern ecs_query_var_t flecs_this_array;
extern char *flecs_this_name_array;

/* Query VM instruction kinds (see eval.c for dispatch) */
typedef enum {
    EcsQueryAll,            /* Yield all tables */
    EcsQueryAnd,            /* Find or match id against variable source */
    EcsQueryAndAny,         /* And with support for matching Any src/id */
    EcsQueryAndWcTgt,       /* And for (*, T) wildcard-first queries */
    EcsQueryTriv,           /* Batched search for multiple trivial And terms */
    EcsQueryCache,          /* Search partially cached query */
    EcsQueryIsCache,        /* Search fully cached query */
    EcsQueryUp,             /* Traverse relationship upward to find component */
    EcsQuerySelfUp,         /* Match self first, then traverse up */
    EcsQueryWith,           /* Match id against already-resolved source */
    EcsQueryWithWcTgt,      /* Match (*, T) against already-resolved source */
    EcsQueryTrav,           /* Transitive/reflexive relationship traversal */
    EcsQueryAndFrom,        /* AndFrom operator */
    EcsQueryOrFrom,         /* OrFrom operator */
    EcsQueryNotFrom,        /* NotFrom operator */
    EcsQueryIds,            /* Test for existence of ids matching wildcard */
    EcsQueryIdsRight,       /* Find ids in use that match (R, *) wildcard */
    EcsQueryIdsLeft,        /* Find ids in use that match (*, T) wildcard */
    EcsQueryEach,           /* Iterate entities in table, populate entity variable */
    EcsQueryStore,          /* Store table or entity in variable */
    EcsQueryReset,          /* Reset value of variable to wildcard (*) */
    EcsQueryOr,             /* Or operator */
    EcsQueryOptional,       /* Optional operator */
    EcsQueryIfVar,          /* Conditional execution on whether variable is set */
    EcsQueryIfSet,          /* Conditional execution on whether term is set */
    EcsQueryNot,            /* Sets iterator state after term was not matched */
    EcsQueryEnd,            /* End of control flow block */
    EcsQueryPredEq,         /* Test equality, or assign if variable is unset */
    EcsQueryPredNeq,        /* Test inequality */
    EcsQueryPredEqName,     /* Equality test by entity name */
    EcsQueryPredNeqName,    /* Inequality test by entity name */
    EcsQueryPredEqMatch,    /* Equality test by fuzzy name matching */
    EcsQueryPredNeqMatch,   /* Inequality test by fuzzy name matching */
    EcsQueryMemberEq,       /* Compare member value */
    EcsQueryMemberNeq,      /* Compare member value */
    EcsQueryToggle,         /* Evaluate toggle bitset, if present */
    EcsQueryToggleOption,   /* Toggle for optional terms */
    EcsQuerySparse,         /* Evaluate sparse component */
    EcsQuerySparseNot,      /* Evaluate sparse component with not operator */
    EcsQuerySparseSelfUp,   /* Sparse: match self, then traverse up */
    EcsQuerySparseUp,       /* Sparse: traverse up only */
    EcsQuerySparseWith,     /* Sparse: match against already-resolved source */
    EcsQueryTree,           /* ChildOf hierarchy: iterate children of target */
    EcsQueryTreeWildcard,   /* ChildOf hierarchy: iterate children of wildcard target */
    EcsQueryTreeWith,       /* ChildOf hierarchy: match against resolved source */
    EcsQueryTreeUp,         /* ChildOf up: union of up(ChildOf) and Parent tables */
    EcsQueryTreeSelfUp,     /* ChildOf self|up traversal */
    EcsQueryTreePre,        /* ChildOf: return entire tables without Parent filter */
    EcsQueryTreePost,       /* ChildOf: filter results by Parent component */
    EcsQueryTreeUpPre,      /* ChildOf up: return entire tables without Parent filter */
    EcsQueryTreeSelfUpPre,  /* ChildOf self|up: return entire tables without Parent filter */
    EcsQueryTreeUpPost,     /* ChildOf up: filter cached tables with Parent */
    EcsQueryTreeSelfUpPost, /* ChildOf self|up: filter cached tables with Parent */
    EcsQueryChildren,       /* Return children for parent, if possible in order */
    EcsQueryChildrenWc,     /* Return children for parents, if possible in order */
    EcsQueryLookup,         /* Lookup relative to variable */
    EcsQuerySetVars,        /* Populate it.sources from variables */
    EcsQuerySetThis,        /* Populate This entity variable */
    EcsQuerySetFixed,       /* Set fixed source entity ids */
    EcsQuerySetIds,         /* Set fixed (component) ids */
    EcsQuerySetId,          /* Set id if not set */
    EcsQueryContain,        /* Test if table contains entity */
    EcsQueryPairEq,         /* Test if both elements of pair are the same */
    EcsQueryYield,          /* Yield result back to application */
    EcsQueryNothing         /* Must be last */
} ecs_query_op_kind_t;

/* Per-reference flags: whether a ref slot holds an entity or variable */
#define EcsQueryIsEntity  (1 << 0)
#define EcsQueryIsVar     (1 << 1)
#define EcsQueryIsSelf    (1 << 6)

/* Bit offsets for shifting EcsQueryIsEntity/EcsQueryIsVar per ref slot */
#define EcsQuerySrc     0
#define EcsQueryFirst   2
#define EcsQuerySecond  4

/* A reference to either a variable or a fixed entity */
typedef union {
    ecs_var_id_t var;
    ecs_entity_t entity;
} ecs_query_ref_t;

/* Single instruction in the query VM program */
typedef struct ecs_query_op_t {
    uint8_t kind;              /* Instruction kind (ecs_query_op_kind_t) */
    ecs_flags8_t flags;        /* Packed IsEntity/IsVar flags for src/first/second */
    int8_t field_index;        /* Iterator field this op populates (-1 = none) */
    int8_t term_index;         /* Original query term index */
    ecs_query_lbl_t prev;      /* Backtracking label (jump on redo failure) */
    ecs_query_lbl_t next;      /* Forward label (jump on success) */
    ecs_query_lbl_t other;     /* Auxiliary label for control flow (e.g., filter mask) */
    ecs_flags16_t match_flags; /* Flags that modify matching behavior */
    ecs_query_ref_t src;
    ecs_query_ref_t first;
    ecs_query_ref_t second;
    ecs_flags64_t written;     /* Bitmask of variables written by this op */
} ecs_query_op_t;

/* Per-op context for EcsQueryAll */
typedef struct {
    int32_t cur;
    ecs_table_record_t dummy_tr;
} ecs_query_all_ctx_t;

/* Per-op context for EcsQueryAnd and similar table-iterating ops */
typedef struct {
    ecs_component_record_t *cr;
    ecs_table_cache_iter_t it;
    int16_t column;
    int16_t remaining;        /* Remaining wildcard matches in current table */
    bool non_fragmenting;
} ecs_query_and_ctx_t;

/* Per-op context for sparse component iteration */
typedef struct {
    ecs_query_and_ctx_t and_; /* Fallback for mixed sparse/regular results */

    ecs_sparse_t *sparse;
    ecs_table_range_t range;
    int32_t cur;
    bool self;
    bool exclusive;

    ecs_component_record_t *cr;
    ecs_table_range_t prev_range;
    int32_t prev_cur;
} ecs_query_sparse_ctx_t;

typedef enum ecs_query_tree_iter_state_t {
    EcsQueryTreeIterNext = 1,
    EcsQueryTreeIterTables = 2,
    EcsQueryTreeIterEntities = 3
} ecs_query_tree_iter_state_t;

typedef struct {
    ecs_query_and_ctx_t and_; /* For mixed results */
    ecs_component_record_t *cr;
    ecs_entity_t tgt;
    ecs_entity_t *entities;
    const EcsParent *parents;
    ecs_table_range_t range;
    int32_t cur;
    ecs_query_tree_iter_state_t state;
} ecs_query_tree_ctx_t;

typedef struct {
    ecs_component_record_t *cr;
    ecs_table_cache_iter_t it;
    ecs_entity_t *entities;
    int32_t count;
    int32_t cur;
    ecs_query_tree_iter_state_t state;
} ecs_query_tree_wildcard_ctx_t;

/* Element in the down-traversal cache: a table reachable through a relationship */
typedef struct {
    ecs_table_range_t range;
    bool leaf; /* Table both owns and inherits the id (for Up-only queries) */
} ecs_trav_down_elem_t;

typedef struct {
    ecs_vec_t elems;      /* vector<ecs_trav_down_elem_t> */
    bool ready;
} ecs_trav_down_t;

typedef struct {
    ecs_entity_t src;
    ecs_id_t id;
    ecs_table_record_t *tr;
    bool ready;
} ecs_trav_up_t;

typedef enum {
    EcsTravUp = 1,
    EcsTravDown = 2
} ecs_trav_direction_t;

typedef struct {
    ecs_map_t src;        /* map<entity, trav_down_t> or map<table_id, trav_up_t> */
    ecs_id_t with;
    ecs_trav_direction_t dir;
} ecs_trav_up_cache_t;

/* Up-traversal context: walks traversable entities to find component sources */
typedef struct {
    ecs_table_t *table;
    int32_t row;
    int32_t end;
    ecs_entity_t trav;           /* Relationship used for traversal */
    ecs_id_t with;               /* Component id being searched for */
    ecs_id_t matched;            /* Actual id that was matched */
    ecs_component_record_t *cr_with;
    ecs_component_record_t *cr_trav;

    /* For non-fragmenting ChildOf: iterate ordered children vector directly
     * instead of using tables with ChildOf pairs as down-cache roots */
    ecs_entity_t *entities;
    int32_t entities_cur;
    int32_t entities_count;

    ecs_trav_down_t *down;
    int32_t cache_elem;
    ecs_trav_up_cache_t cache;
} ecs_query_up_impl_t;

typedef struct {
    union {
        ecs_query_and_ctx_t and;
        ecs_query_sparse_ctx_t sparse_;
    } is;

    /* Indirection because otherwise the ctx struct gets too large */
    ecs_query_up_impl_t *impl;

    /* Data for returning tables with non-fragmenting ChildOf */
    const EcsParent *parents;
    ecs_table_range_t range;
    int32_t cur;
} ecs_query_up_ctx_t;

typedef struct {
    union {
        ecs_query_and_ctx_t and;
        ecs_query_up_ctx_t up_;
    } is;
    ecs_query_tree_iter_state_t state;
} ecs_query_tree_pre_ctx_t;

/* Element in the full-tree traversal cache (walks and caches the entire tree) */
typedef struct {
    ecs_entity_t entity;
    ecs_component_record_t *cr;
    const ecs_table_record_t *tr;
} ecs_trav_elem_t;

typedef struct {
    ecs_id_t id;
    ecs_component_record_t *cr;
    ecs_vec_t entities;
    bool up;
} ecs_trav_cache_t;

/* Per-op context for transitive/reflexive relationship traversal */
typedef struct {
    ecs_query_and_ctx_t and;
    int32_t index;
    int32_t offset;
    int32_t count;
    ecs_trav_cache_t cache;
    bool yield_reflexive;
} ecs_query_trav_ctx_t;

/* Per-op context for equality predicates */
typedef struct {
    ecs_table_range_t range;
    int32_t index;
    int16_t name_col;
    bool redo;
} ecs_query_eq_ctx_t;

/* Per-op context for EcsQueryEach (row-by-row entity iteration) */
typedef struct {
    int32_t row;
} ecs_query_each_ctx_t;

/* Per-op context for EcsQuerySetThis */
typedef struct {
    ecs_table_range_t range;
} ecs_query_setthis_ctx_t;

/* Per-op context for wildcard id discovery */
typedef struct {
    ecs_component_record_t *cur;
} ecs_query_ids_ctx_t;

/* Per-op context for control flow (Or, Not, Optional, IfVar, IfSet) */
typedef struct {
    ecs_query_lbl_t op_index;
    ecs_id_t field_id;
    bool is_set;
} ecs_query_ctrl_ctx_t;

/* Per-op context for trivial (batched) term evaluation */
typedef struct {
    ecs_table_cache_iter_t it;
    const ecs_table_record_t *tr;
    int32_t start_from;       /* Index of first trivial term */
    int32_t first_to_eval;    /* Index of second trivial term (first is the driver) */
} ecs_query_trivial_ctx_t;

/* Per-op context for *From operators (AndFrom, OrFrom, NotFrom) */
typedef struct {
    ecs_query_and_ctx_t and;
    ecs_entity_t type_id;
    ecs_type_t *type;
    int32_t first_id_index;
    int32_t cur_id_index;
} ecs_query_xfrom_ctx_t;

/* Per-op context for component member comparison */
typedef struct {
    ecs_query_each_ctx_t each;
    void *data;
} ecs_query_membereq_ctx_t;

/* Per-op context for toggle bitset evaluation */
typedef struct {
    ecs_table_range_t range;
    int32_t cur;               /* Current entity offset */
    int32_t block_index;       /* Current 64-bit block in the bitset */
    ecs_flags64_t block;       /* Current block value (masked by toggle fields) */
    ecs_termset_t prev_set_fields;
    bool optional_not;
    bool has_bitset;
} ecs_query_toggle_ctx_t;

/* Per-op context for optional terms */
typedef struct {
    ecs_table_range_t range;
} ecs_query_optional_ctx_t;

typedef struct ecs_query_op_ctx_t {
    union {
        ecs_query_all_ctx_t all;
        ecs_query_and_ctx_t and;
        ecs_query_xfrom_ctx_t xfrom;
        ecs_query_up_ctx_t up;
        ecs_query_trav_ctx_t trav;
        ecs_query_ids_ctx_t ids;
        ecs_query_eq_ctx_t eq;
        ecs_query_each_ctx_t each;
        ecs_query_setthis_ctx_t setthis;
        ecs_query_ctrl_ctx_t ctrl;
        ecs_query_trivial_ctx_t trivial;
        ecs_query_membereq_ctx_t membereq;
        ecs_query_toggle_ctx_t toggle;
        ecs_query_sparse_ctx_t sparse;
        ecs_query_tree_ctx_t tree;
        ecs_query_tree_pre_ctx_t tree_pre;
        ecs_query_tree_wildcard_ctx_t tree_wildcard;
        ecs_query_optional_ctx_t optional;
    } is;
} ecs_query_op_ctx_t;

/* Per-scope control flow state during compilation */
typedef struct {
    ecs_query_lbl_t lbl_query;       /* Op index that performs the actual search */
    ecs_query_lbl_t lbl_begin;       /* Start of current control flow block */
    ecs_query_lbl_t lbl_cond_eval;   /* Start of conditional evaluation block */
    ecs_write_flags_t written_or;      /* Written vars snapshot before OR chain */
    ecs_write_flags_t cond_written_or; /* Conditional writes snapshot before OR chain */
    ecs_query_ref_t src_or;  /* Source variable for current OR chain */
    bool src_written_or;     /* Whether src was written before OR chain */
    bool in_or;              /* Currently inside an OR chain */
} ecs_query_compile_ctrlflow_t;

/* Compiler state: tracks written variables, scopes, and emitted ops */
typedef struct {
    ecs_vec_t *ops;
    ecs_write_flags_t written;      /* Bitmask of variables written so far */
    ecs_write_flags_t cond_written; /* Variables conditionally written (e.g., optional) */

    /* Control flow state per nesting level */
    ecs_query_compile_ctrlflow_t ctrlflow[FLECS_QUERY_SCOPE_NESTING_MAX];
    ecs_query_compile_ctrlflow_t *cur; /* Active scope */

    int32_t scope;           /* Current nesting depth */
    ecs_flags32_t scope_is_not; /* Whether current scope is negated */
    ecs_oper_kind_t oper;    /* Current term operator (temporary) */
    int32_t skipped;         /* Number of terms skipped during compilation */
} ecs_query_compile_ctx_t;

/* Runtime execution state for the query VM */
typedef struct {
    uint64_t *written;                 /* Per-op bitmask of written variables */
    ecs_query_lbl_t op_index;          /* Index of currently executing op */
    ecs_var_t *vars;                   /* Variable value storage */
    ecs_iter_t *it;                    /* Output iterator */
    ecs_query_op_ctx_t *op_ctx;        /* Per-op context stack */
    ecs_world_t *world;
    const ecs_query_impl_t *query;
    const ecs_query_var_t *query_vars; /* Variable metadata array */
    ecs_query_iter_t *qit;
} ecs_query_run_ctx_t;

struct ecs_query_impl_t {
    ecs_query_t pub;              /* Public query data */

    ecs_stage_t *stage;           /* Stage used for allocations */

    /* Variables (index 0 = $this, rest = user/anonymous variables) */
    ecs_query_var_t *vars;        /* Variable metadata array */
    int32_t var_count;            /* Total variables (including anonymous) */
    int32_t var_size;             /* Allocated capacity of vars array */
    ecs_hashmap_t tvar_index;     /* Name -> id index for table variables */
    ecs_hashmap_t evar_index;     /* Name -> id index for entity variables */
    ecs_var_id_t *src_vars;       /* Maps field index -> source variable id */

    /* Compiled query plan (VM program) */
    ecs_query_op_t *ops;          /* Instruction array */
    int32_t op_count;             /* Number of instructions */

    /* String storage */
    int16_t tokens_len;           /* Length of tokens buffer */
    char *tokens;                 /* Interned string tokens referenced by terms */
    int32_t *monitor;             /* Change monitor for fixed-source fields */

#ifdef FLECS_DEBUG
    ecs_termset_t final_terms;    /* Terms using Final trait (no component inheritance) */
#endif

    /* Query cache (NULL if uncached) */
    struct ecs_query_cache_t *cache;

    /* User context cleanup */
    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t binding_ctx_free;

    /* Polymorphic destructor */
    flecs_poly_dtor_t dtor;
};

#endif
