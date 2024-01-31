// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/Blueprint.h"

#include "GamePhaseBlueprint.generated.h"

/**
 * Blueprint class used to make game phase processes on the editor
 * 
 * Tips:
 *	The game phase factory should pick this for you automatically
 */
UCLASS(BlueprintType)
class GEPHASE_API UGamePhaseBlueprint : public UBlueprint
{
	GENERATED_BODY()
public:
	UGamePhaseBlueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////
	// Editor Only
public:
#if WITH_EDITOR
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
#endif

};
