#pragma once
#include "structs.h"
#include "Vector2f.h"
#include <vector>
#include "Texture.h"
#include <cmath>
#include <chrono>
#include "BaseMeleeWeapon.h"
#include <array> // Add this include to ensure std::array is fully defined    #include <array> // Add this include to ensure std::array is fully defined
#include <string>

class Player
{
public:

    enum class Upgrade { M_DAMAGE, R_DAMAGE, M_RANGE, R_Range, F_RATE, B_PIERCE, MAX_HEALTH, MOVE_SPEED };

    Player(float posX, float posY, float health);
    ~Player();

    void Draw(); // Draws player + health bar
    void Update(float elapsedSec); // Added for I-frame timer
    void DrawHealthBar() const; // Just the health bar
    void HandleMovement(float elapsedSec, const Uint8* pStates);
    Vector2f GetPosition() const;
    Rectf GetPlayerRect() const;
    void TakeDamage(int amount);
	void DrawWeaponCircle() const; // Draws the weapon circle
    float GetHealth() const;
    bool IsDead() const { return m_Health <= 0; } // Game-over check
    bool IsInvincible() const { return m_Invincible; }
	float GetCurrentXP() const { return m_XP; }
	float GetXPToNextLevel() const { return m_XPToNextLevel; }
	float GetMaxHealth() const { return m_MaxHealth; } // Accessor for max health

    void GainXP(int xp);
    int GetLevel() const { return m_Level; }

    std::array<Upgrade, 3> GenerateUpgradeOptions() const;
    void ApplyUpgrade(Upgrade up);
    void TryPickUpWeapon(const Vector2f& mousePos);

    void UpdateWeapons(float elapsedSec, const Vector2f& mousePos);

private:
    enum class WalkingState { none, down, left, right, up };
    WalkingState m_WalkingState;
    Texture* m_pSpriteSheet;
    Vector2f m_Position;
    float m_Health;
    float m_MaxHealth{ 100.f }; // Track max health for the bar
    float m_MoveSpeed{ 100.0f };

	float m_PickupRadius{ 20.0f }; // Radius for picking up items
	std::vector<BaseMeleeWeapon> m_MeleeWeapons; // List of weapons
    // I-frame variables
    bool m_Invincible{ false };
    float m_InvincibilityDuration{ 1.0f }; // 1 second of I-frames
    float m_InvincibilityTimer{ 0.0f };
    bool m_IsFlashing{ false };
    float m_FlashTimer{ 0.0f };
    const float m_FlashInterval{ 0.1f }; // Flash every 0.1 seconds

    Rectf GetCurrFrameRect() const
    {
        int m_FrameNr{ 0 };
        const int frameWidth{ 32 };
        const int frameHeight{ 32 };
        int frameIdleRow{ 0 };
        const int maxFrames{ 4 };
        const int maxWalkingFrames{ 16 };

        if (m_WalkingState == WalkingState::none)
        {
            m_FrameNr = static_cast<int>(fmod((SDL_GetTicks() / 100) * 0.5, maxFrames));
        }
        else
        {
            m_FrameNr = static_cast<int>(fmod((SDL_GetTicks() / 100) * 0.5, maxWalkingFrames));
        }

        switch (m_WalkingState)
        {
        case WalkingState::down:  frameIdleRow = 2; break;
        case WalkingState::left:  frameIdleRow = 3; break;
        case WalkingState::right: frameIdleRow = 2; break;
        case WalkingState::up:    frameIdleRow = 2; break;
        case WalkingState::none:  frameIdleRow = 0; break;
        }

        return Rectf{
            static_cast<float>(m_FrameNr * frameWidth),
            static_cast<float>(frameIdleRow * frameHeight),
            static_cast<float>(frameWidth),
            static_cast<float>(frameHeight)
        };
    }
    int m_Level{ 1 };
    int m_XP{ 0 };
    int m_XPToNextLevel{ 100 };

    // Upgradeable stats
    float m_MeleeDamage;
    float m_RangedDamage;
    float m_MeleeRange;
	float m_RangedRange;
    float m_FireRate;
    int   m_BulletPierce{ 1 };

    // Level-up handlers
    void CheckLevelUp();
    void LevelUp();
    void ChooseUpgrade();
};