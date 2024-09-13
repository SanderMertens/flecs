// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPhysicsModule.h"

#include "FlecsPhysicsComponent.h"
#include "FlecsPhysicsSceneComponent.h"
#include "PBDRigidsSolver.h"
#include "TickerPhysicsHistoryComponent.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Ticker/FlecsTickerComponent.h"
#include "Ticker/FlecsTickerModule.h"
#include "Transforms/FlecsTransformComponent.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPhysicsModule)

UNLOG_CATEGORY(LogFlecsPhysicsModule);

void UFlecsPhysicsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	FPhysScene* Scene = GetFlecsWorld()->GetWorld()->GetPhysicsScene();
	solid_check(Scene);

	solid_checkf(GetFlecsWorld()->IsModuleImported<UFlecsTickerModule>(), TEXT("FlecsTickerModule is not imported!"));
	
	GetFlecsWorld()->SetSingleton<FFlecsPhysicsSceneComponent>(FFlecsPhysicsSceneComponent{ Scene });

	const UFlecsTickerModule* TickerModule = GetFlecsWorld()->GetModule<UFlecsTickerModule>();
	solid_check(IsValid(TickerModule));
	
	Scene->GetSolver()->EnableAsyncMode(1.0 / TickerModule->GetTickerRate());
	Scene->GetSolver()->SetIsDeterministic(true);
	Scene->GetSolver()->SetAsyncPhysicsBlockMode(Chaos::BlockForBestInterpolation);

	if (bAllowResimulation)
	{
		IConsoleVariable* ResimConsoleVariable =
			IConsoleManager::Get().FindConsoleVariable(TEXT("p.Resim.AllowRewindToResimulatedFrames"));
		PreResimValue = ResimConsoleVariable->GetInt();
		ResimConsoleVariable->Set(1);

		Scene->GetSolver()->EnableRewindCapture(MaxFrameHistory, true);
		Scene->GetSolver()->SetThreadingMode_External(Chaos::EThreadingModeTemp::SingleThread);

		GetFlecsWorld()->AddSingleton<FTickerPhysicsHistoryComponent>();
		
		FTickerPhysicsHistoryComponent* HistoryComponentPtr = GetFlecsWorld()->GetSingletonPtr<FTickerPhysicsHistoryComponent>();
		solid_check(HistoryComponentPtr);

		HistoryComponentPtr->HistoryItems.Reserve(MaxFrameHistory);

		FSolverPostAdvance::FDelegate PostAdvanceDelegate;
		PostAdvanceDelegate.BindWeakLambda(this, [&](MAYBE_UNUSED float InDeltaTime)
		{
			FTickerPhysicsHistoryComponent* HistoryComponent = GetFlecsWorld()->GetSingletonPtr<FTickerPhysicsHistoryComponent>();
			solid_check(HistoryComponent);

			if UNLIKELY_IF(!GetFlecsWorld()->GetWorld()->GetPhysicsScene()
				|| !GetFlecsWorld()->GetWorld()->GetPhysicsScene()->GetSolver())
			{
				return;
			}

			const int32 CurrentPhysicsFrame = GetFlecsWorld()->GetWorld()->GetPhysicsScene()->GetSolver()->GetCurrentFrame();

			if LIKELY_IF(HistoryComponent->HistoryItems.Num() >= MaxFrameHistory)
			{
				HistoryComponent->HistoryItems.RemoveAt(0);
			}

			FTickerPhysicsHistoryItem CurrentPhysicsFrameItem;
			CurrentPhysicsFrameItem.TickId = GetFlecsWorld()->GetSingleton<FFlecsTickerComponent>().TickId;
			CurrentPhysicsFrameItem.PhysicsFrame = CurrentPhysicsFrame;

			HistoryComponent->HistoryItems.Add(CurrentPhysicsFrameItem);

			GetFlecsWorld()->ModifiedSingleton<FTickerPhysicsHistoryComponent>();
		});

		Scene->GetSolver()->AddPostAdvanceCallback(PostAdvanceDelegate);
	}

	AddPhysicsComponentObserver = GetFlecsWorld()->CreateObserver<FFlecsPhysicsComponent,
		const FFlecsTransformComponent>(TEXT("PhysicsComponentObserver"))
		.event(flecs::OnAdd)
		.yield_existing()
		.term_at(0).read_write()
		.each([](flecs::iter& It, size_t Index, FFlecsPhysicsComponent& PhysicsComponent,
			const FFlecsTransformComponent& Transform)
		{
			FActorCreationParams Params;
			Params.bSimulatePhysics = PhysicsComponent.bSimulatePhysics;
			Params.bEnableGravity = PhysicsComponent.bEnableGravity;
			Params.bStartAwake = PhysicsComponent.bStartAwake;
			Params.bUpdateKinematicFromSimulation = PhysicsComponent.bUpdateKinematicFromSimulation;
			
			
		});
}

void UFlecsPhysicsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	GetFlecsWorld()->RemoveSingleton<FFlecsPhysicsSceneComponent>();

	if (bAllowResimulation)
	{
		IConsoleVariable* ResimConsoleVariable =
		IConsoleManager::Get().FindConsoleVariable(TEXT("p.Resim.AllowRewindToResimulatedFrames"));
		ResimConsoleVariable->Set(PreResimValue);
	}
}

