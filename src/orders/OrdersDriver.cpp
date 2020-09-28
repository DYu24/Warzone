#include "Orders.h"
#include <memory>

int main()
{
    OrdersList ordersList;
    ordersList.addOrder(make_shared<DeployOrder>());
    ordersList.addOrder(make_shared<AdvanceOrder>());
    ordersList.addOrder(make_shared<BombOrder>());
    ordersList.addOrder(make_shared<BlockadeOrder>());
    ordersList.addOrder(make_shared<AirliftOrder>());
    ordersList.addOrder(make_shared<NegotiateOrder>());

    cout << "------" << "Original orders list: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
        cout << boolalpha << "Order is valid: " << o->validate() << endl;
        o->execute();
        cout << endl;
    }

    shared_ptr<Order> order = ordersList.getOrders().front();
    ordersList.moveOrder(order, 5);
    cout << "------" << "Orders list after moving an order: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
    }

    ordersList.deleteOrder(order);
    cout << "\n------" << "Orders list after deleting an order: " << ordersList << "------" << endl;
    for (auto const &o : ordersList.getOrders())
    {
        cout << *o << endl;
    }
}