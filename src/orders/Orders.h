#pragma once

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

enum OrderType { DEPLOY, ADVANCE, BOMB, BLOCKADE, AIRLIFT, NEGOTIATE };

class Order
{
public:
    void execute();
    friend ostream &operator<<(ostream &output, const Order &order);
    virtual bool validate() = 0;

protected:
    virtual void execute_() = 0;
    virtual ostream& print_(ostream &output) const = 0;
};

class OrdersList
{
public:
    OrdersList();
    OrdersList(const OrdersList &orders);
    const OrdersList &operator=(const OrdersList &orders);
    friend ostream &operator<<(ostream &output, const OrdersList &orders);
    vector<shared_ptr<Order>> getOrders();
    void setOrders(vector<shared_ptr<Order>> &orders);
    void addOrder(shared_ptr<Order> order);
    void moveOrder(shared_ptr<Order> order, int target);
    void deleteOrder(shared_ptr<Order> order);

private:
    shared_ptr<vector<shared_ptr<Order>>> orders_;
};

class DeployOrder : public Order
{
public:
    DeployOrder();
    DeployOrder(const DeployOrder &order);
    const DeployOrder &operator=(const DeployOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};

class AdvanceOrder : public Order
{
public:
    AdvanceOrder();
    AdvanceOrder(const AdvanceOrder &order);
    const AdvanceOrder &operator=(const AdvanceOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};

class BombOrder : public Order
{
public:
    BombOrder();
    BombOrder(const BombOrder &order);
    const BombOrder &operator=(const BombOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};

class BlockadeOrder : public Order
{
public:
    BlockadeOrder();
    BlockadeOrder(const BlockadeOrder &order);
    const BlockadeOrder &operator=(const BlockadeOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};

class AirliftOrder : public Order
{
public:
    AirliftOrder();
    AirliftOrder(const AirliftOrder &order);
    const AirliftOrder &operator=(const AirliftOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};

class NegotiateOrder : public Order
{
public:
    NegotiateOrder();
    NegotiateOrder(const NegotiateOrder &order);
    const NegotiateOrder &operator=(const NegotiateOrder &order);
    bool validate();

protected:
    void execute_();
    ostream& print_(ostream &output) const;
};