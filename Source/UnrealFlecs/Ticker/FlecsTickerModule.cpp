// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsTickerModule.h"
#include "FlecsTickerComponent.h"
#include "FlecsTickerEngineSubsystem.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerModule)

UNLOG_CATEGORY(LogFlecsTicker);

DECLARE_STATS_GROUP(TEXT("FlecsTickerModule"), STATGROUP_FlecsTickerModule, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FlecsTickerModule::ProgressModule"),
	STAT_FlecsTickerModule_ProgressModule, STATGROUP_FlecsTickerModule);
DECLARE_CYCLE_STAT(TEXT("FlecsTickerModule::ProgressModule::RunPipeline"),
	STAT_FlecsTickerModule_ProgressModule_RunPipeline, STATGROUP_FlecsTickerModule);

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
		.kind(flecs::PreFrame)
		.term_at(0).singleton()
		.write<FFlecsTickerComponent>()
		.immediate()
		.each([](FFlecsTickerComponent& Ticker)
		{
			++Ticker.TickId;
		}).add(FlecsFixedTick);

	MainPipeline = InWorld->CreatePipeline()
		.cached()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		.without(FlecsFixedTick)
		.build()
		.set_name("MainPipeline");

	TickerPipeline = InWorld->CreatePipeline()
		.cached()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.with(FlecsFixedTick)
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		.build()
		.set_name("TickerPipeline");

	InWorld->SetPipeline(MainPipeline);
}

void UFlecsTickerModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if (IsValid(InWorld))
	{
		TickerSystem.GetEntity().Destroy();
		InWorld->RemoveSingleton<FFlecsTickerComponent>();
		TickerPipeline.Destroy();
	}
}

void UFlecsTickerModule::ProgressModule(double InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_FlecsTickerModule_ProgressModule);

	TickerAccumulator += InDeltaTime;
	
	while (TickerAccumulator >= 1.0 / TickerRate)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsTickerModule_ProgressModule_RunPipeline);
		
		TickerAccumulator -= 1.0 / TickerRate;
		
		GetFlecsWorld()->RunPipeline(TickerPipeline);
	}
}
