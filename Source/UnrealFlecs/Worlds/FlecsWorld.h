// Solstice Games © 2024. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "flecs.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsGameplayTagEntityComponent.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsTypeMapComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Components/FlecsWorldNameComponent.h"
#include "Entities/FlecsEntityRecord.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsDependenciesComponent.h"
#include "Modules/FlecsModuleInitEvent.h"
#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleProgressInterface.h"
#include "Prefabs/FlecsPrefabAsset.h"
#include "FlecsWorld.generated.h"

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"), STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);
DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress::ProgressModule"),
	STAT_FlecsWorldProgressModule, STATGROUP_FlecsWorld);


struct FFlecsThreadWrapper
{
	std::thread Thread;
	std::atomic<bool> bStopped{false};
	std::atomic<bool> bJoined{false};
    
	FFlecsThreadWrapper(ecs_os_thread_callback_t Callback, void* Data)
	{
		Thread = std::thread([this, Callback, Data]()
		{
			while (!bStopped)
			{
				Callback(Data);
				break;
			}
		});
	}
    
	~FFlecsThreadWrapper()
	{
		if (!bJoined && Thread.joinable())
		{
			Stop();
			Thread.join();
		}
	}
    
	void Stop()
	{
		bStopped = true;
	}
    
	void Join()
	{
		if (!bJoined && Thread.joinable())
		{
			bStopped = true;
			Thread.join();
			bJoined = true;
		}
	}
	
}; // struct FFlecsThreadWrapper

struct FFlecsTask
{
	static constexpr ENamedThreads::Type TaskThread = ENamedThreads::AnyHiPriThreadNormalTask;
	
	FGraphEventRef TaskEvent;

	FORCEINLINE FFlecsTask(const ecs_os_thread_callback_t Callback, void* Data)
	{
		TaskEvent = FFunctionGraphTask::CreateAndDispatchWhenReady(
			[Callback, Data]()
			{
				Callback(Data);
			},
			TStatId(), nullptr, TaskThread);
	}

	FORCEINLINE ~FFlecsTask()
	{
		if (TaskEvent.IsValid())
		{
			TaskEvent->Wait();
		}
	}

	FORCEINLINE void Wait() const
	{
		if (TaskEvent.IsValid())
		{
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(TaskEvent);
		}
	}
	
}; // struct FFlecsTask

struct FFlecsConditionWrapper
{
	std::condition_variable ConditionalVariable;
	std::mutex* Mutex;
}; // struct ConditionWrapper

struct FOSApiInitializer
{
	
	FOSApiInitializer()
	{
		UN_LOG(LogFlecsCore, Log, "Initializing Flecs OS API");
		InitializeOSAPI();
	}
	
	void InitializeOSAPI()
	{
        ecs_os_set_api_defaults();
		
        ecs_os_api_t os_api = ecs_os_api;

	    os_api.mutex_new_ = []() -> ecs_os_mutex_t
	    {
	    	std::mutex* Mutex = new std::mutex();
	        return reinterpret_cast<ecs_os_mutex_t>(Mutex);
	    };

		os_api.mutex_free_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			delete MutexPtr;
		};

		os_api.mutex_lock_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			MutexPtr->lock();
		};

		os_api.mutex_unlock_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			MutexPtr->unlock();
		};

		os_api.cond_new_ = []() -> ecs_os_cond_t
		{
			FFlecsConditionWrapper* Wrapper = new FFlecsConditionWrapper();
			Wrapper->Mutex = new std::mutex();
			return reinterpret_cast<ecs_os_cond_t>(Wrapper);
		};

		os_api.cond_free_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				delete Wrapper->Mutex;
				delete Wrapper;
			}
		};

		os_api.cond_signal_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				Wrapper->ConditionalVariable.notify_one();
			}
		};

		os_api.cond_broadcast_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				Wrapper->ConditionalVariable.notify_all();
			}
		};

		os_api.cond_wait_ = [](ecs_os_cond_t Cond, ecs_os_mutex_t Mutex)
		{
			if (Cond && Mutex)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				std::mutex* std_mutex = reinterpret_cast<std::mutex*>(Mutex);
				std::unique_lock<std::mutex> Lock(*std_mutex, std::adopt_lock);
				Wrapper->ConditionalVariable.wait(Lock);
				Lock.release(); // Don't unlock on destruction since Flecs manages the lock
			}
		};

		os_api.thread_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsThreadWrapper* ThreadWrapper = new FFlecsThreadWrapper(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(ThreadWrapper);
		};

		os_api.thread_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			if (FFlecsThreadWrapper* ThreadWrapper = reinterpret_cast<FFlecsThreadWrapper*>(Thread))
			{
				ThreadWrapper->Join();
				delete ThreadWrapper;
			}
			
			return nullptr;
		};

		os_api.thread_self_ = []() -> ecs_os_thread_t
		{
			return FPlatformTLS::GetCurrentThreadId();
		};

		os_api.task_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsTask* FlecsTask = new FFlecsTask(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(FlecsTask);
		};

		os_api.task_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			const FFlecsTask* FlecsTask = reinterpret_cast<FFlecsTask*>(Thread);
			solid_check(FlecsTask);
			
			if (FlecsTask)
			{
				FlecsTask->Wait();
				delete FlecsTask;
			}
			
			return nullptr;
		};
		
        os_api.sleep_ = [](int32_t Seconds, int32_t Nanoseconds)
        {
            const double TotalSeconds = Seconds + Nanoseconds / 1e9;
            FPlatformProcess::SleepNoStats(static_cast<float>(TotalSeconds));
        };
		
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
                    UN_LOGF(LogFlecsCore, Fatal, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line,
						StringCast<TCHAR>(Message).Get());
                    break;
                case -3: // Error
                	UN_LOGF(LogFlecsCore, Warning, "Error Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
                case -2: // Warning
                	UN_LOGF(LogFlecsCore, Warning, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
            	case 0: // Verbose
                	UN_LOGF(LogFlecsCore, Verbose, "Flecs - File: %s, Line: %d, Message: %s",
                		StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
                default: // Info and Debug
                	UN_LOGF(LogFlecsCore, Log, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
            }
#endif // UNLOG_ENABLED
        };

		os_api.perf_trace_push_ = [](const char* FileName, size_t Line, const char* Name)
		{
			FCpuProfilerTrace::OutputBeginDynamicEvent(Name, FileName, Line);
		};

		os_api.perf_trace_pop_ = [](const char* FileName, size_t Line, const char* Name)
		{
			FCpuProfilerTrace::OutputEndEvent();
		};

		os_api.adec_ = [](int32_t* Value) -> int32
		{
			return FPlatformAtomics::InterlockedDecrement(Value);
		};

		os_api.ainc_ = [](int32_t* Value) -> int32
		{
			return FPlatformAtomics::InterlockedIncrement(Value);
		};

		os_api.lainc_ = [](int64_t* Value) -> int64
		{
			return FPlatformAtomics::InterlockedIncrement(Value);
		};

		os_api.ladec_ = [](int64_t* Value) -> int64
		{
			return FPlatformAtomics::InterlockedDecrement(Value);
		};

		os_api.malloc_ = [](int Size) -> void*
		{
			return FMemory::Malloc(Size);
		};

		os_api.realloc_ = [](void* Ptr, int Size) -> void*
		{
			return FMemory::Realloc(Ptr, Size);
		};

		os_api.calloc_ = [](int Size) -> void*
		{
			return FMemory::MallocZeroed(Size);
		};

		os_api.free_ = [](void* Ptr)
		{
			FMemory::Free(Ptr);
		};
		
        ecs_os_set_api(&os_api);
	}
	
}; // struct FOSApiInitializer

static FOSApiInitializer OSApiInitializer;

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	UFlecsWorld()
		: TypeMapComponent(nullptr)
	{
		World = flecs::world();
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
	}

	void WorldBeginPlay()
	{
		UN_LOG(LogFlecsWorld, Log, "Flecs World begin play");
		
		InitializeSystems();
		InitializeAssetRegistry();
	}

	void InitializeDefaultComponents() const
	{
		World.component<FString>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FString* Data)
			{
				auto* String = Data->GetCharArray().GetData();
				return Serializer->value(flecs::String, String);
			})
			.assign_string([](FString* Data, const char* String)
			{
				*Data = String;
			});

		World.component<FName>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FName* Data)
			{
				const auto* String = Data->ToString().GetCharArray().GetData();
				return Serializer->value(flecs::String, String);
			})
			.assign_string([](FName* Data, const char* String)
			{
				*Data = FName(String);
			});

		World.component<FText>()
			.opaque(flecs::String)
			.serialize([](const flecs::serializer* Serializer, const FText* Data)
			{
				auto* String = Data->ToString().GetCharArray().GetData();
				return Serializer->value(flecs::String, String);
			})
			.assign_string([](FText* Data, const char* String)
			{
				*Data = FText::FromString(String);
			});

		World.component<FFlecsGameplayTagEntityComponent>()
			.opaque(flecs::Entity)
			.serialize([](const flecs::serializer* Serializer, const FFlecsGameplayTagEntityComponent* Data)
			{
				const flecs::entity_t TagEntity = ecs_lookup(Serializer->world,
					StringCast<char>(*Data->Tag.ToString()).Get());
				return Serializer->value(flecs::Entity, &TagEntity);
			});

		World.component<FFlecsEntityHandle>()
            .opaque(flecs::Entity)
            .serialize([](const flecs::serializer* Serializer, const FFlecsEntityHandle* Data)
            {
                const flecs::entity_t Entity = Data->GetEntity();
                return Serializer->value(flecs::Entity, &Entity);
            })
            .assign_entity([](FFlecsEntityHandle* Handle, ecs_world_t* World, ecs_entity_t Entity)
            {
                *Handle = FFlecsEntityHandle(World, Entity);
            });
	}

	void InitializeSystems()
	{
		ObtainComponentTypeStruct(FFlecsScriptStructComponent::StaticStruct());
		
		CreateObserver<flecs::_::type_impl_struct_event_info>(TEXT("StructComponentObserver"))
				.with_symbol_component()
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
							*EntityHandle.GetSymbol(), Properties->Entities.size(),
							Properties->ComponentPropertyStructs.Num());
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

					//#if WITH_EDITOR

					RegisterMemberProperties(InEventInfo.scriptStruct, EntityHandle);
					
					//#endif // WITH_EDITOR

					EntityHandle.Remove<flecs::_::type_impl_struct_event_info>();
				});

		ObjectDestructionComponentQuery = World.query_builder<FFlecsUObjectComponent>("UObjectDestructionComponentQuery")
			.without<FFlecsUObjectComponent>(DontDeleteUObjectEntity)
			.begin_scope_traits<FFlecsUObjectComponent>().optional()
				.without(DontDeleteUObjectEntity)
			.end_scope_traits()
			.cached()
			.build();

		FCoreUObjectDelegates::GarbageCollectComplete.AddWeakLambda(this, [&]
		{
			ObjectDestructionComponentQuery.each([&](
				flecs::entity InEntity, FFlecsUObjectComponent& InUObjectComponent)
			{
				if (InUObjectComponent.IsStale(true, true))
				{
					UN_LOGF(LogFlecsWorld, Log, "Garbage  collected %s",
						StringCast<TCHAR>(InEntity.name().c_str()).Get());
					InEntity.destruct();
				}
			});
		});

		ModuleComponentQuery = World.query_builder<FFlecsModuleComponent>("ModuleComponentQuery")
			.cached()
			.build();

		DependenciesComponentQuery = World.query_builder<FFlecsDependenciesComponent>("DependenciesComponentQuery")
			.build();

		CreateObserver<const FFlecsUObjectComponent&,
			const FFlecsModuleComponent&>(TEXT("AddModuleComponentObserver"))
			.term_at(0).filter()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&](flecs::entity InEntity,
				const FFlecsUObjectComponent& InUObjectComponent, const FFlecsModuleComponent& InModuleComponent)
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
			.each([&](flecs::iter& Iter, const size_t IterIndex,
				const FFlecsModuleComponent& InModuleComponent, const FFlecsUObjectComponent& InUObjectComponent)
			{
				FFlecsEntityHandle ModuleEntity = Iter.entity(IterIndex);
				
				DependenciesComponentQuery.each([&](flecs::entity InEntity,
					const FFlecsDependenciesComponent& InDependenciesComponent)
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
			.each([&](flecs::iter& Iter, const size_t IterIndex)
			{
				for (int32 Index = ProgressModules.Num(); Index > 0; --Index)
				{
					const UObject* Module = ProgressModules[Index - 1].Get();
					
					if UNLIKELY_IF(!IsValid(Module))
					{
						ProgressModules.RemoveAt(Index - 1);
						break;
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

		AssetRegistry.OnAssetAdded().AddWeakLambda(this, [&](const FAssetData& InAssetData)
		{
			if UNLIKELY_IF(!IsValid(this))
			{
				UN_LOG(LogFlecsWorld, Error, "Flecs World is not valid");
				AssetRegistry.OnAssetAdded().RemoveAll(this);
				return;
			}

			if (InAssetData.IsInstanceOf<UFlecsPrimaryDataAsset>())
			{
				RegisterFlecsAsset(CastChecked<UFlecsPrimaryDataAsset>(InAssetData.GetAsset()));
			}
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

	/**
	 * @brief Asynchronously Register a module dependency,
	 * if the dependency is already imported, the function is called immediately
	 * if the dependency is not imported, the function is called when the dependency is imported (if it is)
	 * @param InModuleObject The module object
	 * @param InFunction The function to call when the dependency is imported
	 * @tparam TModule The module class
	 */
	template <Solid::TStaticClassConcept TModule, typename TFunction>
	FORCEINLINE_DEBUGGABLE void RegisterModuleDependency(UObject* InModuleObject, TFunction&& InFunction)
	{
		RegisterModuleDependency(
			InModuleObject, TModule::StaticClass(), [InFunction](UObject* InDependencyObject, UFlecsWorld* InWorld,
			FFlecsEntityHandle InDependencyEntity) FORCEINLINE_ATTRIBUTE
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
		return World.entity(InEntity);
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
		return flecs::entity(World, InId.GetFlecsId());
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithId(const flecs::entity_t InId) const
	{
		return World.make_alive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
	{
		return World.entity().is_a(InPrefab.GetEntity());
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
		const FFlecsEntityHandle Entity = CreateEntityWithId(InId);
		InRecord.ApplyRecordToEntity(Entity);
		return Entity;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World",
		meta = (AdvancedDisplay = "Separator, RootSeparator, bRecursive"))
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle LookupEntity(const FString& Name,
		const FString& Separator = "::", const FString& RootSeparator = "::", const bool bRecursive = true) const
	{
		return World.lookup(StringCast<char>(*Name).Get(), StringCast<char>(*Separator).Get(),
			StringCast<char>(*RootSeparator).Get(), bRecursive);
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

	template <typename T>
	FORCEINLINE_DEBUGGABLE void RemoveSingleton() const
	{
		World.remove<T>();
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD T GetSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *World.get<T>();
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
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD const T& GetSingletonRef() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return *GetSingletonPtr<T>();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::ref<T> GetSingletonFlecsRef() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
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
		return GetSingleton<FFlecsWorldNameComponent>().WorldName;
	}
	
	FORCEINLINE_DEBUGGABLE void SetWorldName(const FString& InName) const
	{
		SetSingleton<FFlecsWorldNameComponent>({ InName });
		
		#if WITH_EDITOR

		GetSingletonEntity<FFlecsWorldNameComponent>().SetDocName(InName);

		#endif // WITH_EDITOR
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void ImportFlecsModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value,
			"T must not be derived from IFlecsModuleInterface, use ImportModule(Non-Template) instead");
		World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void ImportModule(const TScriptInterface<IFlecsModuleInterface> InModule)
	{
		solid_checkf(InModule != nullptr, TEXT("Module is nullptr"));
		InModule->ImportModule(World);
	}
	
	FORCEINLINE_DEBUGGABLE void ImportModule(UObject* InModule)
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
			
				IFlecsModuleProgressInterface* ModuleProgress = Cast<IFlecsModuleProgressInterface>(Module.Get());
				ModuleProgress->ProgressModule(DeltaTime);
			}
		}
		
		return World.progress(DeltaTime);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE void SetTimeScale(const double InTimeScale) const
	{
		World.set_time_scale(InTimeScale);
	}
	
	FORCEINLINE_DEBUGGABLE void DestroyWorld()
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

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetAlive(const flecs::entity_t InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetEntity(const FFlecsId& InId) const
	{
		return World.get_alive(InId.GetFlecsId());
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetEntity(const flecs::entity_t InId) const
	{
		return World.get_alive(InId);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World.make_alive(InId.GetFlecsId());
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

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void SetEntityRange(const int32 InMin, const int32 InMax, FunctionType&& Function) const
	{
		World.set_entity_range(InMin, InMax);
		EnforceEntityRange(true);
		Function();
		EnforceEntityRange(false);
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
		return TypeMapComponent->ScriptStructMap.contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptStructEntity(UScriptStruct* ScriptStruct) const
	{
		return TypeMapComponent->ScriptStructMap.at(ScriptStruct);
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(StaticStruct<T>());
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(StaticStruct<T>());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE bool HasScriptEnum(UEnum* ScriptEnum) const
	{
		return TypeMapComponent->ScriptEnumMap.contains(ScriptEnum);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptEnumEntity(UEnum* ScriptEnum) const
	{
		return TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
	}
	
	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD bool HasScriptEnum() const
	{
		return HasScriptEnum(StaticEnum<T>());
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD FFlecsEntityHandle GetScriptEnumEntity() const
	{
		return GetScriptEnumEntity(StaticEnum<T>());
	}

	//#if WITH_EDITOR

	FORCEINLINE_DEBUGGABLE void RegisterMemberProperties(const UStruct* InStruct,
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

	//#endif // WITH_EDITOR

	FORCEINLINE_DEBUGGABLE void RegisterEnumProperties(UEnum* Enum, FFlecsEntityHandle InComponentEntity) const
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
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptEnum(UEnum* Enum) const
	{
		solid_check(IsValid(Enum));

		FFlecsEntityHandle OldScope = ClearScope();

		if (const FFlecsEntityHandle Handle = LookupEntity(Enum->GetName()))
		{
			RegisterEnumProperties(Enum, Handle);
			SetScope(OldScope);
			return Handle;
		}
		
		FFlecsEntityHandle EnumComponent = World.component(StringCast<char>(*Enum->GetName()).Get())
			.set<flecs::Component>({ sizeof(int32), alignof(int32) })
			.set<FFlecsScriptEnumComponent>({ Enum });
		
		TypeMapComponent->ScriptEnumMap.emplace(Enum, EnumComponent);

		//#if WITH_EDITOR

		RegisterEnumProperties(Enum, EnumComponent);

		//#endif // WITH_EDITOR

		SetScope(OldScope);
		return EnumComponent;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct) const
	{
		solid_check(IsValid(ScriptStruct));

		FFlecsEntityHandle OldScope = ClearScope();

		if (const FFlecsEntityHandle Handle = LookupEntity(ScriptStruct->GetName()))
		{
			RegisterScriptStruct(ScriptStruct, Handle);
			SetScope(OldScope);
			return Handle;
		}
		
		FFlecsEntityHandle ScriptStructComponent = World.component(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get())
			.set<flecs::Component>({ ScriptStruct->GetStructureSize(), ScriptStruct->GetMinAlignment() })
			.set<FFlecsScriptStructComponent>({ ScriptStruct });
		
		TypeMapComponent->ScriptStructMap.emplace(ScriptStruct, ScriptStructComponent);

		//#if WITH_EDITOR

		RegisterMemberProperties(ScriptStruct, ScriptStructComponent);

		//#endif // WITH_EDITOR

		if (ScriptStruct->GetSuperStruct())
		{
			const FFlecsEntityHandle ParentEntity = ObtainComponentTypeStruct(
				static_cast<UScriptStruct*>(ScriptStruct->GetSuperStruct()));
			
			ScriptStructComponent.SetParent(ParentEntity);
		}

		SetScope(OldScope);
		return ScriptStructComponent;
	}

	FORCEINLINE_DEBUGGABLE void RegisterScriptStruct(UScriptStruct* ScriptStruct, FFlecsEntityHandle InComponentEntity) const
	{
		TypeMapComponent->ScriptStructMap
			.emplace(ScriptStruct, InComponentEntity);

		//#if WITH_EDITOR

		RegisterMemberProperties(ScriptStruct, InComponentEntity);

		//#endif // WITH_EDITOR
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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ObtainScriptEnumType(UEnum* Enum) const
	{
		if (HasScriptEnum(Enum))
		{
			return GetScriptEnumEntity(Enum);
		}

		return RegisterScriptEnum(Enum);
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
	FORCEINLINE_DEBUGGABLE flecs::observer_builder<TComponents...> CreateObserver(
		const FFlecsEntityHandle& InEntity, TArgs&&... Args) const
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
	FORCEINLINE_DEBUGGABLE flecs::event_builder_typed<TEvent> Event() const
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

	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::timer CreateTimer(FString Name) const
	{
		return World.timer(StringCast<char>(*Name).Get());
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE NO_DISCARD flecs::timer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool HasEntityWithName(const FString& Name) const
	{
		return LookupEntity(Name).IsAlive();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const
	{
		solid_checkf(Tag.IsValid(), TEXT("Tag is not valid"));
		return LookupEntity(StringCast<char>(*Tag.GetTagName().ToString()).Get(), ".", ".");
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
		
		#if WITH_EDITOR
		
		Prefab.SetDocName(InRecord.Name);

		#endif // WITH_EDITOR
		
		return Prefab;
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab(const FString& Name) const
	{
		return World.prefab(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle FindPrefabEntity(UFlecsPrefabAsset* InPrefabAsset) const
	{
		solid_checkf(IsValid(InPrefabAsset), TEXT("Prefab asset is nullptr"));
		return LookupEntity(InPrefabAsset->GetPrimaryAssetId().ToString());
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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ClearScope() const
	{
		return World.set_scope(0);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle SetScope(const FFlecsEntityHandle& InScope) const
	{
		return World.set_scope(InScope.GetEntity());
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE void Scope() const
	{
		World.scope<T>();
	}

	template <typename T, typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Scope(FunctionType&& Function) const
	{
		World.scope<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Scope(FFlecsId InId, FunctionType&& Function) const
	{
		World.scope(InId.GetFlecsId(), std::forward<FunctionType>(Function));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScope() const
	{
		return World.get_scope();
	}
	
	flecs::world World;

	UPROPERTY()
	TArray<TWeakObjectPtr<UObject>> ProgressModules;

	flecs::query<FFlecsModuleComponent> ModuleComponentQuery;
	flecs::query<FFlecsUObjectComponent> ObjectDestructionComponentQuery;
	flecs::query<FFlecsDependenciesComponent> DependenciesComponentQuery;

	FFlecsTypeMapComponent* TypeMapComponent;
	
}; // class UFlecsWorld
