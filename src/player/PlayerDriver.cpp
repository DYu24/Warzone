#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"

int main()
{
    // Setup
    Territory* t1 = new Territory("Territory1");
    Territory* t2 = new Territory("Territory2");
    Territory* t3 = new Territory("Territory3");
    Territory* t4 = new Territory("Territory4");

    vector<Continent*> continents;
    unordered_map<Territory*, vector<Territory*>> adjacencyList;
    adjacencyList[t1].push_back(t3);
    adjacencyList[t2].push_back(t4);
    
    Map* map = new Map(continents, adjacencyList);
    Player p1 = Player("Player 1", map);
    p1.addOwnedTerritory(t1);
    p1.addOwnedTerritory(t2);
    p1.addCardToHand(new BombCard());

    // Show the Player object
    cout << p1 << endl;

    // Show the list of territories to defend
    cout << "\n-----Calling Player.toDefend(): -----" << endl;
    vector<Territory*> toDefend = p1.toDefend();

    cout << "\nTerritories to defend: \n" << endl;
    for (auto const &territory : toDefend)
    {
        cout << "- " << *territory << endl;
    }

    cout << endl;

    // Show the list of territories to attack
    cout << "\n-----Calling Player.toAttack(): -----" << endl;
    vector<Territory*> toAttack = p1.toAttack();

    cout << "\nTerritories to attack: \n" << endl;
    for (auto territory : toAttack)
    {
        cout << "- " << *territory << endl;
    }

    cout << endl;

    p1.issueOrder();

    cout << endl;
    cout << p1 << endl;

    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete map;
    
    return 0;
}