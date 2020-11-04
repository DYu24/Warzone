#pragma once

#include "../map/Map.h"
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Player;
class Territory;

class Order
{
public:
    virtual ~Order(){};
    friend ostream &operator<<(ostream &output, const Order &order);
    virtual unique_ptr<Order> clone() const = 0;
    void execute(const shared_ptr<Player> owner);
    virtual bool validate(const shared_ptr<Player> owner) = 0;
    virtual int getPriority();

protected:
    virtual void execute_() = 0;
    virtual ostream &print_(ostream &output) const = 0;
};

class OrdersList
{
public:
    OrdersList();
    OrdersList(const OrdersList &orders);
    const OrdersList &operator=(const OrdersList &orders);
    friend ostream &operator<<(ostream &output, const OrdersList &orders);
    vector<unique_ptr<Order>> &getOrders();
    void setOrders(const vector<unique_ptr<Order>> &orders);
    void add(unique_ptr<Order> order);
    void move(int source, int destination);
    void remove(int target);
    unique_ptr<Order> popTopOrder();

private:
    vector<unique_ptr<Order>> orders_;
};

class DeployOrder : public Order
{
public:
    DeployOrder();
    DeployOrder(int numberOfArmies, shared_ptr<Territory> destination);
    DeployOrder(const DeployOrder &order);
    const DeployOrder &operator=(const DeployOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);
    int getPriority();

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    shared_ptr<Territory> destination_;
};

class AdvanceOrder : public Order
{
public:
    AdvanceOrder();
    AdvanceOrder(int numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination);
    AdvanceOrder(const AdvanceOrder &order);
    const AdvanceOrder &operator=(const AdvanceOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    shared_ptr<Territory> source_;
    shared_ptr<Territory> destination_;
    bool offensive_;
};

class BombOrder : public Order
{
public:
    BombOrder();
    BombOrder(shared_ptr<Territory> target);
    BombOrder(const BombOrder &order);
    const BombOrder &operator=(const BombOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    shared_ptr<Territory> target_;
};

class BlockadeOrder : public Order
{
public:
    BlockadeOrder();
    BlockadeOrder(shared_ptr<Territory> territory);
    BlockadeOrder(const BlockadeOrder &order);
    const BlockadeOrder &operator=(const BlockadeOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);
    int getPriority();

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    shared_ptr<Territory> territory_;
};

class AirliftOrder : public Order
{
public:
    AirliftOrder();
    AirliftOrder(int numberOfArmies, shared_ptr<Territory> source, shared_ptr<Territory> destination);
    AirliftOrder(const AirliftOrder &order);
    const AirliftOrder &operator=(const AirliftOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);
    int getPriority();

protected:
    void execute_();
    ostream &print_(ostream &output) const;

private:
    int numberOfArmies_;
    shared_ptr<Territory> source_;
    shared_ptr<Territory> destination_;
};

class NegotiateOrder : public Order
{
public:
    NegotiateOrder();
    NegotiateOrder(shared_ptr<Player> initiator, shared_ptr<Player> target);
    NegotiateOrder(const NegotiateOrder &order);
    const NegotiateOrder &operator=(const NegotiateOrder &order);
    unique_ptr<Order> clone() const;
    bool validate(const shared_ptr<Player> owner);

protected:
    void execute_();
    ostream &print_(ostream &output) const;
    
private:
    shared_ptr<Player> initiator_;
    shared_ptr<Player> target_;
};