// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsRenderingModule.generated.h"

// @TODO: Not Implemented
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsRenderingModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	UFlecsRenderingModule(const FObjectInitializer& ObjectInitializer);

}; // class UFlecsRenderingModule
