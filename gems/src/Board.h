//
//  Board.h
//  gems
//
//  Created by Богдан on 08.05.2026.
//

// Board.h
#pragma once

#include "Cell.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Board
{
public:
    static const int SIZE = 8;
    static const int CELL_SIZE = 64;

    Board();

    void update(float dt);
    void draw(sf::RenderWindow& window);

    bool swapCells(int r1, int c1, int r2, int c2);

    std::vector<std::pair<int,int>> findMatches();
    void destroyMatches(const std::vector<std::pair<int,int>>& matches);

    void dropCells();
    void spawnCells();

    bool animationsFinished();

    // Новые методы для бонусов
    void triggerBonus(int r, int c, GemColor originalColor);
    void repaintBonus(int r, int c, GemColor color);
    void bombBonus();

private:
    std::vector<std::vector<Cell>> grid;
    sf::Texture textures[COLOR_COUNT];
};
