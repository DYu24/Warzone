#include "../game_engine/GameEngine.h"
#include "MapLoader.h"

int main()
{
    // Load a Domination map
    std::cout << "----- LOADING DOMINATION MAP: CANADA -----" << std::endl;
    MapLoader* dominationMapLoader = new MapLoader();
    Map* domination = dominationMapLoader->loadMap("resources/canada.map");
    std::cout << *domination << "\n" << std::endl;

    // Load a Conquest map
    std::cout << "----- LOADING CONQUEST MAP: ASIA -----" << std::endl;
    MapLoader* conquestMapLoader = new ConquestFileReaderAdapter();
    Map* conquest = conquestMapLoader->loadMap("resources/Asia.map");
    std::cout << *conquest << "\n" << std::endl;

    // Selecting from game menu
    GameEngine* gameEngine = new GameEngine();
    gameEngine->startGame();

    delete dominationMapLoader;
    delete conquestMapLoader;
    delete domination;
    delete conquest;
    delete gameEngine;

    GameEngine::resetGameEngine();

    return 0;
}