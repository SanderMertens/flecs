
/**
 * @file addons/serialize.h
 * @brief Visitor-api addon.
 *
 * Implements visitor pattern to allow parsing of flecs entity-component
 * graphs.
 *
 * Used by the json parser addon to serialize to json.
 */

#ifdef FLECS_SERIALIZE

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

#ifndef FLECS_SERIALIZE_H
#define FLECS_SERIALIZE_H

/**
 * @defgroup c_addons_serialize Serialize
 * @ingroup c_addons
 * Functions for user-defined serialization.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Visitor Seriailzation API
 */
typedef struct ecs_visitor_desc_t {
    /** Here Be Visitor Functions
     * If the visit has a return, it has some form of flow control.
     */

    /** visit primitives */
    void (*visit_char)(const ecs_char_t value, void* user_data);
    void (*visit_u8)(const ecs_u8_t value, void* user_data);
    void (*visit_u16)(const ecs_u16_t value, void* user_data);
    void (*visit_u32)(const ecs_u32_t value, void* user_data);
    void (*visit_u64)(const ecs_u64_t value, void* user_data);
    void (*visit_i8)(const ecs_i8_t value, void* user_data);
    void (*visit_i16)(const ecs_i16_t value, void* user_data);
    void (*visit_i32)(const ecs_i32_t value, void* user_data);
    void (*visit_i64)(const ecs_i64_t value, void* user_data);
    void (*visit_f32)(const ecs_f32_t value, void* user_data);
    void (*visit_f64)(const ecs_f64_t value, void* user_data);
    void (*visit_bool)(const ecs_bool_t value, void* user_data);
    void (*visit_byte)(const ecs_byte_t value, void* user_data);
    void (*visit_uptr)(const ecs_uptr_t value, void* user_data);
    void (*visit_iptr)(const ecs_iptr_t value, void* user_data);
    void (*visit_string)(const char* value, void* user_data);
    void (*visit_entity)(const ecs_world_t*, const ecs_entity_t value, void* user_data);
    void (*visit_id)(const ecs_world_t*, const ecs_id_t value, void* user_data);

    void (*visit_member)(const char* member_name, void* user_data);

    void (*visit_enum)(uint32_t value, const char* constant_name, void* user_data);
    struct {
        int (*enter)(uint32_t initial_value, void* user_data);
        void (*value)(uint32_t value, const char* constant_name, void* user_data);
        void (*exit)(uint32_t values_visited, void* user_data);
    } visit_bitmask;
    struct {
        void (*enter)(uint32_t size, void* user_data);
        void (*next_value)(uint32_t index, void* user_data);
        void (*exit)(void* user_data);
    } visit_array;
    struct {
        void (*enter)(void* user_data);
        void (*exit)(void* user_data);
    } visit_struct;

    struct {
      void (*enter)(void* user_data);
      // void (*next_value)(void* ptr, void* user_data);
      void (*exit)(void* user_data);
    } visit_iter_result;

    void* (*exit)(void* user_data);
    void* (*error)(void* user_data);

    /** User Data that is handed out to each visit.
     * Ex: JSON serialization passes a string buffer
     */
    void* user_data;
    
    /** Used to indicate an error has occured while visiting
     */
    bool _error;
} ecs_visitor_desc_t;


int ecs_ser_ptr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_visitor_desc_t *visitor_desc);

int ecs_ser_array(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_visitor_desc_t *visitor_desc);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif