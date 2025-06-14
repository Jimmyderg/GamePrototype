#pragma once
#include "BaseGame.h"
#include "Vector2f.h"
#include "Texture.h"
#include <vector>
#include <memory>

class Player;
class Enemy; // Forward declaration to avoid circular includes
class BaseMeleeWeapon; // Forward declaration for weapon
class BaseRangedWeapon; // Forward declaration for ranged weapon
class WaveManager;
class UI;
class UpgradeUI; // Forward declaration for upgrade UI

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	// DATA MEMBERS

	bool m_IsMouseDown{ false }; // Mouse button state
	float m_ReloadPct{};
	float XPGainFromEnemy{ 200.f }; // XP gain from killing an enemy
	bool m_RangedMode{ false }; // Whether ranged mode is active

	bool m_PausedForUpgrade{ false };
	UpgradeUI* m_pUpgradeUI{ nullptr };

	Vector2f m_MousePos{ 0.f, 0.f }; // Mouse position

	Player* m_pPlayer = nullptr;

	std::vector<Enemy*> m_Enemies;	
	std::vector<BaseMeleeWeapon*> m_pWeapons; // List of weapons
	std::vector<BaseRangedWeapon*> m_pRangedWeapons; // List of ranged weapons

	UI* m_pUI;
	WaveManager* m_pWaveManager;


};