#include "Orders.h"
#include <algorithm>
#include <iterator>

/* 
===================================
 Implementation for Order class
===================================
 */

// Validate and execute the Order. Invalid orders will have no effect.
void Order::execute()
{
    if (validate())
    {
        execute_();
    }
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Order &order)
{
    return order.print_(output);
}

/* 
===================================
 Implementation for OrdersList class
===================================
 */

// Constructor
OrdersList::OrdersList() : orders_(make_unique<vector<unique_ptr<Order>>>()) {}

// Copy constructor
OrdersList::OrdersList(const OrdersList &orders)
{
    setOrders(*orders.orders_);
}

// Assignment operator overloading
const OrdersList &OrdersList::operator=(const OrdersList &orders)
{
    setOrders(*orders.orders_);
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const OrdersList &orders)
{
    output << "[Orders List] Size=" << orders.orders_->size();
    return output;
}

// Getter and setter
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

// Add an order to the OrderList.
void OrdersList::addOrder(unique_ptr<Order> order)
{
    orders_->push_back(move(order));
}

// Move an order within the OrderList from `source` position to `destination` position.
void OrdersList::moveOrder(int source, int destination)
{
    bool sourceInRange = source >= 0 && source < orders_->size();
    bool destinationInRange = destination >= 0 && destination < orders_->size();

    if (sourceInRange && destinationInRange)
    {
        auto orderPosition = next(orders_->begin(), source);
        auto destinationPosition = next(orders_->begin(), destination);

        // If the order is before its destination, move it forwards
        if (destinationPosition > orderPosition)
        {
            while (orderPosition != destinationPosition)
            {
                swap(*orderPosition, *next(orderPosition, 1));
                orderPosition++;
            }
        }

        // If the order is ahead of its destination, move it backwards
        if (destinationPosition < orderPosition)
        {
            while (orderPosition != destinationPosition)
            {
                swap(*orderPosition, *prev(orderPosition, 1));
                orderPosition--;
            }
        }
    }
}

// Delete an order from the OrderList specified by the `target` index.
void OrdersList::deleteOrder(int target)
{
    orders_->erase(orders_->begin() + target);
}

/* 
===================================
 Implementation for DeployOrder class
===================================
 */

// Default constructor
DeployOrder::DeployOrder() : numberOfArmies_(make_unique<int>(0)), destination_(make_shared<Territory>()) {}

// Constructor
DeployOrder::DeployOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> destination) 
    : numberOfArmies_(move(numberOfArmies)), destination_(destination) {}

// Copy constructor
DeployOrder::DeployOrder(const DeployOrder &order) : destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

// Assignment operator overloading
const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    destination_ = order.destination_;
    return *this;
}

// Return a pointer to a new instance of DeployOrder.
unique_ptr<Order> DeployOrder::clone() const
{
    return make_unique<DeployOrder>(*this);
}

// Checks that the DeployOrder is valid.
bool DeployOrder::validate()
{
    return true;
}

// Executes the DeployOrder.
void DeployOrder::execute_()
{
    cout << "Deploy executed!" << endl;
}

// Stream insertion operator overloading
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

// Default constructor
AdvanceOrder::AdvanceOrder() 
    : numberOfArmies_(make_unique<int>(0)), source_(make_shared<Territory>()), destination_(make_shared<Territory>()) {}

// Constructor
AdvanceOrder::AdvanceOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination)
    : numberOfArmies_(move(numberOfArmies)), source_(source), destination_(destination) {}

// Copy constructor
AdvanceOrder::AdvanceOrder(const AdvanceOrder &order) : source_(order.source_), destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

// Assignment operator overloading
const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

// Return a pointer to a new instace of AdvanceOrder
unique_ptr<Order> AdvanceOrder::clone() const
{
    return make_unique<AdvanceOrder>(*this);
}

// Checks that the AdvanceOrder is valid.
bool AdvanceOrder::validate()
{
    return true;
}

// Executes the AdvanceOrder.
void AdvanceOrder::execute_()
{
    cout << "Advance executed!" << endl;
}

// Stream insertion operator overloading
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

// Default constructor
BombOrder::BombOrder() : target_(make_shared<Territory>()) {}

// Constructor
BombOrder::BombOrder(shared_ptr<Territory> target) : target_(target) {}

// Copy constructor
BombOrder::BombOrder(const BombOrder &order) : target_(order.target_) {}

// Assignment operator overloading
const BombOrder &BombOrder::operator=(const BombOrder &order)
{
    target_ = order.target_;
    return *this;
}

// Return a pointer to a new instance of BombOrder.
unique_ptr<Order> BombOrder::clone() const
{
    return make_unique<BombOrder>(*this);
}

// Checks that the BombOrder is valid.
bool BombOrder::validate()
{
    return true;
}

// Executes the BombOrder.
void BombOrder::execute_()
{
    cout << "Bomb executed!" << endl;
}

// Stream insertion operator overloading
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

// Default constructor
BlockadeOrder::BlockadeOrder() : territory_(make_shared<Territory>()) {}

// Constructor
BlockadeOrder::BlockadeOrder(shared_ptr<Territory> territory) : territory_(territory) {}

// Copy constructor
BlockadeOrder::BlockadeOrder(const BlockadeOrder &order) : territory_(order.territory_) {}

// Assignment operator overloading
const BlockadeOrder &BlockadeOrder::operator=(const BlockadeOrder &order)
{
    territory_ = order.territory_;
    return *this;
}

// Return a pointer to a new instance of BlockadeOrder.
unique_ptr<Order> BlockadeOrder::clone() const
{
    return make_unique<BlockadeOrder>(*this);
}

// Checks that the BlockadeOrder is valid.
bool BlockadeOrder::validate()
{
    return true;
}

// Executes the BlockadeOrder.
void BlockadeOrder::execute_()
{
    cout << "Blockade executed!" << endl;
}

// Stream insertion operator overloading
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

// Default constructor
AirliftOrder::AirliftOrder()
    : numberOfArmies_(make_unique<int>(0)), source_(make_shared<Territory>()), destination_(make_shared<Territory>()) {}

// Constructor
AirliftOrder::AirliftOrder(unique_ptr<int> numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination)
    : numberOfArmies_(move(numberOfArmies)), source_(source), destination_(destination) {}

// Copy constructor
AirliftOrder::AirliftOrder(const AirliftOrder &order) : source_(order.source_), destination_(order.destination_)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
}

// Assignment operator overloading
const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    numberOfArmies_ = make_unique<int>(*order.numberOfArmies_);
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

// Return a pointer to a new instance of AirliftOrder.
unique_ptr<Order> AirliftOrder::clone() const
{
    return make_unique<AirliftOrder>(*this);
}

// Checks that the AirliftOrder is valid.
bool AirliftOrder::validate()
{
    return true;
}

// Executes the AirliftOrder.
void AirliftOrder::execute_()
{
    cout << "Airlift executed!" << endl;
}

// Stream insertion operator overloading
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

// Return a pointer to a new instance of NegotiateOrder.
unique_ptr<Order> NegotiateOrder::clone() const
{
    return make_unique<NegotiateOrder>(*this);
}

// Checks that the NegotiateOrder is valid.
bool NegotiateOrder::validate()
{
    return true;
}

// Executes the NegotiateOrder.
void NegotiateOrder::execute_()
{
    cout << "Negotiate executed!" << endl;
}

// Stream insertion operator overloading
ostream &NegotiateOrder::print_(ostream &output) const
{
    output << "[NegotiateOrder]";
    return output;
}
