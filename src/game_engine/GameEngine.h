#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../player/Player.h"
#include <vector>
using namespace std;

class GameEngine
{
public:
    GameEngine();
    friend ostream &operator<<(ostream &output, const GameEngine &gameEngine);
    static Deck* getDeck();
    static Map* getMap();
    static vector<Player*> getPlayers();
    static Player* getOwnerOf(Territory* territory);
    static void assignToNeutralPlayer(Territory* territory);
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
};
