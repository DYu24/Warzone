#pragma once

#include "../map/Map.h"
#include <iostream>
using namespace std;

class MapLoader {
    public:
        static Map* loadMap(string filename);
};