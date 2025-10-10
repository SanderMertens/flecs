// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCommonHandle.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldConverter.h"
#include "Worlds/UnrealFlecsWorldTag.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCommonHandle)

FFlecsCommonHandle::FFlecsCommonHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity)
{
	SetEntity(flecs::entity(InWorld->World, InEntity));
}

FFlecsCommonHandle::FFlecsCommonHandle(const flecs::world_t* InWorld, const FFlecsId InEntity)
{
	SetEntity(flecs::entity(InWorld, InEntity));
}

flecs::world FFlecsCommonHandle::GetNativeFlecsWorld() const
{
	return GetEntity().world();
}

TSolidNotNull<UFlecsWorld*> FFlecsCommonHandle::GetFlecsWorld() const
{
	solid_checkf(IsUnrealFlecsWorld(), TEXT("Entity is not in an Unreal Flecs World"));
	return Unreal::Flecs::ToFlecsWorld(GetEntity().world());
}

bool FFlecsCommonHandle::IsUnrealFlecsWorld() const
{
	return GetEntity().world().has<FUnrealFlecsWorldTag>();
}

TSolidNotNull<UWorld*> FFlecsCommonHandle::GetOuterWorld() const
{
	solid_checkf(IsUnrealFlecsWorld(), TEXT("Entity is not in an Unreal Flecs World"));
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

