// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Collections/FlecsCollectionInterface.h"

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

UCLASS()
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
	
}; // class UFlecsCollectionTestClassWithInterface

UCLASS()
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface_Inherited : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
	
}; // class UFlecsCollectionTestClassWithInterface_Inherited