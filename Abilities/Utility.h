#pragma once

#include "Vector.h"

class AActor;

/** Checks if the given actors are spherically in range of each other. */
bool CheckIsInRange (const AActor& primary, const AActor& secondary, float range);

/** 
    Ensures the given actors and close enough together to be in range and then checks the perpendicularity of the
    primary actors forward vector and the direction to the second actor. The perpendicularity should be 1 if
    perpendicular and -1 if opposing.
*/
bool CheckIsReachable (const AActor& primary, const AActor& secondary, float range, float perpendicularity);

/** 
    Compares the perpendicularity factor between the primary actors forward vector and the direction to the second
    actor. Perpendicularity is 1 if perpendicular and -1 if opposing.
*/
bool CheckPerpendicularity (const AActor& primary, const AActor& secondary, float perpendicularity);

/** Returns the direction vector from the primary actor to the secondary actor. */
FVector DirectionTo (const AActor& primary, const AActor& secondary);

/** 
    Given a collection of functions, each function will be called and their return value will be checked to see if it
    is zero. Each function should take no parameters and should return an FVector. If all values are zero, a zero
    vector will be returned.
*/
template <typename Function, typename... Functions>
FVector FirstNonZeroVector (const Function& function, const Functions&... functions);

template <typename Function>
FVector FirstNonZeroVector (const Function& function)
{
    return function();
}

template <typename Function, typename... Functions>
FVector FirstNonZeroVector (const Function& function, const Functions&... functions)
{
    FVector result = function();
    if (!result.IsZero())
    {
        return result;
    }
    
    return FirstNonZeroVector (functions...);
}