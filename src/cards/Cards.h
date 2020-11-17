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
    virtual ~Card(){};
    friend ostream &operator<<(ostream &output, const Card &card);
    virtual Card* clone() const = 0;
    virtual Order* play() = 0;
    Player getOwner() const;
    void setOwner(Player* owner);

protected:
    virtual ostream &print_(ostream &output) const = 0;
    Player* owner_;
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
    int size() const;
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