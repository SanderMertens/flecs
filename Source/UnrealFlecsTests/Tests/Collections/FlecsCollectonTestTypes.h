// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlecsCollectonTestTypes.generated.h"

USTRUCT()
struct UNREALFLECSTESTS_API FFlecsCollectionTestStruct
{
	GENERATED_BODY()
}; // struct FFlecsCollectionTestStruct

UCLASS()
class UNREALFLECSTESTS_API UFlecsCollectionTestClassNoInterface : public UObject
{
	GENERATED_BODY()

public:
	UFlecsCollectionTestClassNoInterface();
	
}; // class UFlecsCollectionTestClassNoInterface
