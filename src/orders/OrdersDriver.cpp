#include "../game_engine/GameEngine.h"
#include "Orders.h"

int main()
{
    // Setup
    Territory* t1 = new Territory("Quebec");
    Territory* t2 = new Territory("BC");
    Territory* t3 = new Territory("Ontario");

    Player* player = new Player("Bob");
    Player* enemy = new Player("Alice");
    player->addOwnedTerritory(t1);
    player->addOwnedTerritory(t2);
    enemy->addOwnedTerritory(t3);

    GameEngine::setPlayers({ player, enemy });

    OrdersList ordersList;
    ordersList.add(new DeployOrder(player, 5, t1));
    ordersList.add(new AdvanceOrder(player, 5, t1, t3));
    ordersList.add(new BombOrder(player, t3));
    ordersList.add(new AirliftOrder(player, 10, t1, t2));
    ordersList.add(new BlockadeOrder(player, t1));
    ordersList.add(new NegotiateOrder(player, enemy));



    // Show the OrderList
    cout << "===== " << "Original orders list: " << ordersList << " =====" << endl;
    for (const auto &order : ordersList.getOrders())
    {
        cout << *order << endl;
        cout << boolalpha << "Order is valid: " << order->validate() << endl;
        order->execute();
        cout << endl;
    }

    // Show the OrderList after moving an order from index 1 to index 4
    ordersList.move(1, 4);
    cout << "===== " << "Orders list after moving an order: " << ordersList << " =====" << endl;
    for (const auto &order : ordersList.getOrders())
    {
        cout << *order << endl;
    }

    // Show the OrderList after deleting an order at index 5
    ordersList.remove(5);
    cout << "\n===== " << "Orders list after deleting an order: " << ordersList << " =====" << endl;
    for (const auto &order : ordersList.getOrders())
    {
        cout << *order << endl;
    }

    delete t1;
    delete t2;
    delete t3;
    delete player;
    delete enemy;

    return 0;
}