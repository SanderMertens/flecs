// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsTickerComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "Pipelines/FlecsGameLoopObject.h"
#include "Systems/FlecsSystem.h"
#include "FlecsTickerGameLoop.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsTickerGameLoop : public UFlecsGameLoopObject
{
	GENERATED_BODY()

public:

	virtual void InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld) override;
	virtual bool Progress(double DeltaTime, TSolidNotNull<UFlecsWorld*> InWorld) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticker",
		meta = (Units = "Hz", ClampMin = "1", ClampMax = "240")) //EditCondition = "!bUsePhysicsTick"))
	int64 TickerRate = 60;

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE int64 GetTickerRate() const { return TickerRate; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE void SetTickerRate(const int64 InRate) { TickerRate = InRate; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE FFlecsSystem GetTickerSystem() const { return TickerSystem; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE FFlecsEntityHandle GetTickerSource() const { return TickerSystem.GetEntity(); }
	
	UPROPERTY()
	double TickerAccumulator = 0.0;

	double TickerInterval = 0.0;

	FFlecsTickerComponent* TickerComponentPtr = nullptr;

	UPROPERTY()
	FFlecsSystem TickerSystem;

	UPROPERTY()
	FFlecsEntityHandle MainPipeline;

	UPROPERTY()
	FFlecsEntityHandle TickerPipeline;

}; // class UFlecsTickerGameLoop
