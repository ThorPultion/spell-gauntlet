// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/CoreSystems/SpellAssetManager.h"
#include "AbilitySystemGlobals.h"

void USpellAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// For syncing ability system with tags declared in C++
	UAbilitySystemGlobals::Get().InitGlobalData();
}