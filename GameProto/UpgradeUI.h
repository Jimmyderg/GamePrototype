#pragma once  
#include "Player.h"  
#include "Texture.h"  
#include "structs.h"  
#include "utils.h"
#include <array>  
#include <string>  

class UpgradeUI
{
public:
    struct Opt
    {
        Player::Upgrade type;
        std::string title;
    };

    UpgradeUI(const std::array<Player::Upgrade, 3>& opts, const std::string& fontPath, int fontSize, float screenW, float screenH);
    ~UpgradeUI();
    void Draw() const;
    bool HandleMouseClick(float mx, float my, Player& player);

private:
    std::array<Opt, 3> m_Opts;
    std::string m_FontPath;
    int m_FontSize;
    float m_ScrW;
    float m_ScrH;
    Texture* m_Atlas;

    static Rectf GetIconSrc(Player::Upgrade u);
};