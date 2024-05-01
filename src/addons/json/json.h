/**
 * @file json/json.h
 * @brief Internal functions for JSON addon.
 */

#ifndef FLECS_JSON_PRIVATE_H
#define FLECS_JSON_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_JSON

/* Deserialize from JSON */
typedef enum ecs_json_token_t {
    JsonObjectOpen,
    JsonObjectClose,
    JsonArrayOpen,
    JsonArrayClose,
    JsonColon,
    JsonComma,
    JsonNumber,
    JsonString,
    JsonBoolean,
    JsonTrue,
    JsonFalse,
    JsonNull,
    JsonLargeInt,
    JsonLargeString,
    JsonInvalid
} ecs_json_token_t;

typedef struct ecs_json_value_ser_ctx_t {
    ecs_entity_t type;
    const EcsMetaTypeSerialized *ser;
    char *id_label;
    bool initialized;
} ecs_json_value_ser_ctx_t;

/* Cached data for serializer */
typedef struct ecs_json_ser_ctx_t {
    ecs_id_record_t *idr_doc_name;
    ecs_id_record_t *idr_doc_color;
    ecs_json_value_ser_ctx_t value_ctx[64];
} ecs_json_ser_ctx_t;

const char* flecs_json_parse(
    const char *json,
    ecs_json_token_t *token_kind,
    char *token);

const char* flecs_json_parse_large_string(
    const char *json,
    ecs_strbuf_t *buf);

const char* flecs_json_expect(
    const char *json,
    ecs_json_token_t token_kind,
    char *token,
    const ecs_from_json_desc_t *desc);

const char* flecs_json_expect_string(
    const char *json,
    char *token,
    char **out,
    const ecs_from_json_desc_t *desc);

const char* flecs_json_expect_member(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc);

const char* flecs_json_expect_member_name(
    const char *json,
    char *token,
    const char *member_name,
    const ecs_from_json_desc_t *desc);

const char* flecs_json_skip_object(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc);

const char* flecs_json_skip_array(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc);

/* Serialize to JSON */
void flecs_json_next(
    ecs_strbuf_t *buf);

void flecs_json_number(
    ecs_strbuf_t *buf,
    double value);

void flecs_json_u32(
    ecs_strbuf_t *buf,
    uint32_t value);

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

void flecs_json_string_escape(
    ecs_strbuf_t *buf,
    const char *value);

void flecs_json_member(
    ecs_strbuf_t *buf,
    const char *name);

void flecs_json_membern(
    ecs_strbuf_t *buf,
    const char *name,
    int32_t name_len);

#define flecs_json_memberl(buf, name)\
    flecs_json_membern(buf, name, sizeof(name) - 1)

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

void flecs_json_id_member(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id);

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind);

bool flecs_json_serialize_get_field_ctx(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    int32_t f,
    ecs_json_ser_ctx_t *ser_ctx);

int flecs_json_serialize_iter_result_rows(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx);

bool flecs_json_serialize_iter_result_is_set(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf);

bool flecs_json_skip_variable(
    const char *name);

void flecs_json_serialize_field(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    const ecs_filter_t *q,
    int field,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ctx);

void flecs_json_serialize_query(
    const ecs_world_t *world,
    const ecs_filter_t *q,
    ecs_strbuf_t *buf);

int flecs_json_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *ser,
    const void *base,
    ecs_strbuf_t *str);

#endif

#endif /* FLECS_JSON_PRIVATE_H */
