#pragma once

#include "../orders/Orders.h"
#include "../player/Player.h"
#include <iostream>
#include <list>
#include <vector>
using namespace std;

class Order;
class Player;


class Card
{
public:
    virtual ~Card();
    friend ostream &operator<<(ostream &output, const Card &card);
    virtual Card* clone() const = 0;
    virtual Order* play() const = 0;
    Player getOwner() const;
    void setOwner(Player* owner);

protected:
    Player* owner_;
    virtual ostream &print_(ostream &output) const = 0;
    virtual Order* buildOrder_() const = 0;
};


class Deck
{
public:
    Deck();
    Deck(const Deck &deck);
    ~Deck();
    const Deck &operator=(const Deck &deck);
    friend ostream &operator<<(ostream &output, const Deck &deck);
    vector<Card*> getCards() const;
    void setCards(vector<Card*> cards);
    int size() const;
    void addCard(Card* card);
    void generateCards(int numberOfCards);
    Card* draw();

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
    vector<Card*> getCards() const;
    void setCards(vector<Card*> cards);
    Card* at(int position);
    int size() const;
    void addCard(Card* card);
    Card* removeCard(int position);

private:
    vector<Card*> cards_;
};


class BombCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
    Order* buildOrder_() const;
};


class ReinforcementCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
    Order* buildOrder_() const;
};


class BlockadeCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
    Order* buildOrder_() const;
};


class AirliftCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
    Order* buildOrder_() const;
};


class DiplomacyCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    ostream &print_(ostream &output) const;
    Order* buildOrder_() const;
};
