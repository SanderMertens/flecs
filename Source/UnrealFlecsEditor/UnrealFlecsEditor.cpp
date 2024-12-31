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
		"OpenFlecsExplorer",
		FUIAction(
			FExecuteAction::CreateLambda([]()
			{
				if (!ensure(GEditor))
				{
					return;
				}
				
				TOptional<FPlayInEditorSessionInfo> PIEInfo = GEditor->GetPlayInEditorSessionInfo();
				if (!PIEInfo.IsSet())
				{
					return;
				}
				
				for (int32 i = 0; i < PIEInfo->NumClientInstancesCreated; ++i)
				{
					FString TargetUrl = "https://www.flecs.dev/explorer/?host=localhost:";
					// ECS_REST_DEFAULT_PORT is 27750
					// Each PIE instance in the rest module incremements the port by 1
					TargetUrl.Append(FString::FromInt(27750 + i));
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