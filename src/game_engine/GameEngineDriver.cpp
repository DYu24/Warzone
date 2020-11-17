#include "GameEngine.h"
#include <iostream>

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

    Map map = *GameEngine::getMap();
    vector<Territory*> allTerritoriesInMap = map.getTerritories();
    vector<Territory*> ownedTerritories;

    for (const auto &player : GameEngine::getPlayers())
    {
        // Print out the player to show that they now have reinforcements assigned to them
        cout << *player << endl;
        vector<Territory*> playerTerritories = player->getOwnedTerritories();
        ownedTerritories.insert(ownedTerritories.end(), playerTerritories.begin(), playerTerritories.end());
    }

    cout << endl;

    // If every territory is owned by only one player, then the size of `allTerritoriesInMap` and `ownedTerritories` should be the same.
    // (i.e. there are no duplicates/two players do not own a common territory)
    cout << "Number of territories in the game map: " << allTerritoriesInMap.size() << endl;
    cout << "Number of territories owned by players: " << ownedTerritories.size() << endl;

    cout << endl;


    // ====================================================================================================================


    // ==============================
    // Part 3 & 4:
    // ==============================

    cout << "\n********** PART 3 & 4 **********" << endl;
    gameEngine.mainGameLoop();

    return 0;
}