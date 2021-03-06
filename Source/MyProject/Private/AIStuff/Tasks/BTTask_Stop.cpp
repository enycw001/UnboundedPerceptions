// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"

#include "BTTask_Stop.h"

#include "UnitController.h"

UBTTask_Stop::UBTTask_Stop()
{
   NodeName = "Stop";
   bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_Stop::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
   AUnitController* unitC = Cast<AUnitController>(ownerComp.GetAIOwner());
   unitC->Stop();
   return EBTNodeResult::Succeeded;
}
