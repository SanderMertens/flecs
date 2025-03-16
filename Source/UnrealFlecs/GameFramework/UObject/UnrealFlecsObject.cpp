// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "UnrealFlecsObject.h"
#include "Net/UnrealNetwork.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnrealFlecsObject)

UUnrealFlecsObject::UUnrealFlecsObject()
{
}

UUnrealFlecsObject::UUnrealFlecsObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUnrealFlecsObject::InitializeFlecsObject(UFlecsWorld* InFlecsWorld)
{
	solid_check(IsValid(InFlecsWorld));
	FlecsWorld = InFlecsWorld;

	ObjectEntityHandle = CreateObjectEntity();
	ObjectEntityHandle.Set<FFlecsUObjectComponent>({ this });
	
	UFlecsWorldSubsystem* FlecsWorldSubsystem = FlecsWorld->GetContext();
	solid_check(IsValid(FlecsWorldSubsystem));

	FlecsWorldSubsystem->ListenBeginPlay(
		FFlecsOnWorldBeginPlay::FDelegate::CreateWeakLambda(this, [this](UWorld* InWorld)
	{
		solid_check(IsValid(InWorld));
		solid_check(IsValid(FlecsWorld.Get()));
		solid_check(FlecsWorld->GetWorld() == InWorld);
		
		BeginPlay();
	}));
}

UFlecsWorld* UUnrealFlecsObject::GetFlecsWorld() const
{
	return FlecsWorld.Get();
}

UWorld* UUnrealFlecsObject::GetWorld() const
{
	if (FlecsWorld.IsValid())
	{
		return FlecsWorld->GetWorld();
	}
	
	return Super::GetWorld();
}

FFlecsEntityHandle UUnrealFlecsObject::CreateObjectEntity()
{
	return FlecsWorld->CreateEntity();
}

void UUnrealFlecsObject::BeginPlay()
{
	BP_BeginPlay();
}

void UUnrealFlecsObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	// In case the Inheriting class is ever replicated, we need to replicate the ObjectEntityHandle
	DOREPLIFETIME_WITH_PARAMS_FAST(UUnrealFlecsObject, ObjectEntityHandle, Params);
}


