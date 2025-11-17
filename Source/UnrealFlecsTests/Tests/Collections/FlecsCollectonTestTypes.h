// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Collections/FlecsCollectionInterface.h"

#include "FlecsCollectonTestTypes.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct UNREALFLECSTESTS_API FFlecsCollectionTestStruct
{
	GENERATED_BODY()
}; // struct FFlecsCollectionTestStruct

UCLASS(BlueprintInternalUseOnlyHierarchical)
class UNREALFLECSTESTS_API UFlecsCollectionTestClassNoInterface : public UObject
{
	GENERATED_BODY()

public:
	UFlecsCollectionTestClassNoInterface();
	
}; // class UFlecsCollectionTestClassNoInterface

UCLASS(BlueprintInternalUseOnly)
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
	
}; // class UFlecsCollectionTestClassWithInterface

UCLASS(BlueprintInternalUseOnlyHierarchical)
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface_Inherited : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
	
}; // class UFlecsCollectionTestClassWithInterface_Inherited

UCLASS(BlueprintInternalUseOnlyHierarchical)
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface_Parameterized : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
	
}; // class UFlecsCollectionTestClassWithInterface_Parameterized

UCLASS(BlueprintInternalUseOnlyHierarchical)
class UNREALFLECSTESTS_API UFlecsCollectionTestClassWithInterface_WithSubEntities : public UObject, public IFlecsCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const override;
};