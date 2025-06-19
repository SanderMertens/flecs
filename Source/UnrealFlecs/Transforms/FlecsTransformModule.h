// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsTransformModule.generated.h"

// @TODO: Not Fully Implemented
UCLASS(BlueprintType, NotBlueprintable, DisplayName = "Flecs Transform Module")
class UNREALFLECS_API UFlecsTransformModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	UFlecsTransformModule();

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return "Flecs Transform Module";
	}

	virtual void InitializeModule(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(TSolidNotNull<UFlecsWorld*> InWorld) override;


}; // class UFlecsTransformModule
