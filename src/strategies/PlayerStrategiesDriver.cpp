#include "PlayerStrategies.h"
#include "../game_engine/GameEngine.h"
#include "../map_loader/MapLoader.h"
#include <iostream>
using namespace std;

int main()
{
    // Setup
    GameEngine gameEngine;
    MapLoader loader;

    Player* player1 = new Player("Player 1");
    Player* player2 = new Player("Player 2");
    Player* player3 = new Player("Player 3");
    Player* player4 = new Player("Player 4");

    player1->setStrategy(new HumanPlayerStrategy());
    player2->setStrategy(new AggressivePlayerStrategy());
    player3->setStrategy(new BenevolentPlayerStrategy());
    player4->setStrategy(new NeutralPlayerStrategy());

    gameEngine.setMap(loader.loadMap("resources/canada.map"));
    gameEngine.setPlayers({ player1, player2, player3, player4 });
    GameEngine::getDeck()->generateCards(20);

    gameEngine.startupPhase();

    // Display the players at the start of the game
    cout << endl;
    for (const auto &player : gameEngine.getCurrentPlayers())
    {
        cout << *player << endl;
    }

    // ======================================================================================================

    // Round 1
    cout << "\n============================ Simulated game: Round 1 ============================" << endl;
    cout << "\n~~~ Reinforcement phase ~~~\n" << endl;
    gameEngine.reinforcementPhase();

    cout << "\n~~~ Issue orders phase ~~~\n" << endl;
    gameEngine.issueOrdersPhase();

    cout << "\n~~~ Execute orders phase ~~~\n" << endl;
    gameEngine.executeOrdersPhase();



    // ======================================================================================================

    // Change the strategy for one of the players
    player4->setStrategy(new HumanPlayerStrategy());

    // Round 2
    cout << "\n============================ Simulated game: Round 2 ============================" << endl;
    cout << "\n~~~ Reinforcement phase ~~~\n" << endl;
    gameEngine.reinforcementPhase();

    cout << "\n~~~ Issue orders phase ~~~\n" << endl;
    gameEngine.issueOrdersPhase();

    cout << "\n~~~ Execute orders phase ~~~\n" << endl;
    gameEngine.executeOrdersPhase();


    // Clean up dynamically allocated memory
    GameEngine::resetGameEngine();

    return 0;
}
