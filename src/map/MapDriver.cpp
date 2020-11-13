#include "Map.h"
using namespace std;

int main()
{
    // Setup
    Territory* t1 = new Territory("Territory1");
    Territory* t2 = new Territory("Territory2");
    Territory* t3 = new Territory("Territory3");
    Territory* t4 = new Territory("Territory4");

    Continent* c1 = new Continent("Continent1", 3);
    Continent* c2 = new Continent("Continent2", 4);

    // ====== Create a valid map ======
    unordered_map<Territory*, vector<Territory*>> adjacencyList;
    adjacencyList[t1].push_back(t2);
    adjacencyList[t2].push_back(t1);
    adjacencyList[t2].push_back(t3);
    adjacencyList[t3].push_back(t2);
    adjacencyList[t3].push_back(t4);
    adjacencyList[t4].push_back(t3);

    c1->addTerritory(t1);
    c1->addTerritory(t2);
    c2->addTerritory(t3);
    c2->addTerritory(t4);
    vector<Continent*> continents { c1, c2 };

    Map map1 = Map(continents, adjacencyList);

    cout << boolalpha << "Map 1: VALID = " << map1.validate() << endl;


    // ====== Create invalid maps ======

    // ------ Map is not a connected graph ------
    // Add a lone territory
    unordered_map<Territory*, vector<Territory*>> invalidAdjacencyList = adjacencyList;
    Territory* t5 = new Territory("Territory5");
    invalidAdjacencyList[t5];
    Map map2 = Map(continents, invalidAdjacencyList);

    cout << boolalpha << "Map 2: VALID = " << map2.validate() << endl;


    // ------ Continent(s) is not a connected graph ------
    // Add a lone territory within a continent
    Territory* t6 = new Territory("Territory6");
    continents.front()->addTerritory(t6);
    invalidAdjacencyList = adjacencyList;
    invalidAdjacencyList[t6];
    Map map3 = Map(continents, invalidAdjacencyList);
    
    cout << boolalpha << "Map 3: VALID = " << map3.validate() << endl;


    // ------ Continent(s) is not a subgraph of the Map ------
    Territory* t7 = new Territory("Territory7");
    continents.front()->addTerritory(t7);
    Map map4 = Map(continents, adjacencyList);
    
    cout << boolalpha << "Map 4: VALID = " << map4.validate() << endl;


    // ------ Territory belonging to more than one continent ------
    continents.at(1)->addTerritory(t1);
    Map map5 = Map(continents, adjacencyList);

    cout << boolalpha << "Map 5: VALID = " << map5.validate() << endl;

    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    delete t6;
    delete t7;
    delete c1;
    delete c2;

    return 0;
}