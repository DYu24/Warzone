#include "Orders.h"
#include <algorithm>
#include <iterator>
#include <math.h>

namespace
{
    /*
     * Custom comparator to sort Orders by priority
     */
    bool compareOrders(Order* order1, Order* order2)
    {
        return order1->getPriority() < order2->getPriority();
    }

    /**
     * Helper function to check whether a territory can be attacked by a specific player.
     */
    bool canAttack(Player* attacker, Territory* target)
    {
        for (auto enemy : attacker->getDiplomaticRelations())
        {
            auto enemyTerritories = enemy->getOwnedTerritories();
            if (find(enemyTerritories.begin(), enemyTerritories.end(), target) != enemyTerritories.end())
            {
                return false;
            }
        }

        return true;
    }
}


/* 
===================================
 Implementation for Order class
===================================
 */

// Constructors
Order::Order(): priority_(4) {}

Order::Order(int priority) : priority_(priority) {}

Order::Order(const Order &order) : priority_(order.priority_) {}

// Assignment operator overloading
const Order &Order::operator=(const Order &order)
{
    priority_ = order.priority_;
    return *this;
}

// Validate and execute the Order. Invalid orders will have no effect.
void Order::execute(Player* owner)
{
    if (validate(owner))
    {
        execute_();
    }
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Order &order)
{
    return order.print_(output);
}

// Get order priority
int Order::getPriority()
{
    return priority_;
}


/* 
===================================
 Implementation for OrdersList class
===================================
 */

// Constructor
OrdersList::OrdersList() {}

// Copy constructor
OrdersList::OrdersList(const OrdersList &orders)
{
    for (auto order : orders.orders_)
    {
        orders_.push_back(order->clone());
    }
}

// Destructor
OrdersList::~OrdersList()
{
    for (auto order : orders_)
    {
        delete order;
    }
    orders_.clear();
}

// Assignment operator overloading
const OrdersList &OrdersList::operator=(const OrdersList &orders)
{
    setOrders(orders.orders_);
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const OrdersList &orders)
{
    output << "[Orders List] Size=" << orders.orders_.size();
    return output;
}

// Getter and setter
vector<Order*> OrdersList::getOrders()
{
    return orders_;
}

void OrdersList::setOrders(vector<Order*> orders)
{
    for (auto order : orders_)
    {
        delete order;
    }
    orders_.clear();

    for (auto order : orders)
    {
        orders_.push_back(order->clone());
    }
}

// Add an order to the OrderList.
void OrdersList::add(Order* order)
{
    orders_.push_back(order);
}

// Move an order within the OrderList from `source` position to `destination` position.
void OrdersList::move(int source, int destination)
{
    bool sourceInRange = source >= 0 && source < orders_.size();
    bool destinationInRange = destination >= 0 && destination < orders_.size();

    if (sourceInRange && destinationInRange)
    {
        auto orderPosition = next(orders_.begin(), source);
        auto destinationPosition = next(orders_.begin(), destination);

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
void OrdersList::remove(int target)
{
    orders_.erase(orders_.begin() + target);
}

// Pop the first order in the OrderList according to priority
Order* OrdersList::popTopOrder()
{
    Order* topOrder = peek();
    orders_.erase(orders_.begin());

    return topOrder;
}

// Get the first order in the OrderList according to priority without removing it
Order* OrdersList::peek()
{
    if (orders_.empty())
    {
        return NULL;
    }

    sort(orders_.begin(), orders_.end(), compareOrders);

    return orders_.at(0);
}


/* 
===================================
 Implementation for DeployOrder class
===================================
 */

// Default constructor
DeployOrder::DeployOrder() : Order(1), numberOfArmies_(0), destination_(NULL) {}

// Constructor
DeployOrder::DeployOrder(int numberOfArmies, Territory* destination) : Order(1), numberOfArmies_(numberOfArmies), destination_(destination) {}

// Copy constructor
DeployOrder::DeployOrder(const DeployOrder &order) : Order(order), numberOfArmies_(order.numberOfArmies_), destination_(order.destination_) {}

// Assignment operator overloading
const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    destination_ = order.destination_;
    return *this;
}

// Getters
int DeployOrder::getNumberOfArmies()
{
    return numberOfArmies_;
}

Territory DeployOrder::getDestination()
{
    return *destination_;
}

// Add a number of armies to deploy to the order
void DeployOrder::addArmies(int additional)
{
    numberOfArmies_ += additional;
}

// Return a pointer to a new instance of DeployOrder.
Order* DeployOrder::clone() const
{
    return new DeployOrder(*this);
}

// Checks that the DeployOrder is valid.
bool DeployOrder::validate(Player* owner)
{   
    if (destination_ == NULL)
    {
        return false;
    }

    auto currentPlayerTerritories = owner->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), destination_) != currentPlayerTerritories.end();
}

// Executes the DeployOrder.
void DeployOrder::execute_()
{
    destination_->addArmies(numberOfArmies_);
    destination_->setPendingIncomingArmies(0);
    cout << "Deployed " << numberOfArmies_ << " armies to " << destination_->getName() << "." << endl;
}

// Stream insertion operator overloading
ostream &DeployOrder::print_(ostream &output) const
{
    output << "[DeployOrder]";

    if (destination_ != NULL)
    {
        output << " " << numberOfArmies_ << " armies to " << destination_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType DeployOrder::getType()
{
    return DEPLOY;
}

/* 
===================================
 Implementation for AdvanceOrder class
===================================
 */

// Default constructor
AdvanceOrder::AdvanceOrder() : numberOfArmies_(0), offensive_(false), source_(NULL), destination_(NULL) {}

// Constructor
AdvanceOrder::AdvanceOrder(int numberOfArmies, Territory* source, Territory* destination, bool offensive)
    : numberOfArmies_(numberOfArmies), source_(source), destination_(destination), offensive_(offensive) {}

// Copy constructor
AdvanceOrder::AdvanceOrder(const AdvanceOrder &order)
    : Order(order), numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_), offensive_(order.offensive_) {}

// Assignment operator overloading
const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    source_ = order.source_;
    destination_ = order.destination_;
    offensive_ = order.offensive_;
    return *this;
}

// Return a pointer to a new instace of AdvanceOrder
Order* AdvanceOrder::clone() const
{
    return new AdvanceOrder(*this);
}

// Checks that the AdvanceOrder is valid.
bool AdvanceOrder::validate(Player* owner)
{
    if (source_ == NULL || destination_ == NULL)
    {
        return false;
    }

    auto currentPlayerTerritories = owner->getOwnedTerritories();

    bool validSourceTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), source_) != currentPlayerTerritories.end();
    return validSourceTerritory && canAttack(owner, destination_);
}

// Executes the AdvanceOrder.
void AdvanceOrder::execute_()
{
    if (offensive_)
    {
        source_->removeArmies(numberOfArmies_);

        int defendersKilled = round(numberOfArmies_ * 0.6);
        int attackersKilled = round(destination_->getNumberOfArmies() * 0.7);

        int survivingAttackers = max(numberOfArmies_ - attackersKilled, 0);
        int survivingDefenders = max(destination_->getNumberOfArmies() - defendersKilled, 0);
        destination_->removeArmies(defendersKilled);

        if (survivingDefenders > 0)
        {
            source_->addArmies(survivingAttackers);
            cout << "Failed attack on " << destination_->getName() << " with " << survivingDefenders << " enemy armies left standing.";

            if (survivingAttackers > 0)
            {
                cout << " Retreating " << survivingAttackers << " attacking armies back to" << source_->getName() << endl;
            }
            else
            {
                cout << endl;
            }
        }
        else
        {
            destination_->addArmies(survivingAttackers);
            cout << "Successful attack on " << destination_->getName() << ". " << survivingAttackers << " armies now occupy this territory." << endl;
        }
    }
    else
    {
        source_->removeArmies(numberOfArmies_);
        destination_->addArmies(numberOfArmies_);
        cout << "Advanced " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName() << "." << endl;
    }
}

// Stream insertion operator overloading
ostream &AdvanceOrder::print_(ostream &output) const
{
    output << "[AdvanceOrder]";

    if (source_ != NULL && destination_ != NULL)
    {
        output << " " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType AdvanceOrder::getType()
{
    return ADVANCE;
}


/* 
===================================
 Implementation for BombOrder class
===================================
 */

// Default constructor
BombOrder::BombOrder() : target_(NULL) {}

// Constructor
BombOrder::BombOrder(Territory* target) : target_(target) {}

// Copy constructor
BombOrder::BombOrder(const BombOrder &order) : Order(order), target_(order.target_) {}

// Assignment operator overloading
const BombOrder &BombOrder::operator=(const BombOrder &order)
{
    Order::operator=(order);
    target_ = order.target_;
    return *this;
}

// Return a pointer to a new instance of BombOrder.
Order* BombOrder::clone() const
{
    return new BombOrder(*this);
}

// Checks that the BombOrder is valid.
bool BombOrder::validate(Player* owner)
{
    if (target_ == NULL)
    {
        return false;
    }

    auto currentPlayerTerritories = owner->getOwnedTerritories();
    bool validTargetTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), target_) == currentPlayerTerritories.end();
    return validTargetTerritory && canAttack(owner, target_);
}

// Executes the BombOrder.
void BombOrder::execute_()
{
    int armiesOnTarget = target_->getNumberOfArmies();
    target_->removeArmies(armiesOnTarget / 2);
    cout << "Bombed " << armiesOnTarget / 2 << " enemy armies on " << target_->getName() << "." << endl;
}

// Stream insertion operator overloading
ostream &BombOrder::print_(ostream &output) const
{
    output << "[BombOrder]";

    if (target_ != NULL)
    {
        output << " Target: " << target_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType BombOrder::getType()
{
    return BOMB;
}


/* 
===================================
 Implementation for BlockadeOrder class
===================================
 */

// Default constructor
BlockadeOrder::BlockadeOrder() : Order(3), territory_(NULL) {}

// Constructor
BlockadeOrder::BlockadeOrder(Territory* territory) : Order(3), territory_(territory) {}

// Copy constructor
BlockadeOrder::BlockadeOrder(const BlockadeOrder &order) : Order(order), territory_(order.territory_) {}

// Assignment operator overloading
const BlockadeOrder &BlockadeOrder::operator=(const BlockadeOrder &order)
{
    Order::operator=(order);
    territory_ = order.territory_;
    return *this;
}

// Return a pointer to a new instance of BlockadeOrder.
Order* BlockadeOrder::clone() const
{
    return new BlockadeOrder(*this);
}

// Checks that the BlockadeOrder is valid.
bool BlockadeOrder::validate(Player* owner)
{
    if (territory_ == NULL)
    {
        return false;
    }

    auto currentPlayerTerritories = owner->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), territory_) != currentPlayerTerritories.end();
}

// Executes the BlockadeOrder.
void BlockadeOrder::execute_()
{
    territory_->addArmies(territory_->getNumberOfArmies());
    cout << "Blockade called on " << territory_->getName() << ". ";
    cout << territory_->getNumberOfArmies() << " neutral armies now occupy this territory." << endl;
}

// Stream insertion operator overloading
ostream &BlockadeOrder::print_(ostream &output) const
{
    output << "[BlockadeOrder]";

    if (territory_ != NULL)
    {
        output << " Territory: " << territory_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType BlockadeOrder::getType()
{
    return BLOCKADE;
}


/* 
===================================
 Implementation for AirliftOrder class
===================================
 */

// Default constructor
AirliftOrder::AirliftOrder() : Order(2), numberOfArmies_(0), source_(NULL), destination_(NULL) {}

// Constructor
AirliftOrder::AirliftOrder(int numberOfArmies, Territory* source, Territory* destination) : Order(2), numberOfArmies_(numberOfArmies), source_(source), destination_(destination) {}

// Copy constructor
AirliftOrder::AirliftOrder(const AirliftOrder &order) : Order(order), numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_) {}

// Assignment operator overloading
const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

// Return a pointer to a new instance of AirliftOrder.
Order* AirliftOrder::clone() const
{
    return new AirliftOrder(*this);
}

// Checks that the AirliftOrder is valid.
bool AirliftOrder::validate(Player* owner)
{
    if (source_ == NULL || destination_ == NULL)
    {
        return false;
    }

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
    output << "[AirliftOrder]";

    if (source_ != NULL && destination_ != NULL)
    {
        output << " " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType AirliftOrder::getType()
{
    return AIRLIFT;
}


/* 
===================================
 Implementation for NegotiateOrder class
===================================
 */

// Default constructor
NegotiateOrder::NegotiateOrder() : initiator_(NULL), target_(NULL) {}

// Constructor
NegotiateOrder::NegotiateOrder(Player* initiator, Player* target) : initiator_(initiator), target_(target) {}

// Copy constructor
NegotiateOrder::NegotiateOrder(const NegotiateOrder &order) : Order(order), initiator_(order.initiator_), target_(order.target_) {}

// Assignment operator overloading
const NegotiateOrder &NegotiateOrder::operator=(const NegotiateOrder &order)
{
    Order::operator=(order);
    initiator_ = order.initiator_;
    target_ = order.target_;
    return *this;
}

// Return a pointer to a new instance of NegotiateOrder.
Order* NegotiateOrder::clone() const
{
    return new NegotiateOrder(*this);
}

// Checks that the NegotiateOrder is valid.
bool NegotiateOrder::validate(Player* owner)
{
    if (initiator_ == NULL || target_ == NULL)
    {
        return false;
    }

    return owner != target_;
}

// Executes the NegotiateOrder.
void NegotiateOrder::execute_()
{
    initiator_->addDiplomaticRelation(target_);
    target_->addDiplomaticRelation(initiator_);
    cout << "Negotiated diplomacy between " << initiator_->getName() << " and " << target_->getName() << "." << endl;
}

// Stream insertion operator overloading
ostream &NegotiateOrder::print_(ostream &output) const
{
    output << "[NegotiateOrder]";

    if (initiator_ != NULL && target_ != NULL)
    {
        output << " Initiator: " << initiator_->getName() << ", Target: " << target_->getName();
    }

    return output;
}

// Get the type of the Order sub-class
OrderType NegotiateOrder::getType()
{
    return NEGOTIATE;
}
