//
//  Board.h
//  gems
//
//

#pragma once

#include "Cell.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Board
{
public:
    static const int SIZE      = 8;
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

private:
    GemGrid grid;

    sf::Texture textures[COLOR_COUNT];

    void initCell(int r, int c, bool allowBonus);
    Gem& at(int r, int c) { return *grid[r][c]; }
    const Gem& at(int r, int c) const { return *grid[r][c]; }
};
