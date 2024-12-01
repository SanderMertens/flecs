

#pragma once

#include "CoreMinimal.h"
#include "FlecsGlobalTransformComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsGlobalTransformComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FTransform Transform;
	
}; // struct FFlecsGlobalTransformComponent
