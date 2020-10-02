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
Territory::Territory() : name_(make_unique<string>()), numberOfArmies_(make_unique<int>(0)), adjacentTerritories_(make_unique<vector<shared_ptr<Territory>>>()) {}

Territory::Territory(unique_ptr<string> name) : name_(move(name)), numberOfArmies_(make_unique<int>(0)), adjacentTerritories_(make_unique<vector<shared_ptr<Territory>>>()) {}

Territory::Territory(const Territory &territory)
{
    name_ = make_unique<string>(*territory.name_);
    numberOfArmies_ = make_unique<int>(*territory.numberOfArmies_);
    adjacentTerritories_ = make_unique<vector<shared_ptr<Territory>>>(*territory.adjacentTerritories_);
}

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
    name_ = make_unique<string>(*territory.name_);
    numberOfArmies_ = make_unique<int>(*territory.numberOfArmies_);
    adjacentTerritories_ = make_unique<vector<shared_ptr<Territory>>>(*territory.adjacentTerritories_);
    return *this;
}

ostream &operator<<(ostream &output, const Territory &territory)
{
    output << "[Territory]: " << *territory.name_ << ", " << *territory.numberOfArmies_ << " Armies, " << territory.adjacentTerritories_->size() << " Adjacent Territories";
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
Continent::Continent() : name_(make_unique<string>()), controlValue_(make_unique<int>(0)), territories_(make_unique<vector<shared_ptr<Territory>>>()) {}

Continent::Continent(unique_ptr<string> name, unique_ptr<int> controlValue) : name_(move(name)), controlValue_(move(controlValue)), territories_(make_unique<vector<shared_ptr<Territory>>>()) {}

Continent::Continent(const Continent &continent)
{
    name_ = make_unique<string>(*continent.name_);
    controlValue_ = make_unique<int>(*continent.controlValue_);
    territories_ = make_unique<vector<shared_ptr<Territory>>>(*continent.territories_);
}

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
    name_ = make_unique<string>(*continent.name_);
    controlValue_ = make_unique<int>(*continent.controlValue_);
    territories_ = make_unique<vector<shared_ptr<Territory>>>(*continent.territories_);
    return *this;
}

ostream &operator<<(ostream &output, const Continent &continent)
{
    output << "[Continent]: " << *continent.name_ << ", " << *continent.controlValue_ << " Control Value, " << continent.territories_->size() << " Territories";
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
Map::Map() : adjacencyList_(make_unique<vector<shared_ptr<Territory>>>()), continents_(make_unique<vector<unique_ptr<Continent>>>()) {}

Map::Map(const Map &map)
{
    adjacencyList_ = make_unique<vector<shared_ptr<Territory>>>(*map.adjacencyList_);
    setContinents(*map.continents_);
}

// Getters and Setters
vector<shared_ptr<Territory>> Map::getAdjacencyList()
{
    return *adjacencyList_;
}

void Map::setAdjacencyList(vector<shared_ptr<Territory>> &adjacencyList)
{
    *adjacencyList_ = adjacencyList;
}

vector<unique_ptr<Continent>> &Map::getContinents()
{
    return *continents_;
}

void Map::setContinents(vector<unique_ptr<Continent>> &continents)
{
    continents_ = make_unique<vector<unique_ptr<Continent>>>();
    for (auto const &continentPointer : continents)
    {
        continents_->push_back(make_unique<Continent>(*continentPointer));
    }
}

// Operator overloading
const Map &Map::operator=(const Map &map)
{
    adjacencyList_ = make_unique<vector<shared_ptr<Territory>>>(*map.adjacencyList_);
    setContinents(*map.continents_);
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
    for (auto const &continent : getContinents())
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

            // If the territory isn't a member of the set of all territories in the map, then continent is not a subgraph
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
    unordered_set<shared_ptr<Territory>> visitedTerritories;

    for (auto const &continent : getContinents())
    {
        for (auto const &territory : continent->getTerritories())
        {
            if (visitedTerritories.find(territory) == visitedTerritories.end())
            {
                visitedTerritories.insert(territory);
            }
            else
            {
                // This continent has a territory that was already visited through another continent.
                return false;
            }
        }
    }

    return true;
}