#include "GOAT.h"
#include "Goal.h"


AGoal::AGoal() : Super()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = false;
    Tags.Add ("Goal");
}