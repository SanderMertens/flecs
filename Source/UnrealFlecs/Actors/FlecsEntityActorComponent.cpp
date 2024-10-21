// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityActorComponent.h"
#include "Logs/FlecsCategories.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityActorComponent)

UFlecsEntityActorComponent::UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlecsEntityActorComponent::BeginPlay()
{
	Super::BeginPlay();
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
	if (!GWorld  || !GWorld->IsGameWorld())
	{
		return;
	}

	if (EntityHandle.GetEntity().id() != 0)
	{
		return;
	}

	if UNLIKELY_IF(!EntityRecord.IsValid())
	{
		UN_LOG(LogFlecsEntity, Error, "Entity Record is not valid!");
		return;
	}

	if UNLIKELY_IF(WorldName.IsEmpty())
	{
		UN_LOG(LogFlecsEntity, Error, "World Name is empty!");
		return;
	}

	if (GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->HasValidFlecsWorld())
	{
		EntityHandle = GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->GetDefaultWorld()
			->CreateEntityWithRecord(EntityRecord);
	}
	else
	{
		GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->OnWorldCreated
			.AddUObject(this, &UFlecsEntityActorComponent::OnWorldCreated);
	}
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

void UFlecsEntityActorComponent::OnWorldCreated(const FString& InWorldName, UFlecsWorld* InWorld)
{
	if (InWorldName == WorldName)
	{
		EntityHandle = InWorld->CreateEntityWithRecord(EntityRecord);
		GetWorld()->GetSubsystem<UFlecsWorldSubsystem>()->OnWorldCreated.RemoveAll(this);
	}
}
