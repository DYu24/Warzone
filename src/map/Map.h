#pragma once

#include "../player/Player.h"

#include <memory>
#include <string>
#include <vector>
using namespace std;

class Territory {
    public:
        Territory();
        Territory(string *name);
        Territory(const Territory &territory);
        string getName();
        void setName(string &name);
        int getNumberOfArmies();
        void setNumberOfArmies(int &armies);
        vector<shared_ptr<Territory>> getAdjacentTerritories();
        void setAdjacentTerritories(vector<shared_ptr<Territory>> &territories);
        const Territory& operator=(const Territory &territory);
        friend ostream& operator<<(ostream &output, const Territory &territory);
        void addAdjacentTerritory(shared_ptr<Territory> territory);

    private:
        shared_ptr<string> name_;
        shared_ptr<int> numberOfArmies_;
        shared_ptr<vector<shared_ptr<Territory>>> adjacentTerritories_;
};

class Continent {
    public:
        Continent();
        Continent(string *name, int *controlValue);
        Continent(const Continent &continent);
        string getName();
        void setName(string &name);
        int getControlValue();
        void setControlValue(int &value);
        vector<shared_ptr<Territory>> getTerritories();
        void setTerritories(vector<shared_ptr<Territory>> &territories);
        const Continent& operator=(const Continent &continent);
        friend ostream& operator<<(ostream &output, const Continent &continent);
        void addTerritory(shared_ptr<Territory> territory);

    private:
        shared_ptr<string> name_;
        shared_ptr<int> controlValue_;
        shared_ptr<vector<shared_ptr<Territory>>> territories_;
};

class Map {
    public:
        Map();
        Map(const Map &map);
        vector<shared_ptr<Territory>> getAdjacencyList();
        void setAdjacencyList(vector<shared_ptr<Territory>> &adjacencyList);
        vector<shared_ptr<Continent>> getContinents();
        void setContinents(vector<shared_ptr<Continent>> &continents);
        const Map& operator=(const Map &map);
        friend ostream& operator<<(ostream &output, const Map &map);
        bool validate();

    private:
        shared_ptr<vector<shared_ptr<Territory>>> adjacencyList_;
        shared_ptr<vector<shared_ptr<Continent>>> continents_;
};