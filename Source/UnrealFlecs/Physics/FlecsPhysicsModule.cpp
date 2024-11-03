// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPhysicsModule.h"
#include "FlecsPhysicsSceneComponent.h"
#include "PBDRigidsSolver.h"
#include "TickerPhysicsHistoryComponent.h"
#include "Components/UWorldPtrComponent.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Ticker/FlecsTickerComponent.h"
#include "Ticker/FlecsTickerModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPhysicsModule)

UNLOG_CATEGORY(LogFlecsPhysicsModule);

DECLARE_STATS_GROUP(TEXT("FlecsPhysicsModule"), STATGROUP_FlecsPhysicsModule, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FlecsPhysicsModule::ResimulationHandlers"),
	STAT_FlecsPhysicsModule_ResimulationHandlers, STATGROUP_FlecsPhysicsModule);

void UFlecsPhysicsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	FPhysScene* Scene = GetFlecsWorld()->GetWorld()->GetPhysicsScene();
	solid_check(Scene);

	GetFlecsWorld()->RegisterModuleDependency<UFlecsTickerModule>
		(this, [&](MAYBE_UNUSED UFlecsTickerModule* InModuleObject,
			UFlecsWorld* InFlecsWorld, FFlecsEntityHandle& InTickerEntity)
		{
			InFlecsWorld->SetSingleton<FFlecsPhysicsSceneComponent>(FFlecsPhysicsSceneComponent{ Scene });

			const UFlecsTickerModule* TickerModule = InFlecsWorld->GetModule<UFlecsTickerModule>();
			solid_check(IsValid(TickerModule));
			
			Scene->GetSolver()->EnableAsyncMode(1.0 / TickerModule->GetTickerRate());
			Scene->GetSolver()->SetIsDeterministic(true);
			Scene->GetSolver()->SetAsyncPhysicsBlockMode(Chaos::BlockForBestInterpolation);

			if (bAllowResimulation)
			{
				ResimulationHandlers();
			}
		});
}

void UFlecsPhysicsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if (!IsValid(InWorld))
	{
		return;
	}
	
	GetFlecsWorld()->RemoveSingleton<FFlecsPhysicsSceneComponent>();

	if (bAllowResimulation)
	{
		IConsoleVariable* ResimConsoleVariable =
			IConsoleManager::Get().FindConsoleVariable(TEXT("p.Resim.AllowRewindToResimulatedFrames"));
		ResimConsoleVariable->Set(PreResimValue);
	}
}

void UFlecsPhysicsModule::ResimulationHandlers()
{
	const FPhysScene* Scene = GetFlecsWorld()->GetWorld()->GetPhysicsScene();
	solid_check(Scene);
	
	IConsoleVariable* ResimConsoleVariable =
			IConsoleManager::Get().FindConsoleVariable(TEXT("p.Resim.AllowRewindToResimulatedFrames"));
	PreResimValue = ResimConsoleVariable->GetInt();
	ResimConsoleVariable->Set(1);

	Scene->GetSolver()->EnableRewindCapture(MaxFrameHistory, true);
	Scene->GetSolver()->SetThreadingMode_External(Chaos::EThreadingModeTemp::SingleThread);

	GetFlecsWorld()->AddSingleton<FTickerPhysicsHistoryComponent>();
		
	PhysicsHistoryComponentRef = GetFlecsWorld()->GetSingletonFlecsRef<FTickerPhysicsHistoryComponent>();
	solid_check(PhysicsHistoryComponentRef);

	PhysicsHistoryComponentRef->HistoryItems.Reserve(MaxFrameHistory);

	TickerComponentRef = GetFlecsWorld()->GetSingletonFlecsRef<FFlecsTickerComponent>();

	FSolverPostAdvance::FDelegate PostAdvanceDelegate;
	PostAdvanceDelegate.BindWeakLambda(this, [&](MAYBE_UNUSED float InDeltaTime)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsPhysicsModule_ResimulationHandlers);

		if UNLIKELY_IF(!IsWorldValid())
		{
			return;
		}

		const UFlecsWorld* FlecsWorld = GetFlecsWorld();
		solid_checkf(IsValid(FlecsWorld), TEXT("FlecsWorld is invalid"));

		if UNLIKELY_IF(!FlecsWorld->GetWorld()->GetPhysicsScene())
		{
			return;
		}
		
		Chaos::FPhysicsSolver* Solver = FlecsWorld->GetWorld()->GetPhysicsScene()->GetSolver();
		solid_check(Solver);

		solid_check(PhysicsHistoryComponentRef);

		const int32 CurrentPhysicsFrame = Solver->GetCurrentFrame();

		if LIKELY_IF(PhysicsHistoryComponentRef->HistoryItems.Num() >= MaxFrameHistory)
		{
			PhysicsHistoryComponentRef->HistoryItems.RemoveAt(0);
		}

		FTickerPhysicsHistoryItem CurrentPhysicsFrameItem;
		CurrentPhysicsFrameItem.TickId = TickerComponentRef->TickId;
		CurrentPhysicsFrameItem.PhysicsFrame = CurrentPhysicsFrame;

		PhysicsHistoryComponentRef->HistoryItems.Add(CurrentPhysicsFrameItem);

		FlecsWorld->ModifiedSingleton<FTickerPhysicsHistoryComponent>();
	});
	
	Scene->GetSolver()->AddPostAdvanceCallback(PostAdvanceDelegate);
}

