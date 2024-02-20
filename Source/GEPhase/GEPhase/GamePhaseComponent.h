// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTasksComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "Phase/ActiveGamePhase.h"

#include "GamePhaseComponent.generated.h"

/**
 * Components to manage game phases
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GEPHASE_API UGamePhaseComponent : public UGameplayTasksComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UGamePhaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

	//
	// Name of the event that signals that the game phase is ready to be applied.
	//
	static const FName NAME_GamePhaseReady;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

protected:
	virtual bool CanChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager) const { return true; }

	virtual void HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager) {}


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


	/////////////////////////////////////////////////////////////////
	// Utilities
public:
	bool HasAuthority() const;

};
