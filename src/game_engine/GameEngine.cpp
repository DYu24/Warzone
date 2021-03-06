#include "GameEngine.h"
#include "../map/Map.h"
#include "../map_loader/MapLoader.h"
#include "../orders/Orders.h"
#include <algorithm>
#include <filesystem>
#include <limits>
#include <math.h>
#include <random>
#include <string>
#include <time.h>
#include <unordered_set>

namespace fs = std::filesystem;

namespace
{
    // Helper method for debugging/demo. Pauses game execution until the user presses ENTER on the console.
    void pause()
    {
        std::cout << "Press [Enter] to Continue..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }

    // Reads the specified directory and returns a vector of filenames for all the `.map` files.
    std::vector<std::string> getMapFileNames(std::string directory)
    {
        std::vector<std::string> fileNames;
        for (const auto &entry : fs::directory_iterator(directory))
        {
            fileNames.push_back(entry.path().filename().string());
        }

        return fileNames;
    }

    // Select and create a game map based on the user's selection.
    Map* selectMap()
    {
        MapLoader* loader = nullptr;
        Map* map = nullptr;
        const std::string RESOURCES_DIRECTORY = "resources";

        std::cout << "Select a map to play on: " << std::endl;
        std::vector<std::string> maps = getMapFileNames(RESOURCES_DIRECTORY);
        int i = 1;
        for (const auto &map : maps)
        {
            std::cout << "[" << i++ << "] " << map << std::endl;
        }

        while (map == nullptr)
        {
            int selection;
            std::cin >> selection;

            if (std::cin.fail() || selection - 1 < 0 || selection - 1 >= maps.size())
            {
                std::cout << "That was not a valid option. Please try again:" << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            try
            {
                std::cout << "Loading map..." << std::endl;
                loader = new MapLoader();
                map = loader->loadMap(RESOURCES_DIRECTORY + "/" + maps.at(selection - 1));
            }
            catch (char const *errorMessage)
            {
                std::cout << "The selected map was invalid. Please try another option:" << std::endl;
            }
            catch (std::string const errorMessage)
            {
                try
                {
                    delete loader;
                    loader = new ConquestFileReaderAdapter();
                    map = loader->loadMap(RESOURCES_DIRECTORY + "/" + maps.at(selection - 1));
                }
                catch(char const *errorMessage)
                {
                    std::cout << "The selected map was invalid. Please try another option:" << std::endl;
                }
                catch (std::string const errorMessage)
                {
                    std::cout << "The selected map was invalid. Please try another option:" << std::endl;
                }
                
            }

            delete loader;
        }

        return map;
    }

    // Create players based on user's input.
    std::vector<Player*> setupPlayers()
    {
        std::cout << "Enter the number of players for this game: ";
        
        int numberOfPlayers = 0;

        while (numberOfPlayers == 0)
        {
            std::cin >> numberOfPlayers;

            if (std::cin.fail() || numberOfPlayers < 2 || numberOfPlayers > 5)
            {
                std::cout << "Please enter a valid number of players (2-5): ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                numberOfPlayers = 0;
                continue;
            }

            break;
        }

        std::vector<Player*> players;
        for (int i = 1; i <= numberOfPlayers; i++)
        {
            std::cout << "Enter a name for Player " << i << ": ";
            std::string name;
            std::cin >> name;

            players.push_back(new Player(name, new HumanPlayerStrategy()));
        }

        return players;
    }

    // Turn on/off specified observers
    void setupObservers(GameEngine* gameEngine)
    {
        bool gameStatsObserverOn = true;
        bool phaseObserverOn = true;
        int selection = 0;

        while (selection != 3)
        {
            std::cout << "Configure game observers (enter 1 or 2 to toggle):" << std::endl;
            std::cout << "[1] Game Statistics Observer: " << (gameStatsObserverOn ? "ON" : "OFF") << std::endl;
            std::cout << "[2] Phase Observer: " << (phaseObserverOn ? "ON" : "OFF") << std::endl;
            std::cout << "[3] Confirm" << std::endl;

            while (true)
            {
                std::cin >> selection;
                if (std::cin.fail() || selection < 1 || selection > 3)
                {
                    std::cout << "Please select a valid option: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                break;
            }

            gameStatsObserverOn = selection == 1 ? !gameStatsObserverOn : gameStatsObserverOn;
            phaseObserverOn = selection == 2 ? !phaseObserverOn : phaseObserverOn;
        }

        if (gameStatsObserverOn)
        {
            gameEngine->attach(new GameStatisticsObserver(gameEngine));
        }

        if (phaseObserverOn)
        {
            gameEngine->attach(new PhaseObserver(gameEngine));
        }
    }
}


// Initialize static members
Deck* GameEngine::deck_ = new Deck();
Map* GameEngine::map_ = new Map();
std::vector<Player*> GameEngine::players_;


/* 
===================================
 Implementation for GameEngine class
===================================
 */

// Constructors
GameEngine::GameEngine() : currentPhase_(NONE), activePlayer_(nullptr) {}

GameEngine::GameEngine(const GameEngine &gameEngine) : currentPhase_(gameEngine.currentPhase_), activePlayer_(gameEngine.activePlayer_) {}

// Operator overloading
const GameEngine &GameEngine::operator=(const GameEngine &gameEngine)
{
    if (this != &gameEngine)
    {
        currentPhase_ = gameEngine.currentPhase_;
        activePlayer_ = gameEngine.activePlayer_;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &output, const GameEngine &gameEngine)
{
    return output;
}

// Static getters
Deck* GameEngine::getDeck()
{
    return deck_;
}

Map* GameEngine::getMap()
{
    return map_;
}

std::vector<Player*> GameEngine::getPlayers()
{
    std::vector<Player*> allPlayers;
    for (const auto &player : players_)
    {
        if (!player->isNeutral())
        {
            allPlayers.push_back(player);
        }
    }

    return allPlayers;
}

// Static setters
void GameEngine::setMap(Map* map)
{
    delete map_;
    map_ = map;
}

void GameEngine::setPlayers(std::vector<Player*> players)
{
    for (const auto &player : players_)
    {
        delete player;
    }
    players_.clear();
    players_ = players;
}

// Getters (for subject state)
Phase GameEngine::getPhase() const
{
    return currentPhase_;
}

Player* GameEngine::getActivePlayer() const
{
    return activePlayer_;
}

std::vector<Player*> GameEngine::getCurrentPlayers() const
{
    return players_;
}

// Find the player who owns the specified territory. Return nullptr if the territory is unowned.
Player* GameEngine::getOwnerOf(Territory* territory)
{
    for (const auto &player : players_)
    {
        std::vector<Territory*> territories = player->getOwnedTerritories();
        if (find(territories.begin(), territories.end(), territory) != territories.end())
        {
            return player;
        }
    }

    return nullptr;
}

// Assign a territory to the Neutral Player. If no such player exists, create one.
void GameEngine::assignToNeutralPlayer(Territory* territory)
{
    Player* owner = getOwnerOf(territory);
    owner->removeOwnedTerritory(territory);

    auto isNeutralPlayer = [](const auto &player) { return player->isNeutral(); };
    auto iterator = find_if(players_.begin(), players_.end(), isNeutralPlayer);
    if (iterator == players_.end())
    {
        Player* neutralPlayer = new Player();
        neutralPlayer->addOwnedTerritory(territory);
        players_.push_back(neutralPlayer);
    }
    else
    {
        Player* neutralPlayer = *iterator;
        neutralPlayer->addOwnedTerritory(territory);
    }
}

// Deallocate static members of GameEngine class
void GameEngine::resetGameEngine()
{
    delete deck_;
    delete map_;
    deck_ = nullptr;
    map_ = nullptr;

    for (const auto &player : players_)
    {
        delete player;
    }
    players_.clear();
}

// Setup the map and players to be included in the game based on the user's input
void GameEngine::startGame()
{
    std::cout << "====================================================" << std::endl;
    std::cout << "                      WARZONE" << std::endl;
    std::cout << "====================================================" << std::endl;

    setMap(selectMap());
    std::cout << std::endl;

    players_ = setupPlayers();
    std::cout << std::endl;

    setupObservers(this);

    deck_->generateCards(50);
}

// Goes through the startup phase of the game where:
// - Order of the players is determined at random
// - Territories are assigned to each player at random
// - A base number of armies are assigned to each player
void GameEngine::startupPhase()
{
    currentPhase_ = STARTUP;
    notify();

    // Shuffle the order of players in the game
    shuffle(players_.begin(), players_.end(), std::default_random_engine(time(nullptr)));

    // Assign territories
    int playerIndex = 0;
    std::vector<Territory*> assignableTerritories = map_->getTerritories();
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
    for (auto &player : players_)
    {
        player->addReinforcements(NUMBER_OF_INITIAL_ARMIES);
    }
}

// Assigns the appropriate amount of reinforcements for each player based on the territories they control.
void GameEngine::reinforcementPhase()
{
    for (auto &player : players_)
    {
        if (player->isNeutral())
        {
            continue;
        }

        activePlayer_ = player;

        std::vector<Territory*> playerTerritories = player->getOwnedTerritories();
        int reinforcements = floor(playerTerritories.size() / 3);
    
        // Check if the player owns all members of any continents
        for (const auto &continent : map_->getContinents())
        {
            int numberOfContinentMembersOwned = 0;
            std::vector<Territory*> continentMembers = continent->getTerritories();
            for (const auto &member : continentMembers)
            {
                if (find(playerTerritories.begin(), playerTerritories.end(), member) != playerTerritories.end())
                {
                    numberOfContinentMembersOwned++;
                }
            }

            if (numberOfContinentMembersOwned == continentMembers.size())
            {
                reinforcements += continent->getControlValue();
            }
        }

        if (reinforcements < 3)
        {
            reinforcements = 3;
        }

        player->addReinforcements(reinforcements);
        notify();
        std::cout << "[" << player->getName() << "] received " << reinforcements << " reinforcements and now has " << player->getReinforcements() << " in total." << std::endl;
    }
}

// Issue orders one-by-one in a round-robin fashion over all the players
void GameEngine::issueOrdersPhase()
{
    std::unordered_set<Player*> playersFinishedIssuingOrders;
    while (playersFinishedIssuingOrders.size() != players_.size())
    {
        for (auto &player : players_)
        {
            activePlayer_ = player;

            if (player->isDoneIssuingOrders())
            {
                playersFinishedIssuingOrders.insert(player);
                continue;                
            }

            notify();
            std::cout << "[" << player->getName() << "] ";
            player->issueOrder();
        }
    }
}

// Executes players' orders in a round-robin fashion until all players have no orders left to execute.
void GameEngine::executeOrdersPhase()
{
    std::vector<Player*> playersInTurn = players_;
    std::unordered_set<Player*> playersFinishedDeploying;
    std::unordered_set<Player*> playersFinishedExecutingOrders;

    // ====== PRE-EXECUTION ======
    // Take a snapshot of the players' owned territories before proceeding with the order executions
    std::unordered_map<Player*, std::vector<Territory*>> preExecuteSnapshot;
    for (const auto &player : playersInTurn)
    {
        preExecuteSnapshot[player] = player->getOwnedTerritories();
    }

    // ====== EXECUTION ======
    while (playersFinishedExecutingOrders.size() != playersInTurn.size())
    {
        for (auto &player : playersInTurn)
        {
            activePlayer_ = player;
            Order* order = player->peekNextOrder();

            // Current player still has orders to execute
            if (order != nullptr)
            {
                // Ignore non-deploy orders until everyone has finished executing their deployments
                if (order->getType() != DEPLOY && playersFinishedDeploying.size() != playersInTurn.size())
                {
                    playersFinishedDeploying.insert(player);
                    continue;
                }

                // Notify for phase observer
                notify();

                order = player->getNextOrder();
                std::cout << "[" << player->getName() << "] ";
                order->execute();
                delete order;
                order = nullptr;

                // Notify for game statistics observer
                notify();
            }
            // Current player has no orders left to execute this turn
            else
            {
                player->endTurn();
                playersFinishedExecutingOrders.insert(player);
                playersFinishedDeploying.insert(player);
            }
        }
    }

    // ====== POST-EXECUTION ======
    // If a player has conquered at least one territory, draw a card
    for (auto &player : playersInTurn)
    {
        std::vector<Territory*> preExecuteTerritories = preExecuteSnapshot.at(player);
        std::vector<Territory*> postExecuteTerritories = player->getOwnedTerritories();
        if (preExecuteTerritories.size() <= postExecuteTerritories.size() && preExecuteTerritories != postExecuteTerritories)
        {
            player->drawCardFromDeck();
        }
    }
}

// Core game loop
void GameEngine::mainGameLoop()
{
    int round = 0;
    bool shouldContinueGame = true;
    while (shouldContinueGame)
    {
        pause();
        std::cout << "\n================================================= ROUND " << ++round << " =====================================================" << std::endl;
        currentPhase_ = REINFORCEMENT;
        reinforcementPhase();

        pause();
        std::cout << "\n=====================================================================================================================" << std::endl;
        currentPhase_ = ISSUE_ORDERS;
        issueOrdersPhase();

        pause();
        std::cout << "\n=====================================================================================================================" << std::endl;
        currentPhase_ = EXECUTE_ORDERS;
        executeOrdersPhase();

        currentPhase_ = NONE;

        // Check for any winner and remove players who do not own any territories
        for (auto &player : players_)
        {
            if (player->getOwnedTerritories().size() == map_->getAdjacencyList().size())
            {
                shouldContinueGame = false;
            }
                        
            if (player->getOwnedTerritories().size() == 0)
            {
                if (player == activePlayer_)
                {
                    activePlayer_ = nullptr;
                }

                delete player;
                player = nullptr;
            }
        }

        auto removeIterator = remove_if(players_.begin(), players_.end(), [](const auto &p) { return p == nullptr; });
        players_.erase(removeIterator, players_.end());

        notify();
    }

    std::cout << "Total rounds played: " << round << std::endl;
}
