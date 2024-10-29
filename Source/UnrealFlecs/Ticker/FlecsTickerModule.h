// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsTickerComponent.h"
#include "Modules/FlecsModuleObject.h"
#include "Modules/FlecsModuleProgressInterface.h"
#include "Systems/FlecsSystem.h"
#include "UObject/Object.h"
#include "FlecsTickerModule.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class UNREALFLECS_API UFlecsTickerModule final : public UFlecsModuleObject, public IFlecsModuleProgressInterface
{
	GENERATED_BODY()

public:
	UFlecsTickerModule(const FObjectInitializer& InObjectInitializer);

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE virtual void ProgressModule(double InDeltaTime) override;

	FORCEINLINE virtual FString GetModuleName_Implementation() const override { return TEXT("Flecs Ticker Module"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker",
		meta = (Units = "Hz", ClampMin = "1", ClampMax = "240", EditCondition = "!bUsePhysicsTick"))
	int64 TickerRate = 60;

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE int64 GetTickerRate() const { return TickerRate; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE void SetTickerRate(const int64 InRate) { TickerRate = InRate; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE FFlecsSystem GetTickerSystem() const { return TickerSystem; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE FFlecsEntityHandle GetTickerSource() const { return TickerSystem.GetEntity(); }

	FFlecsTickerComponent* TickerComponentPtr = nullptr;

	UPROPERTY()
	FFlecsSystem TickerSystem;

	UPROPERTY()
	FFlecsEntityHandle MainPipeline;

	UPROPERTY()
	FFlecsEntityHandle TickerPipeline;

	UPROPERTY()
	double TickerAccumulator = 0.0;

	double TickerInterval = 0.0;

}; // class UFlecsTickerModule
