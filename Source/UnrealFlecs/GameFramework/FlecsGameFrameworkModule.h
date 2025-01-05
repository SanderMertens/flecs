// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsGameFrameworkModule.generated.h"

UCLASS(BlueprintType, DisplayName = "Flecs GameFramework Module")
class UNREALFLECS_API UFlecsGameFrameworkModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Flecs GameFramework Module");
	}


}; // class UFlecsGameFrameworkModule
