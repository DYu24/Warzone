#pragma once

#include "../player/Player.h"

#include <string>
#include <vector>
using namespace std;

class Territory
{
public:
    Territory();
    Territory(string name);
    Territory(string name, int numberOfArmies);
    Territory(const Territory &territory);
    const Territory &operator=(const Territory &territory);
    friend ostream &operator<<(ostream &output, const Territory &territory);
    string getName();
    void setName(string name);
    int getNumberOfArmies();
    void addArmies(int armies);
    void removeArmies(int armies);
    vector<Territory*> getAdjacentTerritories();
    void setAdjacentTerritories(vector<Territory*> territories);
    void addAdjacentTerritory(Territory* territory);
    void removeAdjacentTerritory(Territory* territory);

private:
    string name_;
    int numberOfArmies_;
    vector<Territory*> adjacentTerritories_;
};

class Continent
{
public:
    Continent();
    Continent(string name, int controlValue);
    Continent(const Continent &continent);
    const Continent &operator=(const Continent &continent);
    friend ostream &operator<<(ostream &output, const Continent &continent);
    string getName();
    void setName(string name);
    int getControlValue();
    void setControlValue(int value);
    vector<Territory*> getTerritories();
    void setTerritories(vector<Territory*> territories);
    void addTerritory(Territory* territory);

private:
    string name_;
    int controlValue_;
    vector<Territory*> territories_;
};

class Map
{
public:
    Map();
    Map(const Map &map);
    ~Map();
    vector<Territory*> getAdjacencyList();
    const Map &operator=(const Map &map);
    friend ostream &operator<<(ostream &output, const Map &map);
    void setAdjacencyList(vector<Territory*> adjacencyList);
    vector<Continent*> getContinents();
    void setContinents(vector<Continent*> continents);
    bool validate();

private:
    vector<Territory*> adjacencyList_;
    vector<Continent*> continents_;
    bool checkGraphValidity();
    bool checkContinentsValidity();
    bool checkTerritoriesValidity();
};