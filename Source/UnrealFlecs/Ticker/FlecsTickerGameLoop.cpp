// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTickerGameLoop.h"

#include "FlecsTickerDefaultEntities.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerGameLoop)

DECLARE_STATS_GROUP(TEXT("FlecsTickerGameLoop"), STATGROUP_FlecsTickerGameLoop, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("UFlecsTickerGameLoop::Progress"),
	STAT_FlecsFlecsTickerGameLoop_Progress, STATGROUP_FlecsTickerGameLoop);
DECLARE_CYCLE_STAT(TEXT("UFlecsTickerGameLoop::Progress::RunPipeline"),
	STAT_FlecsTickerGameLoop_Progress_RunPipeline, STATGROUP_FlecsTickerGameLoop);

DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("UFlecsTickerGameLoop::Progress::RunPipeline::Ticks"),
	STAT_FlecsTickerGameLoop_Progress_RunPipeline_Ticks, STATGROUP_FlecsTickerGameLoop);

static NO_DISCARD FORCEINLINE int flecs_entity_compare(
	const ecs_entity_t e1,
	const ecs_entity_t e2) 
{
	return (e1 > e2) - (e1 < e2);
}

static NO_DISCARD FORCEINLINE int flecs_priority_compare(
	const flecs::entity_t InEntityA,
	const flecs::SystemPriority* InPtrA,
	const flecs::entity_t InEntityB,
	const flecs::SystemPriority* InPtrB) 
{
	solid_check(InPtrA);
	solid_check(InPtrB);
	
	if (InPtrA->value == InPtrB->value)
	{
		return flecs_entity_compare(InEntityA, InEntityB);
	}
	else // lower value has higher priority
	{
		return InPtrA->value >= InPtrB->value ? 1 : -1;
	}
}

void UFlecsTickerGameLoop::InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld)
{
	InWorld->RegisterComponentType<FFlecsTickerSingletonComponent>();
	InWorld->AddSingleton<FFlecsTickerSingletonComponent>();

	TickerComponentPtr = InWorld->GetMutSingletonPtr<FFlecsTickerSingletonComponent>();
	solid_check(TickerComponentPtr);

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

bool UFlecsTickerGameLoop::Progress(double DeltaTime, TSolidNotNull<UFlecsWorld*> InWorld)
{
	SCOPE_CYCLE_COUNTER(STAT_FlecsFlecsTickerGameLoop_Progress);

	TickerAccumulator += DeltaTime;
	
	while (TickerAccumulator >= TickerInterval)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsTickerGameLoop_Progress_RunPipeline);

		solid_check(TickerComponentPtr);
		
		TickerAccumulator -= TickerInterval;
		
		++TickerComponentPtr->TickId;
		INC_DWORD_STAT(STAT_FlecsTickerGameLoop_Progress_RunPipeline_Ticks);

		solid_checkf(TickerPipeline.IsValid(), TEXT("TickerPipeline is not valid!"));
		InWorld->RunPipeline(TickerPipeline, TickerInterval);
	}
	
	return InWorld->Progress();
}
