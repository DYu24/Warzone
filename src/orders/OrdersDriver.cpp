#include "Orders.h"

int main()
{
    // Setup
    Territory* territory = new Territory("Quebec");
    Territory* targetTerritory = new Territory("Ontario");

    Player* player = new Player();
    player->addOwnedTerritory(territory);

    OrdersList ordersList;
    ordersList.add(new DeployOrder(5, territory));
    ordersList.add(new AdvanceOrder(5, territory, targetTerritory));
    ordersList.add(new BombOrder(targetTerritory));
    ordersList.add(new BlockadeOrder(territory));
    ordersList.add(new AirliftOrder(10, territory, targetTerritory));
    ordersList.add(new NegotiateOrder());

    // Show the OrderList
    cout << "------" << "Original orders list: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
        cout << boolalpha << "Order is valid: " << orderPointer->validate(player) << endl;
        orderPointer->execute(player);
        cout << endl;
    }

    // Show the OrderList after moving an order from index 1 to index 4
    ordersList.move(1, 4);
    cout << "------" << "Orders list after moving an order: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
    }

    // Show the OrderList after deleting an order at index 5
    ordersList.remove(5);
    cout << "\n------" << "Orders list after deleting an order: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
    }

    return 0;
}