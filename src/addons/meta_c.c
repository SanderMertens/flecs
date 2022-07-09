#include "../private_api.h"

#ifdef FLECS_META_C

#include <ctype.h>

#define ECS_META_IDENTIFIER_LENGTH (256)

#define ecs_meta_error(ctx, ptr, ...)\
    ecs_parser_error((ctx)->name, (ctx)->desc, ptr - (ctx)->desc, __VA_ARGS__);

typedef char ecs_meta_token_t[ECS_META_IDENTIFIER_LENGTH];

typedef struct meta_parse_ctx_t {
    const char *name;
    const char *desc;
} meta_parse_ctx_t;

typedef struct meta_type_t {
    ecs_meta_token_t type;
    ecs_meta_token_t params;
    bool is_const;
    bool is_ptr;
} meta_type_t;

typedef struct meta_member_t {
    meta_type_t type;
    ecs_meta_token_t name;
    int64_t count;
    bool is_partial;
} meta_member_t;

typedef struct meta_constant_t {
    ecs_meta_token_t name;
    int64_t value;
    bool is_value_set;
} meta_constant_t;

typedef struct meta_params_t {
    meta_type_t key_type;
    meta_type_t type;
    int64_t count;
    bool is_key_value;
    bool is_fixed_size;
} meta_params_t;

static
const char* skip_scope(const char *ptr, meta_parse_ctx_t *ctx) {
    /* Keep track of which characters were used to open the scope */
    char stack[256];
    int32_t sp = 0;
    char ch;

    while ((ch = *ptr)) {
        if (ch == '(' || ch == '<') {
            stack[sp] = ch;

            sp ++;
            if (sp >= 256) {
                ecs_meta_error(ctx, ptr, "maximum level of nesting reached");
                goto error;
            }            
        } else if (ch == ')' || ch == '>') {
            sp --;
            if ((sp < 0) || (ch == '>' && stack[sp] != '<') || 
                (ch == ')' && stack[sp] != '(')) 
            {
                ecs_meta_error(ctx, ptr, "mismatching %c in identifier", ch);
                goto error;
            }
        }

        ptr ++;

        if (!sp) {
            break;
        }
    }

    return ptr;
error:
    return NULL;
}

static
const char* parse_c_digit(
    const char *ptr,
    int64_t *value_out)
{
    char token[24];
    ptr = ecs_parse_eol_and_whitespace(ptr);
    ptr = ecs_parse_digit(ptr, token);
    if (!ptr) {
        goto error;
    }

    *value_out = strtol(token, NULL, 0);

    return ecs_parse_eol_and_whitespace(ptr);
error:
    return NULL;
}

static
const char* parse_c_identifier(
    const char *ptr, 
    char *buff,
    char *params,
    meta_parse_ctx_t *ctx) 
{
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(buff != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    char *bptr = buff, ch;

    if (params) {
        params[0] = '\0';
    }

    /* Ignore whitespaces */
    ptr = ecs_parse_eol_and_whitespace(ptr);

    if (!isalpha(*ptr)) {
        ecs_meta_error(ctx, ptr, 
            "invalid identifier (starts with '%c')", *ptr);
        goto error;
    }

    while ((ch = *ptr) && !isspace(ch) && ch != ';' && ch != ',' && ch != ')' && ch != '>' && ch != '}') {
        /* Type definitions can contain macros or templates */
        if (ch == '(' || ch == '<') {
            if (!params) {
                ecs_meta_error(ctx, ptr, "unexpected %c", *ptr);
                goto error;
            }

            const char *end = skip_scope(ptr, ctx);
            ecs_os_strncpy(params, ptr, (ecs_size_t)(end - ptr));
            params[end - ptr] = '\0';

            ptr = end;
        } else {
            *bptr = ch;
            bptr ++;
            ptr ++;
        }
    }

    *bptr = '\0';

    if (!ch) {
        ecs_meta_error(ctx, ptr, "unexpected end of token");
        goto error;
    }

    return ptr;
error:
    return NULL;
}

static
const char * meta_open_scope(
    const char *ptr,
    meta_parse_ctx_t *ctx)    
{
    /* Skip initial whitespaces */
    ptr = ecs_parse_eol_and_whitespace(ptr);

    /* Is this the start of the type definition? */
    if (ctx->desc == ptr) {
        if (*ptr != '{') {
            ecs_meta_error(ctx, ptr, "missing '{' in struct definition");
            goto error; 
        }

        ptr ++;
        ptr = ecs_parse_eol_and_whitespace(ptr);
    }

    /* Is this the end of the type definition? */
    if (!*ptr) {
        ecs_meta_error(ctx, ptr, "missing '}' at end of struct definition");
        goto error;
    }   

    /* Is this the end of the type definition? */
    if (*ptr == '}') {
        ptr = ecs_parse_eol_and_whitespace(ptr + 1);
        if (*ptr) {
            ecs_meta_error(ctx, ptr, 
                "stray characters after struct definition");
            goto error;
        }
        return NULL;
    }

    return ptr;
error:
    return NULL;
}

static
const char* meta_parse_constant(
    const char *ptr,
    meta_constant_t *token,
    meta_parse_ctx_t *ctx)
{    
    ptr = meta_open_scope(ptr, ctx);
    if (!ptr) {
        return NULL;
    }

    token->is_value_set = false;

    /* Parse token, constant identifier */
    ptr = parse_c_identifier(ptr, token->name, NULL, ctx);
    if (!ptr) {
        return NULL;
    }

    ptr = ecs_parse_eol_and_whitespace(ptr);
    if (!ptr) {
        return NULL;
    }

    /* Explicit value assignment */
    if (*ptr == '=') {
        int64_t value = 0;
        ptr = parse_c_digit(ptr + 1, &value);
        token->value = value;
        token->is_value_set = true;
    }

    /* Expect a ',' or '}' */
    if (*ptr != ',' && *ptr != '}') {
        ecs_meta_error(ctx, ptr, "missing , after enum constant");
        goto error;
    }

    if (*ptr == ',') {
        return ptr + 1;
    } else {
        return ptr;
    }
error:
    return NULL;
}

static
const char* meta_parse_type(
    const char *ptr,
    meta_type_t *token,
    meta_parse_ctx_t *ctx)
{
    token->is_ptr = false;
    token->is_const = false;

    ptr = ecs_parse_eol_and_whitespace(ptr);

    /* Parse token, expect type identifier or ECS_PROPERTY */
    ptr = parse_c_identifier(ptr, token->type, token->params, ctx);
    if (!ptr) {
        goto error;
    }

    if (!strcmp(token->type, "ECS_PRIVATE")) {
        /* Members from this point are not stored in metadata */
        ptr += ecs_os_strlen(ptr);
        goto done;
    }

    /* If token is const, set const flag and continue parsing type */
    if (!strcmp(token->type, "const")) {
        token->is_const = true;

        /* Parse type after const */
        ptr = parse_c_identifier(ptr + 1, token->type, token->params, ctx);
    }

    /* Check if type is a pointer */
    ptr = ecs_parse_eol_and_whitespace(ptr);
    if (*ptr == '*') {
        token->is_ptr = true;
        ptr ++;
    }

done:
    return ptr;
error:
    return NULL;
}

static
const char* meta_parse_member(
    const char *ptr,
    meta_member_t *token,
    meta_parse_ctx_t *ctx)
{
    ptr = meta_open_scope(ptr, ctx);
    if (!ptr) {
        return NULL;
    }

    token->count = 1;
    token->is_partial = false;

    /* Parse member type */
    ptr = meta_parse_type(ptr, &token->type, ctx);
    if (!ptr) {
        token->is_partial = true;
        goto error;
    }

    /* Next token is the identifier */
    ptr = parse_c_identifier(ptr, token->name, NULL, ctx);
    if (!ptr) {
        goto error;
    }

    /* Skip whitespace between member and [ or ; */
    ptr = ecs_parse_eol_and_whitespace(ptr);

    /* Check if this is an array */
    char *array_start = strchr(token->name, '[');
    if (!array_start) {
        /* If the [ was separated by a space, it will not be parsed as part of
         * the name */
        if (*ptr == '[') {
            array_start = (char*)ptr; /* safe, will not be modified */
        }
    }

    if (array_start) {
        /* Check if the [ matches with a ] */
        char *array_end = strchr(array_start, ']');
        if (!array_end) {
            ecs_meta_error(ctx, ptr, "missing ']'");
            goto error;

        } else if (array_end - array_start == 0) {
            ecs_meta_error(ctx, ptr, "dynamic size arrays are not supported");
            goto error;
        }

        token->count = atoi(array_start + 1);

        if (array_start == ptr) {
            /* If [ was found after name, continue parsing after ] */
            ptr = array_end + 1;
        } else {
            /* If [ was fonud in name, replace it with 0 terminator */
            array_start[0] = '\0';
        }
    }

    /* Expect a ; */
    if (*ptr != ';') {
        ecs_meta_error(ctx, ptr, "missing ; after member declaration");
        goto error;
    }

    return ptr + 1;
error:
    return NULL;
}

static
int meta_parse_desc(
    const char *ptr,
    meta_params_t *token,
    meta_parse_ctx_t *ctx)
{
    token->is_key_value = false;
    token->is_fixed_size = false;

    ptr = ecs_parse_eol_and_whitespace(ptr);
    if (*ptr != '(' && *ptr != '<') {
        ecs_meta_error(ctx, ptr, 
            "expected '(' at start of collection definition");
        goto error;
    }

    ptr ++;

    /* Parse type identifier */
    ptr = meta_parse_type(ptr, &token->type, ctx);
    if (!ptr) {
        goto error;
    }

    ptr = ecs_parse_eol_and_whitespace(ptr);

    /* If next token is a ',' the first type was a key type */
    if (*ptr == ',') {
        ptr = ecs_parse_eol_and_whitespace(ptr + 1);
        
        if (isdigit(*ptr)) {
            int64_t value;
            ptr = parse_c_digit(ptr, &value);
            if (!ptr) {
                goto error;
            }

            token->count = value;
            token->is_fixed_size = true;
        } else {
            token->key_type = token->type;

            /* Parse element type */
            ptr = meta_parse_type(ptr, &token->type, ctx);
            ptr = ecs_parse_eol_and_whitespace(ptr);

            token->is_key_value = true;
        }
    }

    if (*ptr != ')' && *ptr != '>') {
        ecs_meta_error(ctx, ptr, 
            "expected ')' at end of collection definition");
        goto error;
    }

    return 0;
error:
    return -1;
}

static
ecs_entity_t meta_lookup(
    ecs_world_t *world,
    meta_type_t *token,
    const char *ptr,
    int64_t count,
    meta_parse_ctx_t *ctx);

static
ecs_entity_t meta_lookup_array(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *params_decl,
    meta_parse_ctx_t *ctx)
{
    meta_parse_ctx_t param_ctx = {
        .name = ctx->name,
        .desc = params_decl
    };

    meta_params_t params;
    if (meta_parse_desc(params_decl, &params, &param_ctx)) {
        goto error;
    }
    if (!params.is_fixed_size) {
        ecs_meta_error(ctx, params_decl, "missing size for array");
        goto error;
    }

    if (!params.count) {
        ecs_meta_error(ctx, params_decl, "invalid array size");
        goto error;
    }

    ecs_entity_t element_type = ecs_lookup_symbol(world, params.type.type, true);
    if (!element_type) {
        ecs_meta_error(ctx, params_decl, "unknown element type '%s'",
            params.type.type);
    }

    if (!e) {
        e = ecs_new_id(world);
    }

    ecs_check(params.count <= INT32_MAX, ECS_INVALID_PARAMETER, NULL);

    return ecs_set(world, e, EcsArray, { element_type, (int32_t)params.count });
error:
    return 0;
}

static
ecs_entity_t meta_lookup_vector(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *params_decl,
    meta_parse_ctx_t *ctx)
{
    meta_parse_ctx_t param_ctx = {
        .name = ctx->name,
        .desc = params_decl
    };

    meta_params_t params;
    if (meta_parse_desc(params_decl, &params, &param_ctx)) {
        goto error;
    }

    if (params.is_key_value) {
        ecs_meta_error(ctx, params_decl,
            "unexpected key value parameters for vector");
        goto error;
    }

    ecs_entity_t element_type = meta_lookup(
        world, &params.type, params_decl, 1, &param_ctx);

    if (!e) {
        e = ecs_new_id(world);
    }

    return ecs_set(world, e, EcsVector, { element_type });
error:
    return 0;
}

static
ecs_entity_t meta_lookup_bitmask(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *params_decl,
    meta_parse_ctx_t *ctx)
{
    (void)e;

    meta_parse_ctx_t param_ctx = {
        .name = ctx->name,
        .desc = params_decl
    };

    meta_params_t params;
    if (meta_parse_desc(params_decl, &params, &param_ctx)) {
        goto error;
    }

    if (params.is_key_value) {
        ecs_meta_error(ctx, params_decl,
            "unexpected key value parameters for bitmask");
        goto error;
    }

    if (params.is_fixed_size) {
        ecs_meta_error(ctx, params_decl,
            "unexpected size for bitmask");
        goto error;
    }

    ecs_entity_t bitmask_type = meta_lookup(
        world, &params.type, params_decl, 1, &param_ctx);
    ecs_check(bitmask_type != 0, ECS_INVALID_PARAMETER, NULL);

#ifndef FLECS_NDEBUG
    /* Make sure this is a bitmask type */
    const EcsMetaType *type_ptr = ecs_get(world, bitmask_type, EcsMetaType);
    ecs_check(type_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(type_ptr->kind == EcsBitmaskType, ECS_INVALID_PARAMETER, NULL);
#endif

    return bitmask_type;
error:
    return 0;
}

static
ecs_entity_t meta_lookup(
    ecs_world_t *world,
    meta_type_t *token,
    const char *ptr,
    int64_t count,
    meta_parse_ctx_t *ctx)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(token != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *typename = token->type;
    ecs_entity_t type = 0;

    /* Parse vector type */
    if (!token->is_ptr) {
        if (!ecs_os_strcmp(typename, "ecs_array")) {
            type = meta_lookup_array(world, 0, token->params, ctx);

        } else if (!ecs_os_strcmp(typename, "ecs_vector") || 
                !ecs_os_strcmp(typename, "flecs::vector")) 
        {
            type = meta_lookup_vector(world, 0, token->params, ctx);

        } else if (!ecs_os_strcmp(typename, "flecs::bitmask")) {
            type = meta_lookup_bitmask(world, 0, token->params, ctx);

        } else if (!ecs_os_strcmp(typename, "flecs::byte")) {
            type = ecs_id(ecs_byte_t);

        } else if (!ecs_os_strcmp(typename, "char")) {
            type = ecs_id(ecs_char_t);

        } else if (!ecs_os_strcmp(typename, "bool") || 
                !ecs_os_strcmp(typename, "_Bool")) 
        {
            type = ecs_id(ecs_bool_t);

        } else if (!ecs_os_strcmp(typename, "int8_t")) {
            type = ecs_id(ecs_i8_t);
        } else if (!ecs_os_strcmp(typename, "int16_t")) {
            type = ecs_id(ecs_i16_t);
        } else if (!ecs_os_strcmp(typename, "int32_t")) {
            type = ecs_id(ecs_i32_t);
        } else if (!ecs_os_strcmp(typename, "int64_t")) {
            type = ecs_id(ecs_i64_t);

        } else if (!ecs_os_strcmp(typename, "uint8_t")) {
            type = ecs_id(ecs_u8_t);
        } else if (!ecs_os_strcmp(typename, "uint16_t")) {
            type = ecs_id(ecs_u16_t);
        } else if (!ecs_os_strcmp(typename, "uint32_t")) {
            type = ecs_id(ecs_u32_t);
        } else if (!ecs_os_strcmp(typename, "uint64_t")) {
            type = ecs_id(ecs_u64_t);

        } else if (!ecs_os_strcmp(typename, "float")) {
            type = ecs_id(ecs_f32_t);
        } else if (!ecs_os_strcmp(typename, "double")) {
            type = ecs_id(ecs_f64_t);

        } else if (!ecs_os_strcmp(typename, "ecs_entity_t")) {
            type = ecs_id(ecs_entity_t);

        } else if (!ecs_os_strcmp(typename, "char*")) {
            type = ecs_id(ecs_string_t);
        } else {
            type = ecs_lookup_symbol(world, typename, true);
        }
    } else {
        if (!ecs_os_strcmp(typename, "char")) {
            typename = "flecs.meta.string";
        } else
        if (token->is_ptr) {
            typename = "flecs.meta.uptr";
        } else
        if (!ecs_os_strcmp(typename, "char*") || 
            !ecs_os_strcmp(typename, "flecs::string")) 
        {
            typename = "flecs.meta.string";
        }

        type = ecs_lookup_symbol(world, typename, true);
    }

    if (count != 1) {
        ecs_check(count <= INT32_MAX, ECS_INVALID_PARAMETER, NULL);

        type = ecs_set(world, 0, EcsArray, {type, (int32_t)count});
    }

    if (!type) {
        ecs_meta_error(ctx, ptr, "unknown type '%s'", typename);
        goto error;
    }

    return type;
error:
    return 0;
}

static
int meta_parse_struct(
    ecs_world_t *world,
    ecs_entity_t t,
    const char *desc)
{
    const char *ptr = desc;
    const char *name = ecs_get_name(world, t);

    meta_member_t token;
    meta_parse_ctx_t ctx = {
        .name = name,
        .desc = ptr
    };

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    while ((ptr = meta_parse_member(ptr, &token, &ctx)) && ptr[0]) {
        ecs_entity_t m = ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = token.name
        });

        ecs_entity_t type = meta_lookup(
            world, &token.type, ptr, 1, &ctx);
        if (!type) {
            goto error;
        }

        ecs_set(world, m, EcsMember, {
            .type = type, 
            .count = (ecs_size_t)token.count
        });
    }

    ecs_set_scope(world, old_scope);

    return 0;
error:
    return -1;
}

static
int meta_parse_constants(
    ecs_world_t *world,
    ecs_entity_t t,
    const char *desc,
    bool is_bitmask)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *ptr = desc;
    const char *name = ecs_get_name(world, t);

    meta_parse_ctx_t ctx = {
        .name = name,
        .desc = ptr
    };

    meta_constant_t token;
    int64_t last_value = 0;

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    while ((ptr = meta_parse_constant(ptr, &token, &ctx))) {
        if (token.is_value_set) {
            last_value = token.value;
        } else if (is_bitmask) {
            ecs_meta_error(&ctx, ptr,
                "bitmask requires explicit value assignment");
            goto error;
        }

        ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = token.name
        });

        if (!is_bitmask) {
            ecs_set_pair_object(world, c, EcsConstant, ecs_i32_t, 
                {(ecs_i32_t)last_value});
        } else {
            ecs_set_pair_object(world, c, EcsConstant, ecs_u32_t, 
                {(ecs_u32_t)last_value});
        }

        last_value ++;
    }

    ecs_set_scope(world, old_scope);

    return 0;
error:
    return -1;
}

static
int meta_parse_enum(
    ecs_world_t *world,
    ecs_entity_t t,
    const char *desc)
{
    ecs_add(world, t, EcsEnum);
    return meta_parse_constants(world, t, desc, false);
}

static
int meta_parse_bitmask(
    ecs_world_t *world,
    ecs_entity_t t,
    const char *desc)
{
    ecs_add(world, t, EcsBitmask);
    return meta_parse_constants(world, t, desc, true);
}

int ecs_meta_from_desc(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_kind_t kind,
    const char *desc)
{
    switch(kind) {
    case EcsStructType:
        if (meta_parse_struct(world, component, desc)) {
            goto error;
        }
        break;
    case EcsEnumType:
        if (meta_parse_enum(world, component, desc)) {
            goto error;
        }
        break;
    case EcsBitmaskType:
        if (meta_parse_bitmask(world, component, desc)) {
            goto error;
        }
        break;
    default:
        break;
    }

    return 0;
error:
    return -1;
}

#endif
