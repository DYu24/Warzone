#include "Cards.h"
#include <memory>

int main()
{
    // Setup
    Deck deck;
    deck.generateCards(5);

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
        Order* playResult = hand.playCardAt(0);
        if (playResult != NULL)
        {
            cout << *playResult << endl;
        }
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