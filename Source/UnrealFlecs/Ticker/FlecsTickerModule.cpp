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
	FFlecsTickerComponent TickerComponent;
	TickerComponent.TickId = 0;
	InWorld->SetSingleton<FFlecsTickerComponent>(TickerComponent);
	InWorld->World.modified<FFlecsTickerComponent>();

	TickerSystem = InWorld->CreateSystemWithBuilder<FFlecsTickerComponent>(TEXT("FlecsTickerSystem"))
		.cached()
		.interval(1.0 / GetTickerRate())
		.kind(flecs::PreUpdate)
		.term_at(0).singleton()
		.write<FFlecsTickerComponent>()
		.immediate()
		.each([](FFlecsTickerComponent& Ticker)
		{
			++Ticker.TickId;
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
