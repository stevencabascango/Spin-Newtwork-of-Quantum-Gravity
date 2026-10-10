#pragma once
#include "QuantumGraph.hpp"
namespace snl {
struct GraphMetrics {std::size_t vertices=0,edges=0,beta0=0;std::int64_t beta1=0,euler=0;double averageDegree=0;};
inline GraphMetrics calculateMetrics(const QuantumGraph& g){GraphMetrics m;m.vertices=g.size();m.edges=g.edgeCount();HashTable<std::uint64_t,bool> seen;
for(auto id:g.ids()){if(seen.contains(id))continue;++m.beta0;std::vector<std::uint64_t> stack{id};seen.insert(id,true);while(!stack.empty()){auto current=stack.back();stack.pop_back();for(auto n:g.neighbors(current))if(!seen.contains(n)){seen.insert(n,true);stack.push_back(n);}}}
m.euler=static_cast<std::int64_t>(m.vertices)-static_cast<std::int64_t>(m.edges);m.beta1=static_cast<std::int64_t>(m.beta0)-m.euler;m.averageDegree=m.vertices?2.0*m.edges/m.vertices:0;return m;}
}
