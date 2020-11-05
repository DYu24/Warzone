#include "GameEngine.h"
#include <iostream>

int main()
{
    GameEngine gameEngine;

    // ==============================
    // Part 1:
    // ==============================
    cout << "========== PART 1 ==========" << endl;
    gameEngine.startGame();

    // Should output the same number of players as specified during user input
    for (auto const &player : gameEngine.getPlayers())
    {
        cout << *player << endl;
    }

    cout << endl;


    // ==============================
    // Part 2:
    // ==============================
    cout << "========== PART 2 ==========" << endl;
    gameEngine.startupPhase();

    Map map = gameEngine.getMap();

    vector<Territory*> allTerritoriesInMap = map.getTerritories();
    vector<Territory*> ownedTerritories;

    for (auto const &player : gameEngine.getPlayers())
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
    

    // ==============================
    // Part 3:
    // ==============================
    cout << "========== PART 3 ==========" << endl;
    gameEngine.mainGameLoop();

    return 0;
}