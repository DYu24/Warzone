#include "Map.h"
#include <iostream>
#include<memory>
#include <string>
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
    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
    t2->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t2);
    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);
    vector<Territory*> territories { t1, t2, t3, t4 };

    c1->addTerritory(t1);
    c1->addTerritory(t2);
    c2->addTerritory(t3);
    c2->addTerritory(t4);
    vector<Continent*> continents { c1, c2 };

    Map map1;
    map1.setAdjacencyList(territories);
    map1.setContinents(continents);

    cout << boolalpha << "Map 1: VALID = " << map1.validate() << endl;

    // ====== Create invalid maps ======

    // ------ Map is not a connected graph ------
    Map map2;
    map2.setAdjacencyList(territories);
    map2.setContinents(continents);

    // Add a lone territory
    Territory* t5 = new Territory("Territory5");
    vector<Territory*> adjacencyList = map2.getAdjacencyList();
    adjacencyList.push_back(t5);
    map2.setAdjacencyList(adjacencyList);

    cout << boolalpha << "Map 2: VALID = " << map2.validate() << endl;


    // ------ Continent(s) is not a connected graph ------
    Map map3;
    map3.setAdjacencyList(territories);
    map3.setContinents(continents);

    // Add a lone territory within a continent
    Territory* t6 = new Territory("Territory6");
    map3.getContinents().at(0)->addTerritory(t6);
    adjacencyList = map3.getAdjacencyList();
    adjacencyList.push_back(t6);
    map3.setAdjacencyList(adjacencyList);
    
    cout << boolalpha << "Map 3: VALID = " << map3.validate() << endl;


    // ------ Continent(s) is not a subgraph of the Map ------
    Map map4;
    map4.setAdjacencyList(territories);
    map4.setContinents(continents);

    Territory* t7 = new Territory("Territory7");
    map4.getContinents().at(0)->addTerritory(t7);
    
    cout << boolalpha << "Map 4: VALID = " << map4.validate() << endl;


    // ------ Territory belonging to more than one continent ------
    Map map5;
    map5.setAdjacencyList(territories);
    map5.setContinents(continents);

    map5.getContinents().at(1)->addTerritory(t1);
    
    cout << boolalpha << "Map 5: VALID = " << map5.validate() << endl;

    return 0;
}