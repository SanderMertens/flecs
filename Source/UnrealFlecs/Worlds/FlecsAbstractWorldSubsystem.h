// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"

#include "FlecsWorld.h"

#include "FlecsAbstractWorldSubsystem.generated.h"

// @TODO:Add Tests for this class!

UCLASS(Abstract)
class UNREALFLECS_API UFlecsAbstractWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override final;

	// Will return null if the world isn't valid or initialized yet.
	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorldRef.Get();
	}

	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorldChecked() const
	{
		solid_checkf(FlecsWorldRef.IsValid(), TEXT("FlecsWorld is not valid!"));
		return FlecsWorldRef.Get();
	}

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorldRef;
	
}; // class UFlecsAbstractWorldSubsystem


