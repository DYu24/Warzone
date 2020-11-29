#include "../game_engine/GameEngine.h"
#include "Map.h"
#include <iostream>
using std::boolalpha;
using std::cout;
using std::endl;

int main()
{
    {
        Territory* t1 = new Territory("Territory1");
        Territory* t2 = new Territory("Territory2");
        Territory* t3 = new Territory("Territory3");
        Territory* t4 = new Territory("Territory4");

        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);


        // ====== Create a valid map ======
        unordered_map<Territory*, vector<Territory*>> adjacencyList{
            {t1, {t2}},
            {t2, {t1, t3}},
            {t3, {t2, t4}},
            {t4, {t3}}
        };

        c1->setTerritories({ t1, t2 });
        c2->setTerritories({ t3, t4 });

        Map map1 = Map({c1, c2}, adjacencyList);
        cout << boolalpha << "Map 1: VALID = " << map1.validate() << endl;
        cout << "All is good :)" << endl;

        cout << endl;
    }

    // ====== Create invalid maps ======
    {

        // ------ Map is not a connected graph ------
        // Add a lone territory
        Territory* t1 = new Territory("Territory1");
        Territory* t2 = new Territory("Territory2");
        Territory* t3 = new Territory("Territory3");

        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);

        unordered_map<Territory*, vector<Territory*>> adjacencyList{
            {t1, {t2}},
            {t2, {t1}},
            {t3, {}},
        };

        c1->setTerritories({ t1, t2 });
        c2->setTerritories({ t3 });

        Map map2 = Map({c1, c2}, adjacencyList);
        cout << boolalpha << "Map 2: VALID = " << map2.validate() << endl;
        cout << "This map is not a connected graph." << endl;

        cout << endl;
    }

    {
        // ------ Continent(s) is not a connected graph ------
        // Add a lone territory within a continent
        Territory* t1 = new Territory("Territory1");
        Territory* t2 = new Territory("Territory2");
        Territory* t3 = new Territory("Territory3");
        Territory* t4 = new Territory("Territory4");

        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);

        c1->setTerritories({t1, t2, t3});
        c2->setTerritories({t4});

        unordered_map<Territory*, vector<Territory*>> adjacencyList{
            {t1, {t2}},
            {t2, {t1}},
            {t3, {t4}},
            {t4, {t3}}
        };

        Map map3 = Map({c1, c2}, adjacencyList);
        cout << boolalpha << "Map 3: VALID = " << map3.validate() << endl;
        cout << "A continent in this map is not a connected graph." << endl;

        cout << endl;
    }

    {
        // ------ Continent(s) is not a subgraph of the Map ------
        Territory* t1 = new Territory("Territory1");
        Territory* t2 = new Territory("Territory2");
        Territory* t3 = new Territory("Territory3");
        Territory* t4 = new Territory("Territory4");

        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);

        c1->setTerritories({t1, t2, t3});
        c2->setTerritories({t4});

        unordered_map<Territory*, vector<Territory*>> adjacencyList{
            {t1, {t2}},
            {t2, {t1, t4}},
            {t4, {t2}}
        };

        Map map4 = Map({c1, c2}, adjacencyList);
        cout << boolalpha << "Map 4: VALID = " << map4.validate() << endl;
        cout << "A continent in this map is not a subgraph of the whole map." << endl;

        cout << endl;

    }

    {
        // ------ Territory belonging to more than one continent ------
        Territory* t1 = new Territory("Territory1");

        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);

        c1->setTerritories({t1});
        c2->setTerritories({t1});

        unordered_map<Territory*, vector<Territory*>> adjacencyList{
            {t1, {}},
        };

        Map map5 = Map({c1, c2}, adjacencyList);
        cout << boolalpha << "Map 5: VALID = " << map5.validate() << endl;
        cout << "Multiple continents contain the same territory." << endl;

        cout << endl;

        // Set the first continent's territories to empty vector so that the end of this driver can properly clean up this last Map object.
        c1->setTerritories({});
    }

    GameEngine::resetGameEngine();

    return 0;
}