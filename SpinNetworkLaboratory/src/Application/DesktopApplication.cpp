#include "Application/DesktopApplication.hpp"
#include "Graphics/GraphRenderer.hpp"
#include "Graphics/InputController.hpp"
#include <iostream>
#include <stdexcept>
namespace snl {
int runDesktop(const std::filesystem::path& executable,bool smokeTest){
    sf::Font font;
    bool loaded=false;
    const auto folder=std::filesystem::absolute(executable).parent_path();
    for(const auto& root:{std::filesystem::current_path(),folder,folder.parent_path(),folder.parent_path().parent_path()}){
        const auto path=root/"assets/fonts/DejaVuSans.ttf";
        if(std::filesystem::exists(path)&&font.loadFromFile(path.string())){loaded=true;break;}
    }
    if(!loaded)throw std::runtime_error("No se encontro assets/fonts/DejaVuSans.ttf. Ejecuta desde la carpeta del proyecto.");
    sf::RenderWindow window(sf::VideoMode(1200,760),"Spin Network Laboratory | Workshop 1 | C++ / SFML");
    window.setFramerateLimit(60);window.setKeyRepeatEnabled(false);
    SimulationController simulation;simulation.loadGrid(smokeTest?10:4,smokeTest?10:5);
    InputController input;input.fit(simulation.graph,window.getSize());
    GraphRenderer renderer;HUD hud;sf::Clock clock;unsigned frames=0;double fps=60;
    while(window.isOpen()){
        sf::Event event{};
        while(window.pollEvent(event))input.handle(event,window,simulation,hud);
        if(!window.isOpen())break;
        const double elapsed=clock.restart().asSeconds();
        simulation.update(elapsed);fps=elapsed>0?0.9*fps+0.1/elapsed:fps;
        window.clear(sf::Color(13,20,27));
        window.setView(input.view());renderer.draw(window,simulation,font);
        window.setView(sf::View(sf::FloatRect(0,0,float(window.getSize().x),float(window.getSize().y))));
        hud.draw(window,simulation,font,fps);window.display();++frames;
        if(smokeTest&&frames==120){
            std::filesystem::create_directories("docs/screenshots");
            sf::Texture texture;texture.create(window.getSize().x,window.getSize().y);texture.update(window);
            if(!texture.copyToImage().saveToFile("docs/screenshots/workshop1-sfml.png"))throw std::runtime_error("Screenshot failed");
            event.type=sf::Event::KeyPressed;event.key.code=sf::Keyboard::Space;input.handle(event,window,simulation,hud);
            if(!simulation.paused)throw std::runtime_error("Pause event failed");
            event.key.code=sf::Keyboard::Up;input.handle(event,window,simulation,hud);
            if(simulation.params.repulsion!=12500)throw std::runtime_error("Parameter event failed");
            simulation.graph.forEachVertex([](const QuantumVertex& v){if(!finite(v.position)||!finite(v.velocity))throw std::runtime_error("Non-finite physics");});
            event.type=sf::Event::Closed;input.handle(event,window,simulation,hud);
            if(window.isOpen())throw std::runtime_error("Close event failed");
            std::cout<<"PASS: SFML window, font, 100 dynamic nodes, render, pause, parameter event, screenshot and clean close.\n";
        }
    }
    return 0;
}
}
