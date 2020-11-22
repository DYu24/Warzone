#include "MapLoader.h"
#include "../map/Map.h"
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

namespace
{
    // Skips lines of the input stream until the specified section is reached.
    void skipToSection(string section, ifstream &stream)
    {
        string line;
        while (getline(stream, line) && line != section) {}
        if (stream.eof())
        {
            throw "Invalid map file: missing `" + section + "` section.";
        }
    }

    // Trims whitespaces from a string
    string trim(const string& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    // Capitalize all characters in the provided string
    string capitalize(string str)
    {
        transform(str.begin(), str.end(),str.begin(), ::toupper);
        return str;
    }
}


/* 
===================================
 Implementation for MapLoader class
===================================
 */

// Reads the input `.map` file and populates the Map object with continents.
vector<Continent*> MapLoader::getContinents(ifstream &stream)
{
    vector<Continent*> continents;
    string line;
    istringstream ss;

    skipToSection("[continents]", stream);

    while (getline(stream, line) && line != "")
    {
        // Skip commented lines
        if (line.rfind(";", 0) == 0)
        {
            continue;
        }

        ss.clear();
        ss.str(line);

        string name;
        int continentValue;
        string color;
        ss >> name >> continentValue >> color;

        continents.push_back(new Continent(name, continentValue));
    }

    return continents;
}

// Reads the input `.map` file and populates the Map object with territories.
vector<Territory*> MapLoader::getTerritories(ifstream &stream, vector<Continent*> &continents)
{
    vector<Territory*> territories;
    string line;
    istringstream ss;

    skipToSection("[countries]", stream);
    while (getline(stream, line) && line != "")
    {
        // Skip commented lines
        if (line.rfind(";", 0) == 0)
        {
            continue;
        }

        ss.clear();
        ss.str(line);

        int index;
        string name;
        int continent;
        ss >> index >> name >> continent;

        Territory *territory = new Territory(name);
        territories.push_back(territory);

        continents.at(continent - 1)->addTerritory(territory);
    }

    return territories;
}

// Reads the input `.map` file and connects the territories to each other.
unordered_map<Territory*, vector<Territory*>> MapLoader::buildAdjacencyList(ifstream &stream, vector<Territory*> territories)
{
    unordered_map<Territory*, vector<Territory*>> adjacencyList;
    string line;
    istringstream ss;

    skipToSection("[borders]", stream);
    while (getline(stream, line) && line != "")
    {
        // Skip commented lines
        if (line.rfind(";", 0) == 0)
        {
            continue;
        }

        ss.clear();
        ss.str(line);

        int countryIndex;
        ss >> countryIndex;
        Territory* territoryPointer = territories.at(countryIndex - 1);

        int adjacentIndex;
        while (ss >> adjacentIndex)
        {
            Territory* adjacentTerritory = territories.at(adjacentIndex - 1);
            adjacencyList[territoryPointer].push_back(adjacentTerritory);
        }
    }

    return adjacencyList;
}

// Read the input `.map` file and generate a Map instance.
Map* MapLoader::loadMap(string filename)
{
    ifstream mapFile(filename);

    if (mapFile.is_open())
    {
        vector<Continent*> continents = getContinents(mapFile);
        vector<Territory*> territories = getTerritories(mapFile, continents);
        unordered_map<Territory*, vector<Territory*>> adjacencyList = buildAdjacencyList(mapFile, territories);

        Map* map = new Map(continents, adjacencyList);

        mapFile.close();

        if (!map->validate())
        {
            delete map;
            map = nullptr;
            throw "Invalid map structure.";
        }

        cout << "Map successfully loaded." << endl;
        return map;
    }

    throw "Unable to open map file";
};


/* 
===================================
 Implementation for ConquestFileReader class
===================================
 */

// Reads the input `.map` file and populates the Map object with continents.
unordered_map<string, Continent*> ConquestFileReader::getContinents(ifstream &stream)
{
    unordered_map<string, Continent*> continents;
    string line;
    istringstream ss;

    skipToSection("[Continents]", stream);
    while (getline(stream, line) && line != "")
    {
        ss.clear();
        ss.str(line);

        string token;
        string name;
        int continentValue;

        while (getline(ss, token, '='))
        {
            token = trim(token);
            if (token.length() == 1)
            {
                continentValue = stoi(token);
            }
            else
            {
                name = token;
            }
        }

        continents[capitalize(name)] = new Continent(name, continentValue);
    }

    return continents;
}

// Reads the input `.map` file and connects the territories to each other.
unordered_map<Territory*, vector<Territory*>> ConquestFileReader::buildAdjacencyList(ifstream &stream, unordered_map<string, Continent*> &continents)
{
    unordered_map<Territory*, vector<Territory*>> adjacencyList;
    unordered_map<Territory*, vector<string>> adjacentTerritories;
    unordered_map<string, Territory*> territoryPointers;
    string line;
    istringstream ss;

    skipToSection("[Territories]", stream);
    while (getline(stream, line))
    {
        ss.clear();
        ss.str(line);

        int idx = 0;
        string token;
        Territory* currentTerritory;
        while (getline(ss, token, ','))
        {
            token = trim(token);
            switch (idx)
            {
                // First column of data is the territory name
                case 0:
                    currentTerritory = new Territory(token);
                    territoryPointers[capitalize(token)] = currentTerritory;
                    break;
                // Second and third column of data is GUI coordinates. Skip
                case 1:
                case 2:
                    break;
                // Fourth column of data is the continent to which this territory belongs to
                case 3:
                    continents[capitalize(token)]->addTerritory(currentTerritory);
                    break;
                // Rest of the columns are the adjacent territories
                default:
                    adjacentTerritories[currentTerritory].push_back(capitalize(token));
            }

            idx++;
        }
    }

    // build the actual adjacency list with real Territory objects
    for (auto &entry : adjacentTerritories)
    {
        for (const auto &territoryName : entry.second)
        {
            Territory* neighbor = territoryPointers[territoryName];
            adjacencyList[entry.first].push_back(neighbor);
        }
    }

    return adjacencyList;
}


// Read the input `.map` conquest file and generate a Map instance.
Map* ConquestFileReader::readConquestFile(string filename)
{
    ifstream mapFile(filename);

    if (mapFile.is_open())
    {
        unordered_map<string, Continent*> continentMap = getContinents(mapFile);
        unordered_map<Territory*, vector<Territory*>> adjacencyList = buildAdjacencyList(mapFile, continentMap);

        vector<Continent*> continents;
        for (const auto &entry : continentMap)
        {
            continents.push_back(entry.second);
        }

        Map* map = new Map(continents, adjacencyList);

        mapFile.close();

        if (!map->validate())
        {
            delete map;
            map = nullptr;
            throw "Invalid map structure.";
        }

        cout << "Map successfully loaded." << endl;
        return map;
    }

    throw "Unable to open map file";
}


/* 
===================================
 Implementation for ConquestFileReaderAdapter class
===================================
 */

// Constructors
ConquestFileReaderAdapter::ConquestFileReaderAdapter() : fileReader_(new ConquestFileReader()) {}

ConquestFileReaderAdapter::ConquestFileReaderAdapter(const ConquestFileReaderAdapter &adapter) : fileReader_(new ConquestFileReader()) {}

// Destructor
ConquestFileReaderAdapter::~ConquestFileReaderAdapter()
{
    delete fileReader_;
    fileReader_ = nullptr;
}

// Operator overloading
const ConquestFileReaderAdapter &ConquestFileReaderAdapter::operator=(const ConquestFileReaderAdapter &adapter)
{
    delete fileReader_;
    fileReader_ = new ConquestFileReader();
    return *this;
}

// Read the input `.map` file and generate a Map instance.
Map* ConquestFileReaderAdapter::loadMap(string filename)
{
    return fileReader_->readConquestFile(filename);
}