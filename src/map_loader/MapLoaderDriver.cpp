#include "MapLoader.h"
#include <utility>
#include <string>
#include <vector>
using namespace std;

int main()
{
    vector<pair<string, string>> maps;

    // Valid maps
    maps.push_back(make_pair("canada", "CANADA MAP"));
    maps.push_back(make_pair("europass", "EUROPASS MAP"));

    // Invalid maps
    maps.push_back(make_pair("disconnected_graph", "INVALID MAP: UNCONNECTED GRAPH"));
    maps.push_back(make_pair("duplicated_territory", "INVALID MAP: TERRITORY BELONGING IN MORE THAN ONE CONTINENT"));
    maps.push_back(make_pair("disconnected_continent", "INVALID MAP: CONTINENT IS NOT A CONNECTED SUBGRAPH"));

    for (auto const &m : maps)
    {
        try
        {
            cout << "-----" << m.second << "-----" << endl;
            MapLoader::loadMap("resources/" + m.first + ".map");
            cout << endl;
        }
        catch (char const *errorMessage)
        {
            cout << errorMessage << "\n"
                 << endl;
        }
    }

    return 0;
};