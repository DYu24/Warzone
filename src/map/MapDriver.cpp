#include "Map.h"
#include <iostream>
#include<memory>
#include <string>
using namespace std;

int main()
{
    shared_ptr<Territory> t1 = make_shared<Territory>(Territory(new string("Territory1")));
    shared_ptr<Territory> t2 = make_shared<Territory>(Territory(new string("Territory2")));
    shared_ptr<Territory> t3 = make_shared<Territory>(Territory(new string("Territory3")));
    shared_ptr<Territory> t4 = make_shared<Territory>(Territory(new string("Territory4")));

    shared_ptr<Continent> c1 = make_shared<Continent>(Continent(new string("Continent1"), new int(3)));
    shared_ptr<Continent> c2 = make_shared<Continent>(Continent(new string("Continent2"), new int(4)));

    // ====== Create a valid map ======
    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
    t2->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t2);
    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);
    vector<shared_ptr<Territory>> territories { t1, t2, t3, t4 };

    c1->addTerritory(t1);
    c1->addTerritory(t2);
    c2->addTerritory(t3);
    c2->addTerritory(t4);
    vector<shared_ptr<Continent>> continents { c1, c2 };

    Map map1;
    map1.setAdjacencyList(territories);
    map1.setContinents(continents);

    cout << boolalpha << "Map 1: VALID = " << map1.validate() << endl;

    // ====== Create invalid maps ======
    // ------ Map is not a connected graph ------
    Map map2;
    map2.setAdjacencyList(territories);
    map2.setContinents(continents);

    shared_ptr<Territory> t5 = make_shared<Territory>(Territory(new string("Territory5")));
    vector<shared_ptr<Territory>> adjacencyList = map2.getAdjacencyList();
    adjacencyList.push_back(t5);
    map2.setAdjacencyList(adjacencyList);

    cout << boolalpha << "Map 2: VALID = " << map2.validate() << endl;

    // ------ Continent(s) is not a connected graph ------
    Map map3;
    map3.setAdjacencyList(territories);
    map3.setContinents(continents);

    shared_ptr<Territory> t6 = make_shared<Territory>(Territory(new string("Territory6")));
    c1->addTerritory(t6);
    adjacencyList = map3.getAdjacencyList();
    adjacencyList.push_back(t6);
    map3.setAdjacencyList(adjacencyList);
    
    cout << boolalpha << "Map 3: VALID = " << map3.validate() << endl;

    // ------ Continent(s) is not a subgraph of the Map ------
    Map map4;
    map4.setAdjacencyList(territories);
    map4.setContinents(continents);

    shared_ptr<Territory> t7 = make_shared<Territory>(Territory(new string("Territory7")));
    c1->addTerritory(t7);
    
    cout << boolalpha << "Map 4: VALID = " << map4.validate() << endl;

    // ------ Territory belonging to more than one continent ------
    Map map5;
    map5.setAdjacencyList(territories);
    map5.setContinents(continents);

    c2->addTerritory(t1);
    
    cout << boolalpha << "Map 5: VALID = " << map5.validate() << endl;
}