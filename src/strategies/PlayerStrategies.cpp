#include "PlayerStrategies.h"
#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include <algorithm>
#include <math.h>
#include <unordered_set>


namespace
{
    // Custom comparator to sort Territories by the number of armies and then by the number of adjacent enemy territories.
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

        if (t1->getNumberOfArmies() != t2->getNumberOfArmies())
        {
            return t1->getNumberOfArmies() > t2->getNumberOfArmies();
        }

        return t1EnemyNeighbors > t2EnemyNeighbors;
    }
}


/* 
===================================
 Implementation for PlayerStrategy class
===================================
 */

// Operator overloading
ostream &operator<<(ostream &output, const PlayerStrategy &strategy)
{
    return strategy.print_(output);
}


/* 
===================================
 Implementation for AggressivePlayerStrategy class
===================================
 */

// Operator overloading
ostream &AggressivePlayerStrategy::print_(ostream &output) const
{
    output << "[AggressivePlayerStrategy]";
    return output;
}

// Return a pointer to a new instance of AggressivePlayerStrategy.
PlayerStrategy* AggressivePlayerStrategy::clone() const
{
    return new AggressivePlayerStrategy();
}

// Return a list of territories to defend
vector<Territory*> AggressivePlayerStrategy::toDefend(const Player* player) const
{
    vector<Territory*> territoriesToDefend = player->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() > t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return a list of territories to attack
vector<Territory*> AggressivePlayerStrategy::toAttack(const Player* player) const
{
    vector<Territory*> sources = toDefend(player);
    vector<Territory*> territoriesToAttack;
    unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (const auto &territory : sources)
    {
        vector<Territory*> adjacentTerritories = map->getAdjacentTerritories(territory);
        auto sortLambda = [&player](auto t1, auto t2){ return compareTerritoriesByEnemiesAndArmies(t1, t2, player); };
        sort(adjacentTerritories.begin(), adjacentTerritories.end(), sortLambda);

        for (const auto &neighbor : adjacentTerritories)
        {
            bool isEnemyOwned = find(sources.begin(), sources.end(), neighbor) == sources.end();
            bool alreadySeen = territoriesSeen.find(neighbor) != territoriesSeen.end();

            if (isEnemyOwned && !alreadySeen)
            {
                territoriesToAttack.push_back(neighbor);
                territoriesSeen.insert(neighbor);
            }
        }
    }

    return territoriesToAttack;
}

// 
void AggressivePlayerStrategy::issueOrder(Player* player)
{
    int oldNumberOfOrders = player->orders_->size();
    vector<Territory*> territoriesToAttack = toAttack(player);
    vector<Territory*> territoriesToDefend = toDefend(player);

    bool finishedDeploying = deployToTopTerritory(player, territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedAttacking = attackFromTopTerritory(player, territoriesToDefend.front(), territoriesToAttack);
        if (finishedAttacking)
        {
            bool finishedFortifying = fortifyTopTerritory(player, territoriesToDefend);
            player->committed_ = finishedFortifying;
        }
    }

    if (oldNumberOfOrders == player->orders_->size())
    {
        cout << "No new order issued." << endl;
    }
}

// Deploy all reinfocements to the strongest territory (the one with the most armies already present)
bool AggressivePlayerStrategy::deployToTopTerritory(Player* player, vector<Territory*> territoriesToDefend)
{
    if (player->reinforcements_ == 0)
    {
        return true;
    }

    Territory* territory = territoriesToDefend.front();
    DeployOrder* order = new DeployOrder(player, player->reinforcements_, territory);
    player->addOrder(order);
    territory->addPendingIncomingArmies(player->reinforcements_);
    player->reinforcements_ = 0;

    cout << "Issued: " << *order << endl;
    return false;
}

// Advance armies from strongest territory to enemy territories
bool AggressivePlayerStrategy::attackFromTopTerritory(Player* player, Territory* attackFrom, vector<Territory*> territoriesToAttack)
{
    Map* map = GameEngine::getMap();

    for (auto &territory : map->getAdjacentTerritories(attackFrom))
    {
        bool isEnemyTerritory = find(territoriesToAttack.begin(), territoriesToAttack.end(), territory) != territoriesToAttack.end();
        if (isEnemyTerritory && !player->advancePairingExists(attackFrom, territory))
        {
            // Only advance to this enemy territory if the player has any chance of conquering it
            int movableArmies = attackFrom->getNumberOfArmies() + attackFrom->getPendingIncomingArmies() - attackFrom->getPendingOutgoingArmies();
            int minimumArmiesRequiredToWin = max((int)ceil(territory->getNumberOfArmies() / 0.6), 1);
            if (movableArmies > 0)
            {
                int armiesToMove = min(movableArmies, minimumArmiesRequiredToWin);
                AdvanceOrder* order = new AdvanceOrder(player, armiesToMove, attackFrom, territory);
                player->addOrder(order);
                attackFrom->addPendingOutgoingArmies(armiesToMove);
                player->issuedDeploymentsAndAdvancements_[attackFrom].push_back(territory);
                
                cout << "Issued: " << *order << endl;
                return false;
            }
        }
    }

    return true;
}

// Advance armies from other adjacent territories to the strongest territory
bool AggressivePlayerStrategy::fortifyTopTerritory(Player* player, vector<Territory*> territoriesToDefend)
{
    Map* map = GameEngine::getMap();
    Territory* territoryToFortify = territoriesToDefend.front();

    for (auto &territory : map->getAdjacentTerritories(territoryToFortify))
    {
        bool isFriendlyTerritory = find(territoriesToDefend.begin(), territoriesToDefend.end(), territory) != territoriesToDefend.end();
        if (isFriendlyTerritory && !player->advancePairingExists(territory, territoryToFortify))
        {
            int movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
            if (movableArmies > 0)
            {
                AdvanceOrder* order = new AdvanceOrder(player, movableArmies, territory, territoryToFortify);
                player->addOrder(order);
                territory->addPendingOutgoingArmies(movableArmies);
                player->issuedDeploymentsAndAdvancements_[territory].push_back(territoryToFortify);
                
                cout << "Issued: " << *order << endl;
                return false;
            }
        }
    }

    return true;
}


/* 
===================================
 Implementation for BenevolentPlayerStrategy class
===================================
 */

// Operator overloading
ostream &BenevolentPlayerStrategy::print_(ostream &output) const
{
    output << "[BenevolentPlayerStrategy]";
    return output;
}

// Return a pointer to a new instance of BenevolentPlayerStrategy.
PlayerStrategy* BenevolentPlayerStrategy::clone() const
{
    return new BenevolentPlayerStrategy();
}

// Return a list of territories to defend
vector<Territory*> BenevolentPlayerStrategy::toDefend(const Player* player) const
{
    vector<Territory*> territoriesToDefend = player->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() < t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return an empty list of territories to attack (since this strategy never attacks)
vector<Territory*> BenevolentPlayerStrategy::toAttack(const Player* player) const
{
    return {};
}

// 
void BenevolentPlayerStrategy::issueOrder(Player* player)
{
    int oldNumberOfOrders = player->orders_->size();
    vector<Territory*> territoriesToDefend = toDefend(player);
    
    bool finishedDeploying = deployToWeakTerritories(player, territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedFortifying = fortifyWeakTerritories(player, territoriesToDefend);
        player->committed_ = finishedFortifying;
    }

    if (oldNumberOfOrders == player->orders_->size())
    {
        cout << "No new order issued." << endl;
    }
}

// Deploy reinforcements to the weakest territories (i.e. those that have fewest armies)
bool BenevolentPlayerStrategy::deployToWeakTerritories(Player* player, vector<Territory*> territoriesToDefend)
{
    if (player->reinforcements_ == 0)
    {
        return true;
    }

    const int DEFAULT_ARMIES_TO_DEPLOY = 5;
    int armiesToDeploy = min(DEFAULT_ARMIES_TO_DEPLOY, player->reinforcements_);


    // In order of highest priority, try to find a territory that hasn't already been deployed to
    Territory* destination = nullptr;
    auto iterator = territoriesToDefend.begin();
    for (; iterator != territoriesToDefend.end(); iterator++)
    {
        if (player->issuedDeploymentsAndAdvancements_.find(*iterator) == player->issuedDeploymentsAndAdvancements_.end())
        {
            // Initialize the vector at the key specified by this territory
            player->issuedDeploymentsAndAdvancements_[*iterator];
            destination = *iterator;
            break;
        }
    }

    // If all territories in `territoriesToDefend` have already been deployed to, then just pick the first one (highest priority)
    if (destination == nullptr)
    {
        destination = territoriesToDefend.front();
    }

    DeployOrder* order = new DeployOrder(player, armiesToDeploy, destination);
    player->addOrder(order);
    destination->addPendingIncomingArmies(armiesToDeploy);
    player->reinforcements_ -= armiesToDeploy;
    
    cout << "Issued: " << *order << endl;
    return false;
}

// Advance armies from other adjacent territories to the weaker territories
bool BenevolentPlayerStrategy::fortifyWeakTerritories(Player* player, vector<Territory*> territoriesToDefend)
{
    Map* map = GameEngine::getMap();
    for (const auto &territory : territoriesToDefend)
    {
        int movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        if (movableArmies > 1)
        {
            vector<Territory*> adjacentTerritories = map->getAdjacentTerritories(territory);
            auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() < t2->getNumberOfArmies(); };
            sort(adjacentTerritories.begin(), adjacentTerritories.end(), sortLambda);

            for (const auto &neighbor : adjacentTerritories)
            {
                bool isFriendlyTerritory = find(territoriesToDefend.begin(), territoriesToDefend.end(), neighbor) != territoriesToDefend.end();
                if (isFriendlyTerritory && !player->advancePairingExists(territory, neighbor))
                {
                    int armiesToMove = movableArmies / 2;
                    AdvanceOrder* order = new AdvanceOrder(player, armiesToMove, territory, neighbor);
                    player->addOrder(order);
                    territory->addPendingOutgoingArmies(armiesToMove);
                    player->issuedDeploymentsAndAdvancements_[territory].push_back(neighbor);
                    
                    cout << "Issued: " << *order << endl;
                    return false;
                }
            }
        }
    }

    return true;
}


/* 
===================================
 Implementation for NeutralPlayerStrategy class
===================================
 */

// Operator overloading
ostream &NeutralPlayerStrategy::print_(ostream &output) const
{
    output << "[NeutralPlayerStrategy]";
    return output;
}

// Return a pointer to a new instance of NeutralPlayerStrategy.
PlayerStrategy* NeutralPlayerStrategy::clone() const
{
    return new NeutralPlayerStrategy();
}

// Return an empty list of territories to defend
vector<Territory*> NeutralPlayerStrategy::toDefend(const Player* player) const
{
    return {};
}

// Return an empty list of territories to attack
vector<Territory*> NeutralPlayerStrategy::toAttack(const Player* player) const
{
    return {};
}

// Do nothing when called to issue order
void NeutralPlayerStrategy::issueOrder(Player* player)
{
    return;
}
