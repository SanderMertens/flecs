// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsScriptStructComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptStructComponent
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    TObjectPtr<UScriptStruct> ScriptStruct;
    
}; // struct FFlecsScriptStructComponent
