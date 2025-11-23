// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once


#include <functional>

#include "CoreMinimal.h"

#include "UObject/Object.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"
#include "Standard/robin_hood.h"

#define UE_API SOLIDMACROS_API

#define START_SOLID_ASSET_VERSION(Name) \
	struct F##Name##CustomVersion \
	{ \
		static const FGuid GUID; \
		\
		enum Type \
		{ \
			/** Before any version changes were made */ \
			BeforeCustomVersionWasAdded = 0,

#define DEFINE_SOLID_ASSET_VERSION(VersionName) \
			VersionName,

#define END_SOLID_ASSET_VERSION() \
			\
			/* -----<new versions can be added above this line>----- */ \
			VersionPlusOne, \
			LatestVersion = VersionPlusOne - 1 \
		}; \
	};

#define GET_SOLID_ASSET_VERSION_VALUE(ObjectType, VersionName) \
	F##ObjectType##CustomVersion::VersionName

#define IMPLEMENT_SOLID_ASSET_VERSION(Name, GuidA, GuidB, GuidC, GuidD, FriendlyName) \
	const FGuid F##Name##CustomVersion::GUID(GuidA, GuidB, GuidC, GuidD);             \
	static FCustomVersionRegistration GRegister_##Name##CustomVersion(                 \
		F##Name##CustomVersion::GUID,                                                 \
		F##Name##CustomVersion::LatestVersion,                                        \
		TEXT(FriendlyName)                                                            \
	)

#define DECLARE_SOLID_VERSION_TYPE_TRAITS(Name) \
	template<> \
	struct TStructOpsTypeTraits<Name> : public TStructOpsTypeTraitsBase2<Name> \
	{ \
		enum \
		{ \
			WithIdentical = true, \
			WithStructuredSerializer = true, \
		}; \
	};

// @TODO: compile this out in non-editor builds?

namespace Solid
{
	class UE_API FAssetMigrationRegistry
	{
	public:
		using FStepFunctionType = std::function<void(const TSolidNotNull<UObject*>)>;

		struct FAssetMigrationStep
		{
			int32 ToVersion;
			FStepFunctionType StepFunction;
		}; // struct FAssetMigrationStep

		static FAssetMigrationRegistry& Get()
		{
			static FAssetMigrationRegistry GAssetMigrationRegistry;
			
			return GAssetMigrationRegistry;
		}

		void Register(const TSolidNotNull<const UClass*> ForClass, const int32 ToVersion, FStepFunctionType Func)
		{
			TArray<FAssetMigrationStep>& Array = Steps.FindOrAdd(ForClass);
			Array.Add({ .ToVersion = ToVersion, .StepFunction = MoveTemp(Func) });
			
			Array.Sort([](const FAssetMigrationStep& Left, const FAssetMigrationStep& Right)
			{
				return Left.ToVersion < Right.ToVersion;
			});
		}

		bool Migrate(const TSolidNotNull<UObject*> Object, const int32 FromVersion, const int32 ToVersion) const  // NOLINT(modernize-use-nodiscard)
		{
			bool bChanged = false;
			
			if (const TArray<FAssetMigrationStep>* Array = Steps.Find(Object->GetClass()))
			{
				for (const FAssetMigrationStep& Step : *Array)
				{
					if (Step.ToVersion > FromVersion && Step.ToVersion <= ToVersion)
					{
						Step.StepFunction(Object);
						bChanged = true;
					}
				}
			}
			
			return bChanged;
		}

	private:
		TMap<const UClass*, TArray<FAssetMigrationStep>> Steps;
		
	}; // class FAssetMigrationRegistry

	NO_DISCARD static bool FPropertyMatchesCDO(const TSolidNotNull<const UObject*> Object, const TSolidNotNull<const FProperty*> Property);
	NO_DISCARD static bool PropertyMatchesCDO(const TSolidNotNull<const UObject*> Object, const FName PropertyName);
	
} // namespace Solid

#if WITH_EDITOR

#define REGISTER_ASSET_MIGRATION_STEP(ClassType, ToVersion, FunctionBody) \
	namespace \
	{ \
		struct FAutoRegister##ClassType##ToVersion \
		{ \
			FAutoRegister##ClassType##ToVersion() \
			{ \
				Solid::FAssetMigrationRegistry::Get().Register(ClassType::StaticClass(), GET_SOLID_ASSET_VERSION_VALUE(ClassType, ToVersion), \
					[](const TSolidNotNull<UObject*> InAssetObject) \
					{ \
						const TSolidNotNull<ClassType*> Self = CastChecked<ClassType>(InAssetObject); \
						const TSolidNotNull<ClassType*> Archetype = CastChecked<ClassType>(Self->GetArchetype()); \
						 \
						FunctionBody \
					}); \
			} \
		}; \
		static FAutoRegister##ClassType##ToVersion AutoRegister##ClassType##ToVersionInstance; \
	}

#ifndef IMPLEMENT_ASSET_MIGRATION_POST_LOAD

#define IMPLEMENT_ASSET_MIGRATION_POST_LOAD(ObjectType) \
		do \
		{ \
			if (HasAnyFlags(RF_ClassDefaultObject)) \
			{ \
				break; \
			} \
			 \
			const int32 FromVersion = GetLinkerCustomVersion(F##ObjectType##CustomVersion::GUID); \
			const int32 ToVersion = F##ObjectType##CustomVersion::LatestVersion; \
			 \
			if (Solid::FAssetMigrationRegistry::Get().Migrate(TSolidNotNull<UObject*>(this), FromVersion, ToVersion)) \
			{ \
				MarkPackageDirty(); \
			} \
		} while (0)
		
#endif // IMPLEMENT_ASSET_MIGRATION_POST_LOAD

#else // !WITH_EDITOR

#define REGISTER_ASSET_MIGRATION_STEP(ClassType, ToVersion, FunctionBody) UE_EMPTY
#define IMPLEMENT_ASSET_MIGRATION_POST_LOAD(ObjectType) UE_EMPTY

#endif // WITH_EDITOR


#undef UE_API // SOLIDVERSIONING_API