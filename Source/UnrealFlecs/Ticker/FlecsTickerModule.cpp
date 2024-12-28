// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsTickerModule.h"
#include "FlecsTickerComponent.h"
#include "FlecsTickerDefaultEntities.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerModule)

UNLOG_CATEGORY(LogFlecsTicker);

DECLARE_STATS_GROUP(TEXT("FlecsTickerModule"), STATGROUP_FlecsTickerModule, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FlecsTickerModule::ProgressModule"),
	STAT_FlecsTickerModule_ProgressModule, STATGROUP_FlecsTickerModule);
DECLARE_CYCLE_STAT(TEXT("FlecsTickerModule::ProgressModule::RunPipeline"),
	STAT_FlecsTickerModule_ProgressModule_RunPipeline, STATGROUP_FlecsTickerModule);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("FlecsTickerModule::ProgressModule::RunPipeline::Ticks"),
	STAT_FlecsTickerModule_ProgressModule_RunPipeline_Ticks, STATGROUP_FlecsTickerModule);

static FORCEINLINE NO_DISCARD int flecs_entity_compare(
	ecs_entity_t e1, 
	ecs_entity_t e2) 
{
	return (e1 > e2) - (e1 < e2);
}

static FORCEINLINE NO_DISCARD int flecs_priority_compare(
	const flecs::entity_t InEntityA, 
	const flecs::SystemPriority* InPtrA, 
	const flecs::entity_t InEntityB, 
	const flecs::SystemPriority* InPtrB) 
{
	if (InPtrA->value == InPtrB->value) {
		return flecs_entity_compare(InEntityA, InEntityB);
	}
	else // lower value has higher priority
	{
		return InPtrA->value >= InPtrB->value ? 1 : -1;
	}
}

UFlecsTickerModule::UFlecsTickerModule(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UFlecsTickerModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	FFlecsTickerComponent TickerComponent;
	TickerComponent.TickId = 0;
	InWorld->SetSingleton<FFlecsTickerComponent>(TickerComponent);

	TickerComponentPtr = InWorld->GetSingletonPtr<FFlecsTickerComponent>();
	solid_checkf(TickerComponentPtr, TEXT("TickerComponentRef is not valid!"));

	MainPipeline = InWorld->CreatePipeline()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		.with<flecs::SystemPriority>()
		.without(FlecsFixedTick)
		.order_by<flecs::SystemPriority>(flecs_priority_compare)
		.build()
		.set_name("MainPipeline");

	TickerPipeline = InWorld->CreatePipeline()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.with(FlecsFixedTick)
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		.with<flecs::SystemPriority>()
		.order_by<flecs::SystemPriority>(flecs_priority_compare)
		.build()
		.set_name("TickerPipeline");

	InWorld->SetPipeline(MainPipeline);

	TickerInterval = 1.0 / static_cast<double>(TickerRate);
}

void UFlecsTickerModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if LIKELY_IF(IsValid(InWorld))
	{
		InWorld->RemoveSingleton<FFlecsTickerComponent>();
		TickerComponentPtr = nullptr;
		
		TickerPipeline.Destroy();
	}
}

void UFlecsTickerModule::ProgressModule(double InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_FlecsTickerModule_ProgressModule);

	TickerAccumulator += InDeltaTime;
	
	while (TickerAccumulator >= TickerInterval)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsTickerModule_ProgressModule_RunPipeline);
		
		TickerAccumulator -= TickerInterval;

		solid_check(TickerComponentPtr);
		++TickerComponentPtr->TickId;
		SET_DWORD_STAT(STAT_FlecsTickerModule_ProgressModule_RunPipeline_Ticks, TickerComponentPtr->TickId);

		GetFlecsWorld()->ModifiedSingleton<FFlecsTickerComponent>();
		
		GetFlecsWorld()->RunPipeline(TickerPipeline, TickerInterval);
	}
}
