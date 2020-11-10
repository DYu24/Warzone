#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <unordered_set>

namespace
{
    /*
     * Get a valid input from the user when entering a single number.
     */
    int getValidUserInput(int restriction, string message)
    {
        bool validInput = false;
        int input;
    
        while (!validInput)
        {
            cin >> input;

            if (cin.fail() || input - 1 < 0 || input - 1 >= restriction)
            {
                cout << message << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            validInput = true;
        }

        return input;
    }

    /*
     * Custom comparator to sort Territories by their number of armies. 
     */
    bool compareTerritories(Territory* t1, Territory* t2)
    {
        return t1->getNumberOfArmies() < t2->getNumberOfArmies();
    }

    /*
     * Custom comparator to sort DeployOrders by the number of armies on their destination Territory.
     * This is to ensure that the order of issued DeployOrders corresponds to the order
     * of territories returned by `toDefend()`.
     */
    bool compareDeployOrders(DeployOrder* o1, DeployOrder* o2)
    {
        return o1->getDestination().getNumberOfArmies() < o2->getDestination().getNumberOfArmies();
    }
}

// Default constructor
Player::Player() : reinforcements_(0), name_("unknown_player"), orders_(new OrdersList()), hand_(new Hand()), map_(NULL) {}

// Constructor
Player::Player(string name, Map* map) : reinforcements_(0), name_(name), orders_(new OrdersList()), hand_(new Hand()), map_(map) {}

// Copy constructor
Player::Player(const Player &player)
    : reinforcements_(player.reinforcements_), name_(player.name_), ownedTerritories_(player.ownedTerritories_), orders_(new OrdersList(*player.orders_)),
    hand_(new Hand(*player.hand_)), diplomaticRelations_(player.diplomaticRelations_), map_(player.map_) {}

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
    map_ = player.map_;
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

// Add a card to the Player's hand
void Player::addCardToHand(Card* card)
{
    hand_->addCard(card);
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

// Return a list of territories to defend
vector<Territory*> Player::toDefend()
{
    cout << "Which territories would you like to defend?\n" << endl;
    for (int i = 0; i < ownedTerritories_.size(); i++)
    {
        cout << "[" << i + 1 << "] " << ownedTerritories_.at(i)->getName() << " (" << ownedTerritories_.at(i)->getNumberOfArmies() << " armies)" << endl;
    }

    cin.ignore();
    vector<Territory*> territoriesToDefend;
    while (true)
    {
        bool badInput = false;

        string decisions;
        getline(cin, decisions);

        unordered_set<int> indexesSeen;
        istringstream ss(decisions);
        for (int idx; ss >> idx;)
        {
            if (idx - 1 < 0 || idx - 1 >= ownedTerritories_.size())
            {
                badInput = true;
                break;
            }

            if (indexesSeen.find(idx) == indexesSeen.end())
            {
                territoriesToDefend.push_back(ownedTerritories_.at(idx - 1));
                indexesSeen.insert(idx);
            }

            if (ss.peek() == ',')
            {
                ss.ignore();
            }
        }

        if ((ss.fail() && !ss.eof()) || badInput)
        {
            cout << "Invalid input. Please try again:" << endl;
            ss.clear();
            ss.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        break;
    }

    cout << endl;
    sort(territoriesToDefend.begin(), territoriesToDefend.end(), compareTerritories);
    return territoriesToDefend;
}

// Return a list of territories to attack
vector<Territory*> Player::toAttack()
{
    vector<Territory*> attackableTerritories;
    unordered_set<Territory*> territoriesSeen;

    for (auto territory : ownedTerritories_)
    {
        for (auto neighbor : map_->getAdjacentTerritories(territory))
        {
            bool ownsNeighbor = find(ownedTerritories_.begin(), ownedTerritories_.end(), neighbor) != ownedTerritories_.end();
            bool alreadySeen = territoriesSeen.find(neighbor) != territoriesSeen.end();

            if (!ownsNeighbor && !alreadySeen)
            {
                attackableTerritories.push_back(neighbor);
                territoriesSeen.insert(neighbor);
            }
        }
    }

    cout << "Which territories would you like to attack?\n" << endl;
    for (int i = 0; i < attackableTerritories.size(); i++)
    {
        cout << "[" << i + 1 << "] " << attackableTerritories.at(i)->getName() << " (" << attackableTerritories.at(i)->getNumberOfArmies() << " enemy armies)" << endl;
    }
    
    vector<Territory*> territoriesToAttack;
    while (true)
    {
        bool badInput = false;

        string decisions;
        getline(cin, decisions);

        unordered_set<int> indexesSeen;
        istringstream ss(decisions);
        for (int idx; ss >> idx;)
        {
            if (idx - 1 < 0 || idx - 1 >= attackableTerritories.size())
            {
                badInput = true;
                break;
            }

            if (indexesSeen.find(idx) == indexesSeen.end())
            {
                territoriesToAttack.push_back(attackableTerritories.at(idx - 1));
                indexesSeen.insert(idx);
            }

            if (ss.peek() == ',')
            {
                ss.ignore();
            }
        }

        if ((ss.fail() && !ss.eof()) || badInput)
        {
            cout << "Invalid input. Please try again:" << endl;
            ss.clear();
            ss.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        break;
    }
    
    cout << endl;
    sort(territoriesToAttack.begin(), territoriesToAttack.end(), compareTerritories);
    return territoriesToAttack;
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

// Create an Order and place it in the Player's list of orders
void Player::issueOrder()
{
    cout << "~~~ " << name_ << " ~~~" << endl;
    vector<Territory*> territoriesToDefend = toDefend();
    vector<Territory*> territoriesToAttack = toAttack();

    bool committed = false;
    vector<DeployOrder*> deployOrders;
    while (!committed)
    {
        cout << "What would you like to do?" << endl;

        if (reinforcements_ > 0)
        {
            cout << "[D] Deploy" << endl;
        }
        else
        {
            cout << "[A] Advance" << endl;

            if (!hand_->getCards().empty())
            {
                cout << "[C] Play a card" << endl;
            }
        }

        // if (!orders_->getOrders().empty())
        // {
        //     cout << "[M] Move an order" << endl;
        //     cout << "[R] Remove an order" << endl;
        // }

        if (reinforcements_ == 0)
        {
            cout << "[X] Commit" << endl;
        }

        cout << "--------------" << endl;

        bool validSelection = false;
        while (!validSelection)
        {
            char selection;
            cin >> selection;
            switch (toupper(selection))
            {
                case 'D':
                    validSelection = true;
                    issueDeploy(territoriesToDefend, deployOrders);
                    break;
                case 'A':
                    validSelection = true;
                    issueAdvance(territoriesToAttack, territoriesToDefend);
                    break;
                case 'C':
                    validSelection = true;
                    break;
                case 'M':
                    validSelection = true;
                    break;
                case 'R':
                    validSelection = true;
                    break;
                case 'X':
                    validSelection = true;
                    
                    for (auto order : deployOrders)
                    {
                        orders_->add(order);
                    }

                    committed = true;
                    break;
                default:
                    cout << "Please select a valid option: ";
            }
        }
    }
}

/*
 * Helper method to issue DeployOrders for the Player
 */
void Player::issueDeploy(vector<Territory*> territoriesToDefend, vector<DeployOrder*> &deployOrders)
{
    cout << "You have " << reinforcements_ << " reinforcements left." << endl;
    cout << "Select a territory to deploy your armies to: " << endl;
    for (int i = 0; i < territoriesToDefend.size(); i++)
    {
        Territory* territory = territoriesToDefend.at(i);
        cout << "[" << i + 1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " deployed";

        if (territory->getPendingIncomingArmies() > 0)
        {
            cout << ", " << territory->getPendingIncomingArmies() << " pending";
        }

        cout << ")" << endl;
    }

    int selection = getValidUserInput(territoriesToDefend.size(), "That was not a valid option. Please try again:");
    
    cout << "Enter the number of armies to deploy: ";
    int armiesToDeploy = getValidUserInput(reinforcements_, "Please enter a valid number of armies :");

    Territory* selectedTerritory = territoriesToDefend.at(selection - 1);
    auto iterator = find_if(deployOrders.begin(), deployOrders.end(), [&selectedTerritory](auto order) { return order->getDestination() == *selectedTerritory; });
    if (iterator != deployOrders.end())
    {
        DeployOrder* existingDeployOrder = *iterator;
        existingDeployOrder->addArmies(armiesToDeploy);
    }
    else
    {
        deployOrders.push_back(new DeployOrder(armiesToDeploy, selectedTerritory));
        sort(deployOrders.begin(), deployOrders.end(), compareDeployOrders);
    }

    selectedTerritory->addPendingIncomingArmies(armiesToDeploy);
    reinforcements_ -= armiesToDeploy;
}

/*
 * Helper method to issue AdvanceOrders for the Player
 */
void Player::issueAdvance(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend)
{
    vector<Territory*> sources;
    unordered_set<Territory*> sourcesSeen;

    for (auto territory : ownedTerritories_)
    {
        vector<Territory*> neighbors = map_->getAdjacentTerritories(territory);

        bool alreadySeen = sourcesSeen.find(territory) != sourcesSeen.end();
        bool hasArmiesToMove = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies() > 0;
        bool hasNeighborToAttackOrDefend = any_of(neighbors.begin(), neighbors.end(), [&territoriesToAttack, &territoriesToDefend](auto t) { 
            bool toAttack = find(territoriesToAttack.begin(), territoriesToAttack.end(), t) != territoriesToAttack.end();
            bool toDefend = find(territoriesToDefend.begin(), territoriesToDefend.end(), t) != territoriesToDefend.end();
            return toAttack || toDefend;
        });

        if (hasNeighborToAttackOrDefend && hasArmiesToMove && !alreadySeen)
        {
            sources.push_back(territory);
            sourcesSeen.insert(territory);
        }
    }

    cout << "Select a territory to advance from: " << endl;
    for (int i = 0; i < sources.size(); i++)
    {
        Territory* territory = sources.at(i);
        int maxPossibleArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        cout << "[" << i + 1 << "] " << territory->getName() << " (" << maxPossibleArmies << " armies)" << endl;
    }

    int sourceIndex = getValidUserInput(sources.size(), "That was not a valid option. Please try again:");
    Territory* sourceTerritory = sources.at(sourceIndex - 1);

    vector<Territory*> destinations;
    for (auto territory : map_->getAdjacentTerritories(sourceTerritory))
    {
        bool toAttack = find(territoriesToAttack.begin(), territoriesToAttack.end(), territory) != territoriesToAttack.end();
        bool toDefend = find(territoriesToDefend.begin(), territoriesToDefend.end(), territory) != territoriesToDefend.end();
        
        if (toAttack || toDefend)
        {
            destinations.push_back(territory);
        }
    }

    cout << "Select a territory to advance to: " << endl;
    for (int i = 0; i < destinations.size(); i++)
    {
        Territory* territory = destinations.at(i);
        cout << "[" << i + 1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies)" << endl;
    }

    int destinationIndex = getValidUserInput(destinations.size(), "That was not a valid option. Please try again:");
    Territory* destinationTerritory = destinations.at(destinationIndex - 1);

    cout << "Enter the number of armies to move: ";
    int maxPossibleArmies = sourceTerritory->getNumberOfArmies() + sourceTerritory->getPendingIncomingArmies() - sourceTerritory->getPendingOutgoingArmies();
    int armiesToMove = getValidUserInput(maxPossibleArmies, "Please enter a valid number of armies:");

    if (find(territoriesToAttack.begin(), territoriesToAttack.end(), destinationTerritory) != territoriesToAttack.end())
    {
        orders_->add(new AdvanceOrder(armiesToMove, sourceTerritory, destinationTerritory, true));
    }
    else
    {
        orders_->add(new AdvanceOrder(armiesToMove, sourceTerritory, destinationTerritory, false));
    }

    sourceTerritory->addPendingOutgoingArmies(armiesToMove);
}
