// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTransformModule.generated.h"

USTRUCT(BlueprintType)
struct FFlecsDirtyTransformTrait final
{
	GENERATED_BODY()
}; // struct FFlecsDirtyTransform

REGISTER_FLECS_COMPONENT_PROPERTIES(FFlecsDirtyTransformTrait,
	{ flecs::PairIsTag }, {});

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UNREALFLECS_API UFlecsTransformModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	UFlecsTransformModule();

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;


}; // class UFlecsTransformModule
