// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEntityHandleTypes.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldConverter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandleTypes)

FFlecsCommonHandle::FFlecsCommonHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity)
{
	SetEntity(flecs::entity(InWorld->World, InEntity));
}

FFlecsCommonHandle::FFlecsCommonHandle(const flecs::world_t* InWorld, const FFlecsId InEntity)
{
	SetEntity(flecs::entity(InWorld, InEntity));
}

TSolidNotNull<UFlecsWorld*> FFlecsCommonHandle::GetFlecsWorld() const
{
	return Unreal::Flecs::ToFlecsWorld(GetEntity().world());
}

TSolidNotNull<UWorld*> FFlecsCommonHandle::GetOuterWorld() const
{
	solid_checkf(::IsValid(GetFlecsWorld()), TEXT("Flecs World not found"));
	return GetFlecsWorld()->GetWorld();
}

FString FFlecsCommonHandle::GetWorldName() const
{
	return GetFlecsWorld()->GetWorldName();
}

FFlecsId FFlecsCommonHandle::ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const
{
	return GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
}

FFlecsId FFlecsCommonHandle::ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const
{
	return GetFlecsWorld()->ObtainComponentTypeEnum(EnumType);
}

FFlecsId FFlecsCommonHandle::ObtainTypeClass(const TSolidNotNull<UClass*> ClassType) const
{
	return GetFlecsWorld()->ObtainTypedEntity(ClassType);
}

FFlecsId FFlecsCommonHandle::GetTagEntity(const FGameplayTag& InTag) const
{
	return GetFlecsWorld()->GetTagEntity(InTag);
}
