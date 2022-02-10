#include "../../private_api.h"

#ifdef FLECS_JSON

void json_next(
    ecs_strbuf_t *buf);

void json_literal(
    ecs_strbuf_t *buf,
    const char *value);

void json_number(
    ecs_strbuf_t *buf,
    double value);

void json_true(
    ecs_strbuf_t *buf);

void json_false(
    ecs_strbuf_t *buf);

void json_array_push(
    ecs_strbuf_t *buf);

void json_array_pop(
    ecs_strbuf_t *buf);

void json_object_push(
    ecs_strbuf_t *buf);

void json_object_pop(
    ecs_strbuf_t *buf);

void json_string(
    ecs_strbuf_t *buf,
    const char *value);

void json_member(
    ecs_strbuf_t *buf,
    const char *name);

void json_path(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e);

void json_label(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e);

void json_id(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id);

ecs_primitive_kind_t json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind);

#endif
