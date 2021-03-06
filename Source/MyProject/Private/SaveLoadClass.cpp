// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "SaveLoadClass.h"

#include "UserInput.h"
#include "RTSPawn.h"
#include "BasePlayer.h"

#include "AIStuff/AIControllers/UnitController.h"

#include "WorldObjects/Ally.h"
#include "WorldObjects/Summon.h"
#include "Stats/BaseCharacter.h"
#include "WorldObjects/BaseHero.h"

#include "SpellSystem/MySpell.h"

#include "AssetRegistryModule.h"

#include "UpResourceManager.h"

#include "RTSGameMode.h"
#include "MyGameInstance.h"

CSV_DEFINE_CATEGORY(UpLevelLoading, false);

USaveLoadClass::~USaveLoadClass()
{
}

void USaveLoadClass::Init()
{
   controllerRef = Cast<AUserInput>(GetOuter()->GetWorld()->GetFirstPlayerController());
}

// TODO: Make struct with collection of level data (data on each level), struct with collection of quest data, struct with collection of trigger information

void USaveLoadClass::SetupSaveData()
{
   // TODO: Let player input id
   // saveGameData->id =

   // Append SaveGameInfo at the beginning of the save file, or read it when being loaded first.  Should be read when startup to show times of all the saves and loads
   gameSaveSaveData.timestamp = FDateTime::Now();

   SetupSaveControllerData();
   SetupSavePlayerData();
   SetupSaveHeroData();
   SetupSaveSummonData();
   SetupNPCEscortData();
   SetupLevelSaveData();
}

void USaveLoadClass::SetupSaveControllerData()
{
   // Save Scene
   sceneSaveData.levelName = *controllerRef->GetGameMode()->GetCurLevelName();
   // TODO: Save scene interactables
   // Save Camera
   cameraSaveData.cameraTransform  = controllerRef->GetCameraPawn()->GetTransform();
   cameraSaveData.cameraSpeed      = controllerRef->GetCameraPawn()->camMoveSpeedMultiplier;
   cameraSaveData.isCamNavDisabled = controllerRef->GetCameraPawn()->isCamNavDisabled;
}

void USaveLoadClass::SetupSavePlayerData()
{
   ABasePlayer* basePlayer  = controllerRef->GetBasePlayer();
   playerSaveData.money     = basePlayer->money;
   playerSaveData.heroNum   = basePlayer->heroes.Num();
   playerSaveData.summonNum = basePlayer->summons.Num();
   playerSaveData.npcNum    = basePlayer->npcs.Num();

   for (auto it = basePlayer->GetDialogTopics().CreateConstIterator(); *it == basePlayer->GetDialogTopics().Last(); ++it) {
      playerSaveData.dialogTopics.Add(it->GetTagName());
   }
}

void USaveLoadClass::SetupSaveBaseCharacterData(const FBaseCharacter& baseChar, FBaseCharacterSaveInfo& saveInfo)
{
   for (int i = 0; i < CombatInfo::AttCount; ++i) {
      saveInfo.attributes.Add(baseChar.GetAttribute(i)->GetBaseValue());
   }
   for (int i = 0; i < CombatInfo::StatCount; ++i) {
      saveInfo.skills.Add(baseChar.GetSkill(i)->GetBaseValue());
   }
   for (int i = 0; i < CombatInfo::VitalCount; ++i) {
      saveInfo.currentVitals.Add(baseChar.GetMechanic(i)->GetCurrentValue());
      saveInfo.vitals.Add(baseChar.GetMechanic(i)->GetBaseValue());
   }
   for (int i = 0; i < CombatInfo::MechanicCount; ++i) {
      saveInfo.mechanics.Add(baseChar.GetMechanic(i)->GetBaseValue());
   }
   saveInfo.level = baseChar.GetLevel();
}

void USaveLoadClass::SetupSaveAllyData(AAlly& ally, FAllySaveInfo& allyInfo)
{
   allyInfo.name           = ally.GetGameName();
   allyInfo.actorTransform = ally.GetTransform();
   // TODO: Move image data to DataAsset so we can reconfigure it when we load character

   SetupSaveBaseCharacterData(ally.GetBaseCharacter(), allyInfo.baseCSaveInfo);
}

void USaveLoadClass::SetupSaveSummonData()
{
   // Save data for every summon
   int i = 0;
   for (ASummon* summonRef : controllerRef->GetBasePlayer()->summons) {
      summonsSaveData.Emplace();
      SetupSaveAllyData(*summonRef, summonsSaveData[i].allyInfo);
      summonsSaveData[i].duration = summonRef->timeLeft;
      ++i;
   }
}

void USaveLoadClass::SetupSaveHeroData()
{
   for (int i = 0; i < playerSaveData.heroNum; ++i) {
      if (ABaseHero* heroRef = controllerRef->GetBasePlayer()->heroes[i]) // NULL check because heroes is always length 4 and can have some empty entries
      {
         heroesSaveData.Emplace();
         SetupSaveAllyData(*heroRef, heroesSaveData[i].allyInfo);
         heroesSaveData[i].currentExp     = heroRef->GetCurrentExp();
         heroesSaveData[i].expToNextLevel = heroRef->GetExpToLevel();
         heroesSaveData[i].attPoints      = heroRef->attPoints;
         heroesSaveData[i].skillPoints    = heroRef->skillPoints;

         // save our spells
         for (TSubclassOf<UMySpell> spell : heroRef->abilities) {
            // TODO: Properly setup namespace and keys for each spell in table
            if (spell.GetDefaultObject()) heroesSaveData[i].spellIDs.Add(spell.GetDefaultObject()->spellDefaults.id);
         }

         heroRef->backpack->SaveBackpack(heroesSaveData[i].backpackInfo);
      }
   }

   // TODO: Save these things as well!
   // save inventory
   // save equipment
   // save spellbook
}

void USaveLoadClass::SetupNPCEscortData()
{
   int i = 0;
   for (AAlly* npcRef : controllerRef->GetBasePlayer()->npcs) {
      npcsSaveData.Emplace();
      SetupSaveAllyData(*npcRef, npcsSaveData[i]);
      ++i;
   }
}

void USaveLoadClass::SetupLevelSaveData()
{
   controllerRef->GetMyGameInstance()->SaveLevelData(*controllerRef->GetGameMode()->GetCurLevelName());
   //Only save levels that has changed
   for(auto level : controllerRef->GetMyGameInstance()->savedLevels) {
      mapSaveData[level] = controllerRef->GetMyGameInstance()->mapInfo[level];
   }
   controllerRef->GetMyGameInstance()->savedLevels.Empty();
}

void USaveLoadClass::SetupLoad()
{
   SetupController();
   SetupPlayer();
   SetupAlliedUnits();
   SetupLevelLoadData();
}

void USaveLoadClass::SetupController()
{
   GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Testing to see if code can be called after level load!"));
   controllerRef = Cast<AUserInput>(GetOuter()->GetWorld()->GetFirstPlayerController());
   controllerRef->GetCameraPawn()->SetActorTransform(cameraSaveData.cameraTransform);
   controllerRef->GetCameraPawn()->camMoveSpeedMultiplier = cameraSaveData.cameraSpeed;
   controllerRef->GetCameraPawn()->isCamNavDisabled       = cameraSaveData.isCamNavDisabled;
}

void USaveLoadClass::SetupPlayer()
{
   for (FName dialogTopic : playerSaveData.dialogTopics) {
      controllerRef->GetBasePlayer()->LearnDialogTopic(FGameplayTag::RequestGameplayTag(dialogTopic));
   }

   Cast<ABasePlayer>(GetOuter()->GetWorld()->GetGameState()->PlayerArray[0])->money = playerSaveData.money;
}

void USaveLoadClass::SetupAlliedUnits()
{
   FActorSpawnParameters spawnParams;

   for (FAllySaveInfo finishedTalkNPC : npcsSaveData) {
      if (AAlly* spawnedNPCAlly = UpResourceManager::FindTriggerObjectInWorld<AAlly>(*finishedTalkNPC.name.ToString(), controllerRef->GetWorld())) {
         spawnedNPCAlly->SetActorTransform(finishedTalkNPC.actorTransform);
         SetupBaseCharacter(spawnedNPCAlly, finishedTalkNPC.baseCSaveInfo);
         spawnedNPCAlly->GetUnitController()->Stop();
      } else {
         FAssetRegistryModule& assetReg = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
         // TArray<FAssetData> allyAssets;
         // checkf(assetReg.Get().GetAssetsByPath("/Game/RTS_Tutorial/Blueprints/Actors/WorldObjects",allyAssets,false), TEXT("Cannot find ally assets to load, check validity of installation!"));
         // allyAssets.FindByPredicate([&](FAssetData assetData){ return *npc.name.ToString() == assetData.AssetName ;})->GetAsset();
         //!!!---INVARIANT: Names of files of characters must be the same name as their name in the game--!!!
         FAssetData npcAllyAsset = assetReg.Get().GetAssetByObjectPath(*(FString("/Game/RTS_Tutorial/Blueprints/Actors/WorldObjects/") + finishedTalkNPC.name.ToString()));
         spawnedNPCAlly          = controllerRef->GetWorld()->SpawnActorDeferred<AAlly>(npcAllyAsset.GetAsset()->GetClass(), finishedTalkNPC.actorTransform);
         SetupBaseCharacter(spawnedNPCAlly, finishedTalkNPC.baseCSaveInfo);
         spawnedNPCAlly->FinishSpawning(finishedTalkNPC.actorTransform);
      }
   }

   // Load hero specific information
   for (FHeroSaveInfo& heroSaveData : heroesSaveData) {
      if (ABaseHero* spawnedHero = UpResourceManager::FindTriggerObjectInWorld<ABaseHero>(*heroSaveData.allyInfo.name.ToString(), controllerRef->GetWorld())) {
         spawnedHero->SetActorTransform(heroSaveData.allyInfo.actorTransform);
         SetupBaseCharacter(spawnedHero, heroSaveData.allyInfo.baseCSaveInfo);
         for (int i = 0; i < heroSaveData.spellIDs.Num(); ++i) {
#if UE_EDITOR
            // conditionally compiled because in the real game we should make sure these spell classes exist before hand and shouldn't need to check
            // technically indexer checks but it crashes when it can't find key
            if (USpellManager::Get().spellClasses.Contains(heroSaveData.spellIDs[i]))
#endif
               spawnedHero->abilities[i] = USpellManager::Get().spellClasses[heroSaveData.spellIDs[i]];
         }
         spawnedHero->attPoints = heroSaveData.attPoints;
         spawnedHero->SetCurrentExp(heroSaveData.currentExp);
         spawnedHero->expForLevel = heroSaveData.expToNextLevel;
         spawnedHero->GetUnitController()->Stop();

         // Load items into backpack
         spawnedHero->backpack->LoadBackpack(heroSaveData.backpackInfo);
      } else {
         FAssetRegistryModule& assetReg  = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
         FAssetData            heroAsset = assetReg.Get().GetAssetByObjectPath(*(FString("/Game/RTS_Tutorial/Blueprints/Actors/WorldObjects/Allies") + heroSaveData.allyInfo.name.ToString()));
         spawnedHero                     = controllerRef->GetWorld()->SpawnActorDeferred<ABaseHero>(heroAsset.GetAsset()->GetClass(), heroSaveData.allyInfo.actorTransform);
         SetupBaseCharacter(spawnedHero, heroSaveData.allyInfo.baseCSaveInfo);
         for (int i = 0; i < heroSaveData.spellIDs.Num(); ++i) {
            spawnedHero->abilities[i] = USpellManager::Get().spellClasses[heroSaveData.spellIDs[i]];
         }
         spawnedHero->attPoints = heroSaveData.attPoints;
         spawnedHero->SetCurrentExp(heroSaveData.currentExp);
         spawnedHero->expForLevel = heroSaveData.expToNextLevel;
         spawnedHero->FinishSpawning(heroSaveData.allyInfo.actorTransform);

         for (int i = 0; i < heroSaveData.backpackInfo.itemIDs.Num(); ++i) {
            spawnedHero->backpack->AddItemToSlot(FMyItem(heroSaveData.backpackInfo.itemIDs[i], heroSaveData.backpackInfo.itemCounts[i]), heroSaveData.backpackInfo.itemSlots[i]);
         }
      }
   }

   for (FSummonSaveInfo summon : summonsSaveData) {
      if (ASummon* spawnedSummon = UpResourceManager::FindTriggerObjectInWorld<ASummon>(*summon.allyInfo.name.ToString(), controllerRef->GetWorld())) {
         spawnedSummon->SetActorTransform(summon.allyInfo.actorTransform);
         SetupBaseCharacter(spawnedSummon, summon.allyInfo.baseCSaveInfo);
         spawnedSummon->timeLeft = summon.duration;
         spawnedSummon->GetUnitController()->Stop();
      } else {
         FAssetRegistryModule& assetReg    = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
         FAssetData            summonAsset = assetReg.Get().GetAssetByObjectPath(*(FString("/Game/RTS_Tutorial/Blueprints/Actors/WorldObjects/") + summon.allyInfo.name.ToString()));
         spawnedSummon                     = controllerRef->GetWorld()->SpawnActorDeferred<ASummon>(summonAsset.GetAsset()->GetClass(), summon.allyInfo.actorTransform);
         spawnedSummon->SetActorTransform(summon.allyInfo.actorTransform);
         SetupBaseCharacter(spawnedSummon, summon.allyInfo.baseCSaveInfo);
         spawnedSummon->timeLeft = summon.duration;
         spawnedSummon->FinishSpawning(summon.allyInfo.actorTransform);
      }
   }
}

void USaveLoadClass::SetupBaseCharacter(AAlly* spawnedAlly, FBaseCharacterSaveInfo& baseCSaveInfo)
{
   for (int i = 0; i < baseCSaveInfo.attributes.Num(); ++i) {
      spawnedAlly->baseC->GetAttribute(i)->SetBaseValue(baseCSaveInfo.attributes[i]);
   }
   for (int i = 0; i < baseCSaveInfo.skills.Num(); ++i) {
      spawnedAlly->baseC->GetSkill(i)->SetBaseValue(baseCSaveInfo.skills[i]);
   }
   for (int i = 0; i < baseCSaveInfo.vitals.Num(); ++i) {
      spawnedAlly->baseC->GetVital(i)->SetBaseValue(baseCSaveInfo.vitals[i]);
   }
   for (int i = 0; i < baseCSaveInfo.mechanics.Num(); ++i) {
      spawnedAlly->baseC->GetMechanic(i)->SetBaseValue(baseCSaveInfo.mechanics[i]);
   }
}

void USaveLoadClass::SetupLevelLoadData()
{
  controllerRef->GetMyGameInstance()->mapInfo = mapSaveData;
}

void USaveLoadClass::SaveLoadFunction(FArchive& ar, bool isSaving)
{
   /*Slow since we have to construct and assign an empty copy, but this way is safer than the alternatives which could involve looping with manually inputted indices
    *and having to redelete information
    *TODO: Possible Optimizations
    *Save defaults and just copy the default values so we don't have to reconstruct
    *Use arrays of preset sizes that will never overflow and keep overwriting and reading to a section of the data to prevent reconstruction*/

   gameSaveSaveData = FSaveGameDataInfo();
   sceneSaveData    = FSceneSaveInfo();
   cameraSaveData   = FCameraSaveInfo();
   playerSaveData   = FBasePlayerSaveInfo();
   heroesSaveData   = TArray<FHeroSaveInfo>();
   summonsSaveData  = TArray<FSummonSaveInfo>();
   npcsSaveData     = TArray<FAllySaveInfo>();
   // Don't need to reset mapData since not all of it will be changed at once mapSaveData      = TMap<FName, FMapSaveInfo>();

   //If saving, load all the structs with information
   if (isSaving)
      SetupSaveData();

   //If saving, store all information from structs to archive.  If loading, operator overloaded to load all information from binary array to structs
   ar << gameSaveSaveData;
   ar << sceneSaveData;
   ar << cameraSaveData;
   ar << playerSaveData;
   ar << heroesSaveData;
   ar << summonsSaveData;
   ar << npcsSaveData;
   ar << mapSaveData;

   //If loading, use loaded structs to restore world state
   if(!isSaving)
      SetupLoad();
}

bool USaveLoadClass::SaveToFilePath(const FString& filePath)
{
   FBufferArchive binaryArray;

   SaveLoadFunction(binaryArray, true);

   if (binaryArray.Num() <= 0) return false;

   if (FFileHelper::SaveArrayToFile(binaryArray, *filePath)) {
      binaryArray.FlushCache();
      binaryArray.Empty();
      // Client message from Controller
      controllerRef->ClientMessage("Save Success!", NAME_None, 2.f);
      return true;
   }

   // Free Binary ARray
   binaryArray.FlushCache();
   binaryArray.Empty();
   controllerRef->ClientMessage("File Could Not Be Saved!", NAME_None, 2.f);
   return false;
}

bool USaveLoadClass::LoadFromFilePath(const FString& filePath)
{
   TArray<uint8> binaryArray;
   if (!FFileHelper::LoadFileToArray(binaryArray, *filePath)) {
      controllerRef->ClientMessage("FFILEHELPER:>> Invalid File");
      return false;
   }

   // Testing
   controllerRef->ClientMessage("Loaded file size", NAME_None, 2.f);
   controllerRef->ClientMessage(FString::FromInt(binaryArray.Num()), NAME_None, 2.f);

   if (binaryArray.Num() <= 0) return false;
   FMemoryReader fromBinary = FMemoryReader(binaryArray, true); // free data after done
   fromBinary.Seek(0);

   SaveLoadFunction(fromBinary, false);

   fromBinary.FlushCache();
   binaryArray.Empty();
   fromBinary.Close();

   controllerRef->GetGameMode()->StreamLevelAsync(sceneSaveData.levelName);
   return true;
}
