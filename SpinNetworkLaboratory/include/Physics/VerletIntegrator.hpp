#pragma once
#include "ForceEngine.hpp"
#include <optional>
namespace snl {
class VerletIntegrator {
public:
    static void step(QuantumGraph& graph,const PhysicsParameters& params,
                     double dt=PhysicsParameters::timeStep,std::optional<std::uint64_t> dragged={}) {
        if(!std::isfinite(dt)||dt<=0||dt>PhysicsParameters::timeStep+1e-12)
            throw std::invalid_argument("Physics requires a positive step <= 1/120 s");
        ForceEngine::compute(graph,params);
        graph.forEachVertex([&](QuantumVertex& v){
            v.previousPosition=v.position;
            if(v.pinned||dragged==v.id){v.velocity={};v.acceleration={};return;}
            const double decay=std::exp(-params.damping*dt/(2*v.mass));
            v.acceleration=v.force/v.mass;
            v.velocity=v.velocity*decay+v.acceleration*(dt/2);
            v.position+=v.velocity*dt;
        });
        ForceEngine::compute(graph,params);
        graph.forEachVertex([&](QuantumVertex& v){
            if(v.pinned||dragged==v.id)return;
            v.acceleration=v.force/v.mass;
            const double decay=std::exp(-params.damping*dt/(2*v.mass));
            v.velocity=(v.velocity+v.acceleration*(dt/2))*decay;
        });
    }
};
// Rendering contributes elapsed time; physics always uses the same fixed step.
class FixedStepClock {
    double accumulator_=0;
public:
    void reset(){accumulator_=0;}
    template<class Step> unsigned advance(double elapsed,bool paused,Step step) {
        if(paused){reset();return 0;}
        if(!std::isfinite(elapsed)||elapsed<0)throw std::invalid_argument("Invalid elapsed time");
        accumulator_+=std::min(elapsed,0.1); // bound catch-up after stalls
        unsigned count=0;
        while(accumulator_+1e-12>=PhysicsParameters::timeStep&&count<12){
            step();accumulator_-=PhysicsParameters::timeStep;++count;
        }
        accumulator_=std::max(0.0,accumulator_);
        return count;
    }
};
}
