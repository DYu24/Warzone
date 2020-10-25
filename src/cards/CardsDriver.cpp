#include "Cards.h"
#include <memory>

int main()
{
    // Setup
    Deck deck;
    deck.addCard(make_shared<BombCard>());
    deck.addCard(make_shared<ReinforcementCard>());
    deck.addCard(make_shared<BlockadeCard>());
    deck.addCard(make_shared<AirliftCard>());
    deck.addCard(make_shared<DiplomacyCard>());

    cout << "-----" << " Initial Deck of cards - Size=" << deck.getCards().size() << " -----" << endl;
    for (auto const &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << endl;

    // Fill hand by randomly drawing from deck
    cout << "-----" << " Drawing cards from deck " << " -----" << endl;
    Hand hand;
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());

    cout << "*****" << " Deck after drawing - Size=" << deck.getCards().size() << " *****" << endl;
    for (auto const &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << "*****" << " Hand after drawing - Size=" << hand.getCards().size() << " *****" << endl;
    for (auto const &card : hand.getCards())
    {
        cout << *card << endl;
    }

    cout << endl;

    // Play the cards in the hand
    cout << "-----" << " Playing cards in Hand " << " -----" << endl;
    while (!hand.getCards().empty())
    {
        unique_ptr<Order> playResult = hand.playCardAt(0);
        cout << *playResult << endl;
        deck.addCard(hand.removeCard(0));
    }

    cout << endl;

    cout << "*****" << " Deck after playing - Size=" << deck.getCards().size() << " *****" << endl;
    for (auto const &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << "*****" << " Hand after playing - Size=" << hand.getCards().size() << " *****" << endl;
    for (auto const &card : hand.getCards())
    {
        cout << *card << endl;
    }

    return 0;
}