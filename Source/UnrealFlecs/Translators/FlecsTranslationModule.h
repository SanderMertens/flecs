// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsTranslationModule.generated.h"

// @TODO: Not Implemented
UCLASS(BlueprintType, meta = (DisplayName = "Flecs Translation Module"))
class UNREALFLECS_API UFlecsTranslationModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Flecs Translation Module");
	}
	
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

private:

}; // class UFlecsTranslationModule
