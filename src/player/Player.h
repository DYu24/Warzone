#pragma once

#include "../map/Map.h"
#include "../orders/Orders.h"
#include <memory>
#include <vector>
using namespace std;

class Territory;
class OrdersList;
enum OrderType : short;

class Player
{
    public:
        Player();
        Player(unique_ptr<string> name);
        Player(const Player &player);
        const Player &operator=(const Player &player);
        friend ostream &operator<<(ostream &output, const Player &player);
        vector<shared_ptr<Territory>> getOwnedTerritories();
        OrdersList getOrdersList();
        void addOwnedTerritory(shared_ptr<Territory> territory);
        void removeOwnedTerritory(shared_ptr<Territory> territory);
        vector<shared_ptr<Territory>> toDefend();
        vector<shared_ptr<Territory>> toAttack();
        void issueOrder(OrderType type);

    private:
        unique_ptr<string> name_;
        unique_ptr<vector<shared_ptr<Territory>>> ownedTerritories_;
        unique_ptr<OrdersList> orders_;
};
