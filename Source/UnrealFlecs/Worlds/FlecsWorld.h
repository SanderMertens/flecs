// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "ObjectCacheContext.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Components/ObjectTypes/FFlecsActorComponentTag.h"
#include "Components/ObjectTypes/FFlecsModuleComponentTag.h"
#include "Components/ObjectTypes/FFlecsSceneComponentTag.h"
#include "Components/ObjectTypes/FFlecsUObjectTag.h"
#include "Components/ObjectTypes/FlecsActorTag.h"
#include "Engine/AssetManager.h"
#include "Entities/FlecsEntityRecord.h"
#include "Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "General/FlecsObjectRegistrationInterface.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsDependenciesComponent.h"
#include "Modules/FlecsModuleInitEvent.h"
#include "Modules/FlecsModuleInterface.h"
#include "Pipelines/FlecsGameLoopInterface.h"
#include "Prefabs/FlecsPrefabAsset.h"
#include "UObject/PropertyIterator.h"
#include "FlecsWorld.generated.h"

class UFlecsWorldSubsystem;

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"),
	STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress::ProgressModule"),
	STAT_FlecsWorldProgressModule, STATGROUP_FlecsWorld);

/**
 * @brief Component type that represents if the World has begun play.
 * Can be found in the World entity.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsBeginPlay
{
	GENERATED_BODY()
}; // struct FFlecsBeginPlayEvent

UCLASS(BlueprintType, NotBlueprintable)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	UFlecsWorld()
	{
		char* argv[] = { const_cast<char*>(Unreal::Flecs::ToCString(GetName())) }; // NOLINT(clang-diagnostic-dangling, cppcoreguidelines-pro-type-const-cast)
		
		World = flecs::world(1, argv);
		
		TypeMapComponent = GetTypeMapComponent();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
		solid_check(TypeMapComponent);
	}
	
	virtual ~UFlecsWorld() override
	{
		FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);

		const FAssetRegistryModule* AssetRegistryModule
			= FModuleManager::LoadModulePtr<FAssetRegistryModule>("AssetRegistry");

		if (AssetRegistryModule && AssetRegistryModule->IsValid())
		{
			IAssetRegistry& AssetRegistry = AssetRegistryModule->Get();
			AssetRegistry.OnAssetAdded().RemoveAll(this);
			AssetRegistry.OnAssetRemoved().RemoveAll(this);
		}

		TypeMapComponent = nullptr;

		if LIKELY_IF(World)
		{
			World.release();
			World.world_ = nullptr;
		}
	}

	void WorldStart()
	{
		UE_LOG(LogFlecsWorld, Log, TEXT("Flecs World start"));
		
		#if WITH_AUTOMATION_TESTS
		if (!GIsAutomationTesting)
		{
		#endif // WITH_AUTOMATION_TESTS
			//InitializeAssetRegistry();
		#if WITH_AUTOMATION_TESTS
		}
		#endif // WITH_AUTOMATION_TESTS

		InitializeFlecsRegistrationObjects();
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void WorldBeginPlay()
	{
		GetWorldEntity().Add<FFlecsBeginPlay>();
	}

	void InitializeDefaultComponents() const
	{
		World.component<FFlecsEntityHandle>()
			.disable();
		
		World.component<FString>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FString* Data)
			{
				const TCHAR* CharArray = Data->GetCharArray().GetData();
				return Serializer->value(flecs::String, &CharArray);
			})
			.assign_string([](FString* Data, const char* String)
			{
				*Data = String;
			});

		World.component<FName>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FName* Data)
			{
				const FString String = Data->ToString();
				const TCHAR* CharArray = String.GetCharArray().GetData();
				return Serializer->value(flecs::String, &CharArray);
			})
			.assign_string([](FName* Data, const char* String)
			{
				*Data = FName(String);
			});

		World.component<FText>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FText* Data)
			{
				const FString String = Data->ToString();
				const TCHAR* CharArray = String.GetCharArray().GetData();
				return Serializer->value(flecs::String, &CharArray);
			})
			.assign_string([](FText* Data, const char* String)
			{
				*Data = FText::FromString(String);
			});

		 World.component<FGameplayTag>()
		 	.opaque(flecs::Entity)
		 	.serialize([](const flecs::serializer* Serializer, const FGameplayTag* Data)
		 	{
		 		const FFlecsId TagEntity = ecs_lookup(Serializer->world, Unreal::Flecs::ToCString(Data->ToString()));
		 		return Serializer->value(flecs::Entity, &TagEntity);
		 	});
		
		World.component<FObjectPtr>()
			.opaque(flecs::Uptr)
			.serialize([](const flecs::serializer* Serializer, const FObjectPtr* Data)
			{
				const UObject* Object = Data->Get();
				return Serializer->value(flecs::Uptr, std::addressof(Object));
			});
		
		World.component<FWeakObjectPtr>()
			.opaque(flecs::Uptr)
			.serialize([](const flecs::serializer* Serializer, const FWeakObjectPtr* Data)
			{
				const UObject* Object = Data->Get();
				return Serializer->value(flecs::Uptr, std::addressof(Object));
			})
			.assign_null([](FWeakObjectPtr* Data)
			{
				Data->Reset();
			});

		World.component<FSoftObjectPtr>()
			.opaque(flecs::Uptr)
			.serialize([](const flecs::serializer* Serializer, const FSoftObjectPtr* Data)
			{
				const UObject* Object = Data->Get();
				return Serializer->value(flecs::Uptr, std::addressof(Object));
			})
			.assign_null([](FSoftObjectPtr* Data)
			{
				Data->Reset();
			});

		World.component<TSubclassOf<UObject>>()
			.opaque(flecs::Uptr)
			.serialize([](const flecs::serializer* Serializer, const TSubclassOf<UObject>* Data)
			{
				const UClass* Class = Data->Get();
				return Serializer->value(flecs::Uptr, std::addressof(Class));
			})
			.assign_null([](TSubclassOf<UObject>* Data)
			{
				*Data = nullptr;
			});

		RegisterUnrealTypes();

		RegisterComponentType<FFlecsBeginPlay>();

		RegisterComponentType<FFlecsUObjectComponent>();
		
		RegisterComponentType<FFlecsUObjectTag>();
		
		RegisterComponentType<FFlecsActorTag>().SetIsA<FFlecsUObjectTag>();
		RegisterComponentType<FFlecsActorComponentTag>().SetIsA<FFlecsUObjectTag>();
		RegisterComponentType<FFlecsModuleComponentTag>().SetIsA<FFlecsUObjectTag>();
		
		RegisterComponentType<FFlecsSceneComponentTag>().SetIsA<FFlecsActorComponentTag>();

		RegisterComponentType<FFlecsModuleComponent>();
		RegisterComponentType<FFlecsModuleInitEvent>();
		RegisterComponentType<FFlecsDependenciesComponent>();

		RegisterComponentType<FFlecsEntityRecord>();
	}

	void InitializeFlecsRegistrationObjects()
	{
		for (TObjectIterator<UClass> It = TObjectIterator<UClass>(); It; ++It)
		{
			const TNotNull<const UClass*> Class = *It;
			
			if (Class->ImplementsInterface(UFlecsObjectRegistrationInterface::StaticClass()))
			{
				// @TODO: should we log if the class is abstract?
				if (Class->HasAnyClassFlags(CLASS_Abstract))
				{
					UE_LOGFMT(LogFlecsWorld, Log,
						"Skipping registration of {ClassName} because it is abstract",
						Class->GetName());
					continue;
				}
				
				if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
				{
					UE_LOGFMT(LogFlecsWorld, Warning,
						"Skipping registration of {ClassName} because it is deprecated or has a newer version",
						Class->GetName());
					continue;
				}
				
				if UNLIKELY_IF(RegisteredObjectTypes.Contains(Class))
				{
					continue;
				}

				TSolidNotNull<UObject*> ObjectPtr = NewObject<UObject>(this, Class);
				
				const TScriptInterface<IFlecsObjectRegistrationInterface> ScriptInterface(ObjectPtr);
				ScriptInterface->RegisterObject(this);

				RegisteredObjects.Add(ObjectPtr);
				RegisteredObjectTypes.Add(Class, ObjectPtr);

				UE_LOGFMT(LogFlecsWorld, Log,
					"Registering object type {ClassName}", Class->GetName());
			}
		}
	}

	void RegisterUnrealTypes() const
	{
		RegisterComponentType<FGameplayTagContainer>();
		
		RegisterComponentType<FVector>();
		RegisterComponentType<FQuat>();
		RegisterComponentType<FRotator>();
		RegisterComponentType<FTransform>();

		RegisterComponentType<FBox>();
		RegisterComponentType<FBoxSphereBounds>();
		RegisterComponentType<FCapsuleShape>();
		RegisterComponentType<FRay>();

		RegisterComponentType<FVector4>();
		
		RegisterComponentType<FVector2D>();
		RegisterComponentType<FQuat2D>();
		RegisterComponentType<FTransform2D>();

		RegisterComponentType<FBox2D>();

		RegisterComponentType<FColor>();
		RegisterComponentType<FLinearColor>();

		RegisterComponentType<FPrimaryAssetType>();
		RegisterComponentType<FPrimaryAssetId>();

		RegisterComponentType<FTopLevelAssetPath>();
		RegisterComponentType<FSoftClassPath>();
		RegisterComponentType<FSoftObjectPath>();

		RegisterComponentType<FAssetData>();
		RegisterComponentType<FAssetBundleData>();
		RegisterComponentType<FFlecsPrimaryAssetComponent>();

		RegisterComponentType<FIntVector>();
		RegisterComponentType<FIntVector4>();
		RegisterComponentType<FIntPoint>();
		RegisterComponentType<FIntRect>();
	}

	// I hate this
	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void UnlockIter_Internal(flecs::iter& Iter, FunctionType&& Function)
	{
		DeferEndLambda([this, &Iter, Function = std::forward<FunctionType>(Function)]()
		{
			if (IsReadOnly())
			{
				EndReadOnly();
			}

			while (Iter.next())
			{
				const int32 SavedLockCount = internal_ecs_table_disable_lock(Iter.table());

				for (size_t Index : Iter)
				{
					std::invoke(Function, Iter, Index);
				}

				internal_ecs_table_enable_lock(Iter.table(), SavedLockCount);
			}

			if (IsReadOnly())
			{
				BeginReadOnly();
			}
		});
	}

	void InitializeSystems()
	{
		CreateObserver("AnyComponentObserver")
			.with_symbol_component().filter()
			.with<flecs::Component>().event(flecs::OnSet)
			.yield_existing()
			.run([this](flecs::iter& Iter)
			{
				UnlockIter_Internal(Iter, [this](flecs::iter& Iter, const size_t Index)
				{
					const FFlecsEntityHandle EntityHandle = Iter.entity(Index);
					
					const FString StructSymbol = EntityHandle.GetSymbol();
					
					if (FFlecsComponentPropertiesRegistry::Get().ContainsComponentProperties(StructSymbol))
					{
						FFlecsComponentHandle InUntypedComponent = EntityHandle.GetUntypedComponent_Unsafe();
						
						const FFlecsComponentProperties* Properties = FFlecsComponentPropertiesRegistry::Get()
							.GetComponentProperties(StructSymbol);
						solid_check(Properties);

						std::invoke(Properties->RegistrationFunction, Iter.world(), InUntypedComponent);
					}
					#if !NO_LOGGING
					else
					{
						UE_LOG(LogFlecsWorld, Log,
							TEXT("Component properties %s not found"), *StructSymbol);
					}
					#endif // UNLOG_ENABLED
				});
			});

		ObjectDestructionComponentQuery = World.query_builder<FFlecsUObjectComponent>("UObjectDestructionComponentQuery")
			.term_at(0).second(flecs::Wildcard) // FFlecsUObjectComponent
			.cached()
			.build();

		FCoreUObjectDelegates::GarbageCollectComplete.AddWeakLambda(this, [this]
		{
			ObjectDestructionComponentQuery
				.each([](flecs::entity InEntity, const FFlecsUObjectComponent& InUObjectComponent)
			{
				const FFlecsEntityHandle EntityHandle = InEntity;
					
				if (!InUObjectComponent.IsValid())
				{
					UE_LOG(LogFlecsWorld, Log,
						TEXT("Entity Garbage Collected: %s"), *EntityHandle.GetName());
					EntityHandle.Destroy();
				}
			});
		});

		ModuleComponentQuery = World.query_builder<FFlecsModuleComponent>("ModuleComponentQuery")
			.cached()
			.build();

		DependenciesComponentQuery = World.query_builder<FFlecsDependenciesComponent>("DependenciesComponentQuery")
			.cached()
			.build();

		CreateObserver<const FFlecsUObjectComponent>("AddModuleComponentObserver")
			.term_at(0).second<FFlecsModuleComponentTag>().filter() // FFlecsUObjectComponent
			.with<FFlecsModuleComponent>().event(flecs::OnAdd)
			.yield_existing()
			.each([this](flecs::entity InEntity, const FFlecsUObjectComponent& InUObjectComponent)
			{
				const TSolidNotNull<UObject*> ObjectPtr = InUObjectComponent.GetObjectChecked();

				UE_LOG(LogFlecsWorld, Log,
					TEXT("Module component %s added"), *ObjectPtr->GetName());
				
				// if (ObjectPtr->Implements<UFlecsModuleProgressInterface>())
				// {
				// 	ProgressModules.Add(ObjectPtr);
				// 	UN_LOGF(LogFlecsWorld, Log, "Progress module %s added",
				// 		*InUObjectComponent.GetObjectChecked()->GetName());
				// }
			});

		CreateObserver<FFlecsModuleComponent>(TEXT("ModuleInitEventObserver"))
			.with<FFlecsUObjectComponent, FFlecsModuleComponentTag>()
			.event<FFlecsModuleInitEvent>()
			.run([this](flecs::iter& Iter)
			{
				UnlockIter_Internal(Iter, [this](flecs::iter& Iter, size_t Index)
				{
					const FFlecsEntityHandle ModuleEntity = Iter.entity(Index);
					const FFlecsModuleComponent& InModuleComponent = Iter.field_at<FFlecsModuleComponent>(0, Index);
					FFlecsUObjectComponent& InUObjectComponent = Iter.field_at<FFlecsUObjectComponent>(1, Index);
					
					DependenciesComponentQuery.run([InModuleComponent, ModuleEntity, this, InUObjectComponent]
						(flecs::iter& DependenciesIter)
					{
						UnlockIter_Internal(DependenciesIter,
							[this, InModuleComponent, ModuleEntity, InUObjectComponent]
							(flecs::iter& DependenciesIter, size_t DependenciesIndex)
						{
							const FFlecsEntityHandle InEntity = DependenciesIter.entity(DependenciesIndex);
							
							FFlecsDependenciesComponent& DependenciesComponent
								= DependenciesIter.field_at<FFlecsDependenciesComponent>(0, DependenciesIndex);
							
							if (DependenciesComponent.Dependencies.contains(InModuleComponent.ModuleClass))
							{
								const std::function<void(
									TSolidNotNull<UObject*>,
									TSolidNotNull<UFlecsWorld*>, FFlecsEntityHandle)>& Function
									= DependenciesComponent.Dependencies.at(InModuleComponent.ModuleClass);

								InEntity.AddPair(flecs::DependsOn, ModuleEntity);

								std::invoke(Function, InUObjectComponent.GetObjectChecked(), this, ModuleEntity);
							}
						});
					});
				});
			});

		CreateObserver<FFlecsUObjectComponent>(TEXT("RemoveModuleComponentObserver"))
			.event(flecs::OnRemove)
			.with<FFlecsModuleComponent>().inout_none()
			.term_at(0).second(flecs::Wildcard).filter() // FFlecsUObjectComponent
			.each([this](flecs::entity InEntity, const FFlecsUObjectComponent& InUObjectComponent)
			{
				for (int32 Index = ImportedModules.Num() - 1; Index >= 0; --Index)
				{
					UObject* ModuleObject = ImportedModules[Index].GetObject();
					solid_check(IsValid(ModuleObject));
					
					if (ModuleObject == InUObjectComponent.GetObjectChecked())
					{
						ImportedModules.RemoveAt(Index);
						break;
					}
				}
				
				/*for (int32 Index = ProgressModules.Num(); Index > 0; --Index)
				{
					const UObject* Module = ProgressModules[Index - 1].GetObject();
					
					if UNLIKELY_IF(!IsValid(Module))
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
					}
					
					if (Module == InUObjectComponent.GetObjectChecked())
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
					}
				}*/
			});
	}

	/*void InitializeAssetRegistry()
	{
		const FAssetRegistryModule& AssetRegistryModule
			= FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AssetDataArray;
		AssetRegistry.GetAssetsByClass(
			FTopLevelAssetPath(UFlecsPrimaryDataAsset::StaticClass()),
			AssetDataArray, true);

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		for (const FAssetData& AssetData : AssetDataArray)
		{
			TSoftObjectPtr<UFlecsPrimaryDataAsset> AssetPtr(AssetData.ToSoftObjectPath());
			
			if (AssetPtr.IsValid())
			{
				RegisterFlecsAsset(AssetPtr.Get());
			}
			else
			{
				Streamable.RequestAsyncLoad(AssetPtr.ToSoftObjectPath(),
					FStreamableDelegate::CreateLambda([this, AssetPtr]()
					{
						if LIKELY_IF(UFlecsPrimaryDataAsset* LoadedAsset = AssetPtr.Get())
						{
							RegisterFlecsAsset(LoadedAsset);
						}
						else
						{
							UN_LOGF(LogFlecsWorld, Warning,
								"Failed to load asset %s", *AssetPtr.ToString());
						}
					})
				);
			}
		}
		
		AssetRegistry.OnAssetAdded().AddWeakLambda(this, [this](const FAssetData& InAssetData)
		{
			if (InAssetData.IsInstanceOf<UFlecsPrimaryDataAsset>())
			{
				TSoftObjectPtr<UFlecsPrimaryDataAsset> NewAsset(InAssetData.ToSoftObjectPath());
				FStreamableManager& LocalStreamable = UAssetManager::GetStreamableManager();
				LocalStreamable.RequestAsyncLoad(NewAsset.ToSoftObjectPath(),
					FStreamableDelegate::CreateLambda([this, NewAsset]()
					{
						if (UFlecsPrimaryDataAsset* LoadedAsset = NewAsset.Get())
						{
							RegisterFlecsAsset(LoadedAsset);
						}
					})
				);
			}
		});
		
		AssetRegistry.OnAssetRemoved().AddWeakLambda(this, [this](const FAssetData& InAssetData)
		{
			if (InAssetData.IsInstanceOf<UFlecsPrimaryDataAsset>())
			{
				UFlecsPrimaryDataAsset* RemovedAsset = CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset());
				UnregisterFlecsAsset(RemovedAsset);
			}
		});
	}*/

	/**
	 * @brief Asynchronously Register a module dependency,
	 * if the dependency is already imported, the function is called immediately
	 * if the dependency is not imported, the function is called when the dependency is imported (if it is)
	 * @param InModuleObject The module object
	 * @param InFunction The function to call when the dependency is imported
	 * @tparam TModule The module class
	 */
	template <Solid::TStaticClassConcept TModule, typename TFunction>
	FORCEINLINE_DEBUGGABLE void RegisterModuleDependency(const TSolidNotNull<UObject*> InModuleObject,
	                                                     TFunction&& InFunction)
	{
		RegisterModuleDependency(InModuleObject, TModule::StaticClass(),
			[InFunction = std::forward<TFunction>(InFunction)]
				(const TSolidNotNull<UObject*> InDependencyObject,
				TSolidNotNull<UFlecsWorld*> InWorld,
				FFlecsEntityHandle InDependencyEntity)
			{
				std::invoke(InFunction, CastChecked<TModule>(InDependencyObject), InWorld, InDependencyEntity);
			});
	}

	/**
	 * @brief Asynchronously Register a module dependency,
	 * if the dependency is already imported, the function is called immediately
	 * if the dependency is not imported, the function is called when the dependency is imported (if it is)
	 * @param InModuleObject The module object
	 * @param InDependencyClass The dependency class
	 * @param InFunction The function to call when the dependency is imported
	 */
	FORCEINLINE_DEBUGGABLE void RegisterModuleDependency(
		const TSolidNotNull<const UObject*> InModuleObject,
		const TSubclassOf<UFlecsModuleInterface>& InDependencyClass,
		const std::function<void(TSolidNotNull<UObject*>, TSolidNotNull<UFlecsWorld*>, FFlecsEntityHandle)>& InFunction)
	{
		solid_check(InModuleObject->Implements<UFlecsModuleInterface>());

		solid_checkf(IsModuleImported(InModuleObject->GetClass()),
			TEXT("Module %s is not imported"), *InModuleObject->GetClass()->GetName());

		FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModuleObject->GetClass());
		solid_check(ModuleEntity.IsValid());

		ModuleEntity.Add<FFlecsDependenciesComponent>();
		
		auto& [Dependencies] = ModuleEntity.GetMut<FFlecsDependenciesComponent>();
		
		Dependencies.emplace(InDependencyClass, InFunction);
		
		if (IsModuleImported(InDependencyClass))
		{
			const FFlecsEntityHandle DependencyEntity = GetModuleEntity(InDependencyClass);
			
			UObject* DependencyModuleObject = GetModule(InDependencyClass);
			solid_check(IsValid(DependencyModuleObject));

			solid_check(DependencyEntity.IsValid());

			ModuleEntity.AddPair(flecs::DependsOn, DependencyEntity);
			std::invoke(InFunction, DependencyModuleObject, this, DependencyEntity);
		}
	}

	/*FORCEINLINE_DEBUGGABLE void RegisterFlecsAsset(TSolidNotNull<UFlecsPrimaryDataAsset*> InAsset)
	{
		if (!InAsset->ShouldSpawn())
		{
			return;
		}

		const FFlecsEntityHandle AssetEntity = CreateEntity(InAsset->GetPrimaryAssetId().ToString());
		AssetEntity.Set<FFlecsPrimaryAssetComponent>(FSoftObjectPath(InAsset));
		FlecsPrimaryDataAssets.Add(InAsset);
		InAsset->OnEntityCreated(AssetEntity, this);
	}

	FORCEINLINE_DEBUGGABLE void UnregisterFlecsAsset(TSolidNotNull<UFlecsPrimaryDataAsset*> InAsset)
	{
		const FFlecsEntityHandle AssetEntity = LookupEntity(InAsset->GetPrimaryAssetId().ToString());
		
		if (!AssetEntity.IsValid())
		{
			UN_LOGF(LogFlecsWorld, Log,
				"Asset entity %s not found", *InAsset->GetPrimaryAssetId().ToString());
			return;
		}
		
		InAsset->OnEntityDestroyed(AssetEntity, this);
		FlecsPrimaryDataAssets.Remove(InAsset);
		AssetEntity.Destroy();
	}*/

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void Reset()
	{
		World.reset();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void ResetClock() const
	{
		World.reset_clock();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntity(const FString& Name = "") const
	{
		return World.entity(Unreal::Flecs::ToCString(Name));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithId(const FFlecsId InId) const
	{
		solid_checkf(!IsAlive(InId), TEXT("Entity with ID %s is already alive"), *InId.ToString());
		return MakeAlive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		return World.entity().is_a(InPrefab.GetEntity());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithRecord(const FFlecsEntityRecord& InRecord,
	                                                                 const FString& Name = "") const
	{
		const FFlecsEntityHandle Entity = CreateEntity(Name);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
	                                                                       const FFlecsId InId) const
	{
		const FFlecsEntityHandle Entity = CreateEntityWithId(InId);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World",
		meta = (AdvancedDisplay = "Separator, RootSeparator, bRecursive"))
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle LookupEntity(const FString& Name,
	                                                       const FString& Separator = "::",
	                                                       const FString& RootSeparator = "::",
	                                                       const bool bRecursive = true) const
	{
		return World.lookup(Unreal::Flecs::ToCString(Name),
		                    Unreal::Flecs::ToCString(Separator),
		                    Unreal::Flecs::ToCString(RootSeparator),
		                    bRecursive);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void DestroyEntityByName(const FString& Name) const
	{
		solid_checkf(!Name.IsEmpty(), TEXT("Name is empty"));

		const FFlecsEntityHandle Handle = LookupEntity(Name);
		
		if LIKELY_IF(Handle.IsValid())
		{
			Handle.Destroy();
		}
		else
		{
			UE_LOG(LogFlecsWorld, Warning, TEXT("Entity %s not found"), *Name);
		}
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEach(FunctionType&& Function) const
	{
		World.each(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEach(FunctionType&& Function) const
	{
		World.each<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEach(const FFlecsId& InTermId, const FunctionType& Function) const
	{
		World.each(InTermId, Function);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void AddSingleton() const
	{
		World.add<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void SetSingleton(const T& Value) const
	{
		World.set<T>(Value);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton() const
	{
		World.remove<T>();
	}
	
	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE const T& GetSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.get<T>();
	}

	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE T& GetMutSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE const T* GetSingletonPtr() const
	{
		return World.try_get<T>();
	}

	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE T* GetMutSingletonPtr() const
	{
		return World.try_get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ObtainSingletonEntity() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.singleton<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void ModifiedSingleton() const
	{
		World.modified<T>();
	}

	/**
	 * Merge world or stage.
	 * When automatic merging is disabled, an application can call this
	 * operation on either an individual stage, or on the world which will merge
	 * all stages. This operation may only be called when staging is not enabled
	 * (either after progress() or after readonly_end()).
	 *
	 * This operation may be called on an already merged stage or world.
	 *
	 * @see ecs_merge()
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void Merge() const
	{
		World.merge();
	}
	
	/**
	 * @return The Name of the World Entity
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FString GetWorldName() const
	{
		return FString(GetWorldEntity().GetName());
	}

	/**
	 * @brief Set the Name of the World Entity
	 * @param InName The new name
	 */
	FORCEINLINE_DEBUGGABLE void SetWorldName(const FString& InName) const
	{
		GetWorldEntity().SetName(InName);
	}
	
	/**
	 * @brief Import a regular C++ Module to the world,
	 * this MUST NOT be derived from IFlecsModuleInterface nor should it be a UObject
	 * @tparam T The module type
	 * @return The entity handle of the imported module
	 */
	template <typename T>
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ImportFlecsModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value,
			"T must not be derived from IFlecsModuleInterface, use ImportModule (Non-Template) instead");
		return World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void ImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule)
	{
		solid_checkf(InModule, TEXT("Module is nullptr"));

		const UObject* TemplateModuleObject = InModule.GetObject();

		TSolidNotNull<UObject*> NewModuleObject = DuplicateObject(TemplateModuleObject, this);
		ImportedModules.Add(NewModuleObject);
		
		ImportedModules.Last()->ImportModule(World);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsModuleImported(const TSubclassOf<UObject> InModule) const
	{
		solid_check(InModule);

		for (const TScriptInterface<IFlecsModuleInterface>& ModuleObject : ImportedModules)
		{
			if (ModuleObject.GetObject()->GetClass() == InModule)
			{
				return true;
			}
		}
		
		return false;
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE bool IsModuleImported() const
	{
		return IsModuleImported(T::StaticClass());
	}

	/**
	 * @brief Get the entity handle of the module with the given class
	 * @param InModule The module class
	 * @return The entity handle of the module, or an invalid handle if the module is not imported
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetModuleEntity(const TSubclassOf<UObject> InModule) const
	{
		solid_check(InModule);
		
		const FFlecsEntityHandle ModuleEntity = ModuleComponentQuery
			.find([&InModule](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
			{
				return InComponent.ModuleClass == InModule;
			});

		return ModuleEntity;
	}

	/**
	 * @brief Get the entity handle of the module with the given class
	 * @tparam T The module class
	 * @return The entity handle of the module, or an invalid handle if the module is not imported
	 */
	template <Solid::TStaticClassConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetModuleEntity() const
	{
		return GetModuleEntity(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE UObject* GetModule(const TSubclassOf<UObject> InModule) const
	{
		solid_check(IsValid(InModule));
		
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModule);
		return ModuleEntity.GetPairFirst<FFlecsUObjectComponent, FFlecsModuleComponentTag>().GetObjectChecked();
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE TSolidNotNull<T*> GetModule() const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity<T>();
		return ModuleEntity.GetPairFirst<FFlecsUObjectComponent, FFlecsModuleComponentTag>().GetObjectChecked<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool BeginDefer() const
	{
		return World.defer_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool EndDefer() const
	{
		return World.defer_end();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void ResumeDefer() const
	{
		World.defer_resume();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SuspendDefer() const
	{
		World.defer_suspend();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsDeferred() const
	{
		return World.is_deferred();
	}

	template <typename TFunction>
	FORCEINLINE_DEBUGGABLE void Defer(TFunction&& Function) const
	{
		World.defer<TFunction>(std::forward<TFunction>(Function));
	}

	/**
	 * @brief End or Suspend the defer state and execute the lambda function.
	 * @param Function The function to execute after the defer state is ended or suspended.
	 * @param bEndDefer If true, the defer state will be ended after the function is executed, otherwise it will be suspended and resumed after the function is executed.
	 */
	template <typename TFunction>
	FORCEINLINE_DEBUGGABLE void DeferEndLambda(TFunction&& Function, const bool bEndDefer = false) const
	{
		const bool bIsDeferred = IsDeferred();
		
		if (bIsDeferred)
		{
			if (bEndDefer)
			{
				EndDefer();
			}
			else
			{
				SuspendDefer();
			}
		}

		std::invoke(std::forward<TFunction>(Function));

		if (bIsDeferred)
		{
			if (bEndDefer)
			{
				BeginDefer();
			}
			else
			{
				ResumeDefer();
			}
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool BeginReadOnly() const
	{
		return World.readonly_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void EndReadOnly() const
	{
		World.readonly_end();
	}

	template <typename TFunction>
	FORCEINLINE_DEBUGGABLE void ReadOnly(TFunction&& Function) const
	{
		BeginReadOnly();
		std::invoke(std::forward<TFunction>(Function));
		EndReadOnly();
	}

	template <typename TFunction>
	FORCEINLINE_DEBUGGABLE void WithScoped(const FFlecsId InId, TFunction&& Function) const
	{
		World.with(InId, std::forward<TFunction>(Function));
	}

	FORCEINLINE_DEBUGGABLE void SetContext(void* InContext) const
	{
		World.set_ctx(InContext);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool ProgressGameLoop(const double DeltaTime = 0.0)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgress);

		solid_check(GameLoopInterface);
		return GameLoopInterface->Progress(DeltaTime, this);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool Progress(const double DeltaTime = 0.0)
	{
		return World.progress(DeltaTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetTimeScale(const double InTimeScale) const
	{
		World.set_time_scale(InTimeScale);
	}
	
	void DestroyWorld()
	{
		if UNLIKELY_IF(ShouldQuit())
		{
			return;
		}

		FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);

		ModuleComponentQuery.destruct();
		DependenciesComponentQuery.destruct();
		ObjectDestructionComponentQuery.destruct();
		
		const FAssetRegistryModule* AssetRegistryModule
			= FModuleManager::LoadModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry"));

		if (AssetRegistryModule)
		{
			IAssetRegistry& AssetRegistry = AssetRegistryModule->Get();
			AssetRegistry.OnAssetAdded().RemoveAll(this);
			AssetRegistry.OnAssetRemoved().RemoveAll(this);
		}
		
		World.release();
		World.world_ = nullptr;
		MarkAsGarbage();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetPipeline(const FFlecsEntityHandle& InPipeline) const
	{
		World.set_pipeline(InPipeline);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void SetPipeline() const
	{
		World.set_pipeline<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetPipeline() const
	{
		return World.get_pipeline();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE double GetDeltaTime() const
	{
		return World.delta_time();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetAlive(const FFlecsId InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsAlive(const FFlecsId InId) const
	{
		return World.is_alive(InId);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetEntity(const FFlecsId InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World.make_alive(InId);
	}

	template <typename ...TComponents>
	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::system_builder<TComponents...> CreateSystemWithBuilder(const FString& InName) const
	{
		return World.system<TComponents...>(Unreal::Flecs::ToCString(InName));
	}
	
	/**
	 * @brief Equivalent to World.entity(flecs::World)
	 * @return The World Entity
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetWorldEntity() const
	{
		return World.entity(flecs::World);
	}

	/**
	 * Get number of configured stages.
	 * Return number of stages set by set_stage_count().
	 *
	 * @return The number of stages used for threading.
	 *
	 * @see ecs_get_stage_count()
	 * @see flecs::world::set_stage_count()
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE int32 GetStageCount() const
	{
		return World.get_stage_count();
	}

	/**
	 * Test whether the current world object is readonly.
	 * This function allows the code to test whether the currently used world
	 * object is readonly or whether it allows for writing.
	 *
	 * @return True if the world or stage is readonly.
	 *
	 * @see ecs_stage_is_readonly()
	 * @see flecs::world::readonly_begin()
	 * @see flecs::world::readonly_end()
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsReadOnly() const
	{
		return World.is_readonly();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void PreallocateEntities(const int32 InEntityCount) const
	{
		if UNLIKELY_IF(ensureAlwaysMsgf(InEntityCount > 0,
			TEXT("Entity count must be greater than 0")))
		{
			return;
		}
		
		World.dim(InEntityCount);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetEntityRange(const FFlecsId InMin, const FFlecsId InMax) const
	{
		World.set_entity_range(InMin, InMax);
	}

	/**
	 * @brief Set entity range and invoke a function/lambda
	 * @tparam FunctionType The function type
	 * @param InMin Minimum entity id issued.
	 * @param InMax Maximum entity id issued.
	 * @param bEnforceEntityRange Whether to enforce the entity range. Enforce that operations cannot modify entities outside of range.
	 * @param Function The function to invoke
	 */
	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void SetEntityRange(const FFlecsId InMin, const FFlecsId InMax,
	                                           const bool bEnforceEntityRange,
	                                           FunctionType&& Function) const
	{
		const int32 OldMin = static_cast<int32>(ecs_get_world_info(World.c_ptr())->min_id);
		const int32 OldMax = static_cast<int32>(ecs_get_world_info(World.c_ptr())->max_id);
		
		World.set_entity_range(InMin, InMax);
		
		EnforceEntityRange(bEnforceEntityRange);
		std::invoke(std::forward<FunctionType>(Function));
		EnforceEntityRange(false);

		World.set_entity_range(OldMin, OldMax);
	}

	/**
	 * Enforce that operations cannot modify entities outside of range.
	 * This function ensures that only entities within the specified range can
	 * be modified. Use this function if specific parts of the code only are
	 * allowed to modify a certain set of entities, as could be the case for
	 * networked applications.
	 *
	 * @param bInEnforce True if range check should be enabled, false if not.
	 *
	 * @see ecs_enable_range_check()
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void EnforceEntityRange(const bool bInEnforce) const
	{
		World.enable_range_check(bInEnforce);
	}

	/**
	 * @brief Iterate over all Child Entities of the 0 Entity
	 * @tparam FunctionType The function type
	 * @param Function Function to invoke
	 */
	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEachChild(FunctionType&& Function) const
	{
		World.children(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEachChild(FunctionType&& Function) const
	{
		World.children<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEachChild(const FFlecsId& InRelationId, FunctionType&& Function) const
	{
		World.children(InRelationId, std::forward<FunctionType>(Function));
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetThreads(const int32 InThreadCount) const
	{
		World.set_threads(InThreadCount);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE int32 GetThreads() const
	{
		return World.get_threads();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool UsingTaskThreads() const
	{
		return World.using_task_threads();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetTaskThreads(const int32 InThreadCount) const
	{
		World.set_task_threads(InThreadCount);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool HasScriptStruct(const UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		if (TypeMapComponent->ScriptStructMap.contains(ScriptStruct))
		{
			const FFlecsId Component = TypeMapComponent->ScriptStructMap.at(ScriptStruct);
			return ecs_is_valid(World.c_ptr(), Component);
		}
		
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool HasScriptEnum(const UEnum* ScriptEnum) const
	{
		solid_check(ScriptEnum);
		
		if (TypeMapComponent->ScriptEnumMap.contains(ScriptEnum))
		{
			const FFlecsId Component = TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
			return ecs_is_valid(World.c_ptr(), Component);
		}

		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptStructEntity(const UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		const FFlecsId Component = TypeMapComponent->ScriptStructMap.at(ScriptStruct);
		solid_checkf(IsAlive(Component), TEXT("Entity is not alive"));
		
		return FFlecsEntityHandle(World, Component);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptEnumEntity(const UEnum* ScriptEnum) const
	{
		solid_check(ScriptEnum);
		
		const FFlecsId Component = TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
		solid_checkf(ecs_is_valid(World.c_ptr(), Component), TEXT("Entity is not alive"));
		return FFlecsEntityHandle(World, Component);
	}

	template <Solid::TScriptStructConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(TBaseStructure<T>::Get());
	}

	template <Solid::TScriptStructConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE bool HasScriptStruct() const
	{
		return HasScriptStruct(TBaseStructure<T>::Get());
	}

	template <Solid::TStaticEnumConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptEnumEntity() const
	{
		return GetScriptEnumEntity(StaticEnum<T>());
	}

	template <Solid::TStaticEnumConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE bool HasScriptEnum() const
	{
		return HasScriptEnum(StaticEnum<T>());
	}

	void RegisterMemberProperties(const TSolidNotNull<const UStruct*> InStruct,
	                              const FFlecsComponentHandle& InComponent) const
	{
		solid_checkf(InComponent.IsValid(), TEXT("Entity is nullptr"));

		for (TFieldIterator<FProperty> PropertyIt(InStruct, EFieldIterationFlags::IncludeSuper);
			PropertyIt; ++PropertyIt)
		{
			FProperty* Property = *PropertyIt;
			solid_check(Property);

			const char* PropertyNameCStr = Unreal::Flecs::ToCString(Property->GetName());
				
			if (Property->IsA<FBoolProperty>())
			{
				InComponent.AddMember<bool>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FByteProperty>())
			{
				InComponent.AddMember<uint8>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FInt16Property>())
			{
				InComponent.AddMember<int16>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt16Property>())
			{
				InComponent.AddMember<uint16>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FIntProperty>())
			{
				InComponent.AddMember<int32>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt32Property>())
			{
				InComponent.AddMember<uint32>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FInt64Property>())
			{
				InComponent.AddMember<int64>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt64Property>())
			{
				InComponent.AddMember<uint64>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				InComponent.AddMember<float>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				InComponent.AddMember<double>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStrProperty>())
			{
				InComponent.AddMember<FString>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FNameProperty>())
			{
				InComponent.AddMember<FName>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FTextProperty>())
			{
				InComponent.AddMember<FText>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FObjectProperty>())
			{
				InComponent.AddMember<FObjectPtr>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FWeakObjectProperty>())
			{
				InComponent.AddMember<FWeakObjectPtr>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FSoftObjectProperty>())
			{
				InComponent.AddMember<FSoftObjectPtr>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FClassProperty>())
			{
				InComponent.AddMember<TSubclassOf<UObject>>(PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStructProperty>())
			{
				FFlecsEntityHandle StructComponent;
				if (!HasScriptStruct(CastFieldChecked<FStructProperty>(Property)->Struct))
				{
					UE_LOG(LogFlecsWorld, Error,
						TEXT("Property Type Script struct %s is not registered for entity %s"),
						*CastFieldChecked<FStructProperty>(Property)->Struct->GetName(), *InComponent.GetName());
					continue;
				}
				else
				{
					StructComponent = GetScriptStructEntity(CastFieldChecked<FStructProperty>(Property)->Struct);
				}
			 		
				InComponent.AddMember(StructComponent, PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else UNLIKELY_ATTRIBUTE
			{
				UE_LOG(LogFlecsWorld, Log,
					TEXT("Property Type: %s is not supported for member serialization"),
					*Property->GetName());
			}
		}
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptStruct(const UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);

		const FFlecsEntityHandle OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptStructMap.contains(ScriptStruct),
			TEXT("Script struct %s is already registered"), *ScriptStruct->GetStructCPPName());
		
		FFlecsComponentHandle ScriptStructComponent;

		const FString StructName = ScriptStruct->GetStructCPPName();
		const char* StructNameCStr = Unreal::Flecs::ToCString(StructName);
		std::string StructNameStd = std::string(StructNameCStr);

		DeferEndLambda([this, ScriptStruct, &ScriptStructComponent, StructNameCStr, &StructNameStd]()
		{
			ScriptStructComponent = World.component(StructNameCStr);
			solid_check(ScriptStructComponent.IsValid());
			
			ScriptStructComponent.GetEntity().set_symbol(StructNameCStr);
			
			const TFieldIterator<FProperty> PropertyIt(ScriptStruct);
			const bool bIsTag = ScriptStruct->GetStructureSize() <= 1 && !PropertyIt;

			ScriptStructComponent.Set<flecs::Component>({
				.size = bIsTag ? 0 : ScriptStruct->GetStructureSize(),
				.alignment = bIsTag ? 0 : ScriptStruct->GetMinAlignment()
			});

			if (!bIsTag)
			{
				if (ScriptStruct->GetCppStructOps()->HasNoopConstructor())
				{
					UE_LOG(LogFlecsComponent, Warning,
						TEXT("Script struct %s has a noop constructor, this will not be used in flecs"),
						*ScriptStruct->GetName());
				}

				ScriptStructComponent.SetHooksLambda([ScriptStruct](flecs::type_hooks_t& Hooks)
				{
					Hooks.ctx = const_cast<UScriptStruct*>(ScriptStruct);
					
					if (!ScriptStruct->GetCppStructOps()->HasZeroConstructor())
					{
						Hooks.ctor = [](void* Ptr, int32_t Count, const ecs_type_info_t* TypeInfo)
						{
							solid_check(TypeInfo != nullptr);
							solid_check(Ptr != nullptr);
							solid_check(TypeInfo->hooks.ctx != nullptr);

							const UScriptStruct* ContextScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
							solid_check(IsValid(ContextScriptStruct));

							ContextScriptStruct->InitializeStruct(Ptr, Count);
						};
					}
					else
					{
						Hooks.ctor = nullptr;
					}
					
					if (ScriptStruct->GetCppStructOps()->HasDestructor())
					{
						Hooks.dtor = [](void* Ptr, int32_t Count, const ecs_type_info_t* TypeInfo)
						{
							solid_check(TypeInfo != nullptr);
							solid_check(Ptr != nullptr);
							solid_check(TypeInfo->hooks.ctx != nullptr);

							const UScriptStruct* ContextScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
							solid_check(IsValid(ContextScriptStruct));

							ContextScriptStruct->DestroyStruct(Ptr, Count);
						};
					}
					else
					{
						Hooks.dtor = nullptr;
					}
						
					Hooks.copy = [](void* Dst, const void* Src, int32_t Count, const ecs_type_info_t* TypeInfo)
						{
							solid_check(TypeInfo != nullptr);
							solid_check(Src != nullptr);
							solid_check(Dst != nullptr);
							solid_check(TypeInfo->hooks.ctx != nullptr);

							const UScriptStruct* ContextScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
							solid_check(IsValid(ContextScriptStruct));

							ContextScriptStruct->CopyScriptStruct(Dst, Src, Count);
						};
						
						Hooks.move = [](void* Dst, void* Src, int32_t Count, const ecs_type_info_t* TypeInfo)
						{
							solid_check(TypeInfo != nullptr);
							solid_check(Src != nullptr);
							solid_check(Dst != nullptr);
							solid_check(TypeInfo->hooks.ctx != nullptr);

							const UScriptStruct* ContextScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
							solid_check(IsValid(ContextScriptStruct));

							ContextScriptStruct->CopyScriptStruct(Dst, Src, Count);
						};

						if (ScriptStruct->GetCppStructOps()->HasIdentical())
						{
							Hooks.equals = [](const void* Ptr1, const void* Ptr2, const ecs_type_info_t* TypeInfo)
								-> bool
							{
								solid_check(TypeInfo != nullptr);
								solid_check(Ptr1 != nullptr);
								solid_check(Ptr2 != nullptr);
								solid_check(TypeInfo->hooks.ctx != nullptr);

								const UScriptStruct* ContextScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
								solid_check(IsValid(ContextScriptStruct));

								return ContextScriptStruct->CompareScriptStruct(Ptr1, Ptr2, PPF_DeepComparison);
							};
						}
						else
						{
							Hooks.equals = nullptr;
						}

					if (ScriptStruct->GetCppStructOps()->IsPlainOldData())
					{
						Hooks.copy = nullptr;
						Hooks.move = nullptr;
					}
				});
			}

			if (!flecs::_::g_type_to_impl_data.contains(StructNameStd))
			{
				flecs::_::type_impl_data NewData;  // NOLINT(cppcoreguidelines-pro-type-member-init)
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = bIsTag ? 0 : ScriptStruct->GetStructureSize();
				NewData.s_alignment = bIsTag ? 0 : ScriptStruct->GetMinAlignment();
				NewData.s_allow_tag = bIsTag;
				NewData.s_enum_registered = false;
				
				flecs::_::g_type_to_impl_data.emplace(StructNameStd, NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(StructNameStd));
			flecs::_::type_impl_data& Data = flecs::_::g_type_to_impl_data.at(StructNameStd);

			flecs_component_ids_set(World, Data.s_index, ScriptStructComponent);

			TypeMapComponent->ScriptStructMap.emplace(ScriptStruct, ScriptStructComponent);

			RegisterMemberProperties(ScriptStruct, ScriptStructComponent);
		});

		ScriptStructComponent.Set<FFlecsScriptStructComponent>({ ScriptStruct });

		SetScope(OldScope);
		return ScriptStructComponent;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptEnum(const UEnum* ScriptEnum) const
	{
		solid_check(IsValid(ScriptEnum));

		solid_checkf(!TypeMapComponent->ScriptEnumMap.contains(ScriptEnum),
			TEXT("Script enum %s is already registered"), *ScriptEnum->GetName());

		// if (ScriptEnum->HasAnyEnumFlags(EEnumFlags::Flags))
		// {
		// 	return RegisterComponentBitmaskType(ScriptEnum);
		// }
		// else
		// {
		// 	return RegisterComponentEnumType(ScriptEnum);
		// }
		
		return RegisterComponentEnumType(ScriptEnum);
	}
	
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterComponentEnumType(TSolidNotNull<const UEnum*> ScriptEnum) const
	{
		const FFlecsEntityHandle OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptEnumMap.contains(FFlecsScriptEnumComponent(ScriptEnum)),
			TEXT("Script enum %s is already registered"), *ScriptEnum->GetName());

		FFlecsComponentHandle ScriptEnumComponent;

		const FString EnumName = ScriptEnum->GetName();
		const char* EnumNameCStr = Unreal::Flecs::ToCString(EnumName);

		DeferEndLambda([this, ScriptEnum, &ScriptEnumComponent, EnumNameCStr]()
		{
			ScriptEnumComponent = World.component(Unreal::Flecs::ToCString(ScriptEnum->GetName()));
			solid_check(ScriptEnumComponent.IsValid());
			
			ScriptEnumComponent.GetEntity().set_symbol(Unreal::Flecs::ToCString(ScriptEnum->GetName()));

			ScriptEnumComponent.Set<flecs::Component>({
				.size = sizeof(uint8),
				.alignment = alignof(uint8)
			});
			
			ScriptEnumComponent.Set<flecs::Enum>(flecs::Enum{ .underlying_type = flecs::U8 });

			const int32 EnumCount = ScriptEnum->NumEnums();
			
			for (int32 EnumIndex = 0; EnumIndex < EnumCount; ++EnumIndex)
			{
				const int64 EnumValue = ScriptEnum->GetValueByIndex(EnumIndex);
				solid_check(EnumValue >= 0);
				
				const FString EnumValueName = ScriptEnum->GetNameStringByIndex(EnumIndex);

				if (ScriptEnum->GetMaxEnumValue() == EnumValue)
				{
					continue;
				}
				
				ScriptEnumComponent.AddConstant<uint8>(StringCast<char>(*EnumValueName).Get(),
					static_cast<uint8>(EnumValue));
			}

			if (!flecs::_::g_type_to_impl_data.contains(std::string(EnumNameCStr)))
			{
				flecs::_::type_impl_data NewData;
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = sizeof(uint8);
				NewData.s_alignment = alignof(uint8);
				NewData.s_allow_tag = false;
				NewData.s_enum_registered = false;
				
				flecs::_::g_type_to_impl_data.emplace(std::string(EnumNameCStr), NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(std::string(EnumNameCStr)));
			
			auto& [s_index, s_size, s_alignment, s_allow_tag, s_enum_registered]
				= flecs::_::g_type_to_impl_data.at(std::string(EnumNameCStr));
			
			flecs_component_ids_set(World, s_index, ScriptEnumComponent);
			TypeMapComponent->ScriptEnumMap.emplace(ScriptEnum, ScriptEnumComponent);
		});

		ScriptEnumComponent.Set<FFlecsScriptEnumComponent>(FFlecsScriptEnumComponent(ScriptEnum));

		SetScope(OldScope);
		return ScriptEnumComponent;
	}
	
	/*FFlecsEntityHandle RegisterComponentBitmaskType(const UEnum* ScriptEnum) const
	{
		solid_check(IsValid(ScriptEnum));

		const FFlecsEntityHandle OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptEnumMap.contains(ScriptEnum),
			TEXT("Script enum %s is already registered"), *ScriptEnum->GetName());

		flecs::untyped_component ScriptEnumComponent;

		DeferEndScoped([this, ScriptEnum, &ScriptEnumComponent]()
		{
			ScriptEnumComponent = World.component(StringCast<char>(*ScriptEnum->GetName()).Get());
			solid_check(ScriptEnumComponent.is_valid());
			ScriptEnumComponent.set_symbol(StringCast<char>(*ScriptEnum->GetName()).Get());
			ScriptEnumComponent.set<flecs::Component>(
				{ .size = sizeof(uint8), .alignment = alignof(uint8) });
			ScriptEnumComponent.add<flecs::Bitmask>();

			const int32 EnumCount = ScriptEnum->NumEnums();
			
			for (int32 EnumIndex = 0; EnumIndex < EnumCount; ++EnumIndex)
			{
				const FString EnumName = ScriptEnum->GetNameStringByIndex(EnumIndex);
				const int32 EnumValue = ScriptEnum->GetValueByIndex(EnumIndex);
				
				ScriptEnumComponent.bit<uint8>(StringCast<char>(*EnumName).Get(), EnumValue, flecs::U8);
			}

			if (!flecs::_::g_type_to_impl_data.contains(
				std::string(StringCast<char>(*ScriptEnum->GetName()).Get())))
			{
				flecs::_::type_impl_data NewData;
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = sizeof(uint8);
				NewData.s_alignment = alignof(uint8);
				NewData.s_allow_tag = true;
				
				flecs::_::g_type_to_impl_data.emplace(
					std::string(StringCast<char>(*ScriptEnum->GetName()).Get()), NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(
				std::string(StringCast<char>(*ScriptEnum->GetName()).Get())));
			flecs::_::type_impl_data& Data = flecs::_::g_type_to_impl_data.at(
				std::string(StringCast<char>(*ScriptEnum->GetName()).Get()));

			flecs_component_ids_set(World, Data.s_index, ScriptEnumComponent);
			TypeMapComponent->ScriptEnumMap.emplace(ScriptEnum, ScriptEnumComponent);
		});

		ScriptEnumComponent.set<FFlecsScriptEnumComponent>({ ScriptEnum });
		SetScope(OldScope);
		return ScriptEnumComponent;
	}*/

	template <typename T>
	FORCEINLINE_DEBUGGABLE FFlecsComponentHandle RegisterComponentType() const
	{
		FFlecsComponentHandle Component = World.component<T>();
		solid_check(Component.IsValid());
		
		if constexpr (Solid::IsScriptStruct<T>())
		{
			RegisterMemberProperties(TBaseStructure<T>::Get(), Component);
		}

		return Component;
	}
	
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterComponentType(
		const TSolidNotNull<const UScriptStruct*> ScriptStruct) const
	{
		if (HasScriptStruct(ScriptStruct))
		{
			return GetScriptStructEntity(ScriptStruct);
		}

		return RegisterScriptStruct(ScriptStruct);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE FFlecsComponentHandle ObtainComponentTypeStruct() const
	{
		solid_checkf(World.is_valid(flecs::_::type<T>::id(World)),
			TEXT("Component %hs is not registered"), nameof(T).data());
		return World.component<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsComponentHandle ObtainComponentTypeStruct(const UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		solid_checkf(HasScriptStruct(ScriptStruct),
			TEXT("Script struct %s is not registered"), *ScriptStruct->GetStructCPPName());
		
		return GetScriptStructEntity(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsComponentHandle ObtainComponentTypeEnum(const UEnum* ScriptEnum) const
	{
		solid_check(ScriptEnum);
		
		solid_checkf(HasScriptEnum(ScriptEnum),
			TEXT("Script enum %s is not registered"), *ScriptEnum->GetName());
		
		return GetScriptEnumEntity(ScriptEnum);
	}

	template <typename ...TComponents>
	FORCEINLINE_DEBUGGABLE flecs::observer_builder<TComponents...> CreateObserver(const FString& Name = "") const
	{
		return World.observer<TComponents...>(StringCast<char>(*Name).Get());
	}

	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::observer_builder<> CreateObserver(const FString& Name = "") const
	{
		return World.observer<>(StringCast<char>(*Name).Get());
	}

	template <typename ...TComponents, typename ...TArgs>
	FORCEINLINE_DEBUGGABLE flecs::observer_builder<TComponents...> CreateObserver(
		const FFlecsEntityHandle& InEntity, TArgs&&... Args) const
	{
		return World.observer<TComponents...>(InEntity.GetEntity(), std::forward<TArgs>(Args)...);
	}

	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::event_builder Event(const FFlecsEntityHandle& InEntity) const
	{
		return World.event(InEntity.GetEntity());
	}

	template <typename TEvent>
	FORCEINLINE_DEBUGGABLE flecs::event_builder_typed<TEvent> Event() const
	{
		return World.event<TEvent>();
	}

	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::pipeline_builder<> CreatePipeline() const
	{
		return World.pipeline();
	}

	template <typename ...TComponents>
	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::pipeline_builder<TComponents...> CreatePipeline() const
	{
		return World.pipeline<TComponents...>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void RunPipeline(const FFlecsId InPipeline, const double DeltaTime = 0.0) const
	{
		solid_checkf(InPipeline.IsValid(), TEXT("Pipeline is not valid"));
		solid_checkf(IsAlive(InPipeline), TEXT("Pipeline entity is not alive"));
		World.run_pipeline(InPipeline, DeltaTime);
	}

	FORCEINLINE_DEBUGGABLE void RandomizeTimers() const
	{
		World.randomize_timers();
	}

	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::timer CreateTimer(const FString& Name) const
	{
		return World.timer(StringCast<char>(*Name).Get());
	}

	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE flecs::timer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool HasEntityWithName(const FString& Name) const
	{
		return LookupEntity(Name).IsValid();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const
	{
		solid_checkf(Tag.IsValid(), TEXT("Tag is not valid"));
		
		const FString TagName = Tag.GetTagName().ToString();
		return LookupEntity(TagName, ".", ".");
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void EnableType() const
	{
		World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void EnableType(UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		ObtainComponentTypeStruct(ScriptStruct).Enable();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void DisableType() const
	{
		World.component<T>().disable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void DisableType(UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		ObtainComponentTypeStruct(ScriptStruct).Disable();
	}

	template <typename T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE bool IsTypeEnabled() const
	{
		return World.component<T>().enabled();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool IsTypeEnabled(UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		return ObtainComponentTypeStruct(ScriptStruct).IsEnabled();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void ToggleType() const
	{
		World.component<T>().enabled() ? World.component<T>().disable() : World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void ToggleType(UScriptStruct* ScriptStruct) const
	{
		solid_check(ScriptStruct);
		
		ObtainComponentTypeStruct(ScriptStruct).Toggle();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord,
	                                                                 const FString& Name = "") const
	{
		const FFlecsEntityHandle Prefab = World.prefab(StringCast<char>(*Name).Get());
		solid_checkf(Prefab.IsPrefab(), TEXT("Entity is not a prefab"));
		
		InRecord.ApplyRecordToEntity(Prefab);
		Prefab.Set<FFlecsEntityRecord>(InRecord);
		
		#if WITH_EDITOR

		if (!Name.IsEmpty())
		{
			Prefab.SetDocName(Name);
		}

		#endif // WITH_EDITOR
		
		return Prefab;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab(const FString& Name) const
	{
		return World.prefab(StringCast<char>(*Name).Get());
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void DestroyPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		solid_checkf(InPrefab.IsValid(), TEXT("Prefab entity is not valid"));
		InPrefab.Destroy();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool ShouldQuit() const
	{
		return World.should_quit();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsId ClearScope() const
	{
		return World.set_scope(0);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsId SetScope(const FFlecsId InScope) const
	{
		return World.set_scope(InScope);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Scope() const
	{
		World.scope<T>();
	}

	template <typename T, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Scope(const FunctionType& Function) const
	{
		World.scope<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Scope(FFlecsId InId, const FunctionType& Function) const
	{
		World.scope(InId, Function);
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void EndScope(FunctionType&& Function) const
	{
		const flecs::entity OldScope = World.set_scope(0);

		std::invoke(std::forward<FunctionType>(Function));

		World.set_scope(OldScope);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScope() const
	{
		return World.get_scope();
	}

	FORCEINLINE_DEBUGGABLE virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	FORCEINLINE_DEBUGGABLE virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	}

	FORCEINLINE_DEBUGGABLE virtual bool IsNameStableForNetworking() const override
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	UFlecsWorldSubsystem* GetContext() const;

	NO_DISCARD FORCEINLINE_DEBUGGABLE FFlecsTypeMapComponent* GetTypeMapComponent() const
	{
		return static_cast<FFlecsTypeMapComponent*>(World.get_binding_ctx());
	}
	
	flecs::world World;

	UPROPERTY()
	TScriptInterface<IFlecsGameLoopInterface> GameLoopInterface;

	UPROPERTY()
	TArray<TScriptInterface<IFlecsModuleInterface>> ImportedModules;
	
	TMap<const UClass*, TScriptInterface<IFlecsObjectRegistrationInterface>> RegisteredObjectTypes;

	UPROPERTY()
	TArray<TScriptInterface<IFlecsObjectRegistrationInterface>> RegisteredObjects;

	//UPROPERTY()
	//TArray<TScriptInterface<IFlecsModuleProgressInterface>> ProgressModules;

	//UPROPERTY()
	//TArray<TObjectPtr<UFlecsPrimaryDataAsset>> FlecsPrimaryDataAssets;

	flecs::query<FFlecsModuleComponent> ModuleComponentQuery;
	flecs::query<FFlecsUObjectComponent> ObjectDestructionComponentQuery;
	flecs::query<FFlecsDependenciesComponent> DependenciesComponentQuery;

	FFlecsTypeMapComponent* TypeMapComponent;
	
}; // class UFlecsWorld
