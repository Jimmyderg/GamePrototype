#include "pch.h"
#include "Player.h"
#include <SDL.h>
#include <SDL_image.h>
#include "structs.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>

Player::Player(float posX, float posY, float health)
    : m_Position{ posX, posY }
    , m_WalkingState{ WalkingState::none }
    , m_Health{ health }
{
    m_pSpriteSheet = new Texture{ "knight.png" };
}

Player::~Player()
{
    delete m_pSpriteSheet;
    m_pSpriteSheet = nullptr;
}

void Player::Draw() 
{
    // Only draw if not flashing OR if flash state is visible
    if (!m_Invincible || (m_IsFlashing && m_Invincible)) {
        Rectf srcRect{ GetCurrFrameRect() };
        Rectf destRect{ m_Position.x, m_Position.y, 32, 32 };
        m_pSpriteSheet->Draw(destRect, srcRect);
    }

    // Always draw health bar
}

void Player::Update(float elapsedSec) 
{
    // Update I-frames timer
    if (m_Invincible) {
        m_InvincibilityTimer -= elapsedSec;
        if (m_InvincibilityTimer <= 0.0f) {
            m_Invincible = false;
        }

        // Handle flashing effect
        m_FlashTimer -= elapsedSec;
        if (m_FlashTimer <= 0.0f) {
            m_IsFlashing = !m_IsFlashing; // Toggle flash state
            m_FlashTimer = m_FlashInterval;
        }
    }
}
void Player::DrawHealthBar() const
{
    // Position (top-right corner)
    float barX = 600.0f; // Adjust based on screen width
    float barY = 20.0f;
    float barWidth = 100.0f;
    float barHeight = 10.0f;

    // Background (red for missing health)
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();

    // Foreground (green for current health)
    float healthPercent = m_Health / m_MaxHealth;
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth * healthPercent, barY);
    glVertex2f(barX + barWidth * healthPercent, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
}

void Player::DrawWeaponCircle() const
{
	// Draw a circle around the player to represent the weapon range
    const float innerRadius = 40.f;
	const float outerRadius = 80.f;
	const int numSegments = 32;

	// Center the circles on player position
	float centerX = m_Position.x + 16.f; // Center of the player sprite
	float centerY = m_Position.y + 16.f; // Center of the player sprite

	//Draw outer circle
	glColor4f(1.f, 0.f, 0.f, 0.4f); // Red with 40% opacity
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numSegments; ++i) {
		float theta = 2.0f * M_PI * float(i) / float(numSegments);
		float x = outerRadius * cosf(theta);
		float y = outerRadius * sinf(theta);
		glVertex2f(centerX + x, centerY + y);
	}
	glEnd();
	// Draw inner circle
	glColor4f(0.f, 0.f, 1.f, 0.4f); // Red with 40% opacity
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numSegments; ++i) {
		float theta = 2.0f * M_PI * float(i) / float(numSegments);
		float x = innerRadius * cosf(theta);
		float y = innerRadius * sinf(theta);
		glVertex2f(centerX + x, centerY + y);
	}
	glEnd();
}

Vector2f Player::GetPosition() const
{
    return Vector2f{m_Position.x + 16.f, m_Position.y + 16.f};
}

Rectf Player::GetPlayerRect() const
{
    return Rectf{ m_Position.x, m_Position.y, 32.0f, 32.0f };
}

void Player::TakeDamage(int amount) 
{
    if (m_Invincible) return; // Ignore damage during I-frames

    m_Health -= amount;
    if (m_Health <= 0) m_Health = 0;

    // Activate I-frames
    m_Invincible = true;
    m_InvincibilityTimer = m_InvincibilityDuration;
    m_IsFlashing = true;
    m_FlashTimer = m_FlashInterval;
}

float Player::GetHealth() const
{
    return m_Health;
}

void Player::HandleMovement(float elapsedSec, const Uint8* pStates) {
    Vector2f velocity{};

    if (pStates[SDL_SCANCODE_W]) {
        velocity.y += 1.0f;
        m_WalkingState = WalkingState::up;
    }
    if (pStates[SDL_SCANCODE_S]) {
        velocity.y -= 1.0f;
        m_WalkingState = WalkingState::down;
    }
    if (pStates[SDL_SCANCODE_A]) {
        velocity.x -= 1.0f;
        m_WalkingState = WalkingState::left;
    }
    if (pStates[SDL_SCANCODE_D]) {
        velocity.x += 1.0f;
        m_WalkingState = WalkingState::right;
    }

    // Normalize diagonal movement
    if (velocity.Length() > 0.0f) {
        velocity.Normalized();
        m_Position += velocity * m_MoveSpeed * elapsedSec;
    }
    else {
        m_WalkingState = WalkingState::none;
    }
}
void Player::TryPickUpWeapon(const Vector2f& mousePos)
{
	for (auto& weapon : m_MeleeWeapons) {
		if (weapon.CheckPlayerCollision(mousePos, m_PickupRadius)) {
			// Pick up the weapon
			weapon.AttachToPlayer(m_Position, mousePos); // Spawn at random position
		}
	}
}
void Player::UpdateWeapons(float elapsedSec, const Vector2f& mousePos) {
	for (auto& weapon : m_MeleeWeapons) {
		if (weapon.IsAttached()) {
			weapon.UpdatePositionOnCircle(m_Position);
		}
		else {
			weapon.Update(elapsedSec, m_Position, mousePos);
			if (weapon.CheckPlayerCollision(m_Position, m_PickupRadius)) {
				// Pick up the weapon
				weapon.AttachToPlayer(m_Position, mousePos); // Spawn at random position
			}
		}
	}
}
void Player::GainXP(int xp)
{
    m_XP += xp;
    std::cout << "Gained " << xp << " XP! (" << m_XP << "/" << m_XPToNextLevel << ")\n";
    CheckLevelUp();
}

void Player::CheckLevelUp()
{
    while (m_XP >= m_XPToNextLevel) {
        m_XP -= m_XPToNextLevel;
        LevelUp();
        m_XPToNextLevel = static_cast<int>(std::ceil(m_XPToNextLevel * 1.5f));
    }
}

void Player::LevelUp()
{
    ++m_Level;
    std::cout << "Level " << m_Level << " reached!\n";
    if (m_Level % 5 == 0) {
        ChooseUpgrade();
    }
}

void Player::ChooseUpgrade()
{
    enum Upgrade { M_DAMAGE, R_DAMAGE, M_RANGE, R_Range, F_RATE, B_PIERCE, MAX_HEALTH, MOVE_SPEED };
    std::vector<Upgrade> all = { M_DAMAGE, R_DAMAGE, M_RANGE, R_Range, F_RATE, B_PIERCE, MAX_HEALTH, MOVE_SPEED };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(all.begin(), all.end(), gen);
    auto opts = std::vector<Upgrade>(all.begin(), all.begin() + 3);

    std::cout << "Choose an upgrade:\n";
    for (int i = 0; i < opts.size(); ++i) {
        std::cout << (i + 1) << ": ";
        switch (opts[i]) {
        case M_DAMAGE:    std::cout << "+5% melee damage";    break;
        case R_DAMAGE:    std::cout << "+5% ranged damage";   break;
        case M_RANGE:     std::cout << "+10% melee range";     break;
		case R_Range:     std::cout << "+10% ranged range";    break;
        case F_RATE:      std::cout << "+10% fire rate";      break;
        case B_PIERCE:    std::cout << "+1 bullet pierce";    break;
        case MAX_HEALTH:  std::cout << "+20 max health";      break;
        case MOVE_SPEED:  std::cout << "+10% move speed";     break;
        }
        std::cout << "\n";
    }
    int choice = 1; // TODO: replace with real player input
    Upgrade sel = opts[choice - 1];

    switch (sel) {
    case M_DAMAGE:    m_MeleeDamage *= 1.05f; break;
    case R_DAMAGE:    m_RangedDamage *= 1.05f; break;
    case M_RANGE:     m_MeleeRange *= 1.10f; break;
	case R_Range:     m_RangedRange *= 1.10f; break;
    case F_RATE:      m_FireRate *= 1.10f; break;
    case B_PIERCE:    ++m_BulletPierce;      break;
    case MAX_HEALTH:  m_MaxHealth += 20.f; break;
    case MOVE_SPEED:  m_MoveSpeed *= 1.10f; break;
    }
    std::cout << "Upgrade applied!\n";
}

std::array<Player::Upgrade, 3> Player::GenerateUpgradeOptions() const
{
    std::vector<Upgrade> all = {
      Upgrade::M_DAMAGE, Upgrade::R_DAMAGE, Upgrade::M_RANGE,
	  Upgrade::R_Range, Upgrade::F_RATE,   Upgrade::B_PIERCE, 
      Upgrade::MAX_HEALTH, Upgrade::MOVE_SPEED
    };
    std::shuffle(all.begin(), all.end(),
        std::mt19937{ std::random_device{}() });
    return { all[0], all[1], all[2] };
}

void Player::ApplyUpgrade(Upgrade up)
{
    switch (up)
    {
    case Upgrade::M_DAMAGE:   m_MeleeDamage *= 1.05f; break;
    case Upgrade::R_DAMAGE:   m_RangedDamage *= 1.05f; break;
    case Upgrade::M_RANGE:    m_MeleeRange *= 1.10f; break;
	case Upgrade::R_Range:    m_RangedRange *= 1.10f; break;
    case Upgrade::F_RATE:     m_FireRate *= 1.10f; break;
    case Upgrade::B_PIERCE:   ++m_BulletPierce;          break;
    case Upgrade::MAX_HEALTH: m_MaxHealth += 20.f;     break;
    case Upgrade::MOVE_SPEED: m_MoveSpeed *= 1.10f;     break;
    }
}