#include "pch.h"
#include "BaseMeleeWeapon.h"
#include "Enemy.h"
#include "structs.h"   // for Rectf
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <GL/gl.h>

// Constructor
BaseMeleeWeapon::BaseMeleeWeapon()
    : SwingAngle(0.0f)
    , SwingSpeed(5.0f)
    , Damage(20.0f)
    , Range(50.0f)
    , m_Position{ 0.0f, 0.0f }
    , m_Attached(false)
    , m_AttachmentAngle(0.0f)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

// Destructor
BaseMeleeWeapon::~BaseMeleeWeapon() = default;

// Copy constructor
BaseMeleeWeapon::BaseMeleeWeapon(const BaseMeleeWeapon& other)
    : SwingAngle(other.SwingAngle)
    , SwingSpeed(other.SwingSpeed)
    , Damage(other.Damage)
    , Range(other.Range)
    , m_Position(other.m_Position)
    , m_Attached(other.m_Attached)
    , m_AttachmentAngle(other.m_AttachmentAngle)
{
}

// Copy assignment
BaseMeleeWeapon& BaseMeleeWeapon::operator=(const BaseMeleeWeapon& rhs)
{
    if (this != &rhs) {
        SwingAngle = rhs.SwingAngle;
        SwingSpeed = rhs.SwingSpeed;
        Damage = rhs.Damage;
        Range = rhs.Range;
        m_Position = rhs.m_Position;
        m_Attached = rhs.m_Attached;
        m_AttachmentAngle = rhs.m_AttachmentAngle;
    }
    return *this;
}

// Move constructor
BaseMeleeWeapon::BaseMeleeWeapon(BaseMeleeWeapon&& other) noexcept
    : SwingAngle(other.SwingAngle)
    , SwingSpeed(other.SwingSpeed)
    , Damage(other.Damage)
    , Range(other.Range)
    , m_Position(other.m_Position)
    , m_Attached(other.m_Attached)
    , m_AttachmentAngle(other.m_AttachmentAngle)
{
    // Invalidate source
    other.SwingAngle = 0.0f;
    other.SwingSpeed = 0.0f;
    other.Damage = 0.0f;
    other.Range = 0.0f;
    other.m_Position = Vector2f{ 0.0f, 0.0f };
    other.m_Attached = false;
    other.m_AttachmentAngle = 0.0f;
}

// Move assignment
BaseMeleeWeapon& BaseMeleeWeapon::operator=(BaseMeleeWeapon&& rhs) noexcept
{
    if (this != &rhs) {
        SwingAngle = rhs.SwingAngle;
        SwingSpeed = rhs.SwingSpeed;
        Damage = rhs.Damage;
        Range = rhs.Range;
        m_Position = rhs.m_Position;
        m_Attached = rhs.m_Attached;
        m_AttachmentAngle = rhs.m_AttachmentAngle;

        // Invalidate source
        rhs.SwingAngle = 0.0f;
        rhs.SwingSpeed = 0.0f;
        rhs.Damage = 0.0f;
        rhs.Range = 0.0f;
        rhs.m_Position = Vector2f{ 0.0f, 0.0f };
        rhs.m_Attached = false;
        rhs.m_AttachmentAngle = 0.0f;
    }
    return *this;
}

// Random spawn
void BaseMeleeWeapon::SpawnRandom(float screenWidth, float screenHeight)
{
    m_Position.x = static_cast<float>(std::rand() % static_cast<int>(screenWidth));
    m_Position.y = static_cast<float>(std::rand() % static_cast<int>(screenHeight));
    m_Attached = false;
}

// Check for pickup
bool BaseMeleeWeapon::CheckPlayerCollision(const Vector2f& playerPos, float playerRadius) const
{
    if (m_Attached) return false;
    float dx = m_Position.x - playerPos.x;
    float dy = m_Position.y - playerPos.y;
    float dist2 = dx * dx + dy * dy;
    return dist2 <= (Range + playerRadius) * (Range + playerRadius);
}

// Attach to player
void BaseMeleeWeapon::AttachToPlayer(const Vector2f& playerPos, const Vector2f& mousePos)
{
    m_Attached = true;
    Vector2f dir = mousePos - playerPos;
    m_AttachmentAngle = std::atan2f(dir.y, dir.x);
    UpdatePositionOnCircle(playerPos);
}

// Update position on circle
void BaseMeleeWeapon::UpdatePositionOnCircle(const Vector2f& playerPos)
{
    const float radius = 80.0f;
    m_Position.x = playerPos.x + radius * std::cosf(m_AttachmentAngle);
    m_Position.y = playerPos.y + radius * std::sinf(m_AttachmentAngle);
}

// Draw weapon
void BaseMeleeWeapon::Draw() const
{
    // Always draw the weapon at its position, attached or not
    glPushMatrix();
    glTranslatef(m_Position.x, m_Position.y, 0.0f);

    // If attached, apply swing rotation
    if (m_Attached)
        glRotatef(0, 0.0f, 0.0f, 1.0f);

    // Draw the 10×30 rectangle centered at (0,0)
    glBegin(GL_QUADS);
    glVertex2f(-5.0f, -15.0f);
    glVertex2f(5.0f, -15.0f);
    glVertex2f(5.0f, 15.0f);
    glVertex2f(-5.0f, 15.0f);
    glEnd();
    glPopMatrix();
}

// Per-frame update
void BaseMeleeWeapon::Update(float elapsedSec, const Vector2f& playerPos, const Vector2f& mousePos)
{
    if (!m_Attached) return;
    // Swing animation
    SwingAngle += SwingSpeed * elapsedSec;
    if (std::fabsf(SwingAngle) > 60.0f) SwingSpeed = -SwingSpeed;
    // Recalculate angle toward mouse
    Vector2f dir = mousePos - playerPos;
    float dist = std::sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (dist > 5.0f) {
        dir.x /= dist;
        dir.y /= dist;
        m_AttachmentAngle = std::atan2f(dir.y, dir.x);
    }
    UpdatePositionOnCircle(playerPos);
}

// Check collision with enemy
bool BaseMeleeWeapon::CheckEnemyCollision(const Enemy* pEnemy) const
{
    if (!m_Attached || !pEnemy) return false;
    Rectf w{ m_Position.x - 5.0f, m_Position.y - 15.0f, 10.0f, 30.0f };
    Rectf e = pEnemy->GetEnemyRect();
    return !(e.left > w.left + w.width || e.left + e.width < w.left ||
        e.bottom > w.bottom + w.height || e.bottom + e.height < w.bottom);
}

// Deal damage
void BaseMeleeWeapon::DealDamage(Enemy* pEnemy) const
{
    if (m_Attached && pEnemy) pEnemy->TakeDamage(Damage);
}
