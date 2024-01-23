// Copyright (C) 2024 owoDra

#include "GameFeatureAction_InitialGamePhase.h"

#include "GamePhaseComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameStateBase.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_InitialGamePhase)

///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_InitialGamePhase::IsDataValid(FDataValidationContext& Context) const
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid) };

	if (!InitialGamePhase)
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

		Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid InitialGamePhase defined in %s"), *GetNameSafe(this))));
	}

	return Result;
}
#endif


void UGameFeatureAction_InitialGamePhase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensureAlways(ActiveData.GameStateToAdded.IsEmpty()) ||
		!ensureAlways(ActiveData.ComponentRequests.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_InitialGamePhase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}


void UGameFeatureAction_InitialGamePhase::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	const auto bIsGameWorld{ World ? World->IsGameWorld() : false };

	auto GameInstance{ WorldContext.OwningGameInstance };
	auto* ComponentManager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) };

	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if (ComponentManager && bIsGameWorld)
	{
		if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
		{			
			auto NewDelegate{ UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &UGameFeatureAction_InitialGamePhase::HandleActorExtension, ChangeContext) };
			auto ExtensionRequestHandle{ Manager->AddExtensionHandler(AGameStateBase::StaticClass(), NewDelegate)};

			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}


void UGameFeatureAction_InitialGamePhase::Reset(FPerContextData& ActiveData)
{
	ActiveData.ComponentRequests.Empty();

	while (!ActiveData.GameStateToAdded.IsEmpty())
	{
		auto GameStatePtr{ ActiveData.GameStateToAdded.Top() };

		if (GameStatePtr.IsValid())
		{
			RemoveContextData(GameStatePtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.GameStateToAdded.Pop();
		}
	}
}

void UGameFeatureAction_InitialGamePhase::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto* ActiveData{ ContextData.Find(ChangeContext) };
	auto* AsGameState{ Cast<AGameStateBase>(Actor) };

	if (ActiveData)
	{
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
		{
			RemoveContextData(AsGameState, *ActiveData);
		}
		else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGamePhaseComponent::NAME_GamePhaseReady))
		{
			SetInitialGamePhase(AsGameState, *ActiveData);
		}
	}
}

void UGameFeatureAction_InitialGamePhase::SetInitialGamePhase(AGameStateBase* GameState, FPerContextData& ActiveData)
{
	check(GameState);

	if (GameState->HasAuthority())
	{
		if (auto* Component{ GameState->FindComponentByClass<UGamePhaseComponent>() })
		{
			Component->SetGamePhase(InitialGamePhase);
		}
	}

	ActiveData.GameStateToAdded.Add(GameState);
}

void UGameFeatureAction_InitialGamePhase::RemoveContextData(AGameStateBase* GameState, FPerContextData& ActiveData)
{
	check(GameState);

	ActiveData.GameStateToAdded.Remove(GameState);
}
