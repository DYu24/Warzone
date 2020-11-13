#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include <string>
#include <vector>
using namespace std;

class Card;
class Deck;
class Hand;
class Map;
class Territory;
class DeployOrder;
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
        void addReinforcements(int reinforcements);
        void addOwnedTerritory(Territory* territory);
        void removeOwnedTerritory(Territory* territory);
        void addDiplomaticRelation(Player* player);
        void clearDiplomaticRelations();
        Order* getNextOrder();
        Order* peekNextOrder();
        void drawCardFromDeck();
        vector<Territory*> toDefend();
        vector<Territory*> toAttack();
        void issueOrder();

    private:
        int reinforcements_;
        string name_;
        OrdersList* orders_;
        Hand* hand_;
        vector<Territory*> ownedTerritories_;
        vector<Player*> diplomaticRelations_;
        void issueDeployOrders(vector<Territory*> territoriesToDefend);
        void issueAdvanceOrders(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend);
        void playCard();
};
