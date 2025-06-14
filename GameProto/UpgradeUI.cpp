// UpgradeUI.cpp
#include "pch.h"
#include "UpgradeUI.h"

static std::string TitleFor(Player::Upgrade u) {
    switch (u) {
    case Player::Upgrade::M_DAMAGE:  return "+5% melee damage";
    case Player::Upgrade::R_DAMAGE:  return "+5% ranged damage";
    case Player::Upgrade::M_RANGE:   return "+10% melee range";
	case Player::Upgrade::R_Range:   return "+10% ranged range";
    case Player::Upgrade::F_RATE:    return "+10% fire rate";
    case Player::Upgrade::B_PIERCE:  return "+1 bullet pierce";
    case Player::Upgrade::MAX_HEALTH:return "+20 max health";
	case Player::Upgrade::MOVE_SPEED: return "+10% move speed";
    }
    return "";
}
Rectf UpgradeUI::GetIconSrc(Player::Upgrade u)
{
    // assume your atlas is laid out:
    //  [ M_DAMAGE | R_DAMAGE ]
    //  [ M_RANGE  | F_RATE   ]
    //  [ B_PIERCE | MAX_HEALTH ]
    //  [ MOVE_SPEED | <empty> ]
    // each cell is 16×16 in a 32×32 atlas.
    switch (u) {
    case Player::Upgrade::M_DAMAGE:   return Rectf{ 0, 0, 16, 16 };
    case Player::Upgrade::R_DAMAGE:   return Rectf{ 16,0, 16, 16 };
    case Player::Upgrade::M_RANGE:    return Rectf{ 0, 16, 16, 16 };
	case Player::Upgrade::R_Range:    return Rectf{ 16, 16, 16, 16 };
    case Player::Upgrade::F_RATE:     return Rectf{ 0, 32, 16, 16 };
    case Player::Upgrade::B_PIERCE:   return Rectf{ 16, 32, 16, 16 };
    case Player::Upgrade::MAX_HEALTH: return Rectf{ 0 , 48, 16, 16 };
    case Player::Upgrade::MOVE_SPEED: return Rectf{ 16, 48, 16, 16 };
    }
	// default case, if no match found
    return Rectf{ 0,0,16,16 };
}
UpgradeUI::UpgradeUI(const std::array<Player::Upgrade, 3>& opts,
    const std::string& fontPath, int fontSize,
    float screenW, float screenH)
    : m_FontPath(fontPath)
    , m_FontSize(fontSize)
    , m_ScrW(screenW)
    , m_ScrH(screenH)
{
    for (int i = 0; i < 3; ++i) {
        m_Opts[i].type = opts[i];
        m_Opts[i].title = TitleFor(opts[i]);
    }
	for (int i = 0; i < 3; ++i) {
		m_Opts[i].title = TitleFor(opts[i]);
    }
	m_Atlas = new Texture{ "IconPack.png" }; // Load the atlas texture
}
UpgradeUI::~UpgradeUI()
{
	delete m_Atlas;
	m_Atlas = nullptr;
}
void UpgradeUI::Draw() const
{


    // darken background
    utils::SetColor(Color4f{ 0,0,0,0.6f });
    utils::FillRect(0, 0, m_ScrW, m_ScrH);

    float w = m_ScrW / 3.0f;
    float h = m_ScrH / 3.0f;
    for (int i = 0; i < 3; ++i) {

        float x0 = i * w, y0 = m_ScrH / 3.0f;
        // panel background
        utils::SetColor(Color4f{ 0.2f,0.2f,0.2f,0.8f });
        utils::FillRect(x0, y0, w, h);

        // draw icon inside its panel
		const float iconScale = 5.0f; // size of the icon in pixels

        Rectf src = GetIconSrc(m_Opts[i].type);
        Rectf dst{
            x0 + w / 3.f,              // left
            y0 + h / 3.f,              // bottom
            src.width * iconScale, // width
            src.height * iconScale  // height
        };
        m_Atlas->Draw(dst, src);

        // draw title
        Texture{ m_Opts[i].title, m_FontPath, m_FontSize, Color4f{1,1,1,1} }.Draw(Vector2f{ x0, y0});
    }
}

bool UpgradeUI::HandleMouseClick(float mx, float my, Player& player)
{
    if (my < m_ScrH / 3.0f || my > 4 * m_ScrH / 3.0f) return false;
    int idx = static_cast<int>(mx / (m_ScrW / 3.0f));
    if (idx < 0 || idx>2) return false;
    player.ApplyUpgrade(m_Opts[idx].type);
    return true;
}

