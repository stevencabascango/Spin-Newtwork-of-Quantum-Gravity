#include "Core/GraphMetrics.hpp"
#include <iostream>
#include <sstream>
using namespace snl;
int main(){QuantumGraph g;std::cout<<"Spin Network Laboratory | Workshop 1 | C++17\n"
<<"Comandos: v ID | e A B | rv ID | re A B | vecinos ID | info | demo | clear | salir\n";
std::string line;while(std::cout<<"> "&&std::getline(std::cin,line)){std::istringstream in(line);std::string cmd;std::uint64_t a,b;in>>cmd;try{
if(cmd=="salir")break;
else if(cmd=="v"&&in>>a)std::cout<<(g.addVertex(a)?"Vertice agregado":"ID duplicado")<<'\n';
else if(cmd=="e"&&in>>a>>b)std::cout<<(g.addEdge(a,b)?"Arista agregada":"Operacion rechazada")<<'\n';
else if(cmd=="rv"&&in>>a)std::cout<<(g.removeVertex(a)?"Vertice eliminado":"No existe")<<'\n';
else if(cmd=="re"&&in>>a>>b)std::cout<<(g.removeEdge(a,b)?"Arista eliminada":"No existe")<<'\n';
else if(cmd=="vecinos"&&in>>a){for(auto id:g.neighbors(a))std::cout<<id<<' ';std::cout<<'\n';}
else if(cmd=="clear")g.clear();
else if(cmd=="demo"){g.clear();for(int i=1;i<=4;++i)g.addVertex(i);g.addEdge(1,2);g.addEdge(2,3);g.addEdge(3,1);g.addEdge(3,4);std::cout<<"Demo creada\n";}
else if(cmd=="info"){auto m=calculateMetrics(g);std::cout<<"V="<<m.vertices<<" E="<<m.edges<<" beta0="<<m.beta0<<" beta1="<<m.beta1<<" chi="<<m.euler<<" grado medio="<<m.averageDegree<<'\n';}
else std::cout<<"Comando invalido\n";
}catch(const std::exception& e){std::cout<<"Error: "<<e.what()<<'\n';}}
}
