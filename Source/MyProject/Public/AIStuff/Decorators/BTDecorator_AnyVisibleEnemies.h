// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_AnyVisibleEnemies.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UBTDecorator_AnyVisibleEnemies : public UBTDecorator
{
	GENERATED_BODY()
	
	UBTDecorator_AnyVisibleEnemies();

	/** Is AIController owner an enemy or friendly? */
	UPROPERTY(Category=Decorator, EditAnywhere)
	bool					isEnemy;
	
	/** cached description */
	UPROPERTY()
	FString					CachedDescription;

	virtual bool			CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void			DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString			GetStaticDescription() const override;

#if WITH_EDITOR
	void					BuildDescription();
	virtual					void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};