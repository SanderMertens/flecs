// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SolidNotNull.h"

struct FFlecsStartTestWorldCommand : public IAutomationLatentCommand
{
public:

private:
	TUniquePtr<FTestWorldWrapper> TestWorldWrapper;
}; // struct FFlecsStartTestWorldCommand

struct FFlecsTickTestWorldCommand : public IAutomationLatentCommand
{
public:

	FFlecsTickTestWorldCommand(const TNotNull<UWorld*> InWorld,
		const int32 InTickCountRemaining, const float InDeltaTime)
		: World(InWorld)
		, TickCountRemaining(InTickCountRemaining)
		, DeltaTime(InDeltaTime)
	{
	}

	virtual bool Update() override
	{
		if LIKELY_IF(World.IsValid())
		{
			if (TickCountRemaining > 0)
			{
				World->Tick(ELevelTick::LEVELTICK_All, DeltaTime);
				--TickCountRemaining;
				return false;
			}
			else
			{
				return true;
			}
		}
		
		return true;
	}

	TWeakObjectPtr<UWorld> World;
	int32 TickCountRemaining = 0;
	float DeltaTime = 0.0f;
	
}; // struct FFlecsTickTestWorldCommand