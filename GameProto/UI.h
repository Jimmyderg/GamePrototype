// UI.h
#pragma once
#include "Player.h"
#include "WaveManager.h"
#include <string>

class UI
{
public:
    // fontPath = path to your .ttf, ptSize = font size in points
    UI(Player* player, WaveManager* waveManager, const std::string& fontPath, int ptSize);
    ~UI() = default;

    // reloadProgress: [0…1]
    void Draw(float reloadProgress) const;

private:
    void DrawHealthBar()    const;
    void DrawXPBar()        const;
    void DrawReloadCircle(float progress) const;
    void DrawWaveCounter()  const;

    Player* m_pPlayer;
    WaveManager* m_pWaveMgr;
    std::string  m_FontPath;
    int          m_FontSize;
};