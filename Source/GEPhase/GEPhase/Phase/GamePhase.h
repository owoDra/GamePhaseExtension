// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTaskOwnerInterface.h"

#include "GameplayTagContainer.h"

#include "GamePhase.generated.h"

class AGameStateBase;
class UGamePhaseComponent;


/**
 * Class representing the current game phase
 *
 * Tips:
 *	Processing during that game phase and the ability to transition to the next game phase.
 * 
 * Note:
 *	The processing and parameters here are not replicated, but the events are executed on both the server and the client.
 *	Therefore, management of game scores, for example, must be managed by other systems. (For example, StatTagStock from GameFrameworkCore).
 *	However, the event will still run, so it is possible to track scores and provide feedback to the player.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class GEPHASE_API UGamePhase : public UObject, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
public:
	UGamePhase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/////////////////////////////////////////////////////////////////////////////////////
	// Validate Data
public:
#if WITH_EDITOR 
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	/////////////////////////////////////////////////////////////////////////////////////
	// Initialization
protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Owner")
	TWeakObjectPtr<UGamePhaseComponent> OwnerComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Owner")
	TWeakObjectPtr<AGameStateBase> Owner;

public:
	void InitializeGamePhase(AGameStateBase* GameState, UGamePhaseComponent* GamePhaseComponent);


	/////////////////////////////////////////////////////////////////////////////////////
	// IGameplayTaskOwnerInterface
protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameplayTask>> ActiveTasks;

public:
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;


	/////////////////////////////////////////////////////////////////////////////////////
	// Game Phase Tag
protected:
	//
	// Gameplay tags to identify this game phase
	// 
	// Note:
	//	This tag for a game phase used in one game mode must be unique.
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GamePhase", meta = (Categories = "GamePhase"))
	FGameplayTag GamePhaseTag;

public:
	/**
	 * Returns tag of this game phase
	 */
	const FGameplayTag& GetGamePhaseTag() const { return GamePhaseTag; }


	/////////////////////////////////////////////////////////////////////////////////////
	// Transitions
protected:
	/**
	 * Transition to the specified game phase.
	 * 
	 * Note:
	 *	Authority is required
	 * 
	 * Tips:
	 *	Returns true if a transition to a new game phase is possible, or false if a transition is not possible
	 * 
	 *	This game phase ends when the game transitions to a new game phase. 
	 *	Also, if this was a sub-phase of another game phase, its parent game phase is also terminated.
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool NextGamePhase(TSubclassOf<UGamePhase> GamePhaseClass);

	/**
	 * Start sub-phase.
	 * 
	 * Note:
	 *	Authority is required
	 * 
	 * Tips:
	 *	Returns true if it is possible to start, false if it is not possible to start
	 * 
	 *  If this game phase is already a subphase of another game phase, it can still be created as a sub-subphase,
	 *	but this is not recommended because it is difficult to manage.
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool StartSubPhase(TSubclassOf<UGamePhase> GamePhaseClass);

	/**
	 * End this game phase
	 * 
	 * Note:
	 *	Authority is required
	 * 
	 *  This function is only executed if this game phase is a subphase of another game phase
	 * 
	 * Tips:
	 *	This function is used to terminate a subphase
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool EndPhase();


	/////////////////////////////////////////////////////////////////////////////////////
	// Events
public:
	/**
	 * Called when this game phase is started
	 */
	void HandleGamePhaseStart();

	/**
	 * Called when this game phase is end
	 */
	void HandleGamePhaseEnd();

	/**
	 * Called when a subphase of this game phase starts
	 */
	void HandleSubPhaseStart(const FGameplayTag& SubPhaseTag);

	/**
	 * Called when a subphase of this game phase ends
	 */
	void HandleSubPhaseEnd(const FGameplayTag& SubPhaseTag);

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GamePhase")
	void OnGamePhaseStart();
	virtual void OnGamePhaseStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "GamePhase")
	void OnGamePhaseEnd();
	virtual void OnGamePhaseEnd_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "GamePhase")
	void OnSubPhaseStart(const FGameplayTag& SubPhaseTag);
	virtual void OnSubPhaseStart_Implementation(const FGameplayTag& SubPhaseTag) {}

	UFUNCTION(BlueprintNativeEvent, Category = "GamePhase")
	void OnSubPhaseEnd(const FGameplayTag& SubPhaseTag);
	virtual void OnSubPhaseEnd_Implementation(const FGameplayTag& SubPhaseTag) {}


	/////////////////////////////////////////////////////////////////////////////////////
	// Utilities
public:
	virtual class UWorld* GetWorld() const final;

protected:
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (DeterminesOutputType = "InClass"))
	AGameStateBase* GetGameState(TSubclassOf<AGameStateBase> InClass) const;

	template<typename T>
	T* GetGameState() const
	{
		return Cast<T>(GetGameState(T::StaticClass()));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Utilities", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool HasAuthority() const;

};
