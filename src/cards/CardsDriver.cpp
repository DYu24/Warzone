#include "Cards.h"
#include <memory>

int main()
{
    // Setup
    Deck deck;
    deck.generateCards(5);

    cout << "===== " << " Initial Deck of cards - Size=" << deck.size() << " =====" << endl;
    for (const auto &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << endl;



    // Fill hand by randomly drawing from deck
    cout << "===== " << " Drawing cards from deck " << " =====" << endl;
    Hand hand;
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());

    cout << "\n***** " << " Deck after drawing - Size=" << deck.size() << " *****" << endl;
    for (const auto &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << "\n***** " << " Hand after drawing - Size=" << hand.size() << " *****" << endl;
    for (const auto &card : hand.getCards())
    {
        cout << *card << endl;
    }

    cout << endl;



    // Play the cards in the hand
    cout << "===== " << " Playing cards in Hand " << " =====" << endl;
    while (hand.size() != 0)
    {
        Card* card = hand.removeCard(0);
        Order* playResult = card->play();

        cout << *card << " generated: ";
        if (playResult != nullptr)
        {
            cout << *playResult << endl;
        }
        else
        {
            cout << "N/A" << endl;
        }

        deck.addCard(card);
    }

    cout << endl;


    // Show the deck and hand after playing
    cout << "***** " << " Deck after playing - Size=" << deck.size() << " *****" << endl;
    for (const auto &card : deck.getCards())
    {
        cout << *card << endl;
    }

    cout << "\n***** " << " Hand after playing - Size=" << hand.size() << " *****" << endl;
    for (const auto &card : hand.getCards())
    {
        cout << *card << endl;
    }

    return 0;
}