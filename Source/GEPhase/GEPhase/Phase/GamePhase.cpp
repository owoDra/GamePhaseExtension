// Copyright (C) 2024 owoDra

#include "GamePhase.h"

#include "GameFramework/GameStateBase.h"

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


bool UGamePhase::NextGamePhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	return false;
}

bool UGamePhase::StartSubPhase(TSubclassOf<UGamePhase> GamePhaseClass)
{
	return false;
}

bool UGamePhase::EndPhase()
{
	return false;
}


void UGamePhase::HandleGamePhaseStart()
{
	OnGamePhaseStart();
}

void UGamePhase::HandleGamePhaseEnd()
{
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
