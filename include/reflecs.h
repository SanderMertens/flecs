
#ifndef REFLECS_H
#define REFLECS_H

#if REFLECS_IMPL && defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllexport)
#elif REFLECS_IMPL
#define REFLECS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllimport)
#else
#define REFLECS_EXPORT
#endif

#include <reflecs/entity.h>
#include <reflecs/hash.h>
#include <reflecs/entity.h>
#include <reflecs/components.h>

#ifdef __cplusplus
extern "C" {
#endif

void ecs_init(void);

void ecs_fini(void);

#ifdef __cplusplus
}
#endif

#endif
