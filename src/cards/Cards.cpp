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
    cards_ = deck.cards_;
}

// Assignment operator overloading
const Deck &Deck::operator=(const Deck &deck)
{
    cards_ = deck.cards_;
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Deck &deck)
{
    output << "[Deck] Size=" << deck.cards_.size();
    return output;
}

// Getter and setter
vector<shared_ptr<Card>> Deck::getCards()
{
    return cards_;
}

// Pick a random card from the deck and return a pointer to it.
// The selected card is removed from the deck.
shared_ptr<Card> Deck::draw()
{
    if (cards_.empty())
    {
        cout << "Deck is empty." << endl;
        return NULL;
    }

    srand(time(NULL));
    int randomIndex = rand() % cards_.size();
    auto randomCard = cards_.at(randomIndex);
    cards_.erase(cards_.begin() + randomIndex);

    return randomCard;
}

// Add a card to the deck.
void Deck::addCard(shared_ptr<Card> card)
{
    cards_.push_back(card);
}

/**
 * Generate a number of cards and insert into the deck.
 */
void Deck::generateCards(int numberOfCards)
{
    const int NUMBER_OF_CARD_TYPES = 5;
    for (int i = 0; i < numberOfCards; i++)
    {
        switch (i % NUMBER_OF_CARD_TYPES)
        {
            case 0: 
                cards_.push_back(make_shared<BombCard>());
                break;
            case 1:
                cards_.push_back(make_shared<ReinforcementCard>());
                break;
            case 2:
                cards_.push_back(make_shared<BlockadeCard>());
                break;
            case 3:
                cards_.push_back(make_shared<AirliftCard>());
                break;
            default:
                cards_.push_back(make_shared<DiplomacyCard>());
        }
    }
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
    cards_ = hand.cards_;
}

// Assignment operator overloading
const Hand &Hand::operator=(const Hand &hand)
{
    cards_ = hand.cards_;
    return *this;
}

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Hand &hand)
{
    output << "[Hand] Size=" << hand.cards_.size();
    return output;
}

// Getter and setter
vector<shared_ptr<Card>> Hand::getCards()
{
    return cards_;
}

// Add a card to the player's hand.
void Hand::addCard(shared_ptr<Card> card)
{
    cards_.push_back(card);
}

// Remove and return a card from the player's hand indicated by its position.
shared_ptr<Card> Hand::removeCard(int position)
{
    auto cardPosition = cards_.begin() + position;
    shared_ptr<Card> card = *cardPosition;
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
