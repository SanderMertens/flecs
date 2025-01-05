// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsRestModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsRestModule)

void UFlecsRestModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	#ifdef FLECS_REST
	
	uint16 ClientPieInstanceOffset = 0;
	
	const UWorld* UnrealWorld = InWorld->GetWorld();
	solid_check(IsValid(UnrealWorld));
	
	if (UnrealWorld && UnrealWorld->GetNetMode() == NM_Client)
	{
		ClientPieInstanceOffset = UE::GetPlayInEditorID();
	}
	
	const uint16 RestPort = ECS_REST_DEFAULT_PORT + ClientPieInstanceOffset;

	InWorld->SetSingleton<flecs::Rest>(flecs::Rest{ .port = RestPort});
	RestEntity = InWorld->GetSingletonEntity<flecs::Rest>();

	#ifdef FLECS_STATS

	if (bImportStats)
	{
		StatsEntity = InWorld->ImportFlecsModule<flecs::stats>();
	}

	#endif // FLECS_STATS

	#endif // FLECS_REST
}

void UFlecsRestModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	#ifdef FLECS_REST
	
	if (StatsEntity.IsValid())
	{
		StatsEntity.Disable();
	}
	
	if UNLIKELY_IF(!RestEntity.IsValid())
	{
		return;
	}
	
	RestEntity.Disable();

	#endif // FLECS_REST
}
