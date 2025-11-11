// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"

#include "FlecsCollectionInterface.generated.h"

struct FFlecsEntityHandle;
struct FFlecsCollectionBuilder;

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UFlecsCollectionInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsCollectionInterface

class UNREALFLECS_API IFlecsCollectionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BuildCollection(FFlecsCollectionBuilder& Builder) const PURE_VIRTUAL(IFlecsCollectionInterface::BuildCollection, );

	void CallInstantiateParameters(const FFlecsEntityHandle& InEntityHandle, const FInstancedStruct& InParameters) const;

	// currently unused
	virtual void InstantiateParameters(const FFlecsEntityHandle& InEntityHandle, const FInstancedStruct& InParameters) const {}

	// currently unused
	// Defaults to an invalid FInstancedStruct (this is optional)
	virtual FInstancedStruct GetParametersType() const;
	
}; // class IFlecsCollectionInterface
