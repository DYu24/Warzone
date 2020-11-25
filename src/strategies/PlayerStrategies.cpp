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

// Constructors
PlayerStrategy::PlayerStrategy() : player_(nullptr) {}

PlayerStrategy::PlayerStrategy(Player* player) : player_(player) {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy &strategy) : player_(strategy.player_) {}

// Operator overloading
const PlayerStrategy &PlayerStrategy::operator=(const PlayerStrategy &strategy)
{
    player_ = strategy.player_;
    return *this;
}

ostream &operator<<(ostream &output, const PlayerStrategy &strategy)
{
    return strategy.print_(output);
}


/* 
===================================
 Implementation for AggressivePlayerStrategy class
===================================
 */

// Constructors
AggressivePlayerStrategy::AggressivePlayerStrategy() : PlayerStrategy() {};

AggressivePlayerStrategy::AggressivePlayerStrategy(Player* player) : PlayerStrategy(player) {}

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy &strategy) : PlayerStrategy(strategy) {}

// Operator overloading
const AggressivePlayerStrategy &AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy &strategy)
{
    PlayerStrategy::operator=(strategy);
    return *this;
}

ostream &AggressivePlayerStrategy::print_(ostream &output) const
{
    output << "[AggressivePlayerStrategy]";
    return output;
}

// Return a list of territories to defend
vector<Territory*> AggressivePlayerStrategy::toDefend() const
{
    vector<Territory*> territoriesToDefend = player_->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() > t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return a list of territories to attack
vector<Territory*> AggressivePlayerStrategy::toAttack() const
{
    vector<Territory*> sources = toDefend();
    vector<Territory*> territoriesToAttack;
    unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (const auto &territory : sources)
    {
        vector<Territory*> adjacentTerritories = map->getAdjacentTerritories(territory);
        auto sortLambda = [this](auto t1, auto t2){ return compareTerritoriesByEnemiesAndArmies(t1, t2, this->player_); };
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
void AggressivePlayerStrategy::issueOrder()
{
    vector<Territory*> territoriesToAttack = toAttack();
    vector<Territory*> territoriesToDefend = toDefend();

    bool finishedDeploying = deployToTopTerritory(territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedAttacking = attackFromTopTerritory(territoriesToDefend.front(), territoriesToAttack);
        if (finishedAttacking)
        {
            bool finishedFortifying = fortifyTopTerritory(territoriesToDefend);
        }
    }
}

// Deploy all reinfocements to the strongest territory (the one with the most armies already present)
bool AggressivePlayerStrategy::deployToTopTerritory(vector<Territory*> territoriesToDefend)
{
    if (player_->reinforcements_ == 0)
    {
        return true;
    }

    Territory* territory = territoriesToDefend.front();
    territory->addArmies(player_->reinforcements_);
    player_->reinforcements_ = 0;

    return false;
}

// Advance armies from strongest territory to enemy territories
bool AggressivePlayerStrategy::attackFromTopTerritory(Territory* attackFrom, vector<Territory*> territoriesToAttack)
{
    Map* map = GameEngine::getMap();

    for (auto &territory : map->getAdjacentTerritories(attackFrom))
    {
        bool isEnemyTerritory = find(territoriesToAttack.begin(), territoriesToAttack.end(), territory) != territoriesToAttack.end();
        if (isEnemyTerritory && !player_->advancePairingExists(attackFrom, territory))
        {
            // Only advance to this enemy territory if the player has any chance of conquering it
            int movableArmies = attackFrom->getNumberOfArmies() + attackFrom->getPendingIncomingArmies() - attackFrom->getPendingOutgoingArmies();
            int minimumArmiesRequiredToWin = (int)ceil(territory->getNumberOfArmies() / 0.6);
            if (movableArmies > 0)
            {
                int armiesToMove = min(movableArmies, minimumArmiesRequiredToWin);
                AdvanceOrder* order = new AdvanceOrder(player_, armiesToMove, attackFrom, territory);
                player_->addOrder(order);
                attackFrom->addPendingOutgoingArmies(armiesToMove);
                player_->issuedDeploymentsAndAdvancements_[attackFrom].push_back(territory);
                
                cout << "Issued: " << *order << endl;
                return false;
            }
        }
    }

    return true;
}

// Advance armies from other adjacent territories to the strongest territory
bool AggressivePlayerStrategy::fortifyTopTerritory(vector<Territory*> territoriesToDefend)
{
    Map* map = GameEngine::getMap();
    Territory* territoryToFortify = territoriesToDefend.front();

    for (auto &territory : map->getAdjacentTerritories(territoryToFortify))
    {
        bool isFriendlyTerritory = find(territoriesToDefend.begin(), territoriesToDefend.end(), territory) != territoriesToDefend.end();
        if (isFriendlyTerritory && !player_->advancePairingExists(territory, territoryToFortify))
        {
            int movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
            if (movableArmies > 0)
            {
                AdvanceOrder* order = new AdvanceOrder(player_, movableArmies, territory, territoryToFortify);
                player_->addOrder(order);
                territory->addPendingOutgoingArmies(movableArmies);
                player_->issuedDeploymentsAndAdvancements_[territory].push_back(territoryToFortify);
                
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

// Constructors
BenevolentPlayerStrategy::BenevolentPlayerStrategy() : PlayerStrategy() {};

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player* player) : PlayerStrategy(player) {}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy &strategy) : PlayerStrategy(strategy) {}

// Operator overloading
const BenevolentPlayerStrategy &BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy &strategy)
{
    PlayerStrategy::operator=(strategy);
    return *this;
}

ostream &BenevolentPlayerStrategy::print_(ostream &output) const
{
    output << "[BenevolentPlayerStrategy]";
    return output;
}

// Return a list of territories to defend
vector<Territory*> BenevolentPlayerStrategy::toDefend() const
{
    vector<Territory*> territoriesToDefend = player_->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() < t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return an empty list of territories to attack (since this strategy never attacks)
vector<Territory*> BenevolentPlayerStrategy::toAttack() const
{
    return {};
}

// 
void BenevolentPlayerStrategy::issueOrder()
{
    vector<Territory*> territoriesToDefend = toDefend();
    bool finishedDeploying = deployToWeakTerritories(territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedFortifying = fortifyWeakTerritories(territoriesToDefend);
    }
}

// Deploy reinforcements to the weakest territories (i.e. those that have fewest armies)
bool BenevolentPlayerStrategy::deployToWeakTerritories(vector<Territory*> territoriesToDefend)
{
    if (player_->reinforcements_ == 0)
    {
        return true;
    }

    const int DEFAULT_ARMIES_TO_DEPLOY = 5;
    int armiesToDeploy = min(DEFAULT_ARMIES_TO_DEPLOY, player_->reinforcements_);


    // In order of highest priority, try to find a territory that hasn't already been deployed to
    Territory* destination = nullptr;
    auto iterator = territoriesToDefend.begin();
    for (; iterator != territoriesToDefend.end(); iterator++)
    {
        if (player_->issuedDeploymentsAndAdvancements_.find(*iterator) == player_->issuedDeploymentsAndAdvancements_.end())
        {
            // Initialize the vector at the key specified by this territory
            player_->issuedDeploymentsAndAdvancements_[*iterator];
            destination = *iterator;
            break;
        }
    }

    // If all territories in `territoriesToDefend` have already been deployed to, then just pick the first one (highest priority)
    if (destination == nullptr)
    {
        destination = territoriesToDefend.front();
    }

    DeployOrder* order = new DeployOrder(player_, armiesToDeploy, destination);
    player_->addOrder(order);
    destination->addPendingIncomingArmies(armiesToDeploy);
    player_->reinforcements_ -= armiesToDeploy;
    
    cout << "Issued: " << *order << endl;
    return false;
}

// Advance armies from other adjacent territories to the weaker territories
bool BenevolentPlayerStrategy::fortifyWeakTerritories(vector<Territory*> territoriesToDefend)
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
                if (isFriendlyTerritory && !player_->advancePairingExists(territory, neighbor))
                {
                    int armiesToMove = movableArmies / 2;
                    AdvanceOrder* order = new AdvanceOrder(player_, armiesToMove, territory, neighbor);
                    player_->addOrder(order);
                    territory->addPendingOutgoingArmies(armiesToMove);
                    player_->issuedDeploymentsAndAdvancements_[territory].push_back(neighbor);
                    
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

// Constructors
NeutralPlayerStrategy::NeutralPlayerStrategy() : PlayerStrategy() {};

NeutralPlayerStrategy::NeutralPlayerStrategy(Player* player) : PlayerStrategy(player) {}

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy &strategy) : PlayerStrategy(strategy) {}

// Operator overloading
const NeutralPlayerStrategy &NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy &strategy)
{
    PlayerStrategy::operator=(strategy);
    return *this;
}

ostream &NeutralPlayerStrategy::print_(ostream &output) const
{
    output << "[NeutralPlayerStrategy]";
    return output;
}

// Return an empty list of territories to defend
vector<Territory*> NeutralPlayerStrategy::toDefend() const
{
    return {};
}

// Return an empty list of territories to attack
vector<Territory*> NeutralPlayerStrategy::toAttack() const
{
    return {};
}

// Do nothing when called to issue order
void NeutralPlayerStrategy::issueOrder()
{
    return;
}
