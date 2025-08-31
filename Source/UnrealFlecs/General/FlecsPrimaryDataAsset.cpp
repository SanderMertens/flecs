// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsPrimaryDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPrimaryDataAsset)

bool UFlecsPrimaryDataAsset::ShouldSpawn() const
{
	return bEnabledOnStartup && !GetClass()->HasAnyClassFlags(CLASS_Abstract);
}
