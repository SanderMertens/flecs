// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsNetworkingModule.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, meta = (DisplayName = "Flecs Networking Module"))
class UNREALFLECS_API UFlecsNetworkingModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:

	FORCEINLINE virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	FORCEINLINE virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

}; // class UFlecsNetworkingModule
