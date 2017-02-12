#include "GOAT.h"
#include "FastKick.h"


UFastKick::UFastKick() : Super()
{
    GUIName = "Fast Kick";
    BaseCooldown = 2.f;
    KickingForceMultiplier = 1.5f;
}
