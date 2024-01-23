// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_InitialGamePhase.generated.h"

class UGamePhase;
class AGameStateBase;
struct FComponentRequestHandle;


/**
 * GameFeatureAction to set the first game phase in game mode
 */
UCLASS(meta = (DisplayName = "Initial Game Phase"))
class UGameFeatureAction_InitialGamePhase final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_InitialGamePhase() {}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

private:
	struct FPerContextData
	{
		TArray<TWeakObjectPtr<AGameStateBase>> GameStateToAdded;
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

protected:
	UPROPERTY(EditAnywhere, Category = "GamePhase")
	TSubclassOf<UGamePhase> InitialGamePhase{ nullptr };

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void SetInitialGamePhase(AGameStateBase* GameState, FPerContextData& ActiveData);
	void RemoveContextData(AGameStateBase* GameState, FPerContextData& ActiveData);

};
