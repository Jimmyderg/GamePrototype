#include "pch.h"
#include "WaveManager.h"
#include "Enemy.h"
#include "Player.h"
#include <cstdlib>
#include <ctime>

//constructor
WaveManager::WaveManager(int BasePerWave, float intercalSec, Player* player)
	: m_WaveCount(0)
	, m_Timer(0.0f)
	, m_BasePerWave(BasePerWave)
	, m_Interval(intercalSec)
	, m_pPlayer(player)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random number generator
}
WaveManager::~WaveManager()
{
	// Destructor
}
WaveManager::WaveManager(const WaveManager& other)
	: m_WaveCount(other.m_WaveCount)
	, m_Timer(other.m_Timer)
	, m_BasePerWave(other.m_BasePerWave)
	, m_Interval(other.m_Interval)
{
}
WaveManager& WaveManager::operator=(const WaveManager& other)
{
	if (this != &other)
	{
		m_WaveCount = other.m_WaveCount;
		m_Timer = other.m_Timer;
		m_BasePerWave = other.m_BasePerWave;
		m_Interval = other.m_Interval;
	}
	return *this;
}
WaveManager::WaveManager(WaveManager&& other) noexcept
	: m_WaveCount(other.m_WaveCount)
	, m_Timer(other.m_Timer)
	, m_BasePerWave(other.m_BasePerWave)
	, m_Interval(other.m_Interval)
{
	other.m_WaveCount = 0;
	other.m_Timer = 0.0f;
	other.m_BasePerWave = 0;
	other.m_Interval = 0.0f;
}
WaveManager& WaveManager::operator=(WaveManager&& other) noexcept
{
	if (this != &other)
	{
		m_WaveCount = other.m_WaveCount;
		m_Timer = other.m_Timer;
		m_BasePerWave = other.m_BasePerWave;
		m_Interval = other.m_Interval;
		other.m_WaveCount = 0;
		other.m_Timer = 0.0f;
		other.m_BasePerWave = 0;
		other.m_Interval = 0.0f;
	}
	return *this;
}
// Initialize the wave manager
void WaveManager::Initialize(std::vector<Enemy*>& outNew)
{
	m_WaveCount = 0;
	m_Timer = 0.0f;

	SpawnNextWave(outNew); // Spawn the first wave
}

void WaveManager::Update(float deltaTime, std::vector<Enemy*>& outNew)
{
	m_Timer += deltaTime;
	if (m_Timer >= m_Interval)
	{
		m_Timer -= m_Interval;
		SpawnNextWave(outNew);
	}
}

// Spawn the next wave of enemies
void WaveManager::SpawnNextWave(std::vector<Enemy*>& out)
{
	int numEnemies = m_BasePerWave + (m_WaveCount * 2); // Increase number of enemies each wave
	for (int i = 0; i < numEnemies; ++i)
	{
		float posX = RandomRange(0.0f, 800.0f); // Randomize enemy position
		float posY = RandomRange(0.0f, 600.0f); // Randomize enemy position
		out.push_back(new Enemy(posX, posY, m_pPlayer, 100.0f)); // Create new enemy
	}
	m_WaveCount++;
}
// Random range function
float WaveManager::RandomRange(float min, float max)
{
	return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}