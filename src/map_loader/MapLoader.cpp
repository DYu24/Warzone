#include "MapLoader.h"
#include "../map/Map.h"
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

    // Reads the input `.map` file and populates the Map object with continents.
    vector<Continent*> getContinents(ifstream &stream)
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
    vector<Territory*> getTerritories(ifstream &stream, vector<Continent*> &continents)
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
    unordered_map<Territory*, vector<Territory*>> buildAdjacencyList(ifstream &stream, vector<Territory*> territories)
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
}

// Read the input `.map` file and generate a Map instance.
Map* MapLoader::loadMap(string filename)
{
    cout << "Loading map..." << endl;
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