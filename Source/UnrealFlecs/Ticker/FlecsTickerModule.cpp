// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsTickerModule.h"
#include "FlecsTickerComponent.h"
#include "Worlds/FlecsWorld.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerModule)

UNLOG_CATEGORY(LogFlecsTicker);

UFlecsTickerModule::UFlecsTickerModule(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UFlecsTickerModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->SetSingleton<FFlecsTickerComponent>(FFlecsTickerComponent(0));
	
	TickerSystem = InWorld->CreateSystemWithBuilder<FFlecsTickerComponent>(TEXT("FlecsTickerSystem"))
		.interval(1.0f / TickerRate)
		.kind(flecs::PreFrame)
		.singleton()
		.immediate()
		.each([](flecs::iter& Iter, size_t, FFlecsTickerComponent& Ticker)
		{
			Iter.world().defer_suspend();
			++Ticker.TickId;
			Iter.world().defer_resume();
		});
}

void UFlecsTickerModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if (IsValid(TickerSystem) && IsValid(InWorld))
	{
		TickerSystem.GetEntity().Destroy();
		InWorld->RemoveSingleton<FFlecsTickerComponent>();
	}
}
