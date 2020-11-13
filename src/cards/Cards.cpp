#include "Cards.h"
#include "../game_engine/GameEngine.h"
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

// Setter
void Card::setOwner(Player* owner)
{
    owner_ = owner;
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
    for (auto card : deck.cards_)
    {
        cards_.push_back(card->clone());
    }
}

// Destructor
Deck::~Deck()
{
    for (auto card : cards_)
    {
        delete card;
    }
    cards_.clear();
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
vector<Card*> Deck::getCards()
{
    return cards_;
}

void Deck::setCards(vector<Card*> cards)
{
    for (auto card : cards_)
    {
        delete card;
    }
    cards_.clear();

    for (auto card : cards)
    {
        cards_.push_back(card->clone());
    }
}

// Pick a random card from the deck and return a pointer to it.
// The selected card is removed from the deck.
Card* Deck::draw()
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
void Deck::addCard(Card* card)
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
                cards_.push_back(new BombCard());
                break;
            case 1:
                cards_.push_back(new ReinforcementCard());
                break;
            case 2:
                cards_.push_back(new BlockadeCard());
                break;
            case 3:
                cards_.push_back(new AirliftCard());
                break;
            default:
                cards_.push_back(new DiplomacyCard());
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
    for (auto card : hand.cards_)
    {
        cards_.push_back(card->clone());
    }
}

// Destructor
Hand::~Hand()
{
    for (auto card : cards_)
    {
        delete card;
    }
    cards_.clear();
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
vector<Card*> Hand::getCards()
{
    return cards_;
}

void Hand::setCards(vector<Card*> cards)
{
    for (auto card : cards_)
    {
        delete card;
    }
    cards_.clear();

    for (auto card : cards)
    {
        cards_.push_back(card->clone());
    }
}

// Add a card to the player's hand.
void Hand::addCard(Card* card)
{
    cards_.push_back(card);
}

// Remove and return a card from the player's hand indicated by its position.
Card* Hand::removeCard(int position)
{
    auto cardPosition = cards_.begin() + position;
    Card* card = *cardPosition;
    cards_.erase(cardPosition);
    return card;
}

// Play the card at the specified position.
Order* Hand::playCardAt(int position)
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

// Return a pointer to a new instance of BombCard.
Card* BombCard::clone() const
{
    return new BombCard();
}

// Generate a BombOrder when the card is played.
Order* BombCard::play()
{
    if (owner_ == NULL)
    {
        return new BombOrder();
    }

    // Bomb the highest priority enemy territory
    Territory* target = owner_->toAttack().front();
    return new BombOrder(target);
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

// Return a pointer to a new instance of ReinforcementCard.
Card* ReinforcementCard::clone() const
{
    return new ReinforcementCard();
}

// Generate an Order when the card is played.
Order* ReinforcementCard::play()
{
    if (owner_ != NULL)
    {
        owner_->addReinforcements(5);
    }

    return NULL;
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

// Return a pointer to a new instance of BlockadeCard.
Card* BlockadeCard::clone() const
{
    return new BlockadeCard();
}

// Generate an BlockadeOrder when the card is played.
Order* BlockadeCard::play()
{
    if (owner_ == NULL)
    {
        return new BlockadeOrder();
    }

    // Setup a blockade on the highest priority territory to defend
    Territory* territory = owner_->toDefend().front();
    return new BlockadeOrder(territory);
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

// Return a pointer to a new instance of AirliftCard.
Card* AirliftCard::clone() const
{
    return new AirliftCard();
}

// Generate an AirliftOrder when the card is played.
Order* AirliftCard::play()
{
    if (owner_ == NULL)
    {
        return new AirliftOrder();
    }

    // Pick the highest priority territory to defend to airlift to
    Territory* destination = owner_->toDefend().front();

    // Choose the player's territory with the most movable armies as the source
    int movableArmies = 0;
    int maxArmies = 0;
    Territory* source;
    for (auto territory : owner_->getOwnedTerritories())
    {
        movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        if (movableArmies > maxArmies && territory != destination)
        {
            source = territory;
            maxArmies = movableArmies;
        }
    }

    // Move half (arbitrary) the number of armies from the source territory to the destination
    int armiesToMove = max(movableArmies, movableArmies / 2);
    source->addPendingOutgoingArmies(armiesToMove);

    return new AirliftOrder(armiesToMove, source, destination);
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

// Return a pointer to a new instance of DiplomacyCard.
Card* DiplomacyCard::clone() const
{
    return new DiplomacyCard();
}

// Generate an NegotiateOrder when the card is played.
Order* DiplomacyCard::play()
{
    if (owner_ == NULL)
    {
        return new NegotiateOrder();
    }

    Player* targetPlayer = NULL;
    vector<Territory*> ownerTerritories = owner_->getOwnedTerritories();
    vector<Territory*> territoriesToDefend = owner_->toDefend();

    // Try to pick an enemy player who has the most armies on an adjacent territory to the highest priority territory in
    // the player's `toDefend()` list
    auto iterator = territoriesToDefend.begin();
    while (targetPlayer == NULL && iterator != territoriesToDefend.end())
    {
        Territory* highestPriorityTerritory = *iterator;

        int maxArmies = 0;
        Territory* mostReinforcedEnemyTerritory = NULL;
        for (auto territory : GameEngine::getMap()->getAdjacentTerritories(highestPriorityTerritory))
        {
            bool isEnemyTerritory = find(ownerTerritories.begin(), ownerTerritories.end(), territory) == ownerTerritories.end();
            if (isEnemyTerritory && territory->getNumberOfArmies() > maxArmies)
            {
                mostReinforcedEnemyTerritory = territory;
                maxArmies = territory->getNumberOfArmies();
            }
        }

        if (mostReinforcedEnemyTerritory != NULL)
        {
            targetPlayer = GameEngine::getOwnerOf(mostReinforcedEnemyTerritory);
            break;
        }

        // If the current territory to defend has no enemy adjacent territories, move to the next territory in the `toDefend()` list
        iterator++;
    }

    // If no suitable target player is found, pick a random enemy
    if (targetPlayer == NULL)
    {
        vector<Player*> enemyPlayers;
        for (auto player : GameEngine::getPlayers())
        {
            if (player != owner_)
            {
                enemyPlayers.push_back(player);
            }
        }

        targetPlayer = enemyPlayers.at(rand() % enemyPlayers.size());
    }

    return new NegotiateOrder(owner_, targetPlayer);
}
