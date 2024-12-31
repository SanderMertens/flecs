#include "UnrealFlecsEditor.h"
#include "UnrealFlecsEditorStyle.h"
#include "Widgets/EntityHandle/FlecsEntityHandlePinFactory.h"
#include "Widgets/EntityHandle/FlecsEntityHandlePropertyEditor.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsEditorModule"

void FUnrealFlecsEditorModule::StartupModule()
{
	FUnrealFlecsEditorStyle::Initialize();
	
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
    	&FUnrealFlecsEditorModule::RegisterExplorerMenuExtension));

	FPropertyEditorModule& PropertyEditorModule
		= FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("FlecsEntityHandle",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FFlecsEntityHandleCustomization::MakeInstance
			)
		);

	PropertyEditorModule.NotifyCustomizationModuleChanged();

	// FlecsEntityHandlePinFactory = MakeShared<FFlecsEntityHandlePinFactory>();
	// FEdGraphUtilities::RegisterVisualPinFactory(FlecsEntityHandlePinFactory);
}

void FUnrealFlecsEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("FlecsEntityHandle");

		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
	
	// if (FlecsEntityHandlePinFactory.IsValid())
	// {
	// 	FEdGraphUtilities::UnregisterVisualPinFactory(FlecsEntityHandlePinFactory);
	// 	FlecsEntityHandlePinFactory.Reset();
	// }
	
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

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealFlecsEditorModule, UnrealFlecsEditor)