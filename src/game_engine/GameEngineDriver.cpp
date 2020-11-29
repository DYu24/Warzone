#include "GameEngine.h"

int main()
{
    GameEngine gameEngine;

    // ==============================
    // Part 1:
    // ==============================

    std::cout << "\n********** PART 1 **********" << std::endl;
    gameEngine.startGame();

    // Output the created deck
    std::cout << '\n' << *GameEngine::getDeck() << std::endl;

    // Output the created players
    for (const auto &player : GameEngine::getPlayers())
    {
        std::cout << *player << std::endl;
    }

    std::cout << std::endl;


    // ====================================================================================================================


    // ==============================
    // Part 2:
    // ==============================

    std::cout << "\n********** PART 2 **********" << std::endl;
    gameEngine.startupPhase();

    std::cout << std::endl;

    Map* map = GameEngine::getMap();
    std::vector<Territory*> allTerritoriesInMap = map->getTerritories();
    std::vector<Territory*> ownedTerritories;

    for (const auto &player : GameEngine::getPlayers())
    {
        std::cout << *player << std::endl;
        std::vector<Territory*> playerTerritories = player->getOwnedTerritories();
        for (int i = 0; i < playerTerritories.size(); i++)
        {
            std::cout << (i+1) << ". " << playerTerritories.at(i)->getName() << std::endl;
        }
        ownedTerritories.insert(ownedTerritories.end(), playerTerritories.begin(), playerTerritories.end());
        std::cout << std::endl;
    }

    std::cout << std::endl;

    // If every territory is owned by only one player, then the size of `allTerritoriesInMap` and `ownedTerritories` should be the same.
    // (i.e. there are no duplicates/two players do not own a common territory)
    std::cout << "Number of territories in the game map: " << allTerritoriesInMap.size() << std::endl;
    std::cout << "Number of territories owned by players: " << ownedTerritories.size() << std::endl;

    std::cout << std::endl;


    // ====================================================================================================================


    // ==============================
    // Part 3 & 4 & 5:
    // ==============================

    std::cout << "\n********** PART 3 & 4 & 5**********" << std::endl;
    gameEngine.mainGameLoop();

    GameEngine::resetGameEngine();

    return 0;
}