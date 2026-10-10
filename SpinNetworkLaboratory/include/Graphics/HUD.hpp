#pragma once
#include "Application/SimulationController.hpp"
#include <SFML/Graphics.hpp>
namespace snl {
class HUD {
public:
    static constexpr float width=290;
    void draw(sf::RenderTarget& target,const SimulationController& simulation,const sf::Font& font,double fps)const;
    bool click(sf::Vector2i pixel,SimulationController& simulation)const;
};
}
