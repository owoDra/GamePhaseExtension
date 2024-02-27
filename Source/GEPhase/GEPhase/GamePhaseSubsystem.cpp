// Copyright (C) 2024 owoDra

#include "GamePhaseSubsystem.h"

#include "GamePhaseComponent.h"
#include "GEPhaseLogs.h"

#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePhaseSubsystem)


void UGamePhaseSubsystem::Deinitialize()
{
	ListenerMap.Reset();
	GamePhaseTagCache.Reset();

	Super::Deinitialize();
}


void UGamePhaseSubsystem::AddGamePhaseTag(const FGameplayTag& GamePhaseTag)
{
	GamePhaseTagCache.Emplace(GamePhaseTag);

	BroadcastGamePhaseEvent(GamePhaseTag, EGamePhaseEventType::Start);
}

void UGamePhaseSubsystem::RemoveGamePhaseTag(const FGameplayTag& GamePhaseTag)
{
	GamePhaseTagCache.Remove(GamePhaseTag);

	BroadcastGamePhaseEvent(GamePhaseTag, EGamePhaseEventType::End);
}

const FGameplayTag& UGamePhaseSubsystem::GetLastTransitionGamePhaseTag() const
{
	return GamePhaseTagCache.IsEmpty() ? FGameplayTag::EmptyTag : GamePhaseTagCache.Top();
}

FGameplayTagContainer UGamePhaseSubsystem::GetGamePhaseTags() const
{
	return FGameplayTagContainer::CreateFromArray(GamePhaseTagCache);
}


FGamePhaseListenerHandle UGamePhaseSubsystem::RegisterListener(FGameplayTag GamePhaseTag, TFunction<void(FGameplayTag, EGamePhaseEventType)>&& Callback, EGamePhaseTagMatchType MatchType)
{
	auto& List{ ListenerMap.FindOrAdd(GamePhaseTag) };

	auto& Entry{ List.Listeners.AddDefaulted_GetRef() };
	Entry.ReceivedCallback = MoveTemp(Callback);
	Entry.HandleID = ++List.HandleID;
	Entry.MatchType = MatchType;

	return FGamePhaseListenerHandle(this, GamePhaseTag, Entry.HandleID);
}

void UGamePhaseSubsystem::UnregisterListener(FGamePhaseListenerHandle Handle)
{
	if (Handle.IsValid())
	{
		check(Handle.Subsystem == this);

		UnregisterListener(Handle.GamePhaseTag, Handle.ID);
	}
	else
	{
		UE_LOG(LogGameExt_GamePhase, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}

void UGamePhaseSubsystem::UnregisterListener(FGameplayTag GamePhaseTag, int32 HandleID)
{
	if (auto* List{ ListenerMap.Find(GamePhaseTag) })
	{
		auto MatchIndex
		{
			List->Listeners.IndexOfByPredicate(
				[ID = HandleID](const FGamePhaseListenerData& Other)
				{
					return Other.HandleID == ID;
				}
			)
		};

		if (MatchIndex != INDEX_NONE)
		{
			List->Listeners.RemoveAtSwap(MatchIndex);
		}

		if (List->Listeners.Num() == 0)
		{
			ListenerMap.Remove(GamePhaseTag);
		}
	}
}

void UGamePhaseSubsystem::BroadcastGamePhaseEvent(FGameplayTag GamePhaseTag, EGamePhaseEventType EventType)
{
	auto bOnInitialTag{ true };

	for (auto Tag{ GamePhaseTag }; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const auto* List{ ListenerMap.Find(Tag) })
		{
			// Copy in case there are removals while handling callbacks

			auto ListenerArray{ TArray<FGamePhaseListenerData>(List->Listeners) };

			for (const auto& Listener : ListenerArray)
			{
				if (bOnInitialTag || (Listener.MatchType == EGamePhaseTagMatchType::PartialMatch))
				{
					// The receiving type must be either a parent of the sending type or completely ambiguous (for internal use)

					Listener.ReceivedCallback(GamePhaseTag, EventType);
				}
			}
		}

		bOnInitialTag = false;
	}
}


bool UGamePhaseSubsystem::SetGamePhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	if (!GamePhaseClass)
	{
		return false;
	}

	if (auto* GameState{ GetWorld()->GetGameState() })
	{
		if (auto* Component{ GameState->FindComponentByClass<UGamePhaseComponent>() })
		{
			return Component->SetGamePhase(GamePhaseClass);
		}
	}

	return false;
}

bool UGamePhaseSubsystem::EndGamePhase(FGameplayTag GamePhaseTag)
{
	if (!GamePhaseTag.IsValid())
	{
		return false;
	}

	if (auto* GameState{ GetWorld()->GetGameState() })
	{
		if (auto* Component{ GameState->FindComponentByClass<UGamePhaseComponent>() })
		{
			return Component->EndPhaseByTag(GamePhaseTag);
		}
	}

	return false;
}
