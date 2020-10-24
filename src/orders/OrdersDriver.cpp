#include "Orders.h"
#include <memory>

int main()
{
    // Setup
    shared_ptr<Territory> territory = make_shared<Territory>("Quebec");
    shared_ptr<Territory> targetTerritory = make_shared<Territory>("Ontario");

    OrdersList ordersList;
    ordersList.add(make_unique<DeployOrder>(5, territory));
    ordersList.add(make_unique<AdvanceOrder>(5, territory, targetTerritory));
    ordersList.add(make_unique<BombOrder>(targetTerritory));
    ordersList.add(make_unique<BlockadeOrder>(territory));
    ordersList.add(make_unique<AirliftOrder>(10, territory, targetTerritory));
    ordersList.add(make_unique<NegotiateOrder>());

    // Show the OrderList
    cout << "------" << "Original orders list: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
        cout << boolalpha << "Order is valid: " << orderPointer->validate() << endl;
        orderPointer->execute();
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