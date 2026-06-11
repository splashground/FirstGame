//
//  Game.cpp
//  gems
//
//

#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game() :
    window(sf::VideoMode({Board::SIZE * Board::CELL_SIZE,
                          Board::SIZE * Board::CELL_SIZE}), "GEMS")
{
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));
    state = INPUT;
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        draw();
    }
}

void Game::processEvents()
{
    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        if (state == INPUT && event->is<sf::Event::MouseButtonPressed>())
        {
            auto mouse = event->getIf<sf::Event::MouseButtonPressed>();
            if (mouse && mouse->button == sf::Mouse::Button::Left)
                handleClick(mouse->position.x, mouse->position.y);
        }
    }
}

void Game::handleClick(int mx, int my)
{
    int c = mx / Board::CELL_SIZE;
    int r = my / Board::CELL_SIZE;

    if (r < 0 || r >= Board::SIZE || c < 0 || c >= Board::SIZE)
        return;

    if (selectedRow == -1)
    {
        selectedRow = r;
        selectedCol = c;
    }
    else
    {
        if (std::abs(selectedRow - r) + std::abs(selectedCol - c) == 1)
        {
            board.swapCells(selectedRow, selectedCol, r, c);
            secondRow = r;
            secondCol = c;
            state     = SWAPPING;
        }
        else
        {
            selectedRow = r;
            selectedCol = c;
        }
    }
}

void Game::update(float dt)
{
    board.update(dt);

    switch (state)
    {
    case SWAPPING:
        if (board.animationsFinished())
            state = CHECKING;
        break;  // ← was missing in original; caused fall-through into CHECKING

    case CHECKING:
    {
        auto matches = board.findMatches();
        if (!matches.empty())
        {
            board.destroyMatches(matches);
            board.dropCells();
            board.spawnCells();

        }
        else
        {
            // No matches after swap → reverse the swap.
            board.swapCells(selectedRow, selectedCol, secondRow, secondCol);
            selectedRow = selectedCol = -1;
            state = INPUT;
        }
        break;
    }

    case INPUT:
        break;

    default:
        break;
    }
}

void Game::draw()
{
    window.clear(sf::Color(30, 30, 40));
    board.draw(window);
    window.display();
}
