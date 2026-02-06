#include "NoClip.hpp"
#include "game/frontend/Menu.hpp"  // For speed slider

namespace NoClip
{
    void Apply(SDK::ASBZPlayerCharacter* pLocalPlayerPawn, SDK::USBZPlayerMovementComponent* pMovementComponent)
    {
        if (!pLocalPlayerPawn || !pMovementComponent)
            return;

        // Enable noclip mode
        if (pLocalPlayerPawn->GetActorEnableCollision())
            pLocalPlayerPawn->SetActorEnableCollision(false);

        pMovementComponent->MovementMode = SDK::EMovementMode::MOVE_Flying;
        
        // Disable gravity and increase control
        pMovementComponent->GravityScale = 0.0f;
        pMovementComponent->BrakingDecelerationFlying = 8000.0f;  // High deceleration = stops quickly
        
        // Use speed multiplier from menu slider (increased base speed for faster movement)
        float baseSpeed = 2000.0f;  // Increased from 1000.0f
        pMovementComponent->MaxFlySpeed = baseSpeed * Menu::g_fNoClipSpeed;
        pMovementComponent->MaxAcceleration = 10000.0f;  // Faster acceleration
        
        // Reset velocity when not moving (prevents drifting)
        SDK::FVector currentVelocity = pMovementComponent->Velocity;
        float velocityMagnitude = sqrtf(currentVelocity.X * currentVelocity.X + 
                                         currentVelocity.Y * currentVelocity.Y + 
                                         currentVelocity.Z * currentVelocity.Z);
        
        // If moving very slowly, stop completely (prevents floating)
        if (velocityMagnitude < 50.0f) {
            pMovementComponent->Velocity = SDK::FVector(0.0f, 0.0f, 0.0f);
        }
    }

    void Restore(SDK::ASBZPlayerCharacter* pLocalPlayerPawn, SDK::USBZPlayerMovementComponent* pMovementComponent)
    {
        if (!pLocalPlayerPawn || !pMovementComponent)
            return;

        // Disable noclip - restore normal movement
        pLocalPlayerPawn->SetActorEnableCollision(true);
        pMovementComponent->MovementMode = SDK::EMovementMode::MOVE_Walking;
        pMovementComponent->GravityScale = 1.0f;  // Restore gravity
        pMovementComponent->Velocity = SDK::FVector(0.0f, 0.0f, 0.0f);  // Stop movement
    }
}
