#include "Orders.h"
#include <memory>

int main()
{
    // Setup
    shared_ptr<Territory> territory = make_shared<Territory>(Territory(make_unique<string>("Quebec")));
    shared_ptr<Territory> targetTerritory = make_shared<Territory>(Territory(make_unique<string>("Ontario")));

    OrdersList ordersList;
    ordersList.addOrder(make_unique<DeployOrder>(make_unique<int>(5), territory));
    ordersList.addOrder(make_unique<AdvanceOrder>(make_unique<int>(5), territory, targetTerritory));
    ordersList.addOrder(make_unique<BombOrder>(targetTerritory));
    ordersList.addOrder(make_unique<BlockadeOrder>(territory));
    ordersList.addOrder(make_unique<AirliftOrder>(make_unique<int>(10), territory, targetTerritory));
    ordersList.addOrder(make_unique<NegotiateOrder>());

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
    ordersList.moveOrder(1, 4);
    cout << "------" << "Orders list after moving an order: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
    }

    // Show the OrderList after deleting an order at index 5
    ordersList.deleteOrder(5);
    cout << "\n------" << "Orders list after deleting an order: " << ordersList << "------" << endl;
    for (auto const &orderPointer : ordersList.getOrders())
    {
        cout << *orderPointer << endl;
    }

    return 0;
}