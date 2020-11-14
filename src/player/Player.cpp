#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <math.h>
#include <set>
#include <sstream>
#include <unordered_set>

namespace
{
    /*
     * Custom comparator to sort Territories by their number of armies. 
     */
    bool compareTerritoriesByArmies(Territory* t1, Territory* t2)
    {
        return t1->getNumberOfArmies() < t2->getNumberOfArmies();
    }

    /*
     * Custom comparator to sort Territories by the number of adjacent enemy territories and then by the number of armies.
     */
    bool compareTerritoriesByEnemiesAndArmies(Territory* t1, Territory* t2, Player* owner)
    {
        Map* map = GameEngine::getMap();
        int t1EnemyNeighbors = 0;
        for (auto territory : map->getAdjacentTerritories(t1))
        {
            if (GameEngine::getOwnerOf(territory) != owner)
            {
                t1EnemyNeighbors++;
            }
        }

        int t2EnemyNeighbors = 0;
        for (auto territory : map->getAdjacentTerritories(t2))
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

// Default constructor
Player::Player() : reinforcements_(0), name_("Neutral_Player"), orders_(new OrdersList()), hand_(new Hand()), isNeutral_(true) {}

// Constructor
Player::Player(string name) : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()), isNeutral_(false) {}

// Copy constructor
Player::Player(const Player &player)
    : reinforcements_(player.reinforcements_), name_(player.name_), ownedTerritories_(player.ownedTerritories_), orders_(new OrdersList(*player.orders_)),
    hand_(new Hand(*player.hand_)), diplomaticRelations_(player.diplomaticRelations_), isNeutral_(player.isNeutral_) {}

// Destructor
Player::~Player()
{
    delete orders_;
    delete hand_;
    orders_ = NULL;
    hand_ = NULL;
}

// Assignment operator overloading
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

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Player &player)
{
    output << "[Player] " << player.name_ << " has " << player.reinforcements_ << " reinforcements, " << player.ownedTerritories_.size() << " Territories, ";
    output << player.orders_->getOrders().size() << " Orders, " << player.hand_->getCards().size() << " cards in Hand";
    return output;
}

// Getters
vector<Territory*> Player::getOwnedTerritories()
{
    return ownedTerritories_;
}

string Player::getName()
{
    return name_;
}

vector<Player*> Player::getDiplomaticRelations()
{
    return diplomaticRelations_;
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

// Adds a player to the list of diplomatic relations
void Player::addDiplomaticRelation(Player* player)
{
    diplomaticRelations_.push_back(player);
}

// Clears the list of diplomatic relations with other players and the map of issued orders to territories
void Player::endTurn()
{
    diplomaticRelations_.clear();
    issuedDeploymentsAndAdvancements_.clear();
    committed_ = false;
}

// Return the next order to be executed from the Player's list of orders
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
    Card* card = GameEngine::getDeck()->draw();
    card->setOwner(this);
    hand_->addCard(card);
}

// Check whether player is neutral or not
bool Player::isNeutral()
{
    return isNeutral_;
}

// Check if the player is finished issuing orders
bool Player::isDoneIssuingOrders()
{
    return committed_ || isNeutral_;
}

// Return a list of territories to defend
vector<Territory*> Player::toDefend()
{
    vector<Territory*> territoriesToDefend = ownedTerritories_;
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), [this](auto const &t1, auto const &t2){ return compareTerritoriesByEnemiesAndArmies(t1, t2, this); });
    return territoriesToDefend;
}

// Return a list of territories to attack
vector<Territory*> Player::toAttack()
{
    vector<Territory*> attackableTerritories;
    unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (auto territory : ownedTerritories_)
    {
        for (auto neighbor : map->getAdjacentTerritories(territory))
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

// Create an Order and place it in the Player's list of orders
void Player::issueOrder()
{
    if (isNeutral_)
    {
        return;
    }
    
    vector<Territory*> territoriesToAttack = toAttack();
    vector<Territory*> territoriesToDefend = toDefend();

    bool finishedDeploying = issueDeployOrder(territoriesToDefend);
    
    if (finishedDeploying)
    {
        bool finishedAdvancing = issueAdvanceOrder(territoriesToAttack, territoriesToDefend);
        
        if (finishedAdvancing)
        {
            committed_ = playCard();
        }
    }
}

/*
 * Helper method to issue a DeployOrder for the Player.
 * Returns `true` if the Player is finished with deployments (i.e. no more reinforcements).
 * Returns `false` if the Player still has reinforcements to deploy
 */
bool Player::issueDeployOrder(vector<Territory*> territoriesToDefend)
{
    if (reinforcements_ > 0)
    {
        const int DEFAULT_ARMIES_TO_DEPLOY = 3;
        int armiesToDeploy = min(DEFAULT_ARMIES_TO_DEPLOY, reinforcements_);
        
        // In order of highest priority, try to find a territory that hasn't already been deployed to
        Territory* destination = NULL;
        auto iterator = territoriesToDefend.begin();
        while (destination == NULL || iterator != territoriesToDefend.end())
        {
            if (issuedDeploymentsAndAdvancements_.find(*iterator) == issuedDeploymentsAndAdvancements_.end())
            {
                destination = *iterator;
                // Initialize the vector at the key specified by this territory
                issuedDeploymentsAndAdvancements_[*iterator];
                break;
            }

            iterator++;
        }

        // If all territories in `territoriesToDefend` have already been deployed to, then just pick the first one (highest priority)
        if (destination == NULL)
        {
            destination = territoriesToDefend.front();
        }

        orders_->add(new DeployOrder(armiesToDeploy, destination));
        destination->addPendingIncomingArmies(armiesToDeploy);
        reinforcements_ -= armiesToDeploy;

        return false;
    }

    return true;
}

/*
 * Helper method to issue AdvanceOrders for the Player
 * Returns `true` if the Player is finished with advancements.
 * Returns `false` if the Player still wants to issue advance orders.
 */
bool Player::issueAdvanceOrder(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend)
{
    int numberOfExistingOrders = orders_->getOrders().size();

    // Map territories to defend/to attack to a value representing priority
    unordered_map<Territory*, int> priorities;
    for (int i = 0; i < territoriesToAttack.size(); i++)
    {
        Territory* territory = territoriesToAttack.at(i);
        priorities[territory] = i;
    }
    for (int i = 0; i < territoriesToDefend.size(); i++)
    {
        Territory* territory = territoriesToDefend.at(i);
        priorities[territory] = i;
    }

    // Find a territory (and one adjacent to it) to create an advance order
    Map* map = GameEngine::getMap();
    AdvanceOrder* order = NULL;
    for (auto territory : ownedTerritories_)
    {
        int movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        if (movableArmies <= 0)
        {
            continue;
        }

        // Get the adjacent territories and filter out those that have already been advanced to from the current territory in previous orders
        vector<Territory*> neighboringTerritories;
        auto issuedIterator = issuedDeploymentsAndAdvancements_.find(territory);
        if (issuedIterator != issuedDeploymentsAndAdvancements_.end())
        {
            vector<Territory*> usedTerritories = issuedIterator->second;
            for (auto const &neighbor : map->getAdjacentTerritories(territory))
            {
                if (find(usedTerritories.begin(), usedTerritories.end(), neighbor) == usedTerritories.end())
                {
                    neighboringTerritories.push_back(neighbor);
                }
            }
        }
        else
        {
            neighboringTerritories = map->getAdjacentTerritories(territory);
        }

        // Sort the current territory's neighbors according to the `priorities` map
        sort(neighboringTerritories.begin(), neighboringTerritories.end(), [&priorities](auto t1, auto t2) { return priorities[t1] < priorities[t2]; });

        // Try to pick the highest priority territory that is possible to advance to
        auto potentialDestinationIterator = neighboringTerritories.begin();
        for (; potentialDestinationIterator != neighboringTerritories.end(); potentialDestinationIterator++)
        {
            Territory* potentialDestination = *potentialDestinationIterator;
            bool isEnemyTerritory = find(territoriesToAttack.begin(), territoriesToAttack.end(), potentialDestination) != territoriesToAttack.end();
            int armiesToMove = 0;

            if (isEnemyTerritory)
            {
                // Pick this destination if there's a chance of conquering it
                if (movableArmies > ceil(potentialDestination->getNumberOfArmies() / 0.6))
                {
                    armiesToMove = min(movableArmies, (int)ceil(potentialDestination->getNumberOfArmies() / 0.6));
                }
                // Otherwise, try another neighboring territory
                else
                {
                    continue;
                }
            }
            else
            {
                // Move half (arbitrary) the number of armies from this territory to the destination
                armiesToMove = max(movableArmies, movableArmies / 2);
            }

            order = new AdvanceOrder(armiesToMove, territory, potentialDestination, isEnemyTerritory);
            territory->addPendingOutgoingArmies(armiesToMove);
            issuedDeploymentsAndAdvancements_[territory].push_back(potentialDestination);
            break;
        }

        if (order != NULL)
        {
            orders_->add(order);
            break;
        }
    }

    // If no new advance orders were issued, then the player is done with advancements
    return numberOfExistingOrders == orders_->getOrders().size();
}

/*
 * Helper method to play a random Card from the Player's hand, if any.
 * Returns `true` if the played Card requires no further action from the Player.
 * Returns `false` if further action needs to be taken (i.e. Deploy additional units from Reinforcement card).
 */
bool Player::playCard()
{
    if (!hand_->getCards().empty())
    {
        int randomCardIndex = rand() % hand_->getCards().size();
        Order* order = hand_->playCardAt(randomCardIndex);

        if (order != NULL)
        {
            orders_->add(order);
        }
        else if (reinforcements_ > 0)
        {
            // Reinforcement card played: deploy the additional reinforcements
            issueDeployOrder(toDefend());
            return false;
        }

        Card* card = hand_->removeCard(randomCardIndex);
        card->setOwner(NULL);
        GameEngine::getDeck()->addCard(card);
    }

    return true;
}
