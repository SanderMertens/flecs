// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEntityActorComponent.h"
#include "Components/ObjectTypes/FlecsActorTag.h"
#include "Logs/FlecsCategories.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityActorComponent)

UFlecsEntityActorComponent::UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFlecsEntityActorComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeEntity();
}

void UFlecsEntityActorComponent::OnRegister()
{
	Super::OnRegister();
}

void UFlecsEntityActorComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UFlecsEntityActorComponent::InitializeEntity()
{
	if UNLIKELY_IF(!GWorld  || !GWorld->IsGameWorld())
	{
		return;
	}

	if UNLIKELY_IF(EntityHandle.GetEntity().id() != 0)
	{
		return;
	}

	if LIKELY_IF(ensureMsgf(UFlecsWorldSubsystem::HasValidFlecsWorldStatic(this),
		TEXT("Flecs World Subsystem is not initialized.")))
	{
		CreateActorEntity(UFlecsWorldSubsystem::GetDefaultWorldStatic(this));
	}
}

void UFlecsEntityActorComponent::SetEntityHandle(const FFlecsEntityHandle& InEntityHandle)
{
	EntityHandle = InEntityHandle;
	MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsEntityActorComponent, EntityHandle, this);
}

void UFlecsEntityActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UFlecsEntityActorComponent, EntityHandle, Params);
}

#if WITH_EDITORONLY_DATA

bool UFlecsEntityActorComponent::CanEditChange(const FProperty* InProperty) const
{
	const bool bIsEditable = Super::CanEditChange(InProperty);
	
	return bIsEditable;
}

#endif // WITH_EDITORONLY_DATA

void UFlecsEntityActorComponent::OnWorldCreated(UFlecsWorld* InWorld)
{
	CreateActorEntity(InWorld);
	
	GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->OnWorldCreatedDelegate.RemoveAll(this);
}

void UFlecsEntityActorComponent::CreateActorEntity(UFlecsWorld* InWorld)
{
	EntityHandle = InWorld->CreateEntityWithRecord(EntityRecord, EntityName);
	
	EntityHandle.SetPairFirst<FFlecsUObjectComponent, FFlecsActorTag>(FFlecsUObjectComponent{ GetOwner() });
	UN_LOGF(LogFlecsEntity, Log, "Created Actor Entity: %s", *EntityHandle.GetName());

	MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsEntityActorComponent, EntityHandle, this);
}
