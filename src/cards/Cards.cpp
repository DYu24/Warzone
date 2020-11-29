#include "Cards.h"
#include "../game_engine/GameEngine.h"
#include <algorithm>
#include <time.h>
using std::cin;
using std::cout;
using std::endl;
using std::numeric_limits;
using std::streamsize;


namespace
{
    // Helper method to get the adversaries of the specified player.
    vector<Player*> getEnemiesOf(Player* player)
    {
        vector<Player*> enemies;
        for (const auto &p : GameEngine::getPlayers())
        {
            if (p != player)
            {
                enemies.push_back(p);
            }
        }
        return enemies;
    }
}


/* 
===================================
 Implementation for Card class
===================================
 */

// Destructor
Card::~Card() {};

// Operator overloading
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

// Get the card at the specified position
Card* Hand::at(int position)
{
    if (position < 0 || position > cards_.size() - 1)
    {
        return nullptr;
    }

    return cards_.at(position);
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
Order* BombCard::play() const
{
    if (owner_ == nullptr)
    {
        return new BombOrder();
    }

    if (owner_->isHuman())
    {
        return buildOrder_();
    }

    // Bomb the highest priority territory in the `toAttack` list
    return new BombOrder(owner_, owner_->toAttack().front());
}

// Build the BombOrder through user input.
Order* BombCard::buildOrder_() const
{
    // Determine which territories are bombable
    vector<Territory*> bombableTerritories;
    for (const auto &player : getEnemiesOf(owner_))
    {
        vector<Territory*> enemyTerritories = player->getOwnedTerritories();
        bombableTerritories.insert(bombableTerritories.end(), enemyTerritories.begin(), enemyTerritories.end());
    }

    cout << "\nWhich territory would you like to bomb?" << endl;
    for (int i = 0; i < bombableTerritories.size(); i++)
    {
        Territory* territory = bombableTerritories.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " present)" << endl;
    }

    Territory* target = nullptr;
    cout << "\nEnter the territory to bomb: ";
    while (target == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= bombableTerritories.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        target = bombableTerritories.at(selection - 1);
    }

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
Order* ReinforcementCard::play() const
{
    if (owner_ != nullptr)
    {
        owner_->addReinforcements(5);
    }

    return nullptr;
}

// Do nothing since ReinforcementCard returns no order
Order* ReinforcementCard::buildOrder_() const
{
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
Order* BlockadeCard::play() const
{
    if (owner_ == nullptr)
    {
        return new BlockadeOrder();
    }

    if (owner_->isHuman())
    {
        return buildOrder_();
    }

    // Setup a blockade on the territory with lowest defend priority
    return new BlockadeOrder(owner_, owner_->toDefend().back());
}

// Build the BlockadeOrder through user input.
Order* BlockadeCard::buildOrder_() const
{
    vector<Territory*> blockadableTerritories = owner_->getOwnedTerritories();

    cout << "\nWhich territory would you like to blockade?" << endl;
    for (int i = 0; i < blockadableTerritories.size(); i++)
    {
        Territory* territory = blockadableTerritories.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " present, " << territory->getPendingIncomingArmies() << " pending)" << endl;
    }

    Territory* target = nullptr;
    cout << "\nEnter the territory to blockade: ";
    while (target == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= blockadableTerritories.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        target = blockadableTerritories.at(selection - 1);
    }

    return new BlockadeOrder(owner_, target);
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
Order* AirliftCard::play() const
{
    if (owner_ == nullptr || owner_->getOwnedTerritories().size() == 1)
    {
        return new AirliftOrder();
    }

    if (owner_->isHuman())
    {
        return buildOrder_();
    }

    // Choose the player's territory with the most movable armies as the source
    int maxArmies = 0;
    int movableArmies = 0;
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

    // Pick the highest priority territory to defend (that isn't the source)
    Territory* destination = nullptr;
    for (const auto &territory : owner_->toDefend())
    {
        if (territory != source)
        {
            destination = territory;
            break;
        }
    }

    source->addPendingOutgoingArmies(movableArmies);
    return new AirliftOrder(owner_, movableArmies, source, destination);
}

// Build the AirliftOrder through user input.
Order* AirliftCard::buildOrder_() const
{
    vector<Territory*> possibleSources = owner_->getOwnTerritoriesWithMovableArmies();

    cout << "\nWhich territory would you like to airlift from?" << endl;
    for (int i = 0; i < possibleSources.size(); i++)
    {
        Territory* territory = possibleSources.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfMovableArmies() << " armies available)" << endl;
    }

    Territory* source = nullptr;
    cout << "\nEnter the territory to airlift from: ";
    while (source == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= possibleSources.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        source = possibleSources.at(selection - 1);
    }

    // Destinations can only be territories that are not the source
    vector<Territory*> possibleDestinations;
    for (const auto &destination : owner_->toDefend())
    {
        if (destination != source)
        {
            possibleDestinations.push_back(destination);
        }
    }

    cout << "\nWhich territory would you like to airlift to?" << endl;
    for (int i = 0; i < possibleDestinations.size(); i++)
    {
        Territory* territory = possibleDestinations.at(i);
        cout << "[" << i+1 << "] " << territory->getName() << " (" << territory->getNumberOfArmies() << " armies present)" << endl;
    }

    Territory* destination = nullptr;
    cout << "\nEnter the territory to airlift to: ";
    while (destination == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= possibleDestinations.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        destination = possibleDestinations.at(selection - 1);
    }

    int armiesToMove = 0;
    int movableArmies = source->getNumberOfMovableArmies();
    cout << "\nHow many armies do you want to move?" << endl;
    while (armiesToMove == 0)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection < 1 || selection > movableArmies)
        {
            cout << "Please enter a number between 1 and " << movableArmies << ": " << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        armiesToMove = selection;
    }

    source->addPendingOutgoingArmies(armiesToMove);
    return new AdvanceOrder(owner_, armiesToMove, source, destination);
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
Order* DiplomacyCard::play() const
{
    if (owner_ == nullptr || GameEngine::getPlayers().size() < 2)
    {
        return new NegotiateOrder();
    }

    if (owner_->isHuman())
    {
        return buildOrder_();
    }

    Map* map = GameEngine::getMap();
    vector<Territory*> ownerTerritories = owner_->getOwnedTerritories();
    vector<Territory*> territoriesToDefend = owner_->toDefend();

    // Try to pick an enemy player who has the most armies on an adjacent territory to the highest priority territory in
    // the player's `toDefend()` list
    for (const auto &territory : territoriesToDefend)
    {
        int maxArmies = 0;
        Territory* mostReinforcedEnemyTerritory = nullptr;

        for (const auto &neighbor : map->getAdjacentTerritories(territory))
        {
            bool isEnemyTerritory = find(ownerTerritories.begin(), ownerTerritories.end(), neighbor) == ownerTerritories.end();
            if (isEnemyTerritory && neighbor->getNumberOfArmies() > maxArmies)
            {
                mostReinforcedEnemyTerritory = neighbor;
                maxArmies = neighbor->getNumberOfArmies();
            }
        }

        if (mostReinforcedEnemyTerritory != nullptr)
        {
            Player* targetPlayer = GameEngine::getOwnerOf(mostReinforcedEnemyTerritory);
            return new NegotiateOrder(owner_, targetPlayer);
        }
    }

    // If no suitable target player is found, pick a random enemy
    srand(time(nullptr));
    vector<Player*> enemyPlayers = getEnemiesOf(owner_);
    Player* targetPlayer = enemyPlayers.at(rand() % enemyPlayers.size());
    return new NegotiateOrder(owner_, targetPlayer);
}

// Build the NegotiateOrder through user input.
Order* DiplomacyCard::buildOrder_() const
{
    vector<Player*> enemyPlayers = getEnemiesOf(owner_);

    cout << "\nWho would you like to negotiate with?" << endl;
    for (int i = 0; i < enemyPlayers.size(); i++)
    {
        Player* enemy = enemyPlayers.at(i);
        cout << "[" << i+1 << "] " << enemy->getName() << endl;
    }

    Player* targetPlayer = nullptr;
    cout << "\nEnter the player to negotiate with: ";
    while (targetPlayer == nullptr)
    {
        int selection;
        cin >> selection;

        if (cin.fail() || selection - 1 < 0 || selection - 1 >= enemyPlayers.size())
        {
            cout << "That was not a valid option. Please try again:" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        targetPlayer = enemyPlayers.at(selection - 1);
    }

    return new NegotiateOrder(owner_, targetPlayer);
}
