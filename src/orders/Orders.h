#pragma once

#include "../map/Map.h"
#include "../player/Player.h"
#include <iostream>
#include <vector>
using namespace std;

class Player;
class Territory;

enum OrderType : short
{
    DEPLOY,
    ADVANCE,
    BOMB,
    BLOCKADE,
    AIRLIFT,
    NEGOTIATE,
};

class Order
{
public:
    virtual ~Order(){};
    friend ostream &operator<<(ostream &output, const Order &order);
    virtual Order* clone() const = 0;
    void execute();
    virtual bool validate() const = 0;
    int getPriority() const;
    virtual OrderType getType() const = 0;

protected:
    Order();
    Order(Player* issuer, int priority);
    Order(const Order &order);
    const Order &operator=(const Order &order);
    virtual void execute_() = 0;
    virtual ostream &print_(ostream &output) const = 0;
    Player* issuer_;

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
    vector<Order*> getOrders() const;
    void setOrders(vector<Order*> orders);
    Order* popTopOrder();
    Order* peek();
    int size() const;
    void add(Order* order);
    void move(int source, int destination);
    void remove(int target);

private:
    vector<Order*> orders_;
};

class DeployOrder : public Order
{
public:
    DeployOrder();
    DeployOrder(Player* issuer, int numberOfArmies, Territory* destination);
    DeployOrder(const DeployOrder &order);
    const DeployOrder &operator=(const DeployOrder &order);
    Order* clone() const;
    void addArmies(int additional);
    bool validate() const;
    OrderType getType() const;

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
    AdvanceOrder(Player* issuer, int numberOfArmies, Territory* source, Territory* destination);
    AdvanceOrder(const AdvanceOrder &order);
    const AdvanceOrder &operator=(const AdvanceOrder &order);
    Order* clone() const;
    bool validate() const;
    OrderType getType() const;

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    Territory* source_;
    Territory* destination_;
};

class BombOrder : public Order
{
public:
    BombOrder();
    BombOrder(Player* issuer, Territory* target);
    BombOrder(const BombOrder &order);
    const BombOrder &operator=(const BombOrder &order);
    Order* clone() const;
    bool validate() const;
    OrderType getType() const;

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
    BlockadeOrder(Player* issuer, Territory* territory);
    BlockadeOrder(const BlockadeOrder &order);
    const BlockadeOrder &operator=(const BlockadeOrder &order);
    Order* clone() const;
    bool validate() const;
    OrderType getType() const;

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
    AirliftOrder(Player* issuer, int numberOfArmies, Territory* source, Territory* destination);
    AirliftOrder(const AirliftOrder &order);
    const AirliftOrder &operator=(const AirliftOrder &order);
    Order* clone() const;
    bool validate() const;
    OrderType getType() const;

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
    NegotiateOrder(Player* issuer, Player* target);
    NegotiateOrder(const NegotiateOrder &order);
    const NegotiateOrder &operator=(const NegotiateOrder &order);
    Order* clone() const;
    bool validate() const;
    OrderType getType() const;

protected:
    void execute_();
    ostream &print_(ostream &output) const;
    
private:
    Player* target_;
};