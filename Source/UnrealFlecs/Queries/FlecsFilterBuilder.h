// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsFilter.h"
#include "Macros.h"
#include "Entities/FlecsEntityHandle.h"
#include "Entities/FlecsId.h"
#include "FlecsFilterBuilder.generated.h"

USTRUCT(BlueprintType)
struct FFlecsFilterBuilder
{
    GENERATED_BODY()

public:
    FORCEINLINE explicit FFlecsFilterBuilder(const flecs::world& InWorld, const FString& InName = TEXT(""))
        : FilterBuilder(InWorld, TCHAR_TO_ANSI(*InName))
    {
    }

    FORCEINLINE NO_DISCARD flecs::filter_builder<>& GetFlecsFilterBuilder() { return FilterBuilder; }
    FORCEINLINE NO_DISCARD const flecs::filter_builder<>& GetFlecsFilterBuilder() const { return FilterBuilder; }

    FORCEINLINE operator flecs::filter_builder<>&() { return GetFlecsFilterBuilder(); }
    FORCEINLINE operator const flecs::filter_builder<>&() const { return GetFlecsFilterBuilder(); }

    FORCEINLINE NO_DISCARD flecs::filter_builder<>* operator->() { return &FilterBuilder; }
    FORCEINLINE NO_DISCARD const flecs::filter_builder<>* operator->() const { return &FilterBuilder; }

    FORCEINLINE NO_DISCARD FFlecsFilter Build()
    {
        return FilterBuilder.build();
    }

private:
    flecs::filter_builder<> FilterBuilder;
}; // struct FFlecsFilterBuilder
