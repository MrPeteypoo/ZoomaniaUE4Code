// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterStats.generated.h"


USTRUCT(BlueprintType)
struct GOAT_API FCharacterStats
{
	GENERATED_BODY()

    /** Effects how fast the character can move. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Stats")
    float MovementSpeed = 0.f;

    /** Effects the base force applied to objects when kicked. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Stats")
    float KickingForce = 0.f;
    
    /** Not used anywhere yet. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Stats")
    float BuffTimeMultiplier = 1.f;

    /** Effects how long adverse effects such as stunning effect the character. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Stats")
    float DebuffTimeMultiplier = 1.f;
};
