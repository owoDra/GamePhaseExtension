// Copyright (C) 2024 owoDra

#pragma once

#include "Factories/Factory.h"

#include "GamePhaseBlueprintFactory.generated.h"


UCLASS(HideCategories=Object, MinimalAPI)
class UGamePhaseBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:
	UGamePhaseBlueprintFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// The type of blueprint that will be created
	//
	UPROPERTY(EditAnywhere, Category = "GamePhaseBlueprintFactory")
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	//
	// The parent class of the created blueprint
	//
	UPROPERTY(EditAnywhere, Category = "GamePhaseBlueprintFactory")
	TSubclassOf<class UGamePhase> ParentClass;

public:
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
