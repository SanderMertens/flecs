// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Entities/FlecsEntityHandle.h"

#include "DefaultEntitiesLibrary.generated.h"

UCLASS()
class UNREALFLECS_API UDefaultEntitiesLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities",
		meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "Flecs Null Entity"))
	static FFlecsEntityHandle GetNullEntity(const UObject* WorldContextObject);

}; // class UDefaultEntitiesLibrary
