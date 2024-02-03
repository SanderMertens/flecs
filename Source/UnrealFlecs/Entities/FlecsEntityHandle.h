// Solstice Games © 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Macros.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

public:
	FFlecsEntityHandle() = default;
	FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	
	FORCEINLINE NO_DISCARD flecs::entity GetEntity() const { return Entity; }
	
	FORCEINLINE operator flecs::entity() const { return GetEntity(); }
	FORCEINLINE operator flecs::id_t() const { return GetEntity().id(); }
	
	FORCEINLINE NO_DISCARD bool IsValid() const { return GetEntity().is_valid(); }
	FORCEINLINE NO_DISCARD bool IsAlive() const { return GetEntity().is_alive(); }

	FORCEINLINE NO_DISCARD bool Has(const FFlecsEntityHandle& InEntity) const { return GetEntity().has(InEntity); }

	template <typename T>
	FORCEINLINE NO_DISCARD bool Has() const { return GetEntity().has<T>(); }

	FORCEINLINE void Add(const FFlecsEntityHandle& InEntity) const { GetEntity().add(InEntity); }

	template <typename T>
	FORCEINLINE void Add() const { GetEntity().template add<T>(); }
	
	FORCEINLINE void Remove(const FFlecsEntityHandle& InEntity) const { GetEntity().remove(InEntity); }

	template <typename T>
	FORCEINLINE void Remove() const { GetEntity().template remove<T>(); }

	FORCEINLINE void Set(const FFlecsEntityHandle& InEntity) const { GetEntity().set(InEntity); }

	template <typename T>
	FORCEINLINE void Set(const T& InValue) const { GetEntity().template set<T>(InValue); }

	FORCEINLINE void Set(const FFlecsEntityHandle& InEntity, const void* InValue) const { GetEntity().set(InEntity, InValue); }

	FORCEINLINE void Clear() const { GetEntity().clear(); }

	FORCEINLINE void Enable() const { GetEntity().enable(); }
	FORCEINLINE void Disable() const { GetEntity().disable(); }

	FORCEINLINE NO_DISCARD bool IsEnabled() const { return GetEntity().enabled(); }

	FORCEINLINE void Destroy() const { GetEntity().destruct(); }

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

	bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	bool operator!=(const flecs::entity& Other) const
	{
		return GetEntity() != Other;
	}

	FORCEINLINE flecs::entity* operator->() { return &Entity; }
	FORCEINLINE const flecs::entity* operator->() const { return &Entity; }
	
private:
	flecs::entity Entity;
}; // struct FFlecsEntityHandle

template<>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithCopy = true,
	};
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
