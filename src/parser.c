#include <ctype.h>
#include <string.h>
#include "include/private/reflecs.h"

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

static
char* ecs_parse_complex_elem(
    char *bptr,
    EcsSystemExprElemKind *elem_kind,
    EcsSystemExprOperKind *oper_kind)
{
    if (bptr[0] == '!') {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            return NULL;
        }
    }

    char *dot = strchr(bptr, '.');
    if (dot) {
        if (!strncmp(bptr, "COMPONENT", dot - bptr)) {
            *elem_kind = EcsFromComponent;
        } else if (!strncmp(bptr, "ENTITY", dot - bptr)) {
            /* default */
        } else {
            return NULL;
        }
        bptr = dot + 1;

        if (!bptr[0]) {
            return NULL;
        }
    }

    return bptr;
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
    bool complex_expr = false;
    EcsSystemExprElemKind elem_kind = EcsFromEntity;
    EcsSystemExprOperKind oper_kind = EcsOperAnd;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (ch == ',' || ch == '|' || ch == '\0') {
            if (bptr == buffer) {
                goto error;
            }

            *bptr = '\0';
            bptr = buffer;

            if (complex_expr) {
                bptr = ecs_parse_complex_elem(bptr, &elem_kind, &oper_kind);
                if (!bptr) {
                    goto error;
                }
            }

            if (oper_kind != EcsOperAnd && ch == '|') {
                /* Cannot combine OR and NOT */
                goto error;
            }

            if (action(world, elem_kind, oper_kind, bptr, ctx) != EcsOk) {
                goto error;
            }

            complex_expr = false;
            elem_kind = EcsFromEntity;
            oper_kind = EcsOperAnd;
            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == '.' || ch == '!') {
                complex_expr = true;
            }
        }
    }

    free(buffer);
    return EcsOk;
error:
    free(buffer);
    return EcsError;
}
