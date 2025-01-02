// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityActorComponent.h"

#include "Components/FFlecsActorComponentTag.h"
#include "Components/FlecsActorTag.h"
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

	if (EntityHandle.GetEntity().id() != 0)
	{
		return;
	}

	if (UFlecsWorldSubsystem::HasValidFlecsWorldStatic(this))
	{
		CreateActorEntity(UFlecsWorldSubsystem::GetDefaultWorldStatic(this));
	}
	else
	{
		GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->OnWorldCreatedDelegate
			.AddUObject(this, &UFlecsEntityActorComponent::OnWorldCreated);
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
	
	if (bIsEditable)
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UFlecsEntityActorComponent, EntityRecord))
		{
			return EntityHandle.DisplayName.IsNone();
		}
	}
	
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
	EntityHandle.Set<FFlecsUObjectComponent>(GetOwner());
	EntityHandle.AddPair<FFlecsActorTag, FFlecsUObjectComponent>();
	
	UN_LOGF(LogFlecsEntity, Log, "Created Actor Entity: %s",
		*EntityHandle.GetName());

	MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsEntityActorComponent, EntityHandle, this);
}
