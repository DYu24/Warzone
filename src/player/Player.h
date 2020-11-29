#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "../strategies/PlayerStrategies.h"
#include <string>
#include <vector>
#include <unordered_map>

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
        Player(std::string name);
        Player(std::string name, PlayerStrategy* strategy);
        Player(const Player &player);
        ~Player();
        const Player &operator=(const Player &player);
        friend std::ostream &operator<<(std::ostream &output, const Player &player);
        std::vector<Territory*> getOwnedTerritories() const;
        std::string getName() const;
        OrdersList getOrdersList() const;
        Hand getHand() const;
        std::vector<Player*> getDiplomaticRelations() const;
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
        std::vector<Territory*> getOwnTerritoriesWithMovableArmies() const;
        std::vector<Territory*> toDefend() const;
        std::vector<Territory*> toAttack() const;
        void issueOrder();

    private:
        std::string name_;
        int reinforcements_;
        bool committed_;
        PlayerStrategy* strategy_;
        OrdersList* orders_;
        Hand* hand_;
        std::vector<Territory*> ownedTerritories_;
        std::vector<Player*> diplomaticRelations_;
        std::unordered_map<Territory*, std::vector<Territory*>> issuedDeploymentsAndAdvancements_;
        bool advancePairingExists_(Territory* source, Territory* destination);
};
