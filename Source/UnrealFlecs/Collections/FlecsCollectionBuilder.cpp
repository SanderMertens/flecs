// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionBuilder.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionBuilder)

FFlecsSubEntityCollectionBuilder& FFlecsSubEntityCollectionBuilder::ReferenceCollection(
	const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset, const FInstancedStruct& InParameters)
{
	FFlecsCollectionInstancedReference Ref;
	Ref.Collection = FFlecsCollectionReference::FromAsset(InAsset);
	Ref.Parameters = InParameters;
		
	FFlecsSubEntityCollectionReferences& SubEntityCollectionReferences
		= ParentBuilder->CollectionDefinition->SubEntityCollections.FindChecked(SlotIndex);

	SubEntityCollectionReferences.CollectionReferences.Add(Ref);
		
	return *this;
}

FFlecsSubEntityCollectionBuilder& FFlecsSubEntityCollectionBuilder::ReferenceCollection(const FFlecsCollectionId& InId,
	const FInstancedStruct& InParameters)
{
	FFlecsCollectionInstancedReference Ref;
	Ref.Collection = FFlecsCollectionReference::FromId(InId);
	Ref.Parameters = InParameters;
		
	ParentBuilder->Add(Ref);
		
	return *this;
}

FFlecsSubEntityCollectionBuilder& FFlecsSubEntityCollectionBuilder::ReferenceCollection(const UClass* InClass,
	const FInstancedStruct& InParameters)
{
	FFlecsCollectionInstancedReference Ref;
	Ref.Collection = FFlecsCollectionReference::FromClass(const_cast<UClass*>(InClass)); // NOLINT(cppcoreguidelines-pro-type-const-cast)
	Ref.Parameters = InParameters;
		
	ParentBuilder->Add(Ref);
		
	return *this;
}

FFlecsCollectionBuilder& FFlecsSubEntityCollectionBuilder::EndSubEntity() const
{
	solid_cassume(ParentBuilder);
	ParentBuilder->CollectionDefinition->SubEntityCollections[SlotIndex].SubEntityName = IdName;
	
	return *ParentBuilder;
}
