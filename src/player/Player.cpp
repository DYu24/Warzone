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
Player::Player() : reinforcements_(0), name_("unknown_player"), orders_(new OrdersList()), hand_(new Hand()) {}

// Constructor
Player::Player(string name) : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()) {}

// Copy constructor
Player::Player(const Player &player)
    : reinforcements_(player.reinforcements_), name_(player.name_), ownedTerritories_(player.ownedTerritories_), orders_(new OrdersList(*player.orders_)),
    hand_(new Hand(*player.hand_)), diplomaticRelations_(player.diplomaticRelations_) {}

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

// Clears the list of diplomatic relations with other players
void Player::clearDiplomaticRelations()
{
    diplomaticRelations_.clear();
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
    vector<Territory*> territoriesToAttack = toAttack();
    vector<Territory*> territoriesToDefend = toDefend();
    issueDeployOrders(territoriesToDefend);
    issueAdvanceOrders(territoriesToAttack, territoriesToDefend);
    playCard();
}

/*
 * Helper method to issue DeployOrders for the Player
 */
void Player::issueDeployOrders(vector<Territory*> territoriesToDefend)
{
    int territoryDeployments[territoriesToDefend.size()] {};

    int idx = 0;
    while (reinforcements_ > 0)
    {
        const int DEFAULT_ARMIES_TO_DEPLOY = 3;

        int armiesToDeploy = min(DEFAULT_ARMIES_TO_DEPLOY, reinforcements_);
        territoryDeployments[idx] += armiesToDeploy;
        reinforcements_ -= armiesToDeploy;

        idx = (idx + 1) % territoriesToDefend.size();
    }

    for (int i = 0; i < territoriesToDefend.size(); i++)
    {
        int numberOfArmies = territoryDeployments[i];
        Territory* destination = territoriesToDefend.at(i);

        if (numberOfArmies > 0)
        {
            orders_->add(new DeployOrder(numberOfArmies, destination));
            destination->addPendingIncomingArmies(numberOfArmies);
        }
    }
}

/*
 * Helper method to issue AdvanceOrders for the Player
 */
void Player::issueAdvanceOrders(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend)
{
    // Map territories to defend and to attack to a priority value
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

    // Iterate over the player's owned territories (only those that have any armies to move) to issue advance orders
    Map* map = GameEngine::getMap();
    for (auto territory : ownedTerritories_)
    {
        int movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        if (movableArmies <= 0)
        {
            continue;
        }

        // Sort the current territory's neighbors according to the priority specified by `toAttack()` and `toDefend()`
        vector<Territory*> neighboringTerritories = map->getAdjacentTerritories(territory);
        sort(neighboringTerritories.begin(), neighboringTerritories.end(), [&priorities](auto t1, auto t2) { return priorities[t1] < priorities[t2]; });

        // Try to pick the highest priority territory that is possible to advance to
        auto potentialDestinationIterator = neighboringTerritories.begin();
        while (potentialDestinationIterator != neighboringTerritories.end())
        {
            Territory* potentialDestination = *potentialDestinationIterator;
            if (find(territoriesToAttack.begin(), territoriesToAttack.end(), potentialDestination) != territoriesToAttack.end())
            {
                // Pick this destination if there's a chance of conquering it
                if (movableArmies > ceil(potentialDestination->getNumberOfArmies() / 0.6))
                {
                    int armiesToMove = min(movableArmies, (int)ceil(potentialDestination->getNumberOfArmies() / 0.6));
                    orders_->add(new AdvanceOrder(armiesToMove, territory, potentialDestination, true));
                    territory->addPendingOutgoingArmies(armiesToMove);
                    break;
                }
            }
            else
            {
                // Move half (arbitrary) the number of armies from this territory to the destination
                int armiesToMove = max(movableArmies, movableArmies / 2);
                orders_->add(new AdvanceOrder(armiesToMove, territory, potentialDestination, false));
                territory->addPendingOutgoingArmies(armiesToMove);
                break;
            }

            potentialDestinationIterator++;
        }
    }
}

/*
 * Helper method to play a random Card from the Player's hand, if any
 */
void Player::playCard()
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
            issueDeployOrders(toDefend());
        }

        Card* card = hand_->removeCard(randomCardIndex);
        card->setOwner(NULL);
        GameEngine::getDeck()->addCard(card);
    }
}
