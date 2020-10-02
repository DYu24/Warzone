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
 Implementation for OrdersList class
===================================
 */

OrdersList::OrdersList() : orders_(make_unique<vector<unique_ptr<Order>>>()) {}

OrdersList::OrdersList(const OrdersList &orders)
{
    setOrders(*orders.orders_);
}

const OrdersList &OrdersList::operator=(const OrdersList &orders)
{
    setOrders(*orders.orders_);
    return *this;
}

ostream &operator<<(ostream &output, const OrdersList &orders)
{
    output << "[Orders List] Size=" << orders.orders_->size();
    return output;
}

vector<unique_ptr<Order>> &OrdersList::getOrders()
{
    return *orders_;
}

void OrdersList::setOrders(vector<unique_ptr<Order>> &orders)
{
    orders_ = make_unique<vector<unique_ptr<Order>>>();
    for (auto const &orderPointer : orders)
    {
        orders_->push_back(orderPointer->clone());
    }
}

void OrdersList::addOrder(unique_ptr<Order> order)
{
    orders_->push_back(move(order));
}

void OrdersList::moveOrder(int source, int destination)
{
    bool sourceInRange = source >= 0 && source < orders_->size();
    bool destinationInRange = destination >= 0 && destination < orders_->size();

    if (sourceInRange && destinationInRange)
    {
        auto orderPosition = next(orders_->begin(), source);
        auto targetPosition = next(orders_->begin(), destination);

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

void OrdersList::deleteOrder(int target)
{
    orders_->erase(orders_->begin() + target);
}

/* 
===================================
 Implementation for DeployOrder class
===================================
 */

DeployOrder::DeployOrder()
    : numberOfArmies_(make_unique<int>(0)), destination_(make_shared<Territory>()) {}

DeployOrder::DeployOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> destination)
    : numberOfArmies_(move(numberOfArmies)), destination_(destination) {}

DeployOrder::DeployOrder(const DeployOrder &order) : destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    destination_ = order.destination_;
    return *this;
}

unique_ptr<Order> DeployOrder::clone() const
{
    return make_unique<DeployOrder>(*this);
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
    output << "[DeployOrder] " << *numberOfArmies_ << " armies to " << destination_->getName();
    return output;
}

/* 
===================================
 Implementation for AdvanceOrder class
===================================
 */

AdvanceOrder::AdvanceOrder()
    : numberOfArmies_(make_unique<int>(0)), source_(make_shared<Territory>()), destination_(make_shared<Territory>()) {}

AdvanceOrder::AdvanceOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination)
    : numberOfArmies_(move(numberOfArmies)), source_(source), destination_(destination) {}

AdvanceOrder::AdvanceOrder(const AdvanceOrder &order) : source_(order.source_), destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

unique_ptr<Order> AdvanceOrder::clone() const
{
    return make_unique<AdvanceOrder>(*this);
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
    output << "[AdvanceOrder] " << *numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    return output;
}

/* 
===================================
 Implementation for BombOrder class
===================================
 */

BombOrder::BombOrder() : target_(make_shared<Territory>()) {}

BombOrder::BombOrder(shared_ptr<Territory> target) : target_(target) {}

BombOrder::BombOrder(const BombOrder &order) : target_(order.target_) {}

const BombOrder &BombOrder::operator=(const BombOrder &order)
{
    target_ = order.target_;
    return *this;
}

unique_ptr<Order> BombOrder::clone() const
{
    return make_unique<BombOrder>(*this);
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
    output << "[BombOrder] Target: " << target_->getName();
    return output;
}

/* 
===================================
 Implementation for BlockadeOrder class
===================================
 */

BlockadeOrder::BlockadeOrder() : territory_(make_shared<Territory>()) {}

BlockadeOrder::BlockadeOrder(shared_ptr<Territory> territory) : territory_(territory) {}

BlockadeOrder::BlockadeOrder(const BlockadeOrder &order) : territory_(order.territory_) {}

const BlockadeOrder &BlockadeOrder::operator=(const BlockadeOrder &order)
{
    territory_ = order.territory_;
    return *this;
}

unique_ptr<Order> BlockadeOrder::clone() const
{
    return make_unique<BlockadeOrder>(*this);
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
    output << "[BlockadeOrder] Territory: " << territory_->getName();
    return output;
}

/* 
===================================
 Implementation for AirliftOrder class
===================================
 */

AirliftOrder::AirliftOrder()
    : numberOfArmies_(make_unique<int>(0)), source_(make_shared<Territory>()), destination_(make_shared<Territory>()) {}

AirliftOrder::AirliftOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination)
    : numberOfArmies_(move(numberOfArmies)), source_(source), destination_(destination) {}

AirliftOrder::AirliftOrder(const AirliftOrder &order) : source_(order.source_), destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

unique_ptr<Order> AirliftOrder::clone() const
{
    return make_unique<AirliftOrder>(*this);
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
    output << "[AirliftOrder] " << *numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
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

unique_ptr<Order> NegotiateOrder::clone() const
{
    return make_unique<NegotiateOrder>(*this);
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
    output << "[NegotiateOrder]";
    return output;
}
