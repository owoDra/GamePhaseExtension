// Copyright (C) 2024 owoDra

#include "GamePhaseListenerTypes.h"

#include "GamePhaseSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePhaseListenerTypes)


void FGamePhaseListenerHandle::Unregister()
{
	if (auto* StrongSubsystem{ Subsystem.Get() })
	{
		StrongSubsystem->UnregisterListener(*this);
		Subsystem.Reset();
		GamePhaseTag = FGameplayTag();
		ID = 0;
	}
}
