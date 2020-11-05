#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <memory>

int main()
{
    // Setup
    shared_ptr<Territory> t1 = make_shared<Territory>("Territory1");
    shared_ptr<Territory> t2 = make_shared<Territory>("Territory2");
    shared_ptr<Territory> t3 = make_shared<Territory>("Territory3");
    shared_ptr<Territory> t4 = make_shared<Territory>("Territory4");

    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
    t2->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t2);
    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);

    Player p1 = Player("Player 1");
    p1.addOwnedTerritory(t1.get());
    p1.addOwnedTerritory(t2.get());
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