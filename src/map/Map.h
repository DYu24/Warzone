#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Territory
{
public:
    Territory();
    Territory(std::string name);
    Territory(const Territory &territory);
    const Territory &operator=(const Territory &territory);
    friend bool operator== (const Territory &t1, const Territory &t2);
    friend std::ostream &operator<<(std::ostream &output, const Territory &territory);
    std::string getName() const;
    int getNumberOfArmies() const;
    int getPendingIncomingArmies() const;
    int getPendingOutgoingArmies() const;
    void setName(std::string name);
    void setPendingIncomingArmies(int armies);
    void setPendingOutgoingArmies(int armies);
    void removeArmies(int armies);
    void addArmies(int armies);
    void addPendingIncomingArmies(int armies);
    void addPendingOutgoingArmies(int armies);
    int getNumberOfMovableArmies();

private:
    std::string name_;
    int numberOfArmies_;
    int pendingIncomingArmies_;
    int pendingOutgoingArmies_;
};

class Continent
{
public:
    Continent();
    Continent(std::string name, int controlValue);
    Continent(const Continent &continent);
    ~Continent();
    const Continent &operator=(const Continent &continent);
    friend std::ostream &operator<<(std::ostream &output, const Continent &continent);
    std::string getName() const;
    int getControlValue() const;
    std::vector<Territory*> getTerritories() const;
    void setName(std::string name);
    void setControlValue(int value);
    void setTerritories(std::vector<Territory*> territories);
    void addTerritory(Territory* territory);

private:
    std::string name_;
    int controlValue_;
    std::vector<Territory*> territories_;
};

class Map
{
public:
    Map();
    Map(std::vector<Continent*> continents, std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList);
    Map(const Map &map);
    ~Map();
    const Map &operator=(const Map &map);
    friend std::ostream &operator<<(std::ostream &output, const Map &map);
    std::unordered_map<Territory*, std::vector<Territory*>> getAdjacencyList() const;
    std::vector<Continent*> getContinents() const;
    std::vector<Territory*> getTerritories() const;
    std::vector<Territory*> getAdjacentTerritories(Territory* territory);
    bool validate();

private:
    std::vector<Continent*> continents_;
    std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList_;
    bool checkGraphValidity_();
    bool checkContinentsValidity_();
    bool checkTerritoriesValidity_();
    void copyMapContents_(const Map &map);
    void destroyMapContents_();
};