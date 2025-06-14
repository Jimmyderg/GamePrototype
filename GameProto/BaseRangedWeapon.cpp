#include "pch.h"
#include "BaseRangedWeapon.h"
#include "Enemy.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GL/gl.h>

// Constructor
BaseRangedWeapon::BaseRangedWeapon()
    : m_Damage(15.0f)
    , m_Range(500.0f)
    , m_BulletSpeed(300.0f)
    , m_FireRate(2.0f)
    , m_TimeSinceLastShot(0.0f)
    , m_Position{0.0f, 0.0f}
    , m_Attached(true)
    , m_AttachmentAngle(0.0f)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

// Destructor
BaseRangedWeapon::~BaseRangedWeapon() = default;

// Copy constructor
BaseRangedWeapon::BaseRangedWeapon(const BaseRangedWeapon& other)
    : m_Damage(other.m_Damage)
    , m_Range(other.m_Range)
    , m_BulletSpeed(other.m_BulletSpeed)
    , m_FireRate(other.m_FireRate)
    , m_TimeSinceLastShot(other.m_TimeSinceLastShot)
    , m_Position(other.m_Position)
    , m_Attached(other.m_Attached)
    , m_AttachmentAngle(other.m_AttachmentAngle)
    , m_Projectiles(other.m_Projectiles)
{}

// Copy assignment
BaseRangedWeapon& BaseRangedWeapon::operator=(const BaseRangedWeapon& rhs)
{
    if (this != &rhs) {
        m_Damage = rhs.m_Damage;
        m_Range = rhs.m_Range;
        m_BulletSpeed = rhs.m_BulletSpeed;
        m_FireRate = rhs.m_FireRate;
        m_TimeSinceLastShot = rhs.m_TimeSinceLastShot;
        m_Position = rhs.m_Position;
        m_Attached = rhs.m_Attached;
        m_AttachmentAngle = rhs.m_AttachmentAngle;
        m_Projectiles = rhs.m_Projectiles;
    }
    return *this;
}

// Move constructor
BaseRangedWeapon::BaseRangedWeapon(BaseRangedWeapon&& other) noexcept
    : m_Damage(other.m_Damage)
    , m_Range(other.m_Range)
    , m_BulletSpeed(other.m_BulletSpeed)
    , m_FireRate(other.m_FireRate)
    , m_TimeSinceLastShot(other.m_TimeSinceLastShot)
    , m_Position(other.m_Position)
    , m_Attached(other.m_Attached)
    , m_AttachmentAngle(other.m_AttachmentAngle)
    , m_Projectiles(std::move(other.m_Projectiles))
{
    other.m_Attached = false;
}

// Move assignment
BaseRangedWeapon& BaseRangedWeapon::operator=(BaseRangedWeapon&& rhs) noexcept
{
    if (this != &rhs) {
        m_Damage = rhs.m_Damage;
        m_Range = rhs.m_Range;
        m_BulletSpeed = rhs.m_BulletSpeed;
        m_FireRate = rhs.m_FireRate;
        m_TimeSinceLastShot = rhs.m_TimeSinceLastShot;
        m_Position = rhs.m_Position;
        m_Attached = rhs.m_Attached;
        m_AttachmentAngle = rhs.m_AttachmentAngle;
        m_Projectiles = std::move(rhs.m_Projectiles);

        rhs.m_Attached = false;
    }
    return *this;
}

// Spawn randomly on the map
void BaseRangedWeapon::SpawnRandom(float screenWidth, float screenHeight)
{
    m_Position.x = static_cast<float>(std::rand() % static_cast<int>(screenWidth));
    m_Position.y = static_cast<float>(std::rand() % static_cast<int>(screenHeight));
    m_Attached = false;
}

// Check if player can pick up
bool BaseRangedWeapon::CheckPlayerCollision(const Vector2f& playerPos, float playerRadius) const
{
    if (m_Attached) return false;
    float dx = m_Position.x - playerPos.x;
    float dy = m_Position.y - playerPos.y;
    float dist2 = dx * dx + dy * dy;
    return dist2 <= (playerRadius * playerRadius);
}

// Attach weapon to player
void BaseRangedWeapon::AttachToPlayer(const Vector2f& playerPos, const Vector2f& mousePos)
{
    m_Attached = true;
    Vector2f dir = mousePos - playerPos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist > 0.0f) {
        m_AttachmentAngle = std::atan2(dir.y, dir.x);
    }
    m_Position = playerPos;
	UpdatePositionOnCircle(playerPos);
}
// Update position on circle around player
void BaseRangedWeapon::UpdatePositionOnCircle(const Vector2f& playerPos)
{
    const float radius = 40.0f; // Fixed radius for weapon circle
    m_Position.x = playerPos.x + radius * std::cos(m_AttachmentAngle);
    m_Position.y = playerPos.y + radius * std::sin(m_AttachmentAngle);
}

// Fire a projectile only on mouse click
void BaseRangedWeapon::Fire(const Vector2f& playerPos, const Vector2f& mousePos, bool isMouseDown)
{
    if (!m_Attached || !isMouseDown) return;
    if (m_TimeSinceLastShot < 1.0f / m_FireRate) return;

    Vector2f dir = mousePos - playerPos;
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0) return;
    dir.x /= length;
    dir.y /= length;

    Projectile proj;
    proj.position = playerPos;
    proj.spawnPosition = playerPos;
    proj.direction = dir;
    proj.active = true;

    m_Projectiles.push_back(proj);
    m_TimeSinceLastShot = 0.0f;
}

// Update weapon and projectiles
void BaseRangedWeapon::Update(float elapsedSec, const Vector2f& playerPos, const Vector2f& mousePos)
{
    if (m_Attached) {
        m_TimeSinceLastShot += elapsedSec;
        m_Position = playerPos;
		UpdatePositionOnCircle(playerPos);
    }
    if (!m_Attached) return;

    // Recalculate angle toward mouse
    Vector2f dir = mousePos - playerPos;
    float dist = std::sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (dist > 5.0f) {
        dir.x /= dist;
        dir.y /= dist;
        m_AttachmentAngle = std::atan2f(dir.y, dir.x);
    }
    // Update projectiles
    for (auto& proj : m_Projectiles) {
        if (!proj.active) continue;
        proj.position.x += proj.direction.x * m_BulletSpeed * elapsedSec;
        proj.position.y += proj.direction.y * m_BulletSpeed * elapsedSec;

        // Deactivate if out of range
        float dx = proj.position.x - proj.spawnPosition.x;
        float dy = proj.position.y - proj.spawnPosition.y;
        if (dx*dx + dy*dy > m_Range * m_Range) {
            proj.active = false;
        }

    }
}

// Draw weapon and projectiles
void BaseRangedWeapon::Draw() const
{
    // Draw the weapon as a rectangle at m_Position
    glPushMatrix();
    glTranslatef(m_Position.x, m_Position.y, 0.0f);
    glRotatef(m_AttachmentAngle * 180.0f / 3.14159f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -5.0f);
    glVertex2f(10.0f, -5.0f);
    glVertex2f(10.0f, 5.0f);
    glVertex2f(-10.0f, 5.0f);
    glEnd();
    glPopMatrix();

    // Draw projectiles
    for (const auto& proj : m_Projectiles) {
        if (!proj.active) continue;
        glPushMatrix();
        glTranslatef(proj.position.x, proj.position.y, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-2.0f, -2.0f);
        glVertex2f(2.0f, -2.0f);
        glVertex2f(2.0f, 2.0f);
        glVertex2f(-2.0f, 2.0f);
        glEnd();
        glPopMatrix();
    }
}

// Check collision between a projectile and an enemy
bool BaseRangedWeapon::CheckEnemyCollision(const Projectile& proj, const Enemy* pEnemy) const
{
    if (!proj.active || !pEnemy) return false;
    float halfSize = 2.0f;
    Rectf w{ proj.position.x - halfSize, proj.position.y - halfSize, halfSize*2, halfSize*2 };
    Rectf e = pEnemy->GetEnemyRect();
    return !(e.left > w.left + w.width || e.left + e.width < w.left ||
             e.bottom > w.bottom + w.height || e.bottom + e.height < w.bottom);
}

// Deal damage to an enemy
void BaseRangedWeapon::DealDamage(Enemy* pEnemy)
{
    if (pEnemy) {
        pEnemy->TakeDamage(m_Damage);
    }
}
// Get reload progress (for UI)
float BaseRangedWeapon::GetReloadProgress() const
{
    // fireRate is shots/sec ⇒ interval in seconds per shot
    float interval = 1.0f / m_FireRate;
    // m_TimeSinceLastShot counts up from zero
    float pct = m_TimeSinceLastShot / interval;
    return (pct > 1.0f ? 1.0f : pct);
}