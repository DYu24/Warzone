#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "../strategies/PlayerStrategies.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class AggressivePlayerStrategy;
class BenevolentPlayerStrategy;
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
    friend class AggressivePlayerStrategy;
    friend class BenevolentPlayerStrategy;

    public:
        Player();
        Player(string name);
        Player(const Player &player);
        ~Player();
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<Territory*> getOwnedTerritories() const;
        string getName() const;
        OrdersList getOrdersList() const;
        Hand getHand() const;
        vector<Player*> getDiplomaticRelations() const;
        int getReinforcements() const;
        void addReinforcements(int reinforcements);
        void addOwnedTerritory(Territory* territory);
        void removeOwnedTerritory(Territory* territory);
        void addDiplomaticRelation(Player* player);
        void endTurn();
        void addOrder(Order* order);
        Order* getNextOrder();
        Order* peekNextOrder();
        void drawCardFromDeck();
        bool isNeutral() const;
        bool isDoneIssuingOrders() const;
        vector<Territory*> toDefend() const;
        vector<Territory*> toAttack() const;
        void issueOrder();

    private:
        int reinforcements_;
        string name_;
        OrdersList* orders_;
        Hand* hand_;
        vector<Territory*> ownedTerritories_;
        vector<Player*> diplomaticRelations_;
        bool isNeutral_;
        bool committed_;
        unordered_map<Territory*, vector<Territory*>> issuedDeploymentsAndAdvancements_;

        bool issueDeployOrder(vector<Territory*> territoriesToDefend);
        bool issueAdvanceOrder(vector<Territory*> territoriesToAttack, vector<Territory*> territoriesToDefend);
        bool playCard();
        bool advancePairingExists(Territory* source, Territory* destination);
};
