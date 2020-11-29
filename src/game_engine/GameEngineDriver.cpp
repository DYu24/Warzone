#include "GameEngine.h"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
    GameEngine gameEngine;

    // ==============================
    // Part 1:
    // ==============================

    cout << "\n********** PART 1 **********" << endl;
    gameEngine.startGame();

    // Output the created deck
    cout << '\n' << *GameEngine::getDeck() << endl;

    // Output the created players
    for (const auto &player : GameEngine::getPlayers())
    {
        cout << *player << endl;
    }

    cout << endl;


    // ====================================================================================================================


    // ==============================
    // Part 2:
    // ==============================

    cout << "\n********** PART 2 **********" << endl;
    gameEngine.startupPhase();

    cout << endl;

    Map* map = GameEngine::getMap();
    vector<Territory*> allTerritoriesInMap = map->getTerritories();
    vector<Territory*> ownedTerritories;

    for (const auto &player : GameEngine::getPlayers())
    {
        cout << *player << endl;
        vector<Territory*> playerTerritories = player->getOwnedTerritories();
        for (int i = 0; i < playerTerritories.size(); i++)
        {
            cout << (i+1) << ". " << playerTerritories.at(i)->getName() << endl;
        }
        ownedTerritories.insert(ownedTerritories.end(), playerTerritories.begin(), playerTerritories.end());
        cout << endl;
    }

    cout << endl;

    // If every territory is owned by only one player, then the size of `allTerritoriesInMap` and `ownedTerritories` should be the same.
    // (i.e. there are no duplicates/two players do not own a common territory)
    cout << "Number of territories in the game map: " << allTerritoriesInMap.size() << endl;
    cout << "Number of territories owned by players: " << ownedTerritories.size() << endl;

    cout << endl;


    // ====================================================================================================================


    // ==============================
    // Part 3 & 4 & 5:
    // ==============================

    cout << "\n********** PART 3 & 4 & 5**********" << endl;
    gameEngine.mainGameLoop();

    GameEngine::resetGameEngine();

    return 0;
}