// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "FlecsNetworkingModule.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, meta = (DisplayName = "Flecs Networking Module"))
class UNREALFLECS_API UFlecsNetworkingModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override
	{
	}

}; // class UFlecsNetworkingModule
