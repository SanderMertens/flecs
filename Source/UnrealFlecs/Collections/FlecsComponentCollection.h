// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityRecord.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsComponentCollection.generated.h"

USTRUCT(BlueprintType)
struct FFlecsComponentCollection
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFlecsEntityRecord Record;
	
}; // struct FFlecsComponentCollection

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsComponentCollection, ecs_pair(flecs::OnInstantiate, flecs::DontInherit));

