
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

#ifdef __cplusplus
extern "C" {
#endif

#include <ecs/entity.h>
#include <ecs/hash.h>

#ifdef __cplusplus
}
#endif

#endif
