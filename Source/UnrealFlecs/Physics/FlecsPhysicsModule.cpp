// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPhysicsModule.h"
#include "FlecsPhysicsSceneComponent.h"
#include "PBDRigidsSolver.h"
#include "TickerPhysicsHistoryComponent.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Ticker/FlecsTickerComponent.h"
#include "Ticker/FlecsTickerModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPhysicsModule)

UNLOG_CATEGORY(LogFlecsPhysicsModule);

void UFlecsPhysicsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	FPhysScene* Scene = GetFlecsWorld()->GetWorld()->GetPhysicsScene();
	solid_check(Scene);

	solid_checkf(GetFlecsWorld()->IsModuleImported<UFlecsTickerModule>(), TEXT("FlecsTickerModule is not imported!"));
	
	GetFlecsWorld()->SetSingleton<FFlecsPhysicsSceneComponent>(FFlecsPhysicsSceneComponent{ Scene });

	UFlecsTickerModule* TickerModule = GetFlecsWorld()->GetModule<UFlecsTickerModule>();
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
		// GetFlecsWorld()->GetWorld()->StartPhysicsTickFunction.bCanEverTick = false;
		// GetFlecsWorld()->GetWorld()->EndPhysicsTickFunction.bCanEverTick = false;
		// GetFlecsWorld()->GetWorld()->bShouldSimulatePhysics = false;

		GetFlecsWorld()->AddSingleton<FTickerPhysicsHistoryComponent>();
		
		FTickerPhysicsHistoryComponent* HistoryComponentPtr = GetFlecsWorld()->GetSingletonPtr<FTickerPhysicsHistoryComponent>();
		solid_check(HistoryComponentPtr);

		HistoryComponentPtr->HistoryItems.Reserve(MaxFrameHistory);

		FSolverPostAdvance::FDelegate PostAdvanceDelegate;
		PostAdvanceDelegate.BindWeakLambda(this, [&](float InDeltaTime)
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
		
		/*PhysicsTickSystem = GetFlecsWorld()->CreateSystemWithBuilder
			<FFlecsTickerComponent, FFlecsPhysicsSceneComponent, FTickerPhysicsHistoryComponent>(TEXT("PhysicsTickSystem"))
			.cached()
			.kind(flecs::OnUpdate)
			.tick_source(GetFlecsWorld()->GetModule<UFlecsTickerModule>()->GetTickerSource())
			.term_at(0).singleton()
			.read<FFlecsTickerComponent>()
			.term_at(1).singleton()
			.term_at(2).singleton()
			.write<FTickerPhysicsHistoryComponent>()
			.each([&](flecs::iter& Iter, MAYBE_UNUSED size_t Index,
				FFlecsTickerComponent& InTicker, FFlecsPhysicsSceneComponent& InPhysicsScene,
				FTickerPhysicsHistoryComponent& InHistory)
			{
				if UNLIKELY_IF(!InPhysicsScene)
				{
					UN_LOG(LogFlecsPhysicsModule, Error, "Physics scene is null");
					return;
				}
				
				if UNLIKELY_IF(!InPhysicsScene->GetSolver())
				{
					UN_LOG(LogFlecsPhysicsModule, Error, "Physics scene solver is null");
					return;
				}

				InPhysicsScene->StartFrame();
				InPhysicsScene->WaitPhysScenes();
				InPhysicsScene->EndFrame();

				const int32 CurrentPhysicsFrame = Scene->GetSolver()->GetCurrentFrame();

				if LIKELY_IF(InHistory.HistoryItems.Num() >= MaxFrameHistory)
				{
					InHistory.HistoryItems.RemoveAt(0, EAllowShrinking::No);
				}

				FTickerPhysicsHistoryItem CurrentPhysicsFrameItem;
				CurrentPhysicsFrameItem.TickId = GetFlecsWorld()->GetSingleton<FFlecsTickerComponent>().TickId;
				CurrentPhysicsFrameItem.PhysicsFrame = CurrentPhysicsFrame;

				InHistory.HistoryItems.Add(CurrentPhysicsFrameItem);
			});*/
	}

}

void UFlecsPhysicsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	GetFlecsWorld()->RemoveSingleton<FFlecsPhysicsSceneComponent>();

	//PhysicsTickSystem.GetEntity().Destroy();

	if (bAllowResimulation)
	{
		IConsoleVariable* ResimConsoleVariable =
		IConsoleManager::Get().FindConsoleVariable(TEXT("p.Resim.AllowRewindToResimulatedFrames"));
		ResimConsoleVariable->Set(PreResimValue);
	}
}

