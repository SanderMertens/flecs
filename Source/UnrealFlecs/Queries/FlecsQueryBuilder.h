// Solstice Games © 2024. All Rights Reserved.

#ifndef FLECS_QUERY_BUILDER_H
#define FLECS_QUERY_BUILDER_H

#include "CoreMinimal.h"
#include "FlecsBaseBuilder.h"
#include "FlecsQuery.h"

struct FFlecsQueryBuilder : public TFlecsBaseQueryBuilder<flecs::query_builder<>, FFlecsQueryBuilder, FFlecsQuery>
{
}; // struct FFlecsQueryBuilder

#endif // FLECS_QUERY_BUILDER_H
