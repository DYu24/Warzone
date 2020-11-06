#pragma once

#include "../player/Player.h"

#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Territory
{
public:
    Territory();
    Territory(string name);
    Territory(const Territory &territory);
    const Territory &operator=(const Territory &territory);
    friend bool operator== (const Territory &t1, const Territory &t2);
    friend ostream &operator<<(ostream &output, const Territory &territory);
    string getName();
    void setName(string name);
    int getNumberOfArmies();
    void addArmies(int armies);
    void removeArmies(int armies);

private:
    string name_;
    int numberOfArmies_;
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
    Map(vector<Continent*> continents, unordered_map<Territory*, vector<Territory*>> adjacencyList);
    Map(const Map &map);
    ~Map();
    const Map &operator=(const Map &map);
    friend ostream &operator<<(ostream &output, const Map &map);
    unordered_map<Territory*, vector<Territory*>> getAdjacencyList();
    vector<Continent*> getContinents();
    vector<Territory*> getTerritories();
    vector<Territory*> getAdjacentTerritories(Territory* territory);
    bool validate();

private:
    vector<Continent*> continents_;
    unordered_map<Territory*, vector<Territory*>> adjacencyList_;
    bool checkGraphValidity();
    bool checkContinentsValidity();
    bool checkTerritoriesValidity();
    void copyMapContents(const Map &map);
    void destroyMapContents();
};