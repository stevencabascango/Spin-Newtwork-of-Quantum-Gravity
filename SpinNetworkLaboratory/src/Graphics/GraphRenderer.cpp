#include "Graphics/GraphRenderer.hpp"
namespace snl {
void GraphRenderer::draw(sf::RenderTarget& target,const SimulationController& simulation,const sf::Font& font)const {
    simulation.graph.forEachVertex([&](const QuantumVertex& a){
        for(const auto& edge:a.edges){
            if(a.id>=edge.targetId)continue;
            const auto& b=*simulation.graph.findVertex(edge.targetId);
            const bool active=simulation.selected==a.id||simulation.selected==b.id;
            const sf::Color color=active?sf::Color(196,245,126):sf::Color(86,111,127);
            const sf::Vertex line[]={sf::Vertex({float(a.position.x),float(a.position.y)},color),sf::Vertex({float(b.position.x),float(b.position.y)},color)};
            target.draw(line,2,sf::Lines);
        }
    });
    simulation.graph.forEachVertex([&](const QuantumVertex& v){
        sf::CircleShape circle(8);
        circle.setOrigin(8,8);circle.setPosition(float(v.position.x),float(v.position.y));
        circle.setFillColor(v.pinned?sf::Color(107,186,247):sf::Color(196,245,126));
        if(simulation.selected==v.id){circle.setOutlineColor(sf::Color::White);circle.setOutlineThickness(3);}
        target.draw(circle);
        sf::Text label(std::to_string(v.id),font,12);
        const auto bounds=label.getLocalBounds();
        label.setPosition(float(v.position.x)-bounds.width/2,float(v.position.y)+12);
        label.setFillColor(sf::Color(176,196,206));target.draw(label);
    });
}
}
