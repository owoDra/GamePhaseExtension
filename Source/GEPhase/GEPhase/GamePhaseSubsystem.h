// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "Type/GamePhaseListenerTypes.h"

#include "GamePhaseSubsystem.generated.h"

class UGamePhase;
class UAsyncAction_ListenForGamePhase;
struct FActiveGamePhaseContainer;


/** 
 * Subsystem to track current game phase
 */
UCLASS()
class GEPHASE_API UGamePhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class UAsyncAction_ListenForGamePhase;
	friend struct FActiveGamePhaseContainer;

public:
	UGamePhaseSubsystem() {}

public:
	virtual void Deinitialize() override;

	////////////////////////////////////////////////////
	// Game Phase Cache
protected:
	//
	// List of tags for the currently active game phase
	// 
	// Tips:
	//	Game phase in which the last tag in the array was the last to transition
	//
	UPROPERTY(Transient)
	TArray<FGameplayTag> GamePhaseTagCache;

protected:
	void AddGamePhaseTag(const FGameplayTag& GamePhaseTag);
	void RemoveGamePhaseTag(const FGameplayTag& GamePhaseTag);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GamePhase")
	const FGameplayTag& GetLastTransitionGamePhaseTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GamePhase")
	FGameplayTagContainer GetGamePhaseTags() const;


	////////////////////////////////////////////////////
	// Listner
protected:
	/**
	 * List of all entries for a given channel
	 */
	struct FChannelListenerList
	{
		TArray<FGamePhaseListenerData> Listeners;
		int32 HandleID{ 0 };
	};

	//
	// Listen data map for game phase related to GameplayTag
	//
	TMap<FGameplayTag, FChannelListenerList> ListenerMap;

public:
	/**
	 * Register to receive messages on a specified GamePhaseTag
	 */
	FGamePhaseListenerHandle RegisterListener(
		FGameplayTag GamePhaseTag
		, TFunction<void(FGameplayTag, EGamePhaseEventType)>&& Callback
		, EGamePhaseTagMatchType MatchType = EGamePhaseTagMatchType::ExactMatch);

	/**
	 * Remove a GamePhase listener previously registered by RegisterListener
	 */
	void UnregisterListener(FGamePhaseListenerHandle Handle);
	void UnregisterListener(FGameplayTag GamePhaseTag, int32 HandleID);

protected:
	/**
	 * Broadcast a event on the specified game phase
	 */
	void BroadcastGamePhaseEvent(FGameplayTag GamePhaseTag, EGamePhaseEventType EventType);


	////////////////////////////////////////////////////
	// Utilities
public:
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool SetGamePhase(TSubclassOf<UGamePhase> GamePhaseClass);

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	bool EndGamePhase(FGameplayTag GamePhaseTag);


	////////////////////////////////////////////////////
	// Game Mode Option
public:
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "GamePhase")
	virtual bool InitializeFromGameModeOption();

	UFUNCTION(BlueprintCallable, Category = "GamePhase")
	virtual FString ConstructGameModeOption() const;

};
