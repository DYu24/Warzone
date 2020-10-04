#pragma once

#include "../player/Player.h"

#include <memory>
#include <string>
#include <vector>
using namespace std;

class Territory
{
public:
    Territory();
    Territory(unique_ptr<string> name);
    Territory(const Territory &territory);
    const Territory &operator=(const Territory &territory);
    friend ostream &operator<<(ostream &output, const Territory &territory);
    string getName();
    void setName(string &name);
    int getNumberOfArmies();
    void setNumberOfArmies(int &armies);
    vector<shared_ptr<Territory>> getAdjacentTerritories();
    void setAdjacentTerritories(vector<shared_ptr<Territory>> &territories);
    void addAdjacentTerritory(shared_ptr<Territory> territory);

private:
    unique_ptr<string> name_;
    unique_ptr<int> numberOfArmies_;
    unique_ptr<vector<shared_ptr<Territory>>> adjacentTerritories_;
};

class Continent
{
public:
    Continent();
    Continent(unique_ptr<string> name, unique_ptr<int> controlValue);
    Continent(const Continent &continent);
    const Continent &operator=(const Continent &continent);
    friend ostream &operator<<(ostream &output, const Continent &continent);
    string getName();
    void setName(string &name);
    int getControlValue();
    void setControlValue(int &value);
    vector<shared_ptr<Territory>> getTerritories();
    void setTerritories(vector<shared_ptr<Territory>> &territories);
    void addTerritory(shared_ptr<Territory> territory);

private:
    unique_ptr<string> name_;
    unique_ptr<int> controlValue_;
    unique_ptr<vector<shared_ptr<Territory>>> territories_;
};

class Map
{
public:
    Map();
    Map(const Map &map);
    vector<shared_ptr<Territory>> getAdjacencyList();
    const Map &operator=(const Map &map);
    friend ostream &operator<<(ostream &output, const Map &map);
    void setAdjacencyList(vector<shared_ptr<Territory>> &adjacencyList);
    vector<unique_ptr<Continent>> &getContinents();
    void setContinents(vector<unique_ptr<Continent>> &continents);
    bool validate();

private:
    unique_ptr<vector<shared_ptr<Territory>>> adjacencyList_;
    unique_ptr<vector<unique_ptr<Continent>>> continents_;
    bool checkGraphValidity();
    bool checkContinentsValidity();
    bool checkTerritoriesValidity();
};