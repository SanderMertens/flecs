// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleObject.h"
#include "UObject/Object.h"
#include "FlecsRestModule.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsRestModule final : public UFlecsModuleObject
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	bool bImportStats = false;

	UPROPERTY()
	FFlecsEntityHandle StatsEntity;

}; // class UFlecsRestModule
