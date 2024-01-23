// Copyright (C) 2024 owoDra

#include "ActiveGamePhase.h"

#include "GamePhaseSubsystem.h"
#include "GamePhase.h"
#include "GEPhaseLogs.h"

#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActiveGamePhase)


//////////////////////////////////////////////////////
// FActiveGamePhase

#pragma region FActiveGamePhase

const FGameplayTag& FActiveGamePhase::GetGamePhaseTag() const
{
	return Class.GetDefaultObject()->GetGamePhaseTag();
}

FString FActiveGamePhase::GetDebugString() const
{
	return FString::Printf(TEXT("(Class:%s, Instance:%s)"),
		*GetNameSafe(Class), *GetNameSafe(Instance));
}

#pragma endregion


//////////////////////////////////////////////////////
// FActiveGamePhaseContainer

#pragma region FActiveGamePhaseContainer

void FActiveGamePhaseContainer::RegisterOwner(AGameStateBase* InOwner, UGamePhaseComponent* InOwnerComponent)
{
	Owner = InOwner;
	OwnerComponent = InOwnerComponent;
}


void FActiveGamePhaseContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	check(Owner);
	check(OwnerComponent);

	for (const auto& Index : RemovedIndices)
	{
		auto& Entry{ Entries[Index] };

		HandleGamePhaseRemove(Entry);
	}
}

void FActiveGamePhaseContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	check(Owner);
	check(OwnerComponent);

	for (const auto& Index : AddedIndices)
	{
		auto& Entry{ Entries[Index] };

		HandleGamePhaseAdd(Entry);
	}
}

void FActiveGamePhaseContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}


bool FActiveGamePhaseContainer::SetGamePhase(const TSubclassOf<UGamePhase>& GamePhaseClass)
{
	check(Owner);
	check(OwnerComponent);

	// Suspend if class is not valid

	if (!GamePhaseClass)
	{
		return false;
	}

	// End old game phases

	EndAllPhase();

	// Create new active game phase

	auto& NewGamePhase{ Entries.Emplace_GetRef(GamePhaseClass)};
	HandleGamePhaseAdd(NewGamePhase);
	MarkItemDirty(NewGamePhase);

	return true;
}

bool FActiveGamePhaseContainer::AddSubPhase(const TSubclassOf<UGamePhase>& GamePhaseClass, const FGameplayTag& InParentPhaseTag)
{
	check(Owner);
	check(OwnerComponent);

	// Suspend if class is not valid

	if (!GamePhaseClass)
	{
		return false;
	}

	// Suspend if tag is not valid

	if (!InParentPhaseTag.IsValid())
	{
		return false;
	}

	// create new active sub phase

	auto& NewGamePhase{ Entries.Emplace_GetRef(GamePhaseClass, InParentPhaseTag) };
	HandleGamePhaseAdd(NewGamePhase);
	MarkItemDirty(NewGamePhase);

	return true;
}

bool FActiveGamePhaseContainer::EndPhaseByTag(const FGameplayTag& InGamePhaseTag)
{
	check(Owner);
	check(OwnerComponent);

	// Suspend if tag is not valid

	if (!InGamePhaseTag.IsValid())
	{
		return false;
	}

	// Look for game phase and exit if it was a sub-phase

	for (auto It{ Entries.CreateIterator() }; It; ++It)
	{
		auto& Entry{ *It };

		if ((Entry.GetGamePhaseTag() == InGamePhaseTag) && Entry.ParentPhaseTag.IsValid())
		{
			HandleGamePhaseRemove(Entry);
			It.RemoveCurrent();

			MarkArrayDirty();

			return true;
		}
	}

	return false;
}


void FActiveGamePhaseContainer::EndAllPhase()
{
	for (auto& Entry : Entries)
	{
		HandleGamePhaseRemove(Entry);
	}

	Entries.Empty();

	MarkArrayDirty();
}

void FActiveGamePhaseContainer::HandleGamePhaseAdd(FActiveGamePhase& ActiveGamePhase)
{
	// Create new instance

	ActiveGamePhase.Instance = NewObject<UGamePhase>(Owner.Get(), ActiveGamePhase.Class);
	check(ActiveGamePhase.Instance);

	// Handle start

	ActiveGamePhase.Instance->HandleGamePhaseStart();

	// Notify subsystem

	const auto& GamePhaseTag{ ActiveGamePhase.GetGamePhaseTag() };

	if (auto* Subsystem{ UWorld::GetSubsystem<UGamePhaseSubsystem>(Owner->GetWorld()) })
	{
		Subsystem->AddGamePhaseTag(GamePhaseTag);
	}

	// Notifies that a subphase has started if there is a parent game phase

	if (ActiveGamePhase.ParentPhaseTag.IsValid())
	{
		HandleSubPhaseStart(ActiveGamePhase.ParentPhaseTag, GamePhaseTag);
	}
}

void FActiveGamePhaseContainer::HandleGamePhaseRemove(FActiveGamePhase& ActiveGamePhase)
{
	// Handle End

	if (ActiveGamePhase.Instance)
	{
		ActiveGamePhase.Instance->HandleGamePhaseEnd();
	}

	// Notify subsystem

	const auto& GamePhaseTag{ ActiveGamePhase.GetGamePhaseTag() };

	if (auto* Subsystem{ UWorld::GetSubsystem<UGamePhaseSubsystem>(Owner->GetWorld()) })
	{
		Subsystem->RemoveGamePhaseTag(GamePhaseTag);
	}

	// Notifies that a subphase has end if there is a parent game phase

	if (ActiveGamePhase.ParentPhaseTag.IsValid())
	{
		HandleSubPhaseEnd(ActiveGamePhase.ParentPhaseTag, GamePhaseTag);
	}
}

void FActiveGamePhaseContainer::HandleSubPhaseStart(const FGameplayTag& ParentPhaseTag, const FGameplayTag& SubPhaseTag)
{
	for (const auto& Entry : Entries)
	{
		if (Entry.GetGamePhaseTag() == ParentPhaseTag)
		{
			if (ensure(Entry.Instance))
			{
				Entry.Instance->HandleSubPhaseStart(SubPhaseTag);
				return;
			}
		}
	}
}

void FActiveGamePhaseContainer::HandleSubPhaseEnd(const FGameplayTag& ParentPhaseTag, const FGameplayTag& SubPhaseTag)
{
	for (const auto& Entry : Entries)
	{
		if (Entry.GetGamePhaseTag() == ParentPhaseTag)
		{
			if (ensure(Entry.Instance))
			{
				Entry.Instance->HandleSubPhaseEnd(SubPhaseTag);
				return;
			}
		}
	}
}

#pragma endregion

