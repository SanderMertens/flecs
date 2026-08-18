#ifndef FLECS_SCRIPT_DEPS_H
#define FLECS_SCRIPT_DEPS_H

int flecs_script_analyze_dependencies(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

int flecs_script_analyze_template_dependencies(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template);

#endif
