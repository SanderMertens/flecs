// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "UObject/Object.h"
#include "FlecsStatsModule.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UNREALFLECS_API UFlecsStatsModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("FlecsStatsModule");
	}

	UPROPERTY()
	FFlecsEntityHandle StatsEntity;

}; // class UFlecsStatsModule
