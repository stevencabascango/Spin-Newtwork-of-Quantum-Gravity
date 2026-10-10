#include "Application/SimulationController.hpp"
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("Failed: " #x);}while(false)
using namespace snl;
bool close(double a,double b,double tolerance=1e-8){return std::abs(a-b)<tolerance;}
QuantumGraph oscillator(){QuantumGraph g;g.addVertex(1,{-60,0});g.addVertex(2,{60,0});g.addEdge(1,2,0.5,90,1);return g;}
double energy(const QuantumGraph& g){const auto& a=*g.findVertex(1);const auto& b=*g.findVertex(2);double extension=length(b.position-a.position)-90;return .5*(a.velocity.x*a.velocity.x+a.velocity.y*a.velocity.y+b.velocity.x*b.velocity.x+b.velocity.y*b.velocity.y)+extension*extension;}
int main(){
    PhysicsParameters params;params.repulsion=0;params.damping=0;
    auto g=oscillator();ForceEngine::compute(g,params);
    CHECK(close(g.findVertex(1)->force.x,60));CHECK(close(g.findVertex(2)->force.x,-60));
    const double initialEnergy=energy(g);
    for(int i=0;i<1200;++i)VerletIntegrator::step(g,params);
    CHECK(std::abs(energy(g)-initialEnergy)/initialEnergy<0.001);
    CHECK(close(g.findVertex(1)->position.x+g.findVertex(2)->position.x,0));
    params.damping=2;const double beforeDamping=energy(g);
    for(int i=0;i<1200;++i)VerletIntegrator::step(g,params);
    CHECK(energy(g)<beforeDamping*.01);
    QuantumGraph free;free.addVertex(1,{0,0},2);free.findVertex(1)->velocity={10,0};
    params.spring=0;params.repulsion=0;params.damping=2;
    for(int i=0;i<120;++i)VerletIntegrator::step(free,params);
    CHECK(close(free.findVertex(1)->velocity.x,10*std::exp(-1)));
    free.findVertex(1)->pinned=true;const auto pinnedPosition=free.findVertex(1)->position;
    free.findVertex(1)->velocity={9,8};VerletIntegrator::step(free,params);
    CHECK(close(free.findVertex(1)->position.x,pinnedPosition.x));CHECK(length(free.findVertex(1)->velocity)==0);
    QuantumGraph coincident;coincident.addVertex(1);coincident.addVertex(2);
    params.repulsion=12000;ForceEngine::compute(coincident,params);
    CHECK(length(coincident.findVertex(1)->force)>0);
    CHECK(close(coincident.findVertex(1)->force.x+coincident.findVertex(2)->force.x,0));
    for(int i=0;i<1200;++i)VerletIntegrator::step(coincident,params);
    CHECK(finite(coincident.findVertex(1)->position));CHECK(length(coincident.findVertex(1)->position-coincident.findVertex(2)->position)>0);
    SimulationController reference;reference.loadGrid();auto at30=reference,at60=reference,at144=reference;
    for(int i=0;i<30;++i)at30.update(1.0/30);
    for(int i=0;i<60;++i)at60.update(1.0/60);
    for(int i=0;i<144;++i)at144.update(1.0/144);
    for(auto id:reference.graph.ids()){
        CHECK(length(at30.graph.findVertex(id)->position-at60.graph.findVertex(id)->position)<1e-8);
        CHECK(length(at30.graph.findVertex(id)->position-at144.graph.findVertex(id)->position)<1e-8);
    }
    CHECK(at30.update(10)==12);at30.togglePause();CHECK(at30.update(10)==0);
    at30.togglePause();CHECK(at30.update(0)==0);
    bool rejected=false;try{VerletIntegrator::step(g,params,0);}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);
    rejected=false;params.damping=std::numeric_limits<double>::quiet_NaN();try{VerletIntegrator::step(g,params);}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);
    SimulationController large;large.loadGrid(10,10);
    large.graph.findVertex(1)->pinned=true;const auto fixed=large.graph.findVertex(1)->position;
    auto start=std::chrono::steady_clock::now();
    for(int i=0;i<7200;++i)VerletIntegrator::step(large.graph,large.params);
    const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    large.graph.forEachVertex([](const QuantumVertex& v){CHECK(finite(v.position));CHECK(finite(v.velocity));CHECK(length(v.position)<10000);});
    CHECK(length(large.graph.findVertex(1)->position-fixed)==0);
    CHECK(large.graph.size()==100);CHECK(large.graph.edgeCount()==261);
    std::cout<<"PASS: forces, symmetry, Verlet energy, damping/mass, fixed nodes, coincident nodes, 30/60/144 FPS equivalence, catch-up and invalid parameters.\n";
    std::cout<<"PASS: 100 nodes / 261 edges / 7200 steps (60 simulated seconds), "<<elapsed<<" s, "<<elapsed*1000/7200<<" ms/step.\n";
}
