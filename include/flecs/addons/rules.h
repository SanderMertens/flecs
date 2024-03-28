/**
 * @file addons/rules.h
 * @brief Rule query engine addon.
 *
 * Rules are advanced queries that in addition to the capabilities of regular
 * queries and filters have the following features:
 *
 * - query for all components of an entity (vs. all entities for a component)
 * - query for all relationship pairs of an entity
 * - support for query variables that are resolved at evaluation time
 * - automatic traversal of transitive relationships
 */

#ifdef FLECS_RULES

/**
 * @defgroup c_addons_rules Rules
 * @ingroup c_addons
 * Rules are an advanced query engine for matching against entity graphs.
 *
 * @{
 */

#ifndef FLECS_RULES_H
#define FLECS_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

/** Create a rule.
 * A rule accepts the same descriptor as a filter, but has the additional
 * ability to use query variables.
 *
 * Query variables can be used to constrain wildcards across multiple terms to
 * the same entity. Regular ECS queries do this in a limited form, as querying
 * for Position, Velocity only returns entities that have both components.
 *
 * Query variables expand this to constrain entities that are resolved while the
 * query is being matched. Consider a query for all entities and the mission
 * they are on:
 *   (Mission, *)
 *
 * If an entity is on multiple missions, the wildcard will match it multiple
 * times. Now say we want to only list combat missions. Naively we could try:
 *   (Mission, *), CombatMission(*)
 *
 * But this doesn't work, as term 1 returns entities with missions, and term 2
 * returns all combat missions for all entities. Query variables make it
 * possible to apply CombatMission to the found mission:
 *   (Mission, $M), CombatMission($M)
 *
 * By using the same variable ('M') we ensure that CombatMission is applied to
 * the mission found in the current result.
 *
 * Variables can be used in each part of the term (predicate, subject, object).
 * This is a valid query:
 *   Likes($X, $Y), Likes($Y, $X)
 *
 * This is also a valid query:
 *   _Component, Serializable(_Component)
 *
 * In the query expression syntax, variables are prefixed with a $. When using
 * the descriptor, specify the variable kind:
 *   desc.terms[0].second = { .name = "X", .var = EcsVarIsVariable }
 *
 * Different terms with the same variable name are automatically correlated by
 * the query engine.
 * 
 * A query needs to be explicitly deleted with ecs_query_fini.
 * 
 * @param world The world.
 * @param desc The descriptor (see ecs_query_desc_t)
 * @return The rule.
 */
FLECS_API
ecs_query_t* ecs_query_init(
    ecs_world_t *world,
    const ecs_query_desc_t *desc);

/** Delete a rule.
 *
 * @param rule The rule.
 */
FLECS_API
void ecs_query_fini(
    ecs_query_t *rule);

/** Return number of variables in rule.
 *
 * @param rule The rule.
 * @return The number of variables/
 */
FLECS_API
int32_t ecs_query_var_count(
    const ecs_query_t *rule);

/** Find variable index.
 * This operation looks up the index of a variable in the rule. This index can
 * be used in operations like ecs_iter_set_var() and ecs_iter_get_var().
 *
 * @param rule The rule.
 * @param name The variable name.
 * @return The variable index.
 */
FLECS_API
int32_t ecs_query_find_var(
    const ecs_query_t *query,
    const char *name);    

/** Get variable name.
 * This operation returns the variable name for an index.
 *
 * @param rule The rule.
 * @param var_id The variable index.
 */
FLECS_API
const char* ecs_query_var_name(
    const ecs_query_t *query,
    int32_t var_id);

/** Test if variable is an entity.
 * Internally the rule engine has entity variables and table variables. When
 * iterating through rule variables (by using ecs_query_variable_count) only
 * the values for entity variables are accessible. This operation enables an
 * application to check if a variable is an entity variable.
 *
 * @param rule The rule.
 * @param var_id The variable id.
 */
FLECS_API
bool ecs_query_var_is_entity(
    const ecs_query_t *query,
    int32_t var_id);  

/** Iterate a rule.
 * Note that rule iterators may allocate memory, and that unless the iterator
 * is iterated until completion, it may still hold resources. When stopping
 * iteration before ecs_query_next has returned false, use ecs_iter_fini to
 * cleanup any remaining resources.
 *
 * @param world The world.
 * @param rule The rule.
 * @return An iterator.
 */
FLECS_API
ecs_iter_t ecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *rule);

/** Progress rule iterator.
 *
 * @param it The iterator.
 */
FLECS_API
bool ecs_query_next(
    ecs_iter_t *it);

/** Progress instanced iterator.
 * Should not be called unless you know what you're doing :-)
 *
 * @param it The iterator.
 */
FLECS_API
bool ecs_query_next_instanced(
    ecs_iter_t *it);

/** Returns true if rule matches with entity. */
FLECS_API
bool ecs_query_has(
    ecs_query_t *query,
    ecs_entity_t entity,
    ecs_iter_t *it);

/** Returns true if rule matches with table. */
FLECS_API
bool ecs_query_has_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_iter_t *it);

/** Returns true if rule matches with table. */
FLECS_API
bool ecs_query_has_range(
    ecs_query_t *query,
    ecs_table_range_t *range,
    ecs_iter_t *it);

/** Returns how often a match event happened for a cached query. 
 * This operation can be used to determine whether the query cache has been 
 * updated with new tables.
 */
FLECS_API
int32_t ecs_query_match_count(
    const ecs_query_t *query);

/** Convert rule to a string.
 * This will convert the rule program to a string which can aid in debugging
 * the behavior of a rule.
 *
 * The returned string must be freed with ecs_os_free().
 *
 * @param rule The rule.
 * @return The string
 */
FLECS_API
char* ecs_query_plan(
    const ecs_query_t *rule);

/** Convert rule to string with profile.
 * To use this you must set the EcsIterProfile flag on an iterator before
 * starting iteration:
 *   it.flags |= EcsIterProfile
 *
 * @param rule The rule.
 * @return The string
 */
FLECS_API
char* ecs_query_str_w_profile(
    const ecs_query_t *query,
    const ecs_iter_t *it);

/** Populate variables from key-value string.
 * Convenience function to set rule variables from a key-value string separated
 * by comma's. The string must have the following format:
 *   var_a: value, var_b: value
 *
 * The key-value list may optionally be enclosed in parenthesis.
 *
 * @param rule The rule.
 * @param it The iterator for which to set the variables.
 * @param expr The key-value expression.
 */
FLECS_API
const char* ecs_query_parse_vars(
    ecs_query_t *query,
    ecs_iter_t *it,
    const char *expr);

#ifdef __cplusplus
}
#endif

#endif // FLECS_RULES_H

/** @} */

#endif // FLECS_RULES
