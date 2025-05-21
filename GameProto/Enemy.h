#pragma once
#include "structs.h"
#include "Vector2f.h"
#include <SDL.h> // Only needed for SDL types (Rectf)

class Player; // Forward declaration

class Enemy
{
public:
    Enemy(float posX, float posY, Player* player, float health);
    ~Enemy() = default;

    void Draw(); // Now uses OpenGL
    void Update(float elapsedSec);
    void DealDamage();
    Vector2f GetPosition() const;
    Rectf GetEnemyRect() const;
    bool CheckCollision(const Rectf& otherRect) const;
    bool IsDead(); // Check if dead
	float GetHealth() const { return m_Health; } // Get health

	void DrawHealthBar() const; // Draws health bar

    void TakeDamage(float amount);

    void RemoveEnemy(); // Remove enemy from game

private:
    Player* m_pPlayer;
    Vector2f m_Position;
    const float m_Width{ 32.0f };
    const float m_Height{ 32.0f };
    const float m_Speed{ 50.0f };
    const int m_Damage{ 10 };
	float m_Health{ 25.0f };
};