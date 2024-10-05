// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickerPhysicsHistoryComponent.h"
#include "Modules/FlecsModuleInterface.h"
#include "Systems/FlecsSystem.h"
#include "Ticker/FlecsTickerComponent.h"
#include "UObject/Object.h"
#include "FlecsPhysicsModule.generated.h"

UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsPhysicsModule final : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	FORCEINLINE void ResimulationHandlers();
	FORCEINLINE void PhysicsComponentObservers();

	FORCEINLINE virtual FString GetModuleName_Implementation() const override { return TEXT("Flecs Physics Module"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	bool bAllowResimulation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics",
		meta = (EditCondition = "bAllowResimulation"))
	int32 MaxFrameHistory = 300;

	flecs::observer AddPhysicsComponentObserver;

private:
	int32 PreResimValue = 0;

	flecs::ref<FTickerPhysicsHistoryComponent> PhysicsHistoryComponentRef;
	flecs::ref<FFlecsTickerComponent> TickerComponentRef;
	
}; // class UFlecsPhysicsModule
