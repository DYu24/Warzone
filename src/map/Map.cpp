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

Territory::Territory(): name_(new string("")), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(string *name): name_(name), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(const Territory &territory): name_(territory.name_), numberOfArmies_(territory.numberOfArmies_), adjacentTerritories_(territory.adjacentTerritories_) {}

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

void Territory::addAdjacentTerritory(shared_ptr<Territory> territory)
{
    adjacentTerritories_->push_back(territory);
}

/* 
===================================
 Implementation for Continent class
===================================
 */ 

Continent::Continent() : name_(new string("")), controlValue_(new int(0)), territories_(new vector<shared_ptr<Territory>>()) {}

Continent::Continent(string *name, int *controlValue) : name_(name), controlValue_(controlValue), territories_(new vector<shared_ptr<Territory>>()) {}

Continent::Continent(const Continent &continent) : name_(continent.name_), controlValue_(continent.controlValue_), territories_(continent.territories_) {}

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

void Continent::addTerritory(shared_ptr<Territory> territory)
{
    territories_->push_back(territory);
}

/* 
===================================
 Implementation for Map class
===================================
 */ 

Map::Map(): adjacencyList_(new vector<shared_ptr<Territory>>()), continents_(new vector<shared_ptr<Continent>>()) {}

Map::Map(const Map &map): adjacencyList_(map.adjacencyList_), continents_(map.continents_) {}

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

bool Map::validate()
{
    return true;
}