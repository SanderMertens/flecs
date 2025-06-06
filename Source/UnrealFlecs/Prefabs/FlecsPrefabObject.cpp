// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsPrefabObject.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPrefabObject)

void UFlecsPrefabObject::CreatePrefab(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	FlecsWorld = InWorld;

	PrefabEntityHandle = InWorld->CreatePrefab(PrefabName);
	OnPrefabCreated(GetPrefabHandle());
	BP_OnPrefabCreated(GetPrefabHandle());
}
