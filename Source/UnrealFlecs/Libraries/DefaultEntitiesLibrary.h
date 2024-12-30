// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DefaultEntitiesLibrary.generated.h"

UCLASS()
class UNREALFLECS_API UDefaultEntitiesLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities")
	static FORCEINLINE FFlecsEntityHandle GetNullEntity()
	{
		return FFlecsEntityHandle::GetNullHandle();
	}
	
}; // class UDefaultEntitiesLibrary
