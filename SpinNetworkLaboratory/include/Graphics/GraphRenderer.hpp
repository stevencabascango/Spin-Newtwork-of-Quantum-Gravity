#pragma once
#include "Application/SimulationController.hpp"
#include <SFML/Graphics.hpp>
namespace snl {
class GraphRenderer {
public:
    void draw(sf::RenderTarget& target,const SimulationController& simulation,const sf::Font& font)const;
};
}
