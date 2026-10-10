#include "Graphics/InputController.hpp"
namespace snl {
void InputController::resize(sf::Vector2u size){
    const float width=float(std::max(size.x,400u)),height=float(std::max(size.y,1u));
    graphView_.setViewport({HUD::width/width,0,(width-HUD::width)/width,1});
    const float viewHeight=graphView_.getSize().y>0?graphView_.getSize().y:700;
    graphView_.setSize(viewHeight*(width-HUD::width)/height,viewHeight);
}
void InputController::fit(const QuantumGraph& graph,sf::Vector2u size){
    double minX=0,maxX=0,minY=0,maxY=0;bool first=true;
    graph.forEachVertex([&](const QuantumVertex& v){if(first){minX=maxX=v.position.x;minY=maxY=v.position.y;first=false;}else{minX=std::min(minX,v.position.x);maxX=std::max(maxX,v.position.x);minY=std::min(minY,v.position.y);maxY=std::max(maxY,v.position.y);}});
    const double aspect=double(std::max(size.x,400u)-unsigned(HUD::width))/std::max(size.y,1u);
    const double height=std::max({450.0,maxY-minY+150,(maxX-minX+150)/aspect});
    graphView_.setCenter(float((minX+maxX)/2),float((minY+maxY)/2));
    graphView_.setSize(float(height*aspect),float(height));resize(size);
}
void InputController::handle(const sf::Event& event,sf::RenderWindow& window,SimulationController& s,const HUD& hud){
    auto position=[&](sf::Vector2i pixel){const auto p=window.mapPixelToCoords(pixel,graphView_);return Vec2{p.x,p.y};};
    if(event.type==sf::Event::Closed){s.endDrag();window.close();return;}
    if(event.type==sf::Event::Resized){if(event.size.width<600||event.size.height<720)window.setSize({std::max(event.size.width,600u),std::max(event.size.height,720u)});resize(window.getSize());return;}
    if(event.type==sf::Event::LostFocus){s.endDrag();return;}
    if(event.type==sf::Event::MouseButtonPressed&&event.mouseButton.button==sf::Mouse::Left){
        const sf::Vector2i pixel{event.mouseButton.x,event.mouseButton.y};
        if(!hud.click(pixel,s))s.pointerDown(position(pixel));
    }
    if(event.type==sf::Event::MouseMoved)s.pointerMove(position({event.mouseMove.x,event.mouseMove.y}));
    if(event.type==sf::Event::MouseButtonReleased&&event.mouseButton.button==sf::Mouse::Left)s.endDrag();
    if(event.type!=sf::Event::KeyPressed)return;
    switch(event.key.code){
        case sf::Keyboard::Escape:window.close();break;
        case sf::Keyboard::Space:s.togglePause();break;
        case sf::Keyboard::S:s.setTool(Tool::Select);break;
        case sf::Keyboard::C:s.setTool(Tool::Connect);break;
        case sf::Keyboard::D:s.setTool(Tool::Delete);break;
        case sf::Keyboard::A:{const auto mouse=sf::Mouse::getPosition(window);if(mouse.x>=HUD::width&&mouse.x<int(window.getSize().x)&&mouse.y>=0&&mouse.y<int(window.getSize().y))s.addVertex(position(mouse));else{s.setTool(Tool::Add);s.message="Haz clic en el lienzo para agregar.";}break;}
        case sf::Keyboard::Delete:s.removeSelected();break;
        case sf::Keyboard::P:s.togglePin();break;
        case sf::Keyboard::R:s.resetPositions();fit(s.graph,window.getSize());break;
        case sf::Keyboard::F:fit(s.graph,window.getSize());break;
        case sf::Keyboard::Up:s.params.repulsion=std::min(100000.0,s.params.repulsion+500);break;
        case sf::Keyboard::Down:s.params.repulsion=std::max(0.0,s.params.repulsion-500);break;
        case sf::Keyboard::LBracket:s.params.spring=std::max(0.0,s.params.spring-0.1);break;
        case sf::Keyboard::RBracket:s.params.spring=std::min(8.0,s.params.spring+0.1);break;
        case sf::Keyboard::Comma:s.params.damping=std::max(0.0,s.params.damping-0.1);break;
        case sf::Keyboard::Period:s.params.damping=std::min(8.0,s.params.damping+0.1);break;
        case sf::Keyboard::Num1:s.loadGrid();fit(s.graph,window.getSize());break;
        case sf::Keyboard::Num2:s.loadGrid(10,10);fit(s.graph,window.getSize());break;
        default:break;
    }
}
}
