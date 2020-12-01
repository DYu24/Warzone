#include "GameObservers.h"
#include <algorithm>
#include <iomanip>

#ifdef WIN32
#include <windows.h>
#endif

namespace
{
    // Color codes to make console output more distinct
    #ifndef WIN32
        const std::unordered_map<int, string> PLAYER_COLOR_CODES{
            {0, "\e[0;31m"},
            {1, "\e[0;32m"},
            {2, "\e[0;33m"},
            {3, "\e[0;34m"},
            {4, "\e[0;36m"}
        };
        const string WHITE_COLOR_CODE = "\e[0;37m";
        const string WHITE_BOLD_COLOR_CODE = "\e[1;37m";
        const string RESET_COLOR_CODE = "\e[0m";
    #else
        const std::unordered_map<int, int> WINDOWS_PLAYER_COLOR_CODES{
            {0, 1},
            {1, 2},
            {2, 3},
            {3, 4},
            {4, 5}
        };
        const int WINDOWS_WHITE_COLOR_CODE = 7;
        const int WINDOWS_WHITE_BOLD_COLOR_CODE = 15;
        const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    // Helper function to change the output text color based on the current active player
    void setPlayerColorCode(Player* player, std::vector<Player*> allPlayers)
    {
        auto iterator = find(allPlayers.begin(), allPlayers.end(), player);
        int playerIndex = iterator - allPlayers.begin();
        #ifdef WIN32
            int colorCode = 0;
            if (player->isNeutral())
            {
                colorCode = WINDOWS_WHITE_COLOR_CODE;
            }
            else
            {
                colorCode = WINDOWS_PLAYER_COLOR_CODES.at(playerIndex);
            }
            SetConsoleTextAttribute(hConsole, colorCode);
        #else
            if (player->isNeutral())
            {
                std::cout << WHITE_COLOR_CODE;
            }
            std::cout << PLAYER_COLOR_CODES.at(playerIndex);
        #endif
    }

    // Helper function to set white output text to bold
    void setBold()
    {
        #ifdef WIN32
            SetConsoleTextAttribute(hConsole, WINDOWS_WHITE_BOLD_COLOR_CODE);
        #else
            std::cout << WHITE_BOLD_COLOR_CODE;
        #endif
    }

    // Reset any output text attributes
    void resetColorCode()
    {
        #ifdef WIN32
            SetConsoleTextAttribute(hConsole, WINDOWS_WHITE_COLOR_CODE);
        #else
            std::cout << RESET_COLOR_CODE;
        #endif
    }
}


/* 
=========================================
 Implementation for Observer class
=========================================
 */

// Constructors
Observer::Observer() : subject_(nullptr) {}

Observer::Observer(Subject* subject) : subject_(subject) {}

Observer::Observer(const Observer &observer) : subject_(observer.subject_) {}

// Destructor
Observer::~Observer() {}

// Assignment operator overloading
const Observer &Observer::operator=(const Observer &observer)
{
    if (this != &observer)
    {
        subject_ = observer.subject_;
    }
    return *this;
}

/* 
=========================================
 Implementation for PhaseObserver class
=========================================
 */

// Constructors
PhaseObserver::PhaseObserver() : Observer(), lastPhase_(NONE), lastActivePlayer_(nullptr) {}

PhaseObserver::PhaseObserver(Subject* subject) : Observer(subject), lastPhase_(NONE), lastActivePlayer_(nullptr) {}

PhaseObserver::PhaseObserver(const PhaseObserver &observer)
    : Observer(observer), lastPhase_(observer.lastPhase_), lastActivePlayer_(observer.lastActivePlayer_) {}

// Assignment operator overloading
const PhaseObserver &PhaseObserver::operator=(const PhaseObserver &observer)
{
    if (this != &observer)
    {
        Observer::operator=(observer);
        lastPhase_ = observer.lastPhase_;
        lastActivePlayer_ = observer.lastActivePlayer_;
    }
    return *this;
}

// Update observer
void PhaseObserver::update()
{
    if (stateChanged_())
    {
        display();
        saveState_();
    }
}

// Output the subject state to console
void PhaseObserver::display() const
{
    Phase currentPhase = subject_->getPhase();

    if (currentPhase == STARTUP)
    {
        setBold();
        std::cout << "\n===========================================" << std::endl;
        std::cout << "              STARTUP PHASE" << std::endl;
        std::cout << "===========================================" << std::endl;
        std::cout << "Setting up turns and assigning initial territories to each player..." << std::endl;
    }
    else
    {
        std::string phaseTitle;
        std::string phaseBody;

        Player* currentActivePlayer = subject_->getActivePlayer();
        if (currentActivePlayer != nullptr && !currentActivePlayer->isNeutral())
        {
            setPlayerColorCode(currentActivePlayer, subject_->getCurrentPlayers());
            switch (subject_->getPhase())
            {
                case REINFORCEMENT:
                    std::cout << "\n===========================================" << std::endl;
                    std::cout << "       " << currentActivePlayer->getName() << " : REINFORCEMENT PHASE" << std::endl;
                    std::cout << "===========================================" << std::endl;
                    std::cout << "Number of territories controlled: " << currentActivePlayer->getOwnedTerritories().size() << std::endl;
                    std::cout << "Reinforcements: " << currentActivePlayer->getReinforcements() << std::endl;
                    break;
                    
                case ISSUE_ORDERS:
                    std::cout << "\n===========================================" << std::endl;
                    std::cout << "       " << currentActivePlayer->getName() << " : ISSUE ORDERS PHASE" << std::endl;
                    std::cout << "===========================================" << std::endl;
                    std::cout << "Orders placed: " << currentActivePlayer->getOrdersList().size() << std::endl;
                    std::cout << "Cards in hand: " << currentActivePlayer->getHand().size() << std::endl;
                    std::cout << "Reinforcements left: " << currentActivePlayer->getReinforcements() << std::endl;
                    break;

                case EXECUTE_ORDERS:
                    std::cout << "\n===========================================" << std::endl;
                    std::cout << "       " << currentActivePlayer->getName() << " : EXECUTE ORDERS PHASE" << std::endl;
                    std::cout << "===========================================" << std::endl;
                    std::cout << "Orders left: " << currentActivePlayer->getOrdersList().size() << std::endl;
                    std::cout << "Number of territories controlled: " << currentActivePlayer->getOwnedTerritories().size() << std::endl;
                    break;

                default:
                    break;
            }
        }
    }
    resetColorCode();
}

// Check if the state of the subject has changed
bool PhaseObserver::stateChanged_() const
{
    return lastPhase_ != subject_->getPhase() || lastActivePlayer_ != subject_->getActivePlayer();
}

// Take a snapshot of the subject's state
void PhaseObserver::saveState_()
{
    lastPhase_ = subject_->getPhase();
    lastActivePlayer_ = subject_->getActivePlayer();
}


/* 
==================================================
 Implementation for GameStatisticsObserver class
==================================================
 */

// Constructors
GameStatisticsObserver::GameStatisticsObserver() : Observer() {}

GameStatisticsObserver::GameStatisticsObserver(Subject* subject) : Observer(subject)
{
    for (const auto &player : subject_->getCurrentPlayers())
    {
        lastSetOfPlayers_.push_back(*player);
    }
}

GameStatisticsObserver::GameStatisticsObserver(const GameStatisticsObserver &observer)
    : Observer(observer), lastSetOfPlayers_(observer.lastSetOfPlayers_) {}

// Assignment operator overloading
const GameStatisticsObserver &GameStatisticsObserver::operator=(const GameStatisticsObserver &observer)
{
    if (this != &observer)
    {
        Observer::operator=(observer);
        lastSetOfPlayers_ = observer.lastSetOfPlayers_;
    }
    return *this;
}

// Update observer
void GameStatisticsObserver::update()
{
    if (stateChanged_())
    {
        display();
        saveState_();
    }
}

// Output the subject state to console
void GameStatisticsObserver::display() const
{
    std::vector<Player*> allPlayers = subject_->getCurrentPlayers();

    int totalNumberOfTerritories = 0;
    for (const auto &player : allPlayers)
    {
        totalNumberOfTerritories += player->getOwnedTerritories().size();
    }

    setBold();

    std::cout << "\n===========================================" << std::endl;
    std::cout << "              GAME STATISTICS" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Player";
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Territories";
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "% Controlled" << std::endl;

    for (const auto &player : allPlayers)
    {
        int territoriesOwned = player->getOwnedTerritories().size();
        double percentControlled = (double)territoriesOwned / totalNumberOfTerritories * 100;
        
        std::cout << std::left << std::setw(20) << std::setfill(' ') << player->getName();
        std::cout << std::left << std::setw(20) << std::setfill(' ') << territoriesOwned;
        std::cout << std::left << std::setw(20) << std::setfill(' ') << std::fixed << std::setprecision(2) << percentControlled << std::endl;
    }
    std::cout << std::endl;

    if (allPlayers.size() == 1)
    {
        Player* winner = allPlayers.front();
        std::cout << winner->getName() << " wins. Congratulations!" << std::endl;
    }

    resetColorCode();
}

// Check if the state of the subject has changed
bool GameStatisticsObserver::stateChanged_() const
{
    std::vector<Player*> allPlayers = subject_->getCurrentPlayers();
    if (lastSetOfPlayers_.size() == allPlayers.size())
    {
        bool changed = false;
        for (int i = 0; i < lastSetOfPlayers_.size(); i++)
        {
            Player oldPlayer = lastSetOfPlayers_.at(i);
            Player newPlayer = *allPlayers.at(i);
            changed |= oldPlayer.getOwnedTerritories() != newPlayer.getOwnedTerritories();
        }

        return changed;
    }

    return true;
}

// Take a snapshot of the subject's state
void GameStatisticsObserver::saveState_()
{
    lastSetOfPlayers_.clear();
    for (auto const &player : subject_->getCurrentPlayers())
    {
        lastSetOfPlayers_.push_back(*player);
    }
}

/* 
=========================================
 Implementation for Subject class
=========================================
 */

Subject::~Subject()
{
    for (const auto &observer : observers_)
    {
        delete observer;
    }
    observers_.clear();
}

// Add an observer to the list of observers
void Subject::attach(Observer* observer)
{
    observers_.push_back(observer);
}

// Remove an observer from the list of observers
void Subject::detach(Observer* observer)
{
    auto removeIterator = remove(observers_.begin(), observers_.end(), observer);
    observers_.erase(removeIterator, observers_.end());
    delete observer;
    observer = nullptr;
}

// Notify all observers to update
void Subject::notify()
{
    for (const auto &observer : observers_)
    {
        observer->update();
    }
}
