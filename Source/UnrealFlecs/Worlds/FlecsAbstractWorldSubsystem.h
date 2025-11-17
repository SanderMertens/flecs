// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"

#include "FlecsWorld.h"

#include "FlecsAbstractWorldSubsystem.generated.h"

// @TODO:Add Tests for this class!
// @TODO: auto add a singleton component for each inheritor of this class?

class UFlecsWorldSubsystem;

UCLASS(Abstract)
class UNREALFLECS_API UFlecsAbstractWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFlecsAbstractWorldSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld);
	virtual void Deinitialize() override;

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override final;

	NO_DISCARD UFlecsWorldSubsystem* GetFlecsWorldSubsystem() const;
	NO_DISCARD TSolidNotNull<UFlecsWorldSubsystem*> GetFlecsWorldSubsystemChecked() const;

	NO_DISCARD bool IsFlecsWorldValid() const;

	// Will return null if the world isn't valid or initialized yet.
	NO_DISCARD UFlecsWorld* GetFlecsWorld() const;
	NO_DISCARD TSolidNotNull<UFlecsWorld*> GetFlecsWorldChecked() const;

private:
	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorldRef;

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorldSubsystem> FlecsWorldSubsystemRef;
	
}; // class UFlecsAbstractWorldSubsystem

