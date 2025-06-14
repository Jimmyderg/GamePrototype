#include "pch.h"
#include "Enemy.h"
#include "Player.h"
#include "utils.h"

Enemy::Enemy(float posX, float posY, Player* player, float health)
    : m_Position{ posX, posY }, 
    m_pPlayer(player),
	m_Health{ health }
{}

void Enemy::Draw()
{
    // Set color (Red)
    utils::SetColor(Color4f{ 1.f,0.f,0.f,1.f }); // RGB (1.0 = 255)

    // Draw a filled quad (immediate mode)
	utils::FillRect(m_Position.x, m_Position.y, m_Width, m_Height);
}

void Enemy::Update(float elapsedSec)
{
    if (!m_pPlayer) return;

    Vector2f playerPos = m_pPlayer->GetPosition();

    // Calculate direction vector toward player
    Vector2f direction = (playerPos - m_Position) - Vector2f{ 16.f,16.f };
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    // Normalize direction and move
    if (distance > 0)
    {
        direction.x /= distance;
        direction.y /= distance;
        m_Position.x += direction.x * m_Speed * elapsedSec;
        m_Position.y += direction.y * m_Speed * elapsedSec;
    }

    // Check collision and deal damage
    if (!m_pPlayer->IsInvincible() && CheckCollision(m_pPlayer->GetPlayerRect()))
    {
        DealDamage();
    }
}

void Enemy::DealDamage()
{
    if (m_pPlayer)
    {
        m_pPlayer->TakeDamage(m_Damage); // Player needs this method
    }
}

bool Enemy::CheckCollision(const Rectf& otherRect) const
{
    Rectf enemyRect{ m_Position.x, m_Position.y, m_Width, m_Height };
    return (enemyRect.left < otherRect.left + otherRect.width &&
        enemyRect.left + enemyRect.width > otherRect.left &&
        enemyRect.bottom < otherRect.bottom + otherRect.height &&
        enemyRect.bottom + enemyRect.height > otherRect.bottom);
}

Vector2f Enemy::GetPosition() const { return m_Position; }

Rectf Enemy::GetEnemyRect() const
{
    return Rectf{ m_Position.x, m_Position.y, m_Width, m_Height };
}
// Check if enemy collides with player

void Enemy::TakeDamage(float amount)
{
	m_Health -= amount;
    if (m_Health <= 0)
    {
        m_Health = 0; // Ensure health doesn't go negative
        IsDead();
    }
}

void Enemy::DrawHealthBar() const
{
	// Draw health bar above enemy
	float healthBarWidth = m_Width / 3;
	float healthBarHeight = 5.0f; // Height of the health bar
	float healthBarX = m_Position.x;
	float healthBarY = m_Position.y + m_Height + 5.0f; // Position above enemy

	// Calculate health percentage
	float healthPercentage = m_Health / 25.0f; // Assuming max health is 25

	// Set color based on health percentage
	if (healthPercentage > 0.5f)
		glColor3f(0.0f, 1.0f, 0.0f); // Green
	else if (healthPercentage > 0.2f)
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow
	else
		glColor3f(1.0f, 0.0f, 0.0f); // Red

	// Draw the health bar
	utils::FillRect(healthBarX, healthBarY, healthBarWidth * healthPercentage, healthBarHeight);
}
void Enemy::RemoveEnemy()
{

}
bool Enemy::IsDead() {
	return m_Health <= 0;
}