
#ifndef ECS_H
#define ECS_H

#if ECS_IMPL && defined _MSC_VER
#define ECS_EXPORT __declspec(dllexport)
#elif ECS_IMPL
#define ECS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define ECS_EXPORT __declspec(dllimport)
#else
#define ECS_EXPORT
#endif

#include <ecs/entity.h>
#include <ecs/hash.h>
#include <ecs/entity.h>
#include <ecs/components.h>

#ifdef __cplusplus
extern "C" {
#endif

void ecs_init(void);

void ecs_fini(void);

#ifdef __cplusplus
}
#endif

#endif
