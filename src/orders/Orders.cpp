#include "../game_engine/GameEngine.h"
#include "Orders.h"
#include <algorithm>
#include <iterator>
#include <math.h>

namespace
{
    // Custom comparator to sort Orders by priority
    bool compareOrders(Order* order1, Order* order2)
    {
        return order1->getPriority() < order2->getPriority();
    }
    
    // Helper function to check whether a territory can be attacked by a specific player.
    // Returns `true` if the attacker already owns the target territory
    // OR
    // if there is no diplomacy between the attacker and the owner of the target.
    bool canAttack(Player* attacker, Territory* target)
    {
        Player* ownerOfTarget = GameEngine::getOwnerOf(target);
        std::vector<Player*> diplomaticRelations = attacker->getDiplomaticRelations();
        bool diplomacyWithOwnerOfTarget = find(diplomaticRelations.begin(), diplomaticRelations.end(), ownerOfTarget) != diplomaticRelations.end();

        if (diplomacyWithOwnerOfTarget)
        {
            std::cout << attacker->getName() << " and " << ownerOfTarget->getName() << " cannot attack each other for the rest of this turn. ";
        }

        return attacker == ownerOfTarget || !diplomacyWithOwnerOfTarget;
    }
}


/* 
===================================
 Implementation for Order class
===================================
 */

// Constructors
Order::Order(): issuer_(nullptr), priority_(4) {}

Order::Order(Player* issuer, int priority) : issuer_(issuer), priority_(priority) {}

Order::Order(const Order &order) : issuer_(order.issuer_), priority_(order.priority_) {}

// Destructor
Order::~Order() {}

// Operator overloading
const Order &Order::operator=(const Order &order)
{
    issuer_ = order.issuer_;
    priority_ = order.priority_;
    return *this;
}

std::ostream &operator<<(std::ostream &output, const Order &order)
{
    return order.print_(output);
}

// Validate and execute the Order. Invalid orders will have no effect.
void Order::execute()
{
    if (validate())
    {
        execute_();
    }
    else
    {
        std::cout << "Order invalidated. Skipping..." << std::endl;
        undo_();
    }
}

// Get order priority
int Order::getPriority() const
{
    return priority_;
}

// Reverse the pre-orders-execution game state back to before the order was created.
// The default behavior is to do nothing if there is no meta-state to reset
// (e.g. resetting pending incoming/outgoing armies from territories)
void Order::undo_() {}


/* 
===================================
 Implementation for OrdersList class
===================================
 */

// Constructors
OrdersList::OrdersList() {}

OrdersList::OrdersList(const OrdersList &orders)
{
    for (const auto &order : orders.orders_)
    {
        orders_.push_back(order->clone());
    }
}

// Destructor
OrdersList::~OrdersList()
{
    for (const auto &order : orders_)
    {
        delete order;
    }
    orders_.clear();
}

// Operator overloading
const OrdersList &OrdersList::operator=(const OrdersList &orders)
{
    setOrders(orders.orders_);
    return *this;
}

std::ostream &operator<<(std::ostream &output, const OrdersList &orders)
{
    output << "[Orders List] Size=" << orders.size();
    return output;
}

// Getter and setter
std::vector<Order*> OrdersList::getOrders() const
{
    return orders_;
}

void OrdersList::setOrders(std::vector<Order*> orders)
{
    for (const auto &order : orders_)
    {
        delete order;
    }
    orders_.clear();

    for (const auto &order : orders)
    {
        orders_.push_back(order->clone());
    }
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
        return nullptr;
    }

    sort(orders_.begin(), orders_.end(), compareOrders);

    return orders_.front();
}

// Return the number of orders in the OrdersList
int OrdersList::size() const
{
    return orders_.size();
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
                std::swap(*orderPosition, *next(orderPosition, 1));
                orderPosition++;
            }
        }

        // If the order is ahead of its destination, move it backwards
        if (destinationPosition < orderPosition)
        {
            while (orderPosition != destinationPosition)
            {
                std::swap(*orderPosition, *prev(orderPosition, 1));
                orderPosition--;
            }
        }
    }
}

// Delete an order from the OrderList specified by the `target` index.
void OrdersList::remove(int target)
{
    auto orderToRemoveIterator = orders_.begin() + target;
    delete *orderToRemoveIterator;
    orders_.erase(orderToRemoveIterator);
}


/* 
===================================
 Implementation for DeployOrder class
===================================
 */

// Constructors
DeployOrder::DeployOrder() : Order(nullptr, 1), numberOfArmies_(0), destination_(nullptr) {}

DeployOrder::DeployOrder(Player* issuer, int numberOfArmies, Territory* destination) : Order(issuer, 1), numberOfArmies_(numberOfArmies), destination_(destination) {}

DeployOrder::DeployOrder(const DeployOrder &order) : Order(order), numberOfArmies_(order.numberOfArmies_), destination_(order.destination_) {}

// Operator overloading
const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    destination_ = order.destination_;
    return *this;
}

std::ostream &DeployOrder::print_(std::ostream &output) const
{
    output << "[DeployOrder]";

    if (destination_ != nullptr)
    {
        output << " " << numberOfArmies_ << " armies to " << destination_->getName();
    }

    return output;
}

// Return a pointer to a new instance of DeployOrder.
Order* DeployOrder::clone() const
{
    return new DeployOrder(*this);
}

// Add a number of armies to deploy to the order
void DeployOrder::addArmies(int additional)
{
    numberOfArmies_ += additional;
}

// Checks that the DeployOrder is valid.
bool DeployOrder::validate() const
{   
    if (issuer_ == nullptr || destination_ == nullptr)
    {
        return false;
    }

    std::vector<Territory*> currentPlayerTerritories = issuer_->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), destination_) != currentPlayerTerritories.end();
}

// Executes the DeployOrder.
void DeployOrder::execute_()
{
    destination_->addArmies(numberOfArmies_);
    destination_->setPendingIncomingArmies(0);
    std::cout << "Deployed " << numberOfArmies_ << " armies to " << destination_->getName() << "." << std::endl;
}

// Reverse the pre-orders-execution game state back to before the order was created.
// Resets the contribution of this order to the number of pending incoming armies on the destination territory.
void DeployOrder::undo_()
{
    int newPendingIncomingArmies = destination_->getPendingIncomingArmies() - numberOfArmies_;
    destination_->setPendingIncomingArmies(newPendingIncomingArmies);
}

// Get the type of the Order sub-class
OrderType DeployOrder::getType() const
{
    return DEPLOY;
}


/* 
===================================
 Implementation for AdvanceOrder class
===================================
 */

// Constructors
AdvanceOrder::AdvanceOrder() : Order(), numberOfArmies_(0), source_(nullptr), destination_(nullptr) {}

AdvanceOrder::AdvanceOrder(Player* issuer, int numberOfArmies, Territory* source, Territory* destination)
    : Order(issuer, 4), numberOfArmies_(numberOfArmies), source_(source), destination_(destination) {}

AdvanceOrder::AdvanceOrder(const AdvanceOrder &order)
    : Order(order), numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_) {}

// Operator overloading
const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

std::ostream &AdvanceOrder::print_(std::ostream &output) const
{
    output << "[AdvanceOrder]";

    if (source_ != nullptr && destination_ != nullptr)
    {
        output << " " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    }

    return output;
}

// Return a pointer to a new instace of AdvanceOrder
Order* AdvanceOrder::clone() const
{
    return new AdvanceOrder(*this);
}

// Checks that the AdvanceOrder is valid.
bool AdvanceOrder::validate() const
{
    if (issuer_ == nullptr || source_ == nullptr || destination_ == nullptr)
    {
        return false;
    }

    std::vector<Territory*> currentPlayerTerritories = issuer_->getOwnedTerritories();
    bool validSourceTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), source_) != currentPlayerTerritories.end();
    bool hasAnyArmiesToAdvance = source_->getNumberOfArmies() > 0;

    return validSourceTerritory && hasAnyArmiesToAdvance && canAttack(issuer_, destination_);
}

// Executes the AdvanceOrder.
void AdvanceOrder::execute_()
{
    Player* defender = GameEngine::getOwnerOf(destination_);
    bool offensive = issuer_ != defender;

    // Recalculate how many armies could actually be moved (in case the state of the territory has changed due to an attack)
    int movableArmiesFromSource = std::min(source_->getNumberOfArmies(), numberOfArmies_); 

    if (offensive)
    {
        // Simulate battle
        source_->removeArmies(movableArmiesFromSource);

        int defendersKilled = round(movableArmiesFromSource * 0.6);
        int attackersKilled = round(destination_->getNumberOfArmies() * 0.7);

        int survivingAttackers = std::max(movableArmiesFromSource - attackersKilled, 0);
        int survivingDefenders = std::max(destination_->getNumberOfArmies() - defendersKilled, 0);
        destination_->removeArmies(defendersKilled);

        // Failed attack
        if (survivingDefenders > 0 || survivingAttackers <= 0)
        {
            source_->addArmies(survivingAttackers);
            std::cout << "Failed attack on " << destination_->getName() << " with " << survivingDefenders << " enemy armies left standing.";

            if (survivingAttackers > 0)
            {
                std::cout << " Retreating " << survivingAttackers << " attacking armies back to " << source_->getName() << std::endl;
            }
            else
            {
                std::cout << std::endl;
            }
        }
        // Successful attack
        else
        {
            issuer_->addOwnedTerritory(destination_);
            defender->removeOwnedTerritory(destination_);
            destination_->addArmies(survivingAttackers);
            std::cout << "Successful attack on " << destination_->getName() << ". " << survivingAttackers << " armies now occupy this territory." << std::endl;
        }
    }
    else
    {
        source_->removeArmies(movableArmiesFromSource);
        destination_->addArmies(movableArmiesFromSource);
        std::cout << "Advanced " << movableArmiesFromSource << " armies from " << source_->getName() << " to " << destination_->getName() << "." << std::endl;
    }

    source_->setPendingOutgoingArmies(0);
}

// Reverse the pre-orders-execution game state back to before the order was created.
// Resets the contribution of this order to the number of pending outgoing armies from the source territory.
void AdvanceOrder::undo_()
{
    int newPendingOutgoingArmies = source_->getPendingOutgoingArmies() - numberOfArmies_;
    source_->setPendingOutgoingArmies(newPendingOutgoingArmies);
}

// Get the type of the Order sub-class
OrderType AdvanceOrder::getType() const
{
    return ADVANCE;
}


/* 
===================================
 Implementation for BombOrder class
===================================
 */

// Constructors
BombOrder::BombOrder() : Order(), target_(nullptr) {}

BombOrder::BombOrder(Player* issuer, Territory* target) : Order(issuer, 4), target_(target) {}

BombOrder::BombOrder(const BombOrder &order) : Order(order), target_(order.target_) {}

// Operator overloading
const BombOrder &BombOrder::operator=(const BombOrder &order)
{
    Order::operator=(order);
    target_ = order.target_;
    return *this;
}

std::ostream &BombOrder::print_(std::ostream &output) const
{
    output << "[BombOrder]";

    if (target_ != nullptr)
    {
        output << " Target: " << target_->getName();
    }

    return output;
}

// Return a pointer to a new instance of BombOrder.
Order* BombOrder::clone() const
{
    return new BombOrder(*this);
}

// Checks that the BombOrder is valid.
bool BombOrder::validate() const
{
    if (issuer_ == nullptr || target_ == nullptr)
    {
        return false;
    }

    std::vector<Territory*> currentPlayerTerritories = issuer_->getOwnedTerritories();
    bool validTargetTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), target_) == currentPlayerTerritories.end();
    return validTargetTerritory && canAttack(issuer_, target_);
}

// Executes the BombOrder.
void BombOrder::execute_()
{
    int armiesOnTarget = target_->getNumberOfArmies();
    target_->removeArmies(armiesOnTarget / 2);
    std::cout << "Bombed " << armiesOnTarget / 2 << " enemy armies on " << target_->getName() << ". ";
    std::cout << target_->getNumberOfArmies() << " remaining." << std::endl;
}

// Get the type of the Order sub-class
OrderType BombOrder::getType() const
{
    return BOMB;
}


/* 
===================================
 Implementation for BlockadeOrder class
===================================
 */

// Constructors
BlockadeOrder::BlockadeOrder() : Order(nullptr, 3), territory_(nullptr) {}

BlockadeOrder::BlockadeOrder(Player* issuer, Territory* territory) : Order(issuer, 3), territory_(territory) {}

BlockadeOrder::BlockadeOrder(const BlockadeOrder &order) : Order(order), territory_(order.territory_) {}

// Operator overloading
const BlockadeOrder &BlockadeOrder::operator=(const BlockadeOrder &order)
{
    Order::operator=(order);
    territory_ = order.territory_;
    return *this;
}

std::ostream &BlockadeOrder::print_(std::ostream &output) const
{
    output << "[BlockadeOrder]";

    if (territory_ != nullptr)
    {
        output << " Territory: " << territory_->getName() << " (" << territory_->getNumberOfArmies() << " present)";
    }

    return output;
}

// Return a pointer to a new instance of BlockadeOrder.
Order* BlockadeOrder::clone() const
{
    return new BlockadeOrder(*this);
}

// Checks that the BlockadeOrder is valid.
bool BlockadeOrder::validate() const
{
    if (issuer_ == nullptr || territory_ == nullptr)
    {
        return false;
    }

    std::vector<Territory*> currentPlayerTerritories = issuer_->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), territory_) != currentPlayerTerritories.end();
}

// Executes the BlockadeOrder.
void BlockadeOrder::execute_()
{
    territory_->addArmies(territory_->getNumberOfArmies());
    GameEngine::assignToNeutralPlayer(territory_);
    std::cout << "Blockade called on " << territory_->getName() << ". ";
    std::cout << territory_->getNumberOfArmies() << " neutral armies now occupy this territory." << std::endl;
}

// Get the type of the Order sub-class
OrderType BlockadeOrder::getType() const
{
    return BLOCKADE;
}


/* 
===================================
 Implementation for AirliftOrder class
===================================
 */

// Constructors
AirliftOrder::AirliftOrder() : Order(nullptr, 2), numberOfArmies_(0), source_(nullptr), destination_(nullptr) {}

AirliftOrder::AirliftOrder(Player* issuer, int numberOfArmies, Territory* source, Territory* destination)
    : Order(issuer, 2), numberOfArmies_(numberOfArmies), source_(source), destination_(destination) {}

AirliftOrder::AirliftOrder(const AirliftOrder &order) : Order(order), numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_) {}

// Operator overloading
const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    Order::operator=(order);
    numberOfArmies_ = order.numberOfArmies_;
    source_ = order.source_;
    destination_ = order.destination_;
    return *this;
}

std::ostream &AirliftOrder::print_(std::ostream &output) const
{
    output << "[AirliftOrder]";

    if (source_ != nullptr && destination_ != nullptr)
    {
        output << " " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    }

    return output;
}

// Return a pointer to a new instance of AirliftOrder.
Order* AirliftOrder::clone() const
{
    return new AirliftOrder(*this);
}

// Checks that the AirliftOrder is valid.
bool AirliftOrder::validate() const
{
    if (issuer_ == nullptr || source_ == nullptr || destination_ == nullptr || source_ == destination_)
    {
        return false;
    }

    std::vector<Territory*> currentPlayerTerritories = issuer_->getOwnedTerritories();

    bool validSourceTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), source_) != currentPlayerTerritories.end();
    bool validDestinationTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), destination_) != currentPlayerTerritories.end();
    bool hasAnyArmiesToAirlift = source_->getNumberOfMovableArmies() > 0;

    return validSourceTerritory && validDestinationTerritory && hasAnyArmiesToAirlift;
}

// Executes the AirliftOrder.
void AirliftOrder::execute_()
{
    // Recalculate how many armies could actually be moved in case the state of the territory has changed due to an attack
    int movableArmiesFromSource = std::min(source_->getNumberOfArmies(), numberOfArmies_); 

    destination_->addArmies(movableArmiesFromSource);
    source_->removeArmies(movableArmiesFromSource);
    source_->setPendingOutgoingArmies(0);

    std::cout << "Airlifted " << movableArmiesFromSource << " armies from " << source_->getName() << " to " << destination_->getName() << "." << std::endl;
}

// Reverse the pre-orders-execution game state back to before the order was created.
// Resets the contribution of this order to the number of pending outgoing armies from the source territory.
void AirliftOrder::undo_()
{
    int newPendingOutgoingArmies = source_->getPendingOutgoingArmies() - numberOfArmies_;
    source_->setPendingOutgoingArmies(newPendingOutgoingArmies);
}

// Get the type of the Order sub-class
OrderType AirliftOrder::getType() const
{
    return AIRLIFT;
}


/* 
===================================
 Implementation for NegotiateOrder class
===================================
 */

// Constructors
NegotiateOrder::NegotiateOrder() : Order(), target_(nullptr) {}

NegotiateOrder::NegotiateOrder(Player* issuer, Player* target) : Order(issuer, 4), target_(target) {}

NegotiateOrder::NegotiateOrder(const NegotiateOrder &order) : Order(order), target_(order.target_) {}

// Operator overloading
const NegotiateOrder &NegotiateOrder::operator=(const NegotiateOrder &order)
{
    Order::operator=(order);
    target_ = order.target_;
    return *this;
}

std::ostream &NegotiateOrder::print_(std::ostream &output) const
{
    output << "[NegotiateOrder]";

    if (issuer_ != nullptr && target_ != nullptr)
    {
        output << " Initiator: " << issuer_->getName() << ", Target: " << target_->getName();
    }

    return output;
}

// Return a pointer to a new instance of NegotiateOrder.
Order* NegotiateOrder::clone() const
{
    return new NegotiateOrder(*this);
}

// Checks that the NegotiateOrder is valid.
bool NegotiateOrder::validate() const
{
    if (issuer_ == nullptr || target_ == nullptr)
    {
        return false;
    }

    return issuer_ != target_;
}

// Executes the NegotiateOrder.
void NegotiateOrder::execute_()
{
    issuer_->addDiplomaticRelation(target_);
    target_->addDiplomaticRelation(issuer_);
    std::cout << "Negotiated diplomacy between " << issuer_->getName() << " and " << target_->getName() << "." << std::endl;
}

// Get the type of the Order sub-class
OrderType NegotiateOrder::getType() const
{
    return NEGOTIATE;
}
