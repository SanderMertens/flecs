// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <functional>

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsEntityHandleTypes.h"
#include "FlecsArchetype.h"
#include "FlecsEntityView.h"
#include "FlecsId.h"
#include "GameplayTagContainer.h"
#include "General/FlecsStringConverters.h"
#include "SolidMacros/Macros.h"
#include "StructUtils/InstancedStruct.h"
#include "Types/SolidEnumSelector.h"
#include "Types/SolidNotNull.h"
#include "FlecsEntityHandle.generated.h"

struct FFlecsEntityHandle;
class UFlecsWorld;

#define DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(Name) \
	extern Unreal::Flecs::FEntityNetSerializeFunction Name;

#define DEFINE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(Name, Lambda) \
	Unreal::Flecs::FEntityNetSerializeFunction Name = Lambda;

namespace Unreal::Flecs
{
	using FEntityNetSerializeFunction
		= std::function<bool(FFlecsEntityHandle&, TSolidNotNull<UFlecsWorld*>, FArchive&, UPackageMap*, bool&)>;

	UNREALFLECS_API DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(EmptyNetSerializeFunction);

	UNREALFLECS_API extern Unreal::Flecs::FEntityNetSerializeFunction* GNetSerializeFunctionPtr;
	
} // namespace Unreal::Flecs

/**
 * @struct FFlecsEntityHandle
 *
 * A handle for managing flecs entities in Unreal Engine with added blueprint support.
 * The structure provides several utility functions to interact with flecs entities,
 * including validation, component addition/removal, and direct data access.
 * This must be used with a valid `UFlecsWorld` instance to function correctly.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct alignas(8) UNREALFLECS_API FFlecsEntityHandle : public FFlecsEntityView
{
	GENERATED_BODY()

	using FSelfType = FFlecsEntityHandle;

public:

	static NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetNullHandle()
	{
		return FFlecsEntityHandle(flecs::entity::null());
	}

	 static NO_DISCARD FFlecsEntityHandle GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	FFlecsEntityHandle() = default;
	
	SOLID_INLINE FFlecsEntityHandle(const flecs::entity& InEntity) : FFlecsEntityView(InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsEntityHandle(const FFlecsId InEntity) : FFlecsEntityView(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityHandle(const flecs::world& InWorld, const FFlecsId InEntity) : FFlecsEntityView(InWorld, InEntity)
	{
	}

	FFlecsEntityHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity);

	FFlecsEntityHandle(const flecs::world_t* InWorld, const FFlecsId InEntity);
	
	NO_DISCARD SOLID_INLINE FFlecsArchetype GetType() const
	{
		return FFlecsArchetype(GetEntity().type());
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Has() const
	{
		return GetEntity().has<T>();
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE bool Has(const T& InValue) const
	{
		return GetEntity().has(FFlecsCommonHandle::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType) const
	{
		return HasPair(FFlecsCommonHandle::GetInputId(*this, EnumType), flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(EnumType, InValue);
		
		return HasPair(EnumEntity, EnumConstant);
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE bool Has(const T InValue) const
	{
		return GetEntity().has<T>(InValue);
	}

	NO_DISCARD SOLID_INLINE bool Has(const FSolidEnumSelector& EnumSelector) const
	{
		return Has(EnumSelector.Class, EnumSelector.Value);
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Add(const T& InValue) const
	{
		GetEntity().add(FFlecsCommonHandle::GetInputId(*this, InValue));
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Add(const UEnum* EnumType, const uint64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);

		const FFlecsId ValueEntity = ObtainEnumConstant(EnumType, InValue);
		solid_check(ValueEntity.IsValid());
		
		AddPair(EnumEntity, ValueEntity);
		return *this;
	}

	SOLID_INLINE const FSelfType& Add(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			Add(Tag);
		}

		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Add() const
	{
		GetEntity().add<T>();
		return *this;
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	SOLID_INLINE const FSelfType& Add(const T InValue) const
	{
		GetEntity().add<T>(InValue);
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Remove() const
	{
		GetEntity().remove<T>();
		return *this;
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	SOLID_INLINE const FSelfType& Remove() const
	{
		RemovePair<T>(flecs::Wildcard);
		return *this;
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Remove(const T& InValue) const
	{
		GetEntity().remove(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const UEnum* EnumType) const
	{
		RemovePair(FFlecsEntityHandle::GetInputId(*this, EnumType), flecs::Wildcard);
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(EnumType, InValue);
		
		RemovePair(EnumEntity, EnumConstant);
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const FSolidEnumSelector& EnumSelector) const
	{
		return Remove(EnumSelector.Class, EnumSelector.Value);
	}

	SOLID_INLINE const FSelfType& Remove(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			if (!Has(Tag))
			{
				continue;
			}
			
			Remove(Tag);
		}

		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Set(const T& InValue) const
	{
		GetEntity().set<T>(InValue);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& Set(T&& InValue) const
	{
		GetEntity().set(std::forward<T>(InValue));
		return *this;
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE const FSelfType& Set(const TFunction& InFunction) const
	{
		GetEntity().set(InFunction);
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Set(const FFlecsId InId, const uint32 InSize, const void* InValue) const
	{
		if (!Has(InId))
		{
			Add(InId);
		}
		
		GetEntity().set_ptr(InId, InSize, InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	SOLID_INLINE const FSelfType& Set(const T& InTypeValue, const void* InData) const
	{
		const FFlecsId InId = FFlecsEntityHandle::GetInputId(*this, InTypeValue);
		
		if (!Has(InId))
		{
			Add(InId);
		}

		if constexpr (std::is_convertible_v<T, const UScriptStruct*>)
		{
			Set(InId, InTypeValue->GetStructureSize(), InData);
		}
		else
		{
			GetEntity().set_ptr(InId, InData);
		}
		
		return *this;
	}

	SOLID_INLINE const FSelfType& Set(const FInstancedStruct& InValue) const
	{
		Set(FFlecsEntityHandle::GetInputId(*this, InValue.GetScriptStruct()),
			InValue.GetScriptStruct()->GetStructureSize(),
			InValue.GetMemory());
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Assign(const T& InValue) const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());

		GetEntity().assign<T>(InValue);
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Assign(const FFlecsId InEntity, const uint32 InSize, const void* InValue) const
	{
		solid_checkf(Has(InEntity),
			TEXT("Entity does not have component with id %llu"), InEntity.GetId());
		
		GetEntity().set_ptr(InEntity, InSize, InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	SOLID_INLINE const FSelfType& Assign(const T& InTypeValue, const void* InValue) const
	{
		const FFlecsId InId = FFlecsEntityHandle::GetInputId(*this, InTypeValue);
		solid_checkf(Has(InId),
			TEXT("Entity does not have component with id %llu"), InId.GetId());
		
		if constexpr (std::is_convertible_v<T, const UScriptStruct*>)
		{
			Assign(InId, InTypeValue->GetStructureSize(), InValue);
		}
		else
		{
			GetEntity().set_ptr(InId, InValue);
		}
		
		return *this;
	}

	SOLID_INLINE const FSelfType& Assign(const FInstancedStruct& InValue) const
	{
		solid_checkf(Has(InValue.GetScriptStruct()),
			TEXT("Entity does not have component with script struct %s"),
			*InValue.GetScriptStruct()->GetStructCPPName());
		
		Assign(InValue.GetScriptStruct(), InValue.GetMemory());
		return *this;
	}
	
	template <typename T>
	NO_DISCARD SOLID_INLINE const T& Get() const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		return GetEntity().get<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T& GetMut() const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		return GetEntity().get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD SOLID_INLINE const T* GetPtr() const
	{
		return GetEntity().try_get<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T* GetMutPtr() const
	{
		return GetEntity().try_get_mut<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE const void* GetPtr(const T& InTypeValue) const
	{
		return GetEntity().try_get(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE void* GetMutPtr(const T& InTypeValue) const
	{
		return GetEntity().try_get_mut(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE flecs::ref<T> GetFlecsRef() const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		
		return GetEntity().get_ref<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE flecs::untyped_ref GetFlecsRef(const T& InTypeValue) const
	{
		solid_checkf(Has(FFlecsEntityHandle::GetInputId(*this, InTypeValue)),
			TEXT("Entity does not have component with id %llu"),
			FFlecsEntityHandle::GetInputId(*this, InTypeValue).GetId());
		
		return GetEntity().get_ref(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE bool GetLambda(TFunction&& InFunction) const
	{
		return GetEntity().get(std::forward<TFunction>(InFunction));
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE bool Insert(const TFunction& InFunction) const
	{
		return GetEntity().insert<TFunction>(InFunction);
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE T GetEnumValue() const
	{
		return GetEntity().get_constant<T>();
	}
	
	NO_DISCARD SOLID_INLINE uint64 GetEnumValue(const UEnum* EnumType) const
	{
		const FFlecsEntityHandle Target = GetPairTarget(EnumType);
		solid_check(Target.IsValid());
		
		return Target.ToConstant(EnumType);
	}

	SOLID_INLINE void Clear() const
	{
		GetEntity().clear();
	}

	SOLID_INLINE const FSelfType& Enable() const
	{
		GetEntity().enable();
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Disable() const
	{
		GetEntity().disable();
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& Enable() const
	{
		GetEntity().enable<T>();
		return *this;
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Enable(const T& InValue) const
	{
		GetEntity().enable(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Disable() const
	{
		GetEntity().disable<T>();
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Disable(const T& InValue) const
	{
		GetEntity().disable(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	SOLID_INLINE const FSelfType& Toggle() const
	{
		IsEnabled() ? Disable() : Enable();
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& Toggle() const
	{
		IsEnabled<T>() ? Disable<T>() : Enable<T>();
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Toggle(const T& InValue) const
	{
		GetEntity().enable(FFlecsEntityHandle::GetInputId(*this, InValue), !IsEnabled(InValue));
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntity().enabled();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntity().enabled<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	NO_DISCARD SOLID_INLINE bool IsEnabled(const T& InValue) const
	{
		return GetEntity().enabled(FFlecsEntityHandle::GetInputId(*this, InValue));
	}

	SOLID_INLINE void Destroy() const
	{
		GetEntity().destruct();
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Clone(const bool bCloneValue = true,
	                                                 const FFlecsId DestinationId = 0) const
	{
		return GetEntity().clone(bCloneValue, DestinationId);
	}

	SOLID_INLINE const FSelfType& SetName(const FString& InName) const
	{
		GetEntity().set_name(Unreal::Flecs::ToCString(InName));
		return *this;
	}

	SOLID_INLINE const FSelfType& ClearName() const
	{
		GetEntity().set_name(nullptr);
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetName() const
	{
		return FString(GetEntity().name().c_str(), static_cast<int32>(GetEntity().name().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasName() const
	{
		return DoesOwnPair<flecs::Identifier>(flecs::Name);
	}

	NO_DISCARD SOLID_INLINE FString GetSymbol() const
	{
		return FString(GetEntity().symbol().c_str(), static_cast<int32>(GetEntity().symbol().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasSymbol() const
	{
		return DoesOwnPair<flecs::Identifier>(flecs::Symbol);
	}

	SOLID_INLINE const FSelfType& SetDocBrief(const FString& InDocBrief) const
	{
		GetEntity().set_doc_brief(Unreal::Flecs::ToCString(InDocBrief));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocBrief() const
	{
		return GetEntity().doc_brief();
	}

	SOLID_INLINE const FSelfType& SetDocColor(const FString& Link) const
	{
		GetEntity().set_doc_color(Unreal::Flecs::ToCString(Link));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocColor() const
	{
		return GetEntity().doc_color();
	}

	SOLID_INLINE const FSelfType& SetDocName(const FString& InDocName) const
	{
		GetEntity().set_doc_name(Unreal::Flecs::ToCString(InDocName));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocName() const
	{
		return GetEntity().doc_name();
	}

	SOLID_INLINE const FSelfType& SetDocLink(const FString& InDocLink) const
	{
		GetEntity().set_doc_link(Unreal::Flecs::ToCString(InDocLink));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocLink() const
	{
		return GetEntity().doc_link();
	}

	SOLID_INLINE const FSelfType& SetDocDetails(const FString& InDocDetails) const
	{
		GetEntity().set_doc_detail(Unreal::Flecs::ToCString(InDocDetails));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocDetails() const
	{
		return GetEntity().doc_detail();
	}
	
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetParent() const
	{
		return GetEntity().parent();
	}

	NO_DISCARD SOLID_INLINE bool HasParent() const
	{
		return HasPair(flecs::ChildOf, flecs::Wildcard);
	}

	SOLID_INLINE const FSelfType& SetParent(const FFlecsId InParent) const
	{
		GetEntity().child_of(InParent);
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool IsPrefab() const
	{
		return Has(flecs::Prefab);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool DoesOwn() const
	{
		return GetEntity().owns<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE bool DoesOwn(const T& InTypeValue) const
	{
		return GetEntity().owns(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	template <typename First, typename Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair() const
	{
		return GetEntity().owns<First, Second>();
	}

	template <typename First, Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const Second& InSecondTypeValue) const
	{
		return GetEntity().owns<First>(FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntity().owns(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	NO_DISCARD SOLID_INLINE bool IsComponent() const
	{
		return Has<flecs::Component>();
	}

	NO_DISCARD SOLID_INLINE bool IsTag() const
	{
		return !IsComponent() || Get<flecs::Component>().size == 0;
	}

	NO_DISCARD SOLID_INLINE bool IsEnum() const
	{
		return Has<flecs::Enum>();
	}

	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent() const
	{
		solid_checkf(IsComponent(), TEXT("Entity is not a component"));
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent_Unsafe() const
	{
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	template <typename T>
	SOLID_INLINE void Emit() const
	{
		GetEntity().emit<T>();
	}

	template <typename T>
	SOLID_INLINE void Emit(const T& InValue) const
	{
		GetEntity().emit<T>(InValue);
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE void Emit(const T& InValue) const
	{
		GetEntity().emit(FFlecsEntityHandle::GetInputId(*this, InValue));
	}

	template <typename T>
	SOLID_INLINE void Enqueue() const
	{
		GetEntity().enqueue<T>();
	}

	template <typename T>
	SOLID_INLINE void Enqueue(const T& InValue) const
	{
		GetEntity().enqueue<T>(InValue);
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE void Enqueue(const T& InValue) const
	{
		GetEntity().enqueue(FFlecsEntityHandle::GetInputId(*this, InValue));
	}

	template <typename TEvent, typename FunctionType>
	SOLID_INLINE const FSelfType& Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe<TEvent>(std::forward<FunctionType>(InFunction));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T, typename FunctionType>
	SOLID_INLINE const FSelfType& Observe(const T& InValue, FunctionType&& InFunction) const
	{
		GetEntity().observe(FFlecsEntityHandle::GetInputId(*this, InValue), std::forward<FunctionType>(InFunction));
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	NO_DISCARD SOLID_INLINE bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}
	
	NO_DISCARD SOLID_INLINE bool operator==(const FFlecsId Other) const
	{
		return GetFlecsId() == Other;
	}

	NO_DISCARD SOLID_INLINE bool operator!=(const FFlecsId Other) const
	{
		return GetFlecsId() != Other;
	}
	
	SOLID_INLINE flecs::entity operator->() const
	{
		return GetEntity();
	}

	SOLID_INLINE void ResetHandle()
	{
		Entity = flecs::entity::null();
	}

	SOLID_INLINE FFlecsEntityHandle& operator=(TYPE_OF_NULLPTR)
	{
		ResetHandle();
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

	template <typename TEnum>
	NO_DISCARD SOLID_INLINE TEnum ToConstant() const
	{
		return GetEntity().to_constant<TEnum>();
	}
	
	NO_DISCARD SOLID_INLINE uint64 ToConstant(const TSolidNotNull<const UEnum*> InEnumType) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(InEnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const uint64 MaxEnumValue = InEnumType->GetMaxEnumValue();
		
		if (MaxEnumValue <= std::numeric_limits<uint8>::max())
		{
			return GetEntity().to_constant<uint8>(EnumEntity);
		}
		else
		{
			return GetEntity().to_constant<uint64>(EnumEntity);
		}
	}

	NO_DISCARD SOLID_INLINE FString ToJson(const bool bSerializePath = true,
		const bool bSerializeLabel = false, const bool bSerializeBrief = false, const bool bSerializeLink = false,
		const bool bSerializeColor = false, const bool bSerializeIds = true, const bool bSerializeIdLabels = false,
		const bool bSerializeBaseComponents = true, const bool bSerializeComponents = true) const
	{
		return FString(GetEntity().to_json().c_str());
	}

	NO_DISCARD SOLID_INLINE FString ToJson(const ecs_entity_to_json_desc_t& InDesc) const
	{
		return FString(GetEntity().to_json(&InDesc).c_str());
	}

	SOLID_INLINE FString FromJson(const FString& InJson) const
	{
		return GetEntity().from_json(Unreal::Flecs::ToCString(InJson));
	}
	
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	template <typename TFirst, typename TSecond>
	SOLID_INLINE const FSelfType& AddPair() const
	{
		GetEntity().add<TFirst, TSecond>();
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AddPair(const TSecond& InSecond) const
	{
		GetEntity().add<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AddPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		GetEntity().add(FFlecsEntityHandle::GetInputId(*this, InFirst),
			FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	SOLID_INLINE const FSelfType& AddPair(const FFlecsId InFirst, UEnum* InSecond, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(InSecond);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(InSecond, InValue);
		
		AddPair(InFirst, EnumConstant);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& AddPairSecond(const TFirst& InFirst) const
	{
		GetEntity().add_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
		return *this;
	}

	template <typename First, typename Second>
	SOLID_INLINE const FSelfType& RemovePair() const
	{
		GetEntity().remove<First, Second>();
		return *this;
	}

	template <typename First, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& RemovePair(const Second& InSecond) const
	{
		GetEntity().remove<First>(FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& RemovePair(const First& InFirst, const Second& InSecond) const
	{
		GetEntity().remove(FFlecsEntityHandle::GetInputId(*this, InFirst),
			FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& RemovePairSecond(const TFirst& InFirst) const
	{
		GetEntity().remove_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
		return *this;
	}

	template <typename TFirst, typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair() const
	{
		return GetEntity().has<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TSecond& InSecond) const
	{
		return GetEntity().has<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		return GetEntity().has(FFlecsEntityHandle::GetInputId(*this, InFirst),
			FFlecsEntityHandle::GetInputId(*this, InSecond));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const TFirst& InFirst) const
	{
		return GetEntity().has_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE const TActual& GetPairFirst() const
	{
		solid_checkf((HasPair<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntity().get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond,
	typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE TActual& GetPairMutFirst() const
	{
		solid_checkf((HasPair<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntity().get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE const TFirst& GetPairFirst(const TSecond& InSecond) const
	{
		solid_checkf(HasPair<TFirst>(InSecond),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TFirst).data(),
			*InSecond.ToString());
		
		return GetEntity().get<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE TFirst& GetPairMutFirst(const TSecond& InSecond) const
	{
		solid_checkf(HasPair<TFirst>(InSecond),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TFirst).data(),
			*InSecond.ToString());
		
		return GetEntity().get_mut<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE const TActual* GetPairPtrFirst() const
	{
		return GetEntity().try_get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE TActual* GetPairPtrMutFirst() const
	{
		return GetEntity().try_get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE const TFirst* GetPairPtrFirst(const TSecond& InSecond) const
	{
		return GetEntity().try_get<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE TFirst* GetPairPtrMutFirst(const TSecond& InSecond) const
	{
		return GetEntity().try_get_mut<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* GetPairPtrFirst(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntity().try_get(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* GetPairPtrMutFirst(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntity().try_get_mut(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE const TActual& GetPairSecond() const
	{
		solid_checkf((HasPairSecond<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntity().get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE TActual& GetPairMutSecond() const
	{
		solid_checkf((HasPairSecond<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntity().get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE const TSecond& GetPairSecond(const TFirst& InFirst) const
	{
		solid_checkf(HasPairSecond<TSecond>(InFirst),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TSecond).data(),
			*InFirst.ToString());
		
		return GetEntity().get_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE TSecond& GetPairMutSecond(const TFirst& InFirst) const
	{
		solid_checkf(HasPairSecond<TSecond>(InFirst),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TSecond).data(),
			*InFirst.ToString());
		
		return GetEntity().get_mut_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE const TActual* GetPairPtrSecond() const
	{
		return GetEntity().try_get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE TActual* GetPairPtrMutSecond() const
	{
		return GetEntity().try_get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE const TSecond* GetPairPtrSecond(const TFirst& InFirst) const
	{
		return GetEntity().try_get_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE TSecond* GetPairPtrMutSecond(const TFirst& InFirst) const
	{
		return GetEntity().try_get_mut_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* GetPairPtrSecond(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntity().try_get(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* GetPairPtrMutSecond(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntity().try_get_mut(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <typename TFirst>
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetPairTarget(const int32 Index = 0) const
	{
		return GetEntity().target<TFirst>(Index);
	}

	template <Unreal::Flecs::TFlecsEntityFunctionDataTypeWithEnumNoValueConcept TFirst>
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetPairTarget(const TFirst& InFirstTypeValue, const int32 Index = 0) const
	{
		return GetEntity().target(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue), Index);
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	SOLID_INLINE const FSelfType& SetPairFirst(const TActual& InValue) const
	{
		GetEntity().set<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	SOLID_INLINE const FSelfType& SetPairSecond(const TActual& InValue) const
	{
		GetEntity().set_second<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& SetPairFirst(const TSecond& InSecondType, const TFirst& InValue) const
	{
		GetEntity().set<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondType), InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& SetPairFirst(const First& InFirstTypeValue, const void* InValue,
		const Second& InSecondTypeValue) const
	{
		if (!HasPair(InFirstTypeValue, InSecondTypeValue))
		{
			AddPair(InFirstTypeValue, InSecondTypeValue);
		}

		Set(FFlecsId::MakePair(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue), FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InSecondTypeValue->GetStructureSize(),
				InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = TSecond>
	SOLID_INLINE const FSelfType& SetPairSecond(const TActual& InSecond) const
	{
		GetEntity().set_second<TFirst, TSecond>(InSecond);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& SetPairSecond(const TFirst& InFirstType, const TSecond& InValue) const
	{
		GetEntity().set_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstType), InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TSecond>
	SOLID_INLINE const FSelfType& SetPairSecond(const TFirst& InFirstTypeValue,
		const TSecond& InSecondTypeValue, const void* InValue) const
	{
		if (!HasPair(InFirstTypeValue, InSecondTypeValue))
		{
			AddPair(InFirstTypeValue, InSecondTypeValue);
		}

		Set(FFlecsId::MakePair(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue), FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InSecondTypeValue->GetStructureSize(),
				InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	SOLID_INLINE const FSelfType& AssignPairFirst(const TActual& InValue) const
	{
		GetEntity().assign<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	SOLID_INLINE const FSelfType& AssignPairSecond(const TActual& InValue) const
	{
		GetEntity().assign_second<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AssignPairFirst(const TSecond& InSecondType, const TFirst& InValue) const
	{
		GetEntity().assign<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondType), InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& AssignPairFirst(const First& InFirstTypeValue, const void* InValue,
		const Second& InSecondTypeValue) const
	{
		solid_checkf(HasPair(InFirstTypeValue, InSecondTypeValue), 
			TEXT("Entity does not have pair"));

		Assign(FFlecsId::MakePair(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue), FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InSecondTypeValue->GetStructureSize(),
				InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = TSecond>
	SOLID_INLINE const FSelfType& AssignPairSecond(const TActual& InSecond) const
	{
		GetEntity().assign_second<TFirst, TSecond>(InSecond);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& AssignPairSecond(const TFirst& InFirstType, const TSecond& InValue) const
	{
		GetEntity().assign_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstType), InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AssignPairSecond(const TFirst& InFirstTypeValue,
		const TSecond& InSecondTypeValue, const void* InValue) const
	{
		solid_checkf(HasPair(InFirstTypeValue, InSecondTypeValue),
			TEXT("Entity does not have pair"));

		Assign(FFlecsId::MakePair(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue), FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InSecondTypeValue->GetStructureSize(),
				InValue);
		return *this;
	}
	
	template <typename TFirst, typename TSecond>
	SOLID_INLINE void ModifiedPair() const
	{
		GetEntity().modified<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE void ModifiedPair(const TSecond& InSecondTypeValue) const
	{
		GetEntity().modified<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}
	
	template <typename TComponent>
	SOLID_INLINE void Modified() const
	{
		GetEntity().modified<TComponent>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TComponent>
	SOLID_INLINE void Modified(const TComponent& InValueValue) const
	{
		GetEntity().modified(FFlecsEntityHandle::GetInputId(*this, InValueValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE void ModifiedPair(const TFirst& InFirstTypeValue, const TSecond& InSecondTypeValue) const
	{
		GetEntity().modified(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE void ModifiedPairSecond(const TFirst& InFirstTypeValue) const
	{
		GetEntity().modified<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T& Obtain() const
	{
		return GetEntity().obtain<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE T& Obtain(const T& InTypeValue) const
	{
		return GetEntity().obtain(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	template <typename TEntity>
	NO_DISCARD SOLID_INLINE int32 GetDepth() const
	{
		return GetEntity().depth<TEntity>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TComponent>
	NO_DISCARD SOLID_INLINE int32 GetDepth(const TComponent& InValue) const
	{
		return GetEntity().depth(FFlecsEntityHandle::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE FString GetPath() const
	{
		return FString(GetEntity().path());
	}

	NO_DISCARD SOLID_INLINE FString GetPath(const FString& InSeparator, const FString& InitialSeparator) const
	{
		return FString(GetEntity().path(Unreal::Flecs::ToCString(InSeparator),
								Unreal::Flecs::ToCString(InitialSeparator)));
	}

	template <typename TEnumUnderlying = uint64>
	NO_DISCARD SOLID_INLINE FFlecsId ObtainEnumConstant(const TSolidNotNull<const UEnum*> EnumType,
	                                                              const TEnumUnderlying InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		return EnumEntity.Lookup(EnumType->GetNameStringByValue(static_cast<int64>(InValue)));
	}

	SOLID_INLINE const FSelfType& AddPrefab(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& AddPrefab() const
	{
		GetEntity().is_a<T>();
		return *this;
	}

	SOLID_INLINE const FSelfType& RemovePrefab(const FFlecsId InPrefab) const
	{
		RemovePair(flecs::IsA, InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& RemovePrefab() const
	{
		RemovePairSecond<T>(flecs::IsA);
		return *this;
	}
	
	SOLID_INLINE const FSelfType& SetIsA(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& SetIsA() const
	{
		GetEntity().is_a<T>();
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool HasPrefab(const FFlecsId InPrefab) const
	{
		return HasPair(flecs::IsA, InPrefab);
	}

	NO_DISCARD SOLID_INLINE bool HasPrefab(const TSolidNotNull<UClass*> InPrefabClass) const
	{
		return HasPair(flecs::IsA, FFlecsEntityHandle::GetInputId(*this, InPrefabClass));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool HasPrefab() const
	{
		return HasPairSecond<T>(flecs::IsA);
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Lookup(const FString& InPath, const bool bSearchPath = false) const
	{
		return GetEntity().lookup(Unreal::Flecs::ToCString(InPath), bSearchPath);
	}

	NO_DISCARD SOLID_INLINE bool IsInStage() const
	{
		return GetEntity().world().is_stage();
	}

	template <typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntity().each(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntity().each<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntity().each(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	template <typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntity().children(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntity().children<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntity().children(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	// @TODO: Make FFlecsEntityView instead of flecs::entity_view
	SOLID_INLINE FFlecsEntityHandle ToMut(const flecs::entity_view& InView) const
	{
		return GetEntity().mut(InView);
	}

	SOLID_INLINE FFlecsEntityHandle ToMut(const flecs::world& InWorld) const
	{
		return GetEntity().mut(InWorld);
	}

	SOLID_INLINE FFlecsEntityHandle ToMut(const flecs::iter& InIter) const
	{
		return GetEntity().mut(InIter);
	}

	SOLID_INLINE flecs::entity_view ToView() const
	{
		return GetEntity().view();
	}

	void AddCollection(TSolidNotNull<UObject*> Collection) const;
	
protected:
	
}; // struct FFlecsEntityHandle

template <>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
