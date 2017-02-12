#pragma once

#include "Engine/TriggerBox.h"
#include "Characters/Team.h"
#include "Goal.generated.h"

UCLASS(abstract)
class GOAT_API AGoal : public ATriggerBox
{
    GENERATED_BODY()
    
public: 

    /** Stops the goal ticking and adds the "Goal" tag. */
    AGoal();

    /** Gets the team that the goal is linked to. */
    UFUNCTION (BlueprintPure, Category = "Goal")
    ETeam GetTeam() const { return Team; }

protected:

    /** The team that the goal is linked to. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Goal")
    ETeam Team = ETeam::None;
};