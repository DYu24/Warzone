#include "GameEngine.h"
#include "../map/Map.h"
#include "../map_loader/MapLoader.h"
#include <string>

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
    Map selectMap()
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

            if (selection - 1 < 0 || selection - 1 >= maps.size())
            {
                cout << "That was not a valid option. Please try again:" << endl;
                continue;
            }

            try
            {
                Map selectedMap = MapLoader::loadMap("resources/" + maps.at(selection - 1));
                return selectedMap;
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
    vector<unique_ptr<Player>> setupPlayers()
    {
        vector<unique_ptr<Player>> players;
        cout << "Enter the number of players for this game: ";
        
        int numberOfPlayers;

        while (true)
        {
            cin >> numberOfPlayers;

            if (numberOfPlayers < 2 || numberOfPlayers > 5)
            {
                cout << "Please enter a valid number of players (2-5): ";
                continue;
            }

            break;
        }

        for (int i = 1; i <= numberOfPlayers; i++)
        {
            cout << "Enter a name for Player " << i << ": ";
            string name;
            cin >> name;

            unique_ptr<Player> player = make_unique<Player>(name);
            players.push_back(move(player));
        }

        return players;
    }
}

// Constructors
GameEngine::GameEngine() : deck_(make_unique<Deck>()), map_(make_unique<Map>()) {}

GameEngine::GameEngine(const GameEngine &gameEngine) : deck_(make_unique<Deck>(*gameEngine.deck_)), map_(make_unique<Map>(*gameEngine.map_))
{
    setPlayers(gameEngine.players_);
}

// Operator overloading
const GameEngine &GameEngine::operator=(const GameEngine &gameEngine)
{
    deck_ = make_unique<Deck>(*gameEngine.deck_);
    map_ = make_unique<Map>(*gameEngine.map_);
    setPlayers(gameEngine.players_);
    return *this;
}

ostream &operator<<(ostream &output, const GameEngine &gameEngine)
{
    return output;
}

// Getter and setter
vector<unique_ptr<Player>> &GameEngine::getPlayers()
{
    return players_;
}

void GameEngine::setPlayers(const vector<unique_ptr<Player>> &players)
{
    players_.clear();
    for (auto const &player : players)
    {
        players_.push_back(make_unique<Player>(*player));
    }
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

    Map map = selectMap();
    map_ = make_unique<Map>(map);
    cout << endl;

    vector<unique_ptr<Player>> players = setupPlayers();
    setPlayers(players);
    cout << endl;
}

/*
 * Goes through the startup phase of the game where:
 * - Order of the players is determined at random
 * - Territories are assigned to each player at random
 * - A base number of armies are assigned to each player
 */
void GameEngine::startupPhase()
{

}
