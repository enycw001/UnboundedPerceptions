// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "IntimateNPC.h"
#include "RTSGameMode.h"
#include "UserInput.h"
#include "UI/HUDManager.h"
#include "DialogSystem/DialogUI.h"
#include "LevelSaveStructs.h"

AIntimateNPC::AIntimateNPC() : ANPC()
{
	
}

void AIntimateNPC::BeginPlay()
{
	Super::BeginPlay();
	gameModeRef = Cast<ARTSGameMode>(GetGameInstance());
}

void AIntimateNPC::SetRelationshipPoints(int pointsToAdd)
{
	relationshipPoints += pointsToAdd;
	if (relationshipPoints > relationshipEventPointValues[currentRelationshipEventIndex])
	{
		gameModeRef->GetTriggerManager()->ActivateTrigger(relationshipTriggers[currentRelationshipEventIndex]);
	}
}

void AIntimateNPC::Interact_Implementation(ABaseHero* hero)
{
	Super::Interact_Implementation(hero);
}

void AIntimateNPC::SetupAppropriateView()
{
	controllerRef->GetHUDManager()->GetSocialWindow()->SetIntimateView();
}

void AIntimateNPC::SaveNPCData(FMapSaveInfo& mapInfo)
{
	FNPCIntimateSaveInfo intimateNPCSaveInfo;
	MakeNPCData(intimateNPCSaveInfo.npcInfo);
	intimateNPCSaveInfo.currentRelationshipEventIndex = currentRelationshipEventIndex;
	intimateNPCSaveInfo.relationshipPoints = relationshipPoints;

	mapInfo.intimateNPCInfo.Add(intimateNPCSaveInfo);
}

void AIntimateNPC::LoadNPCData(FNPCIntimateSaveInfo& npcSaveInfo)
{
	ANPC::LoadNPCData(npcSaveInfo.npcInfo);
	relationshipPoints = npcSaveInfo.relationshipPoints;
	currentRelationshipEventIndex = npcSaveInfo.currentRelationshipEventIndex;
	
}

