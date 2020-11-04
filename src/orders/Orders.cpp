#include "Orders.h"
#include <algorithm>
#include <iterator>
#include <math.h>

namespace
{
    /*
     * Custom comparator to sort Orders by priority
     */
    bool compareOrders(const unique_ptr<Order> &order1, const unique_ptr<Order> &order2)
    {
        return order1->getPriority() < order2->getPriority();
    }

    /**
     * Helper function to check whether a territory can be attacked by a specific player.
     */
    bool canAttack(const shared_ptr<Player> attacker, const shared_ptr<Territory> target)
    {
        for (auto const &enemy : attacker->getDiplomaticRelations())
        {
            vector<shared_ptr<Territory>> enemyTerritories = enemy->getOwnedTerritories();
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

// Validate and execute the Order. Invalid orders will have no effect.
void Order::execute(const shared_ptr<Player> owner)
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

// Get default order priority (lowest)
int Order::getPriority()
{
    return 4;
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
    setOrders(orders.orders_);
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
vector<unique_ptr<Order>> &OrdersList::getOrders()
{
    return orders_;
}

void OrdersList::setOrders(const vector<unique_ptr<Order>> &orders)
{
    orders_.clear();
    for (auto const &orderPointer : orders)
    {
        orders_.push_back(orderPointer->clone());
    }
}

// Add an order to the OrderList.
void OrdersList::add(unique_ptr<Order> order)
{
    orders_.push_back(std::move(order));
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
unique_ptr<Order> OrdersList::popTopOrder()
{
    if (orders_.empty())
    {
        return NULL;
    }

    sort(orders_.begin(), orders_.end(), compareOrders);

    unique_ptr<Order> topOrder = std::move(orders_.at(0));
    orders_.erase(orders_.begin());

    return topOrder;
}

/* 
===================================
 Implementation for DeployOrder class
===================================
 */

// Default constructor
DeployOrder::DeployOrder() : numberOfArmies_(0), destination_(make_shared<Territory>()) {}

// Constructor
DeployOrder::DeployOrder(int numberOfArmies, shared_ptr<Territory> destination) : numberOfArmies_(numberOfArmies), destination_(destination) {}

// Copy constructor
DeployOrder::DeployOrder(const DeployOrder &order) : numberOfArmies_(order.numberOfArmies_), destination_(order.destination_) {}

// Assignment operator overloading
const DeployOrder &DeployOrder::operator=(const DeployOrder &order)
{
    numberOfArmies_ = order.numberOfArmies_;
    destination_ = order.destination_;
    return *this;
}

// Return a pointer to a new instance of DeployOrder.
unique_ptr<Order> DeployOrder::clone() const
{
    return make_unique<DeployOrder>(*this);
}

// Checks that the DeployOrder is valid.
bool DeployOrder::validate(const shared_ptr<Player> owner)
{   
    auto currentPlayerTerritories = owner->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), destination_) != currentPlayerTerritories.end();
}

// Get priority of DeployOrder
int DeployOrder::getPriority()
{
    return 1;
}

// Executes the DeployOrder.
void DeployOrder::execute_()
{
    destination_->addArmies(numberOfArmies_);
    cout << numberOfArmies_ << " armies deployed to " << destination_->getName() << "." << endl;
}

// Stream insertion operator overloading
ostream &DeployOrder::print_(ostream &output) const
{
    output << "[DeployOrder] " << numberOfArmies_ << " armies to " << destination_->getName();
    return output;
}

/* 
===================================
 Implementation for AdvanceOrder class
===================================
 */

// Default constructor
AdvanceOrder::AdvanceOrder() : numberOfArmies_(0), source_(make_shared<Territory>()), destination_(make_shared<Territory>()), offensive_(false) {}

// Constructor
AdvanceOrder::AdvanceOrder(int numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination)
    : numberOfArmies_(numberOfArmies), source_(source), destination_(destination), offensive_(false) {}

// Copy constructor
AdvanceOrder::AdvanceOrder(const AdvanceOrder &order) : numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_), offensive_(order.offensive_) {}

// Assignment operator overloading
const AdvanceOrder &AdvanceOrder::operator=(const AdvanceOrder &order)
{
    numberOfArmies_ = order.numberOfArmies_;
    source_ = order.source_;
    destination_ = order.destination_;
    offensive_ = order.offensive_;
    return *this;
}

// Return a pointer to a new instace of AdvanceOrder
unique_ptr<Order> AdvanceOrder::clone() const
{
    return make_unique<AdvanceOrder>(*this);
}

// Checks that the AdvanceOrder is valid.
bool AdvanceOrder::validate(const shared_ptr<Player> owner)
{
    auto currentPlayerTerritories = owner->getOwnedTerritories();

    // If the player owns the destination territory, then this AdvanceOrder is not an attack
    bool ownsDestinationTerritory = find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), destination_) != currentPlayerTerritories.end();
    offensive_ = !ownsDestinationTerritory;

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
    output << "[AdvanceOrder] " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
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
bool BombOrder::validate(const shared_ptr<Player> owner)
{
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
bool BlockadeOrder::validate(const shared_ptr<Player> owner)
{
    auto currentPlayerTerritories = owner->getOwnedTerritories();
    return find(currentPlayerTerritories.begin(), currentPlayerTerritories.end(), territory_) != currentPlayerTerritories.end();
}

// Get priority of BlockadeOrder
int BlockadeOrder::getPriority()
{
    return 3;
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
    output << "[BlockadeOrder] Territory: " << territory_->getName();
    return output;
}


/* 
===================================
 Implementation for AirliftOrder class
===================================
 */

// Default constructor
AirliftOrder::AirliftOrder() : numberOfArmies_(0), source_(make_shared<Territory>()), destination_(make_shared<Territory>()) {}

// Constructor
AirliftOrder::AirliftOrder(int numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination) : numberOfArmies_(numberOfArmies), source_(source), destination_(destination) {}

// Copy constructor
AirliftOrder::AirliftOrder(const AirliftOrder &order) : numberOfArmies_(order.numberOfArmies_), source_(order.source_), destination_(order.destination_) {}

// Assignment operator overloading
const AirliftOrder &AirliftOrder::operator=(const AirliftOrder &order)
{
    numberOfArmies_ = order.numberOfArmies_;
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
bool AirliftOrder::validate(const shared_ptr<Player> owner)
{
    return true;
}

// Get priority of AirliftOrder
int AirliftOrder::getPriority()
{
    return 2;
}

// Executes the AirliftOrder.
void AirliftOrder::execute_()
{
    cout << "Airlift executed!" << endl;
}

// Stream insertion operator overloading
ostream &AirliftOrder::print_(ostream &output) const
{
    output << "[AirliftOrder] " << numberOfArmies_ << " armies from " << source_->getName() << " to " << destination_->getName();
    return output;
}


/* 
===================================
 Implementation for NegotiateOrder class
===================================
 */

// Default constructor
NegotiateOrder::NegotiateOrder() : initiator_(make_shared<Player>()), target_(make_shared<Player>()) {}

// Constructor
NegotiateOrder::NegotiateOrder(shared_ptr<Player> initiator, shared_ptr<Player> target) : initiator_(initiator), target_(target) {}

// Copy constructor
NegotiateOrder::NegotiateOrder(const NegotiateOrder &order) : initiator_(order.initiator_), target_(order.target_) {}

// Assignment operator overloading
const NegotiateOrder &NegotiateOrder::operator=(const NegotiateOrder &order)
{
    initiator_ = order.initiator_;
    target_ = order.target_;
    return *this;
}

// Return a pointer to a new instance of NegotiateOrder.
unique_ptr<Order> NegotiateOrder::clone() const
{
    return make_unique<NegotiateOrder>(*this);
}

// Checks that the NegotiateOrder is valid.
bool NegotiateOrder::validate(const shared_ptr<Player> owner)
{
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
    output << "[NegotiateOrder] Initiator: " << initiator_->getName() << ", Target: " << target_->getName();
    return output;
}
