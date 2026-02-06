#pragma once

#include "types/gameoffsets/SDK.hpp"

namespace NoClip
{
    // Apply noclip to the player
    void Apply(SDK::ASBZPlayerCharacter* pLocalPlayerPawn, SDK::USBZPlayerMovementComponent* pMovementComponent);
    
    // Restore normal movement
    void Restore(SDK::ASBZPlayerCharacter* pLocalPlayerPawn, SDK::USBZPlayerMovementComponent* pMovementComponent);
}
