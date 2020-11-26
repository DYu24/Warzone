#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <unordered_set>


// Constructors
Player::Player()
    : reinforcements_(0), name_("Neutral Player"), orders_(new OrdersList()), hand_(new Hand()), committed_(false), strategy_(new NeutralPlayerStrategy()) {}

Player::Player(string name) : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()), committed_(false), strategy_(new NeutralPlayerStrategy()) {}

Player::Player(string name, PlayerStrategy* strategy)
    : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()), committed_(false), strategy_(strategy) {}

Player::Player(const Player &player)
    : reinforcements_(player.reinforcements_),
      name_(player.name_),
      ownedTerritories_(player.ownedTerritories_),
      orders_(new OrdersList(*player.orders_)),
      hand_(new Hand(*player.hand_)),
      diplomaticRelations_(player.diplomaticRelations_),
      committed_(player.committed_),
      strategy_(player.strategy_->clone()) {}

// Destructor
Player::~Player()
{
    delete orders_;
    delete hand_;
    delete strategy_;
    orders_ = nullptr;
    hand_ = nullptr;
    strategy_ = nullptr;
}

// Operator overloading
const Player &Player::operator=(const Player &player)
{
    delete orders_;
    delete hand_;
    delete strategy_;
    orders_ = new OrdersList(*player.orders_);
    hand_ = new Hand(*player.hand_);
    strategy_ = player.strategy_->clone();
    reinforcements_ = player.reinforcements_;
    name_ = player.name_;
    ownedTerritories_ = player.ownedTerritories_;
    return *this;
}

ostream &operator<<(ostream &output, const Player &player)
{
    output << "[Player] " << player.name_ << " has " << player.reinforcements_ << " reinforcements, " << player.ownedTerritories_.size() << " Territories, ";
    output << player.orders_->size() << " Orders, " << player.hand_->size() << " cards in Hand";
    return output;
}

// Getters
vector<Territory*> Player::getOwnedTerritories() const
{
    return ownedTerritories_;
}

string Player::getName() const
{
    return name_;
}

OrdersList Player::getOrdersList() const
{
    return *orders_;
}

Hand Player::getHand() const
{
    return *hand_;
}

vector<Player*> Player::getDiplomaticRelations() const
{
    return diplomaticRelations_;
}

int Player::getReinforcements() const
{
    return reinforcements_;
}

// Setter
void Player::setStrategy(PlayerStrategy* strategy)
{
    delete strategy_;
    strategy_ = strategy;
}

// Add a number of reinforcements to the Player's reinforcement pool
void Player::addReinforcements(int reinforcements)
{
    reinforcements_ += reinforcements;
}

// Add a territory to the Player's list of owned territories
void Player::addOwnedTerritory(Territory* territory)
{
    ownedTerritories_.push_back(territory);
}

// Remove a territory from the Player's list of owned territories
void Player::removeOwnedTerritory(Territory* territory)
{
    auto removeIterator = remove(ownedTerritories_.begin(), ownedTerritories_.end(), territory);
    ownedTerritories_.erase(removeIterator, ownedTerritories_.end());
}

// Add an enemy player to the list of diplomatic relations for this current player
void Player::addDiplomaticRelation(Player* player)
{
    diplomaticRelations_.push_back(player);
}

// Clear the list of diplomatic relations and the map of issued orders to territories
void Player::endTurn()
{
    diplomaticRelations_.clear();
    issuedDeploymentsAndAdvancements_.clear();
    committed_ = false;
}

// Add an order to the player's list of orders
void Player::addOrder(Order* order)
{
    orders_->add(order);
}

// Remove and return the next order to be executed from the Player's list of orders
Order* Player::getNextOrder()
{
    return orders_->popTopOrder();
}

// Return the next order from the Player's list of orders without removing it from the list
Order* Player::peekNextOrder()
{
    return orders_->peek();
}

// Draw a random card from the deck and place it in the Player's hand
void Player::drawCardFromDeck()
{
    if (!isNeutral())
    {
        Card* card = GameEngine::getDeck()->draw();
        if (card != nullptr)
        {
            card->setOwner(this);
            hand_->addCard(card);
        }
    }
}

// Check whether player is neutral or not
bool Player::isNeutral() const
{
    NeutralPlayerStrategy* neutral = dynamic_cast<NeutralPlayerStrategy*>(strategy_);
    return neutral != nullptr;
}

// Check if the player is finished issuing orders
bool Player::isDoneIssuingOrders() const
{
    return committed_ || isNeutral();
}

// Return a list of territories to defend
vector<Territory*> Player::toDefend() const
{
    return strategy_->toDefend(this);
}

// Return a list of territories to attack
vector<Territory*> Player::toAttack() const
{
    return strategy_->toAttack(this);
}

// Create an order and place it in the Player's list of orders
void Player::issueOrder()
{
    int oldNumberOfOrders = orders_->size();

    strategy_->issueOrder(this);

    if (oldNumberOfOrders == orders_->size())
    {
        cout << "No new order issued." << endl;
    }
}

// Helper method to play a random Card from the Player's hand, if any.
// Returns `true` if the played Card requires no further action from the Player.
// Returns `false` if further action needs to be taken (i.e. Deploy additional units from Reinforcement card).
// bool Player::playCard()
// {
//     if (hand_->size() != 0)
//     {
//         // Play a random card from hand
//         int randomCardIndex = rand() % hand_->size();
//         Card* card = hand_->removeCard(randomCardIndex);
//         Order* order = card->play();

//         cout << "Played: " << *card << endl;

//         // Return the played card back to the deck
//         card->setOwner(nullptr);
//         GameEngine::getDeck()->addCard(card);

//         if (order != nullptr)
//         {
//             orders_->add(order);
//         }
//         else if (reinforcements_ > 0)
//         {
//             // Reinforcement card played: deploy the additional reinforcements
//             issueDeployOrder(toDefend());
//         }
//         return false;
//     }

//     return true;
// }

// Check if player has already issued an advance order from `source` to `destination`
bool Player::advancePairingExists(Territory* source, Territory* destination)
{
    auto issuedIterator = issuedDeploymentsAndAdvancements_.find(source);
    if (issuedIterator != issuedDeploymentsAndAdvancements_.end())
    {
        vector<Territory*> pastAdvancements = issuedIterator->second;
        if (find(pastAdvancements.begin(), pastAdvancements.end(), destination) != pastAdvancements.end())
        {
            return true;
        }
    }

    return false;
}
