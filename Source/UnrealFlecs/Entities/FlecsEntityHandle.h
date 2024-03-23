// Solstice Games © 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsType.h"
#include "InstancedStruct.h"
#include "SolidMacros/Macros.h"
#include "FlecsEntityHandle.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsEntityHandle& InEntity)
	{
		return GetTypeHash(InEntity.GetEntity().id());
	}

	FORCEINLINE static NO_DISCARD FFlecsEntityHandle GetNullHandle()
	{
		return flecs::entity::null();
	}

public:
	FFlecsEntityHandle() = default;
	FORCEINLINE FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	
	FORCEINLINE NO_DISCARD flecs::entity GetEntity() const { return Entity; }
	
	FORCEINLINE operator flecs::entity() const { return GetEntity(); }
	FORCEINLINE operator flecs::id_t() const { return GetEntity().id(); }
	FORCEINLINE operator flecs::entity_view() const { return GetEntity().view(); }
	
	FORCEINLINE NO_DISCARD bool IsValid() const { return GetEntity().is_valid(); }
	FORCEINLINE NO_DISCARD bool IsAlive() const { return GetEntity().is_alive(); }

	FORCEINLINE operator bool() const { return IsValid(); }

	FORCEINLINE NO_DISCARD int32 GetId() const { return GetEntity().id(); }
	
	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const;
	FORCEINLINE NO_DISCARD UWorld* GetOuterWorld() const;
	
	FORCEINLINE NO_DISCARD FName GetWorldName() const;
	
	FORCEINLINE NO_DISCARD FFlecsType GetType() const { return FFlecsType(GetEntity().type()); }

	FORCEINLINE NO_DISCARD bool Has(const FFlecsEntityHandle& InEntity) const { return GetEntity().has(InEntity); }

	template <typename T>
	FORCEINLINE NO_DISCARD bool Has() const { return GetEntity().has<T>(); }

	FORCEINLINE NO_DISCARD bool Has(UScriptStruct* StructType) const;

	FORCEINLINE void Add(const FFlecsEntityHandle& InEntity) const { GetEntity().add(InEntity); }

	FORCEINLINE void Add(UScriptStruct* StructType) const;
	
	template <typename T>
	FORCEINLINE void Add() const { GetEntity().template add<T>(); }
	
	FORCEINLINE void Remove(const FFlecsEntityHandle& InEntity) const { GetEntity().remove(InEntity); }

	FORCEINLINE void Remove(UScriptStruct* StructType) const;

	template <typename T>
	FORCEINLINE void Remove() const { GetEntity().template remove<T>(); }

	FORCEINLINE void Set(const FFlecsEntityHandle& InEntity) const { GetEntity().set(InEntity); }

	template <typename T>
	FORCEINLINE void Set(const T& InValue) const { GetEntity().template set<T>(InValue); }

	FORCEINLINE void Set(const FFlecsEntityHandle& InEntity, const void* InValue) const
	{
		GetEntity().set_ptr(InEntity, InValue);
	}

	FORCEINLINE void Set(UScriptStruct* StructType, const void* InValue) const;

	FORCEINLINE void Set(const FInstancedStruct& InValue) const;

	FORCEINLINE void Clear() const { GetEntity().clear(); }

	FORCEINLINE void Enable() const { GetEntity().enable(); }
	FORCEINLINE void Disable() const { GetEntity().disable(); }

	template <typename T>
	FORCEINLINE void Enable() const { GetEntity().template enable<T>(); }

	template <typename T>
	FORCEINLINE void Disable() const { GetEntity().template disable<T>(); }

	FORCEINLINE void Toggle() const { GetEntity().enable(!IsEnabled()); }

	FORCEINLINE NO_DISCARD bool IsEnabled() const { return GetEntity().enabled(); }

	FORCEINLINE void Destroy() const { GetEntity().destruct(); }

	FORCEINLINE NO_DISCARD FFlecsEntityHandle Clone(const bool bCloneValue = true, const int32 DestinationId = 0) const
	{
		return GetEntity().clone(bCloneValue, DestinationId);
	}

	FORCEINLINE void SetName(const FName& InName) const { GetEntity().set_name(TCHAR_TO_ANSI(*InName.ToString())); }
	FORCEINLINE NO_DISCARD FName GetName() const { return FName(GetEntity().name().c_str()); }

	FORCEINLINE void SetDocBrief(const FString& InDocBrief) const { GetEntity().set_doc_brief(TCHAR_TO_ANSI(*InDocBrief)); }
	FORCEINLINE NO_DISCARD FString GetDocBrief() const { return FString(GetEntity().doc_brief()); }

	FORCEINLINE void SetDocColor(const FString& Link) const { GetEntity().set_doc_color(TCHAR_TO_ANSI(*Link)); }
	FORCEINLINE NO_DISCARD FString GetDocColor() const { return FString(GetEntity().doc_color()); }

	FORCEINLINE void SetDocName(const FString& InDocName) const { GetEntity().set_doc_name(TCHAR_TO_ANSI(*InDocName)); }
	FORCEINLINE NO_DISCARD FString GetDocName() const { return FString(GetEntity().doc_name()); }

	FORCEINLINE void SetDocLink(const FString& InDocLink) const { GetEntity().set_doc_link(TCHAR_TO_ANSI(*InDocLink)); }
	FORCEINLINE NO_DISCARD FString GetDocLink() const { return FString(GetEntity().doc_link()); }

	FORCEINLINE void SetDocDetails(const FString& InDocDetails) const { GetEntity().set_doc_detail(TCHAR_TO_ANSI(*InDocDetails)); }
	FORCEINLINE NO_DISCARD FString GetDocDetails() const { return FString(GetEntity().doc_detail()); }

	FORCEINLINE void SetPair(const FFlecsEntityHandle& InRelation, const FFlecsEntityHandle& InTarget) const
	{
		GetEntity().set_second(InRelation, InTarget);
	}

	FORCEINLINE NO_DISCARD bool IsPair() const
	{
		return GetEntity().is_pair();
	}
	
	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetParent() const
	{
		return GetEntity().parent();
	}

	FORCEINLINE NO_DISCARD bool IsPrefab() const
	{
		return GetEntity().has(flecs::Prefab);
	}

	FORCEINLINE void Flatten(const FFlecsEntityHandle& RelationshipEntity) const
	{
		GetEntity().flatten(RelationshipEntity, nullptr);
	}

	template <typename T>
	FORCEINLINE void Emit() const
	{
		GetEntity().template emit<T>();
	}

	template <typename T>
	FORCEINLINE void Emit(const T& InValue) const
	{
		GetEntity().template emit<T>(InValue);
	}

	FORCEINLINE void Emit(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().emit(InEntity);
	}

	template <typename FunctionType>
	FORCEINLINE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe(InFunction);
	}

	template <typename TEvent, typename FunctionType>
	FORCEINLINE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().template observe<TEvent>(InFunction);
	}

	template <typename FunctionType>
	FORCEINLINE void Observe(const FFlecsEntityHandle& InEntity, FunctionType&& InFunction) const
	{
		GetEntity().observe(InEntity, InFunction);
	}

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	FORCEINLINE NO_DISCARD bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const flecs::entity& Other) const
	{
		return GetEntity() != Other;
	}

	FORCEINLINE flecs::entity* operator->() { return &Entity; }
	FORCEINLINE const flecs::entity* operator->() const { return &Entity; }

	FORCEINLINE NO_DISCARD FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

	template <typename TEnum>
	FORCEINLINE NO_DISCARD TEnum ToConstant() const
	{
		return GetEntity().to_constant<TEnum>();
	}

	FORCEINLINE NO_DISCARD FString ToJson() const
	{
		return FString(GetEntity().to_json().c_str());
	}

	FORCEINLINE bool Serialize(FArchive& Ar) const
	{
		FString Json = ToJson();
		Ar << Json;
		return true;
	}

	FORCEINLINE bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) const
	{
		FString Json = ToJson();
		Ar << Json;
		bOutSuccess = true;
		return true;
	}

	template <typename FunctionType>
	FORCEINLINE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each(InFunction);
	}

	template <typename TFirst, typename FunctionType>
	FORCEINLINE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each<TFirst, FunctionType>(InFunction);
	}
	
private:
	flecs::entity Entity;
}; // struct FFlecsEntityHandle

template <>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithSerializer = true,
		WithNetSerializer = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
