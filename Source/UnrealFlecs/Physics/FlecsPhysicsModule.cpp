// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPhysicsModule.h"
#include "FlecsPhysicsSceneComponent.h"
#include "PBDRigidsSolver.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Ticker/FlecsTickerComponent.h"
#include "Ticker/FlecsTickerModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPhysicsModule)

void UFlecsPhysicsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	FPhysScene* Scene = InWorld->GetWorld()->GetPhysicsScene();
	InWorld->SetSingleton(FFlecsPhysicsSceneComponent{ Scene });

	Scene->GetSolver()->SetCurrentFrame(0);
	Scene->GetSolver()->SetIsDeterministic(true);

	if (InWorld->IsModuleImported<UFlecsTickerModule>())
	{
		InWorld->CreateSystemWithBuilder<FFlecsPhysicsSceneComponent, FFlecsTickerComponent>(TEXT("PhysicsTickSystem"))
			.cached()
			.term_at(0).singleton().read()
			.term_at(1).singleton().read()
			.tick_source(InWorld->GetModule<UFlecsTickerModule>()->TickerSystem.GetEntity())
			.each([&](FFlecsPhysicsSceneComponent& InPhysicsScene, FFlecsTickerComponent& InTicker)
			{
				if UNLIKELY_IF(!InPhysicsScene.PhysicsScene)
				{
					return;
				}

				if (InPhysicsScene.PhysicsScene->GetSolver())
				{
					InPhysicsScene.PhysicsScene->GetSolver()->SetCurrentFrame(static_cast<int32>(InTicker.TickId));
				}
			});
	}
	
}

void UFlecsPhysicsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	InWorld->RemoveSingleton<FFlecsPhysicsSceneComponent>();
}
