// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorld.h"
#include "Elements/Framework/TypedElementColumnUtils.h"
#include "flecs/addons/cpp/table.hpp"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

UCLASS()
class UNREALFLECS_API UFlecsWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	FORCEINLINE bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

	TypedElement::ColumnUtils

protected:
	std::vector<FFlecsWorld> Worlds;
};
