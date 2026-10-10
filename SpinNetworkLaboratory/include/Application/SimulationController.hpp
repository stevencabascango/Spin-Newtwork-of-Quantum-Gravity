#pragma once
#include "Physics/VerletIntegrator.hpp"
#include <string>
namespace snl {
enum class Tool { Select, Add, Connect, Delete };
class SimulationController {
    std::uint64_t nextId_=1;
    std::optional<std::uint64_t> source_,dragged_;
    Vec2 grabOffset_;
    FixedStepClock clock_;
public:
    QuantumGraph graph;
    PhysicsParameters params;
    Tool tool=Tool::Select;
    bool paused=false;
    std::optional<std::uint64_t> selected;
    std::string message="Selecciona o arrastra un vertice.";
    void setTool(Tool value){tool=value;source_.reset();endDrag();}
    void loadGrid(unsigned rows=4,unsigned columns=5) {
        graph.clear();nextId_=1;selected.reset();source_.reset();endDrag();clock_.reset();
        for(unsigned row=0;row<rows;++row)for(unsigned col=0;col<columns;++col)
            graph.addVertex(nextId_++,{(double(col)-(columns-1)/2.0)*90+(row%2)*45,(double(row)-(rows-1)/2.0)*78});
        const auto ids=graph.ids();
        for(std::size_t i=0;i<ids.size();++i)for(std::size_t j=i+1;j<ids.size();++j)
            if(length(graph.findVertex(ids[i])->position-graph.findVertex(ids[j])->position)<100)
                graph.addEdge(ids[i],ids[j],0.5,90,1);
        message="Red cargada: "+std::to_string(graph.size())+" vertices.";
    }
    std::optional<std::uint64_t> hit(Vec2 p,double radius=18)const {
        std::optional<std::uint64_t> result;
        graph.forEachVertex([&](const QuantumVertex& v){const double d=length(v.position-p);if(d<radius){radius=d;result=v.id;}});
        return result;
    }
    bool addVertex(Vec2 p) {
        if(graph.size()>=500){message="Limite visual: 500 vertices.";return false;}
        selected=nextId_++;graph.addVertex(*selected,p);source_.reset();message="Vertice agregado.";return true;
    }
    void pointerDown(Vec2 p) {
        const auto id=hit(p);
        if(tool==Tool::Add){addVertex(p);return;}
        if(tool==Tool::Delete){if(id){selected=id;removeSelected();}else removeEdgeAt(p);return;}
        selected=id;
        if(tool==Tool::Connect){
            if(!id)return;
            if(!source_){source_=id;message="Selecciona el destino.";}
            else{message=graph.addEdge(*source_,*id,0.5,90,1)?"Arista agregada.":"Conexion rechazada: lazo o duplicado.";source_.reset();}
            return;
        }
        if(id){dragged_=id;auto& v=*graph.findVertex(*id);grabOffset_=v.position-p;v.velocity={};}
    }
    void pointerMove(Vec2 p){if(dragged_){auto* v=graph.findVertex(*dragged_);if(v){v->position=p+grabOffset_;v->previousPosition=v->position;v->velocity={};}}}
    void endDrag(){dragged_.reset();}
    bool removeSelected(){
        if(!selected)return false;
        const bool removed=graph.removeVertex(*selected);
        selected.reset();source_.reset();endDrag();message="Vertice eliminado con sus aristas.";return removed;
    }
    bool removeEdgeAt(Vec2 p) {
        double best=10;
        std::optional<std::pair<std::uint64_t,std::uint64_t>> edge;
        graph.forEachVertex([&](const QuantumVertex& a){for(const auto& e:a.edges){if(a.id>=e.targetId)continue;const auto& b=*graph.findVertex(e.targetId);
            const Vec2 delta=b.position-a.position,relative=p-a.position;
            const double square=delta.x*delta.x+delta.y*delta.y;
            const double t=square?std::clamp((relative.x*delta.x+relative.y*delta.y)/square,0.0,1.0):0;
            const double distance=length(p-(a.position+delta*t));
            if(distance<best){best=distance;edge={{a.id,b.id}};}
        }});
        if(!edge)return false;
        graph.removeEdge(edge->first,edge->second);source_.reset();message="Arista eliminada.";return true;
    }
    void togglePin(){if(selected){auto& v=*graph.findVertex(*selected);v.pinned=!v.pinned;v.velocity={};message=v.pinned?"Vertice fijado.":"Vertice liberado.";}}
    void togglePause(){paused=!paused;clock_.reset();}
    void resetPositions(){
        const auto ids=graph.ids();
        const double radius=std::max(120.0,double(ids.size())*8.0);
        for(std::size_t i=0;i<ids.size();++i){auto& v=*graph.findVertex(ids[i]);const double angle=6.283185307179586*i/ids.size();v.position={radius*std::cos(angle),radius*std::sin(angle)};v.previousPosition=v.position;v.velocity={};v.acceleration={};v.force={};}
        endDrag();clock_.reset();message="Posiciones reiniciadas; se conserva la topologia y las fijaciones.";
    }
    unsigned update(double elapsed){return clock_.advance(elapsed,paused,[&]{VerletIntegrator::step(graph,params,PhysicsParameters::timeStep,dragged_);});}
};
}
