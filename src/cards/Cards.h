#pragma once

#include "../orders/Orders.h"
#include "../player/Player.h"
#include <iostream>
#include <list>
#include <memory>
#include <vector>
using namespace std;

class Order;

class Card
{
public:
    virtual unique_ptr<Order> play() = 0;
    virtual unique_ptr<Card> clone() const = 0;
    virtual ~Card(){};
    friend ostream &operator<<(ostream &output, const Card &card);

protected:
    virtual ostream &print_(ostream &output) const = 0;
};

class Deck
{
public:
    Deck();
    Deck(const Deck &deck);
    const Deck &operator=(const Deck &deck);
    friend ostream &operator<<(ostream &output, const Deck &deck);
    vector<unique_ptr<Card>> &getCards();
    void setCards(vector<unique_ptr<Card>> &cards);
    unique_ptr<Card> draw();
    void addCard(unique_ptr<Card> card);

private:
    unique_ptr<vector<unique_ptr<Card>>> cards_;
};

class Hand
{
public:
    Hand();
    Hand(vector<unique_ptr<Card>> cards);
    Hand(const Hand &hand);
    const Hand &operator=(const Hand &hand);
    friend ostream &operator<<(ostream &output, const Hand &hand);
    vector<unique_ptr<Card>> &getCards();
    void setCards(vector<unique_ptr<Card>> &cards);
    void addCard(unique_ptr<Card> card);
    unique_ptr<Card> removeCard(int position);
    unique_ptr<Order> playCardAt(int position);

private:
    unique_ptr<vector<unique_ptr<Card>>> cards_;
};

class BombCard : public Card
{
public:
    unique_ptr<Order> play();
    unique_ptr<Card> clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class ReinforcementCard : public Card
{
public:
    unique_ptr<Order> play();
    unique_ptr<Card> clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class BlockadeCard : public Card
{
public:
    unique_ptr<Order> play();
    unique_ptr<Card> clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class AirliftCard : public Card
{
public:
    unique_ptr<Order> play();
    unique_ptr<Card> clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class DiplomacyCard : public Card
{
public:
    unique_ptr<Order> play();
    unique_ptr<Card> clone() const;

protected:
    ostream &print_(ostream &output) const;
};