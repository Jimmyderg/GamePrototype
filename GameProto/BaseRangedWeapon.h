#pragma once

#include "Vector2f.h"
#include <vector>

class Enemy;

struct Projectile
{
    Vector2f position;
    Vector2f direction;
    Vector2f spawnPosition;
    bool active;
};

class BaseRangedWeapon
{
public:
    // Rule of Five
    BaseRangedWeapon();
    virtual ~BaseRangedWeapon();
    BaseRangedWeapon(const BaseRangedWeapon& other);
    BaseRangedWeapon& operator=(const BaseRangedWeapon& other);
    BaseRangedWeapon(BaseRangedWeapon&& other) noexcept;
    BaseRangedWeapon& operator=(BaseRangedWeapon&& other) noexcept;

    // Spawning and pickup
    void SpawnRandom(float screenWidth, float screenHeight);
    bool CheckPlayerCollision(const Vector2f& playerPos, float playerRadius) const;
    void AttachToPlayer(const Vector2f& playerPos, const Vector2f& mousePos);

    // Per-frame logic
    void Update(float elapsedSec, const Vector2f& playerPos, const Vector2f& mousePos);
    void Draw() const;

    // Combat: now requires explicit click input to fire
    void Fire(const Vector2f& playerPos, const Vector2f& mousePos, bool isMouseDown);
    bool CheckEnemyCollision(const Projectile& proj, const Enemy* pEnemy) const;
    void DealDamage(Enemy* pEnemy);

    // Accessors
    Vector2f GetPosition() const { return m_Position; }
    bool IsAttached() const { return m_Attached; }
    const std::vector<Projectile>& GetProjectiles() const { return m_Projectiles; }

	// Public helper if needed
	void UpdatePositionOnCircle(const Vector2f& playerPos);
	float GetFireRate() const { return m_FireRate; }
    float GetReloadProgress() const;

private:
    // Weapon stats
    float m_Damage;
    float m_Range;
    float m_BulletSpeed;
    float m_FireRate; // shots per second
    float m_TimeSinceLastShot;

    // State
    Vector2f m_Position;
    bool m_Attached{ true };
    float m_AttachmentAngle;

    std::vector<Projectile> m_Projectiles;
};
