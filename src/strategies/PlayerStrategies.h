#pragma once

#include "../map/Map.h"
#include "../player/Player.h"
#include <vector>
using namespace std;

class Player;

class PlayerStrategy
{
    public:
        virtual vector<Territory*> toDefend(const Player* player) const = 0;
        virtual vector<Territory*> toAttack(const Player* player) const = 0;
        virtual void issueOrder(Player* player) = 0;
        virtual PlayerStrategy* clone() const = 0;
        friend ostream &operator<<(ostream &output, const PlayerStrategy &strategy);

    protected:
        virtual ostream &print_(ostream &output) const = 0;
};

class AggressivePlayerStrategy : public PlayerStrategy
{
    public:
        PlayerStrategy* clone() const;
        vector<Territory*> toDefend(const Player* player) const;
        vector<Territory*> toAttack(const Player* player) const;
        void issueOrder(Player* player);

    protected:
        ostream &print_(ostream &output) const;

    private:
        bool deployToTopTerritory(Player* player, Territory* territory);
        bool attackFromTopTerritory(Player* player, Territory* attackFrom, vector<Territory*> territoriesToAttack);
        bool advanceToRandomTerritory(Player* player, vector<Territory*> territoriesToDefend);
        bool playCard(Player* player, Territory* topTerritory);
};

class BenevolentPlayerStrategy : public PlayerStrategy
{
    public:
        PlayerStrategy* clone() const;
        vector<Territory*> toDefend(const Player* player) const;
        vector<Territory*> toAttack(const Player* player) const;
        void issueOrder(Player* player);

    protected:
        ostream &print_(ostream &output) const;

    private:
        bool deployToWeakTerritories(Player* player, vector<Territory*> territoriesToDefend);
        bool fortifyWeakTerritories(Player* player, vector<Territory*> territoriesToDefend);
        bool playCard(Player* player, vector<Territory*> territoriesToDefend);
};

class HumanPlayerStrategy : public PlayerStrategy
{
    public:
        PlayerStrategy* clone() const;
        vector<Territory*> toDefend(const Player* player) const;
        vector<Territory*> toAttack(const Player* player) const;
        void issueOrder(Player* player);

    protected:
        ostream &print_(ostream &output) const;

    private:
        void deployReinforcements(Player* player, vector<Territory*> territoriesToDefend);
        void issueAdvance(Player* player, vector<Territory*> territoriesToDefend);
        void playCard(Player* player, vector<Territory*> territoriesToDefend);
};

class NeutralPlayerStrategy : public PlayerStrategy
{
    public:
        PlayerStrategy* clone() const;
        vector<Territory*> toDefend(const Player* player) const;
        vector<Territory*> toAttack(const Player* player) const;
        void issueOrder(Player* player);

    protected:
        ostream &print_(ostream &output) const;
};