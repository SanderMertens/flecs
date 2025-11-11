// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionWorldSubsystem.h"

#include "Engine/World.h"
#include "Misc/AutomationTest.h"

#include "Logs/FlecsCategories.h"

#include "Worlds/FlecsWorldSubsystem.h"
#include "FlecsCollectionTypes.h"
#include "FlecsCollectionDataAsset.h"
#include "FlecsCollectionInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionWorldSubsystem)

void UFlecsCollectionWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UFlecsCollectionWorldSubsystem::OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	Super::OnFlecsWorldInitialized(InWorld);

	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();

	InWorld->RegisterComponentType<FFlecsCollectionSubsystemSingleton>();

	InWorld->SetSingleton<FFlecsCollectionSubsystemSingleton>({this});

	FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionSlotTag>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionParametersComponent>();

	CollectionScopeEntity = FlecsWorld->CreateEntity("CollectionScope")
		.Add(flecs::Module);
	
	UE_LOG(LogFlecsCollections, Verbose, TEXT("UCollectionsModule registered"));
}

void UFlecsCollectionWorldSubsystem::Deinitialize()
{
	if (MAYBE_UNUSED const UFlecsWorld* FlecsWorld = GetFlecsWorld())
	{
		if LIKELY_IF(CollectionScopeEntity.IsValid())
		{
			CollectionScopeEntity.Destroy();
		}
	}
		
	Super::Deinitialize();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionAsset(const UFlecsCollectionDataAsset* InAsset)
{
	solid_cassume(InAsset);

	const FFlecsCollectionId Id = FFlecsCollectionId(InAsset->GetFName());
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}

	const FFlecsEntityHandle Prefab = CreatePrefabEntity(InAsset->GetName(), InAsset->Record);
	RegisteredCollections.Add(Id, Prefab);

	for (const FFlecsCollectionReference& Reference : InAsset->Collections)
	{
		const FFlecsEntityHandle Resolved = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!Resolved.IsValid())
		{
			UE_LOGFMT(LogFlecsCollections, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionAsset: Failed to resolve collection reference on asset '{Asset}'",
				InAsset->GetName());
			continue;
		}

		Prefab.AddPair(flecs::IsA, Resolved);
	}
	
	ExpandChildCollectionReferences(Prefab);

	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionDefinition(
	const FName& InName, 
	const FFlecsCollectionDefinition& InDefinition)
{
	const FFlecsCollectionId Id = FFlecsCollectionId(InName);
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const FFlecsEntityHandle Prefab = CreatePrefabEntity(Id.NameId.ToString(), InDefinition.Record);
	RegisteredCollections.Add(Id, Prefab);
	
	for (const FFlecsCollectionReference& Reference : InDefinition.Collections)
	{
		const FFlecsEntityHandle ResolvedEntityHandle = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!ResolvedEntityHandle.IsValid())
		{
			UE_LOGFMT(LogFlecsCollections, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionDefinition: Failed to resolve collection reference on definition '{NameId}'",
				Id.NameId.ToString());
			continue;
		}

		Prefab.AddPair(flecs::IsA, ResolvedEntityHandle);
	}
	
	ExpandChildCollectionReferences(Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionClass(const TSolidNotNull<UClass*> InClass,
	const FFlecsCollectionBuilder& InBuilder)
{
	const FString CPPClassName = InClass->GetPrefixCPP() + InClass->GetName();
	const FFlecsCollectionId Id = FFlecsCollectionId(CPPClassName);
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const FFlecsEntityHandle Prefab
		= CreatePrefabEntity(CPPClassName, InBuilder.GetCollectionDefinition().Record);
	
	RegisteredCollections.Add(Id, Prefab);
	
	for (const FFlecsCollectionReference& Reference : InBuilder.GetCollectionDefinition().Collections)
	{
		const FFlecsEntityHandle ResolvedEntityHandle = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!ResolvedEntityHandle.IsValid())
		{
			UE_LOGFMT(LogFlecsCollections, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionClass: Failed to resolve collection reference on class '{Class}'",
				InClass->GetName());
			continue;
		}

		Prefab.AddPair(flecs::IsA, ResolvedEntityHandle);
	}
	
	ExpandChildCollectionReferences(Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionInterfaceClass(
	const TSolidNotNull<UClass*> InInterfaceObject)
{
	solid_checkf(InInterfaceObject->ImplementsInterface(UFlecsCollectionInterface::StaticClass()),
		TEXT("Class %s does not implement FlecsCollectionInterface"), *InInterfaceObject->GetName());
	
	const TSolidNotNull<UObject*> DefaultObject = InInterfaceObject->GetDefaultObject();
	const TScriptInterface<IFlecsCollectionInterface> InInterfaceObjectTyped = DefaultObject;
	
	FFlecsCollectionDefinition Definition;
	
	FFlecsCollectionBuilder CollectionBuilder = FFlecsCollectionBuilder::Create(Definition);
	
	// @TODO: how safe is this to be called on a CDO?
	InInterfaceObjectTyped.GetInterface()->BuildCollection(CollectionBuilder);

	return RegisterCollectionClass(InInterfaceObjectTyped.GetObject()->GetClass(), CollectionBuilder);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByAsset(const UFlecsCollectionDataAsset* Asset) const
{
	solid_cassume(Asset);
	
	const FFlecsCollectionId Id = FFlecsCollectionId(Asset->GetFName());
	return GetPrefabByCollectionId(Id);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByIdRaw(const FFlecsId& Id) const
{
	const FFlecsEntityHandle CollectionPrefabHandle = GetFlecsWorldChecked()->GetAlive(Id);
	
	if UNLIKELY_IF(!CollectionPrefabHandle.IsValid() || !CollectionPrefabHandle.Has<FFlecsCollectionPrefabTag>())
	{
		UE_LOGFMT(LogFlecsCollections, Warning,
			"UFlecsCollectionWorldSubsystem::GetPrefabByIdRaw: Id '{EntityId}' is not a valid collection prefab",
			Id.ToString());
		return FFlecsEntityHandle();
	}

	return CollectionPrefabHandle;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByCollectionId(const FFlecsCollectionId& Id) const
{
	if LIKELY_IF(const FFlecsEntityHandle* Found = RegisteredCollections.Find(Id))
	{
		return *Found;
	}
	
	return FFlecsEntityHandle();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByClass(const TSubclassOf<UObject> InClass) const
{
	solid_cassume(InClass);
	
	const FFlecsCollectionId Id = FFlecsCollectionId(InClass->GetFName());
	return GetPrefabByCollectionId(Id);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetCollectionScope() const
{
	return CollectionScopeEntity;
}

void UFlecsCollectionWorldSubsystem::AddCollectionToEntity(const FFlecsEntityHandle& InEntity,
	const FFlecsId InCollectionId, const FInstancedStruct& InParameters)
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: InEntity is invalid"));

	solid_checkf(InCollectionId.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: InCollectionId is invalid"));

	const FFlecsEntityHandle CollectionPrefab = GetPrefabByIdRaw(InCollectionId);

	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: CollectionId '%s' is not registered"),
		*InCollectionId.ToString());
	solid_checkf(CollectionPrefab.Has<FFlecsCollectionPrefabTag>(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: CollectionId '%s' is not a collection prefab"),
		*InCollectionId.ToString());

	InEntity.AddPair(flecs::IsA, CollectionPrefab);
	ApplyCollectionParametersToEntity(InEntity, CollectionPrefab, InParameters);
}

void UFlecsCollectionWorldSubsystem::AddCollectionToEntity(const FFlecsEntityHandle& InEntity,
                                                           const FFlecsCollectionId& InCollectionId, const FInstancedStruct& InParameters)
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: InEntity is invalid"));
	
	solid_checkf(IsCollectionRegistered(InCollectionId),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: CollectionId '%s' is not registered"),
		*InCollectionId.NameId.ToString());

	const FFlecsEntityHandle CollectionPrefab = GetPrefabByCollectionId(InCollectionId);
	
	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: CollectionId '%s' is not registered"),
		*InCollectionId.NameId.ToString());

	InEntity.AddPair(flecs::IsA, CollectionPrefab);

	ApplyCollectionParametersToEntity(InEntity, CollectionPrefab, InParameters);
}

void UFlecsCollectionWorldSubsystem::AddCollectionToEntity(const FFlecsEntityHandle& InEntity,
	const FFlecsCollectionReference& InCollectionReference, const FInstancedStruct& InParameters)
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: InEntity is invalid"));

	const FFlecsEntityHandle CollectionPrefab = ResolveCollectionReference(InCollectionReference);
	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::AddCollectionToEntity: Failed to resolve collection reference"));

	InEntity.AddPair(flecs::IsA, CollectionPrefab);

	ApplyCollectionParametersToEntity(InEntity, CollectionPrefab, InParameters);
}

void UFlecsCollectionWorldSubsystem::AddCollectionToEntity(const FFlecsEntityHandle& InEntity,
	const TSolidNotNull<const UFlecsCollectionDataAsset*> InAsset, const FInstancedStruct& InParameters)
{
	AddCollectionToEntity(InEntity, FFlecsCollectionReference::FromAsset(InAsset), InParameters);
}

void UFlecsCollectionWorldSubsystem::AddCollectionToEntity(const FFlecsEntityHandle& InEntity,
	const TSubclassOf<UObject> InClass, const FInstancedStruct& InParameters)
{
	solid_cassume(InClass);

	solid_checkf(InClass->ImplementsInterface(UFlecsCollectionInterface::StaticClass()),
		TEXT("Class %s does not implement FlecsCollectionInterface"), *InClass->GetName());
	
	AddCollectionToEntity(InEntity, FFlecsCollectionReference::FromClass(InClass), InParameters);
}

void UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity,
                                                                const FFlecsCollectionId& InCollectionId)
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity: InEntity is invalid"));
	solid_checkf(IsCollectionRegistered(InCollectionId),
		TEXT("UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity: CollectionId '%s' is not registered"),
		*InCollectionId.NameId.ToString());

	const FFlecsEntityHandle CollectionPrefab = GetPrefabByCollectionId(InCollectionId);
	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity: CollectionId '%s' is not registered"),
		*InCollectionId.NameId.ToString());

	InEntity.RemovePair(flecs::IsA, CollectionPrefab);
}

void UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity,
	const FFlecsCollectionReference& InCollectionReference)
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity: InEntity is invalid"));

	const FFlecsEntityHandle CollectionPrefab = ResolveCollectionReference(InCollectionReference);
	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity: Failed to resolve collection reference"));

	InEntity.RemovePair(flecs::IsA, CollectionPrefab);
}

void UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity,
	const TSolidNotNull<const UFlecsCollectionDataAsset*> InAsset)
{
	RemoveCollectionFromEntity(InEntity, FFlecsCollectionReference::FromAsset(InAsset));
}

void UFlecsCollectionWorldSubsystem::RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity,
	const TSubclassOf<UObject> InClass)
{
	solid_cassume(InClass);

	solid_checkf(InClass->ImplementsInterface(UFlecsCollectionInterface::StaticClass()),
		TEXT("Class %s does not implement FlecsCollectionInterface"), *InClass->GetName());

	RemoveCollectionFromEntity(InEntity, FFlecsCollectionReference::FromClass(InClass));
}

bool UFlecsCollectionWorldSubsystem::HasCollection(const FFlecsEntityHandle& InEntity,
                                                   const FFlecsCollectionId& InCollectionId) const
{
	const FFlecsEntityHandle CollectionPrefab = GetPrefabByCollectionId(InCollectionId);
	
	solid_checkf(CollectionPrefab.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::HasCollection: CollectionId '%s' is not registered"),
		*InCollectionId.NameId.ToString());

	return InEntity.HasPair(flecs::IsA, CollectionPrefab);
}

bool UFlecsCollectionWorldSubsystem::IsCollectionRegistered(const FFlecsCollectionId& Id) const
{
	return RegisteredCollections.Contains(Id);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::EnsurePrefabShell(const FFlecsCollectionId& Id)
{
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreateEntity(Id.NameId.ToString())
		.SetParent(CollectionScopeEntity)
		.Add(flecs::Prefab)
		.Add<FFlecsCollectionPrefabTag>();
	
	RegisteredCollections.Add(Id, Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::ResolveCollectionReference(const FFlecsCollectionReference& Reference)
{
	switch (Reference.Mode)
	{
		case EFlecsCollectionReferenceMode::Asset:
			{
				if LIKELY_IF(ensureMsgf(IsValid(Reference.Asset),
					TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Asset is Invalid")))
				{
					return RegisterCollectionAsset(Reference.Asset);
				}

				return FFlecsEntityHandle();
			}
		
		case EFlecsCollectionReferenceMode::Id:
			{
				if LIKELY_IF(const FFlecsEntityHandle* Found = RegisteredCollections.Find(Reference.Id))
				{
					return *Found;
				}
				
				UE_LOGFMT(LogFlecsCollections, Error,
					"UFlecsCollectionWorldSubsystem::ResolveReference: Id '{NameId}' not registered",
					Reference.Id.NameId);
				
				return FFlecsEntityHandle();
			}
		
		case EFlecsCollectionReferenceMode::UClass:
			{
				if LIKELY_IF(ensureMsgf(Reference.Class,
					TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Class is null")))
				{
					const FFlecsEntityHandle ClassPrefab = GetPrefabByClass(Reference.Class);
					
					if LIKELY_IF(ensureMsgf(ClassPrefab.IsValid(),
						TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Class '{Class}' not registered"),
							*Reference.Class->GetName()))
					{
						return ClassPrefab;
					}
				}
				
				return FFlecsEntityHandle();
			}
	}

	return FFlecsEntityHandle();
}

void UFlecsCollectionWorldSubsystem::ExpandChildCollectionReferences(const FFlecsEntityHandle& InCollectionEntity)
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();

	FlecsWorld->Defer([this, FlecsWorld, &InCollectionEntity]()
	{
		FlecsWorld->World.query_builder<FFlecsCollectionReferenceComponent*>() // 0 (FFlecsCollectionReferenceComponent)
			.with(flecs::ChildOf, InCollectionEntity) // 1
			.with<FFlecsCollectionSlotTag>().optional() // 2
			.each([&](flecs::iter& Iter, size_t Index, FFlecsCollectionReferenceComponent* ReferenceComponent) // 3
			{
				const FFlecsEntityHandle ChildEntityHandle = Iter.entity(Index);

				for (const FFlecsCollectionReference& CollectionReference : ReferenceComponent->Collections)
				{
					if (ReferenceComponent)
					{
						const FFlecsEntityHandle Resolved = ResolveCollectionReference(CollectionReference);
					
						if UNLIKELY_IF(!Resolved.IsValid())
						{
							UE_LOGFMT(LogFlecsCollections, Error,
								"UFlecsCollectionWorldSubsystem::ExpandChildCollectionReferences: Failed to resolve collection reference on entity '{Entity}'",
								ChildEntityHandle.GetPath());
							return;
						}

						ChildEntityHandle.AddPair(flecs::IsA, Resolved);
					}
				}

				if (Iter.is_set(2))
				{
					ChildEntityHandle.MarkSlot();
					ChildEntityHandle.Remove<FFlecsCollectionSlotTag>();
				}

				ExpandChildCollectionReferences(ChildEntityHandle);
			});
	});
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::CreatePrefabEntity(const FString& Name, const FFlecsEntityRecord& Record) const
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreatePrefabWithRecord(TInstancedStruct<FFlecsEntityRecord>::Make(Record), Name)
		.Add<FFlecsCollectionPrefabTag>();

	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::CreatePrefabEntity(const TSolidNotNull<UClass*> InClass,
	const FFlecsEntityRecord& Record) const
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreatePrefabWithRecord(TInstancedStruct<FFlecsEntityRecord>::Make(Record), InClass->GetName())
		.Add<FFlecsCollectionPrefabTag>();

	return Prefab;
}

void UFlecsCollectionWorldSubsystem::ApplyCollectionParametersToEntity(const FFlecsEntityHandle& InEntity,
	const FFlecsEntityHandle& InCollectionEntity, const FInstancedStruct& InParameters) const
{
	solid_checkf(InEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::ApplyCollectionParametersToEntity: InEntity is invalid"));
	solid_checkf(InCollectionEntity.IsValid(),
		TEXT("UFlecsCollectionWorldSubsystem::ApplyCollectionParametersToEntity: InCollectionEntity is invalid"));

	if (!InCollectionEntity.Has<FFlecsCollectionParametersComponent>())
	{
		return;
	}

	const bool bHasInputParameters = InParameters.IsValid();

	const FFlecsCollectionParametersComponent& ParametersComponent 
		= InCollectionEntity.Get<FFlecsCollectionParametersComponent>();

	if UNLIKELY_IF(!ParametersComponent.ParameterType.IsValid())
	{
		UE_LOGFMT(LogFlecsCollections, Warning,
			"UFlecsCollectionWorldSubsystem::ApplyCollectionParametersToEntity: Collection entity '{Entity}' has invalid ParametersType",
			InCollectionEntity.GetFlecsId().ToString());
		return;
	}

	FInstancedStruct UsedParameters;

	if (bHasInputParameters)
	{
		if UNLIKELY_IF(InParameters.GetScriptStruct() != ParametersComponent.ParameterType.GetScriptStruct())
		{
			UE_LOGFMT(LogFlecsCollections, Error,
				"UFlecsCollectionWorldSubsystem::ApplyCollectionParametersToEntity: Parameter type mismatch on entity '{Entity}'. Expected '{Expected}', got '{Got}'",
				InCollectionEntity.GetFlecsId().ToString(),
				*ParametersComponent.ParameterType.GetScriptStruct()->GetStructCPPName(),
				*InParameters.GetScriptStruct()->GetStructCPPName());
			return;
		}

		UsedParameters = InParameters;
	}
	else
	{
		UsedParameters = ParametersComponent.ParameterType;
	}

	ParametersComponent.ApplyParameters(InEntity, UsedParameters);
}

bool UFlecsCollectionWorldSubsystem::ClassImplementsCollectionInterface(const TSolidNotNull<const UClass*> InClass) const
{
	return InClass->ImplementsInterface(UFlecsCollectionInterface::StaticClass());
}
