
/**
 * @file rules.h
 * @brief Rules addon.
 *
 * The rules addon implements a constraint-solver based query language.
 */

#ifdef FLECS_RULES

#ifndef FLECS_RULES_H
#define FLECS_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
ecs_rule_t* ecs_rule_init(
    ecs_world_t *world,
    ecs_filter_desc_t *desc);

FLECS_API
void ecs_rule_fini(
    ecs_rule_t *rule);

FLECS_API
int32_t ecs_rule_term_count(
    const ecs_rule_t *rule);

FLECS_API
int32_t ecs_rule_variable_count(
    const ecs_rule_t *rule);

FLECS_API
int32_t ecs_rule_find_variable(
    const ecs_rule_t *rule,
    const char *name);    

FLECS_API
const char* ecs_rule_variable_name(
    const ecs_rule_t *rule,
    int32_t var_id);

FLECS_API
ecs_entity_t ecs_rule_variable(
    ecs_iter_t *it,
    int32_t var_id);

FLECS_API
bool ecs_rule_variable_is_entity(
    const ecs_rule_t *rule,
    int32_t var_id);  

FLECS_API
ecs_iter_t ecs_rule_iter(
    const ecs_rule_t *rule);

FLECS_API
void ecs_rule_iter_free(
    ecs_iter_t *iter);

FLECS_API
bool ecs_rule_next(
    ecs_iter_t *it);

FLECS_API
char* ecs_rule_str(
    ecs_rule_t *rule);

#ifdef __cplusplus
}
#endif

#endif // FLECS_RULES_H

#endif // FLECS_RULES
