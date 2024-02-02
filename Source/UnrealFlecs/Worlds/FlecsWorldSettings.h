// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct FFlecsWorldSettings
{
    GENERATED_BODY()
    
public:
    FORCEINLINE FFlecsWorldSettings() = default;
    FORCEINLINE explicit FFlecsWorldSettings(const bool bInAutoMerge)
        : bAutoMerge(bInAutoMerge)
    {
    }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsWorldSettings& Other) const
    {
        return bAutoMerge == Other.bAutoMerge;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsWorldSettings& Other) const
    {
        return !(*this == Other);
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    bool bAutoMerge = true;
    
}; // struct FFlecsWorldSettings

