// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQuery.generated.h"

USTRUCT(BlueprintType)
struct FFlecsQuery
{
    GENERATED_BODY()

public:

private:
    flecs::query<> Query;
    
}; // struct FFlecsQuery
