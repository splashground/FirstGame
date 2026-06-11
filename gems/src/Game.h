//
//  Game.h
//  gems
//
//

#pragma once

#include "Board.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum GameState
{
    INPUT,      // waiting for player input
    SWAPPING,   // swap animation playing
    CHECKING,   // finding / destroying matches
    BONUS       // (reserved for future use)
};

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void draw();
    void handleClick(int mouseX, int mouseY);

    sf::RenderWindow window;
    Board            board;

    GameState state = INPUT;

    int selectedRow = -1;
    int selectedCol = -1;
    int secondRow   = -1;
    int secondCol   = -1;

    sf::Clock clock;
};
