#include "pch.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "BaseMeleeWeapon.h"
#include "WaveManager.h"
#include <SDL.h>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game(const Window& window)
    : BaseGame{ window }
    , m_pWaveManager{ nullptr }
{
    Initialize();
}

Game::~Game()
{
    Cleanup();
}

void Game::Initialize()
{
    // Seed random generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Center start position
    const auto& vp = GetViewPort();
    const float startX = vp.width / 2.0f;
    const float startY = vp.height / 2.0f;

    // Create player
    m_pPlayer = new Player{ startX, startY, 100.0f };

    // Spawn initial weapons
    for (int i = 0; i < 1; ++i) {
        BaseMeleeWeapon* weapon = new BaseMeleeWeapon();
        weapon->SpawnRandom(vp.width, vp.height);
        m_pWeapons.push_back(weapon);
    }

    // Initialize wave manager (3 enemies per wave multiplier, 30s interval)
    m_pWaveManager = new WaveManager(3, 10.0f, m_pPlayer);
    m_pWaveManager->Initialize(m_Enemies);
}

void Game::Cleanup()
{
    // Delete wave manager
    delete m_pWaveManager;
    m_pWaveManager = nullptr;

    // Cleanup enemies
    for (Enemy* e : m_Enemies) {
        delete e;
    }
    m_Enemies.clear();

    // Cleanup weapons
    for (BaseMeleeWeapon* w : m_pWeapons) {
        delete w;
    }
    m_pWeapons.clear();

    // Cleanup player
    delete m_pPlayer;
    m_pPlayer = nullptr;
}

void Game::Update(float elapsedSec)
{
    // Game over check
    if (m_pPlayer->GetHealth() <= 0) {
        std::cout << "GAME OVER!" << std::endl;
        return;
    }

    // Update player
    m_pPlayer->Update(elapsedSec);
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    m_pPlayer->HandleMovement(elapsedSec, keyState);

    // Wave spawning
    m_pWaveManager->Update(elapsedSec, m_Enemies);

    // Update and remove dead enemies
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        Enemy* e = *it;
        e->Update(elapsedSec);
        if (e->IsDead()) {
            delete e;
            it = m_Enemies.erase(it);
        }
        else {
            ++it;
        }
    }

    // Update weapons and handle collisions
    for (BaseMeleeWeapon* w : m_pWeapons) {
		Vector2f PlayerCenter = m_pPlayer->GetPosition();
        w->Update(elapsedSec, PlayerCenter, m_MousePos);

        if (w->CheckPlayerCollision(m_pPlayer->GetPosition(), 5.0f)) {
            w->AttachToPlayer(PlayerCenter, m_MousePos);
        }

        for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
            Enemy* e = *it;
            if (w->CheckEnemyCollision(e)) {
                w->DealDamage(e);
                std::cout << "Enemy hit!" << std::endl;
                if (e->IsDead()) {
                    std::cout << "Enemy is dead!" << std::endl;
                    delete e;
                    it = m_Enemies.erase(it);
                    continue;
                }
            }
            ++it;
        }

        // Debug info
        std::cout << "Player Position: ("
            << m_pPlayer->GetPosition().x << ", "
            << m_pPlayer->GetPosition().y << ")" << std::endl;
        std::cout << "Weapon Position: ("
            << w->GetPosition().x << ", "
            << w->GetPosition().y << ")" << std::endl;
    }
}

void Game::Draw() const
{
    ClearBackground();

    // Draw weapons
    for (const BaseMeleeWeapon* w : m_pWeapons) {
        w->Draw();
    }

    // Draw player
    m_pPlayer->Draw();

    // Draw enemies
    for (Enemy* e : m_Enemies) {
        if (e) {
            e->Draw();
            e->DrawHealthBar();
        }
    }

    // Draw player UI
    m_pPlayer->DrawWeaponCircle();
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e) {}
void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e) {}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
    m_MousePos.x = static_cast<float>(e.x);
    m_MousePos.y = static_cast<float>(e.y);
    std::cout << "Mouse Position: (" << m_MousePos.x << ", " << m_MousePos.y << ")" << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) {}
void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) {}

void Game::ClearBackground() const
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
