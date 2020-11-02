#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include <memory>
#include <vector>
using namespace std;

class Card;
class Hand;
class Territory;
class Order;
class OrdersList;

class Player
{
    public:
        Player();
        Player(string name);
        Player(const Player &player);
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<shared_ptr<Territory>> getOwnedTerritories();
        string getName();
        void setReinforcements(int reinforcements);
        void addCardToHand(shared_ptr<Card> card);
        void addOwnedTerritory(shared_ptr<Territory> territory);
        void removeOwnedTerritory(shared_ptr<Territory> territory);
        vector<shared_ptr<Territory>> toDefend();
        vector<shared_ptr<Territory>> toAttack();
        unique_ptr<Order> getNextOrder();
        void issueOrder();

    private:
        int reinforcements_;
        string name_;
        vector<shared_ptr<Territory>> ownedTerritories_;
        unique_ptr<OrdersList> orders_;
        unique_ptr<Hand> hand_;
};
