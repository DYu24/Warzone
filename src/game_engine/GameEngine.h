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
    GameEngine(const GameEngine &gameEngine);
    const GameEngine &operator=(const GameEngine &gameEngine);
    friend ostream &operator<<(ostream &output, const GameEngine &gameEngine);
    Map getMap();
    vector<Player*> getPlayers();
    void setPlayers(vector<Player*> players);
    void startGame();
    void startupPhase();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void mainGameLoop();

private:
    Deck* deck_;
    Map* map_;
    vector<Player*> players_;
};
