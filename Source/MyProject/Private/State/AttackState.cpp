// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "AttackState.h"

AttackState::AttackState()
{
}

AttackState::~AttackState()
{
}

void AttackState::Enter(AUnit & unit)
{
}

void AttackState::Exit(AUnit & unit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("WEE"));
}

void AttackState::Update(AUnit & unit)
{
}