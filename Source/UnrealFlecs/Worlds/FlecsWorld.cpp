// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorld.h"
#include "FlecsWorldSubsystem.h"
#include "Components/FlecsNetworkSerializeDefinitionComponent.h"
#include "Components/ObjectTypes/FFlecsSceneComponentTag.h"
#include "Components/ObjectTypes/FlecsActorTag.h"

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

UFlecsWorld::UFlecsWorld(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	const FString ObjectName = GetName();
	char* argv[] = { const_cast<char*>(Unreal::Flecs::ToCString(ObjectName)) }; // NOLINT(clang-diagnostic-dangling, cppcoreguidelines-pro-type-const-cast)
		
	World = flecs::world(1, argv);
		
	TypeMapComponent = GetTypeMapComponent();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
	solid_check(TypeMapComponent);
}

UFlecsWorld::~UFlecsWorld()
{
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

	FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);
}

void UFlecsWorld::WorldStart()
{
	UE_LOG(LogFlecsWorld, Log, TEXT("Flecs World start"));

	bIsInitialized = true;
		
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
			     Unreal::Flecs::ToCString(Data->ToString()),
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
		
	RegisterComponentType<FFlecsModuleComponentTag>()
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

void UFlecsWorld::RegisterModuleDependency(const TSolidNotNull<const UObject*> InModuleObject,
	const TSubclassOf<UFlecsModuleInterface>& InDependencyClass,
	const std::function<void(TSolidNotNull<UObject*>, TSolidNotNull<UFlecsWorld*>, FFlecsEntityHandle)>& InFunction)
{
	solid_check(InModuleObject->Implements<UFlecsModuleInterface>());

	solid_checkf(IsModuleImported(InModuleObject->GetClass()),
	             TEXT("Module %s is not imported"), *InModuleObject->GetClass()->GetName());

	const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModuleObject->GetClass());
	solid_check(ModuleEntity.IsValid());

	ModuleEntity.Add<FFlecsDependenciesComponent>();
		
	auto& [Dependencies] = ModuleEntity.GetMut<FFlecsDependenciesComponent>();
		
	Dependencies.emplace(InDependencyClass, InFunction);
		
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
	return World.entity(Unreal::Flecs::ToCString(Name));
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithId(const FFlecsId InId) const
{
	solid_checkf(!IsAlive(InId), TEXT("Entity with ID %s is already alive"), *InId.ToString());
	return MakeAlive(InId);
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithPrefab(const FFlecsEntityHandle& InPrefab) const
{
	return CreateEntity().SetIsA(InPrefab);
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithRecord(const FFlecsEntityRecord& InRecord, const FString& Name) const
{
	const FFlecsEntityHandle Entity = CreateEntity(Name);
	InRecord.ApplyRecordToEntity(Entity);
	return Entity;
}

FFlecsEntityHandle UFlecsWorld::CreateEntityWithRecordWithId(const FFlecsEntityRecord& InRecord,
	const FFlecsId InId) const
{
	const FFlecsEntityHandle Entity = CreateEntityWithId(InId);
	InRecord.ApplyRecordToEntity(Entity);
	return Entity;
}

FFlecsEntityHandle UFlecsWorld::LookupEntity(const FString& Name,
	const FString& Separator, const FString& RootSeparator,
	const bool bRecursive) const
{
	return World.lookup(Unreal::Flecs::ToCString(Name),
	                    Unreal::Flecs::ToCString(Separator),
	                    Unreal::Flecs::ToCString(RootSeparator),
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
	return FString(GetWorldEntity().GetName());
}

void UFlecsWorld::SetWorldName(const FString& InName) const
{
	GetWorldEntity().SetName(InName);
}

void UFlecsWorld::ImportModule(const TScriptInterface<IFlecsModuleInterface>& InModule)
{
	solid_checkf(InModule, TEXT("Module is nullptr"));

	// Doesn't use TSolidNotNull because DuplicateObject work weird with it
	const UObject* TemplateModuleObject = InModule.GetObject();
	solid_check(IsValid(TemplateModuleObject));

	TSolidNotNull<UObject*> NewModuleObject = DuplicateObject(TemplateModuleObject, this);
	ImportedModules.Add(NewModuleObject);
		
	ImportedModules.Last()->ImportModule(World);
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

	return ModuleEntity;
}

UObject* UFlecsWorld::GetModule(const TSubclassOf<UObject> InModule, const bool bAllowChildren) const
{
	solid_check(IsValid(InModule));
		
	const FFlecsEntityHandle ModuleEntity = GetModuleEntity(InModule, bAllowChildren);
	solid_checkf(ModuleEntity.IsValid(),
	             TEXT("Module %s is not imported"), *InModule->GetName());
	
	return ModuleEntity.GetPairFirst<FFlecsUObjectComponent, FFlecsModuleComponentTag>().GetObjectChecked();
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
		return;
	}

	FCoreUObjectDelegates::GarbageCollectComplete.RemoveAll(this);

	ModuleComponentQuery.destruct();
	DependenciesComponentQuery.destruct();
	ObjectComponentQuery.destruct();
		
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

void UFlecsWorld::SetTaskThreads(const int32 InThreadCount) const
{
	World.set_task_threads(InThreadCount);
}

UFlecsWorldSubsystem* UFlecsWorld::GetContext() const
{
	return static_cast<UFlecsWorldSubsystem*>(World.get_ctx());
}

void UFlecsWorld::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	const TSolidNotNull<UFlecsWorld*> This = CastChecked<UFlecsWorld>(InThis);

	if UNLIKELY_IF(!This->TypeMapComponent || !This->bIsInitialized)
	{
		return;
	}
		
	This->World.query_builder<const FFlecsScriptStructComponent>()
	    .with<FFlecsAddReferencedObjectsTrait>().src("$Component") // Term 1
	    .term_at(0).src("$Component") // Term 0
	    .with("$Component") // Term 2
	    .each([&Collector, InThis](flecs::iter& Iter, size_t Index,
	                               const FFlecsScriptStructComponent& InScriptStructComponent)
	    {
		    const FFlecsEntityHandle Component = Iter.get_var("$Component");
		    solid_check(Component.IsValid());

		    void* ComponentPtr = Iter.field_at(1, Index);
		    solid_check(ComponentPtr);

		    Collector.AddPropertyReferencesWithStructARO(InScriptStructComponent.ScriptStruct.Get(),
		                                    ComponentPtr, InThis);
	    });
}
