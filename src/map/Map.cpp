#include "Map.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

/* 
===================================
 Implementation for Territory class
===================================
 */

// Constructors
Territory::Territory() : name_(new string("")), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(string *name) : name_(name), numberOfArmies_(new int(0)), adjacentTerritories_(new vector<shared_ptr<Territory>>()) {}

Territory::Territory(const Territory &territory) : name_(territory.name_), numberOfArmies_(territory.numberOfArmies_), adjacentTerritories_(territory.adjacentTerritories_) {}

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
const Territory &Territory::operator=(const Territory &territory)
{
    name_ = territory.name_;
    numberOfArmies_ = territory.numberOfArmies_;
    adjacentTerritories_ = territory.adjacentTerritories_;
    return *this;
}

ostream &operator<<(ostream &output, const Territory &territory)
{
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
const Continent &Continent::operator=(const Continent &continent)
{
    name_ = continent.name_;
    controlValue_ = continent.controlValue_;
    territories_ = continent.territories_;
    return *this;
}

ostream &operator<<(ostream &output, const Continent &continent)
{
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
Map::Map() : adjacencyList_(new vector<shared_ptr<Territory>>()), continents_(new vector<shared_ptr<Continent>>()) {}

Map::Map(const Map &map) : adjacencyList_(map.adjacencyList_), continents_(map.continents_) {}

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
const Map &Map::operator=(const Map &map)
{
    adjacencyList_ = map.adjacencyList_;
    continents_ = map.continents_;
    return *this;
}

ostream &operator<<(ostream &output, const Map &map)
{
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
    return checkGraphValidity() && checkContinentsValidity() && checkTerritoriesValidity();
}

/*
 * Helper method to validate that the map is a connected graph.
 * This can be done by traversing the graph and comparing the size of visitied territories
 * to the size of all territories.
 * 
 * Graph traversal implemented using Breadth-First Search.
 */
bool Map::checkGraphValidity()
{
    unordered_set<shared_ptr<Territory>> visitedTerritories;
    queue<shared_ptr<Territory>> territoryQueue;
    territoryQueue.push(adjacencyList_->front());

    while (!territoryQueue.empty())
    {
        shared_ptr<Territory> current = territoryQueue.front();
        territoryQueue.pop();
        visitedTerritories.insert(current);

        for (auto const &territory : current->getAdjacentTerritories())
        {
            // If the territory hasn't been visited, add it to the queue
            if (visitedTerritories.find(territory) == visitedTerritories.end())
            {
                territoryQueue.push(territory);
            }
        }
    }

    return visitedTerritories.size() == adjacencyList_->size();
}

/*
 * Helper method to validate that the map's continents are connected subgraphs.
 */
bool Map::checkContinentsValidity()
{
    vector<shared_ptr<Continent>> continents = getContinents();

    for (auto const &continent : continents)
    {
        unordered_set<shared_ptr<Territory>> visitedTerritories;
        queue<shared_ptr<Territory>> territoryQueue;

        vector<shared_ptr<Territory>> continentMembers = continent->getTerritories();
        territoryQueue.push(continentMembers.front());

        while (!territoryQueue.empty())
        {
            shared_ptr<Territory> current = territoryQueue.front();
            territoryQueue.pop();
            visitedTerritories.insert(current);

            // If the territory isn't a member of the set of all territories, then continent is not a subgraph
            if (find(adjacencyList_->begin(), adjacencyList_->end(), current) == adjacencyList_->end())
            {
                return false;
            }

            for (auto const &territory : current->getAdjacentTerritories())
            {
                bool visited = visitedTerritories.find(territory) != visitedTerritories.end();
                bool inCurrentContinent = find(continentMembers.begin(), continentMembers.end(), territory) != continentMembers.end();
                if (inCurrentContinent && !visited)
                {
                    territoryQueue.push(territory);
                }
            }
        }

        // If the visited territories of the continent is not the same as all its territories, then continent is not connected
        if (continentMembers.size() != visitedTerritories.size())
        {
            return false;
        }
    }

    return true;
}

/*
 * Helper method to validate that the map's territories all belong to only one continent.
 */
bool Map::checkTerritoriesValidity()
{
    vector<shared_ptr<Continent>> continents = getContinents();
    unordered_set<shared_ptr<Territory>> visitedTerritories;

    for (auto const &continent : continents)
    {
        for (auto const &territory : continent->getTerritories())
        {
            if (visitedTerritories.find(territory) == visitedTerritories.end())
            {
                visitedTerritories.insert(territory);
            }
            else
            {
                // A continent has a territory that already belongs to another continent.
                return false;
            }
        }
    }

    return true;
}