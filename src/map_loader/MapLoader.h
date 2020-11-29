#pragma once

#include "../map/Map.h"
#include <iostream>
#include <vector>
#include <unordered_map>
using std::ifstream;
using std::ostream;
using std::unordered_map;
using std::vector;

class MapLoader
{
    public:
        virtual Map* loadMap(string filename);

    private:
        vector<Continent*> getContinents(ifstream &stream);
        vector<Territory*> getTerritories(ifstream &stream, vector<Continent*> &continents);
        unordered_map<Territory*, vector<Territory*>> buildAdjacencyList(ifstream &stream, vector<Territory*> territories);
};

class ConquestFileReader
{
    public:
        Map* readConquestFile(string filename);

    private:
        unordered_map<string, Continent*> getContinents(ifstream &stream);
        unordered_map<Territory*, vector<Territory*>> buildAdjacencyList(ifstream &stream, unordered_map<string, Continent*> &continents);
};

class ConquestFileReaderAdapter : public MapLoader
{
    public:
        ConquestFileReaderAdapter();
        ConquestFileReaderAdapter(const ConquestFileReaderAdapter &adapter);
        ~ConquestFileReaderAdapter();
        const ConquestFileReaderAdapter &operator=(const ConquestFileReaderAdapter &adapter);
        Map* loadMap(string filename);

    private:
        ConquestFileReader* fileReader_;
};
