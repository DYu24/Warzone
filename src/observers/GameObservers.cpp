#include "GameObservers.h"
#include <algorithm>
#include <iomanip>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;


namespace
{
    // Color codes to make console output more distinct
    #ifndef WIN32
        const unordered_map<int, string> PLAYER_COLOR_CODES{
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
        const unordered_map<int, int> WINDOWS_PLAYER_COLOR_CODES{
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
    void setPlayerColorCode(Player* player, vector<Player*> allPlayers)
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
                cout << WHITE_COLOR_CODE;
            }
            cout << PLAYER_COLOR_CODES.at(playerIndex);
        #endif
    }

    // Helper function to set white output text to bold
    void setBold()
    {
        #ifdef WIN32
            SetConsoleTextAttribute(hConsole, WINDOWS_WHITE_BOLD_COLOR_CODE);
        #else
            cout << WHITE_BOLD_COLOR_CODE;
        #endif
    }

    // Reset any output text attributes
    void resetColorCode()
    {
        #ifdef WIN32
            SetConsoleTextAttribute(hConsole, WINDOWS_WHITE_COLOR_CODE);
        #else
            cout << RESET_COLOR_CODE;
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
    subject_ = observer.subject_;
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
    Observer::operator=(observer);
    lastPhase_ = observer.lastPhase_;
    lastActivePlayer_ = observer.lastActivePlayer_;
    return *this;
}

// Update observer
void PhaseObserver::update()
{
    if (stateChanged())
    {
        display();
        saveState();
    }
}

// Output the subject state to console
void PhaseObserver::display() const
{
    Phase currentPhase = subject_->getPhase();

    if (currentPhase == STARTUP)
    {
        setBold();
        cout << "\n===========================================" << endl;
        cout << "              STARTUP PHASE" << endl;
        cout << "===========================================" << endl;
        cout << "Setting up turns and assigning initial territories to each player..." << endl;
    }
    else
    {
        string phaseTitle;
        string phaseBody;

        Player* currentActivePlayer = subject_->getActivePlayer();
        if (currentActivePlayer != nullptr && !currentActivePlayer->isNeutral())
        {
            setPlayerColorCode(currentActivePlayer, subject_->getCurrentPlayers());
            switch (subject_->getPhase())
            {
                case REINFORCEMENT:
                    cout << "\n===========================================" << endl;
                    cout << "       " << currentActivePlayer->getName() << " : REINFORCEMENT PHASE" << endl;
                    cout << "===========================================" << endl;
                    cout << "Number of territories controlled: " << currentActivePlayer->getOwnedTerritories().size() << endl;
                    cout << "Reinforcements: " << currentActivePlayer->getReinforcements() << endl;
                    break;
                    
                case ISSUE_ORDERS:
                    cout << "\n===========================================" << endl;
                    cout << "       " << currentActivePlayer->getName() << " : ISSUE ORDERS PHASE" << endl;
                    cout << "===========================================" << endl;
                    cout << "Orders placed: " << currentActivePlayer->getOrdersList().size() << endl;
                    cout << "Cards in hand: " << currentActivePlayer->getHand().size() << endl;
                    cout << "Reinforcements left: " << currentActivePlayer->getReinforcements() << endl;
                    break;

                case EXECUTE_ORDERS:
                    cout << "\n===========================================" << endl;
                    cout << "       " << currentActivePlayer->getName() << " : EXECUTE ORDERS PHASE" << endl;
                    cout << "===========================================" << endl;
                    cout << "Orders left: " << currentActivePlayer->getOrdersList().size() << endl;
                    cout << "Number of territories controlled: " << currentActivePlayer->getOwnedTerritories().size() << endl;
                    break;

                default:
                    break;
            }
        }
    }
    resetColorCode();
}

// Check if the state of the subject has changed
bool PhaseObserver::stateChanged() const
{
    return lastPhase_ != subject_->getPhase() || lastActivePlayer_ != subject_->getActivePlayer();
}

// Take a snapshot of the subject's state
void PhaseObserver::saveState()
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
    Observer::operator=(observer);
    lastSetOfPlayers_ = observer.lastSetOfPlayers_;
    return *this;
}

// Update observer
void GameStatisticsObserver::update()
{
    if (stateChanged())
    {
        display();
        saveState();
    }
}

// Output the subject state to console
void GameStatisticsObserver::display() const
{
    vector<Player*> allPlayers = subject_->getCurrentPlayers();

    int totalNumberOfTerritories = 0;
    for (const auto &player : allPlayers)
    {
        totalNumberOfTerritories += player->getOwnedTerritories().size();
    }

    setBold();

    cout << "\n===========================================" << endl;
    cout << "              GAME STATISTICS" << endl;
    cout << "===========================================" << endl;
    cout << left << setw(20) << setfill(' ') << "Player";
    cout << left << setw(20) << setfill(' ') << "Territories";
    cout << left << setw(20) << setfill(' ') << "% Controlled" << endl;

    for (const auto &player : allPlayers)
    {
        int territoriesOwned = player->getOwnedTerritories().size();
        double percentControlled = (double)territoriesOwned / totalNumberOfTerritories * 100;
        
        cout << left << setw(20) << setfill(' ') << player->getName();
        cout << left << setw(20) << setfill(' ') << territoriesOwned;
        cout << left << setw(20) << setfill(' ') << fixed << setprecision(2) << percentControlled << endl;
    }
    cout << endl;

    if (allPlayers.size() == 1)
    {
        Player* winner = allPlayers.front();
        cout << winner->getName() << " wins. Congratulations!" << endl;
    }

    resetColorCode();
}

// Check if the state of the subject has changed
bool GameStatisticsObserver::stateChanged() const
{
    vector<Player*> allPlayers = subject_->getCurrentPlayers();
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
void GameStatisticsObserver::saveState()
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
