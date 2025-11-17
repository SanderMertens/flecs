// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"
#include "Concepts/SolidConcepts.h"
#include "Types/SolidNotNull.h"

#include "FlecsEntityHandleTypes.h"
#include "FlecsCommonHandle.h"
#include "FlecsArchetype.h"

#include "FlecsEntityView.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityView : public FFlecsCommonHandle
{
	GENERATED_BODY()

	using FSelfType = FFlecsEntityView;
	
	static NO_DISCARD SOLID_INLINE FFlecsEntityView GetNullHandle()
	{
		return FFlecsEntityView(flecs::entity::null());
	}

	static NO_DISCARD SOLID_INLINE FFlecsEntityView Invalid()
	{
		return FFlecsEntityView(flecs::entity::null());
	}

	static NO_DISCARD FFlecsEntityView GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	SOLID_INLINE FFlecsEntityView() = default;
	
	SOLID_INLINE FFlecsEntityView(const flecs::entity& InEntity) : FFlecsCommonHandle(InEntity)
	{
	}
	
	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const flecs::entity& InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world_t* InWorld, const flecs::entity_t InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsEntityView(const FFlecsId InEntity) : FFlecsCommonHandle(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const FFlecsId InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::entity_view& View)
		: FFlecsCommonHandle(View)
	{
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::entity_view& InView) const
	{
		return GetEntityView().mut(InView);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::world& InWorld) const
	{
		return GetEntityView().mut(InWorld);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::iter& InIter) const
	{
		return GetEntityView().mut(InIter);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Has() const
	{
		return GetEntityView().has<T>();
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE bool Has(const T& InValue) const
	{
		return GetEntityView().has(FFlecsCommonHandle::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType) const
	{
		return HasPair(FFlecsCommonHandle::GetInputId(*this, EnumType), flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant<FFlecsId>(EnumType, InValue);
		
		return HasPair(EnumEntity, EnumConstant);
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE bool Has(const T InValue) const
	{
		return GetEntityView().has<T>(InValue);
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE bool Has(const FFlecsId InValue) const
	{
		return GetEntityView().has<T>(FFlecsCommonHandle::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE bool Has(const FSolidEnumSelector& EnumSelector) const
	{
		return Has(EnumSelector.Class, EnumSelector.Value);
	}

	template <typename TFirst, typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair() const
	{
		return GetEntityView().has<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TSecond& InSecond) const
	{
		return GetEntityView().has<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionUEnumTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE bool HasPair(const TFirst& InFirst, const int64 InSecond) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(InFirst);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant<FFlecsId>(InFirst, InSecond);
		
		return GetEntityView().has(FFlecsEntityView::GetInputId(*this, InFirst), EnumConstant);
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		return GetEntityView().has(FFlecsEntityView::GetInputId(*this, InFirst),
			FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().has_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE const T& Get() const
	{
		return GetEntityView().get<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T& GetMut() const
	{
		return GetEntityView().get_mut<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE const T* TryGet() const
	{
		return GetEntityView().try_get<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T* TryGetMut() const
	{
		return GetEntityView().try_get_mut<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE const void* TryGet(const T& InTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE void* TryGetMut(const T& InTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE bool GetLambda(TFunction&& InFunction) const
	{
		return GetEntityView().get(std::forward<TFunction>(InFunction));
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE T GetEnumValue() const
	{
		return GetEntity().get_constant<T>();
	}

	NO_DISCARD SOLID_INLINE uint64 GetEnumValue(const UEnum* EnumType) const
	{
		const FFlecsEntityView Target = GetPairTarget<FFlecsEntityView>(EnumType);
		
		return Target.ToConstant(EnumType);
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE const TActual& GetPairFirst() const
	{
		return GetEntityView().get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE TActual& GetPairFirstMut() const
	{
		return GetEntityView().get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE const TFirst& GetPairFirst(const TSecond& InSecond) const
	{
		return GetEntityView().get<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE TFirst& GetPairFirstMut(const TSecond& InSecond) const
	{
		return GetEntityView().get_mut<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE const TActual* TryGetPairFirst() const
	{
		return GetEntityView().try_get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual>)
	NO_DISCARD SOLID_INLINE TActual* TryGetPairFirstMut() const
	{
		return GetEntityView().try_get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE const TFirst* TryGetPairFirst(const TSecond& InSecond) const
	{
		return GetEntityView().try_get<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE TFirst* TryGetPairFirstMut(const TSecond& InSecond) const
	{
		return GetEntityView().try_get_mut<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* TryGetPairFirst(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* TryGetPairFirstMut(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE const TActual& GetPairSecond() const
	{
		return GetEntityView().get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual>)
	NO_DISCARD SOLID_INLINE TActual& GetPairSecondMut() const
	{
		return GetEntityView().get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE const TSecond& GetPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().get_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE TSecond& GetPairSecondMut(const TFirst& InFirst) const
	{
		return GetEntityView().get_mut_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	NO_DISCARD SOLID_INLINE const TActual* TryGetPairSecond() const
	{
		return GetEntityView().try_get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	NO_DISCARD SOLID_INLINE TActual* TryGetPairSecondMut() const
	{
		return GetEntityView().try_get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE const TSecond* TryGetPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().try_get_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE TSecond* TryGetPairSecondMut(const TFirst& InFirst) const
	{
		return GetEntityView().try_get_mut_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* TryGetPairSecond(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* TryGetPairSecondMut(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle GetPairTarget(const int32 Index = 0) const
	{
		return GetEntityView().target<TFirst>(Index);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle,
		Unreal::Flecs::TFlecsEntityFunctionDataTypeWithEnumNoValueConcept TFirst>
	NO_DISCARD SOLID_INLINE THandle GetPairTarget(const TFirst& InFirstTypeValue, const int32 Index = 0) const
	{
		return GetEntityView().target(FFlecsEntityView::GetInputId(*this, InFirstTypeValue), Index);
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

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle GetParent() const
	{
		return GetEntityView().parent();
	}

	NO_DISCARD SOLID_INLINE bool HasParent() const
	{
		return HasPair(flecs::ChildOf, flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool IsPrefab() const
	{
		return Has(flecs::Prefab);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Owns() const
	{
		return GetEntityView().owns<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE bool Owns(const T& InTypeValue) const
	{
		return GetEntityView().owns(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <typename First, typename Second>
	NO_DISCARD SOLID_INLINE bool OwnsPair() const
	{
		return GetEntityView().owns<First, Second>();
	}

	template <typename First, Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool OwnsPair(const Second& InSecondTypeValue) const
	{
		return GetEntityView().owns<First>(FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool OwnsPair(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().owns(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <typename Second, Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First>
	NO_DISCARD SOLID_INLINE bool OwnsPairSecond(const First& InFirstTypeValue) const
	{
		return GetEntityView().owns_second<Second>(FFlecsEntityView::GetInputId(*this, InFirstTypeValue));
	}

	NO_DISCARD SOLID_INLINE bool IsA(const FFlecsId InPrefab) const
	{
		return HasPair(flecs::IsA, InPrefab);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool IsA() const
	{
		return HasPairSecond<T>(flecs::IsA);
	}

	// @TODO: Optimize this to not require looking up the enum entity each time
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle, typename TEnumUnderlying = uint64>
	NO_DISCARD SOLID_INLINE THandle ObtainEnumConstant(const TSolidNotNull<const UEnum*> EnumType,
																  const TEnumUnderlying InValue) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		return EnumEntity.Lookup<THandle>(EnumType->GetNameStringByValue(static_cast<int64>(InValue)));
	}

	// @TODO: make this default to FFlecsEntityView or handle maybe?
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle Lookup(const FString& InPath, const bool bSearchPath = false) const
	{
		return GetEntityView().lookup(StringCast<char>(*InPath).Get(), bSearchPath);
	}

	template <typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntityView().each(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntityView().each<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntityView().each(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	template <typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntityView().children(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntityView().children<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntityView().children(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	template <typename TEnum>
	NO_DISCARD SOLID_INLINE TEnum ToConstant() const
	{
		return GetEntityView().to_constant<TEnum>();
	}

	template <typename TInt>
	NO_DISCARD SOLID_INLINE TInt ToConstant(const FFlecsId InEnumType) const
	{
		return GetEntityView().to_constant<TInt>(InEnumType);
	}
	
	NO_DISCARD SOLID_INLINE uint64 ToConstant(const TSolidNotNull<const UEnum*> InEnumType) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(InEnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const uint64 MaxEnumValue = InEnumType->GetMaxEnumValue();
		
		if (MaxEnumValue <= std::numeric_limits<uint8>::max())
		{
			return ToConstant<uint8>(EnumEntity);
		}
		else
		{
			return ToConstant<uint64>(EnumEntity);
		}
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntityView().enabled();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntityView().enabled<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	NO_DISCARD SOLID_INLINE bool IsEnabled(const T& InValue) const
	{
		return GetEntityView().enabled(FFlecsEntityView::GetInputId(*this, InValue));
	}

	template <typename TFirst, typename TSecond>
	NO_DISCARD SOLID_INLINE bool IsEnabledPair() const
	{
		return GetEntityView().enabled<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool IsEnabledPair(const TSecond& InSecond) const
	{
		return GetEntityView().enabled<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool IsEnabledPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		return GetEntityView().enabled(FFlecsEntityView::GetInputId(*this, InFirst),
			FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE bool IsEnabledPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().enabled_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	NO_DISCARD SOLID_INLINE FString GetName() const
	{
		return FString(GetEntity().name().c_str(), static_cast<int32>(GetEntity().name().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasName() const
	{
		return OwnsPair<flecs::Identifier>(flecs::Name);
	}

	NO_DISCARD SOLID_INLINE FString GetSymbol() const
	{
		return FString(GetEntityView().symbol().c_str(), static_cast<int32>(GetEntityView().symbol().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasSymbol() const
	{
		return OwnsPair<flecs::Identifier>(flecs::Symbol);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle Clone(const bool bCloneValue = true, const FFlecsId DestinationId = 0) const
	{
		return GetEntityView().clone(bCloneValue, DestinationId);
	}

	NO_DISCARD SOLID_INLINE FString ToJson() const
	{
		return FString(GetEntityView().to_json().c_str());
	}

	NO_DISCARD SOLID_INLINE FString ToJson(const ecs_entity_to_json_desc_t& InDesc) const
	{
		return FString(GetEntityView().to_json(&InDesc).c_str());
	}

	template <typename TEntity>
	NO_DISCARD SOLID_INLINE int32 GetDepth() const
	{
		return GetEntityView().depth<TEntity>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TComponent>
	NO_DISCARD SOLID_INLINE int32 GetDepth(const TComponent& InValue) const
	{
		return GetEntityView().depth(FFlecsEntityView::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE FString GetPath() const
	{
		return FString(GetEntityView().path());
	}

	NO_DISCARD SOLID_INLINE FString GetPath(const FString& InSeparator, const FString& InitialSeparator) const
	{
		return FString(GetEntityView().path(StringCast<char>(*InSeparator).Get(),
			StringCast<char>(*InitialSeparator).Get()));
	}

	NO_DISCARD SOLID_INLINE FFlecsArchetype GetType() const
	{
		return FFlecsArchetype(GetEntityView().type());
	}

	NO_DISCARD SOLID_INLINE bool HasCollection(const FFlecsId InCollection) const
	{
		return IsA(InCollection);
	}

	NO_DISCARD SOLID_INLINE bool HasCollection(UClass* InCollection) const
	{
		return HasCollection(ObtainTypeClass(InCollection));
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD SOLID_INLINE bool HasCollection() const
	{
		return HasCollection(T::StaticClass());
	}

#if defined(FLECS_DOC)

	NO_DISCARD SOLID_INLINE FString GetDocBrief() const
	{
		return GetEntityView().doc_brief();
	}

	NO_DISCARD SOLID_INLINE FString GetDocColor() const
	{
		return GetEntityView().doc_color();
	}

	NO_DISCARD SOLID_INLINE FString GetDocName() const
	{
		return GetEntityView().doc_name();
	}

	NO_DISCARD SOLID_INLINE FString GetDocLink() const
	{
		return GetEntityView().doc_link();
	}

	NO_DISCARD SOLID_INLINE FString GetDocDetails() const
	{
		return GetEntityView().doc_detail();
	}

#endif // #if defined(FLECS_DOC)

private:
	
}; // struct FFlecsEntityView
