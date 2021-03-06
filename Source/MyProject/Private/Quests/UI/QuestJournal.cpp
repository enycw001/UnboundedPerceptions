// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "QuestJournal.h"
#include "QuestJournalEntry.h"
#include "RTSGameMode.h"
#include "../QuestManager.h"

void UQuestJournal::NativeConstruct()
{  
   Super::NativeConstruct();
   gameModeRef = CPC->GetGameMode();
   if (gameModeRef->GetQuestManager()) { gameModeRef->GetQuestManager()->questJournalRef = this; }
}

bool UQuestJournal::OnWidgetAddToViewport_Implementation()
{
   return true;
}

void UQuestJournal::OnQuestEntryClicked(AQuest* quest, UQuestJournalEntry* questButton)
{
   if (currentQuestWidget) currentQuestWidget->ToggleButtonEnabled(true);

   if (IsValid(quest) && IsValid(questButton)) {
      selectedQuest      = quest;
      currentQuestWidget = questButton;
      questButton->ToggleButtonEnabled(false);
   } else // reset everything
   {
      selectedQuest      = nullptr;
      currentQuestWidget = nullptr;
   }

   UpdateDetailWindow();
}

UQuestJournalEntry* UQuestJournal::GetQuestJournalEntry(AQuest* quest)
{
   auto pred  = [&](UQuestJournalEntry* questEntry) { return questEntry->assignedQuest == quest; };
   int  index = questJournalEntries.IndexOfByPredicate(pred);
   if (index != INDEX_NONE) return questJournalEntries[questJournalEntries.IndexOfByPredicate(pred)];
   return nullptr;
}
