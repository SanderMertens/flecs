// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct FFlecsWorldSettings
{
    GENERATED_BODY()
    
public:
    FFlecsWorldSettings() = default;
    explicit FFlecsWorldSettings(const bool bInAutoMerge)
        : bAutoMerge(bInAutoMerge)
    {
    }

    bool operator==(const FFlecsWorldSettings& Other) const
    {
        return bAutoMerge == Other.bAutoMerge;
    }

    bool operator!=(const FFlecsWorldSettings& Other) const
    {
        return !(*this == Other);
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    bool bAutoMerge = true;
    
}; // struct FFlecsWorldSettings

