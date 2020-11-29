#include "../game_engine/GameEngine.h"
#include "MapLoader.h"
#include <utility>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;

int main()
{
    // Setup
    vector<pair<string, string>> maps;

    // Valid maps
    maps.push_back(make_pair("canada", "CANADA MAP"));
    maps.push_back(make_pair("europass", "EUROPASS MAP"));
    maps.push_back(make_pair("solar", "SOLAR MAP"));
    maps.push_back(make_pair("NYC", "NEW YORK CITY MAP"));

    // Invalid maps
    maps.push_back(make_pair("disconnected_graph", "INVALID MAP: UNCONNECTED GRAPH"));
    maps.push_back(make_pair("duplicated_territory", "INVALID MAP: TERRITORY BELONGING IN MORE THAN ONE CONTINENT"));
    maps.push_back(make_pair("disconnected_continent", "INVALID MAP: CONTINENT IS NOT A CONNECTED SUBGRAPH"));
    maps.push_back(make_pair("empty", "INVALID MAP: EMPTY FILE"));

    MapLoader mapLoader;
    for (const auto &mapPair : maps)
    {
        try
        {
            cout << "-----" << mapPair.second << "-----" << endl;
            Map* map = mapLoader.loadMap("resources/" + mapPair.first + ".map");
            cout << *map << "\n" << endl;
            delete map;
        }
        catch (char const *errorMessage)
        {
            cout << errorMessage << "\n" << endl;
        }
        catch (string const errorMessage)
        {
            cout << errorMessage << "\n" << endl;
        }
    }

    GameEngine::resetGameEngine();

    return 0;
};