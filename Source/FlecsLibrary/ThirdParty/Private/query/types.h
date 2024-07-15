 /**
 * @file query/types.h
 * @brief Internal types and functions for queries.
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

/* -- Variable types -- */
typedef enum {
    EcsVarEntity,          /* Variable that stores an entity id */
    EcsVarTable,           /* Variable that stores a table */
    EcsVarAny              /* Used when requesting either entity or table var */
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

/* -- Instruction kinds -- */
typedef enum {
    EcsQueryAnd,            /* And operator: find or match id against variable source */
    EcsQueryAndId,          /* And operator for fixed id (no wildcards/variables) */
    EcsQueryAndAny,         /* And operator with support for matching Any src/id */
    EcsQueryOnlyAny,        /* Dedicated instruction for _ queries where the src is unknown */
    EcsQueryTriv,           /* Trivial search (batches multiple terms) */
    EcsQueryTrivData,       /* Trivial search with setting data fields */
    EcsQueryTrivWildcard,   /* Trivial search with (exclusive) wildcard ids */
    EcsQueryCache,          /* Cached search */
    EcsQueryCacheData,      /* Cached search with setting data fields */
    EcsQueryIsCache,        /* Cached search for queries that are entirely cached */
    EcsQueryIsCacheData,    /* Same as EcsQueryIsCache with data fields */
    EcsQueryUp,             /* Up traversal */
    EcsQueryUpId,           /* Up traversal for fixed id (like AndId) */
    EcsQuerySelfUp,         /* Self|up traversal */
    EcsQuerySelfUpId,       /* Self|up traversal for fixed id (like AndId) */
    EcsQueryWith,           /* Match id against fixed or variable source */
    EcsQueryTrav,           /* Support for transitive/reflexive queries */
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
    EcsQueryPredEq,         /* Test if variable is equal to, or assign to if not set */
    EcsQueryPredNeq,        /* Test if variable is not equal to */
    EcsQueryPredEqName,     /* Same as EcsQueryPredEq but with matching by name */
    EcsQueryPredNeqName,    /* Same as EcsQueryPredNeq but with matching by name */
    EcsQueryPredEqMatch,    /* Same as EcsQueryPredEq but with fuzzy matching by name */
    EcsQueryPredNeqMatch,   /* Same as EcsQueryPredNeq but with fuzzy matching by name */
    EcsQueryMemberEq,       /* Compare member value */
    EcsQueryMemberNeq,      /* Compare member value */
    EcsQueryToggle,         /* Evaluate toggle bitset, if present */
    EcsQueryToggleOption,   /* Toggle for optional terms */
    EcsQueryUnionEq,        /* Evaluate union relationship */
    EcsQueryUnionEqWith,    /* Evaluate union relationship against fixed or variable source */
    EcsQueryUnionNeq,       /* Evaluate union relationship */
    EcsQueryUnionEqUp,      /* Evaluate union relationship w/up traversal */
    EcsQueryUnionEqSelfUp,  /* Evaluate union relationship w/self|up traversal */
    EcsQueryLookup,         /* Lookup relative to variable */
    EcsQuerySetVars,        /* Populate it.sources from variables */
    EcsQuerySetThis,        /* Populate This entity variable */
    EcsQuerySetFixed,       /* Set fixed source entity ids */
    EcsQuerySetIds,         /* Set fixed (component) ids */
    EcsQuerySetId,          /* Set id if not set */
    EcsQueryContain,        /* Test if table contains entity */
    EcsQueryPairEq,         /* Test if both elements of pair are the same */
    EcsQueryPopulate,       /* Populate any data fields */
    EcsQueryPopulateSelf,   /* Populate only self (owned) data fields */
    EcsQueryPopulateSparse, /* Populate sparse fields */
    EcsQueryYield,          /* Yield result back to application */
    EcsQueryNothing         /* Must be last */
} ecs_query_op_kind_t;

/* Op flags to indicate if ecs_query_ref_t is entity or variable */
#define EcsQueryIsEntity  (1 << 0)
#define EcsQueryIsVar     (1 << 1)
#define EcsQueryIsSelf    (1 << 6)

/* Op flags used to shift EcsQueryIsEntity and EcsQueryIsVar */
#define EcsQuerySrc     0
#define EcsQueryFirst   2
#define EcsQuerySecond  4

/* References to variable or entity */
typedef union {
    ecs_var_id_t var;
    ecs_entity_t entity;
} ecs_query_ref_t;

/* Query instruction */
typedef struct ecs_query_op_t {
    uint8_t kind;              /* Instruction kind */
    ecs_flags8_t flags;        /* Flags storing whether 1st/2nd are variables */
    int8_t field_index;        /* Query field corresponding with operation */
    int8_t term_index;         /* Query term corresponding with operation */
    ecs_query_lbl_t prev;      /* Backtracking label (no data) */
    ecs_query_lbl_t next;      /* Forwarding label. Must come after prev */
    ecs_query_lbl_t other;     /* Misc register used for control flow */
    ecs_flags16_t match_flags; /* Flags that modify matching behavior */
    ecs_query_ref_t src;
    ecs_query_ref_t first;
    ecs_query_ref_t second;
    ecs_flags64_t written;     /* Bitset with variables written by op */
} ecs_query_op_t;

 /* And context */
typedef struct {
    ecs_id_record_t *idr;
    ecs_table_cache_iter_t it;
    int16_t column;
    int16_t remaining;
} ecs_query_and_ctx_t;

/* Union context */
typedef struct {
    ecs_id_record_t *idr;
    ecs_table_range_t range;
    ecs_map_iter_t tgt_iter;
    ecs_entity_t cur;
    ecs_entity_t tgt;
    int32_t row;
} ecs_query_union_ctx_t;

/* Down traversal cache (for resolving up queries w/unknown source) */
typedef struct {
    ecs_table_t *table;
    bool leaf; /* Table owns and inherits id (for Up queries without Self) */
} ecs_trav_down_elem_t;

typedef struct {
    ecs_vec_t elems;      /* vector<ecs_trav_down_elem_t> */
    bool ready;
} ecs_trav_down_t;

typedef struct {
    ecs_entity_t src;
    ecs_id_t id;
    int32_t column;
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

/* And up context */
typedef struct {
    union {
        ecs_query_and_ctx_t and;
        ecs_query_union_ctx_t union_;
    } is;
    ecs_table_t *table;
    int32_t row;
    int32_t end;
    ecs_entity_t trav;
    ecs_id_t with;
    ecs_id_t matched;
    ecs_id_record_t *idr_with;
    ecs_id_record_t *idr_trav;
    ecs_trav_down_t *down;
    int32_t cache_elem;
    ecs_trav_up_cache_t cache;
} ecs_query_up_ctx_t;

/* Cache for storing results of upward/downward "all" traversal. This type of 
 * traversal iterates and caches the entire tree. */
typedef struct {
    ecs_entity_t entity;
    ecs_id_record_t *idr;
    int32_t column;
} ecs_trav_elem_t;

typedef struct {
    ecs_id_t id;
    ecs_id_record_t *idr;
    ecs_vec_t entities;
    bool up;
} ecs_trav_cache_t;

/* Trav context */
typedef struct {
    ecs_query_and_ctx_t and;
    int32_t index;
    int32_t offset;
    int32_t count;
    ecs_trav_cache_t cache;
    bool yield_reflexive;
} ecs_query_trav_ctx_t;

 /* Eq context */
typedef struct {
    ecs_table_range_t range;
    int32_t index;
    int16_t name_col;
    bool redo;
} ecs_query_eq_ctx_t;

 /* Each context */
typedef struct {
    int32_t row;
} ecs_query_each_ctx_t;

 /* Setthis context */
typedef struct {
    ecs_table_range_t range;
} ecs_query_setthis_ctx_t;

/* Ids context */
typedef struct {
    ecs_id_record_t *cur;
} ecs_query_ids_ctx_t;

/* Control flow context */
typedef struct {
    ecs_query_lbl_t op_index;
    ecs_id_t field_id;
    bool is_set;
} ecs_query_ctrl_ctx_t;

/* Trivial iterator context */
typedef struct {
    ecs_table_cache_iter_t it;
    const ecs_table_record_t *tr;
    int32_t first_to_eval;
} ecs_query_trivial_ctx_t;

/* *From operator iterator context */
typedef struct {
    ecs_query_and_ctx_t and;
    ecs_type_t *type;
    int32_t first_id_index;
    int32_t cur_id_index;
} ecs_query_xfrom_ctx_t;

/* Member equality context */
typedef struct {
    ecs_query_each_ctx_t each;
    void *data;
} ecs_query_membereq_ctx_t;

/* Toggle context */
typedef struct {
    ecs_table_range_t range;
    int32_t cur;
    int32_t block_index;
    ecs_flags64_t block;
    ecs_termset_t prev_set_fields;
    bool optional_not;
    bool has_bitset;
} ecs_query_toggle_ctx_t;

typedef struct ecs_query_op_ctx_t {
    union {
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
        ecs_query_union_ctx_t union_;
    } is;
} ecs_query_op_ctx_t;

typedef struct {
    /* Labels used for control flow */
    ecs_query_lbl_t lbl_query; /* Used to find the op that does the actual searching */
    ecs_query_lbl_t lbl_begin;
    ecs_query_lbl_t lbl_cond_eval;
    ecs_write_flags_t written_or; /* Cond written flags at start of or chain */
    ecs_write_flags_t cond_written_or; /* Cond written flags at start of or chain */
    ecs_query_ref_t src_or;  /* Source for terms in current or chain */
    bool src_written_or; /* Was src populated before OR chain */
    bool in_or; /* Whether we're in an or chain */
} ecs_query_compile_ctrlflow_t;

/* Query compiler state */
typedef struct {
    ecs_vec_t *ops;
    ecs_write_flags_t written; /* Bitmask to check which variables have been written */
    ecs_write_flags_t cond_written; /* Track conditional writes (optional operators) */

    /* Maintain control flow per scope */
    ecs_query_compile_ctrlflow_t ctrlflow[FLECS_QUERY_SCOPE_NESTING_MAX];
    ecs_query_compile_ctrlflow_t *cur; /* Current scope */

    int32_t scope; /* Nesting level of query scopes */
    ecs_flags32_t scope_is_not; /* Whether scope is prefixed with not */
    ecs_oper_kind_t oper; /* Temp storage to track current operator for term */
    int32_t skipped; /* Term skipped during compilation */
} ecs_query_compile_ctx_t;    

/* Query run state */
typedef struct {
    uint64_t *written;                 /* Bitset to check which variables have been written */
    ecs_query_lbl_t op_index;          /* Currently evaluated operation */
    ecs_var_t *vars;                   /* Variable storage */
    ecs_iter_t *it;                    /* Iterator */
    ecs_query_op_ctx_t *op_ctx;        /* Operation context (stack) */
    ecs_world_t *world;                /* Reference to world */
    const ecs_query_impl_t *query;     /* Reference to query */
    const ecs_query_var_t *query_vars; /* Reference to query variable array */
    ecs_query_iter_t *qit;
} ecs_query_run_ctx_t;

typedef struct {
    ecs_query_var_t var;
    const char *name;
} ecs_query_var_cache_t;

struct ecs_query_impl_t {
    ecs_query_t pub;              /* Public query data */

    ecs_stage_t *stage;           /* Stage used for allocations */

    /* Variables */
    ecs_query_var_t *vars;        /* Variables */
    int32_t var_count;            /* Number of variables */
    int32_t var_size;             /* Size of variable array */
    ecs_hashmap_t tvar_index;     /* Name index for table variables */
    ecs_hashmap_t evar_index;     /* Name index for entity variables */
    ecs_query_var_cache_t vars_cache; /* For trivial queries with only This variables */
    ecs_var_id_t *src_vars;       /* Array with ids to source variables for fields */

    /* Query plan */
    ecs_query_op_t *ops;          /* Operations */
    int32_t op_count;             /* Number of operations */

    /* Query cache */
    struct ecs_query_cache_t *cache; /* Cache, if query contains cached terms */
    int8_t *field_map;            /* Map field indices from cache to query */

    /* Change detection */
    int32_t *monitor;             /* Change monitor for fields with fixed src */

    /* Misc */
    int16_t tokens_len;           /* Length of tokens buffer */
    char *tokens;                 /* Buffer with string tokens used by terms */

    /* User context */
    ecs_ctx_free_t ctx_free;         /* Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /* Callback to free binding_ctx */

    /* Mixins */
    flecs_poly_dtor_t dtor;
};


/* Query cache types */

/** Table match data.
 * Each table matched by the query is represented by an ecs_query_cache_table_match_t
 * instance, which are linked together in a list. A table may match a query
 * multiple times (due to wildcard queries) with different columns being matched
 * by the query. */
struct ecs_query_cache_table_match_t {
    ecs_query_cache_table_match_t *next, *prev;
    ecs_table_t *table;              /* The current table. */
    int32_t offset;                  /* Starting point in table  */
    int32_t count;                   /* Number of entities to iterate in table */
    int32_t *columns;                /* Mapping from query fields to table columns */
    int32_t *storage_columns;        /* Mapping from query fields to storage columns */
    ecs_id_t *ids;                   /* Resolved (component) ids for current table */
    ecs_entity_t *sources;           /* Subjects (sources) of ids */
    ecs_vec_t refs;                  /* Cached components for non-this terms */
    ecs_termset_t set_fields;        /* Fields that are set */
    ecs_termset_t up_fields;         /* Fields that are matched through traversal */
    uint64_t group_id;               /* Value used to organize tables in groups */
    int32_t *monitor;                /* Used to monitor table for changes */

    /* Next match in cache for same table (includes empty tables) */
    ecs_query_cache_table_match_t *next_match;
};

/** Table record type for query table cache. A query only has one per table. */
typedef struct ecs_query_cache_table_t {
    ecs_table_cache_hdr_t hdr;       /* Header for ecs_table_cache_t */
    ecs_query_cache_table_match_t *first;  /* List with matches for table */
    ecs_query_cache_table_match_t *last;   /* Last discovered match for table */
    uint64_t table_id;
    int32_t rematch_count;           /* Track whether table was rematched */
} ecs_query_cache_table_t;

/** Points to the beginning & ending of a query group */
typedef struct ecs_query_cache_table_list_t {
    ecs_query_cache_table_match_t *first;
    ecs_query_cache_table_match_t *last;
    ecs_query_group_info_t info;
} ecs_query_cache_table_list_t;

/* Query event type for notifying queries of world events */
typedef enum ecs_query_cache_eventkind_t {
    EcsQueryTableMatch,
    EcsQueryTableRematch,
    EcsQueryTableUnmatch,
} ecs_query_cache_eventkind_t;

typedef struct ecs_query_cache_event_t {
    ecs_query_cache_eventkind_t kind;
    ecs_table_t *table;
} ecs_query_cache_event_t;

/* Query level block allocators have sizes that depend on query field count */
typedef struct ecs_query_cache_allocators_t {
    ecs_block_allocator_t columns;
    ecs_block_allocator_t ids;
    ecs_block_allocator_t sources;
    ecs_block_allocator_t monitors;
} ecs_query_cache_allocators_t;

/** Query that is automatically matched against tables */
typedef struct ecs_query_cache_t {
    /* Uncached query used to populate the cache */
    ecs_query_t *query;

    /* Observer to keep the cache in sync */
    ecs_observer_t *observer;

    /* Tables matched with query */
    ecs_table_cache_t cache;

    /* Linked list with all matched non-empty tables, in iteration order */
    ecs_query_cache_table_list_t list;

    /* Contains head/tail to nodes of query groups (if group_by is used) */
    ecs_map_t groups;

    /* Table sorting */
    ecs_entity_t order_by;
    ecs_order_by_action_t order_by_callback;
    ecs_sort_table_action_t order_by_table_callback;
    ecs_vec_t table_slices;
    int32_t order_by_term;

    /* Table grouping */
    ecs_entity_t group_by;
    ecs_group_by_action_t group_by_callback;
    ecs_group_create_action_t on_group_create;
    ecs_group_delete_action_t on_group_delete;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Monitor generation */
    int32_t monitor_generation;

    int32_t cascade_by;              /* Identify cascade term */
    int32_t match_count;             /* How often have tables been (un)matched */
    int32_t prev_match_count;        /* Track if sorting is needed */
    int32_t rematch_count;           /* Track which tables were added during rematch */
    
    ecs_entity_t entity;

    /* Query-level allocators */
    ecs_query_cache_allocators_t allocators;
} ecs_query_cache_t;

#endif
