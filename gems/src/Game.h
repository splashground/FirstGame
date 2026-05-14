//
//  Game.h
//  gems
//
//  Created by Богдан on 08.05.2026.
//

// Game.h
#pragma once

#include "Board.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum GameState
{
    INPUT,      // ожидание ввода игрока
    SWAPPING,   // анимация обмена
    CHECKING,   // поиск и уничтожение матчей
    FALLING,    // падение и спавн
    BONUS       // обработка бонусов (если нужно)
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
    Board board;

    GameState state = INPUT;

    int selectedRow = -1;
    int selectedCol = -1;

    sf::Clock clock;
};
