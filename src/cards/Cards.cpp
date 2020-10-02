#include "Cards.h"
#include <algorithm>
#include <time.h>

/* 
===================================
 Implementation for Card class
===================================
 */

ostream &operator<<(ostream &output, const Card &card)
{
    return card.print_(output);
}

/* 
===================================
 Implementation for Deck class
===================================
 */

Deck::Deck() : cards_(make_unique<vector<unique_ptr<Card>>>()) {}

const Deck &Deck::operator=(const Deck &deck)
{
    setCards(*deck.cards_);
    return *this;
}

ostream &operator<<(ostream &output, const Deck &deck)
{
    output << "[Deck] Size=" << deck.cards_->size();
    return output;
}

vector<unique_ptr<Card>> &Deck::getCards()
{
    return *cards_;
}

void Deck::setCards(vector<unique_ptr<Card>> &cards)
{
    cards_ = make_unique<vector<unique_ptr<Card>>>();
    for (auto const &cardPointer : cards)
    {
        cards_->push_back(cardPointer->clone());
    }
}

unique_ptr<Card> Deck::draw()
{
    if (cards_->empty())
    {
        cout << "Deck is empty." << endl;
        return NULL;
    }

    srand(time(NULL));
    int randomIndex = rand() % cards_->size();
    auto randomCard = move(cards_->at(randomIndex));
    cards_->erase(cards_->begin() + randomIndex);

    return randomCard;
}

void Deck::addCard(unique_ptr<Card> card)
{
    cards_->push_back(move(card));
}

/* 
===================================
 Implementation for Hand class
===================================
 */

Hand::Hand() : cards_(make_unique<vector<unique_ptr<Card>>>()) {}

Hand::Hand(const Hand &hand)
{
    setCards(*hand.cards_);
}

const Hand &Hand::operator=(const Hand &hand)
{
    setCards(*hand.cards_);
    return *this;
}

ostream &operator<<(ostream &output, const Hand &hand)
{
    output << "[Hand] Size=" << hand.cards_->size();
    return output;
}

vector<unique_ptr<Card>> &Hand::getCards()
{
    return *cards_;
}

void Hand::setCards(vector<unique_ptr<Card>> &cards)
{
    cards_ = make_unique<vector<unique_ptr<Card>>>();
    for (auto const &cardPointer : cards)
    {
        cards_->push_back(cardPointer->clone());
    }
}

void Hand::addCard(unique_ptr<Card> card)
{
    cards_->push_back(move(card));
}

unique_ptr<Card> Hand::removeCard(int position)
{
    auto cardPosition = cards_->begin() + position;
    unique_ptr<Card> card = move(*cardPosition);
    cards_->erase(cardPosition);
    return card;
}

unique_ptr<Order> Hand::playCardAt(int position)
{
    return cards_->at(position)->play();
}

/* 
===================================
 Implementation for BombCard class
===================================
 */

unique_ptr<Card> BombCard::clone() const
{
    return make_unique<BombCard>(*this);
}

ostream &BombCard::print_(ostream &output) const
{
    output << "[BombCard]";
    return output;
}

unique_ptr<Order> BombCard::play()
{
    return make_unique<BombOrder>();
}

/* 
===================================
 Implementation for ReinforcementCard class
===================================
 */

unique_ptr<Card> ReinforcementCard::clone() const
{
    return make_unique<ReinforcementCard>(*this);
}

ostream &ReinforcementCard::print_(ostream &output) const
{
    output << "[ReinforcementCard]";
    return output;
}

unique_ptr<Order> ReinforcementCard::play()
{
    return make_unique<BombOrder>();
}


/* 
===================================
 Implementation for BlockadeCard class
===================================
 */

unique_ptr<Card> BlockadeCard::clone() const
{
    return make_unique<BlockadeCard>(*this);
}

ostream &BlockadeCard::print_(ostream &output) const
{
    output << "[BlockadeCard]";
    return output;
}

unique_ptr<Order> BlockadeCard::play()
{
    return make_unique<BlockadeOrder>();
}


/* 
===================================
 Implementation for AirliftCard class
===================================
 */

unique_ptr<Card> AirliftCard::clone() const
{
    return make_unique<AirliftCard>(*this);
}

ostream &AirliftCard::print_(ostream &output) const
{
    output << "[AirliftCard]";
    return output;
}

unique_ptr<Order> AirliftCard::play()
{
    return make_unique<AirliftOrder>();
}


/* 
===================================
 Implementation for DiplomacyCard class
===================================
 */

unique_ptr<Card> DiplomacyCard::clone() const
{
    return make_unique<DiplomacyCard>(*this);
}

ostream &DiplomacyCard::print_(ostream &output) const
{
    output << "[DiplomacyCard]";
    return output;
}

unique_ptr<Order> DiplomacyCard::play()
{
    return make_unique<NegotiateOrder>();
}
