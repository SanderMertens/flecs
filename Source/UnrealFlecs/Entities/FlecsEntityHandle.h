// Solstice Games © 2023. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsArchetype.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "Logs/FlecsCategories.h"
#include "SolidMacros/Macros.h"
#include "FlecsEntityHandle.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct alignas(8) UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

	FORCEINLINE_DEBUGGABLE NO_DISCARD friend uint32 GetTypeHash(const FFlecsEntityHandle& InEntity)
	{
		return GetTypeHash(InEntity.GetEntity().id());
	}

	FORCEINLINE_DEBUGGABLE static NO_DISCARD FFlecsEntityHandle GetNullHandle()
	{
		return flecs::entity::null();
	}

public:
	FFlecsEntityHandle();
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle(const flecs::entity_t& InEntity) : Entity(InEntity) 
	{
		FFlecsEntityHandle();
	}
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::entity GetEntity() const { return Entity; }

	FORCEINLINE_DEBUGGABLE void SetEntity(const flecs::entity& InEntity) { Entity = InEntity; }
	FORCEINLINE_DEBUGGABLE void SetEntity(const flecs::entity_t& InEntity)
	{
		*this = FFlecsEntityHandle(InEntity);
	}
	
	FORCEINLINE_DEBUGGABLE operator flecs::entity() const { return GetEntity(); }
	FORCEINLINE_DEBUGGABLE operator flecs::id_t() const { return GetEntity().id(); }
	FORCEINLINE_DEBUGGABLE operator flecs::entity_view() const { return GetEntity().view(); }
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsValid() const { return GetEntity().is_valid(); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsAlive() const { return GetEntity().is_alive(); }

	FORCEINLINE_DEBUGGABLE operator bool() const { return IsValid(); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD uint32 GetId() const { return GetEntity().id(); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD uint32 GetGeneration() const { return get_generation(GetEntity()); }
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD UFlecsWorld* GetFlecsWorld() const;
	FORCEINLINE_DEBUGGABLE NO_DISCARD UWorld* GetOuterWorld() const;
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetWorldName() const;
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsArchetype GetType() const { return FFlecsArchetype(GetEntity().type()); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has(const FFlecsEntityHandle& InEntity) const { return GetEntity().has(InEntity); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has() const { return GetEntity().has<T>(); }

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has() const { return GetEntity().has<First, Second>(); }

	template <typename First>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has(const FFlecsEntityHandle& InSecond) const { return GetEntity().has<First>(InSecond.GetEntity()); }

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has(const Second& InSecond) const { return GetEntity().has<First, Second>(InSecond); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has(const UScriptStruct* StructType) const
	{
		return Has(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has(const FGameplayTag& InTag) const
	{
		return Has(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE void Add(const FFlecsEntityHandle& InEntity) const { GetEntity().add(InEntity); }

	FORCEINLINE_DEBUGGABLE void Add(const UScriptStruct* StructType) const
	{
		Add(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE void Add(const FGameplayTag& InTag) const
	{
		Add(GetTagEntity(InTag));
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE void Add() const { GetEntity().add<T>(); }
	
	FORCEINLINE_DEBUGGABLE void Remove(const FFlecsEntityHandle& InEntity) const { GetEntity().remove(InEntity); }

	FORCEINLINE_DEBUGGABLE void Remove(const FFlecsEntityHandle InFirst, const FFlecsEntityHandle InSecond) const
	{
		GetEntity().remove(InFirst, InSecond);
	}

	FORCEINLINE_DEBUGGABLE void Remove(const UScriptStruct* StructType) const
	{
		Remove(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE void Remove(const FGameplayTag& InTag) const
	{
		Remove(GetTagEntity(InTag));
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Remove() const { GetEntity().remove<T>(); }

	FORCEINLINE_DEBUGGABLE void Set(const FFlecsEntityHandle& InEntity) const { GetEntity().set(InEntity); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Set(const T& InValue) const { GetEntity().set<T>(InValue); }

	FORCEINLINE_DEBUGGABLE void Set(const FFlecsEntityHandle& InEntity, const void* InValue) const
	{
		GetEntity().set_ptr(InEntity, InValue);
	}

	FORCEINLINE_DEBUGGABLE void Set(const UScriptStruct* StructType, const void* InValue) const
	{
		Set(ObtainComponentTypeStruct(StructType), InValue);
	}

	FORCEINLINE_DEBUGGABLE void Set(const FInstancedStruct& InValue) const
	{
		Set(ObtainComponentTypeStruct(InValue.GetScriptStruct()), InValue.GetMemory());
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T Get() const { return *GetEntity().get<T>(); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD void* GetPtr(const FFlecsEntityHandle& InEntity)
	{
		return GetEntity().get_mut(InEntity);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD const void* GetPtr(const FFlecsEntityHandle& InEntity) const
	{
		return GetEntity().get(InEntity);
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T* GetPtr() { return GetEntity().get_mut<T>(); }
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD const T* GetPtr() const { return GetEntity().get<T>(); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD void* GetPtr(const UScriptStruct* StructType)
	{
		return GetPtr(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD const void* GetPtr(const UScriptStruct* StructType) const
	{
		return GetPtr(ObtainComponentTypeStruct(StructType));
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T& GetRef() { return *GetEntity().get_ref<T>().get(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD const T& GetRef() const { return *GetEntity().get_ref<T>().get(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::ref<T> GetFlecsRef() const { return GetEntity().get_ref<T>(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::ref<T> GetFlecsRef() { return GetEntity().get_ref<T>(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool Has() { return GetEntity().has<T>(); }

	FORCEINLINE_DEBUGGABLE void Clear() const { GetEntity().clear(); }

	FORCEINLINE_DEBUGGABLE void Enable() const { GetEntity().enable(); }
	FORCEINLINE_DEBUGGABLE void Disable() const { GetEntity().disable(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Enable() const { GetEntity().enable<T>(); }

	FORCEINLINE_DEBUGGABLE void Enable(const FFlecsEntityHandle& InEntity) const { GetEntity().enable(InEntity); }
	
	FORCEINLINE_DEBUGGABLE void Enable(const UScriptStruct* StructType) const
	{
		Enable(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE void Enable(const FGameplayTag& InTag) const
	{
		Enable(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE void Disable(const FFlecsEntityHandle& InEntity) const { GetEntity().disable(InEntity); }
	
	FORCEINLINE_DEBUGGABLE void Disable(const UScriptStruct* StructType) const
	{
		Disable(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE void Disable(const FGameplayTag& InTag) const
	{
		Disable(GetTagEntity(InTag));
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Disable() const { GetEntity().disable<T>(); }

	FORCEINLINE_DEBUGGABLE void Toggle() const { IsEnabled() ? Disable() : Enable(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Toggle() const { IsEnabled<T>() ? Disable<T>() : Enable<T>(); }

	FORCEINLINE_DEBUGGABLE void Toggle(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().enable(InEntity, !IsEnabled(InEntity));
	}

	FORCEINLINE_DEBUGGABLE void Toggle(const UScriptStruct* StructType) const
	{
		Toggle(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE void Toggle(const FGameplayTag& InTag) const
	{
		Toggle(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsEnabled() const { return GetEntity().enabled(); }

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsEnabled() const { return GetEntity().enabled<T>(); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsEnabled(const FFlecsEntityHandle& InEntity) const
	{
		return GetEntity().enabled(InEntity);
	}
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsEnabled(const UScriptStruct* StructType) const
	{
		return IsEnabled(ObtainComponentTypeStruct(StructType));
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsEnabled(const FGameplayTag& InTag) const
	{
		return IsEnabled(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE void Destroy() const { GetEntity().destruct(); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle Clone(const bool bCloneValue = true, const int32 DestinationId = 0) const
	{
		return GetEntity().clone(bCloneValue, DestinationId);
	}

	FORCEINLINE_DEBUGGABLE void SetName(const FString& InName) const { GetEntity().set_name(StringCast<char>(*InName).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetName() const { return FString(GetEntity().name()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasName() const { return Has<flecs::Identifier>(flecs::Name); }

	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetSymbol() const { return FString(GetEntity().symbol()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSymbol() const { return Has<flecs::Identifier>(flecs::Symbol); }

	FORCEINLINE_DEBUGGABLE void SetDocBrief(const FString& InDocBrief) const { GetEntity().set_doc_brief(StringCast<char>(*InDocBrief).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetDocBrief() const { return FString(GetEntity().doc_brief()); }

	FORCEINLINE_DEBUGGABLE void SetDocColor(const FString& Link) const { GetEntity().set_doc_color(StringCast<char>(*Link).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetDocColor() const { return FString(GetEntity().doc_color()); }

	FORCEINLINE_DEBUGGABLE void SetDocName(const FString& InDocName) const { GetEntity().set_doc_name(StringCast<char>(*InDocName).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetDocName() const { return FString(GetEntity().doc_name()); }

	FORCEINLINE_DEBUGGABLE void SetDocLink(const FString& InDocLink) const { GetEntity().set_doc_link(StringCast<char>(*InDocLink).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetDocLink() const { return FString(GetEntity().doc_link()); }

	FORCEINLINE_DEBUGGABLE void SetDocDetails(const FString& InDocDetails) const { GetEntity().set_doc_detail(StringCast<char>(*InDocDetails).Get()); }
	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetDocDetails() const { return FString(GetEntity().doc_detail()); }

	FORCEINLINE_DEBUGGABLE void SetPair(const FFlecsEntityHandle& InRelation, const FFlecsEntityHandle& InTarget) const
	{
		GetEntity().set_second(InRelation, InTarget);
	}
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetParent() const
	{
		return GetEntity().parent();
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasParent() const
	{
		return GetEntity().parent().is_valid();
	}

	FORCEINLINE_DEBUGGABLE void SetParent(const FFlecsEntityHandle& InParent) const
	{
		GetEntity().child_of(InParent);
	}

	FORCEINLINE_DEBUGGABLE void SetParent(const FFlecsEntityHandle& InParent, const bool bIsA) const
	{
		GetEntity().child_of(InParent);

		if (bIsA)
		{
			GetEntity().is_a(InParent);
		}
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsPrefab() const
	{
		return Has(flecs::Prefab);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsComponent() const
	{
		return Has<flecs::Component>() || Has<flecs::untyped_component>();
	}
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::untyped_component GetUntypedComponent() const
	{
		solid_checkf(IsComponent(), TEXT("Entity is not a component"));
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::untyped_component GetUntypedComponent_Unsafe() const
	{
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Emit() const
	{
		GetEntity().emit<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Emit(const T& InValue) const
	{
		GetEntity().emit<T>(InValue);
	}

	FORCEINLINE_DEBUGGABLE void Emit(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().emit(InEntity);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Enqueue() const
	{
		GetEntity().enqueue<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Enqueue(const T& InValue) const
	{
		GetEntity().enqueue<T>(InValue);
	}

	FORCEINLINE_DEBUGGABLE void Enqueue(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().enqueue(InEntity.GetEntity());
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe(InFunction);
	}

	template <typename TEvent, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe<TEvent>(InFunction);
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Observe(const FFlecsEntityHandle& InEntity, FunctionType&& InFunction) const
	{
		GetEntity().observe(InEntity.GetEntity(), InFunction);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool operator!=(const flecs::entity& Other) const
	{
		return GetEntity() != Other;
	}

	FORCEINLINE_DEBUGGABLE flecs::entity* operator->() { return &Entity; }
	FORCEINLINE_DEBUGGABLE const flecs::entity* operator->() const { return &Entity; }

	FORCEINLINE_DEBUGGABLE NO_DISCARD FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

	template <typename TEnum>
	FORCEINLINE_DEBUGGABLE NO_DISCARD TEnum ToConstant() const
	{
		return GetEntity().to_constant<TEnum>();
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD FString ToJson(const bool bSerializePath = true,
		const bool bSerializeLabel = false, const bool bSerializeBrief = false, const bool bSerializeLink = false,
		const bool bSerializeColor = false, const bool bSerializeIds = true, const bool bSerializeIdLabels = false,
		const bool bSerializeBaseComponents = true, const bool bSerializeComponents = true) const
	{
		return FString(GetEntity().to_json().c_str());
	}

	FORCEINLINE_DEBUGGABLE void FromJson(const FString& InJson) const
	{
		GetEntity().from_json(StringCast<char>(*InJson).Get());
	}

	FORCEINLINE_DEBUGGABLE bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each(InFunction);
	}

	template <typename TFirst, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each<TFirst, FunctionType>(InFunction);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEntity() const
	{
		return Has(flecs::Trait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void AddTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Add<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void AddTrait(const UScriptStruct* StructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Add(StructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void AddTrait(const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Add(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void AddTrait(const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Add(InTrait);
	}

	FORCEINLINE_DEBUGGABLE void AddTrait(const UScriptStruct* StructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(StructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(StructType).Add(TraitType);
	}

	FORCEINLINE_DEBUGGABLE void AddTrait(const UScriptStruct* StructType, const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has(StructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(StructType).Add(InTag);
	}

	FORCEINLINE_DEBUGGABLE void AddTrait(const UScriptStruct* StructType, const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has(StructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(StructType).Add(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void RemoveTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Remove<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void RemoveTrait(const UScriptStruct* TraitStructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Remove(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void RemoveTrait(const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Remove(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void RemoveTrait(const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Remove(InTrait);
	}

	FORCEINLINE_DEBUGGABLE void RemoveTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Remove(TraitType);
	}

	FORCEINLINE_DEBUGGABLE void RemoveTrait(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Remove(InTag);
	}

	FORCEINLINE_DEBUGGABLE void RemoveTrait(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Remove(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE bool HasTrait() const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().Has<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE bool HasTrait(const UScriptStruct* TraitStructType) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().Has(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE bool HasTrait(const FGameplayTag& InTag) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().Has(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE bool HasTrait(const FFlecsEntityHandle& InTrait) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().Has(InTrait);
	}

	FORCEINLINE_DEBUGGABLE bool HasTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).Has(TraitType);
	}

	FORCEINLINE_DEBUGGABLE bool HasTrait(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).Has(InTag);
	}

	FORCEINLINE_DEBUGGABLE bool HasTrait(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).Has(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void SetTrait(const TTrait& InValue) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Set<TTrait>(InValue);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void SetTrait(const UScriptStruct* TraitStructType, const void* InValue) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Set(TraitStructType, InValue);
	}

	FORCEINLINE_DEBUGGABLE void SetTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType, const void* InValue) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Set(TraitType, InValue);
	}

	FORCEINLINE_DEBUGGABLE void SetTrait(const UScriptStruct* ComponentStructType, const FInstancedStruct& InValue) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Set(InValue);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE TTrait GetTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return TTrait();
		}
		
		return ObtainTraitHolderEntity<TComponent>().Get<TTrait>();
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE TTrait* GetTraitPtr()
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity<TComponent>().GetPtr<TTrait>();
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE const TTrait* GetTraitPtr() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity<TComponent>().GetPtr<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void* GetTraitPtr(const UScriptStruct* TraitStructType)
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity<TComponent>().GetPtr(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE const void* GetTraitPtr(const UScriptStruct* TraitStructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity<TComponent>().GetPtr(TraitStructType);
	}

	FORCEINLINE_DEBUGGABLE void* GetTraitPtr(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType)
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).GetPtr(TraitType);
	}

	FORCEINLINE_DEBUGGABLE const void* GetTraitPtr(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return nullptr;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).GetPtr(TraitType);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE TTrait& GetTraitRef() const
	{
		solid_checkf(Has<TComponent>(), "Entity does not have component %s", nameof(TComponent));
		return ObtainTraitHolderEntity<TComponent>().GetRef<TTrait>();
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE flecs::ref<TTrait> GetTraitFlecsRef() const
	{
		solid_checkf(Has<TComponent>(), "Entity does not have component %s", nameof(TComponent));
		return ObtainTraitHolderEntity<TComponent>().GetFlecsRef<TTrait>();
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void EnableTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Enable<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void EnableTrait(const UScriptStruct* TraitStructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Enable(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void EnableTrait(const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Enable(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void EnableTrait(const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Enable(InTrait);
	}

	FORCEINLINE_DEBUGGABLE void EnableTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Enable(TraitType);
	}

	FORCEINLINE_DEBUGGABLE void EnableTrait(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Enable(InTag);
	}

	FORCEINLINE_DEBUGGABLE void EnableTrait(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Enable(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void DisableTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Disable<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void DisableTrait(const UScriptStruct* TraitStructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Disable(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void DisableTrait(const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Disable(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void DisableTrait(const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Disable(InTrait);
	}

	FORCEINLINE_DEBUGGABLE void DisableTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Disable(TraitType);
	}

	FORCEINLINE_DEBUGGABLE void DisableTrait(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Disable(InTag);
	}

	FORCEINLINE_DEBUGGABLE void DisableTrait(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Disable(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled() const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().IsEnabled<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE bool IsTraitEnabled(const UScriptStruct* TraitStructType) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().IsEnabled(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled(const FGameplayTag& InTag) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().IsEnabled(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled(const FFlecsEntityHandle& InTrait) const
	{
		if (!Has<TComponent>())
		{
			return false;
		}
		
		return ObtainTraitHolderEntity<TComponent>().IsEnabled(InTrait);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).IsEnabled(TraitType);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).IsEnabled(InTag);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTraitEnabled(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if (!Has(ComponentStructType))
		{
			return false;
		}
		
		return ObtainTraitHolderEntity(ComponentStructType).IsEnabled(InTrait);
	}

	template <typename TComponent, typename TTrait>
	FORCEINLINE_DEBUGGABLE void ToggleTrait() const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Toggle<TTrait>();
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void ToggleTrait(const UScriptStruct* TraitStructType) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Toggle(TraitStructType);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void ToggleTrait(const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Toggle(InTag);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void ToggleTrait(const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has<TComponent>())
		{
			return;
		}
		
		ObtainTraitHolderEntity<TComponent>().Toggle(InTrait);
	}

	FORCEINLINE_DEBUGGABLE void ToggleTrait(const UScriptStruct* ComponentStructType, const UScriptStruct* TraitType) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Toggle(TraitType);
	}

	FORCEINLINE_DEBUGGABLE void ToggleTrait(const UScriptStruct* ComponentStructType, const FGameplayTag& InTag) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Toggle(InTag);
	}

	FORCEINLINE_DEBUGGABLE void ToggleTrait(const UScriptStruct* ComponentStructType, const FFlecsEntityHandle& InTrait) const
	{
		if UNLIKELY_IF(!Has(ComponentStructType))
		{
			return;
		}
		
		ObtainTraitHolderEntity(ComponentStructType).Toggle(InTrait);
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void Modified() const
	{
		GetEntity().modified<TComponent>();
	}
	
	FORCEINLINE_DEBUGGABLE void Modified(const UScriptStruct* StructType) const
	{
		GetEntity().modified(ObtainComponentTypeStruct(StructType));
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void Modified(const FGameplayTag& InTag) const
	{
		GetEntity().modified(GetTagEntity(InTag));
	}

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE void Modified(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().modified(InEntity);
	}

	FORCEINLINE_DEBUGGABLE void Modified(const FGameplayTag& InTag) const
	{
		GetEntity().modified(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE void Modified(const FFlecsEntityHandle& InEntity) const
	{
		GetEntity().modified(InEntity);
	}

	FORCEINLINE_DEBUGGABLE void Modified(const UScriptStruct* StructType, const UScriptStruct* TraitType) const
	{
		GetEntity().modified(ObtainTraitHolderEntity(StructType),
			ObtainTraitHolderEntity(TraitType));
	}

	FORCEINLINE_DEBUGGABLE void Modified(const UScriptStruct* StructType, const FGameplayTag& InTag) const
	{
		GetEntity().modified(GetTagEntity(InTag));
	}

	FORCEINLINE_DEBUGGABLE void Modified(const UScriptStruct* StructType, const FFlecsEntityHandle& InTrait) const
	{
		GetEntity().modified(ObtainTraitHolderEntity(StructType), InTrait);
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD int32 GetDepth(const FFlecsEntityHandle& InEntity) const
	{
		return GetEntity().depth(InEntity);
	}

	template <typename TEntity>
	FORCEINLINE_DEBUGGABLE NO_DISCARD int32 GetDepth() const
	{
		return GetEntity().depth<TEntity>();
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetPath() const
	{
		return FString(GetEntity().path());
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD FString GetPath(const FString& InSeparator,
		const FString& InitialSeparator) const
	{
		return FString(GetEntity().path(StringCast<char>(*InSeparator).Get(),
			StringCast<char>(*InitialSeparator).Get()));
	}
	
	#if WITH_EDITORONLY_DATA

	UPROPERTY()
	FName DisplayName;

	#endif // WITH_EDITORONLY_DATA
	
private:
	flecs::entity Entity;

	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle ObtainComponentTypeStruct(const UScriptStruct* StructType) const;
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetTagEntity(const FGameplayTag& InTag) const;

	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::world GetFlecsWorld_Internal() const { return GetEntity().world(); }

	template <typename TComponent>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle ObtainTraitHolderEntity() const
	{
		solid_checkf(Has<TComponent>(), TEXT("Entity does not have component"));

		FFlecsEntityHandle TraitHolder;

		if LIKELY_IF(GetEntity().has<TComponent>(flecs::Wildcard))
		{
			TraitHolder
				= GetEntity()
					.target_for<TComponent>(flecs::ChildOf);

			if LIKELY_IF(TraitHolder.IsValid())
			{
				return TraitHolder;
			}
		}

		const flecs::world World = GetFlecsWorld_Internal();
		TraitHolder = World.entity();
		TraitHolder.Add(flecs::Trait);
		TraitHolder.SetParent(GetEntity());

		GetEntity().add<TComponent>(TraitHolder);
		
		return TraitHolder;
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle ObtainTraitHolderEntity(const UScriptStruct* StructType) const
	{
		solid_checkf(Has(StructType), TEXT("Entity does not have component"));
		
		FFlecsEntityHandle TraitHolder;

		if LIKELY_IF(GetEntity().has(ObtainComponentTypeStruct(StructType), flecs::Wildcard))
		{
			TraitHolder
				= GetEntity()
					.target_for(ObtainComponentTypeStruct(StructType), flecs::ChildOf);

			if LIKELY_IF(TraitHolder.IsValid())
			{
				return TraitHolder;
			}
		}
		
		const flecs::world World = GetFlecsWorld_Internal();
		TraitHolder = World.entity();
		TraitHolder.Add(flecs::Trait);
		TraitHolder.SetParent(GetEntity(), true);

		GetEntity().add(ObtainComponentTypeStruct(StructType), TraitHolder);
		
		return TraitHolder;
	}
	
}; // struct FFlecsEntityHandle

template <>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithNetSerializer = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>

FORCEINLINE_DEBUGGABLE bool IsValid(const FFlecsEntityHandle& Test)
{
	return Test.IsValid();
}
