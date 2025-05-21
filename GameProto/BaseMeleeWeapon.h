#pragma once

#include "Vector2f.h"

class Enemy;

class BaseMeleeWeapon
{
public:
    // Rule of Five
    BaseMeleeWeapon();
    virtual ~BaseMeleeWeapon();
    BaseMeleeWeapon(const BaseMeleeWeapon& other);
    BaseMeleeWeapon& operator=(const BaseMeleeWeapon& other);
    BaseMeleeWeapon(BaseMeleeWeapon&& other) noexcept;
    BaseMeleeWeapon& operator=(BaseMeleeWeapon&& other) noexcept;

    // Spawning and pickup
    void SpawnRandom(float screenWidth, float screenHeight);
    bool CheckPlayerCollision(const Vector2f& playerPos, float playerRadius) const;
    void AttachToPlayer(const Vector2f& playerPos, const Vector2f& mousePos);

    // Per-frame logic
    void Update(float elapsedSec, const Vector2f& playerPos, const Vector2f& mousePos);
    void Draw() const;

    // Combat
    bool CheckEnemyCollision(const Enemy* pEnemy) const;
    void DealDamage(Enemy* pEnemy) const;

    // Accessors
    Vector2f GetPosition() const { return m_Position; }
    bool IsAttached()    const { return m_Attached; }

    // Public helper if needed
    void UpdatePositionOnCircle(const Vector2f& playerPos);

private:
    // Weapon stats
    float SwingAngle;
    float SwingSpeed;
    float Damage;
    float Range;

    // State
    Vector2f m_Position;
    bool     m_Attached;
    float    m_AttachmentAngle;
};