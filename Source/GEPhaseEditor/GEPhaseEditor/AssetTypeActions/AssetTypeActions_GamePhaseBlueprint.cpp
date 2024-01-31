// Copyright (C) 2024 owoDra

#include "AssetTypeActions_GamePhaseBlueprint.h"

#include "Phase/GamePhaseBlueprint.h"
#include "Phase/GamePhase.h"
#include "Factory/GamePhaseBlueprintFactory.h"
#include "GEPhaseEditor.h"

#include "Misc/MessageDialog.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_GamePhaseBlueprint::GetSupportedClass() const
{
	return UGamePhaseBlueprint::StaticClass();
}

FText FAssetTypeActions_GamePhaseBlueprint::GetName() const
{ 
	return LOCTEXT("AssetTypeActions_GamePhaseBlueprint", "Game Phase"); 
}

FColor FAssetTypeActions_GamePhaseBlueprint::GetTypeColor() const
{
	return FColor(0, 255, 255);
}

uint32 FAssetTypeActions_GamePhaseBlueprint::GetCategories()
{
	return FGEPhaseEditorModule::Category;
}

const TArray<FText>& FAssetTypeActions_GamePhaseBlueprint::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("SubMenus_GamePhaseBlueprint", "Game Phase")
	};

	return SubMenus;
}


void FAssetTypeActions_GamePhaseBlueprint::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	auto Mode{ EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone };

	for (auto ObjIt{ InObjects.CreateConstIterator() }; ObjIt; ++ObjIt)
	{
		if (auto Blueprint{ Cast<UBlueprint>(*ObjIt) })
		{
			auto bLetOpen{ true };

			if (!Blueprint->ParentClass)
			{
				const auto MessageReplyType
				{
					FMessageDialog::Open(EAppMsgType::YesNo,
						LOCTEXT("FailedToLoadGamePhaseBlueprintWithContinue", "GamePhase Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?"))
				};

				bLetOpen = (EAppReturnType::Yes == MessageReplyType);
			}
		
			if (bLetOpen)
			{
				TSharedRef<FBlueprintEditor> NewEditor{ new FBlueprintEditor() };

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(Blueprint);

				NewEditor->InitBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadGamePhaseBlueprint", "GamePhase Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}


UFactory* FAssetTypeActions_GamePhaseBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	auto* GamePhaseBlueprintFactory{ NewObject<UGamePhaseBlueprintFactory>() };

	GamePhaseBlueprintFactory->ParentClass = TSubclassOf<UGamePhase>(*InBlueprint->GeneratedClass);

	return GamePhaseBlueprintFactory;
}


bool FAssetTypeActions_GamePhaseBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return FBlueprintEditorUtils::IsDataOnlyBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsLevelScriptBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(Blueprint)
		&& !Blueprint->bForceFullEditor
		&& !Blueprint->bIsNewlyCreated;
}

#undef LOCTEXT_NAMESPACE
