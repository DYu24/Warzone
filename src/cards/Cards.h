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
    virtual Card* clone() const = 0;
    virtual Order* play() = 0;

protected:
    virtual ostream &print_(ostream &output) const = 0;
};

class Deck
{
public:
    Deck();
    Deck(const Deck &deck);
    ~Deck();
    const Deck &operator=(const Deck &deck);
    friend ostream &operator<<(ostream &output, const Deck &deck);
    vector<Card*> getCards();
    void addCard(Card* card);
    Card* draw();
    void generateCards(int numberOfCards);

private:
    vector<Card*> cards_;
};

class Hand
{
public:
    Hand();
    Hand(vector<Card*> cards);
    Hand(const Hand &hand);
    ~Hand();
    const Hand &operator=(const Hand &hand);
    friend ostream &operator<<(ostream &output, const Hand &hand);
    vector<Card*> getCards();
    void addCard(Card* card);
    Card* removeCard(int position);
    Order* playCardAt(int position);

private:
    vector<Card*> cards_;
};

class BombCard : public Card
{
public:
    Order* play();
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class ReinforcementCard : public Card
{
public:
    Order* play();
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class BlockadeCard : public Card
{
public:
    Order* play();
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class AirliftCard : public Card
{
public:
    Order* play();
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
};

class DiplomacyCard : public Card
{
public:
    Order* play();
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
};