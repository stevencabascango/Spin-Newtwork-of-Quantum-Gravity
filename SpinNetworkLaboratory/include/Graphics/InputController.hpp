#pragma once
#include "Graphics/HUD.hpp"
namespace snl {
class InputController {
    sf::View graphView_;
public:
    void resize(sf::Vector2u size);
    void fit(const QuantumGraph& graph,sf::Vector2u size);
    const sf::View& view()const{return graphView_;}
    void handle(const sf::Event& event,sf::RenderWindow& window,SimulationController& simulation,const HUD& hud);
};
}
