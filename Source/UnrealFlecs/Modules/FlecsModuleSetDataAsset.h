// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/SolidNonNullPtr.h"
#include "FlecsModuleSetDataAsset.generated.h"

class UFlecsWorld;

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsModuleSetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Flecs",
		meta = (MustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
	TArray<TObjectPtr<UObject>> Modules;
	
	void ImportModules(const TSolidNonNullPtr<UFlecsWorld> InWorld);
	
}; // class UFlecsModuleSetDataAsset
