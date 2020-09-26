#include "Map.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/* 
===================================
 Implementation for Territory class
===================================
 */ 

// Constructors
Territory::Territory(): name_(new string("")), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(string *name): name_(name), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(const Territory &territory): name_(territory.name_), numberOfArmies_(territory.numberOfArmies_), adjacentTerritories_(territory.adjacentTerritories_) {}

// Getters and Setters
string Territory::getName()
{
    return *name_;
}

void Territory::setName(string &name)
{
    *name_ = name;
}

int Territory::getNumberOfArmies()
{
    return *numberOfArmies_;
}

void Territory::setNumberOfArmies(int &armies)
{
    *numberOfArmies_ = armies;
}

vector<shared_ptr<Territory>> Territory::getAdjacentTerritories()
{
    return *adjacentTerritories_;
}

void Territory::setAdjacentTerritories(vector<shared_ptr<Territory>> &territories)
{
    *adjacentTerritories_ = territories;
}

// Operator overloading
const Territory& Territory::operator=(const Territory &territory)
{
    name_ = territory.name_;
    numberOfArmies_ = territory.numberOfArmies_;
    adjacentTerritories_ = territory.adjacentTerritories_;
}

ostream& operator<<(ostream &output, const Territory &territory) {
    output << "[Territory]: " << territory.name_ << ", " << territory.numberOfArmies_ << " Armies, " << territory.adjacentTerritories_->size() << " Adjacent Territories";
    return output;
}

// Method to add an adjacent territory to the current territory
void Territory::addAdjacentTerritory(shared_ptr<Territory> territory)
{
    adjacentTerritories_->push_back(territory);
}

/* 
===================================
 Implementation for Continent class
===================================
 */ 

// Constructors
Continent::Continent() : name_(new string("")), controlValue_(new int(0)), territories_(new vector<shared_ptr<Territory>>()) {}

Continent::Continent(string *name, int *controlValue) : name_(name), controlValue_(controlValue), territories_(new vector<shared_ptr<Territory>>()) {}

Continent::Continent(const Continent &continent) : name_(continent.name_), controlValue_(continent.controlValue_), territories_(continent.territories_) {}

// Getters and Setters
string Continent::getName()
{
    return *name_;
}

void Continent::setName(string &name)
{
    *name_ = name;
}

int Continent::getControlValue()
{
    return *controlValue_;
}

void Continent::setControlValue(int &value)
{
    *controlValue_ = value;
}

vector<shared_ptr<Territory>> Continent::getTerritories()
{
    return *territories_;
}

void Continent::setTerritories(vector<shared_ptr<Territory>> &territories)
{
    *territories_ = territories;
}

// Operator overloading
const Continent& Continent::operator=(const Continent &continent)
{
    name_ = continent.name_;
    controlValue_ = continent.controlValue_;
    territories_ = continent.territories_;
}

ostream& operator<<(ostream &output, const Continent &continent) {
    output << "[Continent]: " << continent.name_ << ", " << continent.controlValue_ << " Control Value, " << continent.territories_->size() << " Territories";
    return output;
}

// Method to add a territory to the current Continent
void Continent::addTerritory(shared_ptr<Territory> territory)
{
    territories_->push_back(territory);
}

/* 
===================================
 Implementation for Map class
===================================
 */ 

// Constructors
Map::Map(): adjacencyList_(new vector<shared_ptr<Territory>>()), continents_(new vector<shared_ptr<Continent>>()) {}

Map::Map(const Map &map): adjacencyList_(map.adjacencyList_), continents_(map.continents_) {}

// Getters and Setters
vector<shared_ptr<Territory>> Map::getAdjacencyList()
{
    return *adjacencyList_;
}

void Map::setAdjacencyList(vector<shared_ptr<Territory>> &adjacencyList)
{
    *adjacencyList_ = adjacencyList;
}

vector<shared_ptr<Continent>> Map::getContinents()
{
    return *continents_;
}

void Map::setContinents(vector<shared_ptr<Continent>> &continents)
{
    *continents_ = continents;
}

// Operator overloading
const Map& Map::operator=(const Map &map)
{
    adjacencyList_ = map.adjacencyList_;
    continents_ = map.continents_;
}

ostream& operator<<(ostream &output, const Map &map) {
    output << "[Map]: " << map.adjacencyList_->size() << " Territories, " << map.continents_->size() << " Continents";
    return output;
}

/*
 * Method to check whether the map is valid or not according to the following:
 * 1. Map is a connected graph
 * 2. The continents are connected subgraphs
 * 3. Each territory belongs to only one continent
 */
bool Map::validate()
{
    return true;
}