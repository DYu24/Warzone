#pragma once

#include "../map/Map.h"
#include <iostream>
#include <vector>
using namespace std;

class Player;
class Territory;

class Order
{
public:
    virtual ~Order(){};
    friend ostream &operator<<(ostream &output, const Order &order);
    virtual Order* clone() const = 0;
    void execute(Player* owner);
    virtual bool validate(Player* owner) = 0;
    int getPriority();

protected:
    Order();
    Order(int priority);
    Order(const Order &order);
    const Order &operator=(const Order &order);
    virtual void execute_() = 0;
    virtual ostream &print_(ostream &output) const = 0;

private:
    int priority_;
};

class OrdersList
{
public:
    OrdersList();
    OrdersList(const OrdersList &orders);
    ~OrdersList();
    const OrdersList &operator=(const OrdersList &orders);
    friend ostream &operator<<(ostream &output, const OrdersList &orders);
    vector<Order*> getOrders();
    void setOrders(vector<Order*> orders);
    void add(Order* order);
    void move(int source, int destination);
    void remove(int target);
    Order* popTopOrder();

private:
    vector<Order*> orders_;
};

class DeployOrder : public Order
{
public:
    DeployOrder();
    DeployOrder(int numberOfArmies, Territory* destination);
    DeployOrder(const DeployOrder &order);
    const DeployOrder &operator=(const DeployOrder &order);
    int getNumberOfArmies();
    Territory getDestination();
    void addArmies(int additional);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    Territory* destination_;
};

class AdvanceOrder : public Order
{
public:
    AdvanceOrder();
    AdvanceOrder(int numberOfArmies, Territory* source, Territory* destination);
    AdvanceOrder(const AdvanceOrder &order);
    const AdvanceOrder &operator=(const AdvanceOrder &order);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    Territory* source_;
    Territory* destination_;
    bool offensive_;
};

class BombOrder : public Order
{
public:
    BombOrder();
    BombOrder(Territory* target);
    BombOrder(const BombOrder &order);
    const BombOrder &operator=(const BombOrder &order);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    Territory* target_;
};

class BlockadeOrder : public Order
{
public:
    BlockadeOrder();
    BlockadeOrder(Territory* territory);
    BlockadeOrder(const BlockadeOrder &order);
    const BlockadeOrder &operator=(const BlockadeOrder &order);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    Territory* territory_;
};

class AirliftOrder : public Order
{
public:
    AirliftOrder();
    AirliftOrder(int numberOfArmies, Territory* source, Territory* destination);
    AirliftOrder(const AirliftOrder &order);
    const AirliftOrder &operator=(const AirliftOrder &order);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    Territory* source_;
    Territory* destination_;
};

class NegotiateOrder : public Order
{
public:
    NegotiateOrder();
    NegotiateOrder(Player* initiator, Player* target);
    NegotiateOrder(const NegotiateOrder &order);
    const NegotiateOrder &operator=(const NegotiateOrder &order);
    Order* clone() const;
    bool validate(Player* owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;
    
private:
    Player* initiator_;
    Player* target_;
};