// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "PatrolComponent.h"
#include "AIController.h"
#include "AIControllers/NPCAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UpResourceManager.h"

UPatrolComponent::UPatrolComponent()
{
   PrimaryComponentTick.bCanEverTick = false;
   ConstructorHelpers::FObjectFinder<UBehaviorTree> patrolTreeRef(TEXT("/Game/RTS_Tutorial/Blueprints/AI/Patrol/PatrolBT"));
   if (patrolTreeRef.Succeeded()) patrolTree = patrolTreeRef.Object;
}

void UPatrolComponent::BeginPlay()
{
   Super::BeginPlay();
   APawn* pawnOwner = Cast<APawn>(GetOwner());
   check(pawnOwner);

   ownerControllerRef = Cast<AAIController>(pawnOwner->GetController());
   if (enabled) {
      ANPCAIController* aiController = Cast<ANPCAIController>(ownerControllerRef);
      aiController->Patrol();
      //UpResourceManager::ExecuteFunctionFromWorldObject(aiController, "Patrol");
   }
}

void UPatrolComponent::DeletePatrolPoint(int patrolIndex)
{
   if (patrolIndex >= 0 && patrolIndex <= patrolPoints.Num()) { patrolPoints.RemoveAt(patrolIndex, 1); }
}

EPathFollowingRequestResult::Type UPatrolComponent::MoveToNextPatrolPoint()
{
   currentPatrolIndex = (currentPatrolIndex + 1) % patrolPoints.Num();
   return ownerControllerRef->MoveToLocation(patrolPoints[currentPatrolIndex]);
}
