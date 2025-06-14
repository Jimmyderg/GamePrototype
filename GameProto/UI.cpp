// UI.cpp
#include "pch.h"
#include "UI.h"

#include "Texture.h"   // engine text?to?texture helper
#include "utils.h"     // for filled shapes

UI::UI(Player* player, WaveManager* waveManager, const std::string& fontPath, int ptSize)
    : m_pPlayer(player)
    , m_pWaveMgr(waveManager)
    , m_FontPath(fontPath)
    , m_FontSize(ptSize)
{
}

void UI::Draw(float reloadProgress) const
{
    DrawHealthBar();
    DrawXPBar();
    DrawReloadCircle(reloadProgress);
    DrawWaveCounter();
}

void UI::DrawHealthBar() const
{
    const float x = 10.f, y = 10.f;
    const float w = 200.f, h = 20.f;
    float pct = m_pPlayer->GetHealth() / m_pPlayer->GetMaxHealth();

    // background
    utils::SetColor(Color4f{ 0, 0, 0, 0.6f });
    utils::FillRect(x, y, w, h);

    // fill
    utils::SetColor(Color4f{ 0, 1, 0, 1.0f });
    utils::FillRect(x, y, w * pct, h);
}

void UI::DrawXPBar() const
{
    const float x = 10.f, y = 35.f;
    const float w = 200.f, h = 10.f;
    float pct = float(m_pPlayer->GetCurrentXP()) / float(m_pPlayer->GetXPToNextLevel());

    // background
    utils::SetColor(Color4f{ 0, 0, 0, 0.6f });
    utils::FillRect(x, y, w, h);

    // fill
    utils::SetColor(Color4f{ 0, 0, 1, 1.0f });
    utils::FillRect(x, y, w * pct, h);
}

void UI::DrawReloadCircle(float progress) const
{
    const float radius = 5.f;
    Vector2f center{
        m_pPlayer->GetPosition().x + 16.f,
        m_pPlayer->GetPosition().y + 32.f + radius
    };

    // background circle
    utils::SetColor(Color4f{ 0, 0, 0, 0.4f });
    utils::FillEllipse(center, radius, radius);

    // progress arc (from 0 to 2?·progress)
    utils::SetColor(Color4f{ 0, 1, 0, 0.8f });
    utils::FillArc(
        center,
        radius, radius,
        0.0f,
        utils::g_Pi * 2.0f * progress
    );
}

void UI::DrawWaveCounter() const
{
    std::string txt = "Wave: " + std::to_string(static_cast<int>(m_pWaveMgr->GetCurrentWave()));
    Texture waveTex{
      txt,
      m_FontPath,   // your .ttf
      m_FontSize,
      Color4f{1,1,1,1}
    };
	waveTex.Draw(Rectf{
		10.f,  // x
		60.f,  // y
		waveTex.GetWidth(),
		waveTex.GetHeight()
		});
}