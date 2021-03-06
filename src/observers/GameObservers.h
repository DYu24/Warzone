#pragma once

#include "../player/Player.h"
#include <iostream>
#include <vector>

class Subject;

enum Phase : short
{
    STARTUP,
    REINFORCEMENT,
    ISSUE_ORDERS,
    EXECUTE_ORDERS,
    NONE
};

class Observer
{
    public:
        virtual ~Observer();
        virtual void update() = 0;

    protected:
        Subject* subject_;
        Observer();
        Observer(Subject* subject);
        Observer(const Observer &observer);
        const Observer &operator=(const Observer &observer);
        virtual bool stateChanged_() const = 0;
        virtual void saveState_() = 0;
};

class PhaseObserver : public Observer
{
    public:
        PhaseObserver();
        PhaseObserver(Subject* subject);
        PhaseObserver(const PhaseObserver &observer);
        const PhaseObserver &operator=(const PhaseObserver &observer);
        void update();
        void display() const;

    protected:
        bool stateChanged_() const;
        void saveState_();

    private:
        Phase lastPhase_;
        Player* lastActivePlayer_;
};

class GameStatisticsObserver : public Observer
{
    public:
        GameStatisticsObserver();
        GameStatisticsObserver(Subject* subject);
        GameStatisticsObserver(const GameStatisticsObserver &observer);
        const GameStatisticsObserver &operator=(const GameStatisticsObserver &observer);
        void update();
        void display() const;

    protected:
        bool stateChanged_() const;
        void saveState_();

    private:
        std::vector<Player> lastSetOfPlayers_;
};

class Subject
{
    public:
        virtual ~Subject();
        virtual Phase getPhase() const = 0;
        virtual Player* getActivePlayer() const = 0;
        virtual std::vector<Player*> getCurrentPlayers() const = 0;
        void attach(Observer* observer);
        void detach(Observer* observer);
        void notify();
    
    private:
        std::vector<Observer*> observers_;
};
