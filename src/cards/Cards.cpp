#include "Cards.h"
#include "../game_engine/GameEngine.h"
#include <algorithm>
#include <time.h>


/* 
===================================
 Implementation for Card class
===================================
 */

// Destructor
Card::~Card() {};

// Stream insertion operator overloading
ostream &operator<<(ostream &output, const Card &card)
{
    return card.print_(output);
}

// Getter and Setter
Player Card::getOwner() const
{
    return *owner_;
}

void Card::setOwner(Player* owner)
{
    owner_ = owner;
}


/* 
===================================
 Implementation for Deck class
===================================
 */

// Constructors
Deck::Deck() {}

Deck::Deck(const Deck &deck)
{
    for (const auto &card : deck.cards_)
    {
        cards_.push_back(card->clone());
    }
}

// Destructor
Deck::~Deck()
{
    for (const auto &card : cards_)
    {
        delete card;
    }
    cards_.clear();
}

// Operator overloading
const Deck &Deck::operator=(const Deck &deck)
{
    setCards(deck.cards_);
    return *this;
}

ostream &operator<<(ostream &output, const Deck &deck)
{
    output << "[Deck] Size=" << deck.size();
    return output;
}

// Getter and setter
vector<Card*> Deck::getCards() const
{
    return cards_;
}

void Deck::setCards(vector<Card*> cards)
{
    for (const auto &card : cards_)
    {
        delete card;
    }
    cards_.clear();

    for (const auto &card : cards)
    {
        cards_.push_back(card->clone());
    }
}

// Pick a random card from the deck and return a pointer to it. The selected card is removed from the deck.
Card* Deck::draw()
{
    if (cards_.empty())
    {
        cout << "Deck is empty." << endl;
        return nullptr;
    }

    srand(time(nullptr));
    int randomIndex = rand() % cards_.size();
    auto randomCard = cards_.at(randomIndex);
    cards_.erase(cards_.begin() + randomIndex);

    return randomCard;
}

// Get the number of cards in the deck.
int Deck::size() const
{
    return cards_.size();
}

// Add a card to the deck.
void Deck::addCard(Card* card)
{
    cards_.push_back(card);
}

// Generate a number of cards and insert into the deck.
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

// Constructors
Hand::Hand() {}

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
    for (const auto &card : cards_)
    {
        delete card;
    }
    cards_.clear();
}

// Operator overloading
const Hand &Hand::operator=(const Hand &hand)
{
    setCards(hand.cards_);
    return *this;
}

ostream &operator<<(ostream &output, const Hand &hand)
{
    output << "[Hand] Size=" << hand.size();
    return output;
}

// Getter and setter
vector<Card*> Hand::getCards() const
{
    return cards_;
}

void Hand::setCards(vector<Card*> cards)
{
    for (const auto &card : cards_)
    {
        delete card;
    }
    cards_.clear();

    for (const auto &card : cards)
    {
        cards_.push_back(card->clone());
    }
}

// Get the number of cards in the hand.
int Hand::size() const
{
    return cards_.size();
}

// Add a card to the player's hand.
void Hand::addCard(Card* card)
{
    cards_.push_back(card);
}

// Remove and return a card from the player's hand indicated by `position`.
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

// Operator overloading.
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
    if (owner_ == nullptr)
    {
        return new BombOrder();
    }

    // Bomb enemy territory with the most armies (last one in the `toAttack()` list)
    Territory* target = owner_->toAttack().back();
    return new BombOrder(owner_, target);
}


/* 
===================================
 Implementation for ReinforcementCard class
===================================
 */

// Operator overloading.
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

// Add 5 reinforcements to the player's pool when the card is played.
Order* ReinforcementCard::play()
{
    if (owner_ != nullptr)
    {
        owner_->addReinforcements(5);
    }

    return nullptr;
}


/* 
===================================
 Implementation for BlockadeCard class
===================================
 */

// Operator overloading.
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

// Generate a BlockadeOrder when the card is played.
Order* BlockadeCard::play()
{
    if (owner_ == nullptr)
    {
        return new BlockadeOrder();
    }

    // Setup a blockade on the highest priority territory to defend that has any armies
    vector<Territory*> territoriesToDefend = owner_->toDefend();
    for (const auto &potentialTerritory : territoriesToDefend)
    {
        if (potentialTerritory->getNumberOfArmies() > 0)
        {
            return new BlockadeOrder(owner_, potentialTerritory);
        }
    }

    // If unable to find suitable territory, then just pick the first one from the `toDefend()` list
    return new BlockadeOrder(owner_, territoriesToDefend.front());
}


/* 
===================================
 Implementation for AirliftCard class
===================================
 */

// Operator overloading.
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
    if (owner_ == nullptr)
    {
        return new AirliftOrder();
    }

    // Pick the highest priority territory to defend
    Territory* destination = owner_->toDefend().front();

    // Choose the player's territory with the most movable armies as the source
    int movableArmies = 0;
    int maxArmies = 0;
    Territory* source = owner_->getOwnedTerritories().front();
    for (const auto &territory : owner_->getOwnedTerritories())
    {
        movableArmies = territory->getNumberOfArmies() + territory->getPendingIncomingArmies() - territory->getPendingOutgoingArmies();
        if (movableArmies > maxArmies)
        {
            source = territory;
            maxArmies = movableArmies;
        }
    }

    // Move half the number of armies from the source territory to the destination
    int armiesToMove = max(movableArmies / 2, 1);
    source->addPendingOutgoingArmies(armiesToMove);

    return new AirliftOrder(owner_, armiesToMove, source, destination);
}


/* 
===================================
 Implementation for DiplomacyCard class
===================================
 */

// Operator overloading.
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

// Generate a NegotiateOrder when the card is played.
Order* DiplomacyCard::play()
{
    if (owner_ == nullptr || GameEngine::getPlayers().size() < 2)
    {
        return new NegotiateOrder();
    }

    Map* map = GameEngine::getMap();
    vector<Territory*> ownerTerritories = owner_->getOwnedTerritories();
    vector<Territory*> territoriesToDefend = owner_->toDefend();

    // Try to pick an enemy player who has the most armies on an adjacent territory to the highest priority territory in
    // the player's `toDefend()` list
    auto iterator = territoriesToDefend.begin();
    for (; iterator != territoriesToDefend.end(); iterator++)
    {
        Territory* highestPriorityTerritory = *iterator;
        Territory* mostReinforcedEnemyTerritory = nullptr;
        int maxArmies = 0;

        for (const auto &territory : map->getAdjacentTerritories(highestPriorityTerritory))
        {
            bool isEnemyTerritory = find(ownerTerritories.begin(), ownerTerritories.end(), territory) == ownerTerritories.end();
            if (isEnemyTerritory && territory->getNumberOfArmies() > maxArmies)
            {
                mostReinforcedEnemyTerritory = territory;
                maxArmies = territory->getNumberOfArmies();
            }
        }

        if (mostReinforcedEnemyTerritory != nullptr)
        {
            Player* targetPlayer = GameEngine::getOwnerOf(mostReinforcedEnemyTerritory);
            return new NegotiateOrder(owner_, targetPlayer);
        }
    }

    // If no suitable target player is found, pick a random enemy
    vector<Player*> enemyPlayers;
    for (const auto &player : GameEngine::getPlayers())
    {
        if (player != owner_)
        {
            enemyPlayers.push_back(player);
        }
    }
    Player* targetPlayer = enemyPlayers.at(rand() % enemyPlayers.size());
    return new NegotiateOrder(owner_, targetPlayer);
}
