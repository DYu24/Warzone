#include "Map.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_set>
using namespace std;


/* 
===================================
 Implementation for Territory class
===================================
 */

// Constructors
Territory::Territory(): name_("unknown_territory"), numberOfArmies_(0), pendingIncomingArmies_(0), pendingOutgoingArmies_(0) {}

Territory::Territory(string name) : name_(name), numberOfArmies_(0), pendingIncomingArmies_(0), pendingOutgoingArmies_(0) {}

Territory::Territory(const Territory &territory)
    : name_(territory.name_), numberOfArmies_(territory.numberOfArmies_), pendingIncomingArmies_(territory.pendingIncomingArmies_), pendingOutgoingArmies_(territory.pendingOutgoingArmies_) {}

// Operator overloading
const Territory &Territory::operator=(const Territory &territory)
{
    name_ = territory.name_;
    numberOfArmies_ = territory.numberOfArmies_;
    pendingIncomingArmies_ = territory.pendingIncomingArmies_;
    pendingOutgoingArmies_ = territory.pendingOutgoingArmies_;
    return *this;
}

ostream &operator<<(ostream &output, const Territory &territory)
{
    output << "[Territory]: " << territory.name_ << ", " << territory.numberOfArmies_ << " Armies";
    return output;
}

bool operator==(const Territory &t1, const Territory &t2)
{
    return t1.name_ == t2.name_
        && t1.numberOfArmies_ == t2.numberOfArmies_
        && t1.pendingIncomingArmies_ == t2.pendingIncomingArmies_
        && t1.pendingOutgoingArmies_ == t2.pendingOutgoingArmies_;
}

// Getters
string Territory::getName() const
{
    return name_;
}

int Territory::getNumberOfArmies() const
{
    return numberOfArmies_;
}

int Territory::getPendingIncomingArmies() const
{
    return pendingIncomingArmies_;
}

int Territory::getPendingOutgoingArmies() const
{
    return pendingOutgoingArmies_;
}

// Setters
void Territory::setName(string name)
{
    name_ = name;
}

void Territory::setPendingIncomingArmies(int armies)
{
    pendingIncomingArmies_ = armies;
}

void Territory::setPendingOutgoingArmies(int armies)
{
    pendingOutgoingArmies_ = armies;
}

// Remove a number of armies to the current territory
void Territory::removeArmies(int armies)
{
    numberOfArmies_ -= armies;
    if (numberOfArmies_ < 0)
    {
        numberOfArmies_ = 0;
    }
}

// Add a number of armies to the current territory
void Territory::addArmies(int armies)
{
    numberOfArmies_ += armies;
}

// Add a number of armies pending deployment to the current territory
void Territory::addPendingIncomingArmies(int armies)
{
    pendingIncomingArmies_ += armies;
}

// Mark a number of armies that will be moving off the current territory
void Territory::addPendingOutgoingArmies(int armies)
{
    pendingOutgoingArmies_ += armies;
}


/* 
===================================
 Implementation for Continent class
===================================
 */

// Constructors
Continent::Continent(): name_("unknown_continent"), controlValue_(0) {}

Continent::Continent(string name, int controlValue) : name_(name), controlValue_(controlValue) {}

Continent::Continent(const Continent &continent) : name_(continent.name_), controlValue_(continent.controlValue_)
{
    for (const auto &territory : continent.territories_)
    {
        territories_.push_back(new Territory(*territory));
    }
}

// Destructor
Continent::~Continent()
{
    for (const auto &territory : territories_)
    {
        delete territory;
    }
    territories_.clear();
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
    output << "[Continent]: " << continent.name_ << ", " << continent.controlValue_ << " Control Value, " << continent.territories_.size() << " Territories";
    return output;
}

// Getters
string Continent::getName() const
{
    return name_;
}

int Continent::getControlValue() const
{
    return controlValue_;
}

vector<Territory*> Continent::getTerritories() const
{
    return territories_;
}

// Setters
void Continent::setName(string name)
{
    name_ = name;
}

void Continent::setControlValue(int value)
{
    controlValue_ = value;
}

void Continent::setTerritories(vector<Territory*> territories)
{
    territories_ = territories;
}

// Add a territory to the current continent.
void Continent::addTerritory(Territory* territory)
{
    territories_.push_back(territory);
}


/* 
===================================
 Implementation for Map class
===================================
 */

// Constructors
Map::Map() {}

Map::Map(vector<Continent*> continents, unordered_map<Territory*, vector<Territory*>> adjacencyList) : continents_(continents), adjacencyList_(adjacencyList) {}

Map::Map(const Map &map)
{
    copyMapContents(map);
}

// Destructor
Map::~Map()
{
    destroyMapContents();
}

// Operator overloading
const Map &Map::operator=(const Map &map)
{
    destroyMapContents();
    copyMapContents(map);
    return *this;
}

ostream &operator<<(ostream &output, const Map &map)
{
    output << "[Map]: " << map.adjacencyList_.size() << " Territories, " << map.continents_.size() << " Continents";
    return output;
}

// Getters
unordered_map<Territory*, vector<Territory*>> Map::getAdjacencyList() const
{
    return adjacencyList_;
}

vector<Continent*> Map::getContinents() const
{
    return continents_;
}

// Return a list of all the territories in the Map
vector<Territory*> Map::getTerritories() const
{
    vector<Territory*> territories;
    for (const auto &entry : adjacencyList_)
    {
        territories.push_back(entry.first);
    }

    return territories;
}

// Return a list of territories that are adjacent to the one specified
vector<Territory*> Map::getAdjacentTerritories(Territory* territory)
{
    return adjacencyList_[territory];
}

 // Check whether the map is valid or not according to the following:
 // 1. Map is a connected graph
 // 2. The continents are connected subgraphs
 // 3. Each territory belongs to only one continent
bool Map::validate()
{
    return checkGraphValidity() && checkContinentsValidity() && checkTerritoriesValidity();
}

 // Helper method to validate that the map is a connected graph.
 // This can be done by traversing the graph and comparing the size of visitied territories
 // to the size of all territories.
 // 
 // Graph traversal implemented using Breadth-First Search.
bool Map::checkGraphValidity()
{
    unordered_set<Territory*> visitedTerritories;
    queue<Territory*> territoryQueue;
    territoryQueue.push(adjacencyList_.begin()->first);

    while (!territoryQueue.empty())
    {
        Territory* current = territoryQueue.front();
        territoryQueue.pop();
        visitedTerritories.insert(current);

        for (const auto &territory : getAdjacentTerritories(current))
        {
            // If the territory hasn't been visited, add it to the queue
            if (visitedTerritories.find(territory) == visitedTerritories.end())
            {
                territoryQueue.push(territory);
            }
        }
    }

    // If # of visited territories < # of all territories, the map is not a connected graph
    return visitedTerritories.size() == adjacencyList_.size();
}

// Helper method to validate that the map's continents are connected subgraphs.
bool Map::checkContinentsValidity()
{
    for (const auto &continent : getContinents())
    {
        unordered_set<Territory*> visitedTerritories;
        queue<Territory*> territoryQueue;

        vector<Territory*> continentMembers = continent->getTerritories();
        territoryQueue.push(continentMembers.front());

        while (!territoryQueue.empty())
        {
            Territory* current = territoryQueue.front();
            territoryQueue.pop();
            visitedTerritories.insert(current);

            // If the territory isn't a member of the set of all territories in the map, then continent is not a subgraph
            if (adjacencyList_.find(current) == adjacencyList_.end())
            {
                return false;
            }

            for (const auto &territory : getAdjacentTerritories(current))
            {
                bool visited = visitedTerritories.find(territory) != visitedTerritories.end();
                bool inCurrentContinent = find(continentMembers.begin(), continentMembers.end(), territory) != continentMembers.end();
                if (inCurrentContinent && !visited)
                {
                    territoryQueue.push(territory);
                }
            }
        }

        // If # of visited territories in the continent is not the same as # of its territories, then continent is not connected
        if (continentMembers.size() != visitedTerritories.size())
        {
            return false;
        }
    }

    return true;
}

// Helper method to validate that the map's territories all belong to only one continent.
bool Map::checkTerritoriesValidity()
{
    unordered_set<Territory*> visitedTerritories;
    for (const auto &continent : getContinents())
    {
        for (const auto &territory : continent->getTerritories())
        {
            // If territory has not been visited, add to set
            if (visitedTerritories.find(territory) == visitedTerritories.end())
            {
                visitedTerritories.insert(territory);
            }
            else
            {
                // This continent has a territory that was already visited through another continent
                return false;
            }
        }
    }

    return true;
}

// Helper method to copy the contents of another Map object. 
void Map::copyMapContents(const Map &map)
{
    // Copy the continents
    for (const auto &continent : map.continents_)
    {
        continents_.push_back(new Continent(*continent));
    }

    // Get all the territories from the continents
    for (const auto &continent : continents_)
    {
        for (const auto &territory : continent->getTerritories())
        {
            adjacencyList_[territory];
        }
    }

    // Rebuild the adjacency list using `map.adjacencyList_` as reference
    for (const auto &entry : map.adjacencyList_)
    {
        auto matchingEntry = [&entry](const auto &kvp) { return *kvp.first == *entry.first; };
        auto iterator = find_if(adjacencyList_.begin(), adjacencyList_.end(), matchingEntry);
        Territory* territoryKey = iterator->first;

        for (const auto &territoryToCopy : entry.second)
        {
            auto matchingTerritory = [&territoryToCopy](const auto &kvp) { return *kvp.first == *territoryToCopy; };
            auto matchingTerritoryIterator = find_if(adjacencyList_.begin(), adjacencyList_.end(), matchingTerritory);
            adjacencyList_[territoryKey].push_back(matchingTerritoryIterator->first);
        }
    }
}

// Helper method to dealloacte dynamic memory in Map class. 
void Map::destroyMapContents()
{
    for (const auto &continent : continents_)
    {
        delete continent;
    }
    continents_.clear();
    adjacencyList_.clear();
}