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
    vector<Territory*> territoriesToAttack = toAttack(player);
    vector<Territory*> territoriesToDefend = toDefend(player);

    bool finishedDeploying = deployToTopTerritory(player, territoriesToDefend.front());
    if (finishedDeploying)
    {
        bool finishedPlayingCards = playCard(player, territoriesToDefend.front());

        if (finishedPlayingCards)
        {
            bool finishedAttacking = attackFromTopTerritory(player, territoriesToDefend.front(), territoriesToAttack);
            if (finishedAttacking)
            {
                bool finishedIssuingOrders = advanceToRandomTerritory(player, territoriesToDefend);
                player->committed_ = finishedIssuingOrders;
            }
        }

    }
}

// Deploy all reinfocements to the strongest territory (the one with the most armies already present)
bool AggressivePlayerStrategy::deployToTopTerritory(Player* player, Territory* territory)
{
    if (player->reinforcements_ == 0)
    {
        return true;
    }

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

    for (const auto &territory : map->getAdjacentTerritories(attackFrom))
    {
        bool isEnemyTerritory = find(territoriesToAttack.begin(), territoriesToAttack.end(), territory) != territoriesToAttack.end();
        if (isEnemyTerritory && !player->advancePairingExists(attackFrom, territory))
        {
            int movableArmies = attackFrom->getNumberOfMovableArmies();
            if (movableArmies > 0)
            {
                AdvanceOrder* order = new AdvanceOrder(player, movableArmies, attackFrom, territory);
                player->addOrder(order);
                attackFrom->addPendingOutgoingArmies(movableArmies);
                player->issuedDeploymentsAndAdvancements_[attackFrom].push_back(territory);
                
                cout << "Issued: " << *order << endl;
                return false;
            }
        }
    }

    return true;
}

// Advance armies from the top/strongest territory to another adjacent friendly territory.
// This will ensure that the player doesn't get stuck deploying to the top territory indefinitely
// when the territory doesn't have any surrounding enemy territories to attack.
bool AggressivePlayerStrategy::advanceToRandomTerritory(Player* player, vector<Territory*> territoriesToDefend)
{
    Territory* topTerritory = territoriesToDefend.front();
    int movableArmies = topTerritory->getNumberOfMovableArmies();

    // If the player hasn't already moved all the armies to attack an enemy, move to another friendly territory
    if (movableArmies > 0)
    {
        vector<Territory*> adjacentTerritories = GameEngine::getMap()->getAdjacentTerritories(topTerritory);
        
        // Pick random destination
        srand(time(nullptr));
        int randomIndex = rand() % adjacentTerritories.size();
        Territory* destination = adjacentTerritories.at(randomIndex);
        
        AdvanceOrder* order = new AdvanceOrder(player, movableArmies, topTerritory, destination);
        player->addOrder(order);
        topTerritory->addPendingOutgoingArmies(movableArmies);
        player->issuedDeploymentsAndAdvancements_[topTerritory].push_back(destination);
        
        cout << "Issued: " << *order << endl;
        return false;
    }

    return true;
}

// Helper method to play a random Card from the Player's hand, if any.
// Returns `true` if the Player has no more cards to play.
// Returns `false` if the Player (may) still have more cards to play.
bool AggressivePlayerStrategy::playCard(Player* player, Territory* topTerritory)
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
    cout << "Played: " << *card << endl;

    // Return the played card back to the deck
    card->setOwner(nullptr);
    GameEngine::getDeck()->addCard(card);

    if (order != nullptr)
    {
        player->addOrder(order);
        cout << "Issued: " << *order << endl;
    }
    else if (player->reinforcements_ > 0)
    {
        // Reinforcement card played: deploy the additional reinforcements
        deployToTopTerritory(player, topTerritory);
    }

    return false;
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
    vector<Territory*> territoriesToDefend = toDefend(player);
    
    bool finishedDeploying = deployToWeakTerritories(player, territoriesToDefend);
    if (finishedDeploying)
    {
        bool finishedPlayingCards = playCard(player, territoriesToDefend);

        if (finishedPlayingCards)
        {
            bool finishedFortifying = fortifyWeakTerritories(player, territoriesToDefend);
            player->committed_ = finishedFortifying;
        }
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
        int movableArmies = territory->getNumberOfMovableArmies();
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

// Helper method to play a non-aggressive Card from the Player's hand, if any.
// Returns `true` if the Player has no more non-aggressive cards to play.
// Returns `false` if the Player (may) still have more cards to play.
bool BenevolentPlayerStrategy::playCard(Player* player, vector<Territory*> territoriesToDefend)
{
    Hand* playerHand = player->hand_;

    if (playerHand->size() != 0)
    {
        // Play a card from hand (except for BombCards)
        Card* card = nullptr;
        for (int i = 0; i < playerHand->size(); i++)
        {
            // Put the card back in the hand if it's a BombCard
            if (dynamic_cast<BombCard*>(playerHand->at(i)))
            {
                continue;
            }
            
            card = playerHand->removeCard(i);
            break;
        }

        if (card != nullptr)
        {
            Order* order = card->play();
            cout << "Played: " << *card << endl;

            // Return the played card back to the deck
            card->setOwner(nullptr);
            GameEngine::getDeck()->addCard(card);

            if (order != nullptr)
            {
                player->addOrder(order);
                cout << "Issued: " << *order << endl << endl;
            }
            else if (player->reinforcements_ > 0)
            {
                // Reinforcement card played: deploy the additional reinforcements
                deployToWeakTerritories(player, territoriesToDefend);
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
ostream &HumanPlayerStrategy::print_(ostream &output) const
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
vector<Territory*> HumanPlayerStrategy::toDefend(const Player* player) const
{
    return player->ownedTerritories_;
}

// Return a list of territories to attack
vector<Territory*> HumanPlayerStrategy::toAttack(const Player* player) const
{
    vector<Territory*> ownedTerritories = player->ownedTerritories_;
    vector<Territory*> territoriesToAttack;
    unordered_set<Territory*> territoriesSeen;
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

// 
void HumanPlayerStrategy::issueOrder(Player* player)
{
    vector<Territory*> territoriesToAttack = toAttack(player);
    vector<Territory*> territoriesToDefend = toDefend(player);

    if (player->reinforcements_ > 0)
    {
        deployReinforcements(player, territoriesToDefend);
    }
    else
    {
        cout << "What would you like to do?" << endl;
        if (player->getOwnTerritoriesWithMovableArmies().size() > 0)
        {
            cout << "[A] Advance" << endl;
        }
        if (player->hand_->size() > 0)
        {
            cout << "[P] Play a card" << endl;
        }
        cout << "[X] Commit" << endl;

        while (true)
        {
            string selection;
            cin >> selection;
            transform(selection.begin(), selection.end(), selection.begin(), ::toupper);

            if (selection == "A")
            {
                issueAdvance(player, territoriesToDefend);
                break;
            }
            else if (selection == "P")
            {
                playCard(player, territoriesToDefend);
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
void HumanPlayerStrategy::deployReinforcements(Player* player, vector<Territory*> territoriesToDefend)
{
    cout << "You have " << player->reinforcements_ << " reinforcements left." << endl;
    cout << "\nWhere would you like to deploy to?" << endl;
    for (int i = 0; i < territoriesToDefend.size(); i++)
    {
        Territory* territory = territoriesToDefend.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " present, " << territory->getPendingIncomingArmies() << " pending)" << endl;
    }

    Territory* deployTarget = nullptr;
    cout << "\nEnter the territory to deploy to: ";
    while (deployTarget == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= territoriesToDefend.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        deployTarget = territoriesToDefend.at(selection - 1);
    }

    int armiesToDeploy = 0;
    cout << "\nHow many reinforcements do you want to deploy?" << endl;
    while (armiesToDeploy == 0)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection < 1 || selection > player->reinforcements_)
        {
            cout << "Please enter a number between 1 and " << player->reinforcements_ << ": " << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        armiesToDeploy = selection;
    }

    DeployOrder* order = new DeployOrder(player, armiesToDeploy, deployTarget);
    player->addOrder(order);
    deployTarget->addPendingIncomingArmies(armiesToDeploy);
    player->reinforcements_ -= armiesToDeploy;
    
    cout << "Issued: " << *order << endl << endl;
}

// Issue an advance order to either fortify or attack a territory
void HumanPlayerStrategy::issueAdvance(Player* player, vector<Territory*> territoriesToDefend)
{
    vector<Territory*> possibleSources = player->getOwnTerritoriesWithMovableArmies();

    cout << "\nWhich territory would you like to advance from?" << endl;
    for (int i = 0; i < possibleSources.size(); i++)
    {
        Territory* territory = possibleSources.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfMovableArmies() << " armies available)" << endl;
    }

    Territory* source = nullptr;
    cout << "\nEnter the territory to advance from: ";
    while (source == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= possibleSources.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        source = possibleSources.at(selection - 1);
    }

    Map* map = GameEngine::getMap();
    vector<Territory*> attackable;
    vector<Territory*> defendable;
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
    cout << "\nWhich territory would you like to advance to?" << endl;
    if (!defendable.empty())
    {
        cout << "~~~ To Defend ~~~" << endl;
        for (; i < defendable.size(); i++)
        {
            Territory* territory = defendable.at(i);
            cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << endl;
        }
    }
    if (!attackable.empty())
    {
        cout << "\n~~~ To Attack ~~~" << endl;
        for (; i < defendable.size() + attackable.size(); i++)
        {
            int idx = i - defendable.size();
            Territory* territory = attackable.at(idx);
            cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << endl;
        }
    }

    Territory* destination = nullptr;
    cout << "\nEnter the territory to advance to: ";
    while (destination == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= defendable.size() + attackable.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    cout << "\nHow many armies do you want to move?" << endl;
    while (armiesToMove == 0)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection < 1 || selection > movableArmies)
        {
            cout << "Please enter a number between 1 and " << movableArmies << ": " << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        armiesToMove = selection;
    }

    AdvanceOrder* order = new AdvanceOrder(player, armiesToMove, source, destination);
    player->addOrder(order);
    source->addPendingOutgoingArmies(armiesToMove);
    
    cout << "Issued: " << *order << endl << endl;
}

// Play a card from the player's hand
void HumanPlayerStrategy::playCard(Player* player, vector<Territory*> territoriesToDefend)
{
    Hand* playerHand = player->hand_;

    cout << "\nWhich card would you like to play?" << endl;
    for (int i = 0; i < playerHand->size(); i++)
    {
        Card* card = playerHand->getCards().at(i);
        cout << "[" << i+1 << "] " << *card << endl;
    }

    Card* card = nullptr;
    cout << "\nEnter the card to play: ";
    while (card == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= playerHand->size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
        cout << "Issued: " << *order << endl << endl;
    }
    else if (player->reinforcements_ > 0)
    {
        deployReinforcements(player, territoriesToDefend);
    }
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
