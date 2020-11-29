#pragma once

#include "../map/Map.h"
#include <iostream>
#include <vector>
#include <unordered_map>


class MapLoader
{
    public:
        virtual ~MapLoader();
        virtual Map* loadMap(std::string filename);

    private:
        std::vector<Continent*> getContinents(std::ifstream &stream);
        std::vector<Territory*> getTerritories(std::ifstream &stream, std::vector<Continent*> &continents);
        std::unordered_map<Territory*, std::vector<Territory*>> buildAdjacencyList(std::ifstream &stream, std::vector<Territory*> territories);
};

class ConquestFileReader
{
    public:
        Map* readConquestFile(std::string filename);

    private:
        std::unordered_map<std::string, Continent*> getContinents(std::ifstream &stream);
        std::unordered_map<Territory*, std::vector<Territory*>> buildAdjacencyList(std::ifstream &stream, std::unordered_map<std::string, Continent*> &continents);
};

class ConquestFileReaderAdapter : public MapLoader
{
    public:
        ConquestFileReaderAdapter();
        ConquestFileReaderAdapter(const ConquestFileReaderAdapter &adapter);
        ~ConquestFileReaderAdapter();
        const ConquestFileReaderAdapter &operator=(const ConquestFileReaderAdapter &adapter);
        Map* loadMap(std::string filename);

    private:
        ConquestFileReader* fileReader_;
};
