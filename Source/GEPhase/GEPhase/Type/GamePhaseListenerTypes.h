// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "GamePhaseListenerTypes.generated.h"

class UGamePhaseSubsystem;


/**
 * Match rule for message receivers
 */
UENUM(BlueprintType)
enum class EGamePhaseTagMatchType : uint8
{
	// An exact match will only receive messages with exactly the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
	ExactMatch,

	// A partial match will receive any messages rooted in the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
	PartialMatch
};


/**
 * Event type for GamePhase
 */
UENUM(BlueprintType)
enum class EGamePhaseEventType : uint8
{
	Start,
	End
};


/**
 * Entry information for a single registered listener
 */
USTRUCT()
struct FGamePhaseListenerData
{
	GENERATED_BODY()
public:
	FGamePhaseListenerData() {}

public:
	//
	// Callback for when a message has been received
	//
	TFunction<void(FGameplayTag, EGamePhaseEventType)> ReceivedCallback;

	int32 HandleID;
	EGamePhaseTagMatchType MatchType;

};


/**
 * An opaque handle that can be used to remove a previously registered GamePhase listener
 */
USTRUCT(BlueprintType)
struct GEPHASE_API FGamePhaseListenerHandle
{
	GENERATED_BODY()

	friend class UGamePhaseSubsystem;

public:
	FGamePhaseListenerHandle() {}

	FGamePhaseListenerHandle(UGamePhaseSubsystem* InSubsystem, FGameplayTag InGamePhaseTag, int32 InID)
		: Subsystem(InSubsystem)
		, GamePhaseTag(InGamePhaseTag)
		, ID(InID)
	{}

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UGamePhaseSubsystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag GamePhaseTag;

	UPROPERTY(Transient)
	int32 ID{ 0 };

	FDelegateHandle StateClearedHandle;

public:
	void Unregister();

	bool IsValid() const { return ID != 0; }

};
