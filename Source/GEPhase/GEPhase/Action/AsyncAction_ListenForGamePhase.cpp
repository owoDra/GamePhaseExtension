// Copyright (C) 2024 owoDra

#include "AsyncAction_ListenForGamePhase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ListenForGamePhase)


void UAsyncAction_ListenForGamePhase::Activate()
{
	if (auto* Subsystem{ UWorld::GetSubsystem<UGamePhaseSubsystem>(WorldPtr.Get()) })
	{
		auto WeakThis{ TWeakObjectPtr<UAsyncAction_ListenForGamePhase>(this) };

		ListenerHandle = Subsystem->RegisterListener(ChannelToRegister,
			[WeakThis](FGameplayTag GamePhaseTag, EGamePhaseEventType EventType)
			{
				if (auto* StrongThis{ WeakThis.Get() })
				{
					StrongThis->HandleEventReceived(GamePhaseTag, EventType);
				}
			},
			TagMatchType);
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_ListenForGamePhase::SetReadyToDestroy()
{
	ListenerHandle.Unregister();

	Super::SetReadyToDestroy();
}

UAsyncAction_ListenForGamePhase* UAsyncAction_ListenForGamePhase::ListenForGamePhase(UObject* WorldContextObject, FGameplayTag GamePhaseTag, EGamePhaseTagMatchType MatchType)
{
	auto* World{ GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) };
	if (!World)
	{
		return nullptr;
	}

	auto* Action{ NewObject<UAsyncAction_ListenForGamePhase>() };
	Action->WorldPtr = World;
	Action->ChannelToRegister = GamePhaseTag;
	Action->TagMatchType = MatchType;
	//Action->RegisterWithGameInstance(World);

	return Action;
}

void UAsyncAction_ListenForGamePhase::HandleEventReceived(FGameplayTag GamePhaseTag, EGamePhaseEventType EventType)
{
	OnGamePhaseRecived.Broadcast(GamePhaseTag, EventType);

	if (!OnGamePhaseRecived.IsBound())
	{
		// If the BP object that created the async node is destroyed, OnMessageReceived will be unbound after calling the broadcast.
		// In this case we can safely mark this receiver as ready for destruction.
		// Need to support a more proactive mechanism for cleanup FORT-340994

		SetReadyToDestroy();
	}
}
