#pragma once

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
    string getName() const;
    int getNumberOfArmies() const;
    int getPendingIncomingArmies() const;
    int getPendingOutgoingArmies() const;
    void setName(string name);
    void setPendingIncomingArmies(int armies);
    void setPendingOutgoingArmies(int armies);
    void removeArmies(int armies);
    void addArmies(int armies);
    void addPendingIncomingArmies(int armies);
    void addPendingOutgoingArmies(int armies);
    int getNumberOfMovableArmies();

private:
    string name_;
    int numberOfArmies_;
    int pendingIncomingArmies_;
    int pendingOutgoingArmies_;
};

class Continent
{
public:
    Continent();
    Continent(string name, int controlValue);
    Continent(const Continent &continent);
    ~Continent();
    const Continent &operator=(const Continent &continent);
    friend ostream &operator<<(ostream &output, const Continent &continent);
    string getName() const;
    int getControlValue() const;
    vector<Territory*> getTerritories() const;
    void setName(string name);
    void setControlValue(int value);
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
    unordered_map<Territory*, vector<Territory*>> getAdjacencyList() const;
    vector<Continent*> getContinents() const;
    vector<Territory*> getTerritories() const;
    vector<Territory*> getAdjacentTerritories(Territory* territory);
    bool validate();

private:
    vector<Continent*> continents_;
    unordered_map<Territory*, vector<Territory*>> adjacencyList_;
    bool checkGraphValidity_();
    bool checkContinentsValidity_();
    bool checkTerritoriesValidity_();
    void copyMapContents_(const Map &map);
    void destroyMapContents_();
};