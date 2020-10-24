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
class OrdersList;
enum OrderType : short;

class Player
{
    public:
        Player();
        Player(string name);
        Player(const Player &player);
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<shared_ptr<Territory>> getOwnedTerritories();
        OrdersList getOrdersList();
        Hand getHand();
        void setHand(Hand hand);
        void addCardToHand(unique_ptr<Card> card);
        void addOwnedTerritory(shared_ptr<Territory> territory);
        void removeOwnedTerritory(shared_ptr<Territory> territory);
        vector<shared_ptr<Territory>> toDefend();
        vector<shared_ptr<Territory>> toAttack();
        void issueOrder(OrderType type);

    private:
        string name_;
        vector<shared_ptr<Territory>> ownedTerritories_;
        unique_ptr<OrdersList> orders_;
        unique_ptr<Hand> hand_;
};
