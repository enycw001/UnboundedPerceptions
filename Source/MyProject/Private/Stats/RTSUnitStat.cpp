// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "RTSUnitStat.h"

#include "MyAttributeSet.h"

CombatInfo::ModifyingAttribute& CombatInfo::ModifyingAttribute::operator=(const ModifyingAttribute& otherModifyingAttribute)
{
   attribute = otherModifyingAttribute.attribute;
   effectRatio = otherModifyingAttribute.effectRatio;
   return *this;
}

CombatInfo::RTSUnitStat::RTSUnitStat(FGameplayAttribute& attData) : attMod(ModifyingAttribute(&attData)), attribute{attData}
{
}

CombatInfo::RTSUnitStat::RTSUnitStat(FGameplayAttribute& att, int baseV, ModifyingAttribute mod, UMyAttributeSet* attSet) : attMod{mod}, attribute{att}
{
   auto attData = att.GetGameplayAttributeData(attSet);
   float newBaseV = baseV;
   attData->SetBaseValue(newBaseV);
   attSet->PreAttributeBaseChange(att, newBaseV);
}

CombatInfo::RTSUnitStat::~RTSUnitStat()
{
}

void CombatInfo::RTSUnitStat::ChangeModifier(ModifyingAttribute mod)
{
   attMod = mod;
}

// calculate base value
void CombatInfo::RTSUnitStat::CalculateModValue(UMyAttributeSet* attSet)
{
   // Add to our adjusted value the increased base value. Since we add without recalcuating the adjusted value, if the player wants to get large buffs they
   // should try to get a higher base first
   float currentBaseValue = GetBaseValue(attSet);
   float scaledValue = attMod.effectRatio(attMod.attribute->GetNumericValue(attSet));
   float adjustedDifference =  attribute.GetNumericValue(attSet) + scaledValue - currentBaseValue;

   attribute.GetGameplayAttributeData(attSet)->SetBaseValue(scaledValue);
   attSet->PreAttributeBaseChange(attribute, scaledValue);
   attribute.SetNumericValueChecked(adjustedDifference, attSet);
}

float CombatInfo::RTSUnitStat::GetBaseValue(UMyAttributeSet* attSet) const
{
   return attribute.GetGameplayAttributeData(attSet)->GetBaseValue();
}

void CombatInfo::RTSUnitStat::SetBaseValue(float value, UMyAttributeSet* attSet)
{
   auto attData = attribute.GetGameplayAttributeData(attSet);
   float oldBaseVal = attData->GetBaseValue();
   attData->SetBaseValue(value);
   attSet->PreAttributeBaseChange(attribute, value);
   float newCurrentValue = attribute.GetNumericValue(attSet) + value - oldBaseVal;
   attribute.SetNumericValueChecked(newCurrentValue, attSet);
}

float CombatInfo::RTSUnitStat::GetBuffValue(UMyAttributeSet* attSet) const
{
   auto attData = attribute.GetGameplayAttributeData(attSet);
   float baseVal = attData->GetBaseValue();
   return attribute.GetNumericValue(attSet) - baseVal;
}

float CombatInfo::RTSUnitStat::GetAdjustedValue(UMyAttributeSet* attSet) const
{
   return attribute.GetNumericValue(attSet);
}

void CombatInfo::RTSUnitStat::SetAdjustedValue(float value, UMyAttributeSet* attSet)
{
   attribute.SetNumericValueChecked(value, attSet);
}

void CombatInfo::RTSUnitStat::Update(UMyAttributeSet* attSet)
{
   CalculateModValue(attSet);
}
