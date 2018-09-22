// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "AIParamUpdateService.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Unit.h"

void UAIParamUpdateService::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("TESTING"));
}

void UAIParamUpdateService::TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	Super::TickNode(ownerComp, nodeMemory, deltaSeconds);
	FBTAIParamUpdateServiceMemory* myMemory = (FBTAIParamUpdateServiceMemory*)nodeMemory;

	int riskValue = 1;
	int threatValue = 1;

	float unitTotalHealth = myMemory->owner->GetVitalAdjValue(static_cast<int>(Vitals::Health));

	threatValue += myMemory->owner->GetDPS(3);
	//threatValue += p.stunDealt;
	threatValue += myMemory->owner->GetHealingPerSecond(3) * 2; //prioritze healers, and maybe life leeching?

	riskValue += myMemory->owner->GetDamageRecievedPerSecond(3);
	riskValue -= myMemory->owner->GetHealingRecievedPerSecond(3);
	riskValue *= myMemory->owner->GetVitalCurValue(static_cast<int>(Vitals::Health)) / unitTotalHealth;

	ownerComp.GetBlackboardComponent()->SetValueAsInt("risk",riskValue);
	ownerComp.GetBlackboardComponent()->SetValueAsInt("risk",threatValue);
}

void UAIParamUpdateService::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{

}

FString UAIParamUpdateService::GetStaticDescription() const
{
	return "Recalculates AI risk assessment scores.";
}

