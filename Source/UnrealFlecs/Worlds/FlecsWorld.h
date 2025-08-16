// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include <utility>

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsScopedDeferWindow.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Components/ObjectTypes/FFlecsActorComponentTag.h"
#include "Components/ObjectTypes/FFlecsModuleComponentTag.h"
#include "Entities/FlecsEntityRecord.h"
#include "Concepts/SolidConcepts.h"
#include "Entities/FlecsId.h"
#include "General/FlecsGameplayTagManagerEntity.h"
#include "General/FlecsObjectRegistrationInterface.h"
#include "Logs/FlecsCategories.h"
#include "Modules/FlecsDependenciesComponent.h"
#include "Modules/FlecsModuleInitEvent.h"
#include "Modules/FlecsModuleInterface.h"
#include "Pipelines/FlecsGameLoopInterface.h"
#include "FlecsWorld.generated.h"

class UFlecsWorldSubsystem;

/**
 * @brief Component type that represents if the World has begun play.
 * Can be found in the World entity.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsBeginPlaySingletonComponent
{
	GENERATED_BODY()
}; // struct FFlecsBeginPlaySingletonComponent

REGISTER_FLECS_COMPONENT(FFlecsBeginPlaySingletonComponent,
	[](flecs::world InWorld, FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Singleton);
	});


UCLASS(BlueprintType, NotBlueprintable)
class UNREALFLECS_API UFlecsWorld final : public UObject
{
	GENERATED_BODY()

public:
	UFlecsWorld(const FObjectInitializer& ObjectInitializer);
	
	virtual ~UFlecsWorld() override;

	void WorldStart();

	// ReSharper disable once CppMemberFunctionMayBeConst
	void WorldBeginPlay();

	void InitializeDefaultComponents() const;

	void InitializeFlecsRegistrationObjects();

	void RegisterUnrealTypes() const;

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
						
						const TSolidNotNull<const FFlecsComponentProperties*> Properties
							= FFlecsComponentPropertiesRegistry::Get()
							.GetComponentProperties(StructSymbol);

						std::invoke(Properties->RegistrationFunction, Iter.world(), InUntypedComponent);

						UE_LOGFMT(LogFlecsComponent, Log,
							"Component properties {StructName} registered", StructSymbol);
					}
					#if !NO_LOGGING
					else
					{
						UE_LOGFMT(LogFlecsComponent, Log,
							"Component properties {StructName} not found", StructSymbol);
					}
					#endif // UNLOG_ENABLED
				});
			});

		ObjectComponentQuery = World.query_builder<FFlecsUObjectComponent>("UObjectComponentQuery")
			.term_at(0).second(flecs::Wildcard) // FFlecsUObjectComponent
			.cached()
			.build();

		FCoreUObjectDelegates::GarbageCollectComplete.AddWeakLambda(this, [this]
		{
			ObjectComponentQuery.each([](flecs::entity InEntity, const FFlecsUObjectComponent& InUObjectComponent)
			{
				const FFlecsEntityHandle EntityHandle = InEntity;
					
				if (!InUObjectComponent.IsValid())
				{
					UE_CLOGFMT(EntityHandle.HasName(), LogFlecsWorld, Verbose,
						"Entity Garbage Collected: {EntityName}", EntityHandle.GetName());
					
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

		// @TODO: Remove this as ProgressModules have been replaced with a World Game Loop
		/*CreateObserver<const FFlecsUObjectComponent>("AddModuleComponentObserver")
			.term_at(0).second<FFlecsModuleComponentTag>().filter() // FFlecsUObjectComponent
			.with<FFlecsModuleComponent>().event(flecs::OnAdd)
			.yield_existing()
			.each([this](flecs::entity InEntity, const FFlecsUObjectComponent& InUObjectComponent)
			{
				const TSolidNotNull<UObject*> ObjectPtr = InUObjectComponent.GetObjectChecked();

				UE_LOGFMT(LogFlecsWorld, Log,
					"Module component {ModuleName} added", ObjectPtr->GetName());
				
				// if (ObjectPtr->Implements<UFlecsModuleProgressInterface>())
				// {
				// 	ProgressModules.Add(ObjectPtr);
				// 	UN_LOGF(LogFlecsWorld, Log, "Progress module %s added",
				// 		*InUObjectComponent.GetObjectChecked()->GetName());
				// }
			});*/

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
					TSolidNotNull<UObject*> ModuleObject = ImportedModules[Index].GetObject();
					solid_check(IsValid(ModuleObject));
					
					if (ModuleObject == InUObjectComponent.GetObjectChecked())
					{
						ImportedModules.RemoveAt(Index);
						break;
					}
				}
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
	void RegisterModuleDependency(
		const TSolidNotNull<const UObject*> InModuleObject,
		const TSubclassOf<UFlecsModuleInterface>& InDependencyClass,
		const std::function<void(TSolidNotNull<UObject*>, TSolidNotNull<UFlecsWorld*>, FFlecsEntityHandle)>& InFunction);

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void Reset();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ResetClock() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntity(const FString& Name = "") const;

	template <typename T>
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ObtainTypedEntity() const
	{
		const FFlecsEntityHandle EntityHandle = World.entity<T>();
		return EntityHandle;
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle ObtainTypedEntity(TSolidNotNull<UClass*> InClass) const
	{
		const FFlecsEntityHandle EntityHandle = World.entity(RegisterScriptClassType(InClass));
		return EntityHandle;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithId(const FFlecsId InId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntityWithRecord(const FFlecsEntityRecord& InRecord,
	                                          const FString& Name = "") const;

	FFlecsEntityHandle CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
	                                                const FFlecsId InId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World",
		meta = (AdvancedDisplay = "Separator, RootSeparator, bRecursive"))
	FFlecsEntityHandle LookupEntity(const FString& Name,
	                                const FString& Separator = "::",
	                                const FString& RootSeparator = "::",
	                                const bool bRecursive = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void DestroyEntityByName(const FString& Name) const;

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
	FORCEINLINE_DEBUGGABLE UFlecsWorld* AddSingleton() const
	{
		World.add<T>();
		return GetSelf();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE UFlecsWorld* SetSingleton(const T& Value) const
	{
		World.set<T>(Value);
		return GetSelf();
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE UFlecsWorld* RemoveSingleton() const
	{
		World.remove<T>();
		return GetSelf();
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
	void Merge() const;

	/**
	 * @return The Name of the World Entity
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FString GetWorldName() const;

	/**
	 * @brief Set the Name of the World Entity
	 * @param InName The new name
	 */
	void SetWorldName(const FString& InName) const;

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
	void ImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule);

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsModuleImported(const TSubclassOf<UObject> InModule) const;

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
	FFlecsEntityHandle GetModuleEntity(const TSubclassOf<UObject> InModule) const;

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
	UObject* GetModule(const TSubclassOf<UObject> InModule) const;

	template <Solid::TStaticClassConcept T>
	NO_DISCARD FORCEINLINE_DEBUGGABLE TSolidNotNull<T*> GetModule() const
	{
		const FFlecsEntityHandle ModuleEntity = GetModuleEntity<T>();
		return ModuleEntity.GetPairFirst<FFlecsUObjectComponent, FFlecsModuleComponentTag>().GetObjectChecked<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool BeginDefer() const;

	NO_DISCARD FFlecsScopedDeferWindow DeferWindow() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool EndDefer() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ResumeDefer() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SuspendDefer() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsDeferred() const;

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
	bool BeginReadOnly() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void EndReadOnly() const;

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

	void SetContext(void* InContext) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool ProgressGameLoop(const double DeltaTime = 0.0);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	bool Progress(const double DeltaTime = 0.0);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SetTimeScale(const double InTimeScale) const;

	void DestroyWorld();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SetPipeline(const FFlecsEntityHandle& InPipeline) const;

	template <typename T>
	FORCEINLINE_DEBUGGABLE void SetPipeline() const
	{
		World.set_pipeline<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetPipeline() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	double GetDeltaTime() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetAlive(const FFlecsId InId) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsAlive(const FFlecsId InId) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetEntity(const FFlecsId InId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const;

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
	FFlecsEntityHandle GetWorldEntity() const;

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
	int32 GetStageCount() const;

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
	bool IsReadOnly() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void PreallocateEntities(const int32 InEntityCount) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void SetEntityRange(const FFlecsId InMin, const FFlecsId InMax) const;

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
	void EnforceEntityRange(const bool bInEnforce) const;

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
	void SetThreads(const int32 InThreadCount) const;

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
	void SetTaskThreads(const int32 InThreadCount) const;

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
			TSolidNotNull<FProperty*> Property = *PropertyIt;

			const char* PropertyNameCStr = Unreal::Flecs::ToCString(Property->GetName());
				
			if (Property->IsA<FBoolProperty>())
			{
				InComponent.AddMember<bool>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FByteProperty>())
			{
				InComponent.AddMember<uint8>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FInt16Property>())
			{
				InComponent.AddMember<int16>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt16Property>())
			{
				InComponent.AddMember<uint16>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FIntProperty>())
			{
				InComponent.AddMember<int32>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt32Property>())
			{
				InComponent.AddMember<uint32>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FInt64Property>())
			{
				InComponent.AddMember<int64>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FUInt64Property>())
			{
				InComponent.AddMember<uint64>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FFloatProperty>())
			{
				InComponent.AddMember<float>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FDoubleProperty>())
			{
				InComponent.AddMember<double>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStrProperty>())
			{
				InComponent.AddMember<FString>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FNameProperty>())
			{
				InComponent.AddMember<FName>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FTextProperty>())
			{
				InComponent.AddMember<FText>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FObjectProperty>())
			{
				InComponent.AddMember<FObjectPtr>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());

				if UNLIKELY_IF(ecs_id_in_use(World.c_ptr(), InComponent))
				{
					continue;
				}
			}
			else if (Property->IsA<FWeakObjectProperty>())
			{
				InComponent.AddMember<FWeakObjectPtr>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FSoftObjectProperty>())
			{
				InComponent.AddMember<FSoftObjectPtr>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FClassProperty>())
			{
				InComponent.AddMember<TSubclassOf<UObject>>(PropertyNameCStr,
					0, Property->GetOffset_ForInternal());
			}
			else if (Property->IsA<FStructProperty>())
			{
				FFlecsEntityHandle StructComponent;
				if (!HasScriptStruct(CastFieldChecked<FStructProperty>(Property)->Struct))
				{
					UE_LOGFMT(LogFlecsWorld, Error,
						"Property Type Script struct {StructName} is not registered for entity {ComponentName}",
						CastFieldChecked<FStructProperty>(Property)->Struct->GetStructCPPName(),
						InComponent.GetName());
					continue;
				}
				else
				{
					StructComponent
						= GetScriptStructEntity(CastFieldChecked<FStructProperty>(Property)->Struct);
				}
			 		
				InComponent.AddMember(StructComponent, PropertyNameCStr,
					1, Property->GetOffset_ForInternal());
			}
			else UNLIKELY_ATTRIBUTE
			{
				UE_LOGFMT(LogFlecsWorld, Log,
					"Property Type: {PropertyName}({TypeName} is not supported for member serialization",
					Property->GetNameCPP(),
					Property->GetOwnerStruct()->GetName());
			}
		}
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptStruct(const UScriptStruct* ScriptStruct, const bool bComponent = true) const
	{
		solid_check(ScriptStruct);

		const FFlecsEntityHandle OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptStructMap.contains(ScriptStruct),
			TEXT("Script struct %s is already registered"), *ScriptStruct->GetStructCPPName());
		
		FFlecsComponentHandle ScriptStructComponent;

		const FString StructName = ScriptStruct->GetStructCPPName();
		const char* StructNameCStr = Unreal::Flecs::ToCString(StructName);
		std::string StructNameStd = std::string(StructNameCStr);

		DeferEndLambda([this, ScriptStruct, &ScriptStructComponent, StructNameCStr, &StructNameStd, bComponent]()
		{
			ScriptStructComponent = World.component(StructNameCStr);
			solid_check(ScriptStructComponent.IsValid());
			
			ScriptStructComponent.GetEntity().set_symbol(StructNameCStr);
			
			const TFieldIterator<FProperty> PropertyIt(ScriptStruct);
			const bool bIsTag = ScriptStruct->GetStructureSize() <= 1 && !PropertyIt;

			if (bComponent)
			{
				ScriptStructComponent.Set<flecs::Component>({
					.size = bIsTag ? 0 : ScriptStruct->GetStructureSize(),
					.alignment = bIsTag ? 0 : ScriptStruct->GetMinAlignment()
				});

				if (!bIsTag)
				{
					if (ScriptStruct->GetCppStructOps()->HasNoopConstructor())
					{
						UE_LOGFMT(LogFlecsComponent, Warning,
							"Script struct {StructName} has a No-op constructor, this will not be used in flecs",
							ScriptStruct->GetName());
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

		if (HasScriptEnum(ScriptEnum))
		{
			UE_LOGFMT(LogFlecsWorld, Log,
				"Script enum {EnumName} is already registered", ScriptEnum->GetName());
			return GetScriptEnumEntity(ScriptEnum);
		}

		solid_checkf(!ScriptEnum->HasAnyEnumFlags(EEnumFlags::Flags),
			TEXT("Script enum %s is not supported, use RegisterScriptBitmask instead"),
			*ScriptEnum->GetName());

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

	template <typename T>
	requires (std::is_enum<T>::value)
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptEnum() const
	{
		return World.component<T>();
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
			
			ScriptEnumComponent.GetLambda([](flecs::Enum& InEnumComponent)
			{
				InEnumComponent.underlying_type = flecs::U8;
			});

			const int32 EnumCount = ScriptEnum->NumEnums();

			const uint64 MaxEnumValue = ScriptEnum->GetMaxEnumValue();
			const bool bUint8 = MaxEnumValue < std::numeric_limits<uint8>::max();
			
			for (int32 EnumIndex = 0; EnumIndex < EnumCount; ++EnumIndex)
			{
				const int64 EnumValue = ScriptEnum->GetValueByIndex(EnumIndex);
				solid_check(EnumValue >= 0);
				
				const FString EnumValueName = ScriptEnum->GetNameStringByIndex(EnumIndex);

				if (std::cmp_equal(MaxEnumValue, EnumValue))
				{
					continue;
				}

				if (bUint8)
				{
					ScriptEnumComponent.AddConstant<uint8>(StringCast<char>(*EnumValueName).Get(),
						static_cast<uint8>(EnumValue));
				}
				else
				{
					ScriptEnumComponent.AddConstant<uint64>(StringCast<char>(*EnumValueName).Get(),
						static_cast<uint64>(EnumValue));
				}
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

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterScriptClassType(TSolidNotNull<UClass*> ScriptClass) const
	{
		const FFlecsEntityHandle OldScope = ClearScope();

		const FString ClassName = ScriptClass->GetPrefixCPP() + ScriptClass->GetName();

		if (HasScriptClass(ScriptClass))
		{
			UE_LOGFMT(LogFlecsWorld, Log,
				"Script class {ClassName} is already registered", ClassName);
			return GetScriptClassEntity(ScriptClass);
		}

		FFlecsEntityHandle ScriptClassEntity;
		
		const char* ClassNameCStr = Unreal::Flecs::ToCString(ClassName);

		DeferEndLambda([this, ScriptClass, &ScriptClassEntity, ClassNameCStr, &ClassName]()
		{
			ScriptClassEntity = CreateEntity(ClassName);
			solid_check(ScriptClassEntity.IsValid());

			ScriptClassEntity.GetEntity().set_symbol(ClassNameCStr);
			TypeMapComponent->ScriptClassMap.emplace(ScriptClass, ScriptClassEntity);

			ScriptClassEntity.Set<FFlecsScriptClassComponent>(FFlecsScriptClassComponent(ScriptClass));

			//RegisterMemberProperties(ScriptClass, ScriptClassComponent);

			if (!flecs::_::g_type_to_impl_data.contains(std::string(ClassNameCStr)))
			{
				flecs::_::type_impl_data NewData;  // NOLINT(cppcoreguidelines-pro-type-member-init)
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = 0;
				NewData.s_alignment = 0;
				NewData.s_allow_tag = true;
				NewData.s_enum_registered = false;
				
				flecs::_::g_type_to_impl_data.emplace(std::string(ClassNameCStr), NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(std::string(ClassNameCStr)));

			flecs::_::type_impl_data& Data = flecs::_::g_type_to_impl_data.at(std::string(ClassNameCStr));
			
			flecs_component_ids_set(World, Data.s_index, ScriptClassEntity);
			TypeMapComponent->ScriptClassMap.emplace(ScriptClass, ScriptClassEntity);
		});

		SetScope(OldScope);
		
		return ScriptClassEntity;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE bool HasScriptClass(const TSubclassOf<UObject> InClass) const
	{
		solid_check(InClass);
		
		if (TypeMapComponent->ScriptClassMap.contains(FFlecsScriptClassComponent(InClass)))
		{
			const FFlecsId Component = TypeMapComponent->ScriptClassMap.at(FFlecsScriptClassComponent(InClass));
			return ecs_is_valid(World.c_ptr(), Component);
		}

		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle GetScriptClassEntity(const TSubclassOf<UObject> InClass) const
	{
		solid_check(InClass);
		
		const FFlecsId Component = TypeMapComponent->ScriptClassMap.at(FFlecsScriptClassComponent(InClass));
		solid_checkf(IsAlive(Component), TEXT("Entity is not alive"));
		
		return FFlecsEntityHandle(World, Component);
	}
	
	/*
	 FFlecsEntityHandle RegisterComponentBitmaskType(const UEnum* ScriptEnum) const
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
	}
	*/

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

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle RegisterComponentType(
		const TSolidNotNull<const UEnum*> ScriptEnum) const
	{
		if (HasScriptEnum(ScriptEnum))
		{
			return GetScriptEnumEntity(ScriptEnum);
		}

		return RegisterScriptEnum(ScriptEnum);
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

		FFlecsEntityHandle TagEntity;
		
		Scope<FFlecsGameplayTagManagerEntity>([&TagEntity, &TagName, this]()
		{
			TagEntity = LookupEntity(TagName, ".", ".");
		});
		
		return TagEntity;
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
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab(const FString& Name = "") const
	{
		return World.prefab(StringCast<char>(*Name).Get());
	}

	template <typename T>
	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab() const
	{
		return World.prefab<T>();
	}

	FORCEINLINE_DEBUGGABLE FFlecsEntityHandle CreatePrefab(const TSolidNotNull<UClass*> InClass) const
	{
		const FFlecsEntityHandle PrefabEntity = ObtainTypedEntity(InClass).Add(flecs::Prefab);
		
		return PrefabEntity;
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
	FORCEINLINE_DEBUGGABLE void Scope(FunctionType&& Function) const
	{
		World.scope<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE_DEBUGGABLE void Scope(FFlecsId InId, FunctionType&& Function) const
	{
		World.scope(InId, std::forward<FunctionType>(Function));
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
	
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	flecs::world World;

	bool bIsInitialized = false;

	UPROPERTY(Transient)
	TScriptInterface<IFlecsGameLoopInterface> GameLoopInterface;

	UPROPERTY(Transient)
	TArray<TScriptInterface<IFlecsModuleInterface>> ImportedModules;
	
	TMap<const UClass*, TScriptInterface<IFlecsObjectRegistrationInterface>> RegisteredObjectTypes;

	UPROPERTY(Transient)
	TArray<TScriptInterface<IFlecsObjectRegistrationInterface>> RegisteredObjects;

	flecs::query<FFlecsModuleComponent> ModuleComponentQuery;
	flecs::query<FFlecsUObjectComponent> ObjectComponentQuery;
	flecs::query<FFlecsDependenciesComponent> DependenciesComponentQuery;

	FFlecsTypeMapComponent* TypeMapComponent;

private:
	FORCEINLINE_DEBUGGABLE UFlecsWorld* GetSelf() const
	{
		return const_cast<UFlecsWorld*>(this);
	}
	
}; // class UFlecsWorld
