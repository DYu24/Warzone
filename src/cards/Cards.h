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
    virtual ~Card(){};
    friend ostream &operator<<(ostream &output, const Card &card);
    virtual unique_ptr<Order> play() = 0;

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
    vector<shared_ptr<Card>> getCards();
    void addCard(shared_ptr<Card> card);
    shared_ptr<Card> draw();
    void generateCards(int numberOfCards);

private:
    vector<shared_ptr<Card>> cards_;
};

class Hand
{
public:
    Hand();
    Hand(vector<shared_ptr<Card>> cards);
    Hand(const Hand &hand);
    const Hand &operator=(const Hand &hand);
    friend ostream &operator<<(ostream &output, const Hand &hand);
    vector<shared_ptr<Card>> getCards();
    void addCard(shared_ptr<Card> card);
    shared_ptr<Card> removeCard(int position);
    unique_ptr<Order> playCardAt(int position);

private:
    vector<shared_ptr<Card>> cards_;
};

class BombCard : public Card
{
public:
    unique_ptr<Order> play();

protected:
    ostream &print_(ostream &output) const;
};

class ReinforcementCard : public Card
{
public:
    unique_ptr<Order> play();

protected:
    ostream &print_(ostream &output) const;
};

class BlockadeCard : public Card
{
public:
    unique_ptr<Order> play();

protected:
    ostream &print_(ostream &output) const;
};

class AirliftCard : public Card
{
public:
    unique_ptr<Order> play();

protected:
    ostream &print_(ostream &output) const;
};

class DiplomacyCard : public Card
{
public:
    unique_ptr<Order> play();

protected:
    ostream &print_(ostream &output) const;
};