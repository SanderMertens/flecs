// Solstice Games © 2024. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsTypeMapComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Entities/FlecsEntityRecord.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleProgressInterface.h"
#include "Prefabs/FlecsPrefabAsset.h"
#include "Prefabs/FlecsPrefabComponent.h"
#include "Systems/FlecsSystem.h"
#include "FlecsWorld.generated.h"

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"), STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);
DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress::ProgressModule"), STAT_FlecsWorldProgressModule, STATGROUP_FlecsWorld);

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UFlecsWorld() override
	{
		if (World.c_ptr() != nullptr)
		{
			DestroyWorld();
			World.world_ = nullptr;
		}
	}
	
	FORCEINLINE_DEBUGGABLE void WorldBeginPlay()
	{
		UN_LOG(LogFlecsWorld, Log, "Flecs World begin play");
		
		InitializeSystems();
		InitializeDefaultComponents();
		InitializeAssetRegistry();
		InitializeOSAPI();
	}

	FORCEINLINE_DEBUGGABLE void InitializeOSAPI() const
	{
		struct FFlecsThread
        {
            FRunnable* Runnable;
            FRunnableThread* Thread;
        }; // struct FFlecsThread
		
        class FFlecsRunnable : public FRunnable
        {
        public:
            FFlecsRunnable(ecs_os_thread_callback_t InCallback, void* InParam)
                : Callback(InCallback), Param(InParam)
            {
            }

            virtual uint32 Run() override
            {
                Callback(Param);
                return 0;
            }

        private:
            ecs_os_thread_callback_t Callback;
            void* Param;
        }; // class FFlecsRunnable

        ecs_os_set_api_defaults();

        // Thread functions
        ecs_os_api_t os_api = ecs_os_api;

        os_api.thread_new_ = [](ecs_os_thread_callback_t Callback, void* Param) -> ecs_os_thread_t
        {
            // Allocate memory without zero-initialization for speed
            FFlecsThread* FlecsThread = static_cast<FFlecsThread*>(FMemory::Malloc(sizeof(FFlecsThread)));
        	
            FlecsThread->Runnable = new FFlecsRunnable(Callback, Param);
            FlecsThread->Thread = FRunnableThread::Create(FlecsThread->Runnable,
            	TEXT("FlecsThread"), 0, TPri_Normal);

            return reinterpret_cast<ecs_os_thread_t>(FlecsThread);
        };

        os_api.thread_join_ = [](ecs_os_thread_t Thread) -> void*
        {
	        FFlecsThread* FlecsThread = reinterpret_cast<FFlecsThread*>(Thread);
        	
            if (FlecsThread && FlecsThread->Thread)
            {
                FlecsThread->Thread->WaitForCompletion();
                delete FlecsThread->Runnable;
                delete FlecsThread->Thread;
                FMemory::Free(FlecsThread);
            }
        	
            return nullptr;
        };
		
        struct FFlecsSpinLock
        {
            volatile int32 LockFlag = 0;
        }; // struct FFlecsSpinLock

        os_api.mutex_new_ = []() -> ecs_os_mutex_t
        {
	        FFlecsSpinLock* SpinLock = static_cast<FFlecsSpinLock*>(FMemory::Malloc(sizeof(FFlecsSpinLock)));
            SpinLock->LockFlag = 0;
            return reinterpret_cast<ecs_os_mutex_t>(SpinLock);
        };

        os_api.mutex_free_ = [](ecs_os_mutex_t Mutex)
        {
	        FFlecsSpinLock* SpinLock = reinterpret_cast<FFlecsSpinLock*>(Mutex);
            FMemory::Free(SpinLock);
        };

        os_api.mutex_lock_ = [](ecs_os_mutex_t Mutex)
        {
	        FFlecsSpinLock* SpinLock = reinterpret_cast<FFlecsSpinLock*>(Mutex);
            while (FPlatformAtomics::InterlockedCompareExchange(&SpinLock->LockFlag, 1, 0) != 0)
            {
                // Busy wait (spin)
            }
        };

        os_api.mutex_unlock_ = [](ecs_os_mutex_t Mutex)
        {
	        FFlecsSpinLock* SpinLock = reinterpret_cast<FFlecsSpinLock*>(Mutex);
            FPlatformAtomics::InterlockedExchange(&SpinLock->LockFlag, 0);
        };

        // Sleep function using minimal overhead
        os_api.sleep_ = [](int32_t Seconds, int32_t Nanoseconds)
        {
            const double TotalSeconds = Seconds + Nanoseconds / 1e9;
            FPlatformProcess::SleepNoStats(static_cast<float>(TotalSeconds));
        };

        // High-resolution timer
        os_api.now_ = []() -> uint64_t
        {
            return FPlatformTime::Cycles64();
        };

        os_api.get_time_ = [](ecs_time_t* TimeOut)
        {
            const double Seconds = FPlatformTime::Seconds();
            TimeOut->sec = static_cast<int32_t>(Seconds);
            TimeOut->nanosec = static_cast<int32_t>((Seconds - TimeOut->sec) * 1e9);
        };

        os_api.abort_ = []()
        {
            UN_LOG(LogFlecsCore, Fatal, "Flecs: abort");
        };

        os_api.log_ = [](int32_t Level, const char* File, int32_t Line, const char* Message)
        {
#if UNLOG_ENABLED
            switch (Level)
            {
                case -4: // Fatal
                    UN_LOGF(LogFlecsCore, Fatal, "Flecs: %s",
                    	StringCast<TCHAR>(Message).Get());
                    break;
                case -3: // Error
                    UN_LOGF(LogFlecsCore, Error, "Flecs: %s", StringCast<TCHAR>(Message).Get());
                    break;
                case -2: // Warning
                    UN_LOGF(LogFlecsCore, Warning, "Flecs: %s", StringCast<TCHAR>(Message).Get());
                    break;
                default: // Info and Debug
                    UN_LOGF(LogFlecsCore, Log, "Flecs: %s", StringCast<TCHAR>(Message).Get());
                    break;
            }
#endif // UNLOG_ENABLED
        };
		
        ecs_os_set_api(&os_api);
	}

	FORCEINLINE_DEBUGGABLE void InitializeDefaultComponents() const
	{
		/* Opaque FString to flecs::string */
		World.component<FString>()
			.opaque(flecs::String);
	}

	FORCEINLINE_DEBUGGABLE void InitializeSystems()
	{
		ObtainComponentTypeStruct(FFlecsScriptStructComponent::StaticStruct());
		
		CreateObserver<flecs::_::type_impl_struct_event_info>(TEXT("StructComponentObserver"))
				.with_symbol_component()
				.cached()
				.event(flecs::OnSet)
				.yield_existing()
				.each([&](flecs::entity InEntity, flecs::_::type_impl_struct_event_info InEventInfo)
				{
					FFlecsEntityHandle EntityHandle = InEntity;
					
					solid_checkf(InEventInfo.scriptStruct, TEXT("Script struct is nullptr"));
					solid_checkf(EntityHandle.IsComponent(), TEXT("Entity is not a component"));

					if UNLIKELY_IF(InEventInfo.scriptStruct == FFlecsScriptStructComponent::StaticStruct())
					{
						return;
					}

					if (FFlecsComponentPropertiesRegistry::Get().ContainsComponentProperties(
						std::string(InEntity.symbol())))
					{
						const FFlecsComponentProperties* Properties = FFlecsComponentPropertiesRegistry::Get().
							GetComponentProperties(std::string(InEntity.symbol()));
					
						for (const flecs::entity_t& Entity : Properties->Entities)
						{
							EntityHandle.Add(Entity);
						}

						for (const FInstancedStruct& InstancedStruct : Properties->ComponentPropertyStructs)
						{
							EntityHandle.Set(InstancedStruct);
						}

						UN_LOGF(LogFlecsWorld, Log,
							"Component properties %s found with %d entities and %d component properties",
							*EntityHandle.GetSymbol(), Properties->Entities.size(), Properties->ComponentPropertyStructs.Num());
					}
					#if WITH_EDITOR
					else
					{
						UN_LOGF(LogFlecsWorld, Log,
							"Component properties %s not found", *EntityHandle.GetSymbol());
					}
					#endif // WITH_EDITOR

					if UNLIKELY_IF(EntityHandle.Has<FFlecsScriptStructComponent>())
					{
						UN_LOGF(LogFlecsWorld, Warning, "Struct component %s already registered",
							*EntityHandle.GetSymbol());
						EntityHandle.Remove<flecs::_::type_impl_struct_event_info>();
						return;
					}

					EntityHandle.Set<FFlecsScriptStructComponent>({ InEventInfo.scriptStruct });

					UN_LOGF(LogFlecsWorld, Log,
						"Struct component %s registered", *EntityHandle.GetSymbol());

					#if WITH_EDITOR

					RegisterMemberProperties(InEventInfo.scriptStruct, EntityHandle);
					
					#endif // WITH_EDITOR

					EntityHandle.Remove<flecs::_::type_impl_struct_event_info>();
				});

		ObjectComponentQuery = World.query_builder<FFlecsUObjectComponent>("UObjectComponentQuery")
			.cached()
			.build();

		FCoreUObjectDelegates::GarbageCollectComplete.AddWeakLambda(this, [this]
		{
			ObjectComponentQuery.each([this](flecs::iter& Iter, size_t Index, const FFlecsUObjectComponent& InComponent)
			{
				if (!InComponent.IsValid())
				{
					Iter.entity(Index).destruct();
				}
			});
		});

		ModuleComponentQuery = World.query_builder<FFlecsModuleComponent>("ModuleComponentQuery")
			.cached()
			.build();

		CreateObserver<const FFlecsUObjectComponent&>(TEXT("AddModuleComponentObserver"))
			.cached()
			.with<FFlecsModuleComponent>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity InEntity,
				const FFlecsUObjectComponent& InUObjectComponent)
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

		CreateObserver(TEXT("RemoveModuleComponentObserver"))
			.cached()
			.with<FFlecsModuleComponent>().event(flecs::OnRemove)
			.each([&](flecs::iter& Iter, const size_t IterIndex)
			{
				for (int32 Index = ProgressModules.Num(); Index > 0; --Index)
				{
					const UObject* Module = ProgressModules[Index - 1].Get();
					
					if UNLIKELY_IF(Module == nullptr)
					{
						ProgressModules.RemoveAt(Index - 1);
					}
					
					if (Module
						== Iter.entity(IterIndex).get<FFlecsUObjectComponent>()->GetObjectChecked())
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
					}
				}
			});
	}

	FORCEINLINE_DEBUGGABLE void InitializeAssetRegistry()
	{
		const FAssetRegistryModule& AssetRegistryModule
			= FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
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

	FORCEINLINE_DEBUGGABLE void RegisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
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

	FORCEINLINE_DEBUGGABLE void UnregisterFlecsAsset(UFlecsPrimaryDataAsset* InAsset)
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
	
	FORCEINLINE_DEBUGGABLE void SetWorld(flecs::world&& InWorld)
	{
		World = std::move(InWorld);
	}

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

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntity(const flecs::entity_t InEntity) const
	{
		return World.make_alive(InEntity);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntity(const FString& Name = "") const
	{
		if (Name.IsEmpty())
		{
			World.entity();
		}

		return World.entity(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithId(const FFlecsId& InId) const
	{
		return World.entity(InId.GetFlecsId());
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithId(const flecs::entity_t InId) const
	{
		return FFlecsEntityHandle(World.make_alive(InId));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		return World.entity(InPrefab.GetEntity());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithRecord(const FFlecsEntityRecord& InRecord) const
	{
		FFlecsEntityHandle Entity = CreateEntity(InRecord.Name);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
		const flecs::entity_t InId) const
	{
		FFlecsEntityHandle Entity = CreateEntityWithId(InId);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle LookupEntity(const FString& Name) const
	{
		return World.lookup(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void DestroyEntityByName(const FString& Name, const bool bSearchPath = true) const
	{
		World.delete_with(reinterpret_cast<flecs::entity_t>(StringCast<char>(*Name).Get()), bSearchPath);
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
	FORCEINLINE_DEBUGGABLE void ForEach(const FFlecsId& InTermId, FunctionType&& Function) const
	{
		World.each(InTermId.GetFlecsId(), std::forward<FunctionType>(Function));
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

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE void SetSingleton(const First& InFirst, const Second InSecond) const
	{
		World.set<First, Second>(InSecond, InFirst);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE_DEBUGGABLE void SetSingleton(const First& InFirst, const TArgs&... Args) const
	{
		World.set(std::forward<const TArgs&>(Args)..., InFirst);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton() const
	{
		World.remove<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton() const
	{
		World.remove<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton(const Second& InSecond) const
	{
		World.remove<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton(const FFlecsId& InSecond) const
	{
		World.remove<First>(InSecond);
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSingleton() const
	{
		return World.has<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSingleton(const Second& InSecond) const
	{
		return World.has<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSingleton(const FFlecsId& InSecond) const
	{
		return World.has<First>(InSecond);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T GetSingleton() const
	{
		return *World.get<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE_DEBUGGABLE NO_DISCARD First GetSingleton(const Second& InSecond) const
	{
		return *World.get<First, Second>(InSecond);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T* GetSingletonPtr()
	{
		return World.get_mut<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD const T* GetSingletonPtr() const
	{
		return World.get<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T& GetSingletonRef()
	{
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD const T& GetSingletonRef() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %s not found"), *T::StaticStruct()->GetName());
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::ref<T> GetSingletonFlecsRef() const
	{
		return World.get_ref<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetSingletonEntity() const
	{
		return World.entity<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void ModifiedSingleton() const
	{
		World.modified<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void Merge() const
	{
		World.merge();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FString GetWorldName() const
	{
		return GetSingleton<FString>();
	}
	
	FORCEINLINE_DEBUGGABLE void SetWorldName(const FString& InName) const
	{
		SetSingleton<FString>(InName);

		#if WITH_EDITOR

		const char* Name = StringCast<char>(*InName).Get();

		GetWorldEntity().SetDocName(Name);
		
		World.entity<FString>().set_doc_name(Name);

		#endif // WITH_EDITOR
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void ImportModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value, "T must not be derived from IFlecsModuleInterface");
		World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void ImportModule(const TScriptInterface<IFlecsModuleInterface> InModule) const
	{
		solid_checkf(InModule != nullptr, TEXT("Module is nullptr"));
		InModule->ImportModule(World);
	}
	
	FORCEINLINE_DEBUGGABLE void ImportModule(UObject* InModule) const
	{
		solid_checkf(InModule->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
			TEXT("Module %s does not implement UFlecsModuleInterface"), *InModule->GetName());
		
		CastChecked<IFlecsModuleInterface>(InModule)->ImportModule(World);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsModuleImported(const TSubclassOf<UObject> InModule) const
	{
		const flecs::entity ModuleEntity = ModuleComponentQuery
			.find([&](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
			{
				return InComponent.ModuleClass == InModule;
			});

		return ModuleEntity.is_valid();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsModuleImported() const
	{
		return IsModuleImported(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetModuleEntity(const TSubclassOf<UObject> InModule) const
	{
		const flecs::entity ModuleEntity = ModuleComponentQuery
			.find([&](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
			{
				return InComponent.ModuleClass == InModule;
			});

		return ModuleEntity;
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetModuleEntity() const
	{
		return GetModuleEntity(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE UObject* GetModule(const TSubclassOf<UObject> InModule) const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModule);
		return ModuleEntity.Get<FFlecsUObjectComponent>().GetObjectChecked();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T* GetModule() const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity<T>();
		return ModuleEntity.Get<FFlecsUObjectComponent>().GetObjectChecked<T>();
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

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsDeferred() const
	{
		return World.is_deferred();
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

	FORCEINLINE_DEBUGGABLE void SetContext(void* InContext) const
	{
		World.set_ctx(InContext);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool Progress(const double DeltaTime = 0.0)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgress);

		{
			SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgressModule);
			
			for (TWeakObjectPtr<UObject> Module : ProgressModules)
			{
				solid_checkf(Module.IsValid(), TEXT("Module is nullptr"));

				UN_LOGF(LogFlecsWorld, Log, "Progressing module %s", *Module->GetName());
			
				IFlecsModuleProgressInterface* ModuleProgress = Cast<IFlecsModuleProgressInterface>(Module.Get());
				
				ModuleProgress->ProgressModule(World.delta_time());
			}
		}
		
		return World.progress(DeltaTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetTimeScale(const double InTimeScale) const
	{
		World.set_time_scale(InTimeScale);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetTargetFps(const double InTargetFps) const
	{
		World.set_target_fps(InTargetFps);
	}
	
	FORCEINLINE_DEBUGGABLE void DestroyWorld()
	{
		if UNLIKELY_IF(ShouldQuit())
		{
			return;
		}
		
		const FAssetRegistryModule& AssetRegistryModule
			= FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnAssetAdded().RemoveAll(this);

		World.release();
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
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetAlive(const FFlecsId& InId) const
	{
		return World.get_alive(InId.GetFlecsId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World.make_alive(InId.GetFlecsId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsSystem CreateSystem(const FString& InName) const
	{
		return CreateSystem<>(InName);
	}

	template <typename ...TComponents>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsSystem CreateSystem(const FString& InName) const
	{
		const FFlecsEntityHandle SystemEntity = CreateEntity(InName);
		const FFlecsSystem System = World.system(SystemEntity.GetEntity());
		System.ToBuilder<TComponents...>();
		return System;
	}

	template <typename ...TComponents>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::system_builder<TComponents...> CreateSystemWithBuilder(const FString& InName) const
	{
		return World.system<TComponents...>(StringCast<char>(*InName).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetWorldEntity() const
	{
		return World.entity(EcsWorld);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE int32 GetStageCount() const
	{
		return World.get_stage_count();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE int32 GetStageId() const
	{
		return World.get_stage_id();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsStage() const
	{
		return World.is_stage();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool IsReadOnly() const
	{
		return World.is_readonly();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void PreallocateEntities(const int32 InEntityCount) const
	{
		World.dim(InEntityCount);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetEntityRange(const int32 InMin, const int32 InMax) const
	{
		World.set_entity_range(InMin, InMax);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void EnforceEntityRange(const bool bInEnforce) const
	{
		World.enable_range_check(bInEnforce);
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void ForEachChild(FunctionType&& Function) const
	{
		World.children(std::forward<FunctionType>(Function));
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
	FORCEINLINE_DEBUGGABLE bool HasScriptStruct(UScriptStruct* ScriptStruct) const
	{
		return GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptStructEntity(UScriptStruct* ScriptStruct) const
	{
		return GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.at(ScriptStruct);
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(T::StaticStruct());
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(T::StaticStruct());
	}

	#if WITH_EDITOR

	FORCEINLINE_DEBUGGABLE void RegisterMemberProperties(const UScriptStruct* ScriptStruct,
		const FFlecsEntityHandle& InEntity) const
	{
		flecs::untyped_component UntypedComponent = InEntity.GetUntypedComponent_Unsafe();
		
		for (TFieldIterator<FProperty> PropertyIt(ScriptStruct); PropertyIt; ++PropertyIt)
		{
			FProperty* Property = *PropertyIt;
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
				FFlecsEntityHandle StructComponent
					= ObtainComponentTypeStruct(CastFieldChecked<FStructProperty>(Property)->Struct);
				UntypedComponent.member(StructComponent, StringCast<char>(*Property->GetName()).Get());
			}
		}
	}

	#endif // WITH_EDITOR
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct) const
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

		RegisterMemberProperties(ScriptStruct, ScriptStructComponent);

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

	FORCEINLINE_DEBUGGABLE void RegisterScriptStruct(const UScriptStruct* ScriptStruct, FFlecsEntityHandle InComponentEntity) const
	{
		GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap
			.emplace(const_cast<UScriptStruct*>(ScriptStruct), InComponentEntity);

		#if WITH_EDITOR

		RegisterMemberProperties(ScriptStruct, InComponentEntity);

		#endif // WITH_EDITOR
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ObtainComponentTypeStruct(UScriptStruct* ScriptStruct) const
	{
		if (HasScriptStruct(ScriptStruct))
		{
			return GetScriptStructEntity(ScriptStruct);
		}

		return RegisterScriptStruct(ScriptStruct);
	}
	
	template <typename ...TComponents>
	FORCEINLINE_DEBUGGABLE flecs::observer_builder<TComponents...> CreateObserver(const FString& Name) const
	{
		return World.observer<TComponents...>(StringCast<char>(*Name).Get());
	}

	FORCEINLINE_DEBUGGABLE flecs::observer_builder<> CreateObserver(const FString& Name) const
	{
		return World.observer<>(StringCast<char>(*Name).Get());
	}

	template <typename ...TComponents, typename ...TArgs>
	FORCEINLINE_DEBUGGABLE flecs::observer_builder<TComponents...> CreateObserver(const FFlecsEntityHandle& InEntity, TArgs&&... Args) const
	{
		return World.observer<TComponents...>(InEntity.GetEntity(), std::forward<TArgs>(Args)...);
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateObserver(const FFlecsEntityHandle& InEntity) const
	{
		return World.observer(InEntity.GetEntity());
	}

	FORCEINLINE_DEBUGGABLE flecs::event_builder Event(const FFlecsEntityHandle& InEntity) const
	{
		return World.event(InEntity.GetEntity());
	}

	template <typename TEvent>
	FORCEINLINE_DEBUGGABLE flecs::event_builder Event() const
	{
		return World.event<TEvent>();
	}

	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::pipeline_builder<> CreatePipeline() const
	{
		return World.pipeline();
	}

	template <typename ...TComponents>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::pipeline_builder<TComponents...> CreatePipeline() const
	{
		return World.pipeline<TComponents...>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void RunPipeline(const FFlecsEntityHandle& InPipeline, const double DeltaTime = 0.0) const
	{
		World.run_pipeline(InPipeline.GetEntity(), DeltaTime);
	}

	FORCEINLINE_DEBUGGABLE void RandomizeTimers() const
	{
		World.randomize_timers();
	}

	template <typename ...TArgs>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::timer CreateTimer(const TArgs&... Args) const
	{
		return World.timer(Args...);
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::timer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool HasEntityWithName(const FString& Name) const
	{
		return World.lookup(StringCast<char>(*Name).Get()).is_valid();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const
	{
		solid_checkf(Tag.IsValid(), TEXT("Tag is not valid"));
		return World.lookup(StringCast<char>(*Tag.GetTagName().ToString()).Get(), ".", ".");
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void EnableType() const
	{
		World.component<T>().enable();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void EnableType(UScriptStruct* ScriptStruct) const
	{
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
		ObtainComponentTypeStruct(ScriptStruct).Disable();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool IsTypeEnabled() const
	{
		return World.component<T>().enabled();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool IsTypeEnabled(UScriptStruct* ScriptStruct) const
	{
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
		ObtainComponentTypeStruct(ScriptStruct).Toggle();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE UFlecsWorld* GetParentWorld() const
	{
		solid_checkf(IsStage(), TEXT("World is not a stage"));
		return GetTypedOuter<UFlecsWorld>(); 
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab(const FFlecsEntityRecord& InRecord) const
	{
		const FFlecsEntityHandle Prefab = World.prefab(StringCast<char>(*InRecord.Name).Get());
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
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle FindPrefabEntity(UFlecsPrefabAsset* InPrefabAsset) const
	{
		solid_checkf(IsValid(InPrefabAsset), TEXT("Prefab asset is nullptr"));
		return World.lookup(StringCast<char>(*InPrefabAsset->GetPrimaryAssetId().ToString()).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE void DestroyPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		World.delete_with(InPrefab.GetEntity(), true);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool ShouldQuit() const
	{
		return World.should_quit();
	}
	
	flecs::world World;

	UPROPERTY()
	TArray<TWeakObjectPtr<UObject>> ProgressModules;

	flecs::query<FFlecsModuleComponent> ModuleComponentQuery;

	flecs::query<FFlecsUObjectComponent> ObjectComponentQuery;
	
}; // class UFlecsWorld
