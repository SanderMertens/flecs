// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsScriptClassComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptClassComponent
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    TSubclassOf<UObject> ScriptClass;
    
}; // struct FFlecsScriptClassComponent
