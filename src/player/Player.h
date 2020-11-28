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
class HumanPlayerStrategy;
class PlayerStrategy;
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
    friend class HumanPlayerStrategy;

    public:
        Player();
        Player(string name);
        Player(string name, PlayerStrategy* strategy);
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
        void setStrategy(PlayerStrategy* strategy);
        void addReinforcements(int reinforcements);
        void addOwnedTerritory(Territory* territory);
        void removeOwnedTerritory(Territory* territory);
        void addDiplomaticRelation(Player* player);
        void endTurn();
        void addOrder(Order* order);
        Order* getNextOrder();
        Order* peekNextOrder();
        void drawCardFromDeck();
        bool isHuman() const;
        bool isNeutral() const;
        bool isDoneIssuingOrders() const;
        vector<Territory*> getOwnTerritoriesWithMovableArmies() const;
        vector<Territory*> toDefend() const;
        vector<Territory*> toAttack() const;
        void issueOrder();

    private:
        string name_;
        int reinforcements_;
        bool committed_;
        PlayerStrategy* strategy_;
        OrdersList* orders_;
        Hand* hand_;
        vector<Territory*> ownedTerritories_;
        vector<Player*> diplomaticRelations_;
        unordered_map<Territory*, vector<Territory*>> issuedDeploymentsAndAdvancements_;
        bool advancePairingExists_(Territory* source, Territory* destination);
};
