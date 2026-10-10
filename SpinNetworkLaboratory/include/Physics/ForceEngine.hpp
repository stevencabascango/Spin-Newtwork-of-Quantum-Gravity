#pragma once
#include "Core/QuantumGraph.hpp"
#include "PhysicsParameters.hpp"
#include <vector>
namespace snl {
class ForceEngine {
public:
    // Force vectors exclude damping, applied exactly by the integrator.
    static void compute(QuantumGraph& graph,const PhysicsParameters& params) {
        params.validate();
        std::vector<QuantumVertex*> vertices;
        vertices.reserve(graph.size());
        graph.forEachVertex([&](QuantumVertex& v){v.force={};vertices.push_back(&v);});
        for(std::size_t i=0;i<vertices.size();++i) {
            auto& a=*vertices[i];
            for(std::size_t j=i+1;j<vertices.size();++j) {
                auto& b=*vertices[j];
                Vec2 delta=b.position-a.position;
                double distance=length(delta);
                // Deterministic antisymmetric separation for coincident vertices.
                const Vec2 direction=distance>1e-12?delta/distance:Vec2{a.id<b.id?1.0:-1.0,0};
                const Vec2 repulsion=direction*(params.repulsion/(distance*distance+params.softening*params.softening));
                a.force-=repulsion;b.force+=repulsion;
            }
            for(const auto& edge:a.edges) {
                if(a.id>=edge.targetId)continue; // each undirected edge once
                auto& b=*graph.findVertex(edge.targetId);
                const Vec2 delta=b.position-a.position;
                const double distance=length(delta);
                const Vec2 spring=delta*(params.spring*edge.stiffness*(distance-edge.restLength)/(distance+1e-12));
                a.force+=spring;b.force-=spring;
            }
        }
    }
};
}
