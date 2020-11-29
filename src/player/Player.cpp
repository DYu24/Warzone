#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <math.h>
#include <sstream>


// Constructors
Player::Player()
    : reinforcements_(0),
      name_("Neutral Player"),
      orders_(new OrdersList()),
      hand_(new Hand()),
      committed_(false),
      strategy_(new NeutralPlayerStrategy()) {}

Player::Player(std::string name)
    : reinforcements_(0),
      name_(name),
      orders_(new OrdersList()),
      hand_(new Hand()),
      committed_(false),
      strategy_(new NeutralPlayerStrategy()) {}

Player::Player(std::string name, PlayerStrategy* strategy)
    : reinforcements_(0),
      name_(name),
      orders_(new OrdersList()),
      hand_(new Hand()),
      committed_(false),
      strategy_(strategy) {}

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

std::ostream &operator<<(std::ostream &output, const Player &player)
{
    output << "[Player] " << player.name_ << " has " << player.reinforcements_ << " reinforcements, " << player.ownedTerritories_.size() << " Territories, ";
    output << player.orders_->size() << " Orders, " << player.hand_->size() << " cards in Hand";
    return output;
}

// Getters
std::vector<Territory*> Player::getOwnedTerritories() const
{
    return ownedTerritories_;
}

std::string Player::getName() const
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

std::vector<Player*> Player::getDiplomaticRelations() const
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

// Add an enemy player to the list of diplomatic relations for this player
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

// Check whether the player is human or not
bool Player::isHuman() const
{
    HumanPlayerStrategy* human = dynamic_cast<HumanPlayerStrategy*>(strategy_);
    return human != nullptr;
}

// Check whether the player is neutral or not
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

// Get a list of territories with available armies for moving
std::vector<Territory*> Player::getOwnTerritoriesWithMovableArmies() const
{
    std::vector<Territory*> territories;
    for (const auto &territory : ownedTerritories_)
    {
       if (territory->getNumberOfMovableArmies() > 0)
       {
           territories.push_back(territory);
       }
    }

    return territories;
}


// Return a list of territories to defend
std::vector<Territory*> Player::toDefend() const
{
    return strategy_->toDefend(this);
}

// Return a list of territories to attack
std::vector<Territory*> Player::toAttack() const
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
        std::cout << "No new order issued." << std::endl;
    }
}

// Check if the player has already issued an advance order from `source` to `destination`
bool Player::advancePairingExists_(Territory* source, Territory* destination)
{
    auto issuedIterator = issuedDeploymentsAndAdvancements_.find(source);
    if (issuedIterator != issuedDeploymentsAndAdvancements_.end())
    {
        std::vector<Territory*> pastAdvancements = issuedIterator->second;
        return find(pastAdvancements.begin(), pastAdvancements.end(), destination) != pastAdvancements.end();
    }

    return false;
}
