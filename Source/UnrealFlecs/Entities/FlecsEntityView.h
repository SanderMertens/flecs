// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsId.h"
#include "GameplayTagContainer.h"
#include "Types/SolidEnumSelector.h"
#include "FlecsEntityView.generated.h"

USTRUCT()
struct UNREALFLECS_API FFlecsEntityView
{
	GENERATED_BODY()

public:


private:
	
	
}; // struct FFlecsEntityView

template <>
struct TStructOpsTypeTraits<FFlecsEntityView> : public TStructOpsTypeTraitsBase2<FFlecsEntityView>
{
	enum
	{
		
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
