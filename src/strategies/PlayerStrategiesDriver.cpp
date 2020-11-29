#include "PlayerStrategies.h"
#include "../game_engine/GameEngine.h"
#include "../map_loader/MapLoader.h"

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
    std::cout << std::endl;
    for (const auto &player : gameEngine.getCurrentPlayers())
    {
        std::cout << *player << std::endl;
    }

    // ======================================================================================================

    // Round 1
    std::cout << "\n============================ Simulated game: Round 1 ============================" << std::endl;
    std::cout << "\n~~~ Reinforcement phase ~~~\n" << std::endl;
    gameEngine.reinforcementPhase();

    std::cout << "\n~~~ Issue orders phase ~~~\n" << std::endl;
    gameEngine.issueOrdersPhase();

    std::cout << "\n~~~ Execute orders phase ~~~\n" << std::endl;
    gameEngine.executeOrdersPhase();



    // ======================================================================================================

    // Change the strategy for one of the players
    player4->setStrategy(new HumanPlayerStrategy());

    // Round 2
    std::cout << "\n============================ Simulated game: Round 2 ============================" << std::endl;
    std::cout << "\n~~~ Reinforcement phase ~~~\n" << std::endl;
    gameEngine.reinforcementPhase();

    std::cout << "\n~~~ Issue orders phase ~~~\n" << std::endl;
    gameEngine.issueOrdersPhase();

    std::cout << "\n~~~ Execute orders phase ~~~\n" << std::endl;
    gameEngine.executeOrdersPhase();


    // Clean up dynamically allocated memory
    GameEngine::resetGameEngine();

    return 0;
}
