#include "../../private_api.h"

#ifdef FLECS_JSON

void flecs_json_next(
    ecs_strbuf_t *buf);

void flecs_json_literal(
    ecs_strbuf_t *buf,
    const char *value);

void flecs_json_number(
    ecs_strbuf_t *buf,
    double value);

void flecs_json_true(
    ecs_strbuf_t *buf);

void flecs_json_false(
    ecs_strbuf_t *buf);

void flecs_json_bool(
    ecs_strbuf_t *buf,
    bool value);

void flecs_json_array_push(
    ecs_strbuf_t *buf);

void flecs_json_array_pop(
    ecs_strbuf_t *buf);

void flecs_json_object_push(
    ecs_strbuf_t *buf);

void flecs_json_object_pop(
    ecs_strbuf_t *buf);

void flecs_json_string(
    ecs_strbuf_t *buf,
    const char *value);

void flecs_json_member(
    ecs_strbuf_t *buf,
    const char *name);

void flecs_json_path(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_json_label(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_json_color(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_json_id(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id);

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind);

#endif
