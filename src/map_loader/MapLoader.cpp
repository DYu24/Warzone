#include "MapLoader.h"
#include "../map/Map.h"
#include <fstream>
#include <memory>
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
    void populateContinents(ifstream &stream, Map &map)
    {
        vector<unique_ptr<Continent>> continents;
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

            unique_ptr<string> namePointer = make_unique<string>(name);
            unique_ptr<int> continentValuePointer = make_unique<int>(continentValue);

            unique_ptr<Continent> continent = make_unique<Continent>(move(namePointer), move(continentValuePointer));
            continents.push_back(move(continent));
        }

        map.setContinents(continents);
    }

    // Reads the input `.map` file and populates the Map object with territories.
    void populateTerritories(ifstream &stream, Map &map)
    {
        vector<shared_ptr<Territory>> territories = map.getAdjacencyList();
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

            shared_ptr<Territory> territory = make_shared<Territory>(make_unique<string>(name));
            territories.push_back(territory);

            map.getContinents().at(continent - 1)->addTerritory(territory);
        }

        map.setAdjacencyList(territories);
    }

    // Reads the input `.map` file and connects the territories to each other.
    void populateBorders(ifstream &stream, Map &map)
    {
        vector<shared_ptr<Territory>> territories = map.getAdjacencyList();
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
            shared_ptr<Territory> territoryPointer = territories.at(countryIndex - 1);

            int adjacentIndex;
            while (ss >> adjacentIndex)
            {
                shared_ptr<Territory> adjacentTerritory = territories.at(adjacentIndex - 1);
                territoryPointer->addAdjacentTerritory(adjacentTerritory);
            }
        }
    }
}

// Read the input `.map` file and generate a Map instance.
Map MapLoader::loadMap(string filename)
{
    cout << "Loading map..." << endl;
    ifstream mapFile(filename);

    Map map;

    if (mapFile.is_open())
    {
        populateContinents(mapFile, map);
        populateTerritories(mapFile, map);
        populateBorders(mapFile, map);

        mapFile.close();

        if (!map.validate())
        {
            throw "Invalid map structure.";
        }

        cout << "Map successfully loaded." << endl;
        return map;
    }

    throw "Unable to open map file";
};