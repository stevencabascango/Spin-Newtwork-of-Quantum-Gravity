#include "Application/DesktopApplication.hpp"
#include <iostream>
#include <string>
int main(int argc,char** argv){
    try{return snl::runDesktop(argv[0],argc>1&&std::string(argv[1])=="--smoke-test");}
    catch(const std::exception& error){std::cerr<<"Error: "<<error.what()<<'\n';return 1;}
}
