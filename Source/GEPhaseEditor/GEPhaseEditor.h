// Copyright (C) 2024 owoDra

#pragma once

#include "Modules/ModuleManager.h"

#include "AssetTypeCategories.h"
#include "AssetTypeActions_Base.h"
#include "IAssetTools.h"


/**
 *  Modules for the editor features of the Game Phase Extension plugin
 */
class FGEPhaseEditorModule : public IModuleInterface
{
public:
	//
	// Module name used to load/unload AssetToolsModule
	//
	inline static const FName NAME_AssetToolsModule{ TEXT("AssetTools") };

	//
	// Categories in the context menu of the asset creation
	//
	inline static EAssetTypeCategories::Type Category = EAssetTypeCategories::None;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


protected:
	//
	// Registered AssetTypeActions
	//
	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetTypeActions;

protected:
	/**
	 * Add a category in the context menu of the asset creation
	 */
	void RegisterAssetTypeActionCategory();

	/**
	 * Register AssetTypeActions in the module
	 */
	void RegisterAssetTypeActions();

	/**
	 * Unregister AssetTypeActions from the module
	 */
	void UnregisterAssetTypeActions();

	template<typename T>
	void RegisterAsset(TArray<TSharedPtr<FAssetTypeActions_Base>>& InOutRegisteredAssets)
	{
		TSharedPtr<T> SettingsDataTableAction{ MakeShared<T>() };
		IAssetTools::Get().RegisterAssetTypeActions(SettingsDataTableAction.ToSharedRef());
		InOutRegisteredAssets.Emplace(MoveTemp(SettingsDataTableAction));
	}

	void UnregisterAssets(TArray<TSharedPtr<FAssetTypeActions_Base>>& RegisteredAssets);
};


