#pragma once

#include "../map/Map.h"
#include "../orders/Orders.h"
#include <memory>
#include <vector>
using namespace std;

class Territory;

class Player
{
    public:
        Player();
        Player(shared_ptr<string> name);
        Player(const Player &player);
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<shared_ptr<Territory>> getOwnedTerritories();
        void setOwnedTerritories(vector<shared_ptr<Territory>> &territories);
        OrdersList getOrdersList();
        void setOrdersList(OrdersList &orders);
        void addOwnedTerritory(shared_ptr<Territory> territory);
        void removeOwnedTerritory(shared_ptr<Territory> territory);
        vector<shared_ptr<Territory>> toDefend();
        vector<shared_ptr<Territory>> toAttack();
        void issueOrder(OrderType type);

    private:
        shared_ptr<string> name_;
        shared_ptr<vector<shared_ptr<Territory>>> ownedTerritories_;
        shared_ptr<OrdersList> orders_;
};
