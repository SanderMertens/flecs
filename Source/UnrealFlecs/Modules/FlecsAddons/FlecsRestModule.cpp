// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsRestModule.h"

#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"
#include "Ticker/FlecsTickerGameLoop.h"

#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsRestModule)

UFlecsRestModule::UFlecsRestModule()
{
}

void UFlecsRestModule::InitializeModule(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	// End the scope as we dont want to import a module inside another module's scope
	InWorld->EndScope([this, InWorld]()
	{
#ifdef FLECS_REST
	
		uint16 ClientPieInstanceOffset = 0;
		
		const TSolidNotNull<const UWorld*> UnrealWorld = InWorld->GetWorld();

		#if WITH_EDITOR
		
		if (UnrealWorld->GetNetMode() == NM_Client)
		{
			ClientPieInstanceOffset = static_cast<uint16>(UE::GetPlayInEditorID());
		}
		
		#endif // WITH_EDITOR
		
		const uint16 RestPort = ECS_REST_DEFAULT_PORT + ClientPieInstanceOffset;

		InWorld->SetSingleton<flecs::Rest>(flecs::Rest{ .port = RestPort });
		
		RestEntity = InWorld->ObtainSingletonEntity<flecs::Rest>();
		solid_checkf(RestEntity.IsValid(),
			TEXT("Flecs REST module failed to create REST singleton entity on port %d."), RestPort);

		#ifdef FLECS_STATS

		if (bImportStats)
		{
			StatsEntity = InWorld->ImportFlecsModule<flecs::stats>();
		}

		#endif // #ifdef FLECS_STATS

#endif // #ifdef FLECS_REST
	});
}

void UFlecsRestModule::DeinitializeModule(TSolidNotNull<UFlecsWorld*> InWorld)
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
