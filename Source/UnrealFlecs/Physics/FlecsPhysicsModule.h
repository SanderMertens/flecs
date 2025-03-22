// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TickerPhysicsHistoryComponent.h"
#include "Modules/FlecsModuleObject.h"
#include "Systems/FlecsSystem.h"
#include "Ticker/FlecsTickerComponent.h"
#include "UObject/Object.h"
#include "FlecsPhysicsModule.generated.h"

// @TODO: Not Fully Implemented
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsPhysicsModule final : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	virtual void WorldBeginPlay(UFlecsWorld* InWorld, UWorld* InGameWorld) override;

	void ResimulationHandlers();

	FORCEINLINE virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Flecs Physics Module");
	}

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	//bool bAllowResimulation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics",
		meta = (EditCondition = "bAllowResimulation"))
	int32 MaxFrameHistory = 300;

	flecs::observer AddPhysicsComponentObserver;

private:
	int32 PreResimValue = 0;

	FTickerPhysicsHistoryComponent* PhysicsHistoryComponentRef;
	FFlecsTickerComponent* TickerComponentRef;

	FPhysScene* Scene = nullptr;
	
}; // class UFlecsPhysicsModule
