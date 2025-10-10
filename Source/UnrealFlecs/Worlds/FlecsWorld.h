// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include <utility>

#include "flecs.h"

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"
#include "Concepts/SolidConcepts.h"

#include "FlecsScopedDeferWindow.h"
#include "Entities/FlecsId.h"
#include "Modules/FlecsDependenciesComponent.h"
#include "Properties/FlecsComponentProperties.h"

#include "FlecsWorld.generated.h"

struct FFlecsEntityRecord;
struct FFlecsDependenciesComponent;
struct FFlecsUObjectComponent;
struct FFlecsModuleComponent;

class IFlecsObjectRegistrationInterface;
class IFlecsModuleInterface;
class IFlecsGameLoopInterface;
class UFlecsWorldSubsystem;
class UFlecsModuleInterface;

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
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Singleton);
	});


UCLASS(BlueprintType, NotBlueprintable)
class UNREALFLECS_API UFlecsWorld : public UObject
{
	GENERATED_BODY()

public:
	UFlecsWorld(const FObjectInitializer& ObjectInitializer);
	
	virtual ~UFlecsWorld() override;

	static NO_DISCARD UFlecsWorld* GetDefaultWorld(const UObject* WorldContextObject);

	void WorldStart();

	// ReSharper disable once CppMemberFunctionMayBeConst
	void WorldBeginPlay();

	void InitializeDefaultComponents() const;

	void InitializeFlecsRegistrationObjects();

	void RegisterUnrealTypes() const;

	/*
	 * I hate this
	 **/
	template <typename FunctionType>
	void UnlockIter_Internal(flecs::iter& Iter, FunctionType&& Function)
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

				std::invoke(Function, Iter);

				internal_ecs_table_enable_lock(Iter.table(), SavedLockCount);
			}

			if (IsReadOnly())
			{
				BeginReadOnly();
			}
		});
	}

	void InitializeSystems();

	/**
	 * @brief Asynchronously Register a module dependency,
	 * if the dependency is already imported, the function is called immediately
	 * if the dependency is not imported, the function is called when the dependency is imported (if it is)
	 * @param InModuleObject The module object
	 * @param InFunction The function to call when the dependency is imported
	 * @tparam TModule The module class
	 */
	template <Solid::TStaticClassConcept TModule, typename TFunction>
	void RegisterModuleDependency(const TSolidNotNull<const UObject*> InModuleObject,
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
		const FFlecsDependencyFunctionDefinition::FDependencyFunctionType& InFunction);

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void Reset();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ResetClock() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	FFlecsEntityHandle CreateEntity(const FString& Name = "") const;

	template <typename T>
	FFlecsEntityHandle ObtainTypedEntity() const
	{
		const FFlecsEntityHandle EntityHandle = World.entity<T>();
		return EntityHandle;
	}

	FFlecsEntityHandle ObtainTypedEntity(const TSolidNotNull<UClass*> InClass) const;

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
	void ForEach(const FFlecsId& InTermId, const FunctionType& Function) const
	{
		World.each(InTermId, Function);
	}

	template <typename T>
	UFlecsWorld* AddSingleton() const
	{
		World.add<T>();
		return GetSelf();
	}

	template <typename T>
	UFlecsWorld* SetSingleton(const T& Value) const
	{
		World.set<T>(Value);
		return GetSelf();
	}

	template <typename T>
	UFlecsWorld* RemoveSingleton() const
	{
		World.remove<T>();
		return GetSelf();
	}
	
	template <typename T>
	NO_DISCARD bool HasSingleton() const
	{
		return World.has<T>();
	}

	template <typename T>
	NO_DISCARD const T& GetSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.get<T>();
	}

	template <typename T>
	NO_DISCARD T& GetMutSingleton() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD const T* GetSingletonPtr() const
	{
		return World.try_get<T>();
	}

	template <typename T>
	NO_DISCARD T* GetMutSingletonPtr() const
	{
		return World.try_get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD FFlecsEntityHandle ObtainSingletonEntity() const
	{
		solid_checkf(HasSingleton<T>(), TEXT("Singleton %hs not found"), nameof(T).data());
		return World.singleton<T>();
	}

	template <typename T>
	void ModifiedSingleton() const
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
	FFlecsEntityHandle ImportFlecsModule()
	{
		static_assert(!TIsDerivedFrom<T, IFlecsModuleInterface>::Value,
			"T must not be derived from IFlecsModuleInterface, use ImportModule (Non-Template) instead");
		return World.import<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | World")
	void ImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule);

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool IsModuleImported(const TSubclassOf<UObject> InModule, const bool bAllowChildren = false) const;

	template <Solid::TStaticClassConcept T>
	NO_DISCARD bool IsModuleImported(const bool bAllowChildren = false) const
	{
		return IsModuleImported(T::StaticClass(), bAllowChildren);
	}

	/**
	 * @brief Get the entity handle of the module with the given class
	 * @param InModule The module class
	 * @return The entity handle of the module, or an invalid handle if the module is not imported
	 */
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetModuleEntity(const TSubclassOf<UObject> InModule, const bool bAllowChildren = false) const;

	/**
	 * @brief Get the entity handle of the module with the given class
	 * @tparam T The module class
	 * @return The entity handle of the module, or an invalid handle if the module is not imported
	 */
	template <Solid::TStaticClassConcept T>
	NO_DISCARD FFlecsEntityHandle GetModuleEntity(const bool bAllowChildren = false) const
	{
		return GetModuleEntity(T::StaticClass(), bAllowChildren);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	UObject* GetModule(const TSubclassOf<UObject> InModule, const bool bAllowChildren = false) const;

	template <Solid::TStaticClassConcept T>
	NO_DISCARD TSolidNotNull<T*> GetModule(const bool bAllowChildren = false) const
	{
		return CastChecked<T>(GetModule(T::StaticClass(), bAllowChildren));
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
	void Defer(TFunction&& Function) const
	{
		World.defer<TFunction>(std::forward<TFunction>(Function));
	}

	/**
	 * @brief End or Suspend the defer state and execute the lambda function.
	 * @param Function The function to execute after the defer state is ended or suspended.
	 * @param bEndDefer If true, the defer state will be ended after the function is executed, otherwise it will be suspended and resumed after the function is executed.
	 */
	template <typename TFunction>
	void DeferEndLambda(TFunction&& Function, const bool bEndDefer = false) const
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
	void ReadOnly(TFunction&& Function) const
	{
		BeginReadOnly();
		std::invoke(std::forward<TFunction>(Function));
		EndReadOnly();
	}

	template <typename TFunction>
	void WithScoped(const FFlecsId InId, TFunction&& Function) const
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
	void SetPipeline() const
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
	NO_DISCARD flecs::system_builder<TComponents...> CreateSystemWithBuilder(const FString& InName) const
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
	void SetEntityRange(const FFlecsId InMin, const FFlecsId InMax,
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
	void ForEachChild(FunctionType&& Function) const
	{
		World.children(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	void ForEachChild(FunctionType&& Function) const
	{
		World.children<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	void ForEachChild(const FFlecsId& InRelationId, FunctionType&& Function) const
	{
		World.children(InRelationId, std::forward<FunctionType>(Function));
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void SetThreads(const int32 InThreadCount) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	int32 GetThreads() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool UsingTaskThreads() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	void SetTaskThreads(const int32 InThreadCount) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool HasScriptStruct(const UScriptStruct* ScriptStruct) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	bool HasScriptEnum(const UEnum* ScriptEnum) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetScriptStructEntity(const UScriptStruct* ScriptStruct) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | World")
	FFlecsEntityHandle GetScriptEnumEntity(const UEnum* ScriptEnum) const;

	template <Solid::TScriptStructConcept T>
	NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(TBaseStructure<T>::Get());
	}

	template <Solid::TScriptStructConcept T>
	NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(TBaseStructure<T>::Get());
	}

	template <Solid::TStaticEnumConcept T>
	NO_DISCARD FFlecsEntityHandle GetScriptEnumEntity() const
	{
		return GetScriptEnumEntity(StaticEnum<T>());
	}

	template <Solid::TStaticEnumConcept T>
	NO_DISCARD bool HasScriptEnum() const
	{
		return HasScriptEnum(StaticEnum<T>());
	}

	void RegisterMemberProperties(const TSolidNotNull<const UStruct*> InStruct,
	                              const FFlecsComponentHandle& InComponent) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptStruct(const UScriptStruct* ScriptStruct, const bool bComponent = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptEnum(const UEnum* ScriptEnum) const;

	template <typename T>
	requires (std::is_enum<T>::value)
	FFlecsEntityHandle RegisterScriptEnum() const
	{
		return World.component<T>();
	}
	
	FFlecsEntityHandle RegisterComponentEnumType(TSolidNotNull<const UEnum*> ScriptEnum) const;

	FFlecsEntityHandle RegisterScriptClassType(TSolidNotNull<UClass*> ScriptClass) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasScriptClass(const TSubclassOf<UObject> InClass) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScriptClassEntity(const TSubclassOf<UObject> InClass) const;
	
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
	FFlecsComponentHandle RegisterComponentType() const
	{
		FFlecsComponentHandle Component = World.component<T>();
		solid_check(Component.IsValid());
		
		if constexpr (Solid::IsScriptStruct<T>())
		{
			RegisterMemberProperties(TBaseStructure<T>::Get(), Component);
		}

		return Component;
	}
	
	FFlecsEntityHandle RegisterComponentType(const TSolidNotNull<const UScriptStruct*> ScriptStruct) const;

	FFlecsEntityHandle RegisterComponentType(const TSolidNotNull<const UEnum*> ScriptEnum) const;

	template <typename T>
	FFlecsComponentHandle ObtainComponentTypeStruct() const
	{
		solid_checkf(World.is_valid(flecs::_::type<T>::id(World)),
			TEXT("Component %hs is not registered"), nameof(T).data());
		return World.component<T>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsComponentHandle ObtainComponentTypeStruct(const UScriptStruct* ScriptStruct) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsComponentHandle ObtainComponentTypeEnum(const UEnum* ScriptEnum) const;

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
	void RunPipeline(const FFlecsId InPipeline, const double DeltaTime = 0.0) const;

	void RandomizeTimers() const;

	NO_DISCARD flecs::timer CreateTimer(const FString& Name) const;

	template <typename T>
	NO_DISCARD flecs::timer CreateTimer() const
	{
		return World.timer<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasEntityWithName(const FString& Name, FFlecsEntityHandle& OutEntity) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsEntityHandle GetTagEntity(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord,
	                                          const FString& Name = "") const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle CreatePrefab(const FString& Name = "") const;

	template <typename T>
	FFlecsEntityHandle CreatePrefab() const
	{
		return World.prefab<T>();
	}

	FFlecsEntityHandle CreatePrefab(const TSolidNotNull<UClass*> InClass) const;
	FFlecsEntityHandle CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord,
	                                          const TSolidNotNull<UClass*> InClass) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyPrefab(const FFlecsEntityHandle& InPrefab) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool ShouldQuit() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsId ClearScope() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FFlecsId SetScope(const FFlecsId InScope) const;

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
	FFlecsEntityHandle GetScope() const;

	virtual bool IsSupportedForNetworking() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsNameStableForNetworking() const override;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	UFlecsWorldSubsystem* GetContext() const;

	NO_DISCARD FFlecsTypeMapComponent* GetTypeMapComponent() const;

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
	NO_DISCARD FORCEINLINE UFlecsWorld* GetSelf() const
	{
		return const_cast<UFlecsWorld*>(this);
	}
	
}; // class UFlecsWorld
