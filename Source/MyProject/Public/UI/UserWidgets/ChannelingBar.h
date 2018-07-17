// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChannelingBar.generated.h"

/**
 * A channeling bar that is part of the actionbar 
 */

class AUnit;
class AUserInput;

UCLASS()
class MYPROJECT_API UChannelingBar : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "References", Meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	AUserInput*				controllerRef;

public:

	/**Gets the name of the spell/item currently being cast*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interface")
	FText					GetChannelingName();

	/**Get the percentage representing how much of the spell is currently cast*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interface")
	float					GetSpellChannelProgress();

	/**If the focused unit is channeling a spell, then this bar is visible.  Else it's not.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interface")
	ESlateVisibility		IsFocusedUnitChanneling();

};