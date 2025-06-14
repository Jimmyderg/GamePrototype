#pragma once

#include <vector>

class Enemy;
class Player;

class WaveManager
{
public:
	WaveManager(int basePerWave, float intercalSec, Player* player);
	~WaveManager();
	WaveManager(const WaveManager& other);
	WaveManager& operator=(const WaveManager& other);
	WaveManager(WaveManager&& other) noexcept;
	WaveManager& operator=(WaveManager&& other) noexcept;

	void Initialize(std::vector<Enemy*>& outNew);
	void Update(float deltaTime, std::vector<Enemy*>& outNew);
	float GetCurrentWave() const { return static_cast<float>(m_WaveCount); }

private:
	int m_WaveCount{ 0 };
	float m_Timer{ 0.0f };
	int m_BasePerWave{ 0 };
	float m_Interval{ 0.0f };

	void SpawnNextWave(std::vector<Enemy*>& out);
	static float RandomRange(float min, float max);

	Player* m_pPlayer;
};

