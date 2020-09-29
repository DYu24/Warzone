#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

namespace
{
    vector<shared_ptr<Territory>> getUnownedTerritories(vector<shared_ptr<Territory>> v1, vector<shared_ptr<Territory>> v2)
    {
        sort(v1.begin(), v1.end());
        sort(v2.begin(), v2.end());

        vector<shared_ptr<Territory>> difference;
        set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(difference, difference.begin()));

        return difference;
    }

    shared_ptr<DeployOrder> buildDeployOrder(Player &player)
    {
        vector<shared_ptr<Territory>> playerTerritories = player.getOwnedTerritories();
        shared_ptr<Territory> selectedTerritory = NULL;

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

        return make_shared<DeployOrder>();
    }

    shared_ptr<AdvanceOrder> buildAdvanceOrder(Player &player)
    {
        vector<shared_ptr<Territory>> playerTerritories = player.getOwnedTerritories();

        shared_ptr<Territory> source = NULL;
        shared_ptr<Territory> target = NULL;

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

            target = neighboring.at(option - 1);
            break;
        }

        cout << "How many armies do you want to move: ";

        int armies;
        cin >> armies;

        return make_shared<AdvanceOrder>();
    }
} // namespace

Player::Player() : name_(make_shared<string>("unknown_player")), ownedTerritories_(make_shared<vector<shared_ptr<Territory>>>()), orders_(make_shared<OrdersList>()) {}

Player::Player(shared_ptr<string> name) : name_(name), ownedTerritories_(make_shared<vector<shared_ptr<Territory>>>()), orders_(make_shared<OrdersList>()) {}

Player::Player(const Player &player) : name_(player.name_), ownedTerritories_(player.ownedTerritories_), orders_(player.orders_) {}

const Player &Player::operator=(const Player &player)
{
    ownedTerritories_ = player.ownedTerritories_;
    orders_ = player.orders_;
    return *this;
}

ostream &operator<<(ostream &output, const Player &player)
{
    output << "[Player] " << player.ownedTerritories_->size() << " Territories, " << player.orders_->getOrders().size() << " Orders";
    return output;
}

vector<shared_ptr<Territory>> Player::getOwnedTerritories()
{
    return *ownedTerritories_;
}

void Player::setOwnedTerritories(vector<shared_ptr<Territory>> &territories)
{
    *ownedTerritories_ = territories;
}

OrdersList Player::getOrdersList()
{
    return *orders_;
}

void Player::setOrdersList(OrdersList &orders)
{
    *orders_ = orders;
}

void Player::addOwnedTerritory(shared_ptr<Territory> territory)
{
    ownedTerritories_->push_back(territory);
}

void Player::removeOwnedTerritory(shared_ptr<Territory> territory)
{
    auto removeIterator = remove(ownedTerritories_->begin(), ownedTerritories_->end(), territory);
    ownedTerritories_->erase(removeIterator, ownedTerritories_->end());
}

vector<shared_ptr<Territory>> Player::toDefend()
{
    return *ownedTerritories_;
}

vector<shared_ptr<Territory>> Player::toAttack()
{
    return *ownedTerritories_;
}

void Player::issueOrder(OrderType type)
{
    switch (type)
    {
    case DEPLOY:
        orders_->addOrder(buildDeployOrder(*this));
        break;
    case ADVANCE:
        orders_->addOrder(buildAdvanceOrder(*this));
        break;
    default:
        cout << "Unsupported input.";
    }
}