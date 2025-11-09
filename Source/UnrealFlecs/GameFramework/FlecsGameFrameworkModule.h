// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsGameFrameworkModule.generated.h"

UCLASS(BlueprintType, DisplayName = "Flecs Game Framework Module")
class UNREALFLECS_API UFlecsGameFrameworkModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(TSolidNotNull<UFlecsWorld*> InWorld) override;
	
}; // class UFlecsGameFrameworkModule
