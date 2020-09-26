#pragma once

#include "Map.h"
#include <iostream>


class MapLoader {
    public:
        static Map loadMap(std::string filename);
};