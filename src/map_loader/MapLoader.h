#pragma once

#include "../map/Map.h"
#include <iostream>

class MapLoader {
    public:
        static Map loadMap(std::string filename);
};