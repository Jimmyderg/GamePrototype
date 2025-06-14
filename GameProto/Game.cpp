#include "pch.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "BaseMeleeWeapon.h"
#include "BaseRangedWeapon.h"
#include "WaveManager.h"
#include "UI.h"
#include "UpgradeUI.h"
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
		BaseRangedWeapon* rangedWeapon = new BaseRangedWeapon();
        weapon->SpawnRandom(vp.width, vp.height);
		weapon->AttachToPlayer(m_pPlayer->GetPosition(), m_MousePos);
		rangedWeapon->SpawnRandom(vp.width, vp.height);
		rangedWeapon->AttachToPlayer(m_pPlayer->GetPosition(), m_MousePos);
        m_pWeapons.push_back(weapon);
		m_pRangedWeapons.push_back(rangedWeapon);
    }

    // Initialize wave manager (3 enemies per wave multiplier, 30s interval)
    m_pWaveManager = new WaveManager(3, 10.0f, m_pPlayer);
    m_pWaveManager->Initialize(m_Enemies);

	// Initialize UI
	m_pUI = new UI(m_pPlayer, m_pWaveManager, "VCR_OSD_MONO_1.001.ttf", 24);
	if (!m_pUI) {
		throw std::runtime_error("Failed to initialize UI");
	}

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

	for (BaseRangedWeapon* w : m_pRangedWeapons) {
		delete w;
	}
	m_pRangedWeapons.clear();

    // Cleanup player
    delete m_pPlayer;
    m_pPlayer = nullptr;

	// Cleanup UI
	delete m_pUI;
	m_pUI = nullptr;

	// Cleanup upgrade UI
	delete m_pUpgradeUI;
	m_pUpgradeUI = nullptr;
	// Reset ranged mode
	m_RangedMode = false;
	// Reset mouse state
	m_IsMouseDown = false;
	m_MousePos = Vector2f{ 0.f, 0.f };
	// Reset reload percentage
	m_ReloadPct = 0.0f;
	// Reset paused state for upgrades
	m_PausedForUpgrade = false;

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

    if (!m_RangedMode)
    {
        // MELEE: only update & draw melee weapons
        for (BaseMeleeWeapon* w : m_pWeapons)
        {
            Vector2f pc = m_pPlayer->GetPosition();
            w->Update(elapsedSec, pc, m_MousePos);
            if (w->CheckPlayerCollision(pc, 5.f)) w->AttachToPlayer(pc, m_MousePos);
            for (auto it = m_Enemies.begin(); it != m_Enemies.end(); )
            {
                Enemy* e = *it;
                if (w->CheckEnemyCollision(e))
                {
                    w->DealDamage(e);
                    if (e->IsDead())
                    {
                        m_pPlayer->GainXP(XPGainFromEnemy); // Give XP for killing an enemy 
                        delete e;
                        it = m_Enemies.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
    }
    else
    {
        // RANGED: only update & draw ranged weapons
        for (BaseRangedWeapon* w : m_pRangedWeapons)
        {
            Vector2f pc = m_pPlayer->GetPosition();
            w->Fire(pc, m_MousePos, m_IsMouseDown);
            w->Update(elapsedSec, pc, m_MousePos);
            if (w->CheckPlayerCollision(pc, 5.f)) w->AttachToPlayer(pc, m_MousePos);
            for (auto it = m_Enemies.begin(); it != m_Enemies.end(); )
            {
                Enemy* e = *it;
                bool removed = false;
                for (const Projectile& proj : w->GetProjectiles())
                {
                    if (w->CheckEnemyCollision(proj, e))
                    {
                        w->DealDamage(e);
                        if (e->IsDead())
                        {
                            m_pPlayer->GainXP(XPGainFromEnemy); // Give XP for killing an enemy 
                            delete e;
                            it = m_Enemies.erase(it);
                            removed = true;
                            break;
                        }
                    }
                }
                if (!removed) ++it;
            }
        }
    }
    if (m_pRangedWeapons.empty())
    {
        m_ReloadPct = 0.0f;
    }
    else
    {
        // always use the first (equipped) ranged weapon
        auto* w = m_pRangedWeapons[0];
        m_ReloadPct = w->GetReloadProgress();
    }

    //Upgrade UI
    static int lastHandledLevel = 1;
    int currentLevel = m_pPlayer->GetLevel();
    if (!m_PausedForUpgrade
        && currentLevel != lastHandledLevel
        && currentLevel % 5 == 0)
    {
        lastHandledLevel = currentLevel;
        auto opts = m_pPlayer->GenerateUpgradeOptions();
        auto vp = GetViewPort();
        m_pUpgradeUI = new UpgradeUI(
            opts,
            "VCR_OSD_MONO_1.001.ttf",
            18,
            vp.width,
            vp.height
        );
        m_PausedForUpgrade = true;
        return;   // skip the rest of Update() while paused
    }

    // If we’re paused for upgrades, consume only the upgrade?UI input
    if (m_PausedForUpgrade)
        return;

}


void Game::Draw() const
{
    ClearBackground();

    if (!m_RangedMode) {
        for (auto w : m_pWeapons)
            w->Draw();
    }
    else {
        for (auto w : m_pRangedWeapons)
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

	
	m_pUI->Draw(m_ReloadPct);

    if (m_PausedForUpgrade) {
        m_pUpgradeUI->Draw();
    }
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e) 
{
	switch (e.keysym.sym) {
	case SDLK_1:
		// Switch to melee weapon mode
		m_RangedMode = false;
		std::cout << "Switched to Melee Weapon Mode" << std::endl;
		break;
	case SDLK_2:
		// Switch to ranged weapon mode
		m_RangedMode = true;
		std::cout << "Switched to Ranged Weapon Mode" << std::endl;
		break;
	}
}
void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e) {}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	// Update mouse position
	m_MousePos.x = static_cast<float>(e.x);
	m_MousePos.y = static_cast<float>(e.y);
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) 
{
	if (e.button == SDL_BUTTON_LEFT) {
		m_IsMouseDown = true;
		// Fire ranged weapon if attached
		for (BaseRangedWeapon* w : m_pRangedWeapons) {
			if (w->IsAttached()) {
				w->Fire(m_pPlayer->GetPosition(), m_MousePos, m_IsMouseDown);
			}
		}
	}
	else if (e.button == SDL_BUTTON_RIGHT) {
		// Handle right-click if needed
	}

    if (m_PausedForUpgrade && m_pUpgradeUI->HandleMouseClick((float)e.x, (float)e.y, *m_pPlayer)) {
        delete m_pUpgradeUI;
        m_pUpgradeUI = nullptr;
        m_PausedForUpgrade = false;
        std::cout << "Upgrade applied!\n";
        return;
    }
}
void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) 
{
	if (e.button == SDL_BUTTON_LEFT) {
		m_IsMouseDown = false;
		// Stop firing ranged weapon
		for (BaseRangedWeapon* w : m_pRangedWeapons) {
			if (w->IsAttached()) {
				w->Fire(m_pPlayer->GetPosition(), m_MousePos, m_IsMouseDown);
			}
		}
	}
	else if (e.button == SDL_BUTTON_RIGHT) {
		// Handle right-click release if needed
	}
}

void Game::ClearBackground() const
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
