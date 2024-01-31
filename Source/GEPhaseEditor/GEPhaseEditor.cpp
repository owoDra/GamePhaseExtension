// Copyright (C) 2024 owoDra

#include "GEPhaseEditor.h"

#include "AssetTypeActions/AssetTypeActions_GamePhaseBlueprint.h"

IMPLEMENT_MODULE(FGEPhaseEditorModule, GEPhaseEditor)


void FGEPhaseEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
}

void FGEPhaseEditorModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
}


void FGEPhaseEditorModule::RegisterAssetTypeActionCategory()
{
	static const FName CategoryKey{ TEXT("GameGameMode") };
	static const FText CategoryDisplayName{ NSLOCTEXT("GameXXX","GameGameModeCategory", "GameMode") };

	Category = IAssetTools::Get().RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);
}

void FGEPhaseEditorModule::RegisterAssetTypeActions()
{
	RegisterAssetTypeActionCategory();

	RegisterAsset<FAssetTypeActions_GamePhaseBlueprint>(RegisteredAssetTypeActions);
}

void FGEPhaseEditorModule::UnregisterAssetTypeActions()
{
	UnregisterAssets(RegisteredAssetTypeActions);
}

void FGEPhaseEditorModule::UnregisterAssets(TArray<TSharedPtr<FAssetTypeActions_Base>>& RegisteredAssets)
{
	const auto* AssetToolsPtr{ FModuleManager::GetModulePtr<FAssetToolsModule>(NAME_AssetToolsModule) };
	if (!AssetToolsPtr)
	{
		return;
	}

	auto& AssetTools{ AssetToolsPtr->Get() };
	for (auto& AssetTypeActionIt : RegisteredAssets)
	{
		if (AssetTypeActionIt.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(AssetTypeActionIt.ToSharedRef());
		}
	}
}
