// Copyright (C) 2024 owoDra

#include "GamePhaseComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePhaseComponent)


const FName UGamePhaseComponent::NAME_GamePhaseReady("GamePhaseReady");

UGamePhaseComponent::UGamePhaseComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UGamePhaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ActiveGamePhases, Params);
}


void UGamePhaseComponent::OnRegister()
{
	ActiveGamePhases.RegisterOwner(GetGameState<AGameStateBase>(), this);

	Super::OnRegister();
}

void UGamePhaseComponent::HandleChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager)
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetGameState<AGameStateBase>(), UGamePhaseComponent::NAME_GamePhaseReady);
}


bool UGamePhaseComponent::SetGamePhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	if (!HasAuthority())
	{
		return false;
	}

	return ActiveGamePhases.SetGamePhase(GamePhaseClass);
}

bool UGamePhaseComponent::AddSubPhase(TSubclassOf<UGamePhase> GamePhaseClass, FGameplayTag InParentPhaseTag)
{
	if (!HasAuthority())
	{
		return false;
	}

	return ActiveGamePhases.AddSubPhase(GamePhaseClass, InParentPhaseTag);
}

bool UGamePhaseComponent::EndPhaseByTag(FGameplayTag InGamePhaseTag)
{
	if (!HasAuthority())
	{
		return false;
	}

	return ActiveGamePhases.EndPhaseByTag(InGamePhaseTag);
}
