#pragma once

#include "../map/Map.h"
#include "../player/Player.h"
#include <vector>
using namespace std;

class Player;

class PlayerStrategy
{
    public:
        virtual vector<Territory*> toDefend() const = 0;
        virtual vector<Territory*> toAttack() const = 0;
        virtual void issueOrder() = 0;
        friend ostream &operator<<(ostream &output, const PlayerStrategy &strategy);

    protected:
        Player* player_;
        PlayerStrategy();
        PlayerStrategy(Player* player);
        PlayerStrategy(const PlayerStrategy &strategy);
        const PlayerStrategy &operator=(const PlayerStrategy &strategy);
        virtual ostream &print_(ostream &output) const = 0;
};

class AggressivePlayerStrategy : public PlayerStrategy
{
    public:
        AggressivePlayerStrategy();
        AggressivePlayerStrategy(Player* player);
        AggressivePlayerStrategy(const AggressivePlayerStrategy &strategy);
        const AggressivePlayerStrategy &operator=(const AggressivePlayerStrategy &strategy);
        vector<Territory*> toDefend() const;
        vector<Territory*> toAttack() const;
        void issueOrder();

    protected:
        ostream &print_(ostream &output) const;

    private:
        bool deployToTopTerritory(vector<Territory*> territoriesToDefend);
        bool attackFromTopTerritory(Territory* attackFrom, vector<Territory*> territoriesToAttack);
        bool fortifyTopTerritory(vector<Territory*> territoriesToDefend);
};

class BenevolentPlayerStrategy : public PlayerStrategy
{
    public:
        BenevolentPlayerStrategy();
        BenevolentPlayerStrategy(Player* player);
        BenevolentPlayerStrategy(const BenevolentPlayerStrategy &strategy);
        const BenevolentPlayerStrategy &operator=(const BenevolentPlayerStrategy &strategy);
        vector<Territory*> toDefend() const;
        vector<Territory*> toAttack() const;
        void issueOrder();

    protected:
        ostream &print_(ostream &output) const;

    private:
        bool deployToWeakTerritories(vector<Territory*> territoriesToDefend);
        bool fortifyWeakTerritories(vector<Territory*> territoriesToDefend);
};

class NeutralPlayerStrategy : public PlayerStrategy
{
    public:
        NeutralPlayerStrategy();
        NeutralPlayerStrategy(Player* player);
        NeutralPlayerStrategy(const NeutralPlayerStrategy &strategy);
        const NeutralPlayerStrategy &operator=(const NeutralPlayerStrategy &strategy);
        vector<Territory*> toDefend() const;
        vector<Territory*> toAttack() const;
        void issueOrder();

    protected:
        ostream &print_(ostream &output) const;
};