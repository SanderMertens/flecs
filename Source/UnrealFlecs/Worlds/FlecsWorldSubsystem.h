// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include <unordered_map>

#include "CoreMinimal.h"
#include "FlecsWorld.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& CreateWorld(const FName& Name, const TSet<FName>& Modules);

	FORCEINLINE bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

protected:
	std::vector<FFlecsWorld> Worlds;
	std::unordered_map<FName, FFlecsWorld*> WorldNameMap;
}; // class UFlecsWorldSubsystem
