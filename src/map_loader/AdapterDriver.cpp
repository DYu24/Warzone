#include "../game_engine/GameEngine.h"
#include "MapLoader.h"
using std::cout;
using std::endl;

int main()
{
    // Load a Domination map
    cout << "----- LOADING DOMINATION MAP: CANADA -----" << endl;
    MapLoader* dominationMapLoader = new MapLoader();
    Map* domination = dominationMapLoader->loadMap("resources/canada.map");
    cout << *domination << "\n" << endl;

    // Load a Conquest map
    cout << "----- LOADING CONQUEST MAP: ASIA -----" << endl;
    MapLoader* conquestMapLoader = new ConquestFileReaderAdapter();
    Map* conquest = conquestMapLoader->loadMap("resources/Asia.map");
    cout << *conquest << "\n" << endl;

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