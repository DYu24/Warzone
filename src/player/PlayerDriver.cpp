#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"

int main()
{
    // Setup
    Territory* t1 = new Territory("Territory1");
    Territory* t2 = new Territory("Territory2");
    Territory* t3 = new Territory("Territory3");
    Territory* t4 = new Territory("Territory4");
    Territory* t5 = new Territory("Territory5");
    t1->addArmies(10);
    t2->addArmies(2);
    t3->addArmies(5);
    t4->addArmies(1);
    t5->addArmies(1);

    std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList{
        {t2, {t3, t4}},
        {t5, {t4}}
    };

    Continent* c1 = new Continent("Continent1", 3);
    c1->setTerritories({ t1, t2, t3, t4, t5 });
    std::vector<Continent*> continents{ c1 };
    
    Map* map = new Map(continents, adjacencyList);
    GameEngine::setMap(map);

    GameEngine::getDeck()->generateCards(5);
    Player p1 = Player("Player 1", new AggressivePlayerStrategy());

    p1.addOwnedTerritory(t1);
    p1.addOwnedTerritory(t2);
    p1.addOwnedTerritory(t5);
    p1.drawCardFromDeck();
    p1.addReinforcements(20);



    // Show the initial Player object
    std::cout << p1 << std::endl;

    // Show the list of territories to defend
    std::cout << "\n-----Calling Player.toDefend(): -----" << std::endl;
    std::vector<Territory*> toDefend = p1.toDefend();

    std::cout << "\nTerritories to defend: \n" << std::endl;
    for (const auto &territory : toDefend)
    {
        std::cout << "- " << *territory << std::endl;
    }

    // Show the list of territories to attack
    std::cout << "\n-----Calling Player.toAttack(): -----" << std::endl;
    std::vector<Territory*> toAttack = p1.toAttack();

    std::cout << "\nTerritories to attack: \n" << std::endl;
    for (const auto &territory : toAttack)
    {
        std::cout << "- " << *territory << std::endl;
    }

    std::cout << "\n-----Calling Player.issueOrder(): -----" << std::endl;
    p1.issueOrder();
    std::cout << p1 << std::endl;

    GameEngine::resetGameEngine();
    
    return 0;
}