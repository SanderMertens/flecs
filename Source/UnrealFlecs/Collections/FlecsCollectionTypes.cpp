// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionTypes.h"

#include "Logging/StructuredLog.h"

#include "Logs/FlecsCategories.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionTypes)

void FFlecsCollectionParametersComponent::ApplyParameters(const FFlecsEntityHandle& InEntityHandle,
	const FInstancedStruct& InParameters) const
{
	solid_checkf(InEntityHandle.IsValid(),
		TEXT("FFlecsCollectionParametersComponent::ApplyParameters: InEntityHandle is not valid"));
	solid_checkf(InParameters.IsValid(),
		TEXT("FFlecsCollectionParametersComponent::ApplyParameters: InParameters is not valid"));
	solid_checkf(ApplyParametersFunction,
		TEXT("FFlecsCollectionParametersComponent::ApplyParameters: ApplyParametersFunction is not valid"));
	
	std::invoke(ApplyParametersFunction, InEntityHandle, InParameters);
}
