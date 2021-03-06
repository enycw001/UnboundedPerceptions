// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TriggerInteractableDecorator.h"
#include "RTSGameMode.h"

UTriggerInteractableDecorator::UTriggerInteractableDecorator()
{
}

void UTriggerInteractableDecorator::Init()
{
   UInteractableActorDecoratorBase::Init();
   gameModeRef = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());
}

bool UTriggerInteractableDecorator::Interact()
{
   if (!decoratedInteractable || decoratedInteractable->Interact()) {
      for (FTriggerData& finishedTriggerActivation : triggersActivatedOnInteract) {
         gameModeRef->GetTriggerManager()->ActivateTrigger(finishedTriggerActivation);
      }
      return true;
   }
   return false;
}
