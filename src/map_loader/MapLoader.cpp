#include "MapLoader.h"
#include "../map/Map.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace
{
    // Skips lines of the input stream until the specified section is reached.
    void skipToSection(std::string section, std::ifstream &stream)
    {
        std::string line;
        while (getline(stream, line) && line != section) {}
        if (stream.eof())
        {
            throw "Invalid map file: missing `" + section + "` section.";
        }
    }

    // Trims whitespaces from a string
    std::string trim(const std::string& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    // Capitalize all characters in the provided string
    std::string capitalize(std::string str)
    {
        std::transform(str.begin(), str.end(),str.begin(), ::toupper);
        return str;
    }
}


/* 
===================================
 Implementation for MapLoader class
===================================
 */

// Destructor
MapLoader::~MapLoader() {}

// Reads the input `.map` file and returns a list continents.
std::vector<Continent*> MapLoader::getContinents(std::ifstream &stream)
{
    std::vector<Continent*> continents;
    std::string line;
    std::istringstream ss;

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

        std::string name;
        int continentValue;
        std::string color;
        ss >> name >> continentValue >> color;

        continents.push_back(new Continent(name, continentValue));
    }

    return continents;
}

// Reads the input `.map` file and returns a list of territories.
std::vector<Territory*> MapLoader::getTerritories(std::ifstream &stream, std::vector<Continent*> &continents)
{
    std::vector<Territory*> territories;
    std::string line;
    std::istringstream ss;

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
        std::string name;
        int continent;
        ss >> index >> name >> continent;

        Territory *territory = new Territory(name);
        territories.push_back(territory);

        continents.at(continent - 1)->addTerritory(territory);
    }

    return territories;
}

// Reads the input `.map` file and connects the territories to each other.
std::unordered_map<Territory*, std::vector<Territory*>> MapLoader::buildAdjacencyList(std::ifstream &stream, std::vector<Territory*> territories)
{
    std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList;
    std::string line;
    std::istringstream ss;

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
Map* MapLoader::loadMap(std::string filename)
{
    std::ifstream mapFile(filename);

    if (mapFile.is_open())
    {
        std::vector<Continent*> continents = getContinents(mapFile);
        std::vector<Territory*> territories = getTerritories(mapFile, continents);
        std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList = buildAdjacencyList(mapFile, territories);

        Map* map = new Map(continents, adjacencyList);

        mapFile.close();

        if (!map->validate())
        {
            delete map;
            map = nullptr;
            throw "Invalid map structure.";
        }

        std::cout << "Map successfully loaded." << std::endl;
        return map;
    }

    throw "Unable to open map file";
};


/* 
===================================
 Implementation for ConquestFileReader class
===================================
 */

// Reads the input `.map` file and returns a list continents.
std::unordered_map<std::string, Continent*> ConquestFileReader::getContinents(std::ifstream &stream)
{
    std::unordered_map<std::string, Continent*> continents;
    std::string line;
    std::istringstream ss;

    skipToSection("[Continents]", stream);
    while (getline(stream, line) && line != "")
    {
        ss.clear();
        ss.str(line);

        std::string token;
        std::string name;
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

// Reads the input `.map` file and returns an adjacency list of all the territories.
std::unordered_map<Territory*, std::vector<Territory*>> ConquestFileReader::buildAdjacencyList(std::ifstream &stream, std::unordered_map<std::string, Continent*> &continents)
{
    std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList;
    std::unordered_map<Territory*, std::vector<std::string>> adjacentTerritories;
    std::unordered_map<std::string, Territory*> territoryPointers;
    std::istringstream ss;
    std::string line;

    skipToSection("[Territories]", stream);
    while (getline(stream, line))
    {
        ss.clear();
        ss.str(line);

        int column = 0;
        std::string token;
        Territory* currentTerritory;
        while (getline(ss, token, ','))
        {
            token = trim(token);
            switch (column)
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
                // Fourth column of data is the continent to which this territory belongs
                case 3:
                    continents[capitalize(token)]->addTerritory(currentTerritory);
                    break;
                // Rest of the columns are the adjacent territories
                default:
                    adjacentTerritories[currentTerritory].push_back(capitalize(token));
            }

            column++;
        }
    }

    // build the actual adjacency list with real Territory objects
    for (const auto &entry : adjacentTerritories)
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
Map* ConquestFileReader::readConquestFile(std::string filename)
{
    std::ifstream mapFile(filename);

    if (mapFile.is_open())
    {
        std::unordered_map<std::string, Continent*> continentMap = getContinents(mapFile);
        std::unordered_map<Territory*, std::vector<Territory*>> adjacencyList = buildAdjacencyList(mapFile, continentMap);

        std::vector<Continent*> continents;
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

        std::cout << "Map successfully loaded." << std::endl;
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
    if (this != &adapter)
    {
        delete fileReader_;
        fileReader_ = new ConquestFileReader();
    }
    return *this;
}

// Read the input `.map` file and generate a Map instance.
Map* ConquestFileReaderAdapter::loadMap(std::string filename)
{
    return fileReader_->readConquestFile(filename);
}