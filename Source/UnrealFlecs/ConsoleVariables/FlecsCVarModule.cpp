// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsCVarModule.h"
#include "ConsoleVariableComponents.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCVarModule)

UNLOG_CATEGORY(LogFlecsCVarModule);

void UFlecsCVarModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	
	ConsoleManager.ForEachConsoleObjectThatStartsWith(
		FConsoleObjectVisitor::CreateLambda([&, this](const TCHAR* Name, IConsoleObject* CVar)
		{
			RegisterCVar(Name, CVar);
		}
	), TEXT(""));

	GetFlecsWorld()->CreateObserver(TEXT("CVarValueObserver"))
		.event(flecs::OnSet)
		.with<FFlecsCVarComponent>("$CVarValue")
		.with(flecs::I32).var("$CVarValue")
		.or_().with(flecs::F32).var("$CVarValue")
		.or_().with(flecs::String).var("$CVarValue")
		.or_().with(flecs::Bool).var("$CVarValue")
		.each([&](flecs::entity CVarEntity)
		{
			if (bOutsideChange)
			{
				bOutsideChange = false;
				return;
			}

			solid_checkf(CVarEntity.has<FFlecsCVarComponent>(flecs::Wildcard),
				TEXT("CVar entity missing CVar component"));

			IConsoleVariable* CVar = ConsoleManager.FindConsoleVariable(StringCast<TCHAR>(CVarEntity.name().c_str()).Get());
			solid_checkf(CVar, TEXT("CVar not found for entity %s"), StringCast<TCHAR>(CVarEntity.name().c_str()).Get());
			
			if (CVar->IsVariableInt())
			{
				CVar->Set(*CVarEntity.get<FFlecsCVarComponent, int32>());
			}
			else if (CVar->IsVariableFloat())
			{
				CVar->Set(*CVarEntity.get<FFlecsCVarComponent, float>());
			}
			else if (CVar->IsVariableString())
			{
				CVar->Set(CVarEntity.get<FFlecsCVarComponent, FString>()->GetCharArray().GetData());
			}
			else if (CVar->IsVariableBool())
			{
				CVar->Set(*CVarEntity.get<FFlecsCVarComponent, bool>());
			}
			else
			{
				UN_LOGF(LogFlecsCVarModule, Error,
					"Unknown CVar type for %s", *CVarEntity.name().c_str());
			}

			bOutsideChange = false;
		});
}

void UFlecsCVarModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}

void UFlecsCVarModule::RegisterCVar(const TCHAR* Name, IConsoleObject* CVar)
{
	GetFlecsWorld()->Scope(GetModuleEntity().GetId(), [&]()
	{
		if (CVar->AsVariable())
		{
			FFlecsEntityHandle CVarEntity = GetFlecsWorld()->CreateEntity(Name);

			#if WITH_EDITORONLY_DATA

			CVarEntity.SetDocDetails(CVar->GetHelp());

			#endif // WITH_EDITOR

			if (CVar->AsVariable()->IsVariableInt())
			{
				CVarEntity.SetPair<FFlecsCVarComponent, int32>(CVar->AsVariable()->GetInt());
			}
			else if (CVar->AsVariable()->IsVariableFloat())
			{
				CVarEntity.SetPair<FFlecsCVarComponent, float>(CVar->AsVariable()->GetFloat());
			}
			else if (CVar->AsVariable()->IsVariableString())
			{
				CVarEntity.SetPair<FFlecsCVarComponent, FString>(CVar->AsVariable()->GetString());
			}
			else if (CVar->AsVariable()->IsVariableBool())
			{
				CVarEntity.SetPair<FFlecsCVarComponent, bool>(CVar->AsVariable()->GetBool());
			}
			else
			{
				UN_LOGF(LogFlecsCVarModule, Error, "Unknown CVar type for %s", Name);
				return;
			}

			CVar->AsVariable()->SetOnChangedCallback(
				FConsoleVariableDelegate::CreateLambda([&, CVarEntity](IConsoleVariable* ChangedVar)
				{
					bOutsideChange = true;
					
					if (ChangedVar->IsVariableInt())
					{
						CVarEntity.SetPair<FFlecsCVarComponent, int32>(ChangedVar->GetInt());
					}
					else if (ChangedVar->IsVariableFloat())
					{
						CVarEntity.SetPair<FFlecsCVarComponent, float>(ChangedVar->GetFloat());
					}
					else if (ChangedVar->IsVariableString())
					{
						CVarEntity.SetPair<FFlecsCVarComponent, FString>(ChangedVar->GetString());
					}
					else if (ChangedVar->IsVariableBool())
					{
						CVarEntity.SetPair<FFlecsCVarComponent, bool>(ChangedVar->GetBool());
					}
					else
					{
						UN_LOGF(LogFlecsCVarModule, Error, "Unknown CVar type for %s", Name);
						bOutsideChange = false;
						return;
					}
				}
			));
		}
	});
}
