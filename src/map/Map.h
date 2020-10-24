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
    Territory(string name);
    Territory(const Territory &territory);
    const Territory &operator=(const Territory &territory);
    friend ostream &operator<<(ostream &output, const Territory &territory);
    string getName();
    void setName(string name);
    int getNumberOfArmies();
    void setNumberOfArmies(int armies);
    vector<shared_ptr<Territory>> getAdjacentTerritories();
    void setAdjacentTerritories(vector<shared_ptr<Territory>> territories);
    void addAdjacentTerritory(shared_ptr<Territory> territory);

private:
    string name_;
    int numberOfArmies_;
    vector<shared_ptr<Territory>> adjacentTerritories_;
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
    vector<shared_ptr<Territory>> getTerritories();
    void setTerritories(vector<shared_ptr<Territory>> territories);
    void addTerritory(shared_ptr<Territory> territory);

private:
    string name_;
    int controlValue_;
    vector<shared_ptr<Territory>> territories_;
};

class Map
{
public:
    Map();
    Map(const Map &map);
    vector<shared_ptr<Territory>> getAdjacencyList();
    const Map &operator=(const Map &map);
    friend ostream &operator<<(ostream &output, const Map &map);
    void setAdjacencyList(vector<shared_ptr<Territory>> adjacencyList);
    vector<unique_ptr<Continent>> &getContinents();
    void setContinents(const vector<unique_ptr<Continent>> &continents);
    bool validate();

private:
    vector<shared_ptr<Territory>> adjacencyList_;
    vector<unique_ptr<Continent>> continents_;
    bool checkGraphValidity();
    bool checkContinentsValidity();
    bool checkTerritoriesValidity();
};