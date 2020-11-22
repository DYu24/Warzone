#pragma once

#include "../map/Map.h"
#include <iostream>
using namespace std;

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
