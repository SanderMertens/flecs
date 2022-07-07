#include "../private_api.h"

#ifdef FLECS_RULES

#include <stdio.h>

/** Implementation of the rule query engine.
 * 
 * A rule (terminology borrowed from prolog) is a list of constraints that 
 * specify which conditions must be met for an entity to match the rule. While
 * this description matches any kind of ECS query, the rule engine has features
 * that go beyond regular (flecs) ECS queries:
 * 
 * - query for all components of an entity (vs. all entities for a component)
 * - query for all relationship pairs of an entity
 * - support for query variables that are resolved at evaluation time
 * - automatic traversal of transitive relationships
 * 
 * Query terms can have the following forms:
 * 
 * - Component(Subject)
 * - Relation(Subject, Object)
 * 
 * Additionally the query parser supports the following shorthand notations:
 * 
 * - Component             // short for Component(This)
 * - (Relation, Object)    // short for Relation(This, Object)
 * 
 * The subject, or first arugment of a term represents the entity on which the
 * component or relation is matched. By default the subject is set to a builtin
 * This variable, which causes the behavior to match a regular ECS query:
 * 
 * - Position, Velocity
 * 
 * Is equivalent to
 *
 * - Position(This), Velocity(This)
 * 
 * The function of the variable is to ensure that all components are matched on
 * the same entity. Conceptually the query first populates the This variable
 * with all entities that have Position. When the query evaluates the Velocity
 * term, the variable is populated and the entity it contains will be checked
 * for whether it has Velocity.
 * 
 * The actual implementation is more efficient and does not check per-entity.
 * 
 * Custom variables can be used to join parts of different terms. For example, 
 * the following query can be used to find entities with a parent that has a
 * Position component (note that variable names start with a _):
 * 
 * - ChildOf(This, _Parent), Component(_Parent)
 * 
 * The rule engine uses a backtracking algorithm to find the set of entities
 * and variables that match all terms. As soon as the engine finds a term that
 * does not match with the currently evaluated entity, the entity is discarded.
 * When an entity is found for which all terms match, the entity is yielded to
 * the iterator.
 * 
 * While a rule is being evaluated, a variable can either contain a single 
 * entity or a table. The engine will attempt to work with tables as much as
 * possible so entities can be eliminated/yielded in bulk. A rule may store
 * both the table and entity version of a variable and only switch from table to
 * entity when necessary.
 * 
 * The rule engine has an algorithm for computing which variables should be 
 * resolved first. This algorithm works by finding a "root" variable, which is
 * the subject variable that occurs in the term with the least dependencies. The
 * remaining variables are then resolved based on their "distance" from the root
 * with the closest variables being resolved first.
 * 
 * This generally results in an ordering that resolves the variables with the 
 * least dependencies first and the most dependencies last, which is beneficial
 * for two reasons:
 * 
 * - it improves the average performance of all queries
 * - it makes performance less dependent on how an application orders the terms
 * 
 * A possible improvement would be for the query engine to also consider
 * the number of tables that need to be evaluated for each term, as starting
 * with the smallest term reduces the amount of work. Other than static variable
 * analysis however, this can only be determined when the query is executed.
 * 
 * Rules are "compiled" into a set of instructions that encode the operations
 * the query needs to perform in order to find the right set of entities.
 * Operations can either yield data, which progresses the program, or signal
 * that there is no (more) matching data, which discards the current variables.
 * 
 * An operation can yield multiple times, if there are multiple matches for its
 * inputs. Operations are called with a redo flag, which can be either true or
 * false. When redo is true the operation will yield the next result. When redo
 * is false, the operation will reset its state and start from the first result.
 * 
 * Operations can have an input, output and a filter. Most commonly an operation
 * either matches the filter against an input and yields if it matches, or uses
 * the filter to find all matching results and store the result in the output.
 *
 * Variables are resolved by matching a filter against the output of an 
 * operation. When a term contains variables, they are encoded as register ids
 * in the filter. When the filter is evaluated, the most recent values of the
 * register are used to match/lookup the output.
 * 
 * For example, a filter could be (ChildOf, _Parent). When the program starts,
 * the _Parent register is initialized with *, so that when this filter is first
 * evaluated, the operation will find all tables with (ChildOf, *). The _Parent
 * register is then populated by taking the actual value of the table. If the
 * table has type [(ChildOf, Sun)], _Parent will be initialized with Sun.
 * 
 * It is possible that a filter matches multiple times. Consider the filter
 * (Likes, _Food), and a table [(Likes, Apples), (Likes, Pears)]. In this case
 * an operation will yield the table twice, once with _Food=Apples, and once
 * with _Food=Pears.
 * 
 * If a rule contains a term with a transitive relation, it will automatically
 * substitute the parts of the term to find a fact that matches. The following
 * examples illustrate how transitivity is resolved:
 * 
 * Query:
 *   LocatedIn(Bob, SanFrancisco)
 *   
 * Expands to:
 *   LocatedIn(Bob, SanFrancisco:self|subset)
 * 
 * Explanation:
 *   "Is Bob located in San Francisco" - This term is true if Bob is either 
 *   located in San Francisco, or is located in anything that is itself located 
 *   in (a subset of) San Francisco.
 * 
 * 
 * Query:
 *   LocatedIn(Bob, X)
 * 
 * Expands to:
 *   LocatedIn(Bob, X:self|superset)
 * 
 * Explanation:
 *   "Where is Bob located?" - This term recursively returns all places that
 *   Bob is located in, which includes his location and the supersets of his
 *   location. When Bob is located in San Francisco, he is also located in
 *   the United States, North America etc.
 * 
 * 
 * Query:
 *   LocatedIn(X, NorthAmerica)
 * 
 * Expands to:
 *   LocatedIn(X, NorthAmerica:self|subset)
 * 
 * Explanation:
 *   "What is located in North America?" - This term returns everything located
 *   in North America and its subsets, as something located in San Francisco is
 *   located in UnitedStates, which is located in NorthAmerica. 
 * 
 * 
 * Query:
 *   LocatedIn(X, Y)
 * 
 * Expands to:
 *   LocatedIn(X, Y)
 * 
 * Explanation:
 *   "Where is everything located" - This term returns everything that is
 *   located somewhere. No substitution is performed as this would explode the
 *   results while not yielding new information.
 * 
 * 
 * In the above terms, the variable indicates the part of the term that is
 * unknown at evaluation time. In an actual rule the picked strategy depends on
 * whether the variable is known when the term is evaluated. For example, if
 * variable X has been resolved by the time Located(X, Y) is evaluated, the
 * strategy from the LocatedIn(Bob, X) example will be used.
 */

#define ECS_RULE_MAX_VAR_COUNT (32)

#define RULE_PAIR_PREDICATE (1)
#define RULE_PAIR_OBJECT (2)

/* A rule pair contains a predicate and object that can be stored in a register. */
typedef struct ecs_rule_pair_t {
    union {
        int32_t reg;
        ecs_entity_t ent;
    } pred;
    union {
        int32_t reg;
        ecs_entity_t ent;
    } obj;
    int32_t reg_mask; /* bit 1 = predicate, bit 2 = object */

    bool transitive; /* Is predicate transitive */
    bool final;      /* Is predicate final */
    bool reflexive;  /* Is predicate reflexive */
    bool acyclic;    /* Is predicate acyclic */
    bool obj_0;
} ecs_rule_pair_t;

/* Filter for evaluating & reifing types and variables. Filters are created ad-
 * hoc from pairs, and take into account all variables that had been resolved
 * up to that point. */
typedef struct ecs_rule_filter_t {
    ecs_id_t mask;  /* Mask with wildcard in place of variables */

    bool wildcard; /* Does the filter contain wildcards */
    bool pred_wildcard; /* Is predicate a wildcard */
    bool obj_wildcard; /* Is object a wildcard */
    bool same_var; /* True if pred & obj are both the same variable */

    int32_t hi_var; /* If hi part should be stored in var, this is the var id */
    int32_t lo_var; /* If lo part should be stored in var, this is the var id */
} ecs_rule_filter_t;

/* A rule register stores temporary values for rule variables */
typedef enum ecs_rule_var_kind_t {
    EcsRuleVarKindTable, /* Used for sorting, must be smallest */
    EcsRuleVarKindEntity,
    EcsRuleVarKindUnknown
} ecs_rule_var_kind_t;

/* Operations describe how the rule should be evaluated */
typedef enum ecs_rule_op_kind_t {
    EcsRuleInput,       /* Input placeholder, first instruction in every rule */
    EcsRuleSelect,      /* Selects all ables for a given predicate */
    EcsRuleWith,        /* Applies a filter to a table or entity */
    EcsRuleSubSet,      /* Finds all subsets for transitive relationship */
    EcsRuleSuperSet,    /* Finds all supersets for a transitive relationship */
    EcsRuleStore,       /* Store entity in table or entity variable */
    EcsRuleEach,        /* Forwards each entity in a table */
    EcsRuleSetJmp,      /* Set label for jump operation to one of two values */
    EcsRuleJump,        /* Jump to an operation label */
    EcsRuleNot,         /* Invert result of an operation */
    EcsRuleInTable,     /* Test if entity (subject) is in table (r_in) */
    EcsRuleEq,          /* Test if entity in (subject) and (r_in) are equal */
    EcsRuleYield        /* Yield result */
} ecs_rule_op_kind_t;

/* Single operation */
typedef struct ecs_rule_op_t {
    ecs_rule_op_kind_t kind;    /* What kind of operation is it */
    ecs_rule_pair_t filter;     /* Parameter that contains optional filter */
    ecs_entity_t subject;       /* If set, operation has a constant subject */

    int32_t on_pass;            /* Jump location when match succeeds */
    int32_t on_fail;            /* Jump location when match fails */
    int32_t frame;              /* Register frame */

    int32_t term;               /* Corresponding term index in signature */
    int32_t r_in;               /* Optional In/Out registers */
    int32_t r_out;

    bool has_in, has_out;       /* Keep track of whether operation uses input
                                 * and/or output registers. This helps with
                                 * debugging rule programs. */
} ecs_rule_op_t;

/* With context. Shared with select. */
typedef struct ecs_rule_with_ctx_t {
    ecs_id_record_t *idr;      /* Currently evaluated table set */
    ecs_table_cache_iter_t it;
    int32_t column;
} ecs_rule_with_ctx_t;

/* Subset context */
typedef struct ecs_rule_subset_frame_t {
    ecs_rule_with_ctx_t with_ctx;
    ecs_table_t *table;
    int32_t row;
    int32_t column;
} ecs_rule_subset_frame_t;

typedef struct ecs_rule_subset_ctx_t {
    ecs_rule_subset_frame_t storage[16]; /* Alloc-free array for small trees */
    ecs_rule_subset_frame_t *stack;
    int32_t sp;
} ecs_rule_subset_ctx_t;

/* Superset context */
typedef struct ecs_rule_superset_frame_t {
    ecs_table_t *table;
    int32_t column;
} ecs_rule_superset_frame_t;

typedef struct ecs_rule_superset_ctx_t {
    ecs_rule_superset_frame_t storage[16]; /* Alloc-free array for small trees */
    ecs_rule_superset_frame_t *stack;
    ecs_id_record_t *idr;
    int32_t sp;
} ecs_rule_superset_ctx_t;

/* Each context */
typedef struct ecs_rule_each_ctx_t {
    int32_t row;                /* Currently evaluated row in evaluated table */
} ecs_rule_each_ctx_t;

/* Jump context */
typedef struct ecs_rule_setjmp_ctx_t {
    int32_t label;             /* Operation label to jump to */
} ecs_rule_setjmp_ctx_t;

/* Operation context. This is a per-operation, per-iterator structure that
 * stores information for stateful operations. */
typedef struct ecs_rule_op_ctx_t {
    union {
        ecs_rule_subset_ctx_t subset;
        ecs_rule_superset_ctx_t superset;
        ecs_rule_with_ctx_t with;
        ecs_rule_each_ctx_t each;
        ecs_rule_setjmp_ctx_t setjmp;
    } is;
} ecs_rule_op_ctx_t;

/* Rule variables allow for the rule to be parameterized */
typedef struct ecs_rule_var_t {
    ecs_rule_var_kind_t kind;
    char *name;       /* Variable name */
    int32_t id;       /* Unique variable id */
    int32_t other;    /* Id to table variable (-1 if none exists) */
    int32_t occurs;   /* Number of occurrences (used for operation ordering) */
    int32_t depth;  /* Depth in dependency tree (used for operation ordering) */
    bool marked;      /* Used for cycle detection */
} ecs_rule_var_t;

/* Variable ids per term */
typedef struct ecs_rule_term_vars_t {
    int32_t pred;
    int32_t subj;
    int32_t obj;
} ecs_rule_term_vars_t;

/* Top-level rule datastructure */
struct ecs_rule_t {
    ecs_header_t hdr;
    
    ecs_world_t *world;         /* Ref to world so rule can be used by itself */
    ecs_rule_op_t *operations;  /* Operations array */
    ecs_filter_t filter;        /* Filter of rule */

    /* Variable array */
    ecs_rule_var_t vars[ECS_RULE_MAX_VAR_COUNT];

    /* Passed to iterator */
    char *var_names[ECS_RULE_MAX_VAR_COUNT]; 

    /* Variable ids used in terms */
    ecs_rule_term_vars_t term_vars[ECS_RULE_MAX_VAR_COUNT];

    /* Variable evaluation order */
    int32_t var_eval_order[ECS_RULE_MAX_VAR_COUNT];

    int32_t var_count;          /* Number of variables in signature */
    int32_t subj_var_count;
    int32_t frame_count;        /* Number of register frames */
    int32_t operation_count;    /* Number of operations in rule */

    ecs_iterable_t iterable;    /* Iterable mixin */
};

/* ecs_rule_t mixins */
ecs_mixins_t ecs_rule_t_mixins = {
    .type_name = "ecs_rule_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_rule_t, world),
        [EcsMixinIterable] = offsetof(ecs_rule_t, iterable)
    }
};

static
void rule_error(
    const ecs_rule_t *rule,
    const char *fmt,
    ...)
{
    char *fstr = ecs_filter_str(rule->world, &rule->filter);
    va_list valist;
    va_start(valist, fmt);
    ecs_parser_errorv(rule->filter.name, fstr, -1, fmt, valist);
    va_end(valist);
    ecs_os_free(fstr);
}

static
bool subj_is_set(
    ecs_term_t *term)
{
    return ecs_term_id_is_set(&term->subj);
}

static
bool obj_is_set(
    ecs_term_t *term)
{
    return ecs_term_id_is_set(&term->obj) || term->role == ECS_PAIR;
}

static
ecs_rule_op_t* create_operation(
    ecs_rule_t *rule)
{
    int32_t cur = rule->operation_count ++;
    rule->operations = ecs_os_realloc(
        rule->operations, (cur + 1) * ECS_SIZEOF(ecs_rule_op_t));

    ecs_rule_op_t *result = &rule->operations[cur];
    ecs_os_memset_t(result, 0, ecs_rule_op_t);

    return result;
}

static
const char* get_var_name(const char *name) {
    if (name && !ecs_os_strcmp(name, "This")) {
        /* Make sure that both This and . resolve to the same variable */
        name = ".";
    }

    return name;
}

static
ecs_rule_var_t* create_variable(
    ecs_rule_t *rule,
    ecs_rule_var_kind_t kind,
    const char *name)
{
    int32_t cur = ++ rule->var_count;
    
    name = get_var_name(name);
    if (name && !ecs_os_strcmp(name, "*")) {
        /* Wildcards are treated as anonymous variables */
        name = NULL;
    }

    ecs_rule_var_t *var = &rule->vars[cur - 1];
    if (name) {
        var->name = ecs_os_strdup(name);
    } else {
        /* Anonymous register */
        char name_buff[32];
        ecs_os_sprintf(name_buff, "_%u", cur - 1);
        var->name = ecs_os_strdup(name_buff);
    }

    var->kind = kind;

    /* The variable id is the location in the variable array and also points to
     * the register element that corresponds with the variable. */
    var->id = cur - 1;

    /* Depth is used to calculate how far the variable is from the root, where
     * the root is the variable with 0 dependencies. */
    var->depth = UINT8_MAX;
    var->marked = false;
    var->occurs = 0;

    return var;
}

static
ecs_rule_var_t* create_anonymous_variable(
    ecs_rule_t *rule,
    ecs_rule_var_kind_t kind)
{
    return create_variable(rule, kind, NULL);
}

/* Find variable with specified name and type. If Unknown is provided as type,
 * the function will return any variable with the provided name. The root 
 * variable can occur both as a table and entity variable, as some rules
 * require that each entity in a table is iterated. In this case, there are two
 * variables, one for the table and one for the entities in the table, that both
 * have the same name. */
static
ecs_rule_var_t* find_variable(
    const ecs_rule_t *rule,
    ecs_rule_var_kind_t kind,
    const char *name)
{
    ecs_assert(rule != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);

    name = get_var_name(name);

    const ecs_rule_var_t *variables = rule->vars;
    int32_t i, count = rule->var_count;
    
    for (i = 0; i < count; i ++) {
        const ecs_rule_var_t *variable = &variables[i];
        if (!ecs_os_strcmp(name, variable->name)) {
            if (kind == EcsRuleVarKindUnknown || kind == variable->kind) {
                return (ecs_rule_var_t*)variable;
            }
        }
    }

    return NULL;
}

/* Ensure variable with specified name and type exists. If an existing variable
 * is found with an unknown type, its type will be overwritten with the 
 * specified type. During the variable ordering phase it is not yet clear which
 * variable is the root. Which variable is the root determines its type, which
 * is why during this phase variables are still untyped. */
static
ecs_rule_var_t* ensure_variable(
    ecs_rule_t *rule,
    ecs_rule_var_kind_t kind,
    const char *name)
{
    ecs_rule_var_t *var = find_variable(rule, kind, name);
    if (!var) {
        var = create_variable(rule, kind, name);
    } else {
        if (var->kind == EcsRuleVarKindUnknown) {
            var->kind = kind;
        }
    }

    return var;
}

static
const char *term_id_var_name(
    ecs_term_id_t *term_id)
{
    if (term_id->var == EcsVarIsVariable) {
        if (term_id->name) {
            return term_id->name;
        } else if (term_id->entity == EcsThis) {
            return ".";
        } else if (term_id->entity == EcsWildcard) {
            return "*";
        } else if (term_id->entity == EcsAny) {
            return "_";
        } else if (term_id->entity == EcsVariable) {
            return "$";
        } else {
            ecs_check(term_id->name != NULL, ECS_INVALID_PARAMETER, NULL);
        }
    }

error:
    return NULL;
}

static
int ensure_term_id_variable(
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_term_id_t *term_id)
{
    if (term_id->var == EcsVarIsVariable) {
        if (term_id->entity == EcsAny) {
            /* Any variables aren't translated to rule variables since their
             * result isn't stored. */
            return 0;
        }

        const char *name = term_id_var_name(term_id);

        /* If this is a Not term, it should not introduce new variables. It may
         * however create entity variables if there already was an existing
         * table variable */
        if (term->oper == EcsNot) {
            if (!find_variable(rule, EcsRuleVarKindUnknown, name)) {
                rule_error(rule, "variable in '%s' only appears in Not term",
                    name);
                return -1;
            }
        }

        ecs_rule_var_t *var = ensure_variable(rule, EcsRuleVarKindEntity, name);
        ecs_os_strset(&term_id->name, var->name);
        return 0;
    }

    return 0;
}

static
bool term_id_is_variable(
    ecs_term_id_t *term_id)
{
    return term_id->var == EcsVarIsVariable;
}

/* Get variable from a term identifier */
static
ecs_rule_var_t* term_id_to_var(
    ecs_rule_t *rule,
    ecs_term_id_t *id)
{
    if (id->var == EcsVarIsVariable) {
        return find_variable(rule, EcsRuleVarKindUnknown, term_id_var_name(id));
    }
    return NULL;
}

/* Get variable from a term predicate */
static
ecs_rule_var_t* term_pred(
    ecs_rule_t *rule,
    ecs_term_t *term)
{
    return term_id_to_var(rule, &term->pred);
}

/* Get variable from a term subject */
static
ecs_rule_var_t* term_subj(
    ecs_rule_t *rule,
    ecs_term_t *term)
{
    return term_id_to_var(rule, &term->subj);
}

/* Get variable from a term object */
static
ecs_rule_var_t* term_obj(
    ecs_rule_t *rule,
    ecs_term_t *term)
{
    if (obj_is_set(term)) {
        return term_id_to_var(rule, &term->obj);
    } else {
        return NULL;
    }
}

/* Return predicate variable from pair */
static
ecs_rule_var_t* pair_pred(
    ecs_rule_t *rule,
    const ecs_rule_pair_t *pair)
{
    if (pair->reg_mask & RULE_PAIR_PREDICATE) {
        return &rule->vars[pair->pred.reg];
    } else {
        return NULL;
    }
}

/* Return object variable from pair */
static
ecs_rule_var_t* pair_obj(
    ecs_rule_t *rule,
    const ecs_rule_pair_t *pair)
{
    if (pair->reg_mask & RULE_PAIR_OBJECT) {
        return &rule->vars[pair->obj.reg];
    } else {
        return NULL;
    }
}

/* Create new frame for storing register values. Each operation that yields data
 * gets its own register frame, which contains all variables reified up to that
 * point. The preceding frame always contains the reified variables from the
 * previous operation. Operations that do not yield data (such as control flow)
 * do not have their own frames. */
static
int32_t push_frame(
    ecs_rule_t *rule)
{
    return rule->frame_count ++;
}

/* Get register array for current stack frame. The stack frame is determined by
 * the current operation that is evaluated. The register array contains the
 * values for the reified variables. If a variable hasn't been reified yet, its
 * register will store a wildcard. */
static
ecs_var_t* get_register_frame(
    const ecs_rule_iter_t *it,
    int32_t frame)    
{
    if (it->registers) {
        return &it->registers[frame * it->rule->var_count];
    } else {
        return NULL;
    }
}

/* Get register array for current stack frame. The stack frame is determined by
 * the current operation that is evaluated. The register array contains the
 * values for the reified variables. If a variable hasn't been reified yet, its
 * register will store a wildcard. */
static
ecs_var_t* get_registers(
    const ecs_rule_iter_t *it,
    ecs_rule_op_t *op)    
{
    return get_register_frame(it, op->frame);
}

/* Get columns array. Columns store, for each matched column in a table, the 
 * index at which it occurs. This reduces the amount of searching that
 * operations need to do in a type, since select/with already provide it. */
static
int32_t* rule_get_columns_frame(
    ecs_rule_iter_t *it,
    int32_t frame)    
{
    return &it->columns[frame * it->rule->filter.term_count];
}

static
int32_t* rule_get_columns(
    ecs_rule_iter_t *it,
    ecs_rule_op_t *op)    
{
    return rule_get_columns_frame(it, op->frame);
}

static
void entity_reg_set(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r,
    ecs_entity_t entity)
{
    (void)rule;
    ecs_assert(rule->vars[r].kind == EcsRuleVarKindEntity, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(ecs_is_valid(rule->world, entity), ECS_INVALID_PARAMETER, NULL);
    regs[r].entity = entity;
error:
    return;
}

static
ecs_entity_t entity_reg_get(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r)
{
    (void)rule;
    ecs_entity_t e = regs[r].entity;
    if (!e) {
        return EcsWildcard;
    }
    
    ecs_check(ecs_is_valid(rule->world, e), ECS_INVALID_PARAMETER, NULL);   
    return e;
error:
    return 0;
}

static
void table_reg_set(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r,
    ecs_table_t *table)
{
    (void)rule;
    ecs_assert(rule->vars[r].kind == EcsRuleVarKindTable, 
        ECS_INTERNAL_ERROR, NULL);

    regs[r].range.table = table;
    regs[r].range.offset = 0;
    regs[r].range.count = 0;
    regs[r].entity = 0;
}

static 
ecs_table_range_t table_reg_get(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r)
{
    (void)rule;
    ecs_assert(rule->vars[r].kind == EcsRuleVarKindTable, 
        ECS_INTERNAL_ERROR, NULL);

    return regs[r].range;       
}

static
ecs_entity_t reg_get_entity(
    const ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_var_t *regs,
    int32_t r)
{
    if (r == UINT8_MAX) {
        ecs_assert(op->subject != 0, ECS_INTERNAL_ERROR, NULL);

        /* The subject is referenced from the query string by string identifier.
         * If subject entity is not valid, it could have been deletd by the
         * application after the rule was created */
        ecs_check(ecs_is_valid(rule->world, op->subject), 
            ECS_INVALID_PARAMETER, NULL);

        return op->subject;
    }
    if (rule->vars[r].kind == EcsRuleVarKindTable) {
        int32_t offset = regs[r].range.offset;

        ecs_assert(regs[r].range.count == 1, ECS_INTERNAL_ERROR, NULL);
        ecs_data_t *data = &table_reg_get(rule, regs, r).table->data;
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t *entities = ecs_storage_first(&data->entities);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(offset < ecs_storage_count(&data->entities), 
            ECS_INTERNAL_ERROR, NULL);
        ecs_check(ecs_is_valid(rule->world, entities[offset]), 
            ECS_INVALID_PARAMETER, NULL);            
        
        return entities[offset];
    }
    if (rule->vars[r].kind == EcsRuleVarKindEntity) {
        return entity_reg_get(rule, regs, r);
    }

    /* Must return an entity */
    ecs_assert(false, ECS_INTERNAL_ERROR, NULL);

error:
    return 0;
}

static
ecs_table_range_t table_from_entity(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    entity = ecs_get_alive(world, entity);
    
    ecs_table_range_t slice = {0};
    ecs_record_t *record = flecs_entities_get(world, entity);
    if (record) {
        slice.table = record->table;
        slice.offset = ECS_RECORD_TO_ROW(record->row);
        slice.count = 1;
    }

    return slice;
}

static
ecs_table_range_t reg_get_range(
    const ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_var_t *regs,
    int32_t r)
{
    if (r == UINT8_MAX) {
        ecs_check(ecs_is_valid(rule->world, op->subject), 
            ECS_INVALID_PARAMETER, NULL);
        return table_from_entity(rule->world, op->subject);
    }
    if (rule->vars[r].kind == EcsRuleVarKindTable) {
        return table_reg_get(rule, regs, r);
    }
    if (rule->vars[r].kind == EcsRuleVarKindEntity) {
        return table_from_entity(rule->world, entity_reg_get(rule, regs, r));
    } 
error:
    return (ecs_table_range_t){0};
}

static
void reg_set_entity(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r,
    ecs_entity_t entity)
{
    if (rule->vars[r].kind == EcsRuleVarKindTable) {
        ecs_world_t *world = rule->world;
        ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
        regs[r].range = table_from_entity(world, entity);
        regs[r].entity = entity;
    } else {
        entity_reg_set(rule, regs, r, entity);
    }
error:
    return;
}

static
void reg_set_range(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t r,
    const ecs_table_range_t *range)
{
    if (rule->vars[r].kind == EcsRuleVarKindEntity) {
        ecs_check(range->count == 1, ECS_INTERNAL_ERROR, NULL);
        regs[r].range = *range;
        regs[r].entity = ecs_storage_get_t(&range->table->data.entities,
            ecs_entity_t, range->offset)[0];
    } else {
        regs[r].range = *range;
        regs[r].entity = 0;
    }
error:
    return;
}

/* This encodes a column expression into a pair. A pair stores information about
 * the variable(s) associated with the column. Pairs are used by operations to
 * apply filters, and when there is a match, to reify variables. */
static
ecs_rule_pair_t term_to_pair(
    ecs_rule_t *rule,
    ecs_term_t *term)
{
    ecs_rule_pair_t result = {0};

    /* Terms must always have at least one argument (the subject) */
    ecs_assert(subj_is_set(term), ECS_INTERNAL_ERROR, NULL);

    /* If the predicate id is a variable, find the variable and encode its id
     * in the pair so the operation can find it later. */
    if (term->pred.var == EcsVarIsVariable) {
        if (term->pred.entity != EcsAny) {
            /* Always lookup var as an entity, as pairs never refer to tables */
            const ecs_rule_var_t *var = find_variable(
                rule, EcsRuleVarKindEntity, term_id_var_name(&term->pred));

            /* Variables should have been declared */
            ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(var->kind == EcsRuleVarKindEntity, 
                ECS_INTERNAL_ERROR, NULL);
            result.pred.reg = var->id;

            /* Set flag so the operation can see the predicate is a variable */
            result.reg_mask |= RULE_PAIR_PREDICATE;
            result.final = true;
        } else {
            result.pred.ent = EcsWildcard;
            result.final = true;
        }
    } else {
        /* If the predicate is not a variable, simply store its id. */
        ecs_entity_t pred_id = term->pred.entity;
        result.pred.ent = pred_id;

        /* Test if predicate is transitive. When evaluating the predicate, this
         * will also take into account transitive relationships */
        if (ecs_has_id(rule->world, pred_id, EcsTransitive)) {
            /* Transitive queries must have an object */
            if (obj_is_set(term)) {
                result.transitive = true;
            }
        }

        if (ecs_has_id(rule->world, pred_id, EcsFinal)) {
            result.final = true;
        }

        if (ecs_has_id(rule->world, pred_id, EcsReflexive)) {
            result.reflexive = true;
        }

        if (ecs_has_id(rule->world, pred_id, EcsAcyclic)) {
            result.acyclic = true;
        }
    }

    /* The pair doesn't do anything with the subject (subjects are the things that
     * are matched against pairs) so if the column does not have a object, 
     * there is nothing left to do. */
    if (!obj_is_set(term)) {
        return result;
    }

    /* If arguments is higher than 2 this is not a pair but a nested rule */
    ecs_assert(obj_is_set(term), ECS_INTERNAL_ERROR, NULL);

    /* Same as above, if the object is a variable, store it and flag it */
    if (term->obj.var == EcsVarIsVariable) {
        if (term->obj.entity != EcsAny) {
            const ecs_rule_var_t *var = find_variable(
                rule, EcsRuleVarKindEntity, term_id_var_name(&term->obj));

            /* Variables should have been declared */
            ecs_assert(var != NULL, ECS_INTERNAL_ERROR, 
                term_id_var_name(&term->obj));
            ecs_assert(var->kind == EcsRuleVarKindEntity, ECS_INTERNAL_ERROR, 
                term_id_var_name(&term->obj));

            result.obj.reg = var->id;
            result.reg_mask |= RULE_PAIR_OBJECT;
        } else {
            result.obj.ent = EcsWildcard;
        }
    } else {
        /* If the object is not a variable, simply store its id */
        result.obj.ent = term->obj.entity;
        if (!result.obj.ent) {
            result.obj_0 = true;
        }
    }

    return result;
}

/* When an operation has a pair, it is used to filter its input. This function
 * translates a pair back into an entity id, and in the process substitutes the
 * variables that have already been filled out. It's one of the most important
 * functions, as a lot of the filtering logic depends on having an entity that
 * has all of the reified variables correctly filled out. */
static
ecs_rule_filter_t pair_to_filter(
    ecs_rule_iter_t *it,
    ecs_rule_op_t *op,
    ecs_rule_pair_t pair)
{
    ecs_entity_t pred = pair.pred.ent;
    ecs_entity_t obj = pair.obj.ent;
    ecs_rule_filter_t result = {
        .lo_var = -1,
        .hi_var = -1
    };

    /* Get registers in case we need to resolve ids from registers. Get them
     * from the previous, not the current stack frame as the current operation
     * hasn't reified its variables yet. */
    ecs_var_t *regs = get_register_frame(it, op->frame - 1);

    if (pair.reg_mask & RULE_PAIR_OBJECT) {
        obj = entity_reg_get(it->rule, regs, pair.obj.reg);
        obj = ecs_entity_t_lo(obj); /* Filters don't have generations */

        if (obj == EcsWildcard) {
            result.wildcard = true;
            result.obj_wildcard = true;
            result.lo_var = pair.obj.reg;
        }
    }

    if (pair.reg_mask & RULE_PAIR_PREDICATE) {
        pred = entity_reg_get(it->rule, regs, pair.pred.reg);
        pred = ecs_entity_t_lo(pred); /* Filters don't have generations */

        if (pred == EcsWildcard) {
            if (result.wildcard) {
                result.same_var = pair.pred.reg == pair.obj.reg;
            }

            result.wildcard = true;
            result.pred_wildcard = true;

            if (obj) {
                result.hi_var = pair.pred.reg;
            } else {
                result.lo_var = pair.pred.reg;
            }
        }
    }

    if (!obj && !pair.obj_0) {
        result.mask = pred;
    } else {
        result.mask = ecs_pair(pred, obj);
    }

    return result;
}

/* This function is responsible for reifying the variables (filling them out 
 * with their actual values as soon as they are known). It uses the pair 
 * expression returned by pair_get_most_specific_var, and attempts to fill out each of the
 * wildcards in the pair. If a variable isn't reified yet, the pair expression
 * will still contain one or more wildcards, which is harmless as the respective
 * registers will also point to a wildcard. */
static
void reify_variables(
    ecs_rule_iter_t *it,
    ecs_rule_op_t *op,
    ecs_rule_filter_t *filter,
    ecs_type_t type,
    int32_t column)
{
    const ecs_rule_t *rule = it->rule;
    const ecs_rule_var_t *vars = rule->vars;
    (void)vars;

    ecs_var_t *regs = get_registers(it, op);
    ecs_assert(column < type.count, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t *elem = &type.array[column];

    int32_t obj_var = filter->lo_var;
    int32_t pred_var = filter->hi_var;

    if (obj_var != -1) {
        ecs_assert(vars[obj_var].kind == EcsRuleVarKindEntity, 
            ECS_INTERNAL_ERROR, NULL);

        entity_reg_set(rule, regs, obj_var, 
            ecs_get_alive(rule->world, ECS_PAIR_SECOND(*elem)));
    }

    if (pred_var != -1) {
        ecs_assert(vars[pred_var].kind == EcsRuleVarKindEntity, 
            ECS_INTERNAL_ERROR, NULL);            

        entity_reg_set(rule, regs, pred_var, 
            ecs_get_alive(rule->world, 
                ECS_PAIR_FIRST(*elem)));
    }
}

/* Returns whether variable is a subject */
static
bool is_subject(
    ecs_rule_t *rule,
    ecs_rule_var_t *var)
{
    ecs_assert(rule != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!var) {
        return false;
    }

    if (var->id < rule->subj_var_count) {
        return true;
    }

    return false;
}

static
bool skip_term(ecs_term_t *term) {
    if (term->subj.set.mask & EcsNothing) {
        return true;
    }
    return false;
}

static
int32_t get_variable_depth(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    ecs_rule_var_t *root,
    int recur);

static
int32_t crawl_variable(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    ecs_rule_var_t *root,
    int recur)    
{
    ecs_term_t *terms = rule->filter.terms;
    int32_t i, count = rule->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (skip_term(term)) {
            continue;
        }
        
        ecs_rule_var_t 
        *pred = term_pred(rule, term),
        *subj = term_subj(rule, term),
        *obj = term_obj(rule, term);

        /* Variable must at least appear once in term */
        if (var != pred && var != subj && var != obj) {
            continue;
        }

        if (pred && pred != var && !pred->marked) {
            get_variable_depth(rule, pred, root, recur + 1);
        }

        if (subj && subj != var && !subj->marked) {
            get_variable_depth(rule, subj, root, recur + 1);
        }

        if (obj && obj != var && !obj->marked) {
            get_variable_depth(rule, obj, root, recur + 1);
        }
    }

    return 0;
}

static
int32_t get_depth_from_var(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    ecs_rule_var_t *root,
    int recur)
{
    /* If variable is the root or if depth has been set, return depth + 1 */
    if (var == root || var->depth != UINT8_MAX) {
        return var->depth + 1;
    }

    /* Variable is already being evaluated, so this indicates a cycle. Stop */
    if (var->marked) {
        return 0;
    }
    
    /* Variable is not yet being evaluated and depth has not yet been set. 
     * Calculate depth. */
    int32_t depth = get_variable_depth(rule, var, root, recur + 1);
    if (depth == UINT8_MAX) {
        return depth;
    } else {
        return depth + 1;
    }
}

static
int32_t get_depth_from_term(
    ecs_rule_t *rule,
    ecs_rule_var_t *cur,
    ecs_rule_var_t *pred,
    ecs_rule_var_t *obj,
    ecs_rule_var_t *root,
    int recur)
{
    int32_t result = UINT8_MAX;

    /* If neither of the other parts of the terms are variables, this
     * variable is guaranteed to have no dependencies. */
    if (!pred && !obj) {
        result = 0;
    } else {
        /* If this is a variable that is not the same as the current, 
         * we can use it to determine dependency depth. */
        if (pred && cur != pred) {
            int32_t depth = get_depth_from_var(rule, pred, root, recur);
            if (depth == UINT8_MAX) {
                return UINT8_MAX;
            }

            /* If the found depth is lower than the depth found, overwrite it */
            if (depth < result) {
                result = depth;
            }
        }

        /* Same for obj */
        if (obj && cur != obj) {
            int32_t depth = get_depth_from_var(rule, obj, root, recur);
            if (depth == UINT8_MAX) {
                return UINT8_MAX;
            }

            if (depth < result) {
                result = depth;
            }
        }
    }

    return result;
}

/* Find the depth of the dependency tree from the variable to the root */
static
int32_t get_variable_depth(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    ecs_rule_var_t *root,
    int recur)
{
    var->marked = true;

    /* Iterate columns, find all instances where 'var' is not used as subject.
     * If the subject of that column is either the root or a variable for which
     * the depth is known, the depth for this variable can be determined. */
    ecs_term_t *terms = rule->filter.terms;

    int32_t i, count = rule->filter.term_count;
    int32_t result = UINT8_MAX;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (skip_term(term)) {
            continue;
        }

        ecs_rule_var_t 
        *pred = term_pred(rule, term),
        *subj = term_subj(rule, term),
        *obj = term_obj(rule, term);

        if (subj != var) {
            continue;
        }

        if (!is_subject(rule, pred)) {
            pred = NULL;
        }

        if (!is_subject(rule, obj)) {
            obj = NULL;
        }

        int32_t depth = get_depth_from_term(rule, var, pred, obj, root, recur);
        if (depth < result) {
            result = depth;
        }
    }

    if (result == UINT8_MAX) {
        result = 0;
    }

    var->depth = result;    

    /* Dependencies are calculated from subject to (pred, obj). If there were
     * subjects that are only related by object (like (X, Y), (Z, Y)) it is
     * possible that those have not yet been found yet. To make sure those 
     * variables are found, loop again & follow predicate & object links */
    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (skip_term(term)) {
            continue;
        }

        ecs_rule_var_t 
        *subj = term_subj(rule, term),
        *pred = term_pred(rule, term),
        *obj = term_obj(rule, term);

        /* Only evaluate pred & obj for current subject. This ensures that we
         * won't evaluate variables that are unreachable from the root. This
         * must be detected as unconstrained variables are not allowed. */
        if (subj != var) {
            continue;
        }

        crawl_variable(rule, subj, root, recur);

        if (pred && pred != var) {
            crawl_variable(rule, pred, root, recur);
        }

        if (obj && obj != var) {
            crawl_variable(rule, obj, root, recur);
        }        
    }

    return var->depth;
}

/* Compare function used for qsort. It ensures that variables are first ordered
 * by depth, followed by how often they occur. */
static
int compare_variable(
    const void* ptr1, 
    const void *ptr2)
{
    const ecs_rule_var_t *v1 = ptr1;
    const ecs_rule_var_t *v2 = ptr2;

    if (v1->kind < v2->kind) {
        return -1;
    } else if (v1->kind > v2->kind) {
        return 1;
    }

    if (v1->depth < v2->depth) {
        return -1;
    } else if (v1->depth > v2->depth) {
        return 1;
    }

    if (v1->occurs < v2->occurs) {
        return 1;
    } else {
        return -1;
    }

    return (v1->id < v2->id) - (v1->id > v2->id);
}

/* After all subject variables have been found, inserted and sorted, the 
 * remaining variables (predicate & object) still need to be inserted. This
 * function serves two purposes. The first purpose is to ensure that all 
 * variables are known before operations are emitted. This ensures that the
 * variables array won't be reallocated while emitting, which simplifies code.
 * The second purpose of the function is to ensure that if the root variable
 * (which, if it exists has now been created with a table type) is also inserted
 * with an entity type if required. This is used later to decide whether the
 * rule needs to insert an each instruction. */
static
int ensure_all_variables(
    ecs_rule_t *rule)
{
    ecs_term_t *terms = rule->filter.terms;
    int32_t i, count = rule->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (skip_term(term)) {
            continue;
        }

        /* If predicate is a variable, make sure it has been registered */
        if (term->pred.var == EcsVarIsVariable) {
            if (ensure_term_id_variable(rule, term, &term->pred) != 0) {
                return -1;
            }
        }

        /* If subject is a variable and it is not This, make sure it is 
         * registered as an entity variable. This ensures that the program will
         * correctly return all permutations */
        if (term->subj.var == EcsVarIsVariable) {
            if (term->subj.entity != EcsThis) {
                if (ensure_term_id_variable(rule, term, &term->subj) != 0) {
                    return -1;
                }
            }
        }

        /* If object is a variable, make sure it has been registered */
        if (obj_is_set(term) && (term->obj.var == EcsVarIsVariable)) {
            if (ensure_term_id_variable(rule, term, &term->obj) != 0) {
                return -1;
            }
        }
    }

    return 0;   
}

/* Scan for variables, put them in optimal dependency order. */
static
int scan_variables(
    ecs_rule_t *rule)
{
    /* Objects found in rule. One will be elected root */
    int32_t subject_count = 0;

    /* If this (.) is found, it always takes precedence in root election */
    int32_t this_var = UINT8_MAX;

    /* Keep track of the subject variable that occurs the most. In the absence of
     * this (.) the variable with the most occurrences will be elected root. */
    int32_t max_occur = 0;
    int32_t max_occur_var = UINT8_MAX;

    /* Step 1: find all possible roots */
    ecs_term_t *terms = rule->filter.terms;
    int32_t i, term_count = rule->filter.term_count;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        /* Evaluate the subject. The predicate and object are not evaluated, 
         * since they never can be elected as root. */
        if (term_id_is_variable(&term->subj)) {
            const char *subj_name = term_id_var_name(&term->subj);
            
            ecs_rule_var_t *subj = find_variable(
                rule, EcsRuleVarKindTable, subj_name);
            if (!subj) {
                subj = create_variable(rule, EcsRuleVarKindTable, subj_name);
                if (subject_count >= ECS_RULE_MAX_VAR_COUNT) {
                    rule_error(rule, "too many variables in rule");
                    goto error;
                }

                /* Make sure that variable name in term array matches with the
                 * rule name. */
                ecs_os_strset(&term->subj.name, subj->name);
            }

            if (++ subj->occurs > max_occur) {
                max_occur = subj->occurs;
                max_occur_var = subj->id;
            }
        }
    }

    rule->subj_var_count = rule->var_count;

    if (ensure_all_variables(rule) != 0) {
        goto error;
    }

    /* Variables in a term with a literal subject have depth 0 */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (term->subj.var == EcsVarIsEntity) {
            ecs_rule_var_t 
            *pred = term_pred(rule, term),
            *obj = term_obj(rule, term);

            if (pred) {
                pred->depth = 0;
            }
            if (obj) {
                obj->depth = 0;
            }
        }
    }

    /* Elect a root. This is either this (.) or the variable with the most
     * occurrences. */
    int32_t root_var = this_var;
    if (root_var == UINT8_MAX) {
        root_var = max_occur_var;
        if (root_var == UINT8_MAX) {
            /* If no subject variables have been found, the rule expression only
             * operates on a fixed set of entities, in which case no root 
             * election is required. */
            goto done;
        }
    }

    ecs_rule_var_t *root = &rule->vars[root_var];
    root->depth = get_variable_depth(rule, root, root, 0);

    /* Verify that there are no unconstrained variables. Unconstrained variables
     * are variables that are unreachable from the root. */
    for (i = 0; i < rule->subj_var_count; i ++) {
        if (rule->vars[i].depth == UINT8_MAX) {
            rule_error(rule, "unconstrained variable '%s'", 
                rule->vars[i].name);
            goto error;
        } 
    }

    /* For each Not term, verify that variables are known */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->oper != EcsNot) {
            continue;
        }

        ecs_rule_var_t 
        *pred = term_pred(rule, term),
        *obj = term_obj(rule, term);

        if (!pred && term_id_is_variable(&term->pred) && 
            term->pred.entity != EcsAny)
        {
            rule_error(rule, "missing predicate variable '%s'", 
                term_id_var_name(&term->pred));
            goto error;
        }
        if (!obj && term_id_is_variable(&term->obj) && 
            term->obj.entity != EcsAny)
        {
            rule_error(rule, "missing object variable '%s'", 
                term_id_var_name(&term->obj));
            goto error;
        }
    }

    /* Order variables by depth, followed by occurrence. The variable
     * array will later be used to lead the iteration over the terms, and
     * determine which operations get inserted first. */
    int32_t var_count = rule->var_count;
    ecs_rule_var_t vars[ECS_RULE_MAX_VAR_COUNT];
    ecs_os_memcpy_n(vars, rule->vars, ecs_rule_var_t, var_count);
    ecs_qsort_t(&vars, var_count, ecs_rule_var_t, compare_variable);
    for (i = 0; i < var_count; i ++) {
        rule->var_eval_order[i] = vars[i].id;
    }

done:
    return 0;
error:
    return -1;
}

/* Get entity variable from table variable */
static
ecs_rule_var_t* to_entity(
    ecs_rule_t *rule,
    ecs_rule_var_t *var)
{
    if (!var) {
        return NULL;
    }

    ecs_rule_var_t *evar = NULL;
    if (var->kind == EcsRuleVarKindTable) {
        evar = find_variable(rule, EcsRuleVarKindEntity, var->name);
    } else {
        evar = var;
    }

    return evar;
}

/* Ensure that if a table variable has been written, the corresponding entity
 * variable is populated. The function will return the most specific, populated
 * variable. */
static
ecs_rule_var_t* most_specific_var(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    bool *written,
    bool create)
{
    if (!var) {
        return NULL;
    }

    ecs_rule_var_t *tvar, *evar = to_entity(rule, var);
    if (!evar) {
        return var;
    }

    if (var->kind == EcsRuleVarKindTable) {
        tvar = var;
    } else {
        tvar = find_variable(rule, EcsRuleVarKindTable, var->name);
    }

    /* If variable is used as predicate or object, it should have been 
     * registered as an entity. */
    ecs_assert(evar != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Usually table variables are resolved before they are used as a predicate
     * or object, but in the case of cyclic dependencies this is not guaranteed.
     * Only insert an each instruction of the table variable has been written */
    if (tvar && written[tvar->id]) {
        /* If the variable has been written as a table but not yet
         * as an entity, insert an each operation that yields each
         * entity in the table. */
        if (evar) {
            if (written[evar->id]) {
                return evar;
            } else if (create) {
                ecs_rule_op_t *op = create_operation(rule);
                op->kind = EcsRuleEach;
                op->on_pass = rule->operation_count;
                op->on_fail = rule->operation_count - 2;
                op->frame = rule->frame_count;
                op->has_in = true;
                op->has_out = true;
                op->r_in = tvar->id;
                op->r_out = evar->id;

                /* Entity will either be written or has been written */
                written[evar->id] = true;

                push_frame(rule);

                return evar;
            } else {
                return tvar;
            }
        }
    } else if (evar && written[evar->id]) {
        return evar;
    }

    return var;
}

/* Get most specific known variable */
static
ecs_rule_var_t *get_most_specific_var(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    bool *written)
{
    return most_specific_var(rule, var, written, false);
}

/* Get or create most specific known variable. This will populate an entity
 * variable if a table variable is known but the entity variable isn't. */
static
ecs_rule_var_t *ensure_most_specific_var(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    bool *written)
{
    return most_specific_var(rule, var, written, true);
}


/* Ensure that an entity variable is written before using it */
static
ecs_rule_var_t* ensure_entity_written(
    ecs_rule_t *rule,
    ecs_rule_var_t *var,
    bool *written)
{
    if (!var) {
        return NULL;
    }

    /* Ensure we're working with the most specific version of subj we can get */
    ecs_rule_var_t *evar = ensure_most_specific_var(rule, var, written);

    /* The post condition of this function is that there is an entity variable,
     * and that it is written. Make sure that the result is an entity */
    ecs_assert(evar != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(evar->kind == EcsRuleVarKindEntity, ECS_INTERNAL_ERROR, NULL);

    /* Make sure the variable has been written */
    ecs_assert(written[evar->id] == true, ECS_INTERNAL_ERROR, NULL);
    
    return evar;
}

static
ecs_rule_op_t* insert_operation(
    ecs_rule_t *rule,
    int32_t term_index,
    bool *written)
{
    ecs_rule_pair_t pair = {0};

    /* Parse the term's type into a pair. A pair extracts the ids from
     * the term, and replaces variables with wildcards which can then
     * be matched against actual relationships. A pair retains the 
     * information about the variables, so that when a match happens,
     * the pair can be used to reify the variable. */
    if (term_index != -1) {
        ecs_term_t *term = &rule->filter.terms[term_index];

        pair = term_to_pair(rule, term);

        /* If the pair contains entity variables that have not yet been written,
         * insert each instructions in case their tables are known. Variables in
         * a pair that are truly unknown will be populated by the operation,
         * but an operation should never overwrite an entity variable if the 
         * corresponding table variable has already been resolved. */
        if (pair.reg_mask & RULE_PAIR_PREDICATE) {
            ecs_rule_var_t *pred = &rule->vars[pair.pred.reg];
            pred = get_most_specific_var(rule, pred, written);
            pair.pred.reg = pred->id;
        }

        if (pair.reg_mask & RULE_PAIR_OBJECT) {
            ecs_rule_var_t *obj = &rule->vars[pair.obj.reg];
            obj = get_most_specific_var(rule, obj, written);
            pair.obj.reg = obj->id;
        }
    } else {
        /* Not all operations have a filter (like Each) */
    }

    ecs_rule_op_t *op = create_operation(rule);
    op->on_pass = rule->operation_count;
    op->on_fail = rule->operation_count - 2;
    op->frame = rule->frame_count;
    op->filter = pair;

    /* Store corresponding signature term so we can correlate and
     * store the table columns with signature columns. */
    op->term = term_index;

    return op;
}

/* Insert first operation, which is always Input. This creates an entry in
 * the register stack for the initial state. */
static
void insert_input(
    ecs_rule_t *rule)
{
    ecs_rule_op_t *op = create_operation(rule);
    op->kind = EcsRuleInput;

    /* The first time Input is evaluated it goes to the next/first operation */
    op->on_pass = 1;

    /* When Input is evaluated with redo = true it will return false, which will
     * finish the program as op becomes -1. */
    op->on_fail = -1;  

    push_frame(rule);
}

/* Insert last operation, which is always Yield. When the program hits Yield,
 * data is returned to the application. */
static
void insert_yield(
    ecs_rule_t *rule)
{
    ecs_rule_op_t *op = create_operation(rule);
    op->kind = EcsRuleYield;
    op->has_in = true;
    op->on_fail = rule->operation_count - 2;
    /* Yield can only "fail" since it is the end of the program */

    /* Find variable associated with this. It is possible that the variable
     * exists both as a table and as an entity. This can happen when a rule
     * first selects a table for this, but then subsequently needs to evaluate
     * each entity in that table. In that case the yield instruction should
     * return the entity, so look for that first. */
    ecs_rule_var_t *var = find_variable(rule, EcsRuleVarKindEntity, ".");
    if (!var) {
        var = find_variable(rule, EcsRuleVarKindTable, ".");
    }

    /* If there is no this, there is nothing to yield. In that case the rule
     * simply returns true or false. */
    if (!var) {
        op->r_in = UINT8_MAX;
    } else {
        op->r_in = var->id;
    }

    op->frame = push_frame(rule);
}

/* Return superset/subset including the root */
static
void insert_reflexive_set(
    ecs_rule_t *rule,
    ecs_rule_op_kind_t op_kind,
    ecs_rule_var_t *out,
    const ecs_rule_pair_t pair,
    int32_t c,
    bool *written,
    bool reflexive)
{
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_rule_var_t *pred = pair_pred(rule, &pair);
    ecs_rule_var_t *obj = pair_obj(rule, &pair);

    int32_t setjmp_lbl = rule->operation_count;
    int32_t store_lbl = setjmp_lbl + 1;
    int32_t set_lbl = setjmp_lbl + 2;
    int32_t next_op = setjmp_lbl + 4;
    int32_t prev_op = setjmp_lbl - 1;

    /* Insert 4 operations at once, so we don't have to worry about how
     * the instruction array reallocs. If operation is not reflexive, we only
     * need to insert the set operation. */
    if (reflexive) {
        insert_operation(rule, -1, written);
        insert_operation(rule, -1, written);
        insert_operation(rule, -1, written);
    }

    ecs_rule_op_t *op = insert_operation(rule, -1, written);
    ecs_rule_op_t *setjmp = &rule->operations[setjmp_lbl];
    ecs_rule_op_t *store = &rule->operations[store_lbl];
    ecs_rule_op_t *set = &rule->operations[set_lbl];
    ecs_rule_op_t *jump = op;

    if (!reflexive) {
        set_lbl = setjmp_lbl;
        set = op;
        setjmp = NULL;
        store = NULL;
        jump = NULL;
        next_op = set_lbl + 1;
        prev_op = set_lbl - 1;
    }

    /* The SetJmp operation stores a conditional jump label that either
     * points to the Store or *Set operation */
    if (reflexive) {
        setjmp->kind = EcsRuleSetJmp;
        setjmp->on_pass = store_lbl;
        setjmp->on_fail = set_lbl;
    }

    /* The Store operation yields the root of the subtree. After yielding,
     * this operation will fail and return to SetJmp, which will cause it
     * to switch to the *Set operation. */
    if (reflexive) {
        store->kind = EcsRuleStore;
        store->on_pass = next_op;
        store->on_fail = setjmp_lbl;
        store->has_in = true;
        store->has_out = true;
        store->r_out = out->id;
        store->term = c;

        if (!pred) {
            store->filter.pred = pair.pred;
        } else {
            store->filter.pred.reg = pred->id;
            store->filter.reg_mask |= RULE_PAIR_PREDICATE;
        }

        /* If the object of the filter is not a variable, store literal */
        if (!obj) {
            store->r_in = UINT8_MAX;
            store->subject = ecs_get_alive(rule->world, pair.obj.ent);
            store->filter.obj = pair.obj;
        } else {
            store->r_in = obj->id;
            store->filter.obj.reg = obj->id;
            store->filter.reg_mask |= RULE_PAIR_OBJECT;
        }
    }

    /* This is either a SubSet or SuperSet operation */
    set->kind = op_kind;
    set->on_pass = next_op;
    set->on_fail = prev_op;
    set->has_out = true;
    set->r_out = out->id;
    set->term = c;

    /* Predicate can be a variable if it's non-final */
    if (!pred) {
        set->filter.pred = pair.pred;
    } else {
        set->filter.pred.reg = pred->id;
        set->filter.reg_mask |= RULE_PAIR_PREDICATE;
    }

    if (!obj) {
        set->filter.obj = pair.obj;
    } else {
        set->filter.obj.reg = obj->id;
        set->filter.reg_mask |= RULE_PAIR_OBJECT;
    }

    if (reflexive) {
        /* The jump operation jumps to either the store or subset operation,
        * depending on whether the store operation already yielded. The 
        * operation is inserted last, so that the on_fail label of the next 
        * operation will point to it */
        jump->kind = EcsRuleJump;
        
        /* The pass/fail labels of the Jump operation are not used, since it
        * jumps to a variable location. Instead, the pass label is (ab)used to
        * store the label of the SetJmp operation, so that the jump can access
        * the label it needs to jump to from the setjmp op_ctx. */
        jump->on_pass = setjmp_lbl;
        jump->on_fail = -1;
    }

    written[out->id] = true;
}

static
ecs_rule_var_t* store_reflexive_set(
    ecs_rule_t *rule,
    ecs_rule_op_kind_t op_kind,
    ecs_rule_pair_t *pair,
    bool *written,
    bool reflexive,
    bool as_entity)
{
    /* Ensure we're using the most specific version of obj */
    ecs_rule_var_t *obj = pair_obj(rule, pair);
    if (obj) {
        pair->obj.reg = obj->id;
    }

    /* The subset operation returns tables */
    ecs_rule_var_kind_t var_kind = EcsRuleVarKindTable;
    if (op_kind == EcsSuperSet) {
        var_kind = EcsRuleVarKindEntity;
    }

    /* Create anonymous variable for storing the set */
    ecs_rule_var_t *av = create_anonymous_variable(rule, var_kind);
    int32_t ave_id = 0, av_id = av->id;

    /* If the variable kind is a table, also create an entity variable as the
     * result of the set operation should be returned as an entity */
    if (var_kind == EcsRuleVarKindTable && as_entity) {
        create_variable(rule, EcsRuleVarKindEntity, av->name);
        av = &rule->vars[av_id];
        ave_id = av_id + 1;
    }

    /* Generate the operations */
    insert_reflexive_set(rule, op_kind, av, *pair, -1, written, reflexive);

    /* Make sure to return entity variable, and that it is populated */
    if (as_entity) {
        return ensure_entity_written(rule, &rule->vars[ave_id], written);
    } else {
        return &rule->vars[av_id];
    }
}

static
bool is_known(
    ecs_rule_var_t *var,
    bool *written)
{
    if (!var) {
        return true;
    } else {
        return written[var->id];
    }
}

static
bool is_pair_known(
    ecs_rule_t *rule,
    ecs_rule_pair_t *pair,
    bool *written)
{
    ecs_rule_var_t *pred_var = pair_pred(rule, pair);
    if (!is_known(pred_var, written) || pair->pred.ent == EcsWildcard) {
        return false;
    }

    ecs_rule_var_t *obj_var = pair_obj(rule, pair);
    if (!is_known(obj_var, written) || pair->obj.ent == EcsWildcard) {
        return false;
    }

    return true;
}

static
void set_input_to_subj(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_term_t *term,
    ecs_rule_var_t *var)
{
    (void)rule;
    
    op->has_in = true;
    if (!var) {
        op->r_in = UINT8_MAX;
        op->subject = term->subj.entity;

        /* Invalid entities should have been caught during parsing */
        ecs_assert(ecs_is_valid(rule->world, op->subject), 
            ECS_INTERNAL_ERROR, NULL);
    } else {
        op->r_in = var->id;
    }
}

static
void set_output_to_subj(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_term_t *term,
    ecs_rule_var_t *var)
{
    (void)rule;

    op->has_out = true;
    if (!var) {
        op->r_out = UINT8_MAX;
        op->subject = term->subj.entity;

        /* Invalid entities should have been caught during parsing */
        ecs_assert(ecs_is_valid(rule->world, op->subject), 
            ECS_INTERNAL_ERROR, NULL);
    } else {
        op->r_out = var->id;
    }
}

static
void insert_select_or_with(
    ecs_rule_t *rule,
    int32_t c,
    ecs_term_t *term,
    ecs_rule_var_t *subj,
    ecs_rule_pair_t *pair,
    bool *written)
{
    ecs_rule_op_t *op;
    bool eval_subject_supersets = false;

    /* Find any entity and/or table variables for subject */
    ecs_rule_var_t *tvar = NULL, *evar = to_entity(rule, subj), *var = evar;
    if (subj && subj->kind == EcsRuleVarKindTable) {
        tvar = subj;
        if (!evar) {
            var = tvar;
        }
    }

    int32_t lbl_start = rule->operation_count;
    ecs_rule_pair_t filter;
    if (pair) {
        filter = *pair;
    } else {
        filter = term_to_pair(rule, term);
    }

    /* Only insert implicit IsA if filter isn't already an IsA */
    if ((!filter.transitive || filter.pred.ent != EcsIsA) && term->oper != EcsNot) {
        if (!var) {
            ecs_rule_pair_t isa_pair = {
                .pred.ent = EcsIsA,
                .obj.ent = term->subj.entity
            };

            evar = subj = store_reflexive_set(rule, EcsRuleSuperSet, &isa_pair, 
                written, true, true);
            tvar = NULL;
            eval_subject_supersets = true;

        } else if (ecs_id_is_wildcard(term->id)) {
            ecs_assert(subj != NULL, ECS_INTERNAL_ERROR, NULL);

            op = insert_operation(rule, -1, written);

            if (!is_known(subj, written)) {
                op->kind = EcsRuleSelect;
                set_output_to_subj(rule, op, term, subj);
                written[subj->id] = true;
            } else {
                op->kind = EcsRuleWith;
                set_input_to_subj(rule, op, term, subj);
            }

            ecs_rule_pair_t isa_pair = {
                .pred.ent = EcsIsA,
                .obj.reg = subj->id,
                .reg_mask = RULE_PAIR_OBJECT
            };

            op->filter = filter;
            if (op->filter.reg_mask & RULE_PAIR_PREDICATE) {
                op->filter.pred.ent = EcsWildcard;
            }
            if (op->filter.reg_mask & RULE_PAIR_OBJECT) {
                op->filter.obj.ent = EcsWildcard;
            }
            op->filter.reg_mask = 0;

            push_frame(rule);

            tvar = subj = store_reflexive_set(rule, EcsRuleSuperSet, &isa_pair, 
                written, true, false);

            evar = NULL;
        }
    }

    /* If no pair is provided, create operation from specified term */
    if (!pair) {
        op = insert_operation(rule, c, written);

    /* If an explicit pair is provided, override the default one from the 
     * term. This allows for using a predicate or object variable different
     * from what is in the term. One application of this is to substitute a
     * predicate with its subsets, if it is non final */
    } else {
        op = insert_operation(rule, -1, written);
        op->filter = *pair;

        /* Assign the term id, so that the operation will still be correctly
         * associated with the correct expression term. */
        op->term = c;
    }

    /* If entity variable is known and resolved, create with for it */
    if (evar && is_known(evar, written)) {
        op->kind = EcsRuleWith;
        op->r_in = evar->id;
        set_input_to_subj(rule, op, term, subj);

    /* If table variable is known and resolved, create with for it */
    } else if (tvar && is_known(tvar, written)) {
        op->kind = EcsRuleWith;
        op->r_in = tvar->id;
        set_input_to_subj(rule, op, term, subj);

    /* If subject is neither table nor entitiy, with operates on literal */        
    } else if (!tvar && !evar) {
        op->kind = EcsRuleWith;
        set_input_to_subj(rule, op, term, subj);

    /* If subject is table or entity but not known, use select */
    } else {
        ecs_assert(subj != NULL, ECS_INTERNAL_ERROR, NULL);
        op->kind = EcsRuleSelect;
        set_output_to_subj(rule, op, term, subj);
        written[subj->id] = true;
    }

    /* If supersets of subject are being evaluated, and we're looking for a
     * specific filter, stop as soon as the filter has been matched. */
    if (eval_subject_supersets && is_pair_known(rule, &op->filter, written)) {
        op = insert_operation(rule, -1, written);

        /* When the next operation returns, it will first hit SetJmp with a redo
         * which will switch the jump label to the previous operation */
        op->kind = EcsRuleSetJmp;
        op->on_pass = rule->operation_count;
        op->on_fail = lbl_start - 1;
    }

    if (op->filter.reg_mask & RULE_PAIR_PREDICATE) {
        written[op->filter.pred.reg] = true;
    }

    if (op->filter.reg_mask & RULE_PAIR_OBJECT) {
        written[op->filter.obj.reg] = true;
    }
}

static
void prepare_predicate(
    ecs_rule_t *rule,
    ecs_rule_pair_t *pair,
    int32_t term,
    bool *written)  
{
    /* If pair is not final, resolve term for all IsA relationships of the
     * predicate. Note that if the pair has final set to true, it is guaranteed
     * that the predicate can be used in an IsA query */
    if (!pair->final) {
        ecs_rule_pair_t isa_pair = {
            .pred.ent = EcsIsA,
            .obj.ent = pair->pred.ent
        };

        ecs_rule_var_t *pred = store_reflexive_set(rule, EcsRuleSubSet, 
            &isa_pair, written, true, true);

        pair->pred.reg = pred->id;
        pair->reg_mask |= RULE_PAIR_PREDICATE;

        if (term != -1) {
            rule->term_vars[term].pred = pred->id;
        }
    }
}

static
void insert_term_2(
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_pair_t *filter,
    int32_t c,
    bool *written)
{
    int32_t subj_id = -1, obj_id = -1;
    ecs_rule_var_t *subj = term_subj(rule, term);
    if ((subj = get_most_specific_var(rule, subj, written))) {
        subj_id = subj->id;
    }

    ecs_rule_var_t *obj = term_obj(rule, term);
    if ((obj = get_most_specific_var(rule, obj, written))) {
        obj_id = obj->id;
    }

    bool subj_known = is_known(subj, written);
    bool same_obj_subj = false;
    if (subj && obj) {
        same_obj_subj = !ecs_os_strcmp(subj->name, obj->name);
    }

    if (!filter->transitive) {
        insert_select_or_with(rule, c, term, subj, filter, written);
        if (subj) subj = &rule->vars[subj_id];
        if (obj) obj = &rule->vars[obj_id];

    } else if (filter->transitive) {
        if (subj_known) {
            if (is_known(obj, written)) {
                if (filter->obj.ent != EcsWildcard) {
                    ecs_rule_var_t *obj_subsets = store_reflexive_set(
                        rule, EcsRuleSubSet, filter, written, true, true);

                    if (subj) {
                        subj = &rule->vars[subj_id];
                    }

                    rule->term_vars[c].obj = obj_subsets->id;

                    ecs_rule_pair_t pair = *filter;
                    pair.obj.reg = obj_subsets->id;
                    pair.reg_mask |= RULE_PAIR_OBJECT;

                    insert_select_or_with(rule, c, term, subj, &pair, written);
                } else {
                    insert_select_or_with(rule, c, term, subj, filter, written);
                }
            } else {
                ecs_assert(obj != NULL, ECS_INTERNAL_ERROR, NULL);

                /* If subject is literal, find supersets for subject */
                if (subj == NULL || subj->kind == EcsRuleVarKindEntity) {
                    obj = to_entity(rule, obj);

                    ecs_rule_pair_t set_pair = *filter;
                    set_pair.reg_mask &= RULE_PAIR_PREDICATE;

                    if (subj) {
                        set_pair.obj.reg = subj->id;
                        set_pair.reg_mask |= RULE_PAIR_OBJECT;
                    } else {
                        set_pair.obj.ent = term->subj.entity;
                    }

                    insert_reflexive_set(rule, EcsRuleSuperSet, obj, set_pair, 
                        c, written, filter->reflexive);

                /* If subject is variable, first find matching pair for the 
                 * evaluated entity(s) and return supersets */
                } else {
                    ecs_rule_var_t *av = create_anonymous_variable(
                        rule, EcsRuleVarKindEntity);

                    subj = &rule->vars[subj_id];
                    obj = &rule->vars[obj_id];
                    obj = to_entity(rule, obj);

                    ecs_rule_pair_t set_pair = *filter;
                    set_pair.obj.reg = av->id;
                    set_pair.reg_mask |= RULE_PAIR_OBJECT;

                    /* Insert with to find initial object for relation */
                    insert_select_or_with(
                        rule, c, term, subj, &set_pair, written);

                    push_frame(rule);

                    /* Find supersets for returned initial object. Make sure
                     * this is always reflexive since it needs to return the
                     * object from the pair that the entity has itself. */
                    insert_reflexive_set(rule, EcsRuleSuperSet, obj, set_pair, 
                        c, written, true);
                }
            }

        /* subj is not known */
        } else {
            ecs_assert(subj != NULL, ECS_INTERNAL_ERROR, NULL);

            if (is_known(obj, written)) {
                ecs_rule_pair_t set_pair = *filter;
                set_pair.reg_mask &= RULE_PAIR_PREDICATE; /* clear object mask */

                if (obj) {
                    set_pair.obj.reg = obj->id;
                    set_pair.reg_mask |= RULE_PAIR_OBJECT;
                } else {
                    set_pair.obj.ent = term->obj.entity;
                }

                if (obj) {
                    rule->term_vars[c].obj = obj->id;
                } else {
                    ecs_rule_var_t *av = create_anonymous_variable(rule,
                        EcsRuleVarKindEntity);
                    rule->term_vars[c].obj = av->id;
                    written[av->id] = true;
                }

                insert_reflexive_set(rule, EcsRuleSubSet, subj, set_pair, c, 
                    written, filter->reflexive);
            } else if (subj == obj) {
                insert_select_or_with(rule, c, term, subj, filter, written);
            } else {
                ecs_assert(obj != NULL, ECS_INTERNAL_ERROR, NULL);

                ecs_rule_var_t *av = NULL;
                if (!filter->reflexive) {
                    av = create_anonymous_variable(rule, EcsRuleVarKindEntity);
                }

                subj = &rule->vars[subj_id];
                obj = &rule->vars[obj_id];
                obj = to_entity(rule, obj);

                /* Insert instruction to find all subjects and objects */
                ecs_rule_op_t *op = insert_operation(rule, -1, written);
                op->kind = EcsRuleSelect;
                set_output_to_subj(rule, op, term, subj);
                op->filter.pred = filter->pred;

                if (filter->reflexive) {
                    op->filter.obj.ent = EcsWildcard;
                    op->filter.reg_mask = filter->reg_mask & RULE_PAIR_PREDICATE;
                } else {
                    op->filter.obj.reg = av->id;
                    op->filter.reg_mask = filter->reg_mask | RULE_PAIR_OBJECT;
                    written[av->id] = true;
                }

                written[subj->id] = true;

                /* Create new frame for operations that create reflexive set */
                push_frame(rule);

                /* Insert superset instruction to find all supersets */
                if (filter->reflexive) {
                    subj = ensure_most_specific_var(rule, subj, written);
                    ecs_assert(subj->kind == EcsRuleVarKindEntity, 
                        ECS_INTERNAL_ERROR, NULL);
                    ecs_assert(written[subj->id] == true,
                        ECS_INTERNAL_ERROR, NULL);

                    ecs_rule_pair_t super_filter = {0};
                    super_filter.pred = filter->pred;
                    super_filter.obj.reg = subj->id;
                    super_filter.reg_mask = filter->reg_mask | RULE_PAIR_OBJECT;

                    insert_reflexive_set(rule, EcsRuleSuperSet, obj, 
                        super_filter, c, written, true);
                } else {
                    insert_reflexive_set(rule, EcsRuleSuperSet, obj, 
                        op->filter, c, written, true);
                }
            }
        }
    }

    if (same_obj_subj) {
        /* Can't have relation with same variables that is acyclic and not
         * reflexive, this should've been caught earlier. */
        ecs_assert(!filter->acyclic || filter->reflexive, 
            ECS_INTERNAL_ERROR, NULL);

        /* If relation is reflexive and entity has an instance of R, no checks
         * are needed because R(X, X) is always true. */
        if (!filter->reflexive) {
            push_frame(rule);

            /* Insert check if the (R, X) pair that was found matches with one
             * of the entities in the table with the pair. */
            ecs_rule_op_t *op = insert_operation(rule, -1, written);
            obj = get_most_specific_var(rule, obj, written);
            ecs_assert(obj->kind == EcsRuleVarKindEntity, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(written[subj->id] == true, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(written[obj->id] == true, ECS_INTERNAL_ERROR, NULL);
            
            set_input_to_subj(rule, op, term, subj);
            op->filter.obj.reg = obj->id;
            op->filter.reg_mask = RULE_PAIR_OBJECT;

            if (subj->kind == EcsRuleVarKindTable) {
                op->kind = EcsRuleInTable;
            } else {
                op->kind = EcsRuleEq;
            }
        }
    }
}

static
void insert_term_1(
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_pair_t *filter,
    int32_t c,
    bool *written)
{
    ecs_rule_var_t *subj = term_subj(rule, term);
    subj = get_most_specific_var(rule, subj, written);
    insert_select_or_with(rule, c, term, subj, filter, written);
}

static
void insert_term(
    ecs_rule_t *rule,
    ecs_term_t *term,
    int32_t c,
    bool *written)
{
    bool obj_set = obj_is_set(term);

    ensure_most_specific_var(rule, term_pred(rule, term), written);
    if (obj_set) {
        ensure_most_specific_var(rule, term_obj(rule, term), written);
    }

    /* If term has Not operator, prepend Not which turns a fail into a pass */
    int32_t prev = rule->operation_count;
    ecs_rule_op_t *not_pre;
    if (term->oper == EcsNot) {
        not_pre = insert_operation(rule, -1, written);
        not_pre->kind = EcsRuleNot;
        not_pre->has_in = false;
        not_pre->has_out = false;
    }

    ecs_rule_pair_t filter = term_to_pair(rule, term);
    prepare_predicate(rule, &filter, c, written);

    if (subj_is_set(term) && !obj_set) {
        insert_term_1(rule, term, &filter, c, written);
    } else if (obj_set) {
        insert_term_2(rule, term, &filter, c, written);
    }

    /* If term has Not operator, append Not which turns a pass into a fail */
    if (term->oper == EcsNot) {
        ecs_rule_op_t *not_post = insert_operation(rule, -1, written);
        not_post->kind = EcsRuleNot;
        not_post->has_in = false;
        not_post->has_out = false;

        not_post->on_pass = prev - 1;
        not_post->on_fail = prev - 1;
        not_pre = &rule->operations[prev];
        not_pre->on_fail = rule->operation_count;
    }

    if (term->oper == EcsOptional) {
        /* Insert Not instruction that ensures that the optional term is only
         * executed once */
        ecs_rule_op_t *jump = insert_operation(rule, -1, written);
        jump->kind = EcsRuleNot;
        jump->has_in = false;
        jump->has_out = false;
        jump->on_pass = rule->operation_count;
        jump->on_fail = prev - 1;

        /* Find exit instruction for optional term, and make the fail label
         * point to the Not operation, so that even when the operation fails,
         * it won't discard the result */
        int i, min_fail = -1, exit_op = -1;
        for (i = prev; i < rule->operation_count; i ++) {
            ecs_rule_op_t *op = &rule->operations[i];
            if (min_fail == -1 || (op->on_fail >= 0 && op->on_fail < min_fail)){
                min_fail = op->on_fail;
                exit_op = i;
            }
        }

        ecs_assert(exit_op != -1, ECS_INTERNAL_ERROR, NULL);
        ecs_rule_op_t *op = &rule->operations[exit_op];
        op->on_fail = rule->operation_count - 1;
    }

    push_frame(rule);
}

/* Create program from operations that will execute the query */
static
void compile_program(
    ecs_rule_t *rule)
{
    /* Trace which variables have been written while inserting instructions.
     * This determines which instruction needs to be inserted */
    bool written[ECS_RULE_MAX_VAR_COUNT] = { false };

    ecs_term_t *terms = rule->filter.terms;
    int32_t v, c, term_count = rule->filter.term_count;
    ecs_rule_op_t *op;

    /* Insert input, which is always the first instruction */
    insert_input(rule);

    /* First insert all instructions that do not have a variable subject. Such
     * instructions iterate the type of an entity literal and are usually good
     * candidates for quickly narrowing down the set of potential results. */
    for (c = 0; c < term_count; c ++) {
        ecs_term_t *term = &terms[c];
        if (skip_term(term)) {
            continue;
        }

        if (term->oper == EcsOptional || term->oper == EcsNot) {
            continue;
        }

        ecs_rule_var_t* subj = term_subj(rule, term);
        if (subj) {
            continue;
        }

        insert_term(rule, term, c, written);
    }

    /* Insert variables based on dependency order */
    for (v = 0; v < rule->subj_var_count; v ++) {
        int32_t var_id = rule->var_eval_order[v];
        ecs_rule_var_t *var = &rule->vars[var_id];

        ecs_assert(var->kind == EcsRuleVarKindTable, ECS_INTERNAL_ERROR, NULL);

        for (c = 0; c < term_count; c ++) {
            ecs_term_t *term = &terms[c];
            if (skip_term(term)) {
                continue;
            }

            if (term->oper == EcsOptional || term->oper == EcsNot) {
                continue;
            }

            /* Only process columns for which variable is subject */
            ecs_rule_var_t* subj = term_subj(rule, term);
            if (subj != var) {
                continue;
            }

            insert_term(rule, term, c, written);

            var = &rule->vars[var_id];
        }
    }

    /* Insert terms with Not operators */
    for (c = 0; c < term_count; c ++) {
        ecs_term_t *term = &terms[c];
        if (term->oper != EcsNot) {
            continue;
        }

        insert_term(rule, term, c, written);
    }

    /* Insert terms with Optional operators last, as optional terms cannot 
     * eliminate results, and would just add overhead to evaluation of 
     * non-matching entities. */
    for (c = 0; c < term_count; c ++) {
        ecs_term_t *term = &terms[c];
        if (term->oper != EcsOptional) {
            continue;
        }
        
        insert_term(rule, term, c, written);
    }

    /* Verify all subject variables have been written. Subject variables are of
     * the table type, and a select/subset should have been inserted for each */
    for (v = 0; v < rule->subj_var_count; v ++) {
        if (!written[v]) {
            /* If the table variable hasn't been written, this can only happen
             * if an instruction wrote the variable before a select/subset could
             * have been inserted for it. Make sure that this is the case by
             * testing if an entity variable exists and whether it has been
             * written. */
            ecs_rule_var_t *var = find_variable(
                rule, EcsRuleVarKindEntity, rule->vars[v].name);
            ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(written[var->id], ECS_INTERNAL_ERROR, var->name);
            (void)var;
        }
    }

    /* Make sure that all entity variables are written. With the exception of
     * the this variable, which can be returned as a table, other variables need
     * to be available as entities. This ensures that all permutations for all
     * variables are correctly returned by the iterator. When an entity variable
     * hasn't been written yet at this point, it is because it only constrained
     * through a common predicate or object. */
    for (; v < rule->var_count; v ++) {
        if (!written[v]) {
            ecs_rule_var_t *var = &rule->vars[v];
            ecs_assert(var->kind == EcsRuleVarKindEntity, 
                ECS_INTERNAL_ERROR, NULL);

            ecs_rule_var_t *table_var = find_variable(
                rule, EcsRuleVarKindTable, var->name);
            
            /* A table variable must exist if the variable hasn't been resolved
             * yet. If there doesn't exist one, this could indicate an 
             * unconstrained variable which should have been caught earlier */
            ecs_assert(table_var != NULL, ECS_INTERNAL_ERROR, var->name);

            /* Insert each operation that takes the table variable as input, and
             * yields each entity in the table */
            op = insert_operation(rule, -1, written);
            op->kind = EcsRuleEach;
            op->r_in = table_var->id;
            op->r_out = var->id;
            op->frame = rule->frame_count;
            op->has_in = true;
            op->has_out = true;
            written[var->id] = true;
            
            push_frame(rule);
        }
    }     

    /* Insert yield, which is always the last operation */
    insert_yield(rule);
}

static
void create_variable_name_array(
    ecs_rule_t *rule)
{
    if (rule->var_count) {
        int i;
        for (i = 0; i < rule->var_count; i ++) {
            ecs_rule_var_t *var = &rule->vars[i];

            if (var->kind != EcsRuleVarKindEntity) {
                /* Table variables are hidden for applications. */
                rule->var_names[var->id] = NULL;
            } else {
                rule->var_names[var->id] = var->name;
            }
        }
    }
}

static
void create_variable_cross_references(
    ecs_rule_t *rule)
{
    if (rule->var_count) {
        int i;
        for (i = 0; i < rule->var_count; i ++) {
            ecs_rule_var_t *var = &rule->vars[i];
            if (var->kind == EcsRuleVarKindEntity) {
                ecs_rule_var_t *tvar = find_variable(
                    rule, EcsRuleVarKindTable, var->name);
                if (tvar) {
                    var->other = tvar->id;
                } else {
                    var->other = -1;
                }
            } else {
                ecs_rule_var_t *evar = find_variable(
                    rule, EcsRuleVarKindEntity, var->name);
                if (evar) {
                    var->other = evar->id;
                } else {
                    var->other = -1;
                }
            }
        }
    }
}

/* Implementation for iterable mixin */
static
void rule_iter_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter)
{
    ecs_poly_assert(poly, ecs_rule_t);

    if (filter) {
        iter[1] = ecs_rule_iter(world, (ecs_rule_t*)poly);
        iter[0] = ecs_term_chain_iter(&iter[1], filter);
    } else {
        iter[0] = ecs_rule_iter(world, (ecs_rule_t*)poly);
    }
}

static
int32_t find_term_var_id(
    ecs_rule_t *rule,
    ecs_term_id_t *term_id)
{
    if (term_id_is_variable(term_id)) {
        const char *var_name = term_id_var_name(term_id);
        ecs_rule_var_t *var = find_variable(
            rule, EcsRuleVarKindEntity, var_name);
        if (var) {
            return var->id;
        } else {
            /* If this is Any look for table variable. Since Any is only
             * required to return a single result, there is no need to 
             * insert an each instruction for a matching table. */
            if (term_id->entity == EcsAny) {
                var = find_variable(
                    rule, EcsRuleVarKindTable, var_name);
                if (var) {
                    return var->id;
                }
            }
        }
    }
    
    return -1;
}

ecs_rule_t* ecs_rule_init(
    ecs_world_t *world,
    const ecs_filter_desc_t *const_desc)
{
    ecs_rule_t *result = ecs_poly_new(ecs_rule_t);

    /* Initialize the query */
    ecs_filter_desc_t desc = *const_desc;
    desc.storage = &result->filter; /* Use storage of rule */
    result->filter = ECS_FILTER_INIT;
    if (ecs_filter_init(world, &desc) == NULL) {
        goto error;
    }

    result->world = world;

    /* Rule has no terms */
    if (!result->filter.term_count) {
        rule_error(result, "rule has no terms");
        goto error;
    }

    ecs_term_t *terms = result->filter.terms;
    int32_t i, term_count = result->filter.term_count;

    /* Make sure rule doesn't just have Not terms */
    for (i = 0; i < term_count; i++) {
        ecs_term_t *term = &terms[i];
        if (term->oper != EcsNot) {
            break;
        }
    }
    if (i == term_count) {
        rule_error(result, "rule cannot only have terms with Not operator");
        goto error;
    }

    /* Translate terms with a Not operator and Wildcard to Any, as we don't need
     * implicit variables for those */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->oper == EcsNot) {
            if (term->pred.entity == EcsWildcard) {
                term->pred.entity = EcsAny;
            }
            if (term->subj.entity == EcsWildcard) {
                term->subj.entity = EcsAny;
            }
            if (term->obj.entity == EcsWildcard) {
                term->obj.entity = EcsAny;
            }
        }
    }

    /* Find all variables & resolve dependencies */
    if (scan_variables(result) != 0) {
        goto error;
    }

    /* Create lookup array for subject variables */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_rule_term_vars_t *vars = &result->term_vars[i];
        vars->pred = find_term_var_id(result, &term->pred);
        vars->subj = find_term_var_id(result, &term->subj);
        vars->obj = find_term_var_id(result, &term->obj);
    }

    /* Generate the opcode array */
    compile_program(result);

    /* Create array with variable names so this can be easily accessed by 
     * iterators without requiring access to the ecs_rule_t */
    create_variable_name_array(result);

    /* Create cross-references between variables so it's easy to go from entity
     * to table variable and vice versa */
    create_variable_cross_references(result);

    result->iterable.init = rule_iter_init;

    return result;
error:
    ecs_rule_fini(result);
    return NULL;
}

void ecs_rule_fini(
    ecs_rule_t *rule)
{
    int32_t i;
    for (i = 0; i < rule->var_count; i ++) {
        ecs_os_free(rule->vars[i].name);
    }

    ecs_filter_fini(&rule->filter);

    ecs_os_free(rule->operations);
    ecs_os_free(rule);
}

const ecs_filter_t* ecs_rule_get_filter(
    const ecs_rule_t *rule)
{
    return &rule->filter; 
}

/* Quick convenience function to get a variable from an id */
static
ecs_rule_var_t* get_variable(
    const ecs_rule_t *rule,
    int32_t var_id)
{
    if (var_id == UINT8_MAX) {
        return NULL;
    }

    return (ecs_rule_var_t*)&rule->vars[var_id];
}

/* Convert the program to a string. This can be useful to analyze how a rule is
 * being evaluated. */
char* ecs_rule_str(
    ecs_rule_t *rule)
{
    ecs_check(rule != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_world_t *world = rule->world;
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    char filter_expr[256];

    int32_t i, count = rule->operation_count;
    for (i = 1; i < count; i ++) {
        ecs_rule_op_t *op = &rule->operations[i];
        ecs_rule_pair_t pair = op->filter;
        ecs_entity_t pred = pair.pred.ent;
        ecs_entity_t obj = pair.obj.ent;
        const char *pred_name = NULL, *obj_name = NULL;
        char *pred_name_alloc = NULL, *obj_name_alloc = NULL;

        if (pair.reg_mask & RULE_PAIR_PREDICATE) {
            ecs_assert(rule->vars != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_rule_var_t *type_var = &rule->vars[pair.pred.reg];
            pred_name = type_var->name;
        } else if (pred) {
            pred_name_alloc = ecs_get_fullpath(world, ecs_get_alive(world, pred));
            pred_name = pred_name_alloc;
        }

        if (pair.reg_mask & RULE_PAIR_OBJECT) {
            ecs_assert(rule->vars != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_rule_var_t *obj_var = &rule->vars[pair.obj.reg];
            obj_name = obj_var->name;
        } else if (obj) {
            obj_name_alloc = ecs_get_fullpath(world, ecs_get_alive(world, obj));
            obj_name = obj_name_alloc;
        } else if (pair.obj_0) {
            obj_name = "0";
        }

        ecs_strbuf_append(&buf, "%2d: [S:%2d, P:%2d, F:%2d, T:%2d] ", i, 
            op->frame, op->on_pass, op->on_fail, op->term);

        bool has_filter = false;

        switch(op->kind) {
        case EcsRuleSelect:
            ecs_strbuf_append(&buf, "select   ");
            has_filter = true;
            break;
        case EcsRuleWith:
            ecs_strbuf_append(&buf, "with     ");
            has_filter = true;
            break;
        case EcsRuleStore:
            ecs_strbuf_append(&buf, "store    ");
            break;
        case EcsRuleSuperSet:
            ecs_strbuf_append(&buf, "superset ");
            has_filter = true;
            break;             
        case EcsRuleSubSet:
            ecs_strbuf_append(&buf, "subset   ");
            has_filter = true;
            break;            
        case EcsRuleEach:
            ecs_strbuf_append(&buf, "each     ");
            break;
        case EcsRuleSetJmp:
            ecs_strbuf_append(&buf, "setjmp   ");
            break;
        case EcsRuleJump:
            ecs_strbuf_append(&buf, "jump     ");
            break;
        case EcsRuleNot:
            ecs_strbuf_append(&buf, "not      ");
            break;
        case EcsRuleInTable:
            ecs_strbuf_append(&buf, "intable  ");
            has_filter = true;
            break;
        case EcsRuleEq:
            ecs_strbuf_append(&buf, "eq       ");
            has_filter = true;
            break;            
        case EcsRuleYield:
            ecs_strbuf_append(&buf, "yield    ");
            break;
        default:
            continue;
        }

        if (op->has_out) {
            ecs_rule_var_t *r_out = get_variable(rule, op->r_out);
            if (r_out) {
                ecs_strbuf_append(&buf, "O:%s%s ", 
                    r_out->kind == EcsRuleVarKindTable ? "t" : "",
                    r_out->name);
            } else if (op->subject) {
                char *subj_path = ecs_get_fullpath(world, op->subject);
                ecs_strbuf_append(&buf, "O:%s ", subj_path);
                ecs_os_free(subj_path);
            }
        }

        if (op->has_in) {
            ecs_rule_var_t *r_in = get_variable(rule, op->r_in);
            if (r_in) {
                ecs_strbuf_append(&buf, "I:%s%s ", 
                    r_in->kind == EcsRuleVarKindTable ? "t" : "",
                    r_in->name);
            }
            if (op->subject) {
                char *subj_path = ecs_get_fullpath(world, op->subject);
                ecs_strbuf_append(&buf, "I:%s ", subj_path);
                ecs_os_free(subj_path);
            }
        }

        if (has_filter) {
            if (!pred_name) {
                pred_name = "-";
            }
            if (!obj_name && !pair.obj_0) {
                ecs_os_sprintf(filter_expr, "(%s)", pred_name);
            } else {
                ecs_os_sprintf(filter_expr, "(%s, %s)", pred_name, obj_name);
            }
            ecs_strbuf_append(&buf, "F:%s", filter_expr);
        }

        ecs_strbuf_appendstr(&buf, "\n");

        ecs_os_free(pred_name_alloc);
        ecs_os_free(obj_name_alloc);
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

/* Public function that returns number of variables. This enables an application
 * to iterate the variables and obtain their values. */
int32_t ecs_rule_var_count(
    const ecs_rule_t *rule)
{
    ecs_assert(rule != NULL, ECS_INTERNAL_ERROR, NULL);
    return rule->var_count;
}

/* Public function to find a variable by name */
int32_t ecs_rule_find_var(
    const ecs_rule_t *rule,
    const char *name)
{
    ecs_rule_var_t *v = find_variable(rule, EcsRuleVarKindEntity, name);
    if (v) {
        return v->id;
    } else {
        return -1;
    }
}

/* Public function to get the name of a variable. */
const char* ecs_rule_var_name(
    const ecs_rule_t *rule,
    int32_t var_id)
{
    return rule->vars[var_id].name;
}

/* Public function to get the type of a variable. */
bool ecs_rule_var_is_entity(
    const ecs_rule_t *rule,
    int32_t var_id)
{
    return rule->vars[var_id].kind == EcsRuleVarKindEntity;
}

static
void ecs_rule_iter_free(
    ecs_iter_t *iter)
{
    ecs_rule_iter_t *it = &iter->priv.iter.rule;
    ecs_os_free(it->registers);
    ecs_os_free(it->columns);
    ecs_os_free(it->op_ctx);
    iter->columns = NULL;
    it->registers = NULL;
    it->columns = NULL;
    it->op_ctx = NULL;
}

/* Create rule iterator */
ecs_iter_t ecs_rule_iter(
    const ecs_world_t *world,
    const ecs_rule_t *rule)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(rule != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t result = {0};
    int i;

    result.world = (ecs_world_t*)world;
    result.real_world = (ecs_world_t*)ecs_get_world(rule->world);

    flecs_process_pending_tables(result.real_world);

    ecs_rule_iter_t *it = &result.priv.iter.rule;
    it->rule = rule;

    if (rule->operation_count) {
        if (rule->var_count) {
            it->registers = ecs_os_malloc_n(ecs_var_t, 
                rule->operation_count * rule->var_count);
        }
        
        it->op_ctx = ecs_os_calloc_n(ecs_rule_op_ctx_t, rule->operation_count);

        if (rule->filter.term_count) {
            it->columns = ecs_os_malloc_n(int32_t, 
                rule->operation_count * rule->filter.term_count);
        }

        for (i = 0; i < rule->filter.term_count; i ++) {
            it->columns[i] = -1;
        }
    }

    it->op = 0;

    for (i = 0; i < rule->var_count; i ++) {
        if (rule->vars[i].kind == EcsRuleVarKindEntity) {
            entity_reg_set(rule, it->registers, i, EcsWildcard);
        } else {
            table_reg_set(rule, it->registers, i, NULL);
        }
    }

    result.variable_names = (char**)rule->var_names;
    result.variable_count = rule->var_count;
    result.term_count = rule->filter.term_count;
    result.terms = rule->filter.terms;
    result.next = ecs_rule_next;
    result.fini = ecs_rule_iter_free;
    ECS_BIT_COND(result.flags, EcsIterIsFilter, 
        ECS_BIT_IS_SET(rule->filter.flags, EcsFilterIsFilter));

    flecs_iter_init(&result, 
        flecs_iter_cache_ids |
        /* flecs_iter_cache_columns | provided by rule iterator */
        flecs_iter_cache_subjects |
        flecs_iter_cache_sizes |
        flecs_iter_cache_ptrs |
        /* flecs_iter_cache_match_indices | not necessary for iteration */
        flecs_iter_cache_variables);

    result.columns = it->columns; /* prevent alloc */

    return result;
}

/* Edge case: if the filter has the same variable for both predicate and
 * object, they are both resolved at the same time but at the time of 
 * evaluating the filter they're still wildcards which would match columns
 * that have different predicates/objects. Do an additional scan to make
 * sure the column we're returning actually matches. */
static
int32_t find_next_same_var(
    ecs_type_t type,
    int32_t column,
    ecs_id_t pattern)
{
    /* If same_var is true, this has to be a wildcard pair. We cannot have
     * the same variable in a pair, and one part of a pair resolved with
     * another part unresolved. */
    ecs_assert(pattern == ecs_pair(EcsWildcard, EcsWildcard), 
        ECS_INTERNAL_ERROR, NULL);
    (void)pattern;
    
    /* Keep scanning for an id where rel and obj are the same */
    ecs_id_t *ids = type.array;
    int32_t i, count = type.count;
    for (i = column + 1; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (!ECS_HAS_ROLE(id, PAIR)) {
            /* If id is not a pair, this will definitely not match, and we
             * will find no further matches. */
            return -1;
        }

        if (ECS_PAIR_FIRST(id) == ECS_PAIR_SECOND(id)) {
            /* Found a match! */
            return i;
        }
    }

    /* No pairs found with same rel/obj */
    return -1;
}

static
int32_t find_next_column(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t column,
    ecs_rule_filter_t *filter)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t pattern = filter->mask;
    ecs_type_t type = table->type;

    if (column == -1) {
        ecs_table_record_t *tr = flecs_table_record_get(world, table, pattern);
        if (!tr) {
            return -1;
        }
        column = tr->column;
    } else {   
        column = ecs_search_offset(world, table, column + 1, filter->mask, 0);
        if (column == -1) {
            return -1;
        }
    }

    if (filter->same_var) {
        column = find_next_same_var(type, column - 1, filter->mask);
    }

    return column;
}

/* This function finds the next table in a table set, and is used by the select
 * operation. The function automatically skips empty tables, so that subsequent
 * operations don't waste a lot of processing for nothing. */
static
ecs_table_record_t find_next_table(
    ecs_rule_filter_t *filter,
    ecs_rule_with_ctx_t *op_ctx)
{
    ecs_table_cache_iter_t *it = &op_ctx->it;
    ecs_table_t *table = NULL;
    int32_t column = -1;

    const ecs_table_record_t *tr;
    while ((column == -1) && (tr = flecs_table_cache_next(it, ecs_table_record_t))) {
        table = tr->hdr.table;

        /* Should only iterate non-empty tables */
        ecs_assert(ecs_table_count(table) != 0, ECS_INTERNAL_ERROR, NULL);

        column = tr->column;
        if (filter->same_var) {
            column = find_next_same_var(table->type, column - 1, filter->mask);
        }
    }

    if (column == -1) {
        table = NULL;
    }

    return (ecs_table_record_t){.hdr.table = table, .column = column};
}


static
ecs_id_record_t* find_tables(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr || !flecs_table_cache_count(&idr->cache)) {
        /* Skip ids that don't have (non-empty) tables */
        return NULL;
    }
    return idr;
}

static
ecs_id_t rule_get_column(
    ecs_type_t type,
    int32_t column)
{
    ecs_assert(column < type.count, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t *comp = &type.array[column];
    return *comp;
}

static
void set_source(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    ecs_var_t *regs,
    int32_t r)
{
    if (op->term == -1) {
        /* If operation is not associated with a term, don't set anything */
        return;
    }

    ecs_assert(op->term >= 0, ECS_INTERNAL_ERROR, NULL);

    const ecs_rule_t *rule = it->priv.iter.rule.rule;
    if ((r != UINT8_MAX) && rule->vars[r].kind == EcsRuleVarKindEntity) {
        it->subjects[op->term] = reg_get_entity(rule, op, regs, r);
    } else {
        it->subjects[op->term] = 0;
    }
}

static
void set_term_vars(
    const ecs_rule_t *rule,
    ecs_var_t *regs,
    int32_t term,
    ecs_id_t id)
{
    if (term != -1) {
        ecs_world_t *world = rule->world;
        const ecs_rule_term_vars_t *vars = &rule->term_vars[term];
        if (vars->pred != -1) {
            regs[vars->pred].entity = ecs_pair_first(world, id);
            ecs_assert(ecs_is_valid(world, regs[vars->pred].entity), 
                ECS_INTERNAL_ERROR, NULL);
        }
        if (vars->obj != -1) {
            regs[vars->obj].entity = ecs_pair_second(world, id);
            ecs_assert(ecs_is_valid(world, regs[vars->obj].entity), 
                ECS_INTERNAL_ERROR, NULL);
        }
    }
}

/* Input operation. The input operation acts as a placeholder for the start of
 * the program, and creates an entry in the register array that can serve to
 * store variables passed to an iterator. */
static
bool eval_input(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)it;
    (void)op;
    (void)op_index;

    if (!redo) {
        /* First operation executed by the iterator. Always return true. */
        return true;
    } else {
        /* When Input is asked to redo, it means that all other operations have
         * exhausted their results. Input itself does not yield anything, so
         * return false. This will terminate rule execution. */
        return false;
    }
}

static
bool eval_superset(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t  *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_rule_superset_ctx_t *op_ctx = &iter->op_ctx[op_index].is.superset;
    ecs_rule_superset_frame_t *frame = NULL;
    ecs_var_t *regs = get_registers(iter, op);

    /* Get register indices for output */
    int32_t sp;
    int32_t r = op->r_out;

    /* Register cannot be a literal, since we need to store things in it */
    ecs_assert(r != UINT8_MAX, ECS_INTERNAL_ERROR, NULL);

    /* Get queried for id, fill out potential variables */
    ecs_rule_pair_t pair = op->filter;

    ecs_rule_filter_t filter = pair_to_filter(iter, op, pair);
    ecs_entity_t rel = ECS_PAIR_FIRST(filter.mask);
    ecs_rule_filter_t super_filter = { 
        .mask = ecs_pair(rel, EcsWildcard) 
    };
    ecs_table_t *table = NULL;

    /* Check if input register is constrained */
    ecs_entity_t result = iter->registers[r].entity;
    bool output_is_input = ecs_iter_var_is_constrained(it, r);
    if (output_is_input && !redo) {
        ecs_assert(regs[r].entity == iter->registers[r].entity, 
            ECS_INTERNAL_ERROR, NULL);
    }

    if (!redo) {
        op_ctx->stack = op_ctx->storage;
        sp = op_ctx->sp = 0;
        frame = &op_ctx->stack[sp];

        /* Get table of object for which to get supersets */
        ecs_entity_t obj = ECS_PAIR_SECOND(filter.mask);
        if (obj == EcsWildcard) {
            ecs_assert(pair.reg_mask & RULE_PAIR_OBJECT, 
                ECS_INTERNAL_ERROR, NULL);
            table = regs[pair.obj.reg].range.table;
        } else {
            table = table_from_entity(world, obj).table;
        }

        int32_t column;

        /* If output variable is already set, check if it matches */
        if (output_is_input) {
            ecs_id_t id = ecs_pair(rel, result);
            ecs_entity_t subj = 0;
            column = ecs_search_relation(world, table, 0, id, rel, 
                0, 0, &subj, 0, 0, 0);
            if (column != -1) {
                if (subj != 0) {
                    table = ecs_get_table(world, subj);
                }
            }
        } else {
            column = find_next_column(world, table, -1, &super_filter);
        }

        /* If no matching column was found, there are no supersets */
        if (column == -1) {
            return false;
        }

        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_id_t col_id = rule_get_column(table->type, column);
        ecs_assert(ECS_HAS_ROLE(col_id, PAIR), ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t col_obj = ecs_pair_second(world, col_id);

        reg_set_entity(rule, regs, r, col_obj);

        frame->table = table;
        frame->column = column;

        return true;
    } else if (output_is_input) {
        return false;
    }

    sp = op_ctx->sp;
    frame = &op_ctx->stack[sp];
    table = frame->table;
    int32_t column = frame->column;

    ecs_id_t col_id = rule_get_column(table->type, column);
    ecs_entity_t col_obj = ecs_pair_second(world, col_id);
    ecs_table_t *next_table = table_from_entity(world, col_obj).table;

    if (next_table) {
        sp ++;
        frame = &op_ctx->stack[sp];
        frame->table = next_table;
        frame->column = -1;
    }

    do {
        frame = &op_ctx->stack[sp];
        table = frame->table;
        column = frame->column;

        column = find_next_column(world, table, column, &super_filter);
        if (column != -1) {
            op_ctx->sp = sp;
            frame->column = column;
            col_id = rule_get_column(table->type, column);
            col_obj = ecs_pair_second(world, col_id);
            reg_set_entity(rule, regs, r, col_obj);
            return true;        
        }

        sp --;
    } while (sp >= 0);

    return false;
}

static
bool eval_subset(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t  *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_rule_subset_ctx_t *op_ctx = &iter->op_ctx[op_index].is.subset;
    ecs_rule_subset_frame_t *frame = NULL;
    ecs_table_record_t table_record;
    ecs_var_t *regs = get_registers(iter, op);

    /* Get register indices for output */
    int32_t sp, row;
    int32_t r = op->r_out;
    ecs_assert(r != UINT8_MAX, ECS_INTERNAL_ERROR, NULL);

    /* Get queried for id, fill out potential variables */
    ecs_rule_pair_t pair = op->filter;
    ecs_rule_filter_t filter = pair_to_filter(iter, op, pair);
    ecs_id_record_t *idr;
    ecs_table_t *table = NULL;

    if (!redo) {
        op_ctx->stack = op_ctx->storage;
        sp = op_ctx->sp = 0;
        frame = &op_ctx->stack[sp];
        idr = frame->with_ctx.idr = find_tables(world, filter.mask);
        if (!idr) {
            return false;
        }

        flecs_table_cache_iter(&idr->cache, &frame->with_ctx.it);
        table_record = find_next_table(&filter, &frame->with_ctx);
        
        /* If first table set has no non-empty table, yield nothing */
        if (!table_record.hdr.table) {
            return false;
        }

        frame->row = 0;
        frame->column = table_record.column;
        table_reg_set(rule, regs, r, (frame->table = table_record.hdr.table));
        goto yield;
    }

    do {
        sp = op_ctx->sp;
        frame = &op_ctx->stack[sp];
        table = frame->table;
        row = frame->row;

        /* If row exceeds number of elements in table, find next table in frame that
         * still has entities */
        while ((sp >= 0) && (row >= ecs_table_count(table))) {
            table_record = find_next_table(&filter, &frame->with_ctx);

            if (table_record.hdr.table) {
                table = frame->table = table_record.hdr.table;
                ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
                frame->row = 0;
                frame->column = table_record.column;
                table_reg_set(rule, regs, r, table);
                goto yield;
            } else {
                sp = -- op_ctx->sp;
                if (sp < 0) {
                    /* If none of the frames yielded anything, no more data */
                    return false;
                }
                frame = &op_ctx->stack[sp];
                table = frame->table;
                idr = frame->with_ctx.idr;
                row = ++ frame->row;

                ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            }
        }

        int32_t row_count = ecs_table_count(table);

        /* Table must have at least row elements */
        ecs_assert(row_count > row, ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t *entities = ecs_storage_first(&table->data.entities);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);

        /* The entity used to find the next table set */
        do {
            ecs_entity_t e = entities[row];

            /* Create look_for expression with the resolved entity as object */
            pair.reg_mask &= ~RULE_PAIR_OBJECT; /* turn of bit because it's not a reg */
            pair.obj.ent = e;
            filter = pair_to_filter(iter, op, pair);

            /* Find table set for expression */
            table = NULL;
            idr = find_tables(world, filter.mask);

            /* If table set is found, find first non-empty table */
            if (idr) {
                ecs_rule_subset_frame_t *new_frame = &op_ctx->stack[sp + 1];
                new_frame->with_ctx.idr = idr;
                flecs_table_cache_iter(&idr->cache, &new_frame->with_ctx.it);
                table_record = find_next_table(&filter, &new_frame->with_ctx);

                /* If set contains non-empty table, push it to stack */
                if (table_record.hdr.table) {
                    table = table_record.hdr.table;
                    op_ctx->sp ++;
                    new_frame->table = table;
                    new_frame->row = 0;
                    new_frame->column = table_record.column;
                    frame = new_frame;
                }
            }

            /* If no table was found for the current entity, advance row */
            if (!table) {
                row = ++ frame->row;
            }
        } while (!table && row < row_count);
    } while (!table);

    table_reg_set(rule, regs, r, table);

yield:
    set_term_vars(rule, regs, op->term, frame->table->type.array[frame->column]);

    return true;
}

/* Select operation. The select operation finds and iterates a table set that
 * corresponds to its pair expression.  */
static
bool eval_select(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t  *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_rule_with_ctx_t *op_ctx = &iter->op_ctx[op_index].is.with;
    ecs_table_record_t table_record;
    ecs_var_t *regs = get_registers(iter, op);

    /* Get register indices for output */
    int32_t r = op->r_out;
    ecs_assert(r != UINT8_MAX, ECS_INTERNAL_ERROR, NULL);

    /* Get queried for id, fill out potential variables */
    ecs_rule_pair_t pair = op->filter;
    ecs_rule_filter_t filter = pair_to_filter(iter, op, pair);
    ecs_entity_t pattern = filter.mask;
    int32_t *columns = rule_get_columns(iter, op);

    int32_t column = -1;
    ecs_table_t *table = NULL;
    ecs_id_record_t *idr;

    if (!redo && op->term != -1) {
        columns[op->term] = -1;
    }

    /* If this is a redo, we already looked up the table set */
    if (redo) {
        idr = op_ctx->idr;
    
    /* If this is not a redo lookup the table set. Even though this may not be
     * the first time the operation is evaluated, variables may have changed
     * since last time, which could change the table set to lookup. */
    } else {
        /* A table set is a set of tables that all contain at least the 
         * requested look_for expression. What is returned is a table record, 
         * which in addition to the table also stores the first occurrance at
         * which the requested expression occurs in the table. This reduces (and
         * in most cases eliminates) any searching that needs to occur in a
         * table type. Tables are also registered under wildcards, which is why
         * this operation can simply use the look_for variable directly */

        idr = op_ctx->idr = find_tables(world, pattern);
    }

    /* If no table set was found for queried for entity, there are no results */
    if (!idr) {
        return false;
    }

    /* If the input register is not NULL, this is a variable that's been set by
     * the application. */
    table = iter->registers[r].range.table;
    bool output_is_input = table != NULL;

    if (output_is_input && !redo) {
        ecs_assert(regs[r].range.table == iter->registers[r].range.table, 
            ECS_INTERNAL_ERROR, NULL);

        table = iter->registers[r].range.table;

        /* Check if table can be found in the id record. If not, the provided 
        * table does not match with the query. */
        ecs_table_record_t *tr = ecs_table_cache_get(&idr->cache, table);
        if (!tr) {
            return false;
        }

        column = op_ctx->column = tr->column;
    }

    /* If this is not a redo, start at the beginning */
    if (!redo) {
        if (!table) {
            flecs_table_cache_iter(&idr->cache, &op_ctx->it);

            /* Return the first table_record in the table set. */
            table_record = find_next_table(&filter, op_ctx);
        
            /* If no table record was found, there are no results. */
            if (!table_record.hdr.table) {
                return false;
            }

            table = table_record.hdr.table;

            /* Set current column to first occurrence of queried for entity */
            column = op_ctx->column = table_record.column;

            /* Store table in register */
            table_reg_set(rule, regs, r, table);
        }
    
    /* If this is a redo, progress to the next match */
    } else {
        /* First test if there are any more matches for the current table, in 
         * case we're looking for a wildcard. */
        if (filter.wildcard) {
            table = table_reg_get(rule, regs, r).table;
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

            column = op_ctx->column;
            column = find_next_column(world, table, column, &filter);
            op_ctx->column = column;
        }

        /* If no next match was found for this table, move to next table */
        if (column == -1) {
            if (output_is_input) {
                return false;
            }

            table_record = find_next_table(&filter, op_ctx);
            if (!table_record.hdr.table) {
                return false;
            }

            /* Assign new table to table register */
            table_reg_set(rule, regs, r, (table = table_record.hdr.table));

            /* Assign first matching column */
            column = op_ctx->column = table_record.column;
        }
    }

    /* If we got here, we found a match. Table and column must be set */
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);

    if (op->term != -1) {
        columns[op->term] = column;
    }

    /* If this is a wildcard query, fill out the variable registers */
    if (filter.wildcard) {
        reify_variables(iter, op, &filter, table->type, column);
    }

    return true;
}

/* With operation. The With operation always comes after either the Select or
 * another With operation, and applies additional filters to the table. */
static
bool eval_with(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_rule_with_ctx_t *op_ctx = &iter->op_ctx[op_index].is.with;
    ecs_var_t *regs = get_registers(iter, op);

    /* Get register indices for input */
    int32_t r = op->r_in;

    /* Get queried for id, fill out potential variables */
    ecs_rule_pair_t pair = op->filter;
    ecs_rule_filter_t filter = pair_to_filter(iter, op, pair);
    int32_t *columns = rule_get_columns(iter, op);

    /* If looked for entity is not a wildcard (meaning there are no unknown/
     * unconstrained variables) and this is a redo, nothing more to yield. */
    if (redo && !filter.wildcard) {
        return false;
    }

    int32_t column = -1;
    ecs_table_t *table = NULL;
    ecs_id_record_t *idr;

    if (op->term != -1) {
        columns[op->term] = -1;
    }

    /* If this is a redo, we already looked up the table set */
    if (redo) {
        idr = op_ctx->idr;
    
    /* If this is not a redo lookup the table set. Even though this may not be
     * the first time the operation is evaluated, variables may have changed
     * since last time, which could change the table set to lookup. */
    } else {
        /* Predicates can be reflexive, which means that if we have a
         * transitive predicate which is provided with the same subject and
         * object, it should return true. By default with will not return true
         * as the subject likely does not have itself as a relationship, which
         * is why this is a special case. 
         *
         * TODO: might want to move this code to a separate with_reflexive
         * instruction to limit branches for non-transitive queries (and to keep
         * code more readable).
         */
        if (pair.transitive && pair.reflexive) {
            ecs_entity_t subj = 0, obj = 0;
            
            if (r == UINT8_MAX) {
                subj = op->subject;
            } else {
                const ecs_rule_var_t *v_subj = &rule->vars[r];

                if (v_subj->kind == EcsRuleVarKindEntity) {
                    subj = entity_reg_get(rule, regs, r);

                    /* This is the input for the op, so should always be set */
                    ecs_assert(subj != 0, ECS_INTERNAL_ERROR, NULL);
                }
            }

            /* If subj is set, it means that it is an entity. Try to also 
             * resolve the object. */
            if (subj) {
                /* If the object is not a wildcard, it has been reified. Get the
                 * value from either the register or as a literal */
                if (!filter.obj_wildcard) {
                    obj = ECS_PAIR_SECOND(filter.mask);
                    if (ecs_strip_generation(subj) == obj) {
                        return true;
                    }
                }
            }
        }

        /* The With operation finds the table set that belongs to its pair
         * filter. The table set is a sparse set that provides an O(1) operation
         * to check whether the current table has the required expression. */
        idr = op_ctx->idr = find_tables(world, filter.mask);
    }

    /* If no table set was found for queried for entity, there are no results. 
     * If this result is a transitive query, the table we're evaluating may not
     * be in the returned table set. Regardless, if the filter that contains a
     * transitive predicate does not have any tables associated with it, there
     * can be no transitive matches for the filter.  */
    if (!idr) {
        return false;
    }

    table = reg_get_range(rule, op, regs, r).table;
    if (!table) {
        return false;
    }

    /* If this is not a redo, start at the beginning */
    if (!redo) {
        column = op_ctx->column = find_next_column(world, table, -1, &filter);
    
    /* If this is a redo, progress to the next match */
    } else {        
        if (!filter.wildcard) {
            return false;
        }
        
        /* Find the next match for the expression in the column. The columns
         * array keeps track of the state for each With operation, so that
         * even after redoing a With, the search doesn't have to start from
         * the beginning. */
        column = find_next_column(world, table, op_ctx->column, &filter);
        op_ctx->column = column;
    }

    /* If no next match was found for this table, no more data */
    if (column == -1) {
        return false;
    }

    if (op->term != -1) {
        columns[op->term] = column;
    }

    /* If we got here, we found a match. Table and column must be set */
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);

    /* If this is a wildcard query, fill out the variable registers */
    if (filter.wildcard) {
        reify_variables(iter, op, &filter, table->type, column);
    }

    set_source(it, op, regs, r);

    return true;
}

/* Each operation. The each operation is a simple operation that takes a table
 * as input, and outputs each of the entities in a table. This operation is
 * useful for rules that match a table, and where the entities of the table are
 * used as predicate or object. If a rule contains an each operation, an
 * iterator is guaranteed to yield an entity instead of a table. The input for
 * an each operation can only be the root variable. */
static
bool eval_each(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    ecs_rule_each_ctx_t *op_ctx = &iter->op_ctx[op_index].is.each;
    ecs_var_t *regs = get_registers(iter, op);
    int32_t r_in = op->r_in;
    int32_t r_out = op->r_out;
    ecs_entity_t e;

    /* Make sure in/out registers are of the correct kind */
    ecs_assert(iter->rule->vars[r_in].kind == EcsRuleVarKindTable, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(iter->rule->vars[r_out].kind == EcsRuleVarKindEntity, 
        ECS_INTERNAL_ERROR, NULL);

    /* Get table, make sure that it contains data. The select operation should
     * ensure that empty tables are never forwarded. */
    ecs_table_range_t slice = table_reg_get(iter->rule, regs, r_in);
    ecs_table_t *table = slice.table;
    if (table) {       
        int32_t row, count = slice.count;
        int32_t offset = slice.offset;

        if (!count) {
            count = ecs_table_count(table);
            ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
        } else {
            count += offset;
        }

        ecs_entity_t *entities = ecs_storage_first(&table->data.entities);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If this is is not a redo, start from row 0, otherwise go to the
        * next entity. */
        if (!redo) {
            row = op_ctx->row = offset;
        } else {
            row = ++ op_ctx->row;
        }

        /* If row exceeds number of entities in table, return false */
        if (row >= count) {
            return false;
        }

        /* Skip builtin entities that could confuse operations */
        e = entities[row];
        while (e == EcsWildcard || e == EcsThis || e == EcsAny) {
            row ++;
            if (row == count) {
                return false;
            }
            e = entities[row];      
        }
    } else {
        if (!redo) {
            e = entity_reg_get(iter->rule, regs, r_in);
        } else {
            return false;
        }
    }

    /* Assign entity */
    entity_reg_set(iter->rule, regs, r_out, e);

    return true;
}

/* Store operation. Stores entity in register. This can either be an entity 
 * literal or an entity variable that will be stored in a table register. The
 * latter facilitates scenarios where an iterator only need to return a single
 * entity but where the Yield returns tables. */
static
bool eval_store(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)op_index;

    if (redo) {
        /* Only ever return result once */
        return false;
    }

    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_var_t *regs = get_registers(iter, op);
    int32_t r_in = op->r_in;
    int32_t r_out = op->r_out;

    (void)world;

    const ecs_rule_var_t *var_out = &rule->vars[r_out];
    if (var_out->kind == EcsRuleVarKindEntity) {
        ecs_entity_t out, in = reg_get_entity(rule, op, regs, r_in);
        ecs_assert(in != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_is_valid(world, in), ECS_INTERNAL_ERROR, NULL);

        out = iter->registers[r_out].entity;
        bool output_is_input = ecs_iter_var_is_constrained(it, r_out);
        if (output_is_input && !redo) {
            ecs_assert(regs[r_out].entity == iter->registers[r_out].entity, 
                ECS_INTERNAL_ERROR, NULL);

            if (out != in) {
                /* If output variable is set it must match the input */
                return false;
            }
        }

        reg_set_entity(rule, regs, r_out, in);
    } else {
        ecs_table_range_t out, in = reg_get_range(rule, op, regs, r_in);

        out = iter->registers[r_out].range;
        bool output_is_input = out.table != NULL;

        if (output_is_input && !redo) {
            ecs_assert(regs[r_out].entity == iter->registers[r_out].entity, 
                ECS_INTERNAL_ERROR, NULL);

            if (ecs_os_memcmp_t(&out, &in, ecs_table_range_t)) {
                /* If output variable is set it must match the input */
                return false;
            }
        }

        reg_set_range(rule, regs, r_out, &in);

        /* Ensure that if the input was an empty entity, information is not
         * lost */
        if (!regs[r_out].range.table) {
            regs[r_out].entity = reg_get_entity(rule, op, regs, r_in);
            ecs_assert(ecs_is_valid(world, regs[r_out].entity), 
                ECS_INTERNAL_ERROR, NULL);
        }
    }

    ecs_rule_filter_t filter = pair_to_filter(iter, op, op->filter);
    set_term_vars(rule, regs, op->term, filter.mask);

    return true;
}

/* A setjmp operation sets the jump label for a subsequent jump label. When the
 * operation is first evaluated (redo=false) it sets the label to the on_pass
 * label, and returns true. When the operation is evaluated again (redo=true)
 * the label is set to on_fail and the operation returns false. */
static
bool eval_setjmp(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    ecs_rule_setjmp_ctx_t *ctx = &iter->op_ctx[op_index].is.setjmp;

    if (!redo) {
        ctx->label = op->on_pass;
        return true;
    } else {
        ctx->label = op->on_fail;
        return false;
    }
}

/* The jump operation jumps to an operation label. The operation always returns
 * true. Since the operation modifies the control flow of the program directly, 
 * the dispatcher does not look at the on_pass or on_fail labels of the jump
 * instruction. Instead, the on_pass label is used to store the label of the
 * operation that contains the label to jump to. */
static
bool eval_jump(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)it;
    (void)op;
    (void)op_index;

    /* Passthrough, result is not used for control flow */
    return !redo;
}

/* The not operation reverts the result of the operation it embeds */
static
bool eval_not(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)it;
    (void)op;
    (void)op_index;

    return !redo;
}

/* Check if entity is stored in table */
static
bool eval_intable(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)op_index;
    
    if (redo) {
        return false;
    }

    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t  *rule = iter->rule;
    ecs_world_t *world = rule->world;
    ecs_var_t *regs = get_registers(iter, op);
    ecs_table_t *table = table_reg_get(rule, regs, op->r_in).table;

    ecs_rule_pair_t pair = op->filter;
    ecs_rule_filter_t filter = pair_to_filter(iter, op, pair);
    ecs_entity_t obj = ECS_PAIR_SECOND(filter.mask);
    ecs_assert(obj != 0 && obj != EcsWildcard, ECS_INTERNAL_ERROR, NULL);
    obj = ecs_get_alive(world, obj);
    ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *obj_table = ecs_get_table(world, obj);
    return obj_table == table;
}

/* Yield operation. This is the simplest operation, as all it does is return
 * false. This will move the solver back to the previous instruction which
 * forces redo's on previous operations, for as long as there are matching
 * results. */
static
bool eval_yield(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    (void)it;
    (void)op;
    (void)op_index;
    (void)redo;

    /* Yield always returns false, because there are never any operations after
     * a yield. */
    return false;
}

/* Dispatcher for operations */
static
bool eval_op(
    ecs_iter_t *it,
    ecs_rule_op_t *op,
    int32_t op_index,
    bool redo)
{
    switch(op->kind) {
    case EcsRuleInput:
        return eval_input(it, op, op_index, redo);
    case EcsRuleSelect:
        return eval_select(it, op, op_index, redo);
    case EcsRuleWith:
        return eval_with(it, op, op_index, redo);
    case EcsRuleSubSet:
        return eval_subset(it, op, op_index, redo);
    case EcsRuleSuperSet:
        return eval_superset(it, op, op_index, redo);
    case EcsRuleEach:
        return eval_each(it, op, op_index, redo);
    case EcsRuleStore:
        return eval_store(it, op, op_index, redo);
    case EcsRuleSetJmp:
        return eval_setjmp(it, op, op_index, redo);
    case EcsRuleJump:
        return eval_jump(it, op, op_index, redo);
    case EcsRuleNot:
        return eval_not(it, op, op_index, redo);
    case EcsRuleInTable:
        return eval_intable(it, op, op_index, redo);
    case EcsRuleYield:
        return eval_yield(it, op, op_index, redo);
    default:
        return false;
    }
}

/* Utility to copy all registers to the next frame. Keeping track of register
 * values for each operation is necessary, because if an operation is asked to
 * redo matching, it must to be able to pick up from where it left of */
static
void push_registers(
    ecs_rule_iter_t *it,
    int32_t cur,
    int32_t next)
{
    if (!it->rule->var_count) {
        return;
    }

    ecs_var_t *src_regs = get_register_frame(it, cur);
    ecs_var_t *dst_regs = get_register_frame(it, next);

    ecs_os_memcpy_n(dst_regs, src_regs, 
        ecs_var_t, it->rule->var_count);
}

/* Utility to copy all columns to the next frame. Columns keep track of which
 * columns are currently being evaluated for a table, and are populated by the
 * Select and With operations. The columns array is important, as it is used
 * to tell the application where to find component data. */
static
void push_columns(
    ecs_rule_iter_t *it,
    int32_t cur,
    int32_t next)
{
    if (!it->rule->filter.term_count) {
        return;
    }

    int32_t *src_cols = rule_get_columns_frame(it, cur);
    int32_t *dst_cols = rule_get_columns_frame(it, next);

    ecs_os_memcpy_n(dst_cols, src_cols, int32_t, it->rule->filter.term_count);
}

/* Populate iterator with data before yielding to application */
static
void populate_iterator(
    const ecs_rule_t *rule,
    ecs_iter_t *iter,
    ecs_rule_iter_t *it,
    ecs_rule_op_t *op)
{
    ecs_world_t *world = rule->world;
    int32_t r = op->r_in;
    ecs_var_t *regs = get_register_frame(it, op->frame);
    ecs_table_t *table = NULL;
    int32_t count = 0;
    int32_t offset = 0;

    /* If the input register for the yield does not point to a variable,
     * the rule doesn't contain a this (.) variable. In that case, the
     * iterator doesn't contain any data, and this function will simply
     * return true or false. An application will still be able to obtain
     * the variables that were resolved. */
    if (r != UINT8_MAX) {
        const ecs_rule_var_t *var = &rule->vars[r];
        ecs_var_t *reg = &regs[r];

        if (var->kind == EcsRuleVarKindTable) {
            ecs_table_range_t slice = table_reg_get(rule, regs, r);
            table = slice.table;
            count = slice.count;
            offset = slice.offset;
        } else {
            /* If a single entity is returned, simply return the
             * iterator with count 1 and a pointer to the entity id */
            ecs_assert(var->kind == EcsRuleVarKindEntity, 
                ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t e = reg->entity;
            ecs_assert(ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
            ecs_record_t *record = flecs_entities_get(world, e);
            offset = ECS_RECORD_TO_ROW(record->row);

            /* If an entity is not stored in a table, it could not have
             * been matched by anything */
            ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
            table = record->table;
            count = 1;
        }
    }

    int32_t i, var_count = rule->var_count;
    int32_t term_count = rule->filter.term_count;

    for (i = 0; i < var_count; i ++) {
        iter->variables[i] = regs[i];
    }

    for (i = 0; i < term_count; i ++) {
        int32_t v = rule->term_vars[i].subj;
        if (v != -1) {
            const ecs_rule_var_t *var = &rule->vars[v];
            if (var->name[0] != '.') {
                if (var->kind == EcsRuleVarKindEntity) {
                    iter->subjects[i] = regs[var->id].entity;
                } else {
                    /* This can happen for Any variables, where the actual
                     * content of the variable is not of interest to the query.
                     * Just pick the first entity from the table, so that the 
                     * column can be correctly resolved */
                    ecs_table_t *t = regs[var->id].range.table;
                    if (t) {
                        iter->subjects[i] = ecs_storage_first_t(
                            &t->data.entities, ecs_entity_t)[0];
                    } else {
                        /* Can happen if term is optional */
                        iter->subjects[i] = 0;
                    }
                }
            }
        }
    }

    /* Iterator expects column indices to start at 1 */
    iter->columns = rule_get_columns_frame(it, op->frame);
    for (i = 0; i < term_count; i ++) {
        ecs_assert(iter->subjects != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t subj = iter->subjects[i];
        int32_t c = ++ iter->columns[i];
        if (!subj) {
            subj = iter->terms[i].subj.entity;
            if (subj != EcsThis && subj != EcsAny) {
                iter->columns[i] = 0;
            }
        } else if (c) {
            iter->columns[i] = -1;
        }
    }

    /* Set iterator ids */
    for (i = 0; i < term_count; i ++) {
        const ecs_rule_term_vars_t *vars = &rule->term_vars[i];
        ecs_term_t *term = &rule->filter.terms[i];
        if (term->oper == EcsOptional || term->oper == EcsNot) {
            if (iter->columns[i] == 0) {
                iter->ids[i] = term->id;
                continue;
            }
        }

        ecs_id_t id = term->id;
        ecs_entity_t pred = 0;
        ecs_entity_t obj = 0;
        bool is_pair = ECS_HAS_ROLE(id, PAIR);

        if (!is_pair) {
            pred = id;
        } else {
            pred = ECS_PAIR_FIRST(id);
            obj = ECS_PAIR_SECOND(id);
        }

        if (vars->pred != -1) {
            pred = regs[vars->pred].entity;
        }
        if (vars->obj != -1) {
            ecs_assert(is_pair, ECS_INTERNAL_ERROR, NULL);
            obj = regs[vars->obj].entity;
        }

        if (!is_pair) {
            id = pred;
        } else {
            id = ecs_pair(pred, obj);
        }

        iter->ids[i] = id;
    }

    flecs_iter_populate_data(world, iter, table, offset, count, 
        iter->ptrs, iter->sizes);
}

static
bool is_control_flow(
    ecs_rule_op_t *op)
{
    switch(op->kind) {
    case EcsRuleSetJmp:
    case EcsRuleJump:
        return true;
    default:
        return false;
    }
}

static
void rule_iter_set_initial_state(
    ecs_iter_t *it,
    ecs_rule_iter_t *iter,
    const ecs_rule_t *rule)
{
    int32_t i;

    /* Make sure that if there are any terms with literal subjects, they're
     * initialized in the subjects array */
    const ecs_filter_t *filter = &rule->filter;
    int32_t term_count = filter->term_count;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *t = &filter->terms[i];
        ecs_term_id_t *subj = &t->subj;
        ecs_assert(subj->var == EcsVarIsVariable || subj->entity != EcsThis,
            ECS_INTERNAL_ERROR, NULL);

        if (subj->var == EcsVarIsEntity) {
            it->subjects[i] = subj->entity;
        }
    }

    /* Initialize registers of constrained variables */
    if (it->constrained_vars) {
        ecs_var_t *regs = get_register_frame(iter, 0);

        for (i = 0; i < it->variable_count; i ++) {
            if (ecs_iter_var_is_constrained(it, i)) {
                const ecs_rule_var_t *var = &rule->vars[i];
                ecs_assert(var->id == i, ECS_INTERNAL_ERROR, NULL);
                
                int32_t other_var = var->other;
                ecs_var_t *it_var = &it->variables[i];
                ecs_entity_t e = it_var->entity;

                if (e) {
                    ecs_assert(ecs_is_valid(it->world, e), 
                        ECS_INTERNAL_ERROR, NULL);
                    reg_set_entity(rule, regs, i, e);
                    if (other_var != -1) {
                        reg_set_entity(rule, regs, other_var, e);
                    }
                } else {
                    ecs_assert(it_var->range.table != NULL, 
                        ECS_INVALID_PARAMETER, NULL);
                    reg_set_range(rule, regs, i, &it_var->range);
                    if (other_var != -1) {
                        reg_set_range(rule, regs, other_var, &it_var->range);
                    }
                }
            }
        }
    }
}

bool ecs_rule_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_rule_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_rule_next_instanced(it));
error:
    return false;
}

/* Iterator next function. This evaluates the program until it reaches a Yield
 * operation, and returns the intermediate result(s) to the application. An
 * iterator can, depending on the program, either return a table, entity, or
 * just true/false, in case a rule doesn't contain the this variable. */
bool ecs_rule_next_instanced(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_rule_next, ECS_INVALID_PARAMETER, NULL);

    ecs_rule_iter_t *iter = &it->priv.iter.rule;
    const ecs_rule_t *rule = iter->rule;
    bool redo = iter->redo;
    int32_t last_frame = -1;
    bool first_time = !ECS_BIT_IS_SET(it->flags, EcsIterIsValid);

    ecs_poly_assert(rule, ecs_rule_t);

    /* Mark iterator as valid & ensure iterator resources are up to date */
    flecs_iter_validate(it);

    /* Can't iterate an iterator that's already depleted */
    ecs_check(iter->op != -1, ECS_INVALID_PARAMETER, NULL);

    /* If this is the first time the iterator is iterated, set initial state */
    if (first_time) {
        ecs_assert(redo == false, ECS_INTERNAL_ERROR, NULL);
        rule_iter_set_initial_state(it, iter, rule);
    }

    do {
        /* Evaluate an operation. The result of an operation determines the
         * flow of the program. If an operation returns true, the program 
         * continues to the operation pointed to by 'on_pass'. If the operation
         * returns false, the program continues to the operation pointed to by
         * 'on_fail'.
         *
         * In most scenarios, on_pass points to the next operation, and on_fail
         * points to the previous operation.
         *
         * When an operation fails, the previous operation will be invoked with
         * redo=true. This will cause the operation to continue its search from
         * where it left off. When the operation succeeds, the next operation
         * will be invoked with redo=false. This causes the operation to start
         * from the beginning, which is necessary since it just received a new
         * input. */
        int32_t op_index = iter->op;
        ecs_rule_op_t *op = &rule->operations[op_index];
        int32_t cur = op->frame;

        /* If this is not the first operation and is also not a control flow
         * operation, push a new frame on the stack for the next operation */
        if (!redo && !is_control_flow(op) && cur && cur != last_frame) {
            int32_t prev = cur - 1;
            push_registers(iter, prev, cur);
            push_columns(iter, prev, cur);
        }

        /* Dispatch the operation */
        bool result = eval_op(it, op, op_index, redo);
        iter->op = result ? op->on_pass : op->on_fail;

        /* If the current operation is yield, return results */
        if (op->kind == EcsRuleYield) {
            populate_iterator(rule, it, iter, op);
            iter->redo = true;
            return true;
        }

        /* If the current operation is a jump, goto stored label */
        if (op->kind == EcsRuleJump) {
            /* Label is stored in setjmp context */
            iter->op = iter->op_ctx[op->on_pass].is.setjmp.label;
        }

        /* If jumping backwards, it's a redo */
        redo = iter->op <= op_index;

        if (!is_control_flow(op)) {
            last_frame = op->frame;
        }
    } while (iter->op != -1);

    ecs_iter_fini(it);

error:
    return false;
}

#endif
