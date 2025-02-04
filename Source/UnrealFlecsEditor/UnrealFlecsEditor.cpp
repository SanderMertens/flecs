// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "UnrealFlecsEditor.h"
#include "UnrealFlecsEditorStyle.h"
#include "Engine/AssetManagerSettings.h"
#include "Engine/AssetManagerTypes.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Logs/FlecsEditorCategory.h"
#include "Widgets/EntityHandle/FlecsIdPinFactory.h"
#include "Widgets/EntityHandle/FlecsIdPropertyEditor.h"
#include "Widgets/Notifications/SNotificationList.h"

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
	 //FEdGraphUtilities::RegisterVisualPinFactory(FlecsIdPinFactory);
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
	 	//FEdGraphUtilities::UnregisterVisualPinFactory(FlecsIdPinFactory);
	 	//FlecsIdPinFactory.Reset();
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
				
				if (!PIEInfo.IsSet())
				{
					FFlecsEditorLog::Warn<LogFlecsEditor>("No PIE instances found");
					return;
				}
				
				for (int32 Index = 0; Index < PIEInfo->NumClientInstancesCreated; ++Index)
				{
					FString TargetUrl = "https://www.flecs.dev/explorer/?host=localhost:";
					TargetUrl.Append(FString::FromInt(27750 + Index));
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

	FPrimaryAssetTypeInfo FlecsPrimaryDataAssetTypeInfo(
		FName("FlecsPrimaryDataAsset"),
		UFlecsPrimaryDataAsset::StaticClass(),
		false,
		false
	);

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
		UN_LOG(LogFlecsEditor, Log, "Added Flecs asset types to PrimaryAssetTypesToScan.");
		
		FNotificationInfo Info(LOCTEXT("FlecsAssetTypesAdded",
			"Flecs asset types have been added to Asset Manager settings."));
		Info.ExpireDuration = 3.0f;
		
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealFlecsEditorModule, UnrealFlecsEditor)