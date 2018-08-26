#ifndef ECS_PLATFORM_H
#define ECS_PLATFORM_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ecs/log.h>
#include <ecs/export.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
/* Boolean definitions (compatible with C++ and C99 stdbool) */
#undef bool
#undef true
#undef false
#define bool char
#define false 0
#define true !false
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef NULL
#define NULL (0x0)
#endif

#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#define ECS_ALIGNMENT(t) ((uintptr_t)(&((struct {char dummy;t alignMember;}*)(void *)0)->alignMember))
#define ECS_ALIGN(size, alignment) (((((size) - 1) / (alignment)) + 1) * (alignment))
#define ECS_ISNAN(x) ((x) != (x))

#ifdef __cplusplus
}
#endif

#endif
