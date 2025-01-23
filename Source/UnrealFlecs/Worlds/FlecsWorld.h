// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Components/ObjectTypes/FFlecsModuleComponentTag.h"
#include "Entities/FlecsEntityRecord.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsDependenciesComponent.h"
#include "Modules/FlecsModuleInitEvent.h"
#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleProgressInterface.h"
#include "Prefabs/FlecsPrefabAsset.h"
#include "StructUtils/StructView.h"
#include "FlecsWorld.generated.h"

class UFlecsWorldSubsystem;

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"), STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);
DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress::ProgressModule"),
	STAT_FlecsWorldProgressModule, STATGROUP_FlecsWorld);

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsBeginPlay
{
	GENERATED_BODY()
}; // struct FFlecsBeginPlayEvent

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	UFlecsWorld()
	{
		char* argv[] = { const_cast<ANSICHAR*>(StringCast<ANSICHAR>(*GetName()).Get()) };  // NOLINT(clang-diagnostic-dangling)
		World = flecs::world(1, argv);
		TypeMapComponent = GetSingletonPtr<FFlecsTypeMapComponent>();
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

		if (World)
		{
			World.release();
			World.world_ = nullptr;
		}
	}

	void WorldStart()
	{
		UN_LOG(LogFlecsWorld, Log, "Flecs World begin play");

		InitializeAssetRegistry();
		InitializeSystems();
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void WorldBeginPlay()
	{
		GetWorldEntity().Add<FFlecsBeginPlay>();
	}

	void InitializeDefaultComponents() const
	{
		World.component<FString>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FString* Data)
			{
				return Serializer->value(flecs::String, Data->GetCharArray().GetData());
			})
			.assign_string([](FString* Data, const char* String)
			{
				*Data = String;
			});

		World.component<FName>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FName* Data)
			{
				return Serializer->value(flecs::String, Data->ToString().GetCharArray().GetData());
			})
			.assign_string([](FName* Data, const char* String)
			{
				*Data = FName(String);
			});

		World.component<FText>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FText* Data)
			{
				return Serializer->value(flecs::String, Data->ToString().GetCharArray().GetData());
			})
			.assign_string([](FText* Data, const char* String)
			{
				*Data = FText::FromString(String);
			});

		World.component<FGameplayTag>()
			.opaque(flecs::Entity)
			.serialize([](const flecs::serializer* Serializer, const FGameplayTag* Data)
			{
				const FFlecsId TagEntity = ecs_lookup(Serializer->world,
					StringCast<char>(*Data->ToString()).Get());
				return Serializer->value(flecs::Entity, &TagEntity);
			});

		World.component<FFlecsEntityHandle>()
            .opaque(flecs::Entity)
            .serialize([](const flecs::serializer* Serializer, const FFlecsEntityHandle* Data)
            {
                const FFlecsId Entity = Data->GetEntity();
                return Serializer->value(flecs::Entity, &Entity);
            })
            .assign_entity([](FFlecsEntityHandle* Handle, ecs_world_t* World, ecs_entity_t Entity)
            {
                *Handle = FFlecsEntityHandle(World, Entity);
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
	}

	void InitializeSystems()
	{
		CreateObserver<const FFlecsScriptStructComponent>("ScriptStructComponentObserver")
			.with_symbol_component().filter()
			.event(flecs::OnSet)
			.yield_existing()
			.each([this](flecs::iter& Iter, size_t IterIndex,
			             const FFlecsScriptStructComponent& InScriptStructComponent)
			{
				FFlecsEntityHandle EntityHandle = Iter.entity(IterIndex);

				if (InScriptStructComponent.ScriptStruct == FFlecsScriptStructComponent::StaticStruct())
				{
					return;
				}

				const FString StructSymbol = EntityHandle.GetSymbol();
				
				if (FFlecsComponentPropertiesRegistry::Get().ContainsComponentProperties(
					StringCast<char>(*StructSymbol).Get()))
				{
					const FFlecsComponentProperties* Properties = FFlecsComponentPropertiesRegistry::Get().
						GetComponentProperties(StringCast<char>(*StructSymbol).Get());
					
					for (const flecs::entity_t Entity : Properties->Entities)
					{
						EntityHandle.Add(Entity);
					}

					for (const FSharedStruct& InstancedStruct : Properties->ComponentPropertyStructs)
					{
						solid_check(InstancedStruct.IsValid());
						EntityHandle.Set(FInstancedStruct(FConstStructView(InstancedStruct)));
					}

					UN_LOGF(LogFlecsWorld, Log,
						"Component properties %s found with %d entities and %d component properties",
						*StructSymbol, Properties->Entities.size(),
						Properties->ComponentPropertyStructs.Num());
				}
				#if UNLOG_ENABLED
				else
				{
					UN_LOGF(LogFlecsWorld, Log, "Component properties %s not found", *StructSymbol);
				}
				#endif // UNLOG_ENABLED
				
				RegisterMemberProperties(InScriptStructComponent.ScriptStruct.Get(), EntityHandle);
			});

		ObjectDestructionComponentQuery = World.query_builder<FFlecsUObjectComponent>("UObjectDestructionComponentQuery")
			.cached()
			.build();

		FCoreUObjectDelegates::GarbageCollectComplete.AddWeakLambda(this, [this]
		{
			ObjectDestructionComponentQuery.each([](
				flecs::entity InEntity, FFlecsUObjectComponent& InUObjectComponent)
			{
				if (InUObjectComponent.IsStale())
				{
					UN_LOGF(LogFlecsWorld, Log, "Entity Garbage Collected: %s",
						StringCast<TCHAR>(InEntity.name().c_str()).Get());
					InEntity.destruct();
				}
			});
		});

		ModuleComponentQuery = World.query_builder<FFlecsModuleComponent>("ModuleComponentQuery")
			.cached()
			.build();

		DependenciesComponentQuery = World.query_builder<FFlecsDependenciesComponent>("DependenciesComponentQuery")
			.cached()
			.build();

		CreateObserver<const FFlecsUObjectComponent&,const FFlecsModuleComponent&>(TEXT("AddModuleComponentObserver"))
			.term_at(0).filter()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([this](flecs::entity InEntity,
			             const FFlecsUObjectComponent& InUObjectComponent,
			             const FFlecsModuleComponent& InModuleComponent)
			{
				UN_LOGF(LogFlecsWorld, Log, "Module component %s added",
                    *InUObjectComponent.GetObjectChecked()->GetName());
				
				if (InUObjectComponent.GetObjectChecked()->Implements<UFlecsModuleProgressInterface>())
				{
					ProgressModules.Add(InUObjectComponent.GetObjectChecked());
					UN_LOGF(LogFlecsWorld, Log, "Progress module %s added",
						*InUObjectComponent.GetObjectChecked()->GetName());
				}
			});

		CreateObserver<FFlecsModuleComponent, FFlecsUObjectComponent>(TEXT("ModuleInitEventObserver"))
			.event<FFlecsModuleInitEvent>()
			.term_at(1).filter().read()
			.each([this](flecs::iter& Iter, const size_t IterIndex,
			             const FFlecsModuleComponent& InModuleComponent,
			             const FFlecsUObjectComponent& InUObjectComponent)
			{
				const FFlecsEntityHandle ModuleEntity = Iter.entity(IterIndex);
				
				DependenciesComponentQuery.each([&]
					(flecs::entity InEntity, const FFlecsDependenciesComponent& InDependenciesComponent)
				{
					if (InDependenciesComponent.Dependencies.contains(InModuleComponent.ModuleClass))
					{
						const std::function<void(UObject*, UFlecsWorld*, FFlecsEntityHandle)>& Function
							= InDependenciesComponent.Dependencies.at(InModuleComponent.ModuleClass);

						InEntity.add(flecs::DependsOn, ModuleEntity);
						
						Function(InUObjectComponent.GetObjectChecked(), this, ModuleEntity);
					}
				});
			});

		CreateObserver(TEXT("RemoveModuleComponentObserver"))
			.event(flecs::OnRemove)
			.with<FFlecsModuleComponent>().inout_none()
			.with<FFlecsUObjectComponent>().filter().read()
			.each([this](flecs::iter& Iter, const size_t IterIndex)
			{
				for (int32 Index = ProgressModules.Num(); Index > 0; --Index)
				{
					const UObject* Module = ProgressModules[Index - 1].GetObject();
					
					if UNLIKELY_IF(!IsValid(Module))
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
					}
					
					if (Module == Iter.entity(IterIndex).get<FFlecsUObjectComponent>()->GetObjectChecked())
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
					}
				}
			});

		/*CreateObserver<FFlecsScriptStructComponent>(TEXT("ComponentCollectionObserver"))
			.term_at(0).filter()
			.with<FFlecsComponentCollection>().event(flecs::OnAdd)
			.observer_flags(EcsQueryMatchPrefab)
			.yield_existing()
			.each([this](flecs::iter& Iter, size_t IterIndex,
				const FFlecsScriptStructComponent& InScriptStructComponent)
			{
				FFlecsEntityHandle EntityHandle = Iter.entity(IterIndex);
				
				const UScriptStruct* ScriptStruct = InScriptStructComponent.ScriptStruct.Get();
				solid_check(IsValid(ScriptStruct));

				for (TFieldIterator<FStructProperty> PropertyIt(ScriptStruct, EFieldIteratorFlags::ExcludeSuper);
					PropertyIt; ++PropertyIt)
				{
					const FStructProperty* Property = *PropertyIt;
					EntityHandle.Add(ObtainComponentTypeStruct(Property->Struct));
				}
			});*/
	}

	void InitializeAssetRegistry()
	{
		const FAssetRegistryModule& AssetRegistryModule
			= FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AssetData;
		AssetRegistry.GetAssetsByClass(
			FTopLevelAssetPath(UFlecsPrimaryDataAsset::StaticClass()), AssetData, true);
		
		AssetRegistry.OnAssetAdded().AddWeakLambda(this, [this](const FAssetData& InAssetData)
		{
			if (InAssetData.IsInstanceOf<UFlecsPrimaryDataAsset>())
			{
				RegisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset()));
			}
		});

		for (const FAssetData& Asset : AssetData)
		{
			RegisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(Asset.GetAsset()));
		}
		
		AssetRegistry.OnAssetRemoved().AddWeakLambda(this, [this](const FAssetData& InAssetData)
		{
			UnregisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset()));
		});
	}

	/**
	 * @brief Asynchronously Register a module dependency,
	 * if the dependency is already imported, the function is called immediately
	 * if the dependency is not imported, the function is called when the dependency is imported (if it is)
	 * @param InModuleObject The module object
	 * @param InFunction The function to call when the dependency is imported
	 * @tparam TModule The module class
	 */
	template <Solid::TStaticClassConcept TModule, typename TFunction>
	void RegisterModuleDependency(UObject* InModuleObject, TFunction&& InFunction)
	{
		RegisterModuleDependency(InModuleObject, TModule::StaticClass(),
			[InFunction = std::forward<TFunction>(InFunction)]
			(UObject* InDependencyObject, UFlecsWorld* InWorld, FFlecsEntityHandle InDependencyEntity) FORCEINLINE_ATTRIBUTE
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
	void RegisterModuleDependency(
		const UObject* InModuleObject, const TSubclassOf<UFlecsModuleInterface>& InDependencyClass,
		const std::function<void(UObject*, UFlecsWorld*, FFlecsEntityHandle)>& InFunction)
	{
		solid_check(IsValid(InModuleObject));
		solid_check(InModuleObject->Implements<UFlecsModuleInterface>());

		solid_checkf(IsModuleImported(InModuleObject->GetClass()), TEXT("Module %s is not imported"),
			*InModuleObject->GetClass()->GetName());

		FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModuleObject->GetClass());
		solid_check(ModuleEntity.IsValid());

		ModuleEntity.Add<FFlecsDependenciesComponent>();
		const flecs::ref<FFlecsDependenciesComponent> Dependencies = ModuleEntity.GetFlecsRef<FFlecsDependenciesComponent>();
		Dependencies->Dependencies.emplace(InDependencyClass, InFunction);
		
		if (IsModuleImported(InDependencyClass))
		{
			const FFlecsEntityHandle DependencyEntity = GetModuleEntity(InDependencyClass);
			UObject* DependencyModuleObject = GetModule(InDependencyClass);

			solid_check(DependencyEntity.IsValid());
			solid_check(IsValid(DependencyModuleObject));

			ModuleEntity.AddPair(flecs::DependsOn, DependencyEntity);
			InFunction(DependencyModuleObject, this, DependencyEntity);
		}
	}

	void RegisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
	{
		solid_checkf(IsValid(InAsset), TEXT("Asset is nullptr"));

		if (!InAsset->ShouldSpawn())
		{
			return;
		}

		const FFlecsEntityHandle AssetEntity = CreateEntity(InAsset->GetPrimaryAssetId().ToString());
		AssetEntity.Set<FFlecsPrimaryAssetComponent>(FFlecsPrimaryAssetComponent{ InAsset->GetPrimaryAssetId() });
		InAsset->OnEntityCreated(AssetEntity, this);
	}

	void UnregisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
	{
		solid_checkf(IsValid(InAsset), TEXT("Asset is nullptr"));

		const FFlecsEntityHandle AssetEntity = LookupEntity(InAsset->GetPrimaryAssetId().ToString());
		
		if (!AssetEntity.IsValid())
		{
			UN_LOGF(LogFlecsWorld, Log, "Asset entity %s not found",
				*InAsset->GetPrimaryAssetId().ToString());
			return;
		}
		
		InAsset->OnEntityDestroyed(AssetEntity, this);
		AssetEntity.Destroy();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void Reset()
	{
		World.reset();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ResetClock() const
	{
		World.reset_clock();
	}

	FFlecsEntityHandle CreateEntity(const FFlecsId InEntity) const
	{
		return World.entity(InEntity.GetId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntity(const FString& Name = "") const
	{
		return World.entity(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithId(const FFlecsId InId) const
	{
		solid_checkf(!World.is_alive(InId), TEXT("Entity %s is not alive"), *InId.ToString());
		return World.make_alive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		return World.entity().is_a(InPrefab.GetEntity());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithRecord(const FFlecsEntityRecord& InRecord, const FString& Name = "") const
	{
		FFlecsEntityHandle Entity = CreateEntity(Name);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}

	FFlecsEntityHandle CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord, const FFlecsId InId) const
	{
		const FFlecsEntityHandle Entity = CreateEntityWithId(InId);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World",
		meta = (AdvancedDisplay = "Separator, RootSeparator, bRecursive"))
	FFlecsEntityHandle LookupEntity(const FString& Name,
		const FString& Separator = "::", const FString& RootSeparator = "::", const bool bRecursive = true) const
	{
		return World.lookup(StringCast<char>(*Name).Get(), StringCast<char>(*Separator).Get(),
			StringCast<char>(*RootSeparator).Get(), bRecursive);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void DestroyEntityByName(const FString& Name) const
	{
		solid_checkf(!Name.IsEmpty(), TEXT("Name is empty"));
		
		FFlecsEntityHandle Handle = LookupEntity(Name);
		
		if (Handle.IsValid())
		{
			Handle.Destroy();
		}
		else
		{
			UN_LOGF(LogFlecsWorld, Warning, "Entity %s not found", *Name);
		}
	}

	template <typename FunctionType>
	void ForEach(FunctionType&& Function) const
	{
		World.each(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	void ForEach(FunctionType&& Function) const
	{
		World.each<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	void ForEach(const FFlecsId& InTermId, FunctionType&& Function) const
	{
		World.each(InTermId, std::forward<FunctionType>(Function));
	}

	template <typename T>
	void AddSingleton() const
	{
		World.add<T>();
	}

	template <typename T>
	void SetSingleton(const T& Value) const
	{
		World.set<T>(Value);
	}

	template <typename T>
	void RemoveSingleton() const
	{
		World.remove<T>();
	}
	
	template <typename T>
	NO_DISCARD bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename T>
	NO_DISCARD T GetSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *World.get<T>();
	}

	template <typename T>
	NO_DISCARD T* GetSingletonPtr()
	{
		return World.get_mut<T>();
	}

	template <typename T>
	NO_DISCARD const T* GetSingletonPtr() const
	{
		return World.get<T>();
	}

	template <typename T>
	NO_DISCARD T& GetSingletonRef()
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	NO_DISCARD const T& GetSingletonRef() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	NO_DISCARD flecs::ref<T> GetSingletonFlecsRef() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.get_ref<T>();
	}

	template <typename T>
	NO_DISCARD FFlecsEntityHandle ObtainSingletonEntity() const
	{
		return World.entity<T>();
	}

	template <typename T>
	void ModifiedSingleton() const
	{
		World.modified<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void Merge() const
	{
		World.merge();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FString GetWorldName() const
	{
		return GetWorldEntity().GetName();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void SetWorldName(const FString& InName) const
	{
		GetWorldEntity().SetName(InName);
	}

	template <typename T>
	FFlecsEntityHandle ImportFlecsModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value,
			"T must not be derived from IFlecsModuleInterface, use ImportModule(Non-Template) instead");
		return World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ImportModule(const TScriptInterface<IFlecsModuleInterface> InModule)
	{
		solid_checkf(InModule, TEXT("Module is nullptr"));
		InModule->ImportModule(World);
	}
	
	void ImportModule(UObject* InModule)
	{
		solid_checkf(InModule->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
			TEXT("Module %s does not implement UFlecsModuleInterface"), *InModule->GetName());
		
		CastChecked<IFlecsModuleInterface>(InModule)->ImportModule(World);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsModuleImported(const TSubclassOf<UObject> InModule) const
	{
		const flecs::entity ModuleEntity = ModuleComponentQuery
			.find([&](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
			{
				return InComponent.ModuleClass == InModule;
			});

		return ModuleEntity.is_valid();
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD bool IsModuleImported() const
	{
		return IsModuleImported(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetModuleEntity(const TSubclassOf<UObject>& InModule) const
	{
		const flecs::entity ModuleEntity = ModuleComponentQuery
			.find([&](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
			{
				return InComponent.ModuleClass == InModule;
			});

		return ModuleEntity;
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD FFlecsEntityHandle GetModuleEntity() const
	{
		return GetModuleEntity(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	UObject* GetModule(const TSubclassOf<UObject>& InModule) const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModule);
		return ModuleEntity.GetPtr<FFlecsUObjectComponent>()->GetObjectChecked();
	}

	template <Solid::TStaticClassConcept T>
	NO_DISCARD T* GetModule() const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity<T>();
		return ModuleEntity.GetPtr<FFlecsUObjectComponent>()->GetObjectChecked<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool BeginDefer() const
	{
		return World.defer_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool EndDefer() const
	{
		return World.defer_end();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ResumeDefer() const
	{
		World.defer_resume();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SuspendDefer() const
	{
		World.defer_suspend();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsDeferred() const
	{
		return World.is_deferred();
	}

	template <typename TFunction>
	void Defer(TFunction&& Function) const
	{
		World.defer<TFunction>(std::forward<TFunction>(Function));
	}

	template <typename TFunction>
	void DeferEndScoped(TFunction&& Function, const bool bEndDefer = false) const
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
	bool BeginReadOnly() const
	{
		return World.readonly_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void EndReadOnly() const
	{
		World.readonly_end();
	}

	void SetContext(void* InContext) const
	{
		World.set_ctx(InContext);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool Progress(const double DeltaTime = 0.0)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgress);

		{
			SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgressModule);
			
			for (const TScriptInterface<IFlecsModuleProgressInterface>& Module : ProgressModules)
			{
				solid_checkf(IsValid(Module.GetObject()), TEXT("Progress module is nullptr"));
				Module->ProgressModule(DeltaTime);
			}
		}
		
		return World.progress(DeltaTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SetTimeScale(const double InTimeScale) const
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
	void SetPipeline(const FFlecsEntityHandle& InPipeline) const
	{
		World.set_pipeline(InPipeline);
	}

	template <typename T>
	void SetPipeline() const
	{
		World.set_pipeline<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetPipeline() const
	{
		return World.get_pipeline();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	double GetDeltaTime() const
	{
		return World.delta_time();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetAlive(const FFlecsId InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetEntity(const FFlecsId InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World.make_alive(InId);
	}

	template <typename ...TComponents>
	NO_DISCARD flecs::system_builder<TComponents...> CreateSystemWithBuilder(const FString& InName) const
	{
		return World.system<TComponents...>(StringCast<char>(*InName).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetWorldEntity() const
	{
		return World.entity(EcsWorld);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	int32 GetStageCount() const
	{
		return World.get_stage_count();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	int32 GetStageId() const
	{
		return World.get_stage_id();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsStage() const
	{
		return World.is_stage();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsReadOnly() const
	{
		return World.is_readonly();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void PreallocateEntities(const int32 InEntityCount) const
	{
		World.dim(InEntityCount);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SetEntityRange(const int32 InMin, const int32 InMax) const
	{
		World.set_entity_range(InMin, InMax);
	}

	template <typename FunctionType>
	void SetEntityRange(const int32 InMin, const int32 InMax, const bool bEnforceEntityRange, FunctionType&& Function) const
	{
		const int32 OldMin = static_cast<int32>(ecs_get_world_info(World.c_ptr())->min_id);
		const int32 OldMax = static_cast<int32>(ecs_get_world_info(World.c_ptr())->max_id);
		
		World.set_entity_range(InMin, InMax);
		EnforceEntityRange(bEnforceEntityRange);
		std::forward<FunctionType>(Function)();
		EnforceEntityRange(false);

		World.set_entity_range(OldMin, OldMax);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void EnforceEntityRange(const bool bInEnforce) const
	{
		World.enable_range_check(bInEnforce);
	}

	template <typename FunctionType>
	void ForEachChild(FunctionType&& Function) const
	{
		World.children(std::forward<FunctionType>(Function));
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void SetThreads(const int32 InThreadCount) const
	{
		World.set_threads(InThreadCount);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	int32 GetThreads() const
	{
		return World.get_threads();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool UsingTaskThreads() const
	{
		return World.using_task_threads();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void SetTaskThreads(const int32 InThreadCount) const
	{
		World.set_task_threads(InThreadCount);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool HasScriptStruct(const UScriptStruct* ScriptStruct) const
	{
		return TypeMapComponent->ScriptStructMap.contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetScriptStructEntity(const UScriptStruct* ScriptStruct) const
	{
		return TypeMapComponent->ScriptStructMap.at(ScriptStruct);
	}

	template <Solid::TStaticStructConcept T>
	NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(StaticStruct<T>());
	}

	template <Solid::TStaticStructConcept T>
	NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(StaticStruct<T>());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool HasScriptEnum(UEnum* ScriptEnum) const
	{
		return TypeMapComponent->ScriptEnumMap.contains(ScriptEnum);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetScriptEnumEntity(UEnum* ScriptEnum) const
	{
		return TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
	}
	
	template <typename T>
	NO_DISCARD bool HasScriptEnum() const
	{
		return HasScriptEnum(StaticEnum<T>());
	}

	template <typename T>
	NO_DISCARD FFlecsEntityHandle GetScriptEnumEntity() const
	{
		return GetScriptEnumEntity(StaticEnum<T>());
	}

	void RegisterMemberProperties(const UStruct* InStruct,
		const FFlecsEntityHandle& InEntity) const
	{
		flecs::untyped_component UntypedComponent = InEntity.GetUntypedComponent_Unsafe();
		
		for (TFieldIterator<FProperty> PropertyIt(InStruct); PropertyIt; ++PropertyIt)
		{
			FProperty* Property = *PropertyIt;
			solid_checkf(Property != nullptr, TEXT("Property is nullptr"));
			
			if (Property->IsA<FBoolProperty>())
			{
				UntypedComponent.member<bool>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FByteProperty>())
			{
				UntypedComponent.member<uint8>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FIntProperty>())
			{
				UntypedComponent.member<int32>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt32Property>())
			{
				UntypedComponent.member<uint32>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FInt64Property>())
			{
				UntypedComponent.member<int64>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt64Property>())
			{
				UntypedComponent.member<uint64>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				UntypedComponent.member<float>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				UntypedComponent.member<double>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStrProperty>())
			{
				UntypedComponent.member<FString>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FNameProperty>())
			{
				UntypedComponent.member<FName>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FTextProperty>())
			{
				UntypedComponent.member<FText>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FObjectProperty>())
			{
				UntypedComponent.member<FObjectPtr>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FWeakObjectProperty>())
			{
				UntypedComponent.member<FWeakObjectPtr>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FSoftObjectProperty>())
			{
				UntypedComponent.member<FSoftObjectPtr>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FClassProperty>())
			{
				UntypedComponent.member<TSubclassOf<UObject>>(StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FEnumProperty>())
			{
				UntypedComponent.member(flecs::meta::EnumType, StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStructProperty>())
			{
				FFlecsEntityHandle StructComponent
					= ObtainComponentTypeStruct(CastFieldChecked<FStructProperty>(Property)->Struct);
				UntypedComponent.member(StructComponent,
					StringCast<char>(*Property->GetName()).Get(), 1,
					Property->GetOffset_ForInternal());
			}
			else
			{
				UN_LOGF(LogFlecsWorld, Warning,
					"Property Type: %s is not supported", *Property->GetName());
			}
		}
	}

	void RegisterEnumProperties(const UEnum* Enum, const FFlecsEntityHandle& InComponentEntity) const
	{
		flecs::untyped_component UntypedComponent = InComponentEntity.GetUntypedComponent_Unsafe();
		
		if (Enum->HasAnyEnumFlags(EEnumFlags::Flags))
		{
			for (int32 Index = 0; Index < Enum->GetMaxEnumValue(); ++Index)
			{
				const uint32 FlagValue = static_cast<uint32>(1) << Index;
				UntypedComponent.bit(StringCast<char>(*Enum->GetNameStringByIndex(Index)).Get(), FlagValue);
			}
		}
		else
		{
			for (int32 Index = 0; Index < Enum->GetMaxEnumValue(); ++Index)
			{
				UntypedComponent.constant(StringCast<char>(*Enum->GetNameStringByIndex(Index)).Get(), Index);
			}
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptEnum(UEnum* Enum) const
	{
		solid_check(IsValid(Enum));

		const FFlecsEntityHandle OldScope = ClearScope();

		if (const FFlecsEntityHandle Handle = LookupEntity(Enum->GetName()))
		{
			RegisterEnumProperties(Enum, Handle);
			SetScope(OldScope);
			return Handle;
		}
		
		FFlecsEntityHandle EnumComponent = World.component(StringCast<char>(*Enum->GetName()).Get())
			.set<flecs::Component>({ .size = sizeof(int32), .alignment = alignof(int32) })
			.set<FFlecsScriptEnumComponent>({ Enum });
		
		TypeMapComponent->ScriptEnumMap.emplace(Enum, EnumComponent.GetEntity());

		RegisterEnumProperties(Enum, EnumComponent);

		SetScope(OldScope);
		return EnumComponent;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptStruct(const UScriptStruct* ScriptStruct) const
	{
		solid_check(IsValid(ScriptStruct));

		const FFlecsEntityHandle OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptStructMap.contains(ScriptStruct),
			TEXT("Script struct %s is already registered"), *ScriptStruct->GetStructCPPName());
		
		flecs::untyped_component ScriptStructComponent;

		DeferEndScoped([this, ScriptStruct, &ScriptStructComponent]()
		{
			ScriptStructComponent = World.component(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get());
			solid_check(ScriptStructComponent.is_valid());
			ScriptStructComponent.set_symbol(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get());
			ScriptStructComponent.set<flecs::Component>(
			{ .size = ScriptStruct->GetStructureSize(), .alignment = ScriptStruct->GetMinAlignment() });
		});

		ScriptStructComponent.set<FFlecsScriptStructComponent>({ ScriptStruct });

		SetScope(OldScope);
		return ScriptStructComponent;
	}

	// @TODO: May be deprecated in the future
	void RegisterScriptStruct(UScriptStruct* ScriptStruct, const FFlecsEntityHandle& InComponentEntity) const
	{
		TypeMapComponent->ScriptStructMap
			.emplace(ScriptStruct, InComponentEntity.GetEntity());
	}

	template <typename T>
	FFlecsEntityHandle ObtainComponentType() const
	{
		return World.component<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle ObtainComponentTypeStruct(const UScriptStruct* ScriptStruct) const
	{
		if (HasScriptStruct(ScriptStruct))
		{
			return GetScriptStructEntity(ScriptStruct);
		}

		return RegisterScriptStruct(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle ObtainScriptEnumType(UEnum* Enum) const
	{
		if (HasScriptEnum(Enum))
		{
			return GetScriptEnumEntity(Enum);
		}

		return RegisterScriptEnum(Enum);
	}

	template <typename ...TComponents>
	flecs::observer_builder<TComponents...> CreateObserver(const FString& Name = "") const
	{
		return World.observer<TComponents...>(StringCast<char>(*Name).Get());
	}

	NO_DISCARD flecs::observer_builder<> CreateObserver(const FString& Name = "") const
	{
		return World.observer<>(StringCast<char>(*Name).Get());
	}

	template <typename ...TComponents, typename ...TArgs>
	flecs::observer_builder<TComponents...> CreateObserver(
		const FFlecsEntityHandle& InEntity, TArgs&&... Args) const
	{
		return World.observer<TComponents...>(InEntity.GetEntity(), std::forward<TArgs>(Args)...);
	}

	FFlecsEntityHandle CreateObserver(const FFlecsEntityHandle& InEntity) const
	{
		return World.observer(InEntity.GetEntity());
	}

	NO_DISCARD flecs::event_builder Event(const FFlecsEntityHandle& InEntity) const
	{
		return World.event(InEntity.GetEntity());
	}

	template <typename TEvent>
	flecs::event_builder_typed<TEvent> Event() const
	{
		return World.event<TEvent>();
	}

	NO_DISCARD flecs::pipeline_builder<> CreatePipeline() const
	{
		return World.pipeline();
	}

	template <typename ...TComponents>
	NO_DISCARD flecs::pipeline_builder<TComponents...> CreatePipeline() const
	{
		return World.pipeline<TComponents...>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	void RunPipeline(const FFlecsEntityHandle& InPipeline, const double DeltaTime = 0.0) const
	{
		World.run_pipeline(InPipeline.GetEntity(), DeltaTime);
	}

	void RandomizeTimers() const
	{
		World.randomize_timers();
	}

	NO_DISCARD flecs::timer CreateTimer(const FString& Name) const
	{
		return World.timer(StringCast<char>(*Name).Get());
	}

	template <typename T>
	NO_DISCARD flecs::timer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	bool HasEntityWithName(const FString& Name) const
	{
		return LookupEntity(Name).IsValid();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const
	{
		solid_checkf(Tag.IsValid(), TEXT("Tag is not valid"));
		return LookupEntity(StringCast<char>(*Tag.GetTagName().ToString()).Get(),
		 ".", ".");
	}

	template <typename T>
	void EnableType() const
	{
		World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	void EnableType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Enable();
	}

	template <typename T>
	void DisableType() const
	{
		World.component<T>().disable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	void DisableType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Disable();
	}

	template <typename T>
	NO_DISCARD bool IsTypeEnabled() const
	{
		return World.component<T>().enabled();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool IsTypeEnabled(UScriptStruct* ScriptStruct) const
	{
		return ObtainComponentTypeStruct(ScriptStruct).IsEnabled();
	}

	template <typename T>
	void ToggleType() const
	{
		World.component<T>().enabled() ? World.component<T>().disable() : World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	void ToggleType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Toggle();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord, const FString& Name = "") const
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
	FFlecsEntityHandle CreatePrefab(const FString& Name) const
	{
		return World.prefab(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle FindPrefabEntity(UFlecsPrefabAsset* InPrefabAsset) const
	{
		solid_checkf(IsValid(InPrefabAsset), TEXT("Prefab asset is nullptr"));
		return LookupEntity(InPrefabAsset->GetPrimaryAssetId().ToString());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		InPrefab.Destroy();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool ShouldQuit() const
	{
		return World.should_quit();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle ClearScope() const
	{
		return World.set_scope(0);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle SetScope(const FFlecsEntityHandle& InScope) const
	{
		return World.set_scope(InScope.GetEntity());
	}

	template <typename T>
	void Scope() const
	{
		World.scope<T>();
	}

	template <typename T, typename FunctionType>
	void Scope(FunctionType&& Function) const
	{
		World.scope<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	void Scope(FFlecsId InId, FunctionType&& Function) const
	{
		World.scope(InId, std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	void EndScope(FunctionType&& Function) const
	{
		const flecs::entity OldScope = World.set_scope(0);

		std::invoke(std::forward<FunctionType>(Function));

		World.set_scope(OldScope);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScope() const
	{
		return World.get_scope();
	}

	FORCEINLINE virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	}

	FORCEINLINE virtual bool IsNameStableForNetworking() const override
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	UFlecsWorldSubsystem* GetContext() const;
	
	flecs::world World;

	UPROPERTY()
	TArray<TScriptInterface<IFlecsModuleProgressInterface>> ProgressModules;

	flecs::query<FFlecsModuleComponent> ModuleComponentQuery;
	flecs::query<FFlecsUObjectComponent> ObjectDestructionComponentQuery;
	flecs::query<FFlecsDependenciesComponent> DependenciesComponentQuery;

	FFlecsTypeMapComponent* TypeMapComponent;
	
}; // class UFlecsWorld
