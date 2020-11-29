#pragma once

#include "../orders/Orders.h"
#include "../player/Player.h"
#include <iostream>
#include <vector>

class Order;
class Player;

class Card
{
public:
    virtual ~Card();
    friend std::ostream &operator<<(std::ostream &output, const Card &card);
    virtual Card* clone() const = 0;
    virtual Order* play() const = 0;
    Player getOwner() const;
    void setOwner(Player* owner);

protected:
    Player* owner_;
    virtual std::ostream &print_(std::ostream &output) const = 0;
    virtual Order* buildOrder_() const = 0;
};


class Deck
{
public:
    Deck();
    Deck(const Deck &deck);
    ~Deck();
    const Deck &operator=(const Deck &deck);
    friend std::ostream &operator<<(std::ostream &output, const Deck &deck);
    std::vector<Card*> getCards() const;
    void setCards(std::vector<Card*> cards);
    int size() const;
    void addCard(Card* card);
    void generateCards(int numberOfCards);
    Card* draw();

private:
    std::vector<Card*> cards_;
};


class Hand
{
public:
    Hand();
    Hand(std::vector<Card*> cards);
    Hand(const Hand &hand);
    ~Hand();
    const Hand &operator=(const Hand &hand);
    friend std::ostream &operator<<(std::ostream &output, const Hand &hand);
    std::vector<Card*> getCards() const;
    void setCards(std::vector<Card*> cards);
    Card* at(int position);
    int size() const;
    void addCard(Card* card);
    Card* removeCard(int position);

private:
    std::vector<Card*> cards_;
};


class BombCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    std::ostream &print_(std::ostream &output) const;
    Order* buildOrder_() const;
};


class ReinforcementCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    std::ostream &print_(std::ostream &output) const;
    Order* buildOrder_() const;
};


class BlockadeCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    std::ostream &print_(std::ostream &output) const;
    Order* buildOrder_() const;
};


class AirliftCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    std::ostream &print_(std::ostream &output) const;
    Order* buildOrder_() const;
};


class DiplomacyCard : public Card
{
public:
    Order* play() const;
    Card* clone() const;

protected:
    std::ostream &print_(std::ostream &output) const;
    Order* buildOrder_() const;
};
