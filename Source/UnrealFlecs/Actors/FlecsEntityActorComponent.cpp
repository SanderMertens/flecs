// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityActorComponent.h"
#include "Logs/FlecsCategories.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityActorComponent)

UFlecsEntityActorComponent::UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlecsEntityActorComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeEntity();
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

	if (!EntityRecord.IsValid())
	{
		UN_LOG(LogFlecsEntity, Error, "Entity Record is not valid!");
		return;
	}

	if (WorldName.IsEmpty())
	{
		UN_LOG(LogFlecsEntity, Error, "World Name is empty!");
		return;
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
			if (EntityHandle.DisplayName.IsNone())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	
	return bIsEditable;
}

#endif // WITH_EDITORONLY_DATA
