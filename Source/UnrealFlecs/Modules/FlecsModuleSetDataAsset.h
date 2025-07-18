// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/SolidNotNull.h"
#include "FlecsModuleSetDataAsset.generated.h"

class UFlecsWorld;

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsModuleSetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Flecs",
		meta = (ObjectMustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
	TArray<TObjectPtr<UObject>> Modules;
	
	void ImportModules(const TSolidNotNull<UFlecsWorld*> InWorld);
	
}; // class UFlecsModuleSetDataAsset
