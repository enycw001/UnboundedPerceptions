// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MySpell.h"
#include "SpellManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "GameplayEffects/DamageEffect.h"

USpellManager* USpellManager::SingletonManager = nullptr;

USpellManager::USpellManager()
{
   static ConstructorHelpers::FObjectFinder<UDataTable> SpellLookupTableFinder(TEXT("/Game/RTS_Tutorial/Tables/SpellList"));
   // Caching the data table information can be problematic if we reimport
   if (SpellLookupTableFinder.Object) SetupSpells(SpellLookupTableFinder.Object);
}

void USpellManager::SetupSpells(UDataTable* spellLookupTable)
{
   static const FString ContextString(TEXT("GENERAL"));
   if (spellLookupTable) {
      TArray<FSpellbookLookupRow*> table;
      TArray<FName>                rowNames = spellLookupTable->GetRowNames(); // these are the ids since default row name is just row number
      FSpellInfo                   spellInfo;

      spellLookupTable->GetAllRows<FSpellbookLookupRow>(ContextString, table);
      // UE_LOG(LogTemp, Warning, TEXT("%d"), rowNames.Num());
      if (rowNames.Num() > 0 && table.Num() > 0) {
         int rowName;
         for (int i = 0; i < rowNames.Num(); ++i) {
            rowName              = FCString::Atoi(*rowNames[i].ToString());
            spellInfo.name       = table[i]->Name;
            spellInfo.elem       = table[i]->Elem;
            spellInfo.desc       = table[i]->Desc;
            spellInfo.casttime   = table[i]->Casttime;
            spellInfo.secondaryTime = table[i]->SecondaryTime;
            spellInfo.cdDuration = table[i]->Cooldown;
            spellInfo.maxLevel   = table[i]->MaxLevel;
            spellInfo.range      = table[i]->Range;
            spellInfo.reqLevel   = table[i]->LevelReq;
            spellInfo.cost       = table[i]->Cost;
            spellInfo.targetting = table[i]->Targetting;
            spellInfo.duration   = table[i]->Duration;
            spellInfo.damage     = table[i]->Damage;
            spellInfo.period     = table[i]->Period;
            spellInfo.AOE        = table[i]->AOE;
            spellInfo.preReqs    = table[i]->PreReqs;

            spellClasses.Add(rowName, table[i]->spellClass);
            spells.Add(rowName, spellInfo);
         }
      }
   }
}

void USpellManager::InitializeManager()
{
   check(!SingletonManager);
   SingletonManager = NewObject<USpellManager>(GetTransientPackage(), NAME_None);
   SingletonManager->AddToRoot();
}
