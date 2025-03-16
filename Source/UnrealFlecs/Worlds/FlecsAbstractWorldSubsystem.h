// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorld.h"
#include "FlecsWorldSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsAbstractWorldSubsystem.generated.h"

UCLASS(Abstract)
class UNREALFLECS_API UFlecsAbstractWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FORCEINLINE virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);

		Collection.InitializeDependency<UFlecsWorldSubsystem>();
		FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorldStatic(this);
	}
	
	FORCEINLINE virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override final
	{
		return WorldType == EWorldType::Game
			|| WorldType == EWorldType::PIE
			|| WorldType == EWorldType::GameRPC;
	}
	
	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorld.Get();
	}

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;
	
}; // class UFlecsAbstractWorldSubsystem


