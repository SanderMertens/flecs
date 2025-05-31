// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsRestModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsRestModule)

void UFlecsRestModule::InitializeModule(TSolidNonNullPtr<UFlecsWorld> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	#ifdef FLECS_REST
	
	uint16 ClientPieInstanceOffset = 0;
	
	const TSolidNonNullPtr<const UWorld> UnrealWorld = InWorld->GetWorld();

	#if WITH_EDITOR
	
	if (UnrealWorld->GetNetMode() == NM_Client)
	{
		ClientPieInstanceOffset = static_cast<uint16>(UE::GetPlayInEditorID());
	}
	
	#endif // WITH_EDITOR
	
	const uint16 RestPort = ECS_REST_DEFAULT_PORT + ClientPieInstanceOffset;

	InWorld->SetSingleton<flecs::Rest>(flecs::Rest{ .port = RestPort });
	RestEntity = InWorld->ObtainSingletonEntity<flecs::Rest>();

	#ifdef FLECS_STATS

	if (bImportStats)
	{
		StatsEntity = InWorld->ImportFlecsModule<flecs::stats>();
	}

	#endif // FLECS_STATS

	#endif // FLECS_REST
}

void UFlecsRestModule::DeinitializeModule(TSolidNonNullPtr<UFlecsWorld> InWorld)
{
	#ifdef FLECS_REST

	#ifdef FLECS_STATS
	
	if (StatsEntity.IsValid())
	{
		StatsEntity.Disable();
	}

	#endif // FLECS_STATS
	
	if UNLIKELY_IF(!RestEntity.IsValid())
	{
		return;
	}
	
	RestEntity.Disable();

	#endif // FLECS_REST
}
