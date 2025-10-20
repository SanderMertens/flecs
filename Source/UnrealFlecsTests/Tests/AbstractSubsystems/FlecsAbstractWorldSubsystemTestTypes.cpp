// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsAbstractWorldSubsystemTestTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsAbstractWorldSubsystemTestTypes)

UTestFlecsWorldSubsystem_Initialization::UTestFlecsWorldSubsystem_Initialization()
{
}

void UTestFlecsWorldSubsystem_Initialization::OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	Super::OnFlecsWorldInitialized(InWorld);

	bWasFlecsWorldInitialized = true;
}
