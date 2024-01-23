// Copyright (C) 2024 owoDra

#pragma once

#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayTagContainer.h"

#include "ActiveGamePhase.generated.h"

class AGameStateBase;
class UGamePhaseComponent;
class UGamePhase;


/**
 * Data of the GamePhases currently applied
 */
USTRUCT(BlueprintType)
struct GEPHASE_API FActiveGamePhase : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend struct FActiveGamePhaseContainer;

public:
	FActiveGamePhase() {}

	FActiveGamePhase(const TSubclassOf<UGamePhase>& InClass) 
		: Class(InClass) 
	{}

	FActiveGamePhase(const TSubclassOf<UGamePhase>& InClass, const FGameplayTag& InParentPhaseTag) 
		: Class(InClass)
		, ParentPhaseTag(InParentPhaseTag) 
	{}

protected:
	//
	// Class of game phase being applied
	//
	UPROPERTY()
	TSubclassOf<UGamePhase> Class{ nullptr };

	//
	// Parent phase of this game phase
	// 
	// Tips:
	//	Set if this game phase was a subphase initiated by the parent phase
	//
	UPROPERTY()
	FGameplayTag ParentPhaseTag{ FGameplayTag::EmptyTag };

	//
	// Instances of game phases
	// 
	// Tips:
	//	Not replicated, but created on both server and client
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UGamePhase> Instance{ nullptr };

protected:



protected:
	/**
	 * Return game phase tag
	 */
	const FGameplayTag& GetGamePhaseTag() const;

public:
	/**
	 * Returns debug string of this
	 */
	FString GetDebugString() const;

};


/**
 * List of ActiveGamePhase
 */
USTRUCT(BlueprintType)
struct GEPHASE_API FActiveGamePhaseContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FActiveGamePhaseContainer() {}

	void RegisterOwner(AGameStateBase* InOwner, UGamePhaseComponent* InOwnerComponent);

public:
	//
	// List of currently applied ActiveGamePhases
	//
	UPROPERTY()
	TArray<FActiveGamePhase> Entries;

	//
	// The owner of this container
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<AGameStateBase> Owner{ nullptr };

	//
	// The owner of this container
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UGamePhaseComponent> OwnerComponent{ nullptr };

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FActiveGamePhase, FActiveGamePhaseContainer>(Entries, DeltaParms, *this);
	}

public:
	bool SetGamePhase(const TSubclassOf<UGamePhase>& GamePhaseClass);

	bool AddSubPhase(const TSubclassOf<UGamePhase>& GamePhaseClass, const FGameplayTag& InParentPhaseTag);

	bool EndPhaseByTag(const FGameplayTag& InGamePhaseTag);

protected:
	void EndAllPhase();

	void HandleGamePhaseAdd(FActiveGamePhase& ActiveGamePhase);
	void HandleGamePhaseRemove(FActiveGamePhase& ActiveGamePhase);

	void HandleSubPhaseStart(const FGameplayTag& ParentPhaseTag, const FGameplayTag& SubPhaseTag);
	void HandleSubPhaseEnd(const FGameplayTag& ParentPhaseTag, const FGameplayTag& SubPhaseTag);

};

template<>
struct TStructOpsTypeTraits<FActiveGamePhaseContainer> : public TStructOpsTypeTraitsBase2<FActiveGamePhaseContainer>
{
	enum { WithNetDeltaSerializer = true };
};
