#ifndef FLECS_SCRIPT_QUERY_H
#define FLECS_SCRIPT_QUERY_H

extern ECS_COMPONENT_DECLARE(EcsScriptQuery);

int flecs_script_query_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_query_node_t *node);

int flecs_script_eval_query(
    ecs_script_eval_visitor_t *v,
    ecs_script_query_node_t *node);

void flecs_script_query_to_bool(
    void *dst,
    const void *src);

bool flecs_script_query_iter(
    const EcsScriptQuery *value,
    ecs_iter_t *it);

int32_t flecs_script_query_data_fields(
    const ecs_query_t *query,
    int8_t *fields);

void flecs_script_query_import(
    ecs_world_t *world);

#endif
