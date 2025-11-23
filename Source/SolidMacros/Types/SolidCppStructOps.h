// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Misc/CoreDelegates.h"

#include "SolidMacros.h"
#include "SolidNotNull.h"
#include "Concepts/SolidConcepts.h"

struct SOLIDMACROS_API FSolidMoveableStructRegistry : public FNoncopyable
{
	using FMoveFunc = void(*)(void* Dest, void* Src);
	using FCopyFunc = void(*)(void* Dest, const void* Src);

	// @TODO: not used yet, but may be useful in the future
	struct FStructTypeHookInfo
	{
		FMoveFunc MoveConstructor = nullptr;
		FMoveFunc MoveAssignment = nullptr;
	}; // struct FStructTypeHookInfo
	
	static FSolidMoveableStructRegistry& Get();

	template <Solid::TScriptStructConcept TStructType>
	requires (std::is_move_constructible_v<TStructType> || std::is_move_assignable_v<TStructType>)
	void RegisterMovableScriptStruct()
	{
		const TSolidNotNull<const UScriptStruct*> ScriptStruct = TBaseStructure<TStructType>::Get();

		FStructTypeHookInfo TypeHookInfo;

		constexpr bool bIsMoveConstructible = std::is_move_constructible_v<TStructType>;
		constexpr bool bIsMoveAssignable = std::is_move_assignable_v<TStructType>;
		
		if constexpr (bIsMoveAssignable)
		{
			const FMoveFunc MoveAssignmentFunction = [](void* Dest, void* Src)
			{
				TStructType* DestStruct  = static_cast<TStructType*>(Dest);
				TStructType* SrcStruct  = static_cast<TStructType*>(Src);
				solid_cassume(DestStruct);
				solid_cassume(SrcStruct);
			
				*DestStruct = MoveTemp(*SrcStruct);
			};

			TypeHookInfo.MoveAssignment = MoveAssignmentFunction;
		}
		else
		{
			TypeHookInfo.MoveAssignment = nullptr;
		}

		if constexpr (bIsMoveConstructible)
		{
			// @TODO: add a restrict keyword to this?
			const FMoveFunc MoveConstructorFunction = [](void* Dest, void* Src)
			{
				TStructType* SrcStruct  = static_cast<TStructType*>(Src);
				solid_cassume(SrcStruct);
			
				new (Dest) TStructType(MoveTemp(*SrcStruct));
			};

			TypeHookInfo.MoveConstructor = MoveConstructorFunction;
		}
		else
		{
			TypeHookInfo.MoveConstructor = nullptr;
		}

		solid_checkf(TypeHookInfo.MoveConstructor || TypeHookInfo.MoveAssignment,
			TEXT("At least one of MoveConstructor or MoveAssignment must be valid for moveable struct registration!"));

		MoveableStructs.Add(ScriptStruct, MoveTemp(TypeHookInfo));
	}

	template <Solid::TScriptStructConcept TStructType>
	requires (!std::is_move_constructible_v<TStructType> && !std::is_move_assignable_v<TStructType>)
	void RegisterMovableScriptStruct()
	{
		// do nothing
	}

	template <typename TStructType>
	requires (!Solid::TScriptStructConcept<TStructType>)
	void RegisterMovableScriptStruct()
	{
		// do nothing
	}
	
	NO_DISCARD bool IsStructMovable(const TSolidNotNull<const UScriptStruct*> InStruct) const;
	NO_DISCARD bool IsStructMoveConstructible(const TSolidNotNull<const UScriptStruct*> InStruct) const;
	NO_DISCARD bool IsStructMoveAssignable(const TSolidNotNull<const UScriptStruct*> InStruct) const;
	
	NO_DISCARD const FStructTypeHookInfo& GetStructTypeHookInfo(const TSolidNotNull<const UScriptStruct*> InStruct) const;

private:
	TMap<TObjectPtr<const UScriptStruct>, FStructTypeHookInfo> MoveableStructs;
	
}; // struct FSolidMoveableStructRegistry

namespace Solid
{
	// @TODO: add a restrict keyword to this?
	inline void MoveConstructScriptStruct(const TSolidNotNull<const UScriptStruct*> InStruct, void* Dest, void* Src,
	                             const uint32 Count = 1)
	{
		solid_checkf(IsValid(InStruct), TEXT("InStruct must be valid!"));
			
		solid_cassume(Dest);
		solid_cassume(Src);

		solid_checkf(FSolidMoveableStructRegistry::Get().IsStructMovable(InStruct),
					 TEXT("InStruct is not registered as moveable!"));
			
		const FSolidMoveableStructRegistry::FMoveFunc MoveFunction =
			FSolidMoveableStructRegistry::Get().GetStructTypeHookInfo(InStruct).MoveConstructor;
				
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			MoveFunction(
				reinterpret_cast<uint8*>(Dest) + Index * InStruct->GetStructureSize(),
				reinterpret_cast<uint8*>(Src) + Index * InStruct->GetStructureSize()
				);
		}
	}

	inline void MoveAssignScriptStruct(const TSolidNotNull<const UScriptStruct*> InStruct, void* Dest, void* Src,
	                            const uint32 Count = 1)
	{
		solid_checkf(IsValid(InStruct), TEXT("InStruct must be valid!"));
		
		solid_cassume(Dest);
		solid_cassume(Src);

		solid_checkf(FSolidMoveableStructRegistry::Get().IsStructMovable(InStruct),
					 TEXT("InStruct is not registered as moveable!"));
		
		const FSolidMoveableStructRegistry::FMoveFunc MoveFunction =
			FSolidMoveableStructRegistry::Get().GetStructTypeHookInfo(InStruct).MoveAssignment;
			
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			MoveFunction(
				reinterpret_cast<uint8*>(Dest) + Index * InStruct->GetStructureSize(),
				reinterpret_cast<uint8*>(Src) + Index * InStruct->GetStructureSize()
				);
		}
	}

} // namespace Solid

#ifndef DEFINE_SOLID_MOVEABLE_CPP_STRUCT

#define DEFINE_SOLID_MOVEABLE_CPP_STRUCT(StructType) \
	namespace \
	{ \
		struct FSolidOpsAutoReg_##StructType \
		{ \
			FSolidOpsAutoReg_##StructType() \
			{ \
				FCoreDelegates::OnPostEngineInit.AddLambda([]() \
				{ \
					static_assert(Solid::TStaticStructConcept<StructType>, "StructType must be a USTRUCT"); \
					 \
					FSolidMoveableStructRegistry::Get().RegisterMovableScriptStruct<StructType>(); \
				}); \
			} \
		}; \
		static FSolidOpsAutoReg_##StructType GAutoReg_SolidOps_##StructType;     \
	} // namespace

#endif // DEFINE_SOLID_MOVEABLE_CPP_STRUCT