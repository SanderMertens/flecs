// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "UnrealFlecsEditor.h"


#include "Engine/AssetManagerSettings.h"
#include "Engine/AssetManagerTypes.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"

#include "General/FlecsEditorDeveloperSettings.h"
#include "Logs/FlecsEditorCategory.h"

#include "UnrealFlecsEditorStyle.h"
#include "Widgets/EntityHandle/FlecsIdCustomization.h"
#include "Widgets/EntityHandle/FlecsIdPinFactory.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlecsEditor, Log, All);

#define LOCTEXT_NAMESPACE "FUnrealFlecsEditorModule"

void FUnrealFlecsEditorModule::StartupModule()
{
	FUnrealFlecsEditorStyle::Initialize();
	
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
    	&FUnrealFlecsEditorModule::RegisterExplorerMenuExtension));

	FPropertyEditorModule& PropertyEditorModule
		= FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("FlecsId",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FFlecsIdCustomization::MakeInstance
			)
		);

	AddPrimaryAssetTypes();

	PropertyEditorModule.NotifyCustomizationModuleChanged();
	
	 FlecsIdPinFactory = MakeShared<FFlecsIdPinFactory>();
	 FEdGraphUtilities::RegisterVisualPinFactory(FlecsIdPinFactory);
}

void FUnrealFlecsEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("FlecsId");

		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
	
	 if (FlecsIdPinFactory.IsValid())
	 {
	 	FEdGraphUtilities::UnregisterVisualPinFactory(FlecsIdPinFactory);
	 	FlecsIdPinFactory.Reset();
	 }
	
	FUnrealFlecsEditorStyle::Shutdown();
	
    UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void FUnrealFlecsEditorModule::RegisterExplorerMenuExtension()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.ModesToolBar");

	FToolMenuSection& Section = Menu->FindOrAddSection("Content");
	
	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"OpenFlecsExplorer", FUIAction(
			FExecuteAction::CreateLambda([]()
			{
				if (!ensure(GEditor))
				{
					return;
				}
				
				TOptional<FPlayInEditorSessionInfo> PIEInfo = GEditor->GetPlayInEditorSessionInfo();

				const UFlecsEditorDeveloperSettings* FlecsEditorDeveloperSettings = GetDefault<UFlecsEditorDeveloperSettings>();

				if UNLIKELY_IF(!ensureMsgf(IsValid(FlecsEditorDeveloperSettings),
					TEXT("Failed to get Flecs Editor Developer Settings.")))
				{
					return;
				}
				
				if (!PIEInfo.IsSet())
				{
					UE_LOG(LogFlecsEditor, Log, TEXT("No PIE session info found"));

					const FString TargetUrl = FlecsEditorDeveloperSettings->GetFlecsExplorerURL().ToURLString();
					
					// TEXT("Failed to launch Flecs Explorer URL. Explorer Instance 0")
					
					FPlatformProcess::LaunchURL(*TargetUrl, nullptr, nullptr);
					return;
				}
				
				for (int32 Index = 0; Index < PIEInfo->PIEInstanceCount; ++Index)
				{
					FString TargetUrl = FlecsEditorDeveloperSettings->GetFlecsExplorerURL().ToURLString(Index);
					
					FPlatformProcess::LaunchURL(*TargetUrl, nullptr, nullptr);
				}
			})
		),
		INVTEXT("Open Flecs Explorer (for each PIE instance)"),
		INVTEXT("Open Flecs Explorer (for each PIE instance)"),
		FSlateIcon("UnrealFlecsEditorStyle", "UnrealFlecs.FlecsEditor.FlecsLogo")
	));
}

void FUnrealFlecsEditorModule::AddPrimaryAssetTypes()
{
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	
	if UNLIKELY_IF(!ensureMsgf(Settings, TEXT("Failed to get Asset Manager Settings.")))
	{
		return;
	}

	bool bModified = false;
	
	/*FlecsPrimaryDataAssetTypeInfo.Rules.Priority = 1;
	FlecsPrimaryDataAssetTypeInfo.Rules.bApplyRecursively = true;
	FlecsPrimaryDataAssetTypeInfo.Rules.CookRule = EPrimaryAssetCookRule::AlwaysCook;

	// Check if these asset types are already present
	auto IsAssetTypePresent = [&](const FPrimaryAssetTypeInfo& TypeInfo) -> bool
	{
		return Settings->PrimaryAssetTypesToScan.ContainsByPredicate([&](const FPrimaryAssetTypeInfo& Info)
		{
			return Info.PrimaryAssetType == TypeInfo.PrimaryAssetType;
		});
	};

	if (!IsAssetTypePresent(FlecsPrimaryDataAssetTypeInfo))
	{
		Settings->PrimaryAssetTypesToScan.Add(FlecsPrimaryDataAssetTypeInfo);
		bModified = true;
	}

	if (bModified)
	{
		Settings->SaveConfig();
		UE_LOG(LogFlecsEditor, Log,
			TEXT("Added Flecs asset types to PrimaryAssetTypesToScan."));
		
		FNotificationInfo Info(LOCTEXT("FlecsAssetTypesAdded",
			"Flecs asset types have been added to Asset Manager settings."));
		Info.ExpireDuration = 3.0f;
		
		FSlateNotificationManager::Get().AddNotification(Info);
	}*/
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealFlecsEditorModule, UnrealFlecsEditor)