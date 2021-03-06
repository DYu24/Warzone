#include "PlayerStrategies.h"
#include "../game_engine/GameEngine.h"
#include "../orders/Orders.h"
#include <algorithm>
#include <math.h>
#include <time.h>
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
std::ostream &operator<<(std::ostream &output, const PlayerStrategy &strategy)
{
    return strategy.print_(output);
}



/* 
===================================
 Implementation for AggressivePlayerStrategy class
===================================
 */

// Operator overloading
std::ostream &AggressivePlayerStrategy::print_(std::ostream &output) const
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
std::vector<Territory*> AggressivePlayerStrategy::toDefend(const Player* player) const
{
    std::vector<Territory*> territoriesToDefend = player->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() > t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return a list of territories to attack
std::vector<Territory*> AggressivePlayerStrategy::toAttack(const Player* player) const
{
    std::vector<Territory*> sources = toDefend(player);
    std::vector<Territory*> territoriesToAttack;
    std::unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (const auto &territory : sources)
    {
        std::vector<Territory*> adjacentTerritories = map->getAdjacentTerritories(territory);
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

// Issue an order to either:
// - Deploy to strongest territory
// - Play a card from the hand
// - Attack an enemy territory with the strongest territory
// - Advance all armies from the strongest territory to another territory (if surrounded by fristd::endly territories)
void AggressivePlayerStrategy::issueOrder(Player* player)
{
    std::vector<Territory*> territoriesToAttack = toAttack(player);
    std::vector<Territory*> territoriesToDefend = toDefend(player);

    bool finishedDeploying = deployToTopTerritory_(player, territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedPlayingCards = playCard_(player, territoriesToDefend);
        if (finishedPlayingCards)
        {
            bool finishedAttacking = attackFromTopTerritory_(player, territoriesToDefend.front(), territoriesToAttack);
            if (finishedAttacking)
            {
                bool finishedIssuingOrders = advanceToRandomTerritory_(player, territoriesToDefend);
                player->committed_ = finishedIssuingOrders;
            }
        }

    }
}

// Deploy all reinforcements to the strongest territory (the one with the most armies already present).
// Returns `true` if finished deploying/no new order was issued.
// Returns `false` if there was an order issued.
bool AggressivePlayerStrategy::deployToTopTerritory_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    if (player->reinforcements_ == 0)
    {
        return true;
    }
    
    Territory* topTerritory = territoriesToDefend.front();
    DeployOrder* order = new DeployOrder(player, player->reinforcements_, topTerritory);
    player->addOrder(order);
    topTerritory->addPendingIncomingArmies(player->reinforcements_);
    player->reinforcements_ = 0;

    std::cout << "Issued: " << *order << std::endl;
    return false;
}

// Advance all armies from strongest territory to an enemy territory.
// Returns `true` if finished attacking or has no one to attack/no new order was issued.
// Returns `false` if there was an order issued.
bool AggressivePlayerStrategy::attackFromTopTerritory_(Player* player, Territory* attackFrom, std::vector<Territory*> territoriesToAttack)
{
    Map* map = GameEngine::getMap();
    int movableArmies = attackFrom->getNumberOfMovableArmies();

    if (movableArmies > 0)
    {
        for (const auto &territory : map->getAdjacentTerritories(attackFrom))
        {
            bool isEnemyTerritory = find(territoriesToAttack.begin(), territoriesToAttack.end(), territory) != territoriesToAttack.end();
            bool alreadyAdvancedToTerritory = player->advancePairingExists_(attackFrom, territory);

            if (isEnemyTerritory && !alreadyAdvancedToTerritory)
            {
                AdvanceOrder* order = new AdvanceOrder(player, movableArmies, attackFrom, territory);
                player->addOrder(order);
                attackFrom->addPendingOutgoingArmies(movableArmies);
                player->issuedDeploymentsAndAdvancements_[attackFrom].push_back(territory);
                
                std::cout << "Issued: " << *order << std::endl;
                return false;
            }
        }
    }

    return true;
}

// Advance armies from the top/strongest territory to another adjacent fristd::endly territory.
// This will ensure that the player doesn't get stuck deploying to the top territory indefinitely
// when the territory doesn't have any surrounding enemy territories to attack.
// Returns `true` if finished advanstd::cing/no new order was issued.
// Returns `false` if there was an order issued.
bool AggressivePlayerStrategy::advanceToRandomTerritory_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    Territory* topTerritory = territoriesToDefend.front();
    int movableArmies = topTerritory->getNumberOfMovableArmies();

    // If the player hasn't already moved all the armies to attack an enemy, move to another fristd::endly territory
    if (movableArmies > 0)
    {
        std::vector<Territory*> adjacentTerritories = GameEngine::getMap()->getAdjacentTerritories(topTerritory);
        
        // Pick a random destination
        srand(time(nullptr));
        int randomIndex = rand() % adjacentTerritories.size();
        Territory* destination = adjacentTerritories.at(randomIndex);
        
        AdvanceOrder* order = new AdvanceOrder(player, movableArmies, topTerritory, destination);
        player->addOrder(order);
        topTerritory->addPendingOutgoingArmies(movableArmies);
        player->issuedDeploymentsAndAdvancements_[topTerritory].push_back(destination);
        
        std::cout << "Issued: " << *order << std::endl;
        return false;
    }

    return true;
}

// Helper method to play a random Card from the Player's hand, if any.
// Returns `true` if the Player has no more cards to play/no new order was issued.
// Returns `false` if there was an order issued.
bool AggressivePlayerStrategy::playCard_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    Hand* playerHand = player->hand_;
    if (playerHand->size() == 0)
    {
        return true;
    }

    // Play a random card from hand
    int randomCardIndex = rand() % playerHand->size();
    Card* card = playerHand->removeCard(randomCardIndex);
    Order* order = card->play();
    std::cout << "Played: " << *card << std::endl;

    // Return the played card back to the deck
    card->setOwner(nullptr);
    GameEngine::getDeck()->addCard(card);

    if (order != nullptr)
    {
        player->addOrder(order);
        std::cout << "Issued: " << *order << std::endl;
    }
    else if (player->reinforcements_ > 0)
    {
        // Reinforcement card played: deploy the additional reinforcements
        deployToTopTerritory_(player, territoriesToDefend);
    }

    return false;
}



/* 
===================================
 Implementation for BenevolentPlayerStrategy class
===================================
 */

// Operator overloading
std::ostream &BenevolentPlayerStrategy::print_(std::ostream &output) const
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
std::vector<Territory*> BenevolentPlayerStrategy::toDefend(const Player* player) const
{
    std::vector<Territory*> territoriesToDefend = player->ownedTerritories_;
    auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() < t2->getNumberOfArmies(); };
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), sortLambda);
    return territoriesToDefend;
}

// Return an empty list of territories to attack (since this strategy never attacks)
std::vector<Territory*> BenevolentPlayerStrategy::toAttack(const Player* player) const
{
    return {};
}

// Issue an order to either:
// - Deploy to weakest territories
// - Play a card from the hand
// - Advance armies to weaker territories to spread the occupation of forces
void BenevolentPlayerStrategy::issueOrder(Player* player)
{
    std::vector<Territory*> territoriesToDefend = toDefend(player);
    
    bool finishedDeploying = deployToWeakTerritories_(player, territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedPlayingCards = playCard_(player, territoriesToDefend);
        if (finishedPlayingCards)
        {
            bool finishedFortifying = fortifyWeakTerritories_(player, territoriesToDefend);
            player->committed_ = finishedFortifying;
        }
    }
}

// Deploy reinforcements to the weakest territories (i.e. those that have fewest armies)
// Returns `true` if finished deploying/no new order was issued.
// Returns `false` if there was an order issued.
bool BenevolentPlayerStrategy::deployToWeakTerritories_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    if (player->reinforcements_ == 0)
    {
        return true;
    }

    const int DEFAULT_ARMIES_TO_DEPLOY = 5;
    int armiesToDeploy = std::min(DEFAULT_ARMIES_TO_DEPLOY, player->reinforcements_);


    // In order of highest priority, try to find a territory that hasn't already been deployed to
    // If all territories in `territoriesToDefend` have already been deployed to, then the first one (highest priority) will be used
    Territory* destination = territoriesToDefend.front();
    for (const auto &territory : territoriesToDefend)
    {
        if (player->issuedDeploymentsAndAdvancements_.find(territory) == player->issuedDeploymentsAndAdvancements_.end())
        {
            // Initialize the vector at the key specified by this territory
            player->issuedDeploymentsAndAdvancements_[territory];
            destination = territory;
            break;
        }
    }

    DeployOrder* order = new DeployOrder(player, armiesToDeploy, destination);
    player->addOrder(order);
    destination->addPendingIncomingArmies(armiesToDeploy);
    player->reinforcements_ -= armiesToDeploy;
    
    std::cout << "Issued: " << *order << std::endl;
    return false;
}

// Advance armies from other adjacent territories to the weaker territories
// Returns `true` if finished advanstd::cing/no new order was issued.
// Returns `false` if there was an order issued.
bool BenevolentPlayerStrategy::fortifyWeakTerritories_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    Map* map = GameEngine::getMap();
    for (const auto &territory : territoriesToDefend)
    {
        int movableArmies = territory->getNumberOfMovableArmies();
        if (movableArmies > 1)
        {
            // Sort the adjacent territories by number of occupying armies
            std::vector<Territory*> adjacentTerritories = map->getAdjacentTerritories(territory);
            auto sortLambda = [](auto t1, auto t2){ return t1->getNumberOfArmies() < t2->getNumberOfArmies(); };
            sort(adjacentTerritories.begin(), adjacentTerritories.end(), sortLambda);

            for (const auto &neighbor : adjacentTerritories)
            {
                bool isFriendlyTerritory = find(territoriesToDefend.begin(), territoriesToDefend.end(), neighbor) != territoriesToDefend.end();
                bool alreadyAdvancedToTerritory = player->advancePairingExists_(territory, neighbor);

                if (isFriendlyTerritory && !alreadyAdvancedToTerritory)
                {
                    int armiesToMove = movableArmies / 2;
                    AdvanceOrder* order = new AdvanceOrder(player, armiesToMove, territory, neighbor);
                    player->addOrder(order);
                    territory->addPendingOutgoingArmies(armiesToMove);
                    player->issuedDeploymentsAndAdvancements_[territory].push_back(neighbor);
                    
                    std::cout << "Issued: " << *order << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}

// Helper method to play a non-aggressive Card from the Player's hand, if any.
// Returns `true` if the Player has no more non-aggressive cards to play/no new order was issued.
// Returns `false` if there was an order issued.
bool BenevolentPlayerStrategy::playCard_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    Hand* playerHand = player->hand_;

    if (playerHand->size() != 0)
    {
        // Select a card from hand that isn't a BombCard
        Card* card = nullptr;
        for (int i = 0; i < playerHand->size(); i++)
        {
            if (dynamic_cast<BombCard*>(playerHand->at(i)) == nullptr)
            {
                card = playerHand->removeCard(i);
                break;
            }
        }

        if (card != nullptr)
        {
            Order* order = card->play();
            std::cout << "Played: " << *card << std::endl;

            // Return the played card back to the deck
            card->setOwner(nullptr);
            GameEngine::getDeck()->addCard(card);

            if (order != nullptr)
            {
                player->addOrder(order);
                std::cout << "Issued: " << *order << std::endl << std::endl;
            }
            else if (player->reinforcements_ > 0)
            {
                // Reinforcement card played: deploy the additional reinforcements
                deployToWeakTerritories_(player, territoriesToDefend);
            }

            return false;
        }
    }

    return true;
}



/* 
===================================
 Implementation for HumanPlayerStrategy class
===================================
 */

// Operator overloading
std::ostream &HumanPlayerStrategy::print_(std::ostream &output) const
{
    output << "[HumanPlayerStrategy]";
    return output;
}

// Return a pointer to a new instance of HumanPlayerStrategy.
PlayerStrategy* HumanPlayerStrategy::clone() const
{
    return new HumanPlayerStrategy();
}

// Return a list of territories to defend
std::vector<Territory*> HumanPlayerStrategy::toDefend(const Player* player) const
{
    return player->ownedTerritories_;
}

// Return a list of territories to attack
std::vector<Territory*> HumanPlayerStrategy::toAttack(const Player* player) const
{
    std::vector<Territory*> ownedTerritories = player->ownedTerritories_;
    std::vector<Territory*> territoriesToAttack;
    std::unordered_set<Territory*> territoriesSeen;
    Map* map = GameEngine::getMap();

    for (const auto &territory : ownedTerritories)
    {
        for (const auto &neighbor : map->getAdjacentTerritories(territory))
        {
            bool isEnemyOwned = find(ownedTerritories.begin(), ownedTerritories.end(), neighbor) == ownedTerritories.end();
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

// Issue an order based on user input
void HumanPlayerStrategy::issueOrder(Player* player)
{
    std::vector<Territory*> territoriesToAttack = toAttack(player);
    std::vector<Territory*> territoriesToDefend = toDefend(player);

    // Only allow deploy if the player still has reinforcements
    if (player->reinforcements_ > 0)
    {
        deployReinforcements_(player, territoriesToDefend);
    }
    else
    {
        std::cout << "What would you like to do?" << std::endl;
        if (player->getOwnTerritoriesWithMovableArmies().size() > 0)
        {
            std::cout << "[A] Advance" << std::endl;
        }
        if (player->hand_->size() > 0)
        {
            std::cout << "[P] Play a card" << std::endl;
        }
        std::cout << "[X] Commit" << std::endl;

        while (true)
        {
            std::string selection;
            std::cin >> selection;
            std::transform(selection.begin(), selection.end(), selection.begin(), ::toupper);

            if (selection == "A")
            {
                issueAdvance_(player, territoriesToDefend);
                break;
            }
            else if (selection == "P")
            {
                playCard_(player, territoriesToDefend);
                break;
            }
            else if (selection == "X")
            {
                player->committed_ = true;
                break;
            }
        }
    }
}

// Deploy player's reinforcements to specified territory
void HumanPlayerStrategy::deployReinforcements_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    std::cout << "You have " << player->reinforcements_ << " reinforcements left." << std::endl;
    std::cout << "\nWhere would you like to deploy to?" << std::endl;
    for (int i = 0; i < territoriesToDefend.size(); i++)
    {
        Territory* territory = territoriesToDefend.at(i);
        std::cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " present, " << territory->getPendingIncomingArmies() << " pending)" << std::endl;
    }

    Territory* deployTarget = nullptr;
    std::cout << "\nEnter the territory to deploy to: ";
    while (deployTarget == nullptr)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection - 1 < 0 || selection - 1 >= territoriesToDefend.size())
        {
            std::cout << "That was not a valid option. Please try again:" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        deployTarget = territoriesToDefend.at(selection - 1);
    }

    int armiesToDeploy = 0;
    std::cout << "How many reinforcements do you want to deploy? ";
    while (armiesToDeploy == 0)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection < 1 || selection > player->reinforcements_)
        {
            std::cout << "Please enter a number between 1 and " << player->reinforcements_ << ": " << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        armiesToDeploy = selection;
    }

    DeployOrder* order = new DeployOrder(player, armiesToDeploy, deployTarget);
    player->addOrder(order);
    deployTarget->addPendingIncomingArmies(armiesToDeploy);
    player->reinforcements_ -= armiesToDeploy;
    
    std::cout << "Issued: " << *order << std::endl << std::endl;
}

// Issue an advance order to either fortify or attack a territory
void HumanPlayerStrategy::issueAdvance_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    std::vector<Territory*> possibleSources = player->getOwnTerritoriesWithMovableArmies();

    std::cout << "\nWhich territory would you like to advance from?" << std::endl;
    for (int i = 0; i < possibleSources.size(); i++)
    {
        Territory* territory = possibleSources.at(i);
        std::cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfMovableArmies() << " armies available)" << std::endl;
    }

    Territory* source = nullptr;
    std::cout << "\nEnter the territory to advance from: ";
    while (source == nullptr)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection - 1 < 0 || selection - 1 >= possibleSources.size())
        {
            std::cout << "That was not a valid option. Please try again:" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        source = possibleSources.at(selection - 1);
    }

    // Display adjacent territories as either attackable or defendable
    Map* map = GameEngine::getMap();
    std::vector<Territory*> attackable;
    std::vector<Territory*> defendable;
    for (const auto &neighbor : map->getAdjacentTerritories(source))
    {
        if (find(territoriesToDefend.begin(), territoriesToDefend.end(), neighbor) != territoriesToDefend.end())
        {
            defendable.push_back(neighbor);
        }
        else
        {
            attackable.push_back(neighbor);
        }
    }

    int i = 0;
    std::cout << "\nWhich territory would you like to advance to?" << std::endl;
    if (!defendable.empty())
    {
        std::cout << "~~~ To Defend ~~~" << std::endl;
        for (; i < defendable.size(); i++)
        {
            Territory* territory = defendable.at(i);
            std::cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << std::endl;
        }
        std::cout << std::endl;
    }
    if (!attackable.empty())
    {
        std::cout << "~~~ To Attack ~~~" << std::endl;
        for (; i < defendable.size() + attackable.size(); i++)
        {
            int idx = i - defendable.size();
            Territory* territory = attackable.at(idx);
            std::cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << std::endl;
        }
    }

    Territory* destination = nullptr;
    std::cout << "\nEnter the territory to advance to: ";
    while (destination == nullptr)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection - 1 < 0 || selection - 1 >= defendable.size() + attackable.size())
        {
            std::cout << "That was not a valid option. Please try again:" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (selection <= defendable.size())
        {
            destination = defendable.at(selection - 1);
        }
        else
        {
            destination = attackable.at(selection - 1 - defendable.size());
        }
    }

    int armiesToMove = 0;
    int movableArmies = source->getNumberOfMovableArmies();
    std::cout << "How many armies do you want to move? ";
    while (armiesToMove == 0)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection < 1 || selection > movableArmies)
        {
            std::cout << "Please enter a number between 1 and " << movableArmies << ": " << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        armiesToMove = selection;
    }

    AdvanceOrder* order = new AdvanceOrder(player, armiesToMove, source, destination);
    player->addOrder(order);
    source->addPendingOutgoingArmies(armiesToMove);
    
    std::cout << "Issued: " << *order << std::endl << std::endl;
}

// Play a card from the player's hand
void HumanPlayerStrategy::playCard_(Player* player, std::vector<Territory*> territoriesToDefend)
{
    Hand* playerHand = player->hand_;

    std::cout << "\nWhich card would you like to play?" << std::endl;
    for (int i = 0; i < playerHand->size(); i++)
    {
        Card* card = playerHand->getCards().at(i);
        std::cout << "[" << i+1 << "] " << *card << std::endl;
    }

    Card* card = nullptr;
    std::cout << "\nEnter the card to play: ";
    while (card == nullptr)
    {
        int selection;
        std::cin >> selection;

        if (std::cin.fail() || selection - 1 < 0 || selection - 1 >= playerHand->size())
        {
            std::cout << "That was not a valid option. Please try again:" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        card = playerHand->removeCard(selection - 1);
    }

    Order* order = card->play();

    // Return the played card back to the deck
    card->setOwner(nullptr);
    GameEngine::getDeck()->addCard(card);

    if (order != nullptr)
    {
        player->addOrder(order);
        std::cout << "Issued: " << *order << std::endl << std::endl;
    }
    else if (player->reinforcements_ > 0)
    {
        deployReinforcements_(player, territoriesToDefend);
    }
}



/* 
===================================
 Implementation for NeutralPlayerStrategy class
===================================
 */

// Operator overloading
std::ostream &NeutralPlayerStrategy::print_(std::ostream &output) const
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
std::vector<Territory*> NeutralPlayerStrategy::toDefend(const Player* player) const
{
    return {};
}

// Return an empty list of territories to attack
std::vector<Territory*> NeutralPlayerStrategy::toAttack(const Player* player) const
{
    return {};
}

// Do nothing when called to issue order
void NeutralPlayerStrategy::issueOrder(Player* player)
{
    return;
}
