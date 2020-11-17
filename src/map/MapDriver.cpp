#include "Map.h"
using namespace std;

namespace 
{
    vector<Territory*> copyTerritories(int numberOfTerritories)
    {
        vector<Territory*> territories;
        for (int i = 0; i < numberOfTerritories; i++)
        {
            territories.push_back(new Territory("Territory" + i));
        }
        return territories;
    }

    vector<Continent*> generateContinents()
    {
        Continent* c1 = new Continent("Continent1", 3);
        Continent* c2 = new Continent("Continent2", 4);
        return {c1, c2};
    }

    unordered_map<Territory*, vector<Territory*>> generateAdjacencyList()
    {
        Territory* t1 = new Territory("Territory1");
        Territory* t2 = new Territory("Territory2");
        Territory* t3 = new Territory("Territory3");
        Territory* t4 = new Territory("Territory4");

        unordered_map<Territory*, vector<Territory*>> adjacencyList;
        adjacencyList[t1].push_back(t2);
        adjacencyList[t2].push_back(t1);
        adjacencyList[t2].push_back(t3);
        adjacencyList[t3].push_back(t2);
        adjacencyList[t3].push_back(t4);
        adjacencyList[t4].push_back(t3);

        return adjacencyList;
    }
}

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
    unordered_map<Territory*, vector<Territory*>> adjacencyList{
        {t1, {t2}},
        {t2, {t1, t3}},
        {t3, {t2, t4}},
        {t4, {t3}}
    };

    c1->addTerritory(t1);
    c1->addTerritory(t2);
    c2->addTerritory(t3);
    c2->addTerritory(t4);
    vector<Continent*> continents {c1, c2};

    Map map1 = Map(continents, adjacencyList);
    cout << boolalpha << "Map 1: VALID = " << map1.validate() << endl;
    cout << "All is good :)" << endl;

    cout << endl;


    // ====== Create invalid maps ======

    // ------ Map is not a connected graph ------
    // Add a lone territory
    Territory* t5 = new Territory("Territory5");
    Territory* t6 = new Territory("Territory6");
    Territory* t7 = new Territory("Territory7");

    unordered_map<Territory*, vector<Territory*>> invalidAdjacencyList{
        {t5, {t6}},
        {t6, {t5}},
        {t7, {}}
    };

    Map map2 = Map(generateContinents(), invalidAdjacencyList);
    cout << boolalpha << "Map 2: VALID = " << map2.validate() << endl;
    cout << "This map is not a connected graph." << endl;

    cout << endl;


    // ------ Continent(s) is not a connected graph ------
    // Add a lone territory within a continent
    Territory* t8 = new Territory("Territory8");
    Territory* t9 = new Territory("Territory9");
    Territory* t10 = new Territory("Territory10");
    Territory* t11 = new Territory("Territory11");
    
    vector<Continent*> invalidContinents = generateContinents();
    invalidContinents.front()->setTerritories({t8, t9, t10});
    invalidContinents.back()->setTerritories({t11});

    unordered_map<Territory*, vector<Territory*>> invalidAdjacencyList2{
        {t8, {t9}},
        {t9, {t8}},
        {t10, {t11}},
        {t11, {t10}}
    };

    Map map3 = Map(invalidContinents, invalidAdjacencyList2);
    cout << boolalpha << "Map 3: VALID = " << map3.validate() << endl;
    cout << "A continent in this map is not a connected graph." << endl;

    cout << endl;


    // ------ Continent(s) is not a subgraph of the Map ------
    Territory* t12 = new Territory("Territory12");
    Territory* t13 = new Territory("Territory13");
    Territory* t14 = new Territory("Territory14");
    Territory* t15 = new Territory("Territory15");


    vector<Continent*> invalidContinents2 = generateContinents();
    invalidContinents2.front()->setTerritories({t12, t13});
    invalidContinents2.back()->setTerritories({t14, t15});

    unordered_map<Territory*, vector<Territory*>> adjacencyList2{
        {t12, {t13}},
        {t13, {t12, t14}},
        {t14, {t13}},
    };

    Map map4 = Map(invalidContinents2, adjacencyList2);
    cout << boolalpha << "Map 4: VALID = " << map4.validate() << endl;
    cout << "A continent in this map is not a subgraph of the whole map." << endl;

    cout << endl;


    // ------ Territory belonging to more than one continent ------
    Territory* t16 = new Territory("Territory16");

    vector<Continent*> invalidContinents3 = generateContinents();
    invalidContinents3.front()->addTerritory(t16);
    invalidContinents3.back()->addTerritory(t16);

    unordered_map<Territory*, vector<Territory*>> adjacencyList3{
        {t16, {}}
    };

    Map map5 = Map(invalidContinents3, adjacencyList3);
    cout << boolalpha << "Map 5: VALID = " << map5.validate() << endl;
    cout << "Multiple continents contain the same territory." << endl;

    cout << endl;

    // Set the first continent's territories to empty vector so that the end of this driver can properly clean up this last Map object.
    invalidContinents3.front()->setTerritories({});

    return 0;
}