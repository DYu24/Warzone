#include "Orders.h"
#include <algorithm>
#include <iterator>

/* 
===================================
 Implementation for Order class
===================================
 */

void Order::execute()
{
    if (validate())
    {
        execute_();
    }
}

ostream &operator<<(ostream &output, const Order &order)
{
    return order.print_(output);
}

/* 
===================================
 Implementation for Order class
===================================
 */

OrdersList::OrdersList() : orders_(make_shared<vector<shared_ptr<Order>>>()) {}

OrdersList::OrdersList(const OrdersList &orders) : orders_(orders.orders_) {}

const OrdersList &OrdersList::operator=(const OrdersList &orders)
{
    orders_ = orders.orders_;
    return *this;
}

ostream &operator<<(ostream &output, const OrdersList &orders)
{
    output << "[Orders List] Size=" << orders.orders_->size();
    return output;
}

vector<shared_ptr<Order>> OrdersList::getOrders()
{
    return *orders_;
}

void OrdersList::setOrders(vector<shared_ptr<Order>> &orders)
{
    *orders_ = orders;
}

void OrdersList::addOrder(shared_ptr<Order> order)
{
    orders_->push_back(order);
}

void OrdersList::moveOrder(shared_ptr<Order> order, int target)
{
    if (target >= 0 && target < orders_->size())
    {
        auto orderPosition = find(orders_->begin(), orders_->end(), order);
        auto targetPosition = next(orders_->begin(), target);

        if (targetPosition > orderPosition)
        {
            while (orderPosition != targetPosition)
            {
                swap(*orderPosition, *next(orderPosition, 1));
                orderPosition++;
            }
        }

        if (targetPosition < orderPosition)
        {
            while (orderPosition != targetPosition)
            {
                swap(*orderPosition, *prev(orderPosition, 1));
                orderPosition--;
            }
        }
    }
}

void OrdersList::deleteOrder(shared_ptr<Order> order)
{
    orders_->erase(remove(orders_->begin(), orders_->end(), order), orders_->end());
}

/* 
===================================
 Implementation for DeployOrder class
===================================
 */

DeployOrder::DeployOrder() {}

DeployOrder::DeployOrder(const DeployOrder &order) {}

const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    return *this;
}

bool DeployOrder::validate()
{
    return true;
}

void DeployOrder::execute_()
{
    cout << "Deploy executed!" << endl;
}

ostream &DeployOrder::print_(ostream &output) const
{
    output << "This is a Deploy order.";
    return output;
}

/* 
===================================
 Implementation for AdvanceOrder class
===================================
 */

AdvanceOrder::AdvanceOrder() {}

AdvanceOrder::AdvanceOrder(const AdvanceOrder &order) {}

const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    return *this;
}

bool AdvanceOrder::validate()
{
    return true;
}

void AdvanceOrder::execute_()
{
    cout << "Advance executed!" << endl;
}

ostream &AdvanceOrder::print_(ostream &output) const
{
    output << "This is an Advance order.";
    return output;
}

/* 
===================================
 Implementation for BombOrder class
===================================
 */

BombOrder::BombOrder() {}

BombOrder::BombOrder(const BombOrder &order) {}

const BombOrder &BombOrder::operator=(const BombOrder &order)
{
    return *this;
}

bool BombOrder::validate()
{
    return true;
}

void BombOrder::execute_()
{
    cout << "Bomb executed!" << endl;
}

ostream &BombOrder::print_(ostream &output) const
{
    output << "This is a Bomb order.";
    return output;
}

/* 
===================================
 Implementation for BlockadeOrder class
===================================
 */

BlockadeOrder::BlockadeOrder() {}

BlockadeOrder::BlockadeOrder(const BlockadeOrder &order) {}

const BlockadeOrder &BlockadeOrder::operator=(const BlockadeOrder &order)
{
    return *this;
}

bool BlockadeOrder::validate()
{
    return true;
}

void BlockadeOrder::execute_()
{
    cout << "Blockade executed!" << endl;
}

ostream &BlockadeOrder::print_(ostream &output) const
{
    output << "This is a Blockade order.";
    return output;
}

/* 
===================================
 Implementation for AirliftOrder class
===================================
 */

AirliftOrder::AirliftOrder() {}

AirliftOrder::AirliftOrder(const AirliftOrder &order) {}

const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    return *this;
}

bool AirliftOrder::validate()
{
    return true;
}

void AirliftOrder::execute_()
{
    cout << "Airlift executed!" << endl;
}

ostream &AirliftOrder::print_(ostream &output) const
{
    output << "This is an Airlift order.";
    return output;
}

/* 
===================================
 Implementation for NegotiateOrder class
===================================
 */

NegotiateOrder::NegotiateOrder() {}

NegotiateOrder::NegotiateOrder(const NegotiateOrder &order) {}

const NegotiateOrder &NegotiateOrder::operator=(const NegotiateOrder &order)
{
    return *this;
}

bool NegotiateOrder::validate()
{
    return true;
}

void NegotiateOrder::execute_()
{
    cout << "Negotiate executed!" << endl;
}

ostream &NegotiateOrder::print_(ostream &output) const
{
    output << "This is a Negotiate order.";
    return output;
}
