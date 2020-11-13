#include "GameEngine.h"
#include "../map/Map.h"
#include "../map_loader/MapLoader.h"
#include "../orders/Orders.h"
#include <algorithm>
#include <limits>
#include <math.h>
#include <random>
#include <string>
#include <time.h>
#include <unordered_set>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

namespace
{
    /**
     * Reads the specified directory and returns a vector of filenames for all the `.map` files
     */
    vector<string> getMapFileNames(const string &directory)
    {
        vector<string> fileNames;

        #ifdef WINDOWS
            string pattern(directory);
            pattern.append("\\*");
            WIN32_FIND_DATA data;
            HANDLE hFind;
            if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
            {
                do
                {
                    string filename = data.cFileName;
                    if (filename.find(".map") != string::npos)
                    {
                        fileNames.push_back(data.cFileName);
                    }
                } while (FindNextFile(hFind, &data) != 0);
                FindClose(hFind);
            }
        #else
            DIR *dirp = opendir(directory.c_str());
            struct dirent *dp;
            while ((dp = readdir(dirp)) != NULL)
            {
                string filename = dp->d_name;
                if (filename.find(".map") != string::npos)
                {
                    fileNames.push_back(filename);
                }
            }
            closedir(dirp);
        #endif

        return fileNames;
    }

    /*
    * Select and create a game map based on the user's selection
    */
    Map* selectMap()
    {
        cout << "Select a map to play on: " << endl;
        vector<string> maps = getMapFileNames("resources");
        int i = 1;
        for (auto const &m : maps)
        {
            cout << "[" << i++ << "] " << m << endl;
        }

        while (true)
        {
            int selection;
            cin >> selection;

            if (cin.fail() || selection - 1 < 0 || selection - 1 >= maps.size())
            {
                cout << "That was not a valid option. Please try again:" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            try
            {
                return MapLoader::loadMap("resources/" + maps.at(selection - 1));
            }
            catch (char const *errorMessage)
            {
                cout << "The selected map was invalid. Please try another option:" << endl;
            }
            catch (string const errorMessage)
            {
                cout << "The selected map was invalid. Please try another option:" << endl;
            }
        }
    }
    /*
    * Create players based on user's input
    */
    vector<Player*> setupPlayers()
    {
        vector<Player*> players;
        cout << "Enter the number of players for this game: ";
        
        int numberOfPlayers;

        while (true)
        {
            cin >> numberOfPlayers;

            if (cin.fail() || numberOfPlayers < 2 || numberOfPlayers > 5)
            {
                cout << "Please enter a valid number of players (2-5): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            break;
        }

        for (int i = 1; i <= numberOfPlayers; i++)
        {
            cout << "Enter a name for Player " << i << ": ";
            string name;
            cin >> name;

            players.push_back(new Player(name));
        }

        return players;
    }
}

Deck* GameEngine::deck_ = new Deck();
Map* GameEngine::map_ = new Map();
vector<Player*> GameEngine::players_;

// Constructor
GameEngine::GameEngine() {}

// Operator overloading
ostream &operator<<(ostream &output, const GameEngine &gameEngine)
{
    return output;
}

// Getters
Deck* GameEngine::getDeck()
{
    return deck_;
}

Map* GameEngine::getMap()
{
    return map_;
}

vector<Player*> GameEngine::getPlayers()
{
    return players_;
}

// Find the player who owns the specified territory. Return NULL if the territory is unowned.
Player* GameEngine::getOwnerOf(Territory* territory)
{
    for (auto player : players_)
    {
        vector<Territory*> territories = player->getOwnedTerritories();
        if (find(territories.begin(), territories.end(), territory) != territories.end())
        {
            return player;
        }
    }

    return NULL;
}

/*
 * Setup the map and players to be included in the game based on the user's input
 */
void GameEngine::startGame()
{
    string title = R"(
    ====================================================
                          WARZONE
    ====================================================
    )";
    cout << title << endl;

    map_ = selectMap();
    cout << endl;

    players_ = setupPlayers();
    cout << endl;

    deck_->generateCards(20);
}

/*
 * Goes through the startup phase of the game where:
 * - Order of the players is determined at random
 * - Territories are assigned to each player at random
 * - A base number of armies are assigned to each player
 */
void GameEngine::startupPhase()
{
    // Shuffle the order of players in the game
    shuffle(players_.begin(), players_.end(), default_random_engine(time(NULL)));

    // Assign territories
    int playerIndex = 0;

    vector<Territory*> assignableTerritories = map_->getTerritories();
    while (!assignableTerritories.empty())
    {
        // Pop out a random territory
        int randomIndex = rand() % assignableTerritories.size();
        Territory* randomTerritory = assignableTerritories.at(randomIndex);
        assignableTerritories.erase(assignableTerritories.begin() + randomIndex);

        players_.at(playerIndex)->addOwnedTerritory(randomTerritory);
        playerIndex = (playerIndex + 1) % players_.size();
    }

    const int NUMBER_OF_INITIAL_ARMIES = (-5 * players_.size()) + 50;
    for (auto const &player : players_)
    {
        player->addReinforcements(NUMBER_OF_INITIAL_ARMIES);
    }
}

/*
 * Assigns the appropriate amount of reinforcements for each player based on the territories they control.
 */
void GameEngine::reinforcementPhase()
{
    for (auto const &player : players_)
    {
        vector<Territory*> playerTerritories = player->getOwnedTerritories();
        int reinforcements = floor(playerTerritories.size() / 3);
    
        // List of territories need to be sorted so that it can be used to check if the player owns all members of a continent
        sort(playerTerritories.begin(), playerTerritories.end());
        // Check if the player owns all members of any territories
        for (auto const &continent : map_->getContinents())
        {
            vector<Territory*> continentMembers;
            for (auto const & member : continent->getTerritories())
            {
                continentMembers.push_back(member);
            }
            sort(continentMembers.begin(), continentMembers.end());

            if (includes(playerTerritories.begin(), playerTerritories.end(), continentMembers.begin(), continentMembers.end()))
            {
                reinforcements += continent->getControlValue();
            }
        }

        if (reinforcements < 3)
        {
            reinforcements = 3;
        }

        player->addReinforcements(reinforcements);
    }
}

void GameEngine::issueOrdersPhase()
{
    for (auto const &player : players_)
    {
        player->issueOrder();
    }
}

/*
 * Executes players' orders in a round-robin fashion until all players have no orders left to execute.
 */
void GameEngine::executeOrdersPhase()
{
    unordered_set<Player*> playersFinishedExecutingOrders;
    unordered_set<Player*> playersFinishedDeploying;

    // ====== PRE-EXECUTION ======
    // Take a snapshot of the players' owned territories before proceeding with the order executions
    int preExecuteSnapshot[players_.size()] {};
    for (int i = 0; i < players_.size(); i++)
    {
        Player* player = players_.at(i);
        preExecuteSnapshot[i] = player->getOwnedTerritories().size();
    }

    // ====== EXECUTION ======
    while (playersFinishedExecutingOrders.size() != players_.size())
    {
        for (auto player : players_)
        {
            Order* order = player->peekNextOrder();
            // Current player still has orders to execute
            if (order != NULL)
            {
                // Ignore non-deploy orders until everyone has finished executing their deployments
                if (order->getType() != DEPLOY && playersFinishedDeploying.size() != players_.size())
                {
                    playersFinishedDeploying.insert(player);
                    continue;
                }

                order = player->getNextOrder();
                cout << "[" << player->getName() << "] ";
                order->execute(player);
                delete order;
                order = NULL;
            }
            // Current player has no orders left to execute this turn
            else
            {
                player->clearDiplomaticRelations();
                playersFinishedExecutingOrders.insert(player);
            }
        }
    }

    // ====== POST-EXECUTION ======
    // If a player has conquered at least one territory, draw a card
    for (int i = 0; i < players_.size(); i++)
    {
        Player* player = players_.at(i);
        if (preExecuteSnapshot[i] < player->getOwnedTerritories().size())
        {
            player->drawCardFromDeck();
        }
    }
}

/*
 * Core game
 */
void GameEngine::mainGameLoop()
{
    bool shouldContinueGame = true;
    // while (shouldContinueGame)
    for (int j = 0; j < 2; j++)
    {
        // Check for any winner and remove players who do not own any territories
        for (int i = 0; i < players_.size(); i++)
        {
            if (players_.at(i)->getOwnedTerritories().size() == map_->getAdjacencyList().size())
            {
                shouldContinueGame = false;
                cout << players_.at(i)->getName() << " has won the game!" << endl;
                break;
            }

            if (players_.at(i)->getOwnedTerritories().size() == 0)
            {
                cout << players_.at(i)->getName() << " does not control any territories. Removing from game." << endl;
            }
        }

        auto removeIterator = remove_if(players_.begin(), players_.end(), [](Player* p) { return p->getOwnedTerritories().size() == 0; });
        players_.erase(removeIterator, players_.end());

        reinforcementPhase();
        issueOrdersPhase();
        executeOrdersPhase();
    }
}