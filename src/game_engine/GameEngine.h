#pragma once

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../player/Player.h"
#include <memory>
#include <vector>
using namespace std;

class GameEngine
{
public:
    GameEngine();
    GameEngine(const GameEngine &gameEngine);
    const GameEngine &operator=(const GameEngine &gameEngine);
    friend ostream &operator<<(ostream &output, const GameEngine &gameEngine);
    Map &getMap();
    vector<shared_ptr<Player>> &getPlayers();
    void setPlayers(const vector<shared_ptr<Player>> &players);
    void startGame();
    void startupPhase();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void mainGameLoop();

private:
    unique_ptr<Deck> deck_;
    vector<shared_ptr<Player>> players_;
    unique_ptr<Map> map_;
};
