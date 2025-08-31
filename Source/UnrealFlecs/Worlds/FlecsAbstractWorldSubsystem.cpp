// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsAbstractWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsAbstractWorldSubsystem)

void UFlecsAbstractWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UFlecsWorldSubsystem>();
	FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorldStatic(this);
}

bool UFlecsAbstractWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game
		|| WorldType == EWorldType::PIE
		|| WorldType == EWorldType::GameRPC;
}
