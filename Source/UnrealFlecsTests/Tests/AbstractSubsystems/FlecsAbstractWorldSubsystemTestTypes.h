// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Worlds/FlecsAbstractWorldSubsystem.h"

#include "FlecsAbstractWorldSubsystemTestTypes.generated.h"

UCLASS()
class UNREALFLECSTESTS_API UTestFlecsWorldSubsystem_Initialization : public UFlecsAbstractWorldSubsystem
{
	GENERATED_BODY()

public:
	UTestFlecsWorldSubsystem_Initialization();

	virtual void OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld) override;

	UPROPERTY()
	bool bWasFlecsWorldInitialized = false;

	UPROPERTY()
	mutable uint32 TimesChecked = 0;
	
}; // class UTestFlecsWorldSubsystem
