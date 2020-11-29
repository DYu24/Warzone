#include "../game_engine/GameEngine.h"
#include "MapLoader.h"
#include <utility>
#include <string>
#include <vector>

int main()
{
    // Setup
    std::vector<std::pair<std::string, std::string>> maps;

    // Valid maps
    maps.push_back(std::make_pair("canada", "CANADA MAP"));
    maps.push_back(std::make_pair("europass", "EUROPASS MAP"));
    maps.push_back(std::make_pair("solar", "SOLAR MAP"));
    maps.push_back(std::make_pair("NYC", "NEW YORK CITY MAP"));

    // Invalid maps
    maps.push_back(std::make_pair("disconnected_graph", "INVALID MAP: UNCONNECTED GRAPH"));
    maps.push_back(std::make_pair("duplicated_territory", "INVALID MAP: TERRITORY BELONGING IN MORE THAN ONE CONTINENT"));
    maps.push_back(std::make_pair("disconnected_continent", "INVALID MAP: CONTINENT IS NOT A CONNECTED SUBGRAPH"));
    maps.push_back(std::make_pair("empty", "INVALID MAP: EMPTY FILE"));

    MapLoader mapLoader;
    for (const auto &mapPair : maps)
    {
        try
        {
            std::cout << "-----" << mapPair.second << "-----" << std::endl;
            Map* map = mapLoader.loadMap("resources/" + mapPair.first + ".map");
            std::cout << *map << "\n" << std::endl;
            delete map;
        }
        catch (char const *errorMessage)
        {
            std::cout << errorMessage << "\n" << std::endl;
        }
        catch (std::string const errorMessage)
        {
            std::cout << errorMessage << "\n" << std::endl;
        }
    }

    GameEngine::resetGameEngine();

    return 0;
};