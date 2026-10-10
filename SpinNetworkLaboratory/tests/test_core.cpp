#include "Core/GraphMetrics.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#define CHECK(x) do {if(!(x))throw std::runtime_error("Failed: " #x);}while(false)
struct Collisions {std::size_t operator()(int)const{return 0;}};
int main(){using namespace snl;
CHECK(Fnv1a64{}(std::string("hello"))==0xa430d84680aabd0bull);
SinglyLinkedList<int> operations;operations.pushFront(2);operations.pushFront(1);operations.pushBack(3);
CHECK(operations.contains(2));CHECK(*operations.find(3)==3);CHECK(!operations.contains(4));CHECK(operations.remove(3));operations.pushBack(4);CHECK(operations.remove(1));CHECK(operations.remove(2));CHECK(operations.remove(4));CHECK(operations.empty());operations.pushFront(9);CHECK(*operations.begin()==9);
const auto& constList=operations;CHECK(*constList.find(9)==9);CHECK(!constList.find(8));
HashTable<std::uint64_t,QuantumVertex> thousands;for(std::uint64_t i=0;i<5000;++i){QuantumVertex vertex;vertex.id=i;CHECK(thousands.insert(i,vertex));}
CHECK(thousands.size()==5000);CHECK(thousands.capacity()>5000);CHECK(thousands.loadFactor()<=.75);
for(std::uint64_t i=0;i<5000;++i){CHECK(thousands.find(i)->id==i);}
CHECK(!thousands.insert(0,QuantumVertex{}));
thousands.rehash(16384);for(std::uint64_t i=0;i<5000;++i)CHECK(thousands.remove(i));CHECK(thousands.size()==0);thousands.clear();
SinglyLinkedList<int> list;for(int i=0;i<10000;++i)list.pushBack(i);auto copy=list;CHECK(list.removeIf([](int v){return v==0;}));CHECK(copy.size()==10000);auto moved=std::move(copy);CHECK(copy.empty());copy.pushBack(42);CHECK(moved.size()==10000);list.clear();CHECK(list.empty());
HashTable<int,int,Collisions> hash(1);for(int i=0;i<1000;++i)CHECK(hash.insert(i,i*2));CHECK(hash.bucketSize(0)==1000);CHECK(hash.loadFactor()<=0.75);CHECK(!hash.insert(2,999));hash.rehash(4096);for(int i=0;i<1000;++i)CHECK(*hash.find(i)==i*2);CHECK(hash.remove(500));CHECK(hash.remove(0));CHECK(hash.remove(999));CHECK(!hash.remove(999));auto hcopy=hash;hash.clear();CHECK(hcopy.size()==997);auto hmove=std::move(hcopy);CHECK(hcopy.size()==0);CHECK(hcopy.insert(4,8));CHECK(hmove.contains(4));
QuantumGraph g;for(std::uint64_t i=0;i<1000;++i)CHECK(g.addVertex(i));for(std::uint64_t i=0;i<1000;++i)for(int j=1;j<=5;++j)CHECK(g.addEdge(i,(i+j)%1000));CHECK(g.edgeCount()==5000);CHECK(g.degree(0)==10);CHECK(!g.addEdge(0,0));CHECK(!g.addEdge(0,1));CHECK(!g.addEdge(0,1001));auto metrics=calculateMetrics(g);CHECK(metrics.beta0==1);CHECK(metrics.beta1==4001);CHECK(metrics.euler==static_cast<std::int64_t>(metrics.beta0)-metrics.beta1);auto gcopy=g;CHECK(g.removeVertex(0));CHECK(g.edgeCount()==4990);CHECK(gcopy.contains(0));for(auto id:g.ids())CHECK(!g.hasEdge(id,0));CHECK(g.addVertex(0));CHECK(g.degree(0)==0);g.rehash(8192);CHECK(g.size()==1000);QuantumGraph gm=std::move(gcopy);CHECK(gcopy.size()==0);CHECK(gcopy.edgeCount()==0);CHECK(gcopy.addVertex(99));CHECK(gm.edgeCount()==5000);
QuantumGraph empty;CHECK(calculateMetrics(empty).beta0==0);empty.addVertex(1);empty.addVertex(2);CHECK(calculateMetrics(empty).beta0==2);empty.addEdge(1,2);CHECK(calculateMetrics(empty).beta1==0);
bool caught=false;try{empty.addVertex(3,{},0);}catch(const std::invalid_argument&){caught=true;}CHECK(caught);caught=false;try{empty.addEdge(1,2,0.7);}catch(const std::invalid_argument&){caught=true;}CHECK(caught);
std::cout<<"PASS: FNV, listas, 5000 vertices en hash, 1000 vertices en grafo, 5000 aristas, colisiones, rehash, borrado, copia, movimiento e invariantes.\n";
}
