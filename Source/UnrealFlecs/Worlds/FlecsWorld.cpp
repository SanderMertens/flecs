// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppDeclaratorNeverUsed
#include "FlecsWorld.h"

#include "Engine/Engine.h"
#include "Misc/AutomationTest.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetBundleData.h"
#include "AssetRegistry/AssetData.h"

#include "Math/TransformCalculus2D.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"

#include "FlecsWorldSubsystem.h"

#include "Logs/FlecsCategories.h"

#include "Entities/FlecsEntityRecord.h"

#include "Components/FlecsAddReferencedObjectsTrait.h"
#include "Components/FlecsNetworkSerializeDefinitionComponent.h"
#include "Components/FlecsPrimaryAssetComponent.h"
#include "Components/FlecsUObjectComponent.h"
#include "Components/ObjectTypes/FFlecsActorComponentTag.h"
#include "Components/ObjectTypes/FFlecsModuleObject.h"
#include "Components/ObjectTypes/FFlecsSceneComponentTag.h"
#include "Components/ObjectTypes/FlecsActorTag.h"
#include "Components/FlecsModuleComponent.h"
#include "General/FlecsDeveloperSettings.h"

#include "Modules/FlecsDependenciesComponent.h"
#include "Modules/FlecsModuleInitEvent.h"
#include "Modules/FlecsModuleInterface.h"

#include "General/FlecsGameplayTagManagerEntity.h"
#include "General/FlecsObjectRegistrationInterface.h"

#include "Pipelines/FlecsGameLoopInterface.h"
#include "Types/SolidCppStructOps.h"
#include "UObject/UObjectIterator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorld)

// static bool GFlecs_bRegisterMemberTypeStructs = false;
// FAutoConsoleVariable CVarFlecsRegisterMemberTypeStructs(
// 	TEXT("Flecs.RegisterMemberTypeStructs"),
// 	GFlecs_bRegisterMemberTypeStructs,
// 	TEXT("Register the member type structs as components if not previously registered."),
// 	ECVF_Default);

DECLARE_STATS_GROUP(TEXT("FlecsWorld"), STATGROUP_FlecsWorld, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress"),
	STAT_FlecsWorldProgress, STATGROUP_FlecsWorld);

DECLARE_CYCLE_STAT(TEXT("FlecsWorld::Progress::ProgressModule"),
	STAT_FlecsWorldProgressModule, STATGROUP_FlecsWorld);

namespace
{
	inline void ScriptStructConstructor(void* Ptr, int32_t Count, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(Ptr != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		ScriptStruct->InitializeStruct(Ptr, Count);
	}

	inline void ScriptStructDestructor(void* Ptr, int32_t Count, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(Ptr != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		ScriptStruct->DestroyStruct(Ptr, Count);
	}

	// @TODO: maybe add a RESTRICT keyword?
	
	inline void ScriptStructCopy(void* Destination, const void* Source, int32_t Count, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(Destination != nullptr);
		solid_cassume(Source != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		ScriptStruct->CopyScriptStruct(Destination, Source, Count);
	}

	inline void ScriptStructMove(void* Destination, void* Source, int32_t Count, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(Destination != nullptr);
		solid_cassume(Source != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		Solid::MoveAssignScriptStruct(ScriptStruct, Destination, Source, Count);
	}

	inline void ScriptStructMoveConstruct(void* Destination, void* Source, int32_t Count, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(Destination != nullptr);
		solid_cassume(Source != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		Solid::MoveConstructScriptStruct(ScriptStruct, Destination, Source, Count);
	}

	// @TODO: implement
	inline int32 ScriptStructCompare(const void* A, const void* B, const ecs_type_info_t* TypeInfo)
	{
		// empty
		return 0;
	}
	
	inline bool ScriptStructEquals(const void* A, const void* B, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(A != nullptr);
		solid_cassume(B != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		const UScriptStruct* ScriptStruct = static_cast<UScriptStruct*>(TypeInfo->hooks.ctx);
		solid_check(IsValid(ScriptStruct));

		return ScriptStruct->CompareScriptStruct(A, B, PPF_None);
	}

	inline bool ScriptStructEqualsSimple(const void* A, const void* B, const ecs_type_info_t* TypeInfo)
	{
		solid_cassume(TypeInfo != nullptr);
		solid_cassume(A != nullptr);
		solid_cassume(B != nullptr);
		solid_cassume(TypeInfo->hooks.ctx != nullptr);

		return FMemory::Memcmp(A, B, TypeInfo->size) == 0;
	}
	
} // namespace

UFlecsWorld::UFlecsWorld(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	const FString ObjectName = GetName();
	const char* ObjectNameCStr = StringCast<char>(*ObjectName).Get();
	char* argv[] = { const_cast<char*>(ObjectNameCStr) };
		
	World = flecs::world(1, argv);
		
	TypeMapComponent = GetTypeMapComponent();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
	solid_check(TypeMapComponent);
}

UFlecsWorld::~UFlecsWorld()
{
	const FAssetRegistryModule* AssetRegistryModule
		= FModuleManager::LoadModulePtr<FAssetRegistryModule>("AssetRegistry");

	if LIKELY_IF(AssetRegistryModule && AssetRegistryModule->IsValid())
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

	FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);
}

UFlecsWorld* UFlecsWorld::GetDefaultWorld(const UObject* WorldContextObject)
{
	solid_check(WorldContextObject);

	const TSolidNotNull<const UWorld*> GameWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	
	const TSolidNotNull<const UFlecsWorldSubsystem*> WorldSubsystem = GameWorld->GetSubsystemChecked<UFlecsWorldSubsystem>();
	
	return WorldSubsystem->GetDefaultWorld();
}

void UFlecsWorld::WorldStart()
{
	UE_LOGFMT(LogFlecsWorld, Log, "Flecs World started: {WorldObjectName}", *GetName());
		
#if WITH_AUTOMATION_TESTS
	if (!GIsAutomationTesting)
	{
#endif // WITH_AUTOMATION_TESTS
		//InitializeAssetRegistry();
#if WITH_AUTOMATION_TESTS
	}
#endif // WITH_AUTOMATION_TESTS

	InitializeFlecsRegistrationObjects();

	const TSolidNotNull<const UFlecsDeveloperSettings*> FlecsDeveloperSettings = GetDefault<UFlecsDeveloperSettings>();
	
	if (FlecsDeveloperSettings->bDeleteEmptyTablesOnGC)
	{
		DeleteEmptyTablesGCDelegateHandle = FCoreUObjectDelegates::GarbageCollectComplete
			.AddWeakLambda(this, [this, FlecsDeveloperSettings]()
			{
				if UNLIKELY_IF(!bIsInitialized)
				{
					return;
				}
				
				UE_LOGFMT(LogFlecsWorld, Verbose,
							  "Flecs World {WorldName} Deleting empty tables on GC",
							  *GetName());

				const int32 DeletedTables = DeleteEmptyTables(
					FlecsDeveloperSettings->TimeBudget,
					FlecsDeveloperSettings->ClearGeneration,  // NOLINT(clang-diagnostic-implicit-int-conversion)
					FlecsDeveloperSettings->DeleteGeneration); // NOLINT(clang-diagnostic-implicit-int-conversion)
				
				UE_LOGFMT(LogFlecsWorld, Log,
							  "Flecs World {WorldName} Deleted {DeletedTableCount} empty tables on GC",
							  *GetName(),
							  DeletedTables);
			});
	}
	
}

void UFlecsWorld::WorldBeginPlay()
{
	AddSingleton<FFlecsBeginPlaySingletonComponent>();
}

void UFlecsWorld::InitializeDefaultComponents() const
{
	//World.component<FFlecsEntityHandle>()
	//	.disable();
		
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
		     TSolidNotNull<const TCHAR*> CharArray = String.GetCharArray().GetData();
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
		     const FFlecsId TagEntity = ecs_lookup_path_w_sep(
			     Serializer->world,
			     flecs::component<FFlecsGameplayTagManagerEntity>(const_cast<flecs::world_t*>(Serializer->world)),
			     StringCast<char>(*Data->ToString()).Get(),
			     ".",
			     nullptr,
			     true);
		 		
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

	/*World.component<FScriptArray>()
	     .opaque<flecs::Vector>(flecs::meta::VectorType);*/
	/* World.component<FScriptMap>()
	     .opaque<flecs::Map>(flecs::meta::MapType); */

	RegisterUnrealTypes();

	RegisterComponentType<FFlecsAddReferencedObjectsTrait>()
		.Add(flecs::Trait);

	RegisterComponentType<FFlecsBeginPlaySingletonComponent>();

	RegisterComponentType<FFlecsUObjectComponent>();
		
	RegisterComponentType<FFlecsUObjectTag>();
		
	RegisterComponentType<FFlecsActorTag>()
		.SetIsA<FFlecsUObjectTag>();
		
	RegisterComponentType<FFlecsActorComponentTag>()
		.SetIsA<FFlecsUObjectTag>();
		
	RegisterComponentType<FFlecsModuleObjectTarget>()
		.SetIsA<FFlecsUObjectTag>();
		
	RegisterComponentType<FFlecsSceneComponentTag>()
		.SetIsA<FFlecsActorComponentTag>();

	RegisterComponentType<FFlecsModuleComponent>();
	RegisterComponentType<FFlecsModuleInitEvent>();
	RegisterComponentType<FFlecsDependenciesComponent>();

	RegisterComponentType<FFlecsEntityRecord>();

	RegisterComponentType<FFlecsNetworkSerializeDefinitionComponent>()
		.Add(flecs::Sparse);
}

void UFlecsWorld::InitializeFlecsRegistrationObjects()
{
	for (TObjectIterator<UClass> It = TObjectIterator<UClass>(); It; ++It)
	{
		const TNotNull<const UClass*> Class = *It;
			
		if (Class->ImplementsInterface(UFlecsObjectRegistrationInterface::StaticClass()))
		{
			// @TODO: should we log if the class is abstract?
			if (Class->HasAnyClassFlags(CLASS_Abstract))
			{
				UE_LOGFMT(LogFlecsWorld, Verbose,
				          "Skipping registration of {ClassName} because it is abstract",
				          Class->GetName());
				continue;
			}
				
			if UNLIKELY_IF(Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
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

void UFlecsWorld::RegisterUnrealTypes() const
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

void UFlecsWorld::InitializeComponentPropertyObserver() const
{
	/*CreateObserver("AnyComponentObserver")
			.event(flecs::OnSet)
			.with<flecs::Component>() // 0
			.with_symbol_component().filter() // 1
			.yield_existing()
			.each([this](flecs::iter& Iter, size_t Index)
			{
				//while (Iter.next())
					//UnlockIter_Internal(Iter, [this](flecs::iter& Iter)
				{
					//for (const flecs::entity_t Index : Iter)
					{
						const FFlecsEntityHandle EntityHandle = Iter.entity(Index);
					
						const FString StructSymbol = EntityHandle.GetSymbol();
						
						if (FFlecsComponentPropertiesRegistry::Get().ContainsComponentProperties(StructSymbol))
						{
							FFlecsComponentHandle InUntypedComponent = EntityHandle.GetUntypedComponent_Unsafe();
							
							const FFlecsComponentProperties& Properties = FFlecsComponentPropertiesRegistry::Get()
								.GetComponentProperties(StructSymbol);

							std::invoke(Properties.RegistrationFunction, Iter.world(), InUntypedComponent);

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
					}
				};
			});*/

	FlecsLibrary::GetTypeRegisteredDelegate().AddWeakLambda(this, [this](const flecs::id_t InEntityId)
	{
		solid_checkf(!IsDeferred(), TEXT("Cannot register component properties while world is deferred."));
		
		const FFlecsEntityHandle EntityHandle = FFlecsEntityHandle(World, InEntityId);
					
		const FString StructSymbol = EntityHandle.GetSymbol();
						
		if (FFlecsComponentPropertiesRegistry::Get().ContainsComponentProperties(StructSymbol))
		{
			FFlecsComponentHandle InUntypedComponent = EntityHandle.GetUntypedComponent_Unsafe();
							
			const FFlecsComponentProperties& Properties = FFlecsComponentPropertiesRegistry::Get()
				.GetComponentProperties(StructSymbol);

			std::invoke(Properties.RegistrationFunction, World, InUntypedComponent);

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

}

void UFlecsWorld::InitializeSystems()
{
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

		/*CreateObserver<const FFlecsModuleComponent&>(TEXT("ModuleInitEventObserver"))
			.event<FFlecsModuleInitEvent>()
			.with<FFlecsUObjectComponent&, FFlecsModuleComponentTag>()
			.run([this](flecs::iter& Iter)
			{
				// @TODO: Document these workarounds
				UnlockIter_Internal(Iter, [this](flecs::iter& Iter)
				{
					for (const flecs::entity_t Index : Iter)
					{
						const FFlecsEntityHandle ModuleEntity = Iter.entity(Index);
						
						const FFlecsModuleComponent& InModuleComponent = Iter.field_at<FFlecsModuleComponent>(0, Index);
						const FFlecsUObjectComponent& InUObjectComponent = Iter.field_at<FFlecsUObjectComponent>(1, Index);
						
						solid_check(InUObjectComponent.IsValid());
						solid_check(InModuleComponent.ModuleClass);
						
						UE_LOGFMT(LogFlecsWorld, Log,
							"Module initialized: {ModuleName}", *InUObjectComponent.GetObjectChecked()->GetName());

						DependenciesComponentQuery.each([InModuleComponent, ModuleEntity, this, InUObjectComponent]
							(flecs::iter& DependenciesIter, size_t DependenciesIndex)
							{
								const FFlecsEntityHandle InEntity = DependenciesIter.entity(DependenciesIndex);
										
								FFlecsDependenciesComponent& DependenciesComponent
									= DependenciesIter.field_at<FFlecsDependenciesComponent>(0, DependenciesIndex);
											
								if (DependenciesComponent.DependencyFunctionPtrs.Contains(InModuleComponent.ModuleClass))
								{
									const FFlecsDependencyFunctionDefinition& FunctionDefinition
										= DependenciesComponent.DependencyFunctionPtrs[InModuleComponent.ModuleClass];

									InEntity.AddPair(flecs::DependsOn, ModuleEntity);

									FunctionDefinition.Call(InUObjectComponent.GetObjectChecked(),
										this,
										ModuleEntity);
								}
							});
					}
				});
			});*/

		OnModuleImported.AddWeakLambda(this,
			[](const TSolidNotNull<UFlecsWorld*> InFlecsWorld, const FFlecsEntityHandle InModule)
		{
			if UNLIKELY_IF(!ensureAlwaysMsgf(InModule.IsValid(), TEXT("Module entity is not alive")))
			{
				return;
			}
			
			if (!InModule.Has<FFlecsModuleComponent>())
			{
				UE_LOGFMT(LogFlecsWorld, Warning,
					"Module entity {ModuleEntityId} does not have FFlecsModuleComponent",
					InModule.ToString());
				return;
			}

			if (!InModule.HasPair<FFlecsUObjectComponent, FFlecsModuleObjectTarget>())
			{
				UE_LOGFMT(LogFlecsWorld, Warning,
					"Module entity {ModuleEntityId} does not have FFlecsUObjectComponent pair",
					InModule.ToString());
				return;
			}
			
			const TSolidNotNull<UObject*> ModuleObject = InModule.GetPairFirst<FFlecsUObjectComponent>(flecs::Wildcard).GetObjectChecked();
			solid_check(IsValid(ModuleObject));

			const FFlecsEntityHandle ModuleEntity = InFlecsWorld->GetModuleEntity(ModuleObject->GetClass());
			solid_check(ModuleEntity.IsValid());

			InFlecsWorld->DependenciesComponentQuery.each([ModuleEntity, InFlecsWorld, ModuleObject]
				(flecs::iter& DependenciesIter, size_t DependenciesIndex, FFlecsDependenciesComponent& DependenciesComponent)
				{
					const FFlecsEntityHandle InEntity = DependenciesIter.entity(DependenciesIndex);
											
					if (DependenciesComponent.DependencyFunctionPtrs.Contains(ModuleObject->GetClass()))
					{
						const FFlecsDependencyFunctionDefinition& FunctionDefinition
							= DependenciesComponent.DependencyFunctionPtrs[ModuleObject->GetClass()];

						InEntity.AddPair(flecs::DependsOn, ModuleEntity);

						FunctionDefinition.Call(ModuleObject,
							InFlecsWorld,
							ModuleEntity);
					}
				});
		});

		CreateObserver<const FFlecsUObjectComponent&>(TEXT("RemoveModuleComponentObserver"))
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

void UFlecsWorld::RegisterModuleDependency(const TSolidNotNull<const UObject*> InModuleObject,
                                           const TSubclassOf<UFlecsModuleInterface>& InDependencyClass,
                                           const FFlecsDependencyFunctionDefinition::FDependencyFunctionType& InFunction)
{
	solid_check(InModuleObject->Implements<UFlecsModuleInterface>());

	solid_checkf(IsModuleImported(InModuleObject->GetClass()),
	             TEXT("Module %s is not imported"), *InModuleObject->GetClass()->GetName());

	const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModuleObject->GetClass());
	solid_check(ModuleEntity.IsValid());
		
	auto& [Dependencies] = ModuleEntity.Obtain<FFlecsDependenciesComponent>();
		
	Dependencies.Add(InDependencyClass, FFlecsDependencyFunctionDefinition{.Function=InFunction});
		
	if (IsModuleImported(InDependencyClass))
	{
		const FFlecsEntityHandle DependencyEntity = GetModuleEntity(InDependencyClass);
			
		TSolidNotNull<UObject*> DependencyModuleObject = GetModule(InDependencyClass);
		solid_check(IsValid(DependencyModuleObject));

		solid_check(DependencyEntity.IsValid());

		ModuleEntity.AddPair(flecs::DependsOn, DependencyEntity);
		std::invoke(InFunction, DependencyModuleObject, this, DependencyEntity);
	}
}

void UFlecsWorld::Reset()
{
	World.reset();
}

void UFlecsWorld::ResetClock() const
{
	World.reset_clock();
}

FFlecsEntityHandle UFlecsWorld::CreateEntity(const FString& Name) const
{
	return World.entity(StringCast<char>(*Name).Get());
}

FFlecsEntityHandle UFlecsWorld::ObtainTypedEntity(const TSolidNotNull<UClass*> InClass) const
{
	const FFlecsEntityHandle EntityHandle = World.entity(RegisterScriptClassType(InClass));
	return EntityHandle;
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithId(const FFlecsId InId) const
{
	solid_checkf(!IsAlive(InId), TEXT("Entity with ID %s is already alive"), *InId.ToString());
	return MakeAlive(InId);
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithPrefab(const FFlecsId InPrefab) const
{
	return CreateEntity().SetIsA(InPrefab);
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithRecord(const FFlecsEntityRecord& InRecord, const FString& Name) const
{
	const FFlecsEntityHandle Entity = CreateEntity(Name);
	InRecord.ApplyRecordToEntity(this, Entity);
	return Entity;
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
	const FFlecsId InId) const
{
	const FFlecsEntityHandle Entity = CreateEntityWithId(InId);
	InRecord.ApplyRecordToEntity(this, Entity);
	return Entity;
}

FFlecsEntityHandle UFlecsWorld::LookupEntity(const FString& Name,
	const FString& Separator, const FString& RootSeparator,
	const bool bRecursive) const
{
	return World.lookup(StringCast<char>(*Name).Get(),
						StringCast<char>(*Separator).Get(),
	                    StringCast<char>(*RootSeparator).Get(),
	                    bRecursive);
}

void UFlecsWorld::DestroyEntityByName(const FString& Name) const
{
	solid_checkf(!Name.IsEmpty(), TEXT("Name is empty"));

	const FFlecsEntityHandle Handle = LookupEntity(Name);
		
	if LIKELY_IF(Handle.IsValid())
	{
		Handle.Destroy();
	}
	else
	{
		UE_LOGFMT(LogFlecsWorld, Warning, "Entity {EntityName} not found", Name);
	}
}

void UFlecsWorld::Merge() const
{
	World.merge();
}

FString UFlecsWorld::GetWorldName() const
{
	return GetWorldEntity().GetName();
}

void UFlecsWorld::SetWorldName(const FString& InName) const
{
	GetWorldEntity().SetName(InName);
}

void UFlecsWorld::ImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule)
{
	solid_checkf(InModule, TEXT("Module is nullptr"));

	FString FailureReason;
	if (!CanImportModule(InModule, FailureReason))
	{
		UE_LOGFMT(LogFlecsWorld, Warning,
			"Cannot import module {ModuleName}: {FailureReason}",
			InModule.GetObject()->GetName(),
			FailureReason);
		return;
	}

	// Doesn't use TSolidNotNull because DuplicateObject works weird with it
	const UObject* TemplateModuleObject = InModule.GetObject();
	solid_check(IsValid(TemplateModuleObject));

	const TSolidNotNull<UObject*> NewModuleObject = DuplicateObject(TemplateModuleObject, this);
	ImportedModules.Add(NewModuleObject);
		
	ImportedModules.Last()->ImportModule(World);
}

bool UFlecsWorld::CanImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule, FString& OutFailureReason) const
{
	solid_checkf(InModule, TEXT("Module is nullptr"));

	if (IsModuleImported(InModule.GetObject()->GetClass()))
	{
		OutFailureReason = FString::Printf(TEXT("Module %s is already imported"), *InModule.GetObject()->GetName());
		return false;
	}

	const TArray<TSubclassOf<UObject>> HardModuleDependencies = InModule->GetHardDependentModuleClasses();
	
	for (const TSubclassOf<UObject>& DependencyClass : HardModuleDependencies)
	{
		if (!IsModuleImported(DependencyClass))
		{
			OutFailureReason = FString::Printf(TEXT("Module %s dependency %s is not imported"),
				*InModule.GetObject()->GetName(),
				*DependencyClass->GetName());
			return false;
		}
	}

	return true;
}

bool UFlecsWorld::IsModuleImported(const TSubclassOf<UObject> InModule, const bool bAllowChildren) const
{
	solid_check(InModule);

	for (const TScriptInterface<IFlecsModuleInterface>& ModuleObject : ImportedModules)
	{
		if (ModuleObject.GetObject()->GetClass() == InModule)
		{
			return true;
		}
		else if (bAllowChildren && ModuleObject.GetObject()->GetClass()->IsChildOf(InModule))
		{
			return true;
		}
	}
		
	return false;
}

FFlecsEntityHandle UFlecsWorld::GetModuleEntity(const TSubclassOf<UObject> InModule, const bool bAllowChildren) const
{
	solid_check(InModule);
	
	const FFlecsEntityHandle ModuleEntity = ModuleComponentQuery
		.find([&InModule, bAllowChildren](flecs::entity InEntity, const FFlecsModuleComponent& InComponent)
		{
			return InComponent.ModuleClass == InModule || (bAllowChildren && InComponent.ModuleClass->IsChildOf(InModule));
		});

	if UNLIKELY_IF(!ModuleEntity.IsValid())
	{
		UE_LOGFMT(LogFlecsWorld, Warning,
			"Module %s is not imported", *InModule->GetName());
	}

	return ModuleEntity;
}

UObject* UFlecsWorld::GetModule(const TSubclassOf<UObject> InModule, const bool bAllowChildren) const
{
	solid_check(IsValid(InModule));
		
	const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModule, bAllowChildren);
	solid_checkf(ModuleEntity.IsValid(),
	             TEXT("Module %s is not imported"),
	             *InModule->GetName());
	
	return ModuleEntity.GetPairFirst<FFlecsUObjectComponent, FFlecsModuleObjectTarget>().GetObjectChecked();
}

bool UFlecsWorld::BeginDefer() const
{
	return World.defer_begin();
}

FFlecsScopedDeferWindow UFlecsWorld::DeferWindow() const
{
	return FFlecsScopedDeferWindow(this);
}

bool UFlecsWorld::EndDefer() const
{
	return World.defer_end();
}

void UFlecsWorld::ResumeDefer() const
{
	World.defer_resume();
}

void UFlecsWorld::SuspendDefer() const
{
	World.defer_suspend();
}

bool UFlecsWorld::IsDeferred() const
{
	return World.is_deferred();
}

bool UFlecsWorld::BeginReadOnly() const
{
	return World.readonly_begin();
}

void UFlecsWorld::EndReadOnly() const
{
	World.readonly_end();
}

void UFlecsWorld::SetContext(void* InContext) const
{
	World.set_ctx(InContext);
}

bool UFlecsWorld::ProgressGameLoop(const double DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_FlecsWorldProgress);

	solid_check(GameLoopInterface);
	return GameLoopInterface->Progress(DeltaTime, this);
}

bool UFlecsWorld::Progress(const double DeltaTime)
{
	return World.progress(DeltaTime);
}

void UFlecsWorld::SetTimeScale(const double InTimeScale) const
{
	World.set_time_scale(InTimeScale);
}

void UFlecsWorld::DestroyWorld()
{
	if UNLIKELY_IF(ShouldQuit())
	{
		UE_LOGFMT(LogFlecsWorld, Warning, "World is already being destroyed: {WorldObjectName}", *GetName());
		return;
	}

	FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);

	ModuleComponentQuery.destruct();
	DependenciesComponentQuery.destruct();
	ObjectComponentQuery.destruct();
		
	const FAssetRegistryModule* AssetRegistryModule
		= FModuleManager::LoadModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry"));

	if LIKELY_IF(AssetRegistryModule)
	{
		IAssetRegistry& AssetRegistry = AssetRegistryModule->Get();
		AssetRegistry.OnAssetAdded().RemoveAll(this);
		AssetRegistry.OnAssetRemoved().RemoveAll(this);
	}
		
	World.release();
	World.world_ = nullptr;
	MarkAsGarbage();
}

void UFlecsWorld::SetPipeline(const FFlecsEntityHandle& InPipeline) const
{
	World.set_pipeline(InPipeline);
}

FFlecsEntityHandle UFlecsWorld::GetPipeline() const
{
	return World.get_pipeline();
}

double UFlecsWorld::GetDeltaTime() const
{
	return World.delta_time();
}

FFlecsEntityHandle UFlecsWorld::GetAlive(const FFlecsId InId) const
{
	return World.get_alive(InId);
}

bool UFlecsWorld::IsAlive(const FFlecsId InId) const
{
	return World.is_alive(InId);
}

FFlecsEntityHandle UFlecsWorld::GetEntity(const FFlecsId InId) const
{
	return World.get_alive(InId);
}

FFlecsEntityHandle UFlecsWorld::MakeAlive(const FFlecsId& InId) const
{
	return World.make_alive(InId);
}

FFlecsEntityHandle UFlecsWorld::GetWorldEntity() const
{
	return World.entity(flecs::World);
}

int32 UFlecsWorld::GetStageCount() const
{
	return World.get_stage_count();
}

bool UFlecsWorld::IsReadOnly() const
{
	return World.is_readonly();
}

void UFlecsWorld::PreallocateEntities(const int32 InEntityCount) const
{
	if UNLIKELY_IF(ensureAlwaysMsgf(InEntityCount > 0,
		TEXT("Entity count must be greater than 0")))
	{
		return;
	}
		
	World.dim(InEntityCount);
}

void UFlecsWorld::SetEntityRange(const FFlecsId InMin, const FFlecsId InMax) const
{
	World.set_entity_range(InMin, InMax);
}

void UFlecsWorld::EnforceEntityRange(const bool bInEnforce) const
{
	World.enable_range_check(bInEnforce);
}

void UFlecsWorld::SetThreads(const int32 InThreadCount) const
{
	World.set_threads(InThreadCount);
}

int32 UFlecsWorld::GetThreads() const
{
	return World.get_threads();
}

bool UFlecsWorld::UsingTaskThreads() const
{
	return World.using_task_threads();
}

void UFlecsWorld::SetTaskThreads(const int32 InThreadCount) const
{
	World.set_task_threads(InThreadCount);
}

bool UFlecsWorld::HasScriptStruct(const UScriptStruct* ScriptStruct) const
{
	solid_check(ScriptStruct);
		
	if (TypeMapComponent->ScriptStructMap.contains(ScriptStruct))
	{
		const FFlecsId Component = TypeMapComponent->ScriptStructMap.at(ScriptStruct);
		return ecs_is_valid(World.c_ptr(), Component);
	}
		
	return false;
}

bool UFlecsWorld::HasScriptEnum(const UEnum* ScriptEnum) const
{
	solid_check(ScriptEnum);
		
	if (TypeMapComponent->ScriptEnumMap.contains(ScriptEnum))
	{
		const FFlecsId Component = TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
		return ecs_is_valid(World.c_ptr(), Component);
	}

	return false;
}

FFlecsEntityHandle UFlecsWorld::GetScriptStructEntity(const UScriptStruct* ScriptStruct) const
{
	solid_check(ScriptStruct);
		
	const FFlecsId Component = TypeMapComponent->ScriptStructMap.at(ScriptStruct);
	solid_checkf(IsAlive(Component), TEXT("Entity is not alive"));
		
	return GetAlive(Component);
}

FFlecsEntityHandle UFlecsWorld::GetScriptEnumEntity(const UEnum* ScriptEnum) const
{
	solid_check(ScriptEnum);
		
	const FFlecsId Component = TypeMapComponent->ScriptEnumMap.at(ScriptEnum);
	solid_checkf(ecs_is_valid(World.c_ptr(), Component), TEXT("Entity is not alive"));
	return GetAlive(Component);
}

void UFlecsWorld::RegisterMemberProperties(const TSolidNotNull<const UStruct*> InStruct,
	const FFlecsComponentHandle& InComponent) const
{
	solid_checkf(InComponent.IsValid(), TEXT("Entity is nullptr"));

	for (TFieldIterator<FProperty> PropertyIt(InStruct, EFieldIterationFlags::IncludeSuper);
	     PropertyIt; ++PropertyIt)
	{
		TSolidNotNull<FProperty*> Property = *PropertyIt;
		
		if (Property->IsA<FBoolProperty>())
		{
			InComponent.AddMember<bool>(Property->GetName(),
			                            0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FByteProperty>())
		{
			InComponent.AddMember<uint8>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FInt16Property>())
		{
			InComponent.AddMember<int16>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FUInt16Property>())
		{
			InComponent.AddMember<uint16>(Property->GetName(),
			                              0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FIntProperty>())
		{
			InComponent.AddMember<int32>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FUInt32Property>())
		{
			InComponent.AddMember<uint32>(Property->GetName(),
			                              0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FInt64Property>())
		{
			InComponent.AddMember<int64>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FUInt64Property>())
		{
			InComponent.AddMember<uint64>(Property->GetName(),
			                              0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FFloatProperty>())
		{
			InComponent.AddMember<float>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FDoubleProperty>())
		{
			InComponent.AddMember<double>(Property->GetName(),
			                              0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FStrProperty>())
		{
			InComponent.AddMember<FString>(Property->GetName(),
			                               0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FNameProperty>())
		{
			InComponent.AddMember<FName>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FTextProperty>())
		{
			InComponent.AddMember<FText>(Property->GetName(),
			                             0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FObjectProperty>())
		{
			InComponent.AddMember<FObjectPtr>(Property->GetName(),
			                                  0, Property->GetOffset_ForInternal());

			if UNLIKELY_IF(ecs_id_in_use(World.c_ptr(), InComponent))
			{
				continue;
			}
		}
		else if (Property->IsA<FWeakObjectProperty>())
		{
			InComponent.AddMember<FWeakObjectPtr>(Property->GetName(),
			                                      0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FSoftObjectProperty>())
		{
			InComponent.AddMember<FSoftObjectPtr>(Property->GetName(),
			                                      0, Property->GetOffset_ForInternal());
		}
		else if (Property->IsA<FClassProperty>())
		{
			InComponent.AddMember<TSubclassOf<UObject>>(Property->GetName(),
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
			 		
			InComponent.AddMember(StructComponent, Property->GetName(),
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

FFlecsEntityHandle UFlecsWorld::RegisterScriptStruct(const UScriptStruct* ScriptStruct, const bool bComponent) const
{
	solid_check(ScriptStruct);

		const FFlecsId OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptStructMap.contains(ScriptStruct),
			TEXT("Script struct %s is already registered"), *ScriptStruct->GetStructCPPName());
		
		FFlecsComponentHandle ScriptStructComponent;

		const FString StructName = ScriptStruct->GetStructCPPName();
		const char* StructNameCStr = StringCast<char>(*StructName).Get();

		// Register Member properties can't be deferred
		DeferEndLambda([this, ScriptStruct, &ScriptStructComponent, StructNameCStr, bComponent, &StructName]()
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
						UE_LOGFMT(LogFlecsComponent, Log,
							"Script struct {StructName} has a No-op constructor, this will not be used in flecs",
							ScriptStruct->GetName());
					}

					ScriptStructComponent.SetHooksLambda([ScriptStruct, &ScriptStructComponent](flecs::type_hooks_t& Hooks)
					{
						const bool bIsPOD = ScriptStruct->GetCppStructOps()->IsPlainOldData();
						const bool bHasCtor = !ScriptStruct->GetCppStructOps()->HasZeroConstructor();
						const bool bHasDtor = ScriptStruct->GetCppStructOps()->HasDestructor();
						const bool bHasCopy = ScriptStruct->GetCppStructOps()->HasCopy();
						const bool bHasMove = FSolidMoveableStructRegistry::Get().IsStructMoveAssignable(ScriptStruct);
						const bool bHasMoveCtor = FSolidMoveableStructRegistry::Get().IsStructMoveConstructible(ScriptStruct);

						const bool bHasIdentical = ScriptStruct->GetCppStructOps()->HasIdentical();
						
						Hooks.ctx = const_cast<UScriptStruct*>(ScriptStruct);  // NOLINT(cppcoreguidelines-pro-type-const-cast)
						
						if (bIsPOD)
						{
							Hooks.ctor = nullptr;
							Hooks.dtor = nullptr;
							Hooks.copy = nullptr;
							Hooks.move = nullptr;
						}
						else
						{
							if (bHasCtor)
							{
								Hooks.ctor = ScriptStructConstructor;
							}
							else
							{
								Hooks.ctor = nullptr;
							}
						
							if (bHasDtor)
							{
								Hooks.dtor = ScriptStructDestructor;
							}
							else
							{
								Hooks.dtor = nullptr;
							}

							if (bHasCopy)
							{
								Hooks.copy = ScriptStructCopy;
							}
							else
							{
								Hooks.copy = nullptr;
							}

							if (bHasMove)
							{
								Hooks.move = ScriptStructMove;
							}
							else
							{
								Hooks.move = nullptr;
							}

							if (bHasMoveCtor)
							{
								Hooks.move_ctor = ScriptStructMoveConstruct;
							}
							else
							{
								Hooks.move_ctor = nullptr;
							}

							if (!bHasCopy && !bHasMove)
							{
								ScriptStructComponent.Add(flecs::Sparse);
							
								UE_LOGFMT(LogFlecsComponent, Log,
									"Script struct {StructName} registered as Sparse component due to missing copy/move operations",
									ScriptStruct->GetName());
							}
						}

						if (!bHasIdentical && bIsPOD)
						{
							Hooks.equals = ScriptStructEqualsSimple;
						}
						else
						{
							Hooks.equals = ScriptStructEquals;
						}

						// @TODO: Implement this
						Hooks.cmp = nullptr;
					});
				}
			}

			std::string StructNameStdString(StructNameCStr, StructName.Len());
			
			if (!flecs::_::g_type_to_impl_data.contains(StructNameStdString))
			{
				flecs::_::type_impl_data NewData;  // NOLINT(cppcoreguidelines-pro-type-member-init)
				NewData.s_set_values = true;
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = bIsTag ? 0 : ScriptStruct->GetStructureSize();
				NewData.s_alignment = bIsTag ? 0 : ScriptStruct->GetMinAlignment();
				NewData.s_allow_tag = bIsTag;
				NewData.s_enum_registered = false;
				
				flecs::_::g_type_to_impl_data.emplace(StructNameStdString, NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(StructNameStdString));
			flecs::_::type_impl_data& Data = flecs::_::g_type_to_impl_data.at(StructNameStdString);

			flecs_component_ids_set(World, Data.s_index, ScriptStructComponent);

			TypeMapComponent->ScriptStructMap.emplace(ScriptStruct, ScriptStructComponent);

			RegisterMemberProperties(ScriptStruct, ScriptStructComponent);
			FlecsLibrary::GetTypeRegisteredDelegate().Broadcast(ScriptStructComponent);
		});

		ScriptStructComponent.Set<FFlecsScriptStructComponent>({ ScriptStruct });

		SetScope(OldScope);
		return ScriptStructComponent;
}

FFlecsEntityHandle UFlecsWorld::RegisterScriptEnum(const UEnum* ScriptEnum) const
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

FFlecsEntityHandle UFlecsWorld::RegisterComponentEnumType(TSolidNotNull<const UEnum*> ScriptEnum) const
{
	const FFlecsId OldScope = ClearScope();

		solid_checkf(!TypeMapComponent->ScriptEnumMap.contains(FFlecsScriptEnumComponent(ScriptEnum)),
			TEXT("Script enum %s is already registered"), *ScriptEnum->GetName());

		FFlecsComponentHandle ScriptEnumComponent;

		const FString EnumName = ScriptEnum->GetName();
		const char* EnumNameCStr = StringCast<char>(*EnumName).Get();

		DeferEndLambda([this, ScriptEnum, &ScriptEnumComponent, &EnumNameCStr, &EnumName]()
		{
			ScriptEnumComponent = World.component(EnumNameCStr);
			solid_check(ScriptEnumComponent.IsValid());
			
			ScriptEnumComponent.GetEntity().set_symbol(EnumNameCStr);

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
					ScriptEnumComponent.AddConstant<uint8>(EnumValueName, static_cast<uint8>(EnumValue));
				}
				else
				{
					ScriptEnumComponent.AddConstant<uint64>(EnumValueName, static_cast<uint64>(EnumValue));
				}
			}

			std::string EnumNameStdString(EnumNameCStr, EnumName.Len());
			if (!flecs::_::g_type_to_impl_data.contains(EnumNameStdString))
			{
				flecs::_::type_impl_data NewData{};
				NewData.s_set_values = true;
				NewData.s_index = flecs_component_ids_index_get();
				NewData.s_size = sizeof(uint8);
				NewData.s_alignment = alignof(uint8);
				NewData.s_allow_tag = false;
				NewData.s_enum_registered = false;
				
				flecs::_::g_type_to_impl_data.emplace(EnumNameStdString, NewData);
			}

			solid_check(flecs::_::g_type_to_impl_data.contains(EnumNameStdString));
			
			auto& [s_set_values, s_index, s_size, s_alignment, s_allow_tag, s_enum_registered]
				= flecs::_::g_type_to_impl_data.at(EnumNameStdString);
			
			flecs_component_ids_set(World, s_index, ScriptEnumComponent);
			TypeMapComponent->ScriptEnumMap.emplace(ScriptEnum, ScriptEnumComponent);
		});

		ScriptEnumComponent.Set<FFlecsScriptEnumComponent>(FFlecsScriptEnumComponent(ScriptEnum));

		SetScope(OldScope);
		return ScriptEnumComponent;
}

FFlecsEntityHandle UFlecsWorld::RegisterScriptClassType(TSolidNotNull<UClass*> ScriptClass) const
{
	const FFlecsId OldScope = ClearScope();

	const FString ClassName = ScriptClass->GetPrefixCPP() + ScriptClass->GetName();

	if (HasScriptClass(ScriptClass))
	{
		UE_LOGFMT(LogFlecsWorld, Log,
			"Script class {ClassName} is already registered", ClassName);
		return GetScriptClassEntity(ScriptClass);
	}

	FFlecsEntityHandle ScriptClassEntity;

	const char* ClassNameCStr = StringCast<char>(*ClassName).Get();

	DeferEndLambda([this, ScriptClass, &ScriptClassEntity, ClassNameCStr, &ClassName]()
	{
		ScriptClassEntity = CreateEntity(ClassName);
		solid_check(ScriptClassEntity.IsValid());

		ScriptClassEntity.GetEntity().set_symbol(ClassNameCStr);
		TypeMapComponent->ScriptClassMap.emplace(ScriptClass, ScriptClassEntity);

		ScriptClassEntity.Set<FFlecsScriptClassComponent>(FFlecsScriptClassComponent(ScriptClass));

		//RegisterMemberProperties(ScriptClass, ScriptClassComponent);

		std::string ClassNameStdString(ClassNameCStr, ClassName.Len());

		if (!flecs::_::g_type_to_impl_data.contains(ClassNameStdString))
		{
			flecs::_::type_impl_data NewData;  // NOLINT(cppcoreguidelines-pro-type-member-init)]
			NewData.s_set_values = true;
			NewData.s_index = flecs_component_ids_index_get();
			NewData.s_size = 0;
			NewData.s_alignment = 0;
			NewData.s_allow_tag = true;
			NewData.s_enum_registered = false;
				
			flecs::_::g_type_to_impl_data.emplace(ClassNameStdString, NewData);
		}

		solid_check(flecs::_::g_type_to_impl_data.contains(ClassNameStdString));

		flecs::_::type_impl_data& Data = flecs::_::g_type_to_impl_data.at(ClassNameStdString);
			
		flecs_component_ids_set(World, Data.s_index, ScriptClassEntity);
		TypeMapComponent->ScriptClassMap.emplace(ScriptClass, ScriptClassEntity);
	});

	SetScope(OldScope);
		
	return ScriptClassEntity;
}

bool UFlecsWorld::HasScriptClass(const TSubclassOf<UObject> InClass) const
{
	solid_check(InClass);
		
	if (TypeMapComponent->ScriptClassMap.contains(FFlecsScriptClassComponent(InClass)))
	{
		const FFlecsId Component = TypeMapComponent->ScriptClassMap.at(FFlecsScriptClassComponent(InClass));
		return ecs_is_valid(World.c_ptr(), Component);
	}

	return false;
}

FFlecsEntityHandle UFlecsWorld::GetScriptClassEntity(const TSubclassOf<UObject> InClass) const
{
	solid_check(InClass);
		
	const FFlecsId Component = TypeMapComponent->ScriptClassMap.at(FFlecsScriptClassComponent(InClass));
	solid_checkf(IsAlive(Component), TEXT("Entity is not alive"));
		
	return FFlecsEntityHandle(World, Component);
}

FFlecsEntityHandle UFlecsWorld::RegisterComponentType(const TSolidNotNull<const UScriptStruct*> ScriptStruct) const
{
	solid_checkf(!IsDeferred(), TEXT("Cannot register component while deferred"));
	
	if (HasScriptStruct(ScriptStruct))
	{
		return GetScriptStructEntity(ScriptStruct);
	}

	return RegisterScriptStruct(ScriptStruct);
}

FFlecsEntityHandle UFlecsWorld::RegisterComponentType(const TSolidNotNull<const UEnum*> ScriptEnum) const
{
	solid_checkf(!IsDeferred(), TEXT("Cannot register component while deferred"));
	
	if (HasScriptEnum(ScriptEnum))
	{
		return GetScriptEnumEntity(ScriptEnum);
	}

	return RegisterScriptEnum(ScriptEnum);
}

void UFlecsWorld::RunPipeline(const FFlecsId InPipeline, const double DeltaTime) const
{
	solid_checkf(InPipeline.IsValid(), TEXT("Pipeline is not valid"));
	solid_checkf(IsAlive(InPipeline), TEXT("Pipeline entity is not alive"));
	
	World.run_pipeline(InPipeline, DeltaTime);
}

void UFlecsWorld::RandomizeTimers() const
{
	World.randomize_timers();
}

flecs::timer UFlecsWorld::CreateTimer(const FString& Name) const
{
	return World.timer(StringCast<char>(*Name).Get());
}

bool UFlecsWorld::HasEntityWithName(const FString& Name, FFlecsEntityHandle& OutEntity) const
{
	const FFlecsEntityHandle Entity = LookupEntity(Name);
	
	if (Entity.IsValid())
	{
		OutEntity = Entity;
		return true;
	}

	return false;
}

// @TODO: Optimize this to not use lookup
FFlecsEntityHandle UFlecsWorld::GetTagEntity(const FGameplayTag& Tag) const
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

FFlecsEntityHandle UFlecsWorld::CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord, const FString& Name) const
{
	const FFlecsEntityHandle Prefab = World.prefab(StringCast<char>(*Name).Get());
	solid_checkf(Prefab.IsPrefab(), TEXT("Entity is not a prefab"));
		
	InRecord.ApplyRecordToEntity(this, Prefab);
	Prefab.Set<FFlecsEntityRecord>(InRecord);
		
#if WITH_EDITOR

	if (!Name.IsEmpty())
	{
		Prefab.SetDocName(Name);
	}

#endif // WITH_EDITOR
		
	return Prefab;
}

FFlecsEntityHandle UFlecsWorld::CreatePrefab(const FString& Name) const
{
	return World.prefab(StringCast<char>(*Name).Get());
}

FFlecsEntityHandle UFlecsWorld::CreatePrefab(const TSolidNotNull<UClass*> InClass) const
{
	const FFlecsEntityHandle PrefabEntity = ObtainTypedEntity(InClass)
		.Add(flecs::Prefab);
		
	return PrefabEntity;
}

FFlecsEntityHandle UFlecsWorld::CreatePrefabWithRecord(const FFlecsEntityRecord& InRecord,
	const TSolidNotNull<UClass*> InClass) const
{
	const FFlecsEntityHandle PrefabEntity = CreatePrefab(InClass);
	solid_checkf(PrefabEntity.IsPrefab(), TEXT("Entity is not a prefab"));

	InRecord.ApplyRecordToEntity(this, PrefabEntity);
	PrefabEntity.Set<FFlecsEntityRecord>(InRecord);
		
	return PrefabEntity;
}

void UFlecsWorld::DestroyPrefab(const FFlecsEntityHandle& InPrefab) const
{
	solid_checkf(InPrefab.IsValid(), TEXT("Prefab entity is not valid"));
	InPrefab.Destroy();
}

bool UFlecsWorld::ShouldQuit() const
{
	return World.should_quit();
}

FFlecsId UFlecsWorld::ClearScope() const
{
	return World.set_scope(0);
}

FFlecsId UFlecsWorld::SetScope(const FFlecsId InScope) const
{
	return World.set_scope(InScope);
}

FFlecsEntityHandle UFlecsWorld::GetScope() const
{
	return World.get_scope();
}

bool UFlecsWorld::IsSupportedForNetworking() const
{
	return true;
}

void UFlecsWorld::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UFlecsWorld::IsNameStableForNetworking() const
{
	return true;
}

UFlecsWorldSubsystem* UFlecsWorld::GetContext() const
{
	return static_cast<UFlecsWorldSubsystem*>(World.get_ctx());
}

void UFlecsWorld::ShrinkWorld() const
{
	World.shrink();
}

int32 UFlecsWorld::DeleteEmptyTables(const double TimeBudgetSeconds,
                                     const uint16 ClearGeneration,
                                     const uint16 DeleteGeneration) const
{
	ecs_delete_empty_tables_desc_t Desc;
	Desc.clear_generation = ClearGeneration;
	Desc.delete_generation = DeleteGeneration;
	Desc.time_budget_seconds = TimeBudgetSeconds;
	
	return ecs_delete_empty_tables(World, &Desc);
}

FFlecsTypeMapComponent* UFlecsWorld::GetTypeMapComponent() const
{
	return static_cast<FFlecsTypeMapComponent*>(World.get_binding_ctx());
}

void UFlecsWorld::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	const TSolidNotNull<UFlecsWorld*> This = CastChecked<UFlecsWorld>(InThis);
	solid_check(IsValid(This));
	
	if UNLIKELY_IF(!This->TypeMapComponent || !This->bIsInitialized)
	{
		return;
	}

	ecs_exclusive_access_begin(This->World, "Garbage Collection ARO");
		
	This->World.query_builder<const FFlecsScriptStructComponent>() // 0
	    .with<FFlecsAddReferencedObjectsTrait>().src("$Component") //  1
	    .term_at(0).src("$Component") // 0
	    .with("$Component") // 2
		//.cache_kind(flecs::QueryCacheNone)
	    .each([&Collector, InThis](flecs::iter& Iter, size_t Index,
	                               const FFlecsScriptStructComponent& InScriptStructComponent)
	    {
		    const FFlecsEntityHandle Component = Iter.get_var("Component");
		    solid_check(Component.IsValid());

		    void* ComponentPtr = Iter.field_at(1, Index);
		    solid_check(ComponentPtr);

		    Collector.AddPropertyReferencesWithStructARO(InScriptStructComponent.ScriptStruct.Get(),
		                                    ComponentPtr, InThis);
	    	
	    });

	ecs_exclusive_access_end(This->World, false);
}
