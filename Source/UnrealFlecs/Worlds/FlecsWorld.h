// Solstice Games © 2024. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsTypeMapComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Entities/FlecsEntityRecord.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsModuleInterface.h"
#include "Prefabs/FlecsPrefabAsset.h"
#include "Prefabs/FlecsPrefabComponent.h"
#include "Systems/FlecsSystem.h"
#include "Timers/FlecsTimer.h"
#include "FlecsWorld.generated.h"

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"), STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	
	FORCEINLINE void WorldBeginPlay()
	{
		UN_LOG(LogFlecsWorld, Log, "Flecs World begin play");
		
		InitializeSystems();
		InitializeDefaultComponents();
		InitializeAssetRegistry();
	}

	FORCEINLINE void InitializeDefaultComponents()
	{
		/* Opaque FString to flecs::string */
		World.component<FString>()
			.opaque(flecs::String);
	}

	FORCEINLINE void InitializeSystems() const
	{
		static constexpr int32 UOBJECT_VALID_CHECK_FRAME_RATE = 60;

		ObtainComponentTypeStruct(FFlecsScriptStructComponent::StaticStruct());
		CreateObserver<flecs::_::type_impl_struct_event_info>(TEXT("StructComponentObserver"))
				.cached()
				.event(flecs::OnSet)
				.yield_existing()
				.each([&](flecs::entity InEntity, flecs::_::type_impl_struct_event_info InEventInfo)
				{
					solid_checkf(InEventInfo.scriptStruct != nullptr, TEXT("Script struct is nullptr"));

					if UNLIKELY_IF(InEventInfo.scriptStruct == FFlecsScriptStructComponent::StaticStruct())
					{
						return;
					}

					if UNLIKELY_IF(InEntity.has<FFlecsScriptStructComponent>())
					{
						InEntity.remove<flecs::_::type_impl_struct_event_info>();
						return;
					}
					
					InEntity.set<FFlecsScriptStructComponent>({ InEventInfo.scriptStruct });

					UN_LOGF(LogFlecsWorld, Log,
						"Struct component %s registered", *InEventInfo.scriptStruct->GetStructCPPName());

					#if WITH_EDITOR

					this->RegisterMemberProperties(InEventInfo.scriptStruct, InEntity);
					
					#endif // WITH_EDITOR

					InEntity.remove<flecs::_::type_impl_struct_event_info>();
				});
		
		CreateSystemWithBuilder<const FFlecsUObjectComponent&>(TEXT("FlecsUObjectComponentSystem"))
			.kind(flecs::PostFrame)
			.term_at(0)
				.read()
			.rate(UOBJECT_VALID_CHECK_FRAME_RATE)
			.multi_threaded()
			.each([](flecs::entity InEntity, const FFlecsUObjectComponent& InComponent)
			{
				if (!InComponent.IsValid())
				{
					InEntity.destruct();
				}
			});
	}

	FORCEINLINE void InitializeAssetRegistry()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		TArray<FAssetData> AssetData;
		AssetRegistry.GetAssetsByClass(FTopLevelAssetPath(UFlecsPrimaryDataAsset::StaticClass()), AssetData,
			true);

		AssetRegistry.OnAssetAdded().AddLambda([&](const FAssetData& InAssetData)
		{
			RegisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset()));
		});

		for (const FAssetData& Asset : AssetData)
		{
			RegisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(Asset.GetAsset()));
		}
		
		AssetRegistry.OnAssetRemoved().AddLambda([&](const FAssetData& InAssetData)
		{
			UnregisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset()));
		});
	}

	FORCEINLINE void RegisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
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

	FORCEINLINE void UnregisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
	{
		solid_checkf(IsValid(InAsset), TEXT("Asset is nullptr"));

		if (!InAsset->ShouldSpawn())
		{
			return;
		}

		const FFlecsEntityHandle AssetEntity = LookupEntity(InAsset->GetPrimaryAssetId().ToString());
		
		if UNLIKELY_IF(!AssetEntity.IsValid())
		{
			UN_LOGF(LogFlecsWorld, Warning, "Asset entity %s not found",
				*InAsset->GetPrimaryAssetId().ToString());
			return;
		}
		
		InAsset->OnEntityDestroyed(AssetEntity, this);
		AssetEntity.Destroy();
	}
	
	FORCEINLINE void SetWorld(flecs::world&& InWorld)
	{
		World = std::move(InWorld);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE void Reset()
	{
		World.reset();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void ResetClock() const
	{
		World.reset_clock();
	}

	FORCEINLINE FFlecsEntityHandle CreateEntity(const flecs::entity_t InEntity) const
	{
		return World.make_alive(InEntity);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntity(const FString& Name = "") const
	{
		if (Name.IsEmpty())
		{
			World.entity();
		}

		return World.entity(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntityWithId(const FFlecsId& InId) const
	{
		return World.entity(InId.GetFlecsId());
	}

	FORCEINLINE FFlecsEntityHandle CreateEntityWithId(const flecs::entity_t InId) const
	{
		return FFlecsEntityHandle(World.make_alive(InId));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		return World.entity(InPrefab.GetEntity());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntityWithRecord(const FFlecsEntityRecord& InRecord) const
	{
		FFlecsEntityHandle Entity = CreateEntity(InRecord.Name);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}

	FORCEINLINE FFlecsEntityHandle CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
		const flecs::entity_t InId) const
	{
		FFlecsEntityHandle Entity = CreateEntityWithId(InId);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle LookupEntity(const FString& Name) const
	{
		return World.lookup(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void DestroyEntityByName(const FString& Name, const bool bSearchPath = true) const
	{
		World.delete_with(reinterpret_cast<flecs::entity_t>(StringCast<char>(*Name).Get()), bSearchPath);
	}

	template <typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function) const
	{
		World.each(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function) const
	{
		World.each<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE void ForEach(const FFlecsId& InTermId, FunctionType&& Function) const
	{
		World.each(InTermId.GetFlecsId(), std::forward<FunctionType>(Function));
	}

	template <typename T>
	FORCEINLINE void AddSingleton() const
	{
		World.add<T>();
	}

	template <typename T>
	FORCEINLINE void SetSingleton(const T& Value) const
	{
		World.set<T>(Value);
	}

	template <typename First, typename Second>
	FORCEINLINE void SetSingleton(const First& InFirst, const Second InSecond) const
	{
		World.set<First, Second>(InSecond, InFirst);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE void SetSingleton(const First& InFirst, const TArgs&... Args) const
	{
		World.set(std::forward<const TArgs&>(Args)..., InFirst);
	}

	template <typename T>
	FORCEINLINE void RemoveSingleton() const
	{
		World.remove<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton() const
	{
		World.remove<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton(const Second& InSecond) const
	{
		World.remove<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE void RemoveSingleton(const FFlecsId& InSecond) const
	{
		World.remove<First>(InSecond);
	}
	
	template <typename T>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World.has<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton(const Second& InSecond) const
	{
		return World.has<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE NO_DISCARD bool HasSingleton(const FFlecsId& InSecond) const
	{
		return World.has<First>(InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World.has<First, TArgs...>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD T GetSingleton() const
	{
		return *World.get<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD First GetSingleton(const Second& InSecond) const
	{
		return *World.get<First, Second>(InSecond);
	}

	template <typename T>
	FORCEINLINE NO_DISCARD const T* GetSingletonPtr() const
	{
		return World.get<T>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD T& GetSingletonRef()
	{
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD const T& GetSingletonRef() const
	{
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD flecs::ref<T> GetSingletonFlecsRef() const
	{
		return World.get_ref<T>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetSingletonEntity() const
	{
		return World.entity<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void Merge() const
	{
		World.merge();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FString GetWorldName() const
	{
		return GetSingleton<FString>();
	}
	
	FORCEINLINE void SetWorldName(const FString& InName) const
	{
		SetSingleton<FString>(InName);

		#if WITH_EDITOR

		const char* Name = StringCast<char>(*InName).Get();

		GetWorldEntity().SetDocName(Name);
		
		World.entity<FString>().set_doc_name(Name);

		#endif // WITH_EDITOR
	}

	template <typename T>
	FORCEINLINE void ImportModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value, "T must not be derived from IFlecsModuleInterface");
		World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void ImportModule(const TScriptInterface<IFlecsModuleInterface> InModule) const
	{
		solid_checkf(InModule != nullptr, TEXT("Module is nullptr"));
		InModule->ImportModule(World);
	}
	
	FORCEINLINE void ImportModule(UObject* InModule) const
	{
		solid_checkf(InModule->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
			TEXT("Module %s does not implement UFlecsModuleInterface"), *InModule->GetName());
		
		CastChecked<IFlecsModuleInterface>(InModule)->ImportModule(World);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool IsModuleImported(const TSubclassOf<UFlecsModuleInterface> InModule) const
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE bool BeginDefer() const
	{
		return World.defer_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE bool EndDefer() const
	{
		return World.defer_end();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool IsDeferred() const
	{
		return World.is_deferred();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE bool BeginReadOnly() const
	{
		return World.readonly_begin();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void EndReadOnly() const
	{
		World.readonly_end();
	}

	FORCEINLINE void SetContext(void* InContext) const
	{
		World.set_ctx(InContext);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE bool Progress(const double DeltaTime = 0.0) const
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgress);
		return World.progress(DeltaTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetTimeScale(const double InTimeScale) const
	{
		World.set_time_scale(InTimeScale);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetTargetFps(const double InTargetFps) const
	{
		World.set_target_fps(InTargetFps);
	}
	
	FORCEINLINE void Quit() const
	{
		World.quit();
	}
	
	FORCEINLINE void DestroyWorld()
	{
		const FAssetRegistryModule& AssetRegistryModule
			= FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnAssetAdded().RemoveAll(this);
		
		World.~world();
		MarkAsGarbage();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetPipeline(const FFlecsEntityHandle& InPipeline) const
	{
		World.set_pipeline(InPipeline);
	}

	template <typename T>
	FORCEINLINE void SetPipeline() const
	{
		World.set_pipeline<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle GetPipeline() const
	{
		return World.get_pipeline();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE double GetDeltaTime() const
	{
		return World.delta_time();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle GetAlive(const FFlecsId& InId) const
	{
		return World.get_alive(InId.GetFlecsId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World.make_alive(InId.GetFlecsId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsSystem CreateSystem(const FString& InName) const
	{
		return CreateSystem<>(InName);
	}

	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD FFlecsSystem CreateSystem(const FString& InName) const
	{
		const FFlecsEntityHandle SystemEntity = CreateEntity(InName);
		const FFlecsSystem System = World.system(SystemEntity.GetEntity());
		System.ToBuilder<TComponents...>();
		return System;
	}

	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD flecs::system_builder<TComponents...> CreateSystemWithBuilder(const FString& InName) const
	{
		return World.system<TComponents...>(StringCast<char>(*InName).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle GetWorldEntity() const
	{
		return World.entity(EcsWorld);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE int32 GetStageCount() const
	{
		return World.get_stage_count();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE int32 GetStageId() const
	{
		return World.get_stage_id();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool IsStage() const
	{
		return World.is_stage();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool IsReadOnly() const
	{
		return World.is_readonly();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void PreallocateEntities(const int32 InEntityCount) const
	{
		World.dim(InEntityCount);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetEntityRange(const int32 InMin, const int32 InMax) const
	{
		World.set_entity_range(InMin, InMax);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void EnforceEntityRange(const bool bInEnforce) const
	{
		World.enable_range_check(bInEnforce);
	}

	template <typename FunctionType>
	FORCEINLINE void ForEachChild(FunctionType&& Function) const
	{
		World.children(std::forward<FunctionType>(Function));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE void SetThreads(const int32 InThreadCount) const
	{
		World.set_threads(InThreadCount);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE int32 GetThreads() const
	{
		return World.get_threads();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool UsingTaskThreads() const
	{
		return World.using_task_threads();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE void SetTaskThreads(const int32 InThreadCount) const
	{
		World.set_task_threads(InThreadCount);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE bool HasScriptStruct(UScriptStruct* ScriptStruct) const
	{
		return GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle GetScriptStructEntity(UScriptStruct* ScriptStruct) const
	{
		return GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.at(ScriptStruct);
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(T::StaticStruct());
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(T::StaticStruct());
	}

	#if WITH_EDITOR

	FORCEINLINE void RegisterMemberProperties(const UScriptStruct* ScriptStruct,
		const FFlecsEntityHandle& InEntity) const
	{
		flecs::untyped_component UntypedComponent = InEntity.GetUntypedComponent();
		
		for (TFieldIterator<FProperty> PropertyIt(ScriptStruct); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			solid_checkf(Property != nullptr, TEXT("Property is nullptr"));
			
			if (Property->IsA<FBoolProperty>())
			{
				UntypedComponent.member<bool>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FByteProperty>())
			{
				UntypedComponent.member<uint8>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FIntProperty>())
			{
				UntypedComponent.member<int32>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FUInt32Property>())
			{
				UntypedComponent.member<uint32>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FInt64Property>())
			{
				UntypedComponent.member<int64>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FUInt64Property>())
			{
				UntypedComponent.member<uint64>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				UntypedComponent.member<float>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				UntypedComponent.member<double>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FStrProperty>())
			{
				UntypedComponent.member<FString>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FNameProperty>())
			{
				UntypedComponent.member<FName>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FTextProperty>())
			{
				UntypedComponent.member<FText>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FObjectProperty>())
			{
				UntypedComponent.member<TObjectPtr<UObject>>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FWeakObjectProperty>())
			{
				UntypedComponent.member<TWeakObjectPtr<UObject>>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FSoftObjectProperty>())
			{
				UntypedComponent.member<TSoftObjectPtr<UObject>>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FClassProperty>())
			{
				UntypedComponent.member<TSubclassOf<UObject>>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FEnumProperty>())
			{
				UntypedComponent.member(flecs::meta::EnumType, StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FStructProperty>())
			{
				UntypedComponent.member<TStructOnScope<FStructProperty>>(StringCast<char>(*Property->GetName()).Get());
			}
		}
	}

	#endif // WITH_EDITOR
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct) const
	{
		if UNLIKELY_IF(ScriptStruct == nullptr)
		{
			return FFlecsEntityHandle();
		}

		if (const FFlecsEntityHandle Handle = World.lookup(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get()))
		{
			RegisterScriptStruct(ScriptStruct, Handle);
			return Handle;
		}
		
		FFlecsEntityHandle ScriptStructComponent = World.entity(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get())
			.set<flecs::Component>({ ScriptStruct->GetStructureSize(), ScriptStruct->GetMinAlignment() })
			.set<FFlecsScriptStructComponent>({ ScriptStruct });
		
		GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.emplace(ScriptStruct, ScriptStructComponent);

		#if WITH_EDITOR

		this->RegisterMemberProperties(ScriptStruct, ScriptStructComponent);

		#endif // WITH_EDITOR

		if (ScriptStruct->GetSuperStruct())
		{
			FFlecsEntityHandle ParentEntity;
			
			if (!HasScriptStruct(static_cast<UScriptStruct*>(ScriptStruct->GetSuperStruct())))
			{
				ParentEntity = RegisterScriptStruct(static_cast<UScriptStruct*>(ScriptStruct->GetSuperStruct()));
			}
			else
			{
				ParentEntity = GetScriptStructEntity(static_cast<UScriptStruct*>(ScriptStruct->GetSuperStruct()));
			}
			
			ScriptStructComponent.SetParent(ParentEntity, true);
		}
		
		return ScriptStructComponent;
	}

	FORCEINLINE void RegisterScriptStruct(const UScriptStruct* ScriptStruct, FFlecsEntityHandle InComponentEntity) const
	{
		GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap
			.emplace(const_cast<UScriptStruct*>(ScriptStruct), InComponentEntity);

		#if WITH_EDITOR

		this->RegisterMemberProperties(ScriptStruct, InComponentEntity);

		#endif // WITH_EDITOR
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle ObtainComponentTypeStruct(UScriptStruct* ScriptStruct) const
	{
		if (HasScriptStruct(ScriptStruct))
		{
			return GetScriptStructEntity(ScriptStruct);
		}

		return RegisterScriptStruct(ScriptStruct);
	}
	
	template <typename ...TComponents>
	FORCEINLINE flecs::observer_builder<TComponents...> CreateObserver(const FString& Name) const
	{
		return World.observer<TComponents...>(StringCast<char>(*Name).Get());
	}

	FORCEINLINE flecs::observer_builder<> CreateObserver(const FString& Name) const
	{
		return World.observer<>(StringCast<char>(*Name).Get());
	}

	template <typename ...TComponents, typename ...TArgs>
	FORCEINLINE flecs::observer_builder<TComponents...> CreateObserver(const FFlecsEntityHandle& InEntity, TArgs&&... Args) const
	{
		return World.observer<TComponents...>(InEntity.GetEntity(), std::forward<TArgs>(Args)...);
	}

	FORCEINLINE FFlecsEntityHandle CreateObserver(const FFlecsEntityHandle& InEntity) const
	{
		return World.observer(InEntity.GetEntity());
	}

	FORCEINLINE flecs::event_builder Event(const FFlecsEntityHandle& InEntity) const
	{
		return World.event(InEntity.GetEntity());
	}

	template <typename TEvent>
	FORCEINLINE flecs::event_builder Event() const
	{
		return World.event<TEvent>();
	}

	FORCEINLINE NO_DISCARD flecs::pipeline_builder<> CreatePipeline() const
	{
		return World.pipeline();
	}

	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD flecs::pipeline_builder<TComponents...> CreatePipeline() const
	{
		return World.pipeline<TComponents...>();
	}

	FORCEINLINE void RandomizeTimers() const
	{
		World.randomize_timers();
	}

	template <typename ...TArgs>
	FORCEINLINE NO_DISCARD FFlecsTimer CreateTimer(const TArgs&... Args) const
	{
		return World.timer(Args...);
	}

	template <typename T>
	FORCEINLINE NO_DISCARD FFlecsTimer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE bool HasEntityWithName(const FString& Name) const
	{
		return World.lookup(StringCast<char>(*Name).Get()).is_valid();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const
	{
		solid_checkf(Tag.IsValid(), TEXT("Tag is not valid"));
		return World.lookup(StringCast<char>(*Tag.GetTagName().ToString()).Get(), ".", ".");
	}

	template <typename T>
	FORCEINLINE void EnableType() const
	{
		World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE void EnableType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Enable();
	}

	template <typename T>
	FORCEINLINE void DisableType() const
	{
		World.component<T>().disable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE void DisableType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Disable();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD bool IsTypeEnabled() const
	{
		return World.component<T>().enabled();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE bool IsTypeEnabled(UScriptStruct* ScriptStruct) const
	{
		return ObtainComponentTypeStruct(ScriptStruct).IsEnabled();
	}

	template <typename T>
	FORCEINLINE void ToggleType() const
	{
		World.component<T>().enabled() ? World.component<T>().disable() : World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE void ToggleType(UScriptStruct* ScriptStruct) const
	{
		ObtainComponentTypeStruct(ScriptStruct).Toggle();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE UFlecsWorld* GetParentWorld() const
	{
		solid_checkf(IsStage(), TEXT("World is not a stage"));
		return GetTypedOuter<UFlecsWorld>(); 
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle CreatePrefab(const FFlecsEntityRecord& InRecord) const
	{
		FFlecsEntityHandle Prefab = World.prefab(StringCast<char>(*InRecord.Name).Get());
		solid_checkf(Prefab.IsPrefab(), TEXT("Entity is not a prefab"));
		
		InRecord.ApplyRecordToEntity(Prefab);

		FFlecsPrefabComponent PrefabComponent;
		PrefabComponent.EntityRecord = InRecord;

		#if WITH_EDITOR
		
		Prefab.SetDocName(InRecord.Name);

		#endif // WITH_EDITOR

		Prefab.Set<FFlecsPrefabComponent>(PrefabComponent);
		
		return Prefab;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle FindPrefabEntity(UFlecsPrefabAsset* InPrefabAsset) const
	{
		solid_checkf(IsValid(InPrefabAsset), TEXT("Prefab asset is nullptr"));
		return World.lookup(StringCast<char>(*InPrefabAsset->GetPrimaryAssetId().ToString()).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE void DestroyPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		World.delete_with(InPrefab.GetEntity(), true);
	}
	
	flecs::world World;
	
}; // class UFlecsWorld
