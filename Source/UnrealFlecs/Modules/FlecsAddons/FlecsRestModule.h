// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "UObject/Object.h"
#include "FlecsRestModule.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UNREALFLECS_API UFlecsRestModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Flecs Rest Module");
	}

	UPROPERTY()
	FFlecsEntityHandle RestEntity;

}; // class UFlecsRestModule
