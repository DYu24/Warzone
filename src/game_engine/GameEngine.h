#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../observers/GameObservers.h"
#include "../player/Player.h"
#include <vector>
using namespace std;

class GameEngine : public Subject
{
public:
    GameEngine();
    GameEngine(const GameEngine &gameEngine);
    const GameEngine &operator=(const GameEngine &gameEngine);
    friend ostream &operator<<(ostream &output, const GameEngine &gameEngine);
    static Deck* getDeck();
    static Map* getMap();
    static vector<Player*> getPlayers();
    static Player* getOwnerOf(Territory* territory);
    static void setMap(Map* map);
    static void setPlayers(vector<Player*> players);
    static void assignToNeutralPlayer(Territory* territory);
    static void resetGameEngine();
    Phase getPhase() const;
    Player* getActivePlayer() const;
    vector<Player*> getCurrentPlayers() const;
    void startGame();
    void startupPhase();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void mainGameLoop();

private:
    static Deck* deck_;
    static Map* map_;
    static vector<Player*> players_;
    Phase currentPhase_;
    Player* activePlayer_;
};
