#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include <string>
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
        ~Player();
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<Territory*> getOwnedTerritories();
        string getName();
        vector<Player*> getDiplomaticRelations();
        void setReinforcements(int reinforcements);
        void addCardToHand(Card* card);
        void addOwnedTerritory(Territory* territory);
        void removeOwnedTerritory(Territory* territory);
        void addDiplomaticRelation(Player* player);
        void clearDiplomaticRelations();
        vector<Territory*> toDefend();
        vector<Territory*> toAttack();
        Order* getNextOrder();
        void issueOrder();

    private:
        int reinforcements_;
        string name_;
        OrdersList* orders_;
        Hand* hand_;
        vector<Territory*> ownedTerritories_;
        vector<Player*> diplomaticRelations_;
};
