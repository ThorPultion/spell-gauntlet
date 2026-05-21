// YourAttributeSet.h
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // Required for the macros
#include "SpellAttributeSet.generated.h"

UCLASS()
class SPELLGAUNTLET_API USpellAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    USpellAttributeSet();

    // --- Attribute: Health ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", ReplicatedUsing=OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS_BASIC(USpellAttributeSet, Health);

    // --- Attribute: MaxHealth ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS_BASIC(USpellAttributeSet, MaxHealth);

    // --- Attribute: Mana ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS_BASIC(USpellAttributeSet, Mana);

    // --- Attribute: MaxMana ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS_BASIC(USpellAttributeSet, MaxMana);


    void KillPlayer(const FGameplayEffectModCallbackData& Data);

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue) const
    {
        GAMEPLAYATTRIBUTE_REPNOTIFY(USpellAttributeSet, Health, OldValue);
    }
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
    {
        GAMEPLAYATTRIBUTE_REPNOTIFY(USpellAttributeSet, MaxHealth, OldValue);
    }
    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldValue) const
    {
        GAMEPLAYATTRIBUTE_REPNOTIFY(USpellAttributeSet, Mana, OldValue);
    }
    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
    {
        GAMEPLAYATTRIBUTE_REPNOTIFY(USpellAttributeSet, MaxMana, OldValue);
    }

    // This function is called just before an attribute is changed
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // Here is where we actually change attributes and handle the results of it
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;
};