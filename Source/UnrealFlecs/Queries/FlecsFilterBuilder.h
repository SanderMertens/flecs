// Solstice Games © 2024. All Rights Reserved.

#ifndef FLECS_FILTER_BUILDER_H
#define FLECS_FILTER_BUILDER_H

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsBaseBuilder.h"
#include "FlecsFilter.h"

struct FFlecsFilterBuilder : public TFlecsBaseQueryBuilder<flecs::filter_builder<>, FFlecsFilterBuilder, FFlecsFilter>
{
public:
    template <typename FunctionType>
    FORCEINLINE void Each(FunctionType&& InFunction)
    {
        Builder.each(std::forward<FunctionType>(InFunction));
    }
    
}; // struct FFlecsFilterBuilder

#endif // FLECS_FILTER_BUILDER_H
