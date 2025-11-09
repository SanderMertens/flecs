// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleObject.h"
#include "UObject/Object.h"
#include "FlecsRestModule.generated.h"

UCLASS(BlueprintType, NotBlueprintable, DisplayName = "Flecs Rest Module")
class UNREALFLECS_API UFlecsRestModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	UFlecsRestModule();
	
	virtual void InitializeModule(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(TSolidNotNull<UFlecsWorld*> InWorld) override;

	UPROPERTY()
	FFlecsEntityHandle RestEntity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	bool bImportStats = true;

	UPROPERTY()
	FFlecsEntityHandle StatsEntity;

}; // class UFlecsRestModule
