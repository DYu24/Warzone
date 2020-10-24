#include "Cards.h"
#include <algorithm>
#include <time.h>

/* 
===================================
 Implementation for Card class
===================================
 */

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Card &card)
{
    return card.print_(output);
}


/* 
===================================
 Implementation for Deck class
===================================
 */

// Constructor
Deck::Deck() {}

// Copy constructor
Deck::Deck(const Deck &deck)
{
    setCards(deck.cards_);
}

// Assignment operator overloading
const Deck &Deck::operator=(const Deck &deck)
{
    setCards(deck.cards_);
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Deck &deck)
{
    output << "[Deck] Size=" << deck.cards_.size();
    return output;
}

// Getter and setter
vector<unique_ptr<Card>> &Deck::getCards()
{
    return cards_;
}

void Deck::setCards(const vector<unique_ptr<Card>> &cards)
{
    cards_.clear();
    for (auto const &cardPointer : cards)
    {
        cards_.push_back(cardPointer->clone());
    }
}

// Pick a random card from the deck and return a pointer to it.
// The selected card is removed from the deck.
unique_ptr<Card> Deck::draw()
{
    if (cards_.empty())
    {
        cout << "Deck is empty." << endl;
        return NULL;
    }

    srand(time(NULL));
    int randomIndex = rand() % cards_.size();
    auto randomCard = move(cards_.at(randomIndex));
    cards_.erase(cards_.begin() + randomIndex);

    return randomCard;
}

// Add a card to the deck.
void Deck::addCard(unique_ptr<Card> card)
{
    cards_.push_back(move(card));
}


/* 
===================================
 Implementation for Hand class
===================================
 */

// Default constructor
Hand::Hand() {}

// Copy constructor
Hand::Hand(const Hand &hand)
{
    setCards(hand.cards_);
}

// Assignment operator overloading
const Hand &Hand::operator=(const Hand &hand)
{
    setCards(hand.cards_);
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Hand &hand)
{
    output << "[Hand] Size=" << hand.cards_.size();
    return output;
}

// Getter and setter
vector<unique_ptr<Card>> &Hand::getCards()
{
    return cards_;
}

void Hand::setCards(const vector<unique_ptr<Card>> &cards)
{
    cards_.clear();
    for (auto const &cardPointer : cards)
    {
        cards_.push_back(cardPointer->clone());
    }
}

// Add a card to the player's hand.
void Hand::addCard(unique_ptr<Card> card)
{
    cards_.push_back(move(card));
}

// Remove and return a card from the player's hand indicated by its position.
unique_ptr<Card> Hand::removeCard(int position)
{
    auto cardPosition = cards_.begin() + position;
    unique_ptr<Card> card = move(*cardPosition);
    cards_.erase(cardPosition);
    return card;
}

// Play the card at the specified position.
unique_ptr<Order> Hand::playCardAt(int position)
{
    return cards_.at(position)->play();
}


/* 
===================================
 Implementation for BombCard class
===================================
 */

// Return a pointer to a new instance of BombCard.
unique_ptr<Card> BombCard::clone() const
{
    return make_unique<BombCard>(*this);
}

// Stream insertion operator overloading.
ostream &BombCard::print_(ostream &output) const
{
    output << "[BombCard]";
    return output;
}

// Generate a BombOrder when the card is played.
unique_ptr<Order> BombCard::play()
{
    return make_unique<BombOrder>();
}

/* 
===================================
 Implementation for ReinforcementCard class
===================================
 */

// Return a pointer to a new instance of ReinforcementCard.
unique_ptr<Card> ReinforcementCard::clone() const
{
    return make_unique<ReinforcementCard>(*this);
}

// Stream insertion operator overloading.
ostream &ReinforcementCard::print_(ostream &output) const
{
    output << "[ReinforcementCard]";
    return output;
}

// Generate an Order when the card is played.
unique_ptr<Order> ReinforcementCard::play()
{
    return make_unique<BombOrder>();
}


/* 
===================================
 Implementation for BlockadeCard class
===================================
 */

// Return a pointer to a new instance of BlockadeCard.
unique_ptr<Card> BlockadeCard::clone() const
{
    return make_unique<BlockadeCard>(*this);
}

// Stream insertion operator overloading.
ostream &BlockadeCard::print_(ostream &output) const
{
    output << "[BlockadeCard]";
    return output;
}

// Generate an BlockadeOrder when the card is played.
unique_ptr<Order> BlockadeCard::play()
{
    return make_unique<BlockadeOrder>();
}


/* 
===================================
 Implementation for AirliftCard class
===================================
 */

// Return a pointer to a new instance of AirliftCard.
unique_ptr<Card> AirliftCard::clone() const
{
    return make_unique<AirliftCard>(*this);
}

// Stream insertion operator overloading.
ostream &AirliftCard::print_(ostream &output) const
{
    output << "[AirliftCard]";
    return output;
}

// Generate an AirliftOrder when the card is played.
unique_ptr<Order> AirliftCard::play()
{
    return make_unique<AirliftOrder>();
}


/* 
===================================
 Implementation for DiplomacyCard class
===================================
 */

// Return a pointer to a new instance of DiplomacyCard.
unique_ptr<Card> DiplomacyCard::clone() const
{
    return make_unique<DiplomacyCard>(*this);
}

// Stream insertion operator overloading.
ostream &DiplomacyCard::print_(ostream &output) const
{
    output << "[DiplomacyCard]";
    return output;
}

// Generate an NegotiateOrder when the card is played.
unique_ptr<Order> DiplomacyCard::play()
{
    return make_unique<NegotiateOrder>();
}
