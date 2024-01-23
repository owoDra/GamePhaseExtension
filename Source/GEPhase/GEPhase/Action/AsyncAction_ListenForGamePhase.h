// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/CancellableAsyncAction.h"

#include "GamePhaseSubsystem.h"
#include "Type/GamePhaseListenerTypes.h"

#include "AsyncAction_ListenForGamePhase.generated.h"


/**
 * Delegate to signal the beginning or end of a game phase
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGamePhaseEventDelegate, FGameplayTag, GamePhaseTag, EGamePhaseEventType, EventType);


/**
 * Async action for listen for GamePhase Event.
 */
UCLASS(BlueprintType)
class GEPHASE_API UAsyncAction_ListenForGamePhase : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UAsyncAction_ListenForGamePhase() {}

public:
	//
	// Delegate to signal the beginning or end of a game phase
	//
	UPROPERTY(BlueprintAssignable)
	FAsyncGamePhaseEventDelegate OnGamePhaseRecived;

protected:
	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	EGamePhaseTagMatchType TagMatchType{ EGamePhaseTagMatchType::ExactMatch };

	FGamePhaseListenerHandle ListenerHandle;

public:
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	/**
	 * Asynchronously wait for a specific game phase to start or end
	 *
	 * @param GamePhaseTag		The game phase to listen for
	 * @param MatchType			The rule used for matching the game phase tag with broadcasted event
	 */
	UFUNCTION(BlueprintCallable, Category = "GamePhase", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ListenForGamePhase* ListenForGamePhase(UObject* WorldContextObject, UPARAM(meta = (Categories = "GamePhase")) FGameplayTag GamePhaseTag, EGamePhaseTagMatchType MatchType = EGamePhaseTagMatchType::ExactMatch);

private:
	void HandleEventReceived(FGameplayTag GamePhaseTag, EGamePhaseEventType EventType);

};
