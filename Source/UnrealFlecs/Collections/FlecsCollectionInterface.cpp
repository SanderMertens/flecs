// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionInterface)

// Add default functionality here for any IFlecsCollectionInterface functions that are not pure virtual.

void IFlecsCollectionInterface::CallInstantiateParameters(const FFlecsEntityHandle& InEntityHandle,
	const FInstancedStruct& InParameters) const
{
	if (GetParametersType().IsValid())
	{
		InstantiateParameters(InEntityHandle, InParameters);
	}
}

FInstancedStruct IFlecsCollectionInterface::GetParametersType() const
{
	return FInstancedStruct();
}
