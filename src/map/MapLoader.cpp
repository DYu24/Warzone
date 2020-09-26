#include "MapLoader.h"
#include "Map.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
using namespace std;

namespace
{
    void skipToSection(string section, ifstream &stream)
    {
        string line;
        while (getline(stream, line) && line != section)
        {
        }
        if (stream.eof())
        {
            cout << "Invalid map file: missing `" << section << "` section." << endl;
            exit(1);
        }
    }

    /*
     * Reads the input `.map` file and populates the Map object with continents.
     */
    void populateContinents(ifstream &stream, Map &map)
    {
        vector<shared_ptr<Continent>> continents = map.getContinents();
        string line;
        istringstream ss;

        // Skip lines until we reach the continents section
        skipToSection("[continents]", stream);

        while (getline(stream, line) && line != "")
        {
            ss.clear();
            ss.str(line);

            string name;
            int continentValue;
            string color;
            ss >> name >> continentValue >> color;

            string *namePointer = new string(name);
            int *continentValuePointer = new int(continentValue);

            shared_ptr<Continent> continent = make_shared<Continent>(namePointer, continentValuePointer);
            continents.push_back(continent);
        }

        map.setContinents(continents);
    }

    /*
     * Reads the input `.map` file and populates the Map object with territories.
     */
    void populateTerritories(ifstream &stream, Map &map)
    {
        vector<shared_ptr<Territory>> territories = map.getAdjacencyList();
        vector<shared_ptr<Continent>> continents = map.getContinents();
        string line;
        istringstream ss;

        // Skip lines until we reach the continents section
        skipToSection("[countries]", stream);
        while (getline(stream, line) && line != "")
        {
            ss.clear();
            ss.str(line);

            int index;
            string name;
            int continent;
            ss >> index >> name >> continent;

            shared_ptr<Territory> territory = make_shared<Territory>(new string(name));
            territories.push_back(territory);

            shared_ptr<Continent> continentPointer = continents.at(continent - 1);
            continentPointer->addTerritory(territory);
        }

        map.setAdjacencyList(territories);
    }

    /*
     * Reads the input `.map` file and connects the territories to each other.
     */
    void populateBorders(ifstream &stream, Map &map)
    {
        vector<shared_ptr<Territory>> territories = map.getAdjacencyList();
        string line;
        istringstream ss;

        // Skip lines until we reach the continents section
        skipToSection("[borders]", stream);
        while (getline(stream, line) && line != "")
        {
            ss.clear();
            ss.str(line);

            int countryIndex;
            ss >> countryIndex;
            shared_ptr<Territory> territoryPointer = territories.at(countryIndex - 1);

            int adjacentIndex;
            while (ss >> adjacentIndex)
            {
                shared_ptr<Territory> adjacentTerritory = territories.at(adjacentIndex - 1);
                territoryPointer->addAdjacentTerritory(adjacentTerritory);
            }
        }
    }
} // namespace

Map MapLoader::loadMap(string filename)
{
    ifstream mapFile(filename);

    Map map;

    if (mapFile.is_open())
    {
        populateContinents(mapFile, map);
        populateTerritories(mapFile, map);
        populateBorders(mapFile, map);

        mapFile.close();

        if (map.validate())
        {
            return map;
        }

        cout << "Invalid map. Exiting.";
    }
    else
    {
        cout << "Unable to open file. Exiting.";
    }

    exit(1);
};