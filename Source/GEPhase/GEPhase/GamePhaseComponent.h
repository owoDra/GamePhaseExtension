// Copyright (C) 2024 owoDra

#pragma once

#include "Component/GFCGameStateComponent.h"

#include "Phase/ActiveGamePhase.h"

#include "GamePhaseComponent.generated.h"

/**
 * Components to manage game phases
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GEPHASE_API UGamePhaseComponent : public UGFCGameStateComponent
{
	GENERATED_BODY()
public:
	UGamePhaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Name of the event that signals that the game phase is ready to be applied.
	//
	static const FName NAME_GamePhaseReady;

protected:
	virtual void OnRegister() override;

	virtual void HandleChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager) override;


	/////////////////////////////////////////////////////////////////
	// Active Game Phases
protected:
	//
	// List of added ActiveGamePhase
	//
	UPROPERTY(Transient, Replicated)
	FActiveGamePhaseContainer ActiveGamePhases;

public:
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool SetGamePhase(TSubclassOf<UGamePhase> GamePhaseClass);

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool AddSubPhase(TSubclassOf<UGamePhase> GamePhaseClass, FGameplayTag InParentPhaseTag);

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool EndPhaseByTag(FGameplayTag InGamePhaseTag);

};
