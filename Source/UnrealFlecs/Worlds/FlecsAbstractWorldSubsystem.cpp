// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsAbstractWorldSubsystem.h"

#include "Subsystems/SubsystemCollection.h"

#include "FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsAbstractWorldSubsystem)

UFlecsAbstractWorldSubsystem::UFlecsAbstractWorldSubsystem()
{
}

void UFlecsAbstractWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FlecsWorldSubsystemRef = Collection.InitializeDependency<UFlecsWorldSubsystem>();
	FlecsWorldRef = UFlecsWorldSubsystem::GetDefaultWorldStatic(this);

	if (FlecsWorldRef.IsValid())
	{
		OnFlecsWorldInitialized(GetFlecsWorldChecked());
	}
	else // Handles test worlds
	{
		Unreal::Flecs::GOnFlecsWorldInitialized.AddWeakLambda(this, [this](const TSolidNotNull<UFlecsWorld*> InWorld)
		{
			FlecsWorldRef = InWorld;
			OnFlecsWorldInitialized(FlecsWorldRef.Get());
		});
	}
}

void UFlecsAbstractWorldSubsystem::OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	// Empty implementation, meant to be overridden in subclasses
}

void UFlecsAbstractWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UFlecsAbstractWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game
		|| WorldType == EWorldType::PIE
		|| WorldType == EWorldType::GameRPC;
}

UFlecsWorldSubsystem* UFlecsAbstractWorldSubsystem::GetFlecsWorldSubsystem() const
{
	return FlecsWorldSubsystemRef.Get();
}

TSolidNotNull<UFlecsWorldSubsystem*> UFlecsAbstractWorldSubsystem::GetFlecsWorldSubsystemChecked() const
{
	solid_checkf(FlecsWorldRef.IsValid(), TEXT("FlecsWorldSubsystem is not valid!"));
	return FlecsWorldSubsystemRef.Get();
}

bool UFlecsAbstractWorldSubsystem::IsFlecsWorldValid() const
{
	return FlecsWorldRef.IsValid();
}

UFlecsWorld* UFlecsAbstractWorldSubsystem::GetFlecsWorld() const
{
	return FlecsWorldRef.Get();
}

TSolidNotNull<UFlecsWorld*> UFlecsAbstractWorldSubsystem::GetFlecsWorldChecked() const
{
	solid_checkf(FlecsWorldRef.IsValid(), TEXT("FlecsWorld is not valid!"));
	return FlecsWorldRef.Get();
}
