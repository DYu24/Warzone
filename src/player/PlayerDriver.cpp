#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"

int main()
{
    // Setup
    vector<Continent*> continents;
    unordered_map<Territory*, vector<Territory*>> adjacencyList;

    Territory* t1 = new Territory("Territory1");
    Territory* t2 = new Territory("Territory2");
    Territory* t3 = new Territory("Territory3");
    Territory* t4 = new Territory("Territory4");
    Territory* t5 = new Territory("Territory5");
    t1->addArmies(2);
    t2->addArmies(2);
    t3->addArmies(5);
    t4->addArmies(1);
    t5->addArmies(1);

    adjacencyList[t2].push_back(t3);
    adjacencyList[t2].push_back(t4);
    adjacencyList[t5].push_back(t4);

    
    Map* map = GameEngine::getMap();
    map->setAdjacencyList(adjacencyList);
    map->setContinents(continents);

    GameEngine::getDeck()->generateCards(5);
    Player p1 = Player("Player 1");

    p1.addOwnedTerritory(t1);
    p1.addOwnedTerritory(t2);
    p1.addOwnedTerritory(t5);
    p1.drawCardFromDeck();
    p1.addReinforcements(20);

    // Show the initial Player object
    cout << p1 << endl;

    // Show the list of territories to defend
    cout << "\n-----Calling Player.toDefend(): -----" << endl;
    vector<Territory*> toDefend = p1.toDefend();

    cout << "\nTerritories to defend: \n" << endl;
    for (auto const &territory : toDefend)
    {
        cout << "- " << *territory << endl;
    }

    // Show the list of territories to attack
    cout << "\n-----Calling Player.toAttack(): -----" << endl;
    vector<Territory*> toAttack = p1.toAttack();

    cout << "\nTerritories to attack: \n" << endl;
    for (auto territory : toAttack)
    {
        cout << "- " << *territory << endl;
    }

    cout << "\n-----Calling Player.issueOrder(): -----" << endl;
    p1.issueOrder();
    cout << p1 << endl;

    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    
    return 0;
}