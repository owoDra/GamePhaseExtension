// Copyright (C) 2024 owoDra

#include "GamePhase.h"

#include "GamePhaseComponent.h"
#include "GEPhaseLogs.h"

#include "GameFramework/GameStateBase.h"
#include "GameplayTask.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePhase)


UGamePhase::UGamePhase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
EDataValidationResult UGamePhase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!GamePhaseTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		Context.AddError(FText::FromString(FString::Printf(TEXT("GamePhaseTag must be set to a tag representing the current phase."))));
	}

	return Result;
}
#endif


void UGamePhase::InitializeGamePhase(AGameStateBase* GameState, UGamePhaseComponent* GamePhaseComponent)
{
	Owner = GameState;
	OwnerComponent = GamePhaseComponent;
}


UGameplayTasksComponent* UGamePhase::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return OwnerComponent.Get();
}

AActor* UGamePhase::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return Owner.Get();
}

AActor* UGamePhase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return Owner.Get();
}

void UGamePhase::OnGameplayTaskActivated(UGameplayTask& Task)
{
	UE_LOG(LogGameExt_GamePhaseTask, Log, TEXT("Game phase task started: %s"), *Task.GetName());

	ActiveTasks.Add(&Task);
}

void UGamePhase::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(LogGameExt_GamePhaseTask, Log, TEXT("Game phase task ended: %s"), *Task.GetName());

	ActiveTasks.Remove(&Task);
}


bool UGamePhase::NextGamePhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	check(OwnerComponent.IsValid());

	return OwnerComponent->SetGamePhase(GamePhaseClass);
}

bool UGamePhase::StartSubPhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	check(OwnerComponent.IsValid());

	return OwnerComponent->AddSubPhase(GamePhaseClass, GetGamePhaseTag());
}

bool UGamePhase::EndPhase()
{
	check(OwnerComponent.IsValid());

	return OwnerComponent->EndPhaseByTag(GetGamePhaseTag());
}


void UGamePhase::HandleGamePhaseStart()
{
	UE_LOG(LogGameExt_GamePhase, Log, TEXT("[%s] Game phase started: %s")
		, HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")
		, *GetNameSafe(this));

	OnGamePhaseStart();
}

void UGamePhase::HandleGamePhaseEnd()
{
	UE_LOG(LogGameExt_GamePhase, Log, TEXT("[%s] Game phase ended: %s")
		, HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")
		, *GetNameSafe(this));

	// End tasks

	for (auto TaskIdx{ ActiveTasks.Num() - 1 }; (TaskIdx >= 0) && (ActiveTasks.Num() > 0); --TaskIdx)
	{
		if (auto Task{ ActiveTasks[TaskIdx] })
		{
			Task->TaskOwnerEnded();
		}
	}

	ActiveTasks.Reset();

	OnGamePhaseEnd();
}

void UGamePhase::HandleSubPhaseStart(const FGameplayTag& SubPhaseTag)
{
	OnSubPhaseStart(SubPhaseTag);
}

void UGamePhase::HandleSubPhaseEnd(const FGameplayTag& SubPhaseTag)
{
	OnSubPhaseEnd(SubPhaseTag);
}


UWorld* UGamePhase::GetWorld() const
{
	return Owner.IsValid() ? Owner->GetWorld() : nullptr;
}

AGameStateBase* UGamePhase::GetGameState(TSubclassOf<AGameStateBase> InClass) const
{
	return Owner.Get();
}

bool UGamePhase::HasAuthority() const
{
	return Owner.IsValid() ? Owner->HasAuthority() : false;
}
