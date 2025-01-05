// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsPrefabObject.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPrefabObject)

void UFlecsPrefabObject::CreatePrefab(UFlecsWorld* InWorld)
{
	solid_checkf(IsValid(InWorld), TEXT("World is not valid!"));
	FlecsWorld = InWorld;

	PrefabEntityHandle = InWorld->CreatePrefab(PrefabName);
	OnPrefabCreated(GetPrefabHandle());
	BP_OnPrefabCreated(GetPrefabHandle());
}

UWorld* UFlecsPrefabObject::GetWorld() const
{
	return GetFlecsWorld()->GetWorld();
}
