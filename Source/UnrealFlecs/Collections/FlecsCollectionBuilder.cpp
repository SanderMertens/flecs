// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionBuilder.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionBuilder)

const FFlecsCollectionBuilder& FFlecsSubEntityBuilder::End() const
{
	solid_checkf(ParentBuilder, TEXT("ParentBuilder is null"));
	return *ParentBuilder;
}
