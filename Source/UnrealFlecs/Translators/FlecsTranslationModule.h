// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "FlecsTranslationModule.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced,
	ClassGroup = (Flecs), meta = (DisplayName = "Flecs Translation Module"))
class UNREALFLECS_API UFlecsTranslationModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;;

private:

}; // class UFlecsTranslationModule
