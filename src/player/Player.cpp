#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <unordered_set>

namespace
{
    // Custom comparator to sort Territories by their number of armies. 
    bool compareTerritoriesByArmies(Territory* t1, Territory* t2)
    {
        return t1->getNumberOfArmies() < t2->getNumberOfArmies();
    }

    // Custom comparator to sort Territories by the number of adjacent enemy territories and then by the number of armies.
    bool compareTerritoriesByEnemiesAndArmies(Territory* t1, Territory* t2, const Player* owner)
    {
        Map* map = GameEngine::getMap();
        int t1EnemyNeighbors = 0;
        for (const auto &territory : map->getAdjacentTerritories(t1))
        {
            if (GameEngine::getOwnerOf(territory) != owner)
            {
                t1EnemyNeighbors++;
            }
        }

        int t2EnemyNeighbors = 0;
        for (const auto &territory : map->getAdjacentTerritories(t2))
        {
            if (GameEngine::getOwnerOf(territory) != owner)
            {
                t2EnemyNeighbors++;
            }
        }

        if (t1EnemyNeighbors != t2EnemyNeighbors)
        {
            return t1EnemyNeighbors > t2EnemyNeighbors;   
        }

        return t1->getNumberOfArmies() < t2->getNumberOfArmies();
    }
}

// Constructors
Player::Player() : reinforcements_(0), name_("Neutral Player"), orders_(new OrdersList()), hand_(new Hand()), isNeutral_(true), committed_(false) {}

Player::Player(string name) : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()), isNeutral_(false), committed_(false) {}

Player::Player(const Player &player)
    : reinforcements_(player.reinforcements_),
      name_(player.name_),
      ownedTerritories_(player.ownedTerritories_),
      orders_(new OrdersList(*player.orders_)),
      hand_(new Hand(*player.hand_)),
      diplomaticRelations_(player.diplomaticRelations_),
      isNeutral_(player.isNeutral_),
      committed_(player.committed_) {}

// Destructor
Player::~Player()
{
    delete orders_;
    delete hand_;
    orders_ = nullptr;
    hand_ = nullptr;
}

// Operator overloading
const Player &Player::operator=(const Player &player)
{
    reinforcements_ = player.reinforcements_;
    name_ = player.name_;
    ownedTerritories_ = player.ownedTerritories_;
    orders_ = new OrdersList(*player.orders_);
    hand_ = new Hand(*player.hand_);
    isNeutral_ = player.isNeutral_;
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
    if (!isNeutral_)
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
    return isNeutral_;
}

// Check if the player is finished issuing orders
bool Player::isDoneIssuingOrders() const
{
    return committed_ || isNeutral_;
}

// Return a list of territories to defend
vector<Territory*> Player::toDefend() const
{
    vector<Territory*> territoriesToDefend = ownedTerritories_;
    auto sortLambda = [this](auto t1, auto t2){ return compareTerritoriesByEnemiesAndArmies(t1, t2, this); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return a list of territories to attack
vector<Territory*> Player::toAttack() const
{
    vector<Territory*> attackableTerritories;
    unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (const auto &territory : ownedTerritories_)
    {
        for (const auto &neighbor : map->getAdjacentTerritories(territory))
        {
            bool isEnemyOwned = find(ownedTerritories_.begin(), ownedTerritories_.end(), neighbor) == ownedTerritories_.end();
            bool alreadySeen = territoriesSeen.find(neighbor) != territoriesSeen.end();

            if (isEnemyOwned && !alreadySeen)
            {
                attackableTerritories.push_back(neighbor);
                territoriesSeen.insert(neighbor);
            }
        }
    }

    sort(attackableTerritories.begin(), attackableTerritories.end(), compareTerritoriesByArmies);
    return attackableTerritories;
}

// Create an order and place it in the Player's list of orders
void Player::issueOrder()
{
    if (isNeutral_)
    {
        return;
    }

    int oldNumberOfOrders = orders_->size();
    
    vector<Territory*> territoriesToAttack = toAttack();
    vector<Territory*> territoriesToDefend = toDefend();

    bool finishedDeploying = issueDeployOrder(territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedPlayingCards = playCard();
        if (finishedPlayingCards)
        {
            committed_ = issueAdvanceOrder(territoriesToAttack, territoriesToDefend);
        }
    }

    if (oldNumberOfOrders == orders_->size())
    {
        cout << "No new order issued." << endl;
    }
}

// Helper method to issue a DeployOrder for the Player.
// Returns `true` if the Player is finished with deployments (i.e. no more reinforcements).
// Returns `false` if the Player still has reinforcements to deploy
bool Player::issueDeployOrder(vector<Territory*> territoriesToDefend)
{
    if (reinforcements_ > 0)
    {
        const int DEFAULT_ARMIES_TO_DEPLOY = 5;
        int armiesToDeploy = min(DEFAULT_ARMIES_TO_DEPLOY, reinforcements_);
        
        // In order of highest priority, try to find a territory that hasn't already been deployed to
        Territory* destination = nullptr;
        auto iterator = territoriesToDefend.begin();
        for (; iterator != territoriesToDefend.end(); iterator++)
        {
            if (issuedDeploymentsAndAdvancements_.find(*iterator) == issuedDeploymentsAndAdvancements_.end())
            {
                // Initialize the vector at the key specified by this territory
                issuedDeploymentsAndAdvancements_[*iterator];
                destination = *iterator;
                break;
            }
        }

        // If all territories in `territoriesToDefend` have already been deployed to, then just pick the first one (highest priority)
        if (destination == nullptr)
        {
            destination = territoriesToDefend.front();
        }

        DeployOrder* order = new DeployOrder(this, armiesToDeploy, destination);
        orders_->add(order);
        destination->addPendingIncomingArmies(armiesToDeploy);
        reinforcements_ -= armiesToDeploy;
        
        cout << "Issued: " << *order << endl;
        return false;
    }

    return true;
}

// Helper method to issue AdvanceOrders for the Player
// Returns `true` if the Player is finished with advancements.
// Returns `false` if the Player still wants to issue advance orders.
bool Player::issueAdvanceOrder(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend)
{
    Map* map = GameEngine::getMap();
    for (const auto &territory : territoriesToAttack)
    {
        vector<Territory*> neighboringTerritories = map->getAdjacentTerritories(territory);
        for (const auto &potentialSource : neighboringTerritories)
        {
            if (this == GameEngine::getOwnerOf(potentialSource) && !advancePairingExists(potentialSource, territory))
            {
                // Only advance to this enemy territory if the player has any chance of conquering it
                int movableArmies = potentialSource->getNumberOfArmies() + potentialSource->getPendingIncomingArmies() - potentialSource->getPendingOutgoingArmies();
                int minimumArmiesRequiredToWin = (int)ceil(territory->getNumberOfArmies() / 0.6);
                if (minimumArmiesRequiredToWin > 1 && movableArmies > minimumArmiesRequiredToWin)
                {
                    int armiesToMove = max(movableArmies / 2, minimumArmiesRequiredToWin);
                    AdvanceOrder* order = new AdvanceOrder(this, armiesToMove, potentialSource, territory);
                    orders_->add(order);
                    potentialSource->addPendingOutgoingArmies(armiesToMove);
                    issuedDeploymentsAndAdvancements_[potentialSource].push_back(territory);
                    
                    cout << "Issued: " << *order << endl;
                    return false;
                }
            }
        }
    }

    for (const auto &territory : territoriesToDefend)
    {
        // If the target territory already has 10 armies, then ignore since there's already enough
        if (territory->getNumberOfArmies() <= 10)
        {
            vector<Territory*> neighboringTerritories = map->getAdjacentTerritories(territory);
            for (const auto &potentialSource : neighboringTerritories)
            {
                if (this == GameEngine::getOwnerOf(potentialSource) && !advancePairingExists(potentialSource, territory))
                {
                    // Only choose source territories that can move at least 5 armies
                    int movableArmies = potentialSource->getNumberOfArmies() + potentialSource->getPendingIncomingArmies() - potentialSource->getPendingOutgoingArmies();
                    int armiesToMove = movableArmies / 2;
                    if (armiesToMove >= 5)
                    {
                        AdvanceOrder* order = new AdvanceOrder(this, armiesToMove, potentialSource, territory);
                        orders_->add(order);
                        potentialSource->addPendingOutgoingArmies(armiesToMove);
                        issuedDeploymentsAndAdvancements_[potentialSource].push_back(territory);

                        cout << "Issued: " << *order << endl;
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

// Helper method to play a random Card from the Player's hand, if any.
// Returns `true` if the played Card requires no further action from the Player.
// Returns `false` if further action needs to be taken (i.e. Deploy additional units from Reinforcement card).
bool Player::playCard()
{
    if (hand_->size() != 0)
    {
        // Play a random card from hand
        int randomCardIndex = rand() % hand_->size();
        Card* card = hand_->removeCard(randomCardIndex);
        Order* order = card->play();

        cout << "Played :" << *card << endl;

        // Return the played card back to the deck
        card->setOwner(nullptr);
        GameEngine::getDeck()->addCard(card);

        if (order != nullptr)
        {
            orders_->add(order);
        }
        else if (reinforcements_ > 0)
        {
            // Reinforcement card played: deploy the additional reinforcements
            issueDeployOrder(toDefend());
            return false;
        }
    }

    return true;
}

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
