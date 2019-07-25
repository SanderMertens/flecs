#include "flecs_private.h"

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

/** Parse element with a dot-separated qualifier ('CONTAINER.Foo') */
static
char* parse_complex_elem(
    char *bptr,
    ecs_system_expr_elem_kind_t *elem_kind,
    ecs_system_expr_oper_kind_t *oper_kind,
    const char * *source)
{
    if (bptr[0] == '!') {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            ecs_abort(ECS_INVALID_EXPRESSION, bptr);
        }
        bptr ++;
    } else if (bptr[0] == '?') {
        *oper_kind = EcsOperOptional;
        if (!bptr[1]) {
            ecs_abort(ECS_INVALID_EXPRESSION, bptr);
        }
        bptr ++;
    }

    *source = NULL;

    char *dot = strchr(bptr, '.');
    if (dot) {
        if (bptr == dot) {
            *elem_kind = EcsFromEmpty;
        } else if (!strncmp(bptr, "CONTAINER", dot - bptr)) {
            *elem_kind = EcsFromContainer;
        } else if (!strncmp(bptr, "SYSTEM", dot - bptr)) {
            *elem_kind = EcsFromSystem;
        } else if (!strncmp(bptr, "SELF", dot - bptr)) {
            /* default */
        } else if (!strncmp(bptr, "OWNED", dot - bptr)) {
            *elem_kind = EcsFromOwned;
        } else if (!strncmp(bptr, "SHARED", dot - bptr)) {
            *elem_kind = EcsFromShared;
        } else if (!strncmp(bptr, "CASCADE", dot - bptr)) {
            *elem_kind = EcsCascade;   
        } else {
            *elem_kind = EcsFromEntity;
            *source = bptr;
        }
        
        bptr = dot + 1;

        if (!bptr[0]) {
            return NULL;
        }
    }

    return bptr;
}

static
int has_tables(
    ecs_world_t *world,
    ecs_system_expr_elem_kind_t elem_kind,
    ecs_system_expr_oper_kind_t oper_kind,
    const char *component_id,
    const char *source_id,
    void *data)
{
    (void)world;
    (void)oper_kind;
    (void)component_id;
    (void)source_id;
    
    bool *needs_matching = data;
    if (elem_kind == EcsFromSelf || elem_kind == EcsFromContainer) {
        *needs_matching = true;
    }

    return 0;
}

/* -- Private functions -- */

/* Does expression require that a system matches with tables */
bool ecs_needs_tables(
    ecs_world_t *world,
    const char *signature)
{
    bool needs_matching = false;
    ecs_parse_component_expr(world, signature, has_tables, &needs_matching);
    return needs_matching;
}

/** Count components in a signature */
uint32_t ecs_columns_count(
    const char *sig)
{
    const char *ptr = sig;
    uint32_t count = 1;

    while ((ptr = strchr(ptr + 1, ','))) {
        count ++;
    }

    return count;
}

/** Parse component expression */
int ecs_parse_component_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    void *ctx)
{
    size_t len = strlen(sig);
    const char *ptr;
    char ch, *bptr, *buffer = ecs_os_malloc(len + 1);
    ecs_assert(buffer != NULL, ECS_OUT_OF_MEMORY, NULL);

    bool complex_expr = false;
    bool prev_is_0 = false;
    ecs_system_expr_elem_kind_t elem_kind = EcsFromSelf;
    ecs_system_expr_oper_kind_t oper_kind = EcsOperAnd;
    const char *source;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (prev_is_0) {
            /* 0 can only apppear by itself */
            ecs_abort(ECS_INVALID_SIGNATURE, sig);
        }

        if (ch == ',' || ch == '|' || ch == '\0') {
            if (bptr == buffer) {
                ecs_abort(ECS_INVALID_SIGNATURE, sig);
            }

            *bptr = '\0';
            bptr = buffer;

            source = NULL;

            if (complex_expr) {
                ecs_system_expr_oper_kind_t prev_oper_kind = oper_kind;
                bptr = parse_complex_elem(bptr, &elem_kind, &oper_kind, &source);
                if (!bptr) {
                    ecs_abort(ECS_INVALID_EXPRESSION, sig);
                }

                if (oper_kind == EcsOperNot && prev_oper_kind == EcsOperOr) {
                    ecs_abort(ECS_INVALID_EXPRESSION, sig);
                }
            }

           if (oper_kind == EcsOperOr) {
                if (elem_kind == EcsFromEmpty) {
                    /* Cannot OR handles */
                    ecs_abort(ECS_INVALID_EXPRESSION, sig);
                }
            }            

            if (!strcmp(bptr, "0")) {
                if (bptr != buffer) {
                    /* 0 can only appear by itself */
                    ecs_abort(ECS_INVALID_EXPRESSION, sig);
                }

                elem_kind = EcsFromEmpty;
                prev_is_0 = true;
            }

            char *source_id = NULL;
            if (source) {
                char *dot = strchr(source, '.');
                source_id = ecs_os_malloc(dot - source + 1);
                ecs_assert(source_id != NULL, ECS_OUT_OF_MEMORY, NULL);

                strncpy(source_id, source, dot - source);
                source_id[dot - source] = '\0';
            }

            int ret;
            if ((ret = action(
                world, elem_kind, oper_kind, bptr, source_id, ctx))) 
            {
                ecs_abort(ret, sig);
            }

            if (source_id) {
                ecs_os_free(source_id);
            }

            complex_expr = false;
            elem_kind = EcsFromSelf;

            if (ch == '|') {
                oper_kind = EcsOperOr;
            } else {
                oper_kind = EcsOperAnd;
            }

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == '.' || ch == '!' || ch == '?') {
                complex_expr = true;
            }
        }
    }

    ecs_os_free(buffer);
    return 0;
}
