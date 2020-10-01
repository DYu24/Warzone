#include "Orders.h"
#include <memory>

int main()
{
    shared_ptr<Territory> territory = make_shared<Territory>(new string("Quebec"));
    shared_ptr<Territory> targetTerritory = make_shared<Territory>(new string("Ontario"));

    OrdersList ordersList;
    ordersList.addOrder(make_unique<DeployOrder>(make_unique<int>(5), territory));
    ordersList.addOrder(make_unique<AdvanceOrder>(make_unique<int>(5), territory, targetTerritory));
    ordersList.addOrder(make_unique<BombOrder>(targetTerritory));
    ordersList.addOrder(make_unique<BlockadeOrder>(territory));
    ordersList.addOrder(make_unique<AirliftOrder>(make_unique<int>(10), territory, targetTerritory));
    ordersList.addOrder(make_unique<NegotiateOrder>());

    cout << "------"
         << "Original orders list: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
        cout << boolalpha << "Order is valid: " << o->validate() << endl;
        o->execute();
        cout << endl;
    }

    ordersList.moveOrder(1, 4);
    cout << "------"
         << "Orders list after moving an order: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
    }

    ordersList.deleteOrder(5);
    cout << "\n------"
         << "Orders list after deleting an order: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
    }
}