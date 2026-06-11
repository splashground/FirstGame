//
//  Board.cpp
//  gems
//
//

#include "Board.h"
#include <set>
#include <cmath>
#include <cstdlib>
#include <iostream>

Board::Board()
{
    
    const char* colorNames[] = {"red", "green", "blue", "yellow", "purple"};
    for (int i = 0; i < COLOR_COUNT; ++i)
    {
        if (!textures[i].loadFromFile("assets/" + std::string(colorNames[i]) + ".png"))
            std::cout << "Failed to load: assets/" << colorNames[i] << ".png\n";
    }

    grid.resize(SIZE);
    for (int r = 0; r < SIZE; r++)
        grid[r].resize(SIZE);

    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            initCell(r, c, false);

    while (!findMatches().empty())
    {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
            {
                auto color = static_cast<GemColor>(rand() % COLOR_COUNT);
                grid[r][c] = GemFactory::createStandard(color);
                grid[r][c]->row = r;
                grid[r][c]->col = c;
                grid[r][c]->x = grid[r][c]->targetX = c * CELL_SIZE;
                grid[r][c]->y = grid[r][c]->targetY = r * CELL_SIZE;
            }
    }
}

void Board::initCell(int r, int c, bool allowBonus)
{
    auto color = static_cast<GemColor>(rand() % COLOR_COUNT);

    grid[r][c] = allowBonus
                 ? GemFactory::createRandom(color)
                 : GemFactory::createStandard(color);

    Gem& gem = *grid[r][c];
    gem.row     = r;
    gem.col     = c;
    gem.x = gem.targetX = c * CELL_SIZE;
    gem.y = gem.targetY = r * CELL_SIZE;
    gem.empty   = false;
    gem.selected = false;
}

std::vector<std::pair<int,int>> Board::findMatches()
{
    std::set<std::pair<int,int>> matches;

    for (int r = 0; r < SIZE; r++)
    {
        int count = 1;
        for (int c = 1; c < SIZE; c++)
        {
            const Gem& cur  = at(r, c);
            const Gem& prev = at(r, c - 1);

            if (!cur.empty && !prev.empty && cur.color == prev.color)
            {
                ++count;
            }
            else
            {
                if (count >= 3)
                    for (int k = 0; k < count; k++)
                        matches.insert({r, c - 1 - k});
                count = 1;
            }
        }
        if (count >= 3)
            for (int k = 0; k < count; k++)
                matches.insert({r, SIZE - 1 - k});
    }

    for (int c = 0; c < SIZE; c++)
    {
        int count = 1;
        for (int r = 1; r < SIZE; r++)
        {
            const Gem& cur  = at(r, c);
            const Gem& prev = at(r - 1, c);

            if (!cur.empty && !prev.empty && cur.color == prev.color)
            {
                ++count;
            }
            else
            {
                if (count >= 3)
                    for (int k = 0; k < count; k++)
                        matches.insert({r - 1 - k, c});
                count = 1;
            }
        }
        if (count >= 3)
            for (int k = 0; k < count; k++)
                matches.insert({SIZE - 1 - k, c});
    }

    return {matches.begin(), matches.end()};
}


void Board::destroyMatches(const std::vector<std::pair<int,int>>& matches)
{
    
    std::set<std::pair<int,int>> toDestroy(matches.begin(), matches.end());

    for (auto [r, c] : toDestroy)
    {
        Gem& gem = at(r, c);
        if (gem.empty) continue;

        if (gem.hasBonus())
        {
            auto extra = gem.onDestroy(grid, SIZE);
            for (auto& p : extra)
                toDestroy.insert(p);
        }
    }

    for (auto [r, c] : toDestroy)
        at(r, c).empty = true;
}


void Board::dropCells()
{
    for (int c = 0; c < SIZE; ++c)
    {
        int write = SIZE - 1;
        for (int r = SIZE - 1; r >= 0; --r)
        {
            if (!at(r, c).empty)
            {
                if (r != write)
                {
                    std::swap(grid[write][c], grid[r][c]);
                    at(write, c).targetY = write * CELL_SIZE;
                    at(write, c).row     = write;
                }
                --write;
            }
        }
        for (int r = write; r >= 0; --r)
            at(r, c).empty = true;
    }
}


bool Board::swapCells(int r1, int c1, int r2, int c2)
{
    if (r1 < 0 || r1 >= SIZE || c1 < 0 || c1 >= SIZE ||
        r2 < 0 || r2 >= SIZE || c2 < 0 || c2 >= SIZE)
        return false;

    if (std::abs(r1 - r2) + std::abs(c1 - c2) != 1)
        return false;

    std::swap(grid[r1][c1], grid[r2][c2]);

    at(r1, c1).targetX = c1 * CELL_SIZE;  at(r1, c1).targetY = r1 * CELL_SIZE;
    at(r2, c2).targetX = c2 * CELL_SIZE;  at(r2, c2).targetY = r2 * CELL_SIZE;
    at(r1, c1).row = r1;  at(r1, c1).col = c1;
    at(r2, c2).row = r2;  at(r2, c2).col = c2;

    return true;
}


void Board::spawnCells()
{
    for (int c = 0; c < SIZE; ++c)
    {
        for (int r = 0; r < SIZE; ++r)
        {
            if (!at(r, c).empty) continue;

            auto color = static_cast<GemColor>(rand() % COLOR_COUNT);
            grid[r][c] = GemFactory::createRandom(color);   // may be a bonus gem

            Gem& gem    = *grid[r][c];
            gem.empty   = false;
            gem.row     = r;
            gem.col     = c;
            gem.x = gem.targetX = c * CELL_SIZE;
            gem.y       = -CELL_SIZE * (r + 2.0f);   // start above screen
            gem.targetY = r * CELL_SIZE;
        }
    }
}


bool Board::animationsFinished()
{
    for (const auto& row : grid)
        for (const auto& gem : row)
            if (std::fabs(gem->y - gem->targetY) > 0.5f ||
                std::fabs(gem->x - gem->targetX) > 0.5f)
                return false;
    return true;
}

void Board::update(float dt)
{
    for (auto& row : grid)
        for (auto& gem : row)
        {
            gem->x += (gem->targetX - gem->x) * dt * 12.f;
            gem->y += (gem->targetY - gem->y) * dt * 12.f;
        }
}


void Board::draw(sf::RenderWindow& window)
{
    for (auto& row : grid)
        for (auto& gem : row)
            if (!gem->empty)
                gem->draw(window, textures[gem->color]);
}
