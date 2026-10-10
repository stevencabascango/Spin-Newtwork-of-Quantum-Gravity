#include "Graphics/HUD.hpp"
#include <iomanip>
#include <sstream>
namespace snl {
namespace {
std::string number(double value){std::ostringstream out;out<<std::fixed<<std::setprecision(1)<<value;return out.str();}
void text(sf::RenderTarget& target,const sf::Font& font,const std::string& value,float x,float y,unsigned size=14,sf::Color color=sf::Color(176,196,206)){
    sf::Text label(value,font,size);label.setPosition(x,y);label.setFillColor(color);target.draw(label);
}
void button(sf::RenderTarget& target,const sf::Font& font,const std::string& label,float y,bool active=false){
    sf::RectangleShape rectangle({250,32});rectangle.setPosition(20,y);
    rectangle.setFillColor(active?sf::Color(45,65,43):sf::Color(24,35,43));
    rectangle.setOutlineColor(active?sf::Color(196,245,126):sf::Color(40,52,61));rectangle.setOutlineThickness(1);
    target.draw(rectangle);text(target,font,label,30,y+7,13);
}
}
void HUD::draw(sf::RenderTarget& target,const SimulationController& s,const sf::Font& font,double fps)const {
    sf::RectangleShape panel({width,float(target.getSize().y)});panel.setFillColor({17,25,32});target.draw(panel);
    text(target,font,"SPIN NETWORK",20,20,22,sf::Color(196,245,126));
    text(target,font,"LABORATORY / WORKSHOP 1",20,51,12);
    text(target,font,"AVANCE FUNCIONAL - C++ / SFML",20,78,11);
    button(target,font,"Seleccionar / arrastrar [S]",110,s.tool==Tool::Select);
    button(target,font,"Agregar vertice [A]",150,s.tool==Tool::Add);
    button(target,font,"Conectar dos vertices [C]",190,s.tool==Tool::Connect);
    button(target,font,"Eliminar nodo o arista [D]",230,s.tool==Tool::Delete);
    button(target,font,s.paused?"Reanudar [Space]":"Pausar [Space]",275);
    button(target,font,"Fijar / liberar seleccionado [P]",315);
    button(target,font,"Reiniciar posiciones [R]",355);
    text(target,font,"Vertices: "+std::to_string(s.graph.size())+"    Aristas: "+std::to_string(s.graph.edgeCount()),20,411);
    text(target,font,s.paused?"SIMULACION PAUSADA":"SIMULACION ACTIVA",20,439,12,sf::Color(196,245,126));
    text(target,font,"Resorte: "+number(s.params.spring)+"  [ / ]",20,471);
    text(target,font,"Repulsion: "+number(s.params.repulsion)+"  Up/Down",20,499,12);
    text(target,font,"Amortiguamiento: "+number(s.params.damping)+"  , / .",20,527,12);
    text(target,font,"Paso fijo: 1/120 s    FPS: "+number(fps),20,557,12);
    if(s.selected){const auto* v=s.graph.findVertex(*s.selected);if(v)text(target,font,"ID "+std::to_string(v->id)+" | grado "+std::to_string(v->edges.size())+" | "+(v->pinned?"fijo":"libre"),20,591,12);}
    text(target,font,"1: red inicial   2: red de 100 nodos",20,629,12);
    text(target,font,"Delete: borrar seleccionado",20,651,12);
    text(target,font,"F: encuadrar    Esc: salir",20,673,12);
    text(target,font,s.message,width+20,15,14);
    text(target,font,"Fuerzas artificiales de visualizacion | Workshop 2: historial y analisis",width+20,float(target.getSize().y)-30,12);
}
bool HUD::click(sf::Vector2i pixel,SimulationController& s)const {
    if(pixel.x<0||pixel.x>=width)return false;
    if(pixel.x<20||pixel.x>270)return true;
    const int y=pixel.y;
    if(y>=110&&y<=142)s.setTool(Tool::Select);
    else if(y>=150&&y<=182)s.setTool(Tool::Add);
    else if(y>=190&&y<=222)s.setTool(Tool::Connect);
    else if(y>=230&&y<=262)s.setTool(Tool::Delete);
    else if(y>=275&&y<=307)s.togglePause();
    else if(y>=315&&y<=347)s.togglePin();
    else if(y>=355&&y<=387)s.resetPositions();
    return true;
}
}
