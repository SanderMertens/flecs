#include <ctype.h>
#include <string.h>
#include "include/private/flecs.h"

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
    EcsSystemExprElemKind *elem_kind,
    EcsSystemExprOperKind *oper_kind,
    const char * *source)
{
    if (bptr[0] == '!') {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, bptr);
        }
        bptr ++;
    } else if (bptr[0] == '?') {
        *oper_kind = EcsOperOptional;
        if (!bptr[1]) {
            ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, bptr);
        }
        bptr ++;
    } else if (bptr[0] == '$') {
        *elem_kind = EcsFromSingleton;
        if (!bptr[1]) {
            ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, bptr);
        }
        bptr ++;
    }

    *source = NULL;

    char *dot = strchr(bptr, '.');
    if (dot) {
        if (!strncmp(bptr, "CONTAINER", dot - bptr)) {
            *elem_kind = EcsFromContainer;
        } else if (!strncmp(bptr, "SYSTEM", dot - bptr)) {
            *elem_kind = EcsFromSystem;
        } else if (!strncmp(bptr, "ENTITY", dot - bptr)) {
            /* default */
        } else if (!strncmp(bptr, "ID", dot - bptr)) {
            *elem_kind = EcsFromId;
        } else {
            *elem_kind = EcsFromEntity;
            *source = bptr;
        }
        bptr = dot + 1;

        if (!bptr[0]) {
            ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, bptr);
        }
    }

    return bptr;
}

static
EcsResult has_tables(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    const char *source_id,
    void *data)
{
    bool *needs_matching = data;
    if (elem_kind == EcsFromSelf || elem_kind == EcsFromContainer) {
        *needs_matching = true;
    }

    return EcsOk;
}

/* -- Private functions -- */

/* Does expression require that a system matches with tables */
bool ecs_needs_tables(
    EcsWorld *world,
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
EcsResult ecs_parse_component_expr(
    EcsWorld *world,
    const char *sig,
    ecs_parse_action action,
    void *ctx)
{
    size_t len = strlen(sig);
    const char *ptr;
    char ch, *bptr, *buffer = malloc(len + 1);
    ecs_assert(buffer != NULL, ECS_OUT_OF_MEMORY, NULL);

    bool complex_expr = false;
    EcsSystemExprElemKind elem_kind = EcsFromSelf;
    EcsSystemExprOperKind oper_kind = EcsOperAnd;
    const char *source;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (ch == ',' || ch == '|' || ch == '\0') {
            if (bptr == buffer) {
                ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
            }

            *bptr = '\0';
            bptr = buffer;

            source = NULL;

            if (complex_expr) {
                bptr = parse_complex_elem(bptr, &elem_kind, &oper_kind, &source);
                if (!bptr) {
                    ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
                }
            }

            if (oper_kind == EcsOperNot && ch == '|') {
                /* Cannot combine OR and NOT */
                ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
            }

            if (!strcmp(bptr, "0")) {
                if (oper_kind != EcsOperAnd) {
                    /* Cannot combine 0 component with operators */
                    ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
                }

                if (elem_kind != EcsFromSelf) {
                    /* Cannot get 0 component from anything other than entity */
                    ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
                }

                elem_kind = EcsFromId;
            }

            char *source_id = NULL;
            if (source) {
                char *dot = strchr(source, '.');
                source_id = malloc(dot - source + 1);
                ecs_assert(source_id != NULL, ECS_OUT_OF_MEMORY, NULL);

                strncpy(source_id, source, dot - source);
                source_id[dot - source] = '\0';
            }

            if (action(world, elem_kind, oper_kind, bptr, source_id, ctx) != EcsOk) {
                ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
            }

            if (source_id) {
                free(source_id);
            }

            complex_expr = false;
            elem_kind = EcsFromSelf;

            if (ch == '|') {
                if (elem_kind == EcsFromId) {
                    /* Cannot OR handles */
                    ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
                }
                oper_kind = EcsOperOr;
            } else {
                oper_kind = EcsOperAnd;
            }

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == '.' || ch == '!' || ch == '?' || ch == '$') {
                complex_expr = true;
            }
        }
    }

    free(buffer);
    return EcsOk;
}
