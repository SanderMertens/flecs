// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsTickerSingletonComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "Pipelines/FlecsGameLoopObject.h"
#include "Systems/FlecsSystem.h"
#include "FlecsTickerGameLoop.generated.h"

// This should not be used as the main pipeline in a world
UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsTickerGameLoop : public UFlecsGameLoopObject
{
	GENERATED_BODY()

public:

	virtual void InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InGameLoopEntity) override;
	virtual bool Progress(double DeltaTime, TSolidNotNull<UFlecsWorld*> InWorld) override;

	virtual bool IsMainLoop() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticker",
		meta = (Units = "Hz", ClampMin = "1", ClampMax = "240")) //EditCondition = "!bUsePhysicsTick"))
	int64 TickerRate = 60;

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE int64 GetTickerRate() const { return TickerRate; }

	UFUNCTION(BlueprintCallable, Category = "Flecs | Ticker")
	FORCEINLINE void SetTickerRate(const int64 InRate) { TickerRate = InRate; }
	
	UPROPERTY()
	double TickerAccumulator = 0.0;

	UPROPERTY()
	double TickerInterval = 0.0;

	// This is a Sparse component, so the memory is stable
	FFlecsTickerSingletonComponent* TickerComponentPtr = nullptr;

	UPROPERTY()
	FFlecsEntityHandle TickerPipeline;

}; // class UFlecsTickerGameLoop
