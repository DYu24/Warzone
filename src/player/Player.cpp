#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

namespace
{
    // Build a DeployOrder from user input
    unique_ptr<DeployOrder> buildDeployOrder(Player &player)
    {
        vector<Territory*> playerTerritories = player.getOwnedTerritories();
        Territory* selectedTerritory = NULL;

        while (true)
        {
            cout << "Which territory do you want to deploy to:" << endl;
            for (int i = 0; i < playerTerritories.size(); i++)
            {
                Territory territory = *playerTerritories.at(i);
                cout << "[" << i + 1 << "] " << territory.getName() << " (" << territory.getNumberOfArmies() << " armies present)" << endl;
            }

            int option;
            cin >> option;

            if (option - 1 < 0 || option - 1 >= playerTerritories.size())
            {
                cout << "Invalid territory option!" << endl;
                continue;
            }

            selectedTerritory = playerTerritories.at(option - 1);
            break;
        }

        cout << "How many armies do you want to deploy: ";

        int armies;
        cin >> armies;

        return make_unique<DeployOrder>(armies, selectedTerritory);
    }

    // Build an AdvanceOrder from user input
    unique_ptr<AdvanceOrder> buildAdvanceOrder(Player &player)
    {
        vector<Territory*> playerTerritories = player.getOwnedTerritories();

        Territory* source = NULL;
        Territory* target = NULL;

        while (true)
        {
            cout << "Which territory do you want to advance from:" << endl;
            for (int i = 0; i < playerTerritories.size(); i++)
            {
                Territory territory = *playerTerritories.at(i);
                cout << "[" << i + 1 << "] " << territory.getName() << " (" << territory.getNumberOfArmies() << " armies present)" << endl;
            }

            int option;
            cin >> option;

            if (option - 1 < 0 || option - 1 >= playerTerritories.size())
            {
                cout << "Invalid territory option!" << endl;
                continue;
            }

            source = playerTerritories.at(option - 1);
            break;
        }

        vector<shared_ptr<Territory>> neighboring = source->getAdjacentTerritories();
        while (true)
        {
            cout << "Which territory do you want to advance to:" << endl;
            int i = 0;
            for (auto const &territory : neighboring)
            {
                cout << "[" << i + 1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << endl;
                i++;
            }

            int option;
            cin >> option;

            if (option - 1 < 0 || option - 1 >= neighboring.size())
            {
                cout << "Invalid territory option!" << endl;
                continue;
            }

            target = neighboring.at(option - 1).get();
            break;
        }

        cout << "How many armies do you want to move: ";

        int armies;
        cin >> armies;

        return make_unique<AdvanceOrder>(armies, source, target);
    }
}

// Default constructor
Player::Player() : reinforcements_(0), name_("unknown_player"), orders_(make_unique<OrdersList>()), hand_(make_unique<Hand>()) {}

// Constructor
Player::Player(string name) : reinforcements_(0), name_(name), orders_(make_unique<OrdersList>()), hand_(make_unique<Hand>()) {}

// Copy constructor
Player::Player(const Player &player) : reinforcements_(player.reinforcements_), name_(player.name_), ownedTerritories_(player.ownedTerritories_), orders_(make_unique<OrdersList>(*player.orders_)), hand_(make_unique<Hand>(*player.hand_)) {}

// Assignment operator overloading
const Player &Player::operator=(const Player &player)
{
    reinforcements_ = player.reinforcements_;
    name_ = player.name_;
    ownedTerritories_ = player.ownedTerritories_;
    orders_ = make_unique<OrdersList>(*player.orders_);
    hand_ = make_unique<Hand>(*player.hand_);
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Player &player)
{
    output << "[Player] " << player.name_ << " has " << player.reinforcements_ << " reinforcements, " << player.ownedTerritories_.size() << " Territories, " << player.orders_->getOrders().size() << " Orders, " << player.hand_->getCards().size() << " cards in Hand";
    return output;
}

// Getters
vector<Territory*> Player::getOwnedTerritories()
{
    vector<Territory*> territories;
    for (auto const &territory : ownedTerritories_)
    {
        territories.push_back(territory.get());
    }
    return territories;
}

string Player::getName()
{
    return name_;
}

vector<shared_ptr<Player>> Player::getDiplomaticRelations()
{
    return diplomaticRelations_;
}

// Add a number of reinforcements to the Player's reinforcement pool
void Player::setReinforcements(int reinforcements)
{
    reinforcements_ = reinforcements;
}

// Add a card to the Player's hand
void Player::addCardToHand(shared_ptr<Card> card)
{
    hand_->addCard(card.get());
}

// Add a territory to the Player's list of owned territories
void Player::addOwnedTerritory(shared_ptr<Territory> territory)
{
    ownedTerritories_.push_back(territory);
}

// Remove a territory from the Player's list of owned territories
void Player::removeOwnedTerritory(shared_ptr<Territory> territory)
{
    auto removeIterator = remove(ownedTerritories_.begin(), ownedTerritories_.end(), territory);
    ownedTerritories_.erase(removeIterator, ownedTerritories_.end());
}

// Adds a player to the list of diplomatic relations
void Player::addDiplomaticRelation(shared_ptr<Player> player)
{
    diplomaticRelations_.push_back(player);
}

// Clears the list of diplomatic relations with other players
void Player::clearDiplomaticRelations()
{
    diplomaticRelations_.clear();
}

// Return a list of territories to defend
vector<shared_ptr<Territory>> Player::toDefend()
{
    return ownedTerritories_;
}

// Return a list of territories to attack
vector<shared_ptr<Territory>> Player::toAttack()
{
    return ownedTerritories_;
}

// Return the next order to be executed from the Player's list of orders
unique_ptr<Order> Player::getNextOrder()
{
    orders_->popTopOrder();
    return make_unique<DeployOrder>();
}

// Create an Order and place it in the Player's list of orders
void Player::issueOrder()
{
    // orders_->add(buildDeployOrder(*this));
    // orders_->add(buildAdvanceOrder(*this));
}