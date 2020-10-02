#include "../orders/Orders.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

namespace
{
    // vector<shared_ptr<Territory>> getUnownedTerritories(vector<shared_ptr<Territory>> v1, vector<shared_ptr<Territory>> v2)
    // {
    //     sort(v1.begin(), v1.end());
    //     sort(v2.begin(), v2.end());

    //     vector<shared_ptr<Territory>> difference;
    //     set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(difference, difference.begin()));

    //     return difference;
    // }

    unique_ptr<DeployOrder> buildDeployOrder(Player &player)
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

        return make_unique<DeployOrder>(make_unique<int>(armies), selectedTerritory);
    }

    unique_ptr<AdvanceOrder> buildAdvanceOrder(Player &player)
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

        return make_unique<AdvanceOrder>(make_unique<int>(armies), source, target);
    }
} // namespace

Player::Player() : name_(make_unique<string>("unknown_player")), ownedTerritories_(make_unique<vector<shared_ptr<Territory>>>()), orders_(make_unique<OrdersList>()), hand_(make_unique<Hand>()) {}

Player::Player(unique_ptr<string> name) : name_(move(name)), ownedTerritories_(make_unique<vector<shared_ptr<Territory>>>()), orders_(make_unique<OrdersList>()), hand_(make_unique<Hand>()) {}

Player::Player(const Player &player)
{
    name_ = make_unique<string>(*player.name_);
    orders_ = make_unique<OrdersList>(*player.orders_);
    ownedTerritories_ = make_unique<vector<shared_ptr<Territory>>>(*player.ownedTerritories_);\
    setHand(*player.hand_);
}

const Player &Player::operator=(const Player &player)
{
    name_ = make_unique<string>(*player.name_);
    orders_ = make_unique<OrdersList>(*player.orders_);
    ownedTerritories_ = make_unique<vector<shared_ptr<Territory>>>(*player.ownedTerritories_);
    setHand(*player.hand_);
    return *this;
}

ostream &operator<<(ostream &output, const Player &player)
{
    output << "[Player] " << *player.name_ << " has " << player.ownedTerritories_->size() << " Territories, " << player.orders_->getOrders().size() << " Orders, " << player.hand_->getCards().size() << " cards in Hand";
    return output;
}

vector<shared_ptr<Territory>> Player::getOwnedTerritories()
{
    return *ownedTerritories_;
}

OrdersList Player::getOrdersList()
{
    return *orders_;
}

Hand Player::getHand()
{
    return *hand_;
}

void Player::setHand(Hand hand)
{
    hand_ = make_unique<Hand>();
    for (auto const &card : hand.getCards())
    {
        hand_->addCard(card->clone());
    }
}

void Player::addCardToHand(unique_ptr<Card> card)
{
    hand_->addCard(move(card));
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