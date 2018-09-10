#ifndef REFLECS_PLATFORM_H
#define REFLECS_PLATFORM_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <alloca.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
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
#define REFLECS_ALIGNMENT(t) ((uintptr_t)(&((struct {char dummy;t alignMember;}*)(void *)0)->alignMember))
#define REFLECS_ALIGN(size, alignment) (((((size) - 1) / (alignment)) + 1) * (alignment))
#define REFLECS_ISNAN(x) ((x) != (x))

typedef int (*EcsComparator)(void* a, void *b);

#ifdef __cplusplus
}
#endif

#endif
