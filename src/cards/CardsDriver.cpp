#include "../game_engine/GameEngine.h"
#include "Cards.h"

int main()
{
    // Setup
    Deck deck;
    deck.generateCards(5);

    std::cout << "===== " << " Initial Deck of cards - Size=" << deck.size() << " =====" << std::endl;
    for (const auto &card : deck.getCards())
    {
        std::cout << *card << std::endl;
    }

    std::cout << std::endl;



    // Fill hand by randomly drawing from deck
    std::cout << "===== " << " Drawing cards from deck " << " =====" << std::endl;
    Hand hand;
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());
    hand.addCard(deck.draw());

    std::cout << "\n***** " << " Deck after drawing - Size=" << deck.size() << " *****" << std::endl;
    for (const auto &card : deck.getCards())
    {
        std::cout << *card << std::endl;
    }

    std::cout << "\n***** " << " Hand after drawing - Size=" << hand.size() << " *****" << std::endl;
    for (const auto &card : hand.getCards())
    {
        std::cout << *card << std::endl;
    }

    std::cout << std::endl;



    // Play the cards in the hand
    std::cout << "===== " << " Playing cards in Hand " << " =====" << std::endl;
    while (hand.size() != 0)
    {
        Card* card = hand.removeCard(0);
        Order* playResult = card->play();

        std::cout << *card << " generated: ";
        if (playResult != nullptr)
        {
            std::cout << *playResult << std::endl;
            delete playResult;
        }
        else
        {
            std::cout << "N/A" << std::endl;
        }

        deck.addCard(card);
    }

    std::cout << std::endl;


    // Show the deck and hand after playing
    std::cout << "***** " << " Deck after playing - Size=" << deck.size() << " *****" << std::endl;
    for (const auto &card : deck.getCards())
    {
        std::cout << *card << std::endl;
    }

    std::cout << "\n***** " << " Hand after playing - Size=" << hand.size() << " *****" << std::endl;
    for (const auto &card : hand.getCards())
    {
        std::cout << *card << std::endl;
    }

    GameEngine::resetGameEngine();

    return 0;
}