// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "EntityFunctionLibrary.h"

#include "GameFramework/Actor.h"

#include "Worlds/FlecsWorldSubsystem.h"

#include "Interfaces/FlecsEntityInterface.h"
#include "GameFramework/Actor/FlecsEntityActorComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EntityFunctionLibrary)

FFlecsId UEntityFunctionLibrary::MakePairId(const FFlecsId First, const FFlecsId Second)
{
	return FFlecsId::MakePair(First, Second);
}

bool UEntityFunctionLibrary::IsEntityFromObject(UObject* Object)
{
	if UNLIKELY_IF(!ensureMsgf(Object, TEXT("Object is not valid")))
	{
		return false;
	}

	if (Object->Implements<UFlecsEntityInterface>())
	{
		return true;
	}

	if (const AActor* Actor = Cast<AActor>(Object))
	{
		return Actor->FindComponentByClass<UFlecsEntityActorComponent>() != nullptr;
	}
        
	return false;
}

FFlecsEntityHandle UEntityFunctionLibrary::GetEntityFromObject(UObject* Object)
{
	solid_check(Object);

	if (Object->Implements<UFlecsEntityInterface>())
	{
		return CastChecked<IFlecsEntityInterface>(Object)->GetEntityHandle();
	}

	if LIKELY_IF(const AActor* Actor = Cast<AActor>(Object))
	{
		if (const UFlecsEntityActorComponent* EntityActorComponent
			= Actor->FindComponentByClass<UFlecsEntityActorComponent>())
		{
			return EntityActorComponent->GetEntityHandle();
		}
	}

	return FFlecsEntityHandle::GetNullHandle();
}

void UEntityFunctionLibrary::DestroyEntity(const FFlecsEntityHandle& Entity)
{
	Entity.Destroy();
}

void UEntityFunctionLibrary::SetEntityName(const FFlecsEntityHandle& Entity, const FString& EntityName)
{
	Entity.SetName(EntityName);
}

FString UEntityFunctionLibrary::GetEntityName(const FFlecsEntityHandle& Entity)
{
	return FString(Entity.GetName());
}

void UEntityFunctionLibrary::AddComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType)
{
	Entity.Add(ComponentType);
}

void UEntityFunctionLibrary::AddEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
{
	Entity.Add(EntityType);
}

void UEntityFunctionLibrary::RemoveComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType)
{
	Entity.Remove(ComponentType);
}

void UEntityFunctionLibrary::RemoveEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
{
	Entity.Remove(EntityType);
}

bool UEntityFunctionLibrary::HasComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType)
{
	return Entity.Has(ComponentType);
}

bool UEntityFunctionLibrary::HasEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
{
	return Entity.Has(EntityType);
}

bool UEntityFunctionLibrary::IsValidEntity(const FFlecsEntityHandle& Entity)
{
	return Entity.IsValid();
}

bool UEntityFunctionLibrary::IsAlive(const FFlecsEntityHandle& Entity)
{
	return Entity.IsAlive();
}

bool UEntityFunctionLibrary::IsEnabled(const FFlecsEntityHandle& Entity)
{
	return Entity.IsEnabled();
}

void UEntityFunctionLibrary::Enable(const FFlecsEntityHandle& Entity)
{
	Entity.Enable();
}

void UEntityFunctionLibrary::Disable(const FFlecsEntityHandle& Entity)
{
	Entity.Disable();
}

void UEntityFunctionLibrary::Clear(const FFlecsEntityHandle& Entity)
{
	Entity.Clear();
}
