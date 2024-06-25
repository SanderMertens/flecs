// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Components/FlecsTypeMapComponent.h"
#include "Entities/FlecsEntityRecord.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "Modules/FlecsModuleInterface.h"
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

	template <typename ...TArgs>
	FORCEINLINE FFlecsEntityHandle CreateEntity(const TArgs&... Args) const
	{
		return World.entity(Args...);
	}

	FORCEINLINE FFlecsEntityHandle CreateEntity(const flecs::entity_t InEntity) const
	{
		return World.make_alive(InEntity);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntity(const FString& Name = "None") const
	{
		return World.entity(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle CreateEntityWithId(const FFlecsId& InId) const
	{
		return World.entity(InId.GetFlecsId());
	}

	FORCEINLINE FFlecsEntityHandle CreateEntityWithId(const flecs::entity_t InId) const
	{
		return World.entity(InId);
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

	template <typename First, typename ...TArgs>
	FORCEINLINE void RemoveSingleton() const
	{
		World.remove<First, TArgs...>();
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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetName(const FString& InName) const
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

	/*UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle SetScope(const FFlecsEntityHandle& InScope) const
	{
		return World.set_scope(InScope);
	}

	template <typename T>
	FORCEINLINE FFlecsEntityHandle SetScope() const
	{
		return World.set_scope<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE FFlecsEntityHandle GetScope() const
	{
		return World.get_scope();
	}*/

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FORCEINLINE float GetDeltaTime() const
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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE void SetStageCount(const int32 InStageCount) const
	{
		World.set_stage_count(InStageCount);
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
	FORCEINLINE UFlecsWorld* GetStage(const int32 InStageId) const
	{
		UFlecsWorld* Stage = NewObject<UFlecsWorld>();
		Stage->SetWorld(World.get_stage(InStageId));
		return Stage;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FORCEINLINE UFlecsWorld* ObtainAsyncStage() const
	{
		UFlecsWorld* Stage = NewObject<UFlecsWorld>();
		Stage->SetWorld(World.async_stage());
		return Stage;
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
		
		const FFlecsEntityHandle ScriptStructComponent = World.entity(StringCast<char>(*ScriptStruct->GetStructCPPName()).Get())
			.set<flecs::Component>({ ScriptStruct->GetStructureSize(), ScriptStruct->GetMinAlignment() })
			.set<FFlecsScriptStructComponent>({ ScriptStruct });
		
		GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.emplace(ScriptStruct, ScriptStructComponent);

		#if WITH_EDITOR

		flecs::untyped_component* UntypedComponent
			= const_cast<flecs::untyped_component*>(ScriptStructComponent.GetUntypedComponent());
		
		for (TFieldIterator<FProperty> PropertyIt(ScriptStruct); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			solid_checkf(Property != nullptr, TEXT("Property is nullptr"));
			
			if (Property->IsA<FBoolProperty>())
			{
				UntypedComponent->member<bool>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FByteProperty>())
			{
				UntypedComponent->member<uint8>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FIntProperty>())
			{
				UntypedComponent->member<int32>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				UntypedComponent->member<float>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				UntypedComponent->member<double>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FStrProperty>())
			{
				UntypedComponent->member<FString>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FNameProperty>())
			{
				UntypedComponent->member<FName>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FTextProperty>())
			{
				UntypedComponent->member<FText>(StringCast<char>(*Property->GetName()).Get());
			}
		}

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

	FORCEINLINE void RegisterScriptStruct(const UScriptStruct* ScriptStruct, const FFlecsEntityHandle& InEntity) const
	{
		GetSingletonRef<FFlecsTypeMapComponent>().ScriptStructMap.emplace(const_cast<UScriptStruct*>(ScriptStruct), InEntity);

		#if WITH_EDITOR

		flecs::untyped_component* UntypedComponent
			= const_cast<flecs::untyped_component*>(InEntity.GetUntypedComponent());
		
		for (TFieldIterator<FProperty> PropertyIt(ScriptStruct); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			solid_checkf(Property != nullptr, TEXT("Property is nullptr"));
			
			if (Property->IsA<FBoolProperty>())
			{
				UntypedComponent->member<bool>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FByteProperty>())
			{
				UntypedComponent->member<uint8>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FIntProperty>())
			{
				UntypedComponent->member<int32>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				UntypedComponent->member<float>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				UntypedComponent->member<double>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FStrProperty>())
			{
				UntypedComponent->member<FString>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FNameProperty>())
			{
				UntypedComponent->member<FName>(StringCast<char>(*Property->GetName()).Get());
			}
			else if (Property->IsA<FTextProperty>())
			{
				UntypedComponent->member<FText>(StringCast<char>(*Property->GetName()).Get());
			}
		}

		#endif // WITH_EDITOR
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle RegisterComponentType(const FString& Name, const int32 Size, const int32 Alignment) const
	{
		const flecs::entity Component = World.entity(StringCast<char>(*Name).Get())
			.set<flecs::Component>({ Size, Alignment });

		return FFlecsEntityHandle(Component);
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

	template <typename T>
	FORCEINLINE FFlecsEntityHandle CreatePrefab(const FString& Name) const
	{
		return World.prefab<T>(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle CreatePrefab(const FString& Name) const
	{
		return World.prefab(StringCast<char>(*Name).Get());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle CreateEmptyPrefab() const
	{
		return World.prefab();
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
	
	flecs::world World;

	UPROPERTY()
	TArray<TObjectPtr<UFlecsWorld>> Stages;
	
}; // class UFlecsWorld
