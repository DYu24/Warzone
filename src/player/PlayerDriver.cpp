#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <memory>

int main()
{
    // Setup
    Territory* t1 = new Territory("Territory1");
    Territory* t2 = new Territory("Territory2");
    Territory* t3 = new Territory("Territory3");
    Territory* t4 = new Territory("Territory4");

    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
    t2->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t2);
    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);

    Player p1 = Player("Player 1");
    p1.addOwnedTerritory(t1);
    p1.addOwnedTerritory(t2);
    p1.addCardToHand(new BombCard());

    // Show the Player object
    cout << p1 << endl;

    // Show the list of territories to defend
    vector<Territory*> toDefend = p1.toDefend();
    cout << "-----Calling Player.toDefend(): -----" << endl;
    for (auto const &territory : toDefend)
    {
        cout << *territory << endl;
    }

    cout << endl;

    // Show the list of territories to attack
    vector<Territory*> toAttack = p1.toAttack();
    cout << "-----Calling Player.toAttack(): -----" << endl;
    for (auto territory : toAttack)
    {
        cout << *territory << endl;
    }

    cout << endl;

    p1.issueOrder();

    cout << endl;
    cout << p1 << endl;
    
    return 0;
}