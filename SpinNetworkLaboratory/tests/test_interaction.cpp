#include "Application/SimulationController.hpp"
#include <iostream>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("Failed: " #x);}while(false)
using namespace snl;
int main(){
    SimulationController s;s.paused=true;
    s.setTool(Tool::Add);s.pointerDown({-90,0});s.pointerDown({90,0});CHECK(s.graph.size()==2);
    s.setTool(Tool::Connect);s.pointerDown({-90,0});s.pointerDown({90,0});CHECK(s.graph.edgeCount()==1);
    s.pointerDown({-90,0});s.pointerDown({90,0});CHECK(s.graph.edgeCount()==1);
    s.pointerDown({-90,0});s.pointerDown({-90,0});CHECK(s.graph.edgeCount()==1);
    s.setTool(Tool::Select);s.pointerDown({-80,0});s.pointerMove({-50,20});s.endDrag();
    CHECK(length(s.graph.findVertex(1)->position-Vec2{-60,20})<1e-8);
    s.pointerMove({0,0});CHECK(length(s.graph.findVertex(1)->position-Vec2{-60,20})<1e-8);
    s.togglePin();CHECK(s.graph.findVertex(1)->pinned);s.togglePause();s.update(.1);
    CHECK(length(s.graph.findVertex(1)->position-Vec2{-60,20})<1e-8);
    s.togglePin();CHECK(!s.graph.findVertex(1)->pinned);s.togglePause();
    const auto a=s.graph.findVertex(1)->position,b=s.graph.findVertex(2)->position;
    s.setTool(Tool::Delete);s.pointerDown((a+b)/2);CHECK(s.graph.edgeCount()==0);
    s.setTool(Tool::Connect);s.pointerDown(a);s.pointerDown(b);CHECK(s.graph.edgeCount()==1);
    s.setTool(Tool::Select);s.pointerDown(b);s.endDrag();CHECK(s.removeSelected());
    CHECK(s.graph.size()==1);CHECK(s.graph.edgeCount()==0);CHECK(!s.removeSelected());
    s.loadGrid();CHECK(s.graph.size()==20);CHECK(s.graph.edgeCount()==43);
    s.selected=1;s.togglePin();const auto count=s.graph.edgeCount();s.resetPositions();
    CHECK(s.graph.edgeCount()==count);CHECK(s.graph.findVertex(1)->pinned);
    s.loadGrid(10,10);CHECK(s.graph.size()==100);CHECK(s.graph.edgeCount()==261);
    CHECK(!s.selected);s.setTool(Tool::Add);
    for(int i=0;i<400;++i)CHECK(s.addVertex({double(i),0}));
    CHECK(!s.addVertex({0,0}));CHECK(s.graph.size()==500);
    std::cout<<"PASS: mouse editing, duplicates/loops, grab offset, drag release, pin/unpin, pause, edge/node deletion, position reset, presets and visual limit.\n";
}
