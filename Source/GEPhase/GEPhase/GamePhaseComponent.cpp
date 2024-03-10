// Copyright (C) 2024 owoDra

#include "GamePhaseComponent.h"

#include "GEPhaseLogs.h"

#include "InitState/InitStateTags.h"
#include "InitState/InitStateComponent.h"
#include "GFCoreLogs.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePhaseComponent)


const FName UGamePhaseComponent::NAME_ActorFeatureName("GamePhase");

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
	Super::OnRegister();

	// This component can only be added to classes derived from AGameStateBase

	auto* GameState{ GetOwner<AGameStateBase>() };
	ensureAlwaysMsgf((GameState != nullptr), TEXT("[%s] on [%s] can only be added to GameState actors."), *GetNameSafe(GetClass()), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Actor.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(GetClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one [%s] should exist on [%s]."), *GetNameSafe(GetClass()), *GetNameSafe(GetOwner()));

	ActiveGamePhases.RegisterOwner(GameState, this);

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UGamePhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensureMsgf(TryToChangeInitState(TAG_InitState_Spawned), TEXT("[%s] on [%s]."), *GetNameSafe(this), *GetNameSafe(GetOwner()));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UGamePhaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UGamePhaseComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	/**
	 * [InitState None] -> [InitState Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		if (GetOwner() == nullptr)
		{
			return false;
		}

		return CanChangeInitStateToSpawned(Manager);
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		if (!Manager->HasFeatureReachedInitState(GetOwner(), UInitStateComponent::NAME_ActorFeatureName, TAG_InitState_DataAvailable))
		{
			return false;
		}

		return CanChangeInitStateToDataAvailable(Manager);
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		return CanChangeInitStateToDataInitialized(Manager);
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return CanChangeInitStateToGameplayReady(Manager);
	}

	return false;
}

void UGamePhaseComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	check(Manager);

	UE_LOG(LogGameCore_InitState, Log, TEXT("[%s] %s: InitState Reached: %s"),
		GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
		*GetNameSafe(this),
		*DesiredState.GetTagName().ToString());

	/**
	 * [InitState None] -> [InitState Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		HandleChangeInitStateToSpawned(Manager);
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		HandleChangeInitStateToDataAvailable(Manager);
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		HandleChangeInitStateToDataInitialized(Manager);
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetOwner<AGameStateBase>(), UGamePhaseComponent::NAME_GamePhaseReady);

		HandleChangeInitStateToGameplayReady(Manager);
	}
}

void UGamePhaseComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UInitStateComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == TAG_InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGamePhaseComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
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

TSubclassOf<UGamePhase> UGamePhaseComponent::GetCurrentGamePhaseClass() const
{
	return ActiveGamePhases.GetCurrentGamePhaseClass();
}


// Game Mode Option

bool UGamePhaseComponent::InitializeFromGameModeOption()
{
	if (!HasAuthority())
	{
		return false;
	}

	auto* GameMode{ GetWorld()->GetAuthGameMode() };
	if (ensure(GameMode))
	{
		auto OptionString{ GameMode->OptionsString };

		UE_LOG(LogGameExt_GamePhase, Log, TEXT("Initialize Game Phase From Game Mode Option"));

		if (UGameplayStatics::HasOption(OptionString, UGamePhaseComponent::NAME_GamePhaseOptionKey))
		{
			const auto PhaseClassPathFromOptions{ UGameplayStatics::ParseOption(OptionString, UGamePhaseComponent::NAME_GamePhaseOptionKey) };
			const auto PhaseClassPath{ FSoftClassPath(PhaseClassPathFromOptions) };

			UE_LOG(LogGameExt_GamePhase, Log, TEXT("| OptionValue: %s"), *PhaseClassPathFromOptions);
			UE_LOG(LogGameExt_GamePhase, Log, TEXT("| PhaseClass: %s"), *PhaseClassPath.ToString());

			if (auto* PhaseClass{ PhaseClassPath.TryLoadClass<UGamePhase>() })
			{
				return SetGamePhase(PhaseClass);
			}
		}
		else
		{
			UE_LOG(LogGameExt_GamePhase, Log, TEXT("| No Game Mode Option"));
		}
	}

	return false;
}

FString UGamePhaseComponent::ConstructGameModeOption() const
{
	auto PhaseClass{ GetCurrentGamePhaseClass() };

	if (PhaseClass)
	{
		FSoftClassPath Path{ PhaseClass };
		return FString::Printf(TEXT("?%s=%s"), *UGamePhaseComponent::NAME_GamePhaseOptionKey, *Path.GetAssetPathString());
	}

	return FString();
}


// Utilities

bool UGamePhaseComponent::HasAuthority() const
{
	auto* OwnerGameState{ GetOwner<AGameStateBase>() };

	return OwnerGameState ? OwnerGameState->HasAuthority() : false;
}
