#pragma once
#include "QuantumVertex.hpp"
#include "DataStructures/HashTable.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
namespace snl {
// Undirected simple graph: each edge has two adjacency records with the same ID.
class QuantumGraph {
    HashTable<std::uint64_t,QuantumVertex> vertices_;
    std::uint64_t nextEdge_=1;std::size_t edges_=0;
public:
    bool addVertex(std::uint64_t id,Vec2 position={},double mass=1){if(!std::isfinite(mass)||mass<=0||!std::isfinite(position.x)||!std::isfinite(position.y))throw std::invalid_argument("Invalid vertex");QuantumVertex v;v.id=id;v.position=v.previousPosition=position;v.mass=mass;return vertices_.insert(id,v);}
    bool contains(std::uint64_t id)const{return vertices_.contains(id);}
    bool containsVertex(std::uint64_t id)const{return contains(id);}
    // Adjacencies are created exclusively through addEdge; input must be isolated.
    bool addVertex(const QuantumVertex& vertex){
        if(!vertex.edges.empty()||!finite(vertex.position)||!finite(vertex.previousPosition)||!finite(vertex.velocity)||!finite(vertex.acceleration)||!finite(vertex.force)||!std::isfinite(vertex.mass)||vertex.mass<=0||!std::isfinite(vertex.volume)||vertex.volume<=0)
            throw std::invalid_argument("Invalid or non-isolated vertex");
        return vertices_.insert(vertex.id,vertex);
    }
    const QuantumVertex* findVertex(std::uint64_t id)const{return vertices_.find(id);}
    QuantumVertex* findVertex(std::uint64_t id){return vertices_.find(id);}
    bool hasEdge(std::uint64_t a,std::uint64_t b)const{auto v=findVertex(a);if(v)for(const auto& e:v->edges)if(e.targetId==b)return true;return false;}
    bool addEdge(std::uint64_t a,std::uint64_t b,double spin=0.5,double restLength=90,double stiffness=2){
        if(!std::isfinite(spin)||spin<=0||std::abs(spin*2-std::round(spin*2))>1e-9||!std::isfinite(restLength)||restLength<=0||!std::isfinite(stiffness)||stiffness<0)throw std::invalid_argument("Invalid edge parameters");
        auto u=findVertex(a);auto v=findVertex(b);if(!u||!v||a==b||hasEdge(a,b))return false;
        u->edges.pushBack({nextEdge_,b,spin,restLength,stiffness});
        try{v->edges.pushBack({nextEdge_,a,spin,restLength,stiffness});}catch(...){u->edges.removeIf([&](const auto& e){return e.id==nextEdge_;});throw;}
        ++nextEdge_;++edges_;return true;
    }
    bool removeEdge(std::uint64_t a,std::uint64_t b){auto u=findVertex(a);auto v=findVertex(b);if(!u||!v)return false;bool removed=u->edges.removeIf([&](const auto& e){return e.targetId==b;});if(removed){v->edges.removeIf([&](const auto& e){return e.targetId==a;});--edges_;}return removed;}
    bool removeVertex(std::uint64_t id){if(!contains(id))return false;auto adjacent=neighbors(id);for(auto other:adjacent)removeEdge(id,other);return vertices_.remove(id);}
    std::size_t degree(std::uint64_t id)const{auto v=findVertex(id);if(!v)throw std::out_of_range("Unknown vertex");return v->edges.size();}
    std::vector<std::uint64_t> neighbors(std::uint64_t id)const{auto v=findVertex(id);if(!v)throw std::out_of_range("Unknown vertex");std::vector<std::uint64_t> result;for(const auto& e:v->edges)result.push_back(e.targetId);return result;}
    std::vector<std::uint64_t> ids()const{std::vector<std::uint64_t> result;vertices_.forEach([&](auto id,const auto&){result.push_back(id);});std::sort(result.begin(),result.end());return result;}
    std::size_t size()const{return vertices_.size();}std::size_t edgeCount()const{return edges_;}
    std::size_t vertexCount()const{return size();}
    std::size_t numberOfEdges()const{return edgeCount();}
    template<class F> void forEachVertex(F f){vertices_.forEach([&](auto,auto& v){f(v);});}
    template<class F> void forEachVertex(F f)const{vertices_.forEach([&](auto,const auto& v){f(v);});}
    void rehash(std::size_t n){vertices_.rehash(n);}double loadFactor()const{return vertices_.loadFactor();}
    void clear(){vertices_.clear();edges_=0;nextEdge_=1;}
    QuantumGraph()=default;QuantumGraph(const QuantumGraph&)=default;QuantumGraph& operator=(const QuantumGraph&)=default;
    QuantumGraph(QuantumGraph&& o):vertices_(std::move(o.vertices_)),nextEdge_(o.nextEdge_),edges_(o.edges_){o.nextEdge_=1;o.edges_=0;}
    QuantumGraph& operator=(QuantumGraph&& o){if(this!=&o){vertices_=std::move(o.vertices_);nextEdge_=o.nextEdge_;edges_=o.edges_;o.nextEdge_=1;o.edges_=0;}return *this;}
};
}
