//
//  Board.cpp
//  gems
//
//  Created by Богдан on 08.05.2026.
//

// Board.cpp
#include "Board.h"
#include <set>
#include <cmath>
#include <cstdlib>
#include <iostream>

Board::Board()
{
    // === Правильная загрузка текстур ===
    const char* colorNames[] = {"red", "green", "blue", "yellow", "purple"};
    
    for (int i = 0; i < COLOR_COUNT; ++i)
    {
        if (!textures[i].loadFromFile("assets/" + std::string(colorNames[i]) + ".png"))
        {
            std::cout << "Failed to load: assets/" << colorNames[i] << ".png\n";
        }
    }

    grid.resize(SIZE, std::vector<Cell>(SIZE));

    for (int r = 0; r < SIZE; r++)
    {
        for (int c = 0; c < SIZE; c++)
        {
            auto& cell = grid[r][c];
            cell.color = static_cast<GemColor>(rand() % COLOR_COUNT);
            cell.row = r;
            cell.col = c;
            cell.x = cell.targetX = c * CELL_SIZE;
            cell.y = cell.targetY = r * CELL_SIZE;
            cell.empty = false;
            cell.hasBonus = false;
            cell.bonusType = 0;
        }
    }
    while (!findMatches().empty())
    {
        for (auto& row : grid)
        {
            for (auto& cell : row)
            {
                cell.color =
                    static_cast<GemColor>(
                        rand() % COLOR_COUNT
                    );
            }
        }
    }
    
}

// ======================== DFS (поиск групп) ========================
static void dfs(int r, int c, GemColor color,
                std::vector<std::pair<int,int>>& out,
                std::vector<std::vector<bool>>& visited,
                std::vector<std::vector<Cell>>& grid)
{
    if (r < 0 || r >= Board::SIZE || c < 0 || c >= Board::SIZE) return;
    if (visited[r][c] || grid[r][c].empty || grid[r][c].color != color) return;

    visited[r][c] = true;
    out.push_back({r, c});

    dfs(r+1, c, color, out, visited, grid);
    dfs(r-1, c, color, out, visited, grid);
    dfs(r, c+1, color, out, visited, grid);
    dfs(r, c-1, color, out, visited, grid);
}

std::vector<std::pair<int,int>> Board::findMatches()
{
    std::set<std::pair<int,int>> matches;

    // горизонтали
    for (int r = 0; r < SIZE; r++)
    {
        int count = 1;

        for (int c = 1; c < SIZE; c++)
        {
            if (!grid[r][c].empty &&
                !grid[r][c - 1].empty &&
                grid[r][c].color == grid[r][c - 1].color)
            {
                count++;
            }
            else
            {
                if (count >= 3)
                {
                    for (int k = 0; k < count; k++)
                        matches.insert({r, c - 1 - k});
                }

                count = 1;
            }
        }

        if (count >= 3)
        {
            for (int k = 0; k < count; k++)
                matches.insert({r, SIZE - 1 - k});
        }
    }

    // вертикали
    for (int c = 0; c < SIZE; c++)
    {
        int count = 1;

        for (int r = 1; r < SIZE; r++)
        {
            if (!grid[r][c].empty &&
                !grid[r - 1][c].empty &&
                grid[r][c].color == grid[r - 1][c].color)
            {
                count++;
            }
            else
            {
                if (count >= 3)
                {
                    for (int k = 0; k < count; k++)
                        matches.insert({r - 1 - k, c});
                }

                count = 1;
            }
        }

        if (count >= 3)
        {
            for (int k = 0; k < count; k++)
                matches.insert({SIZE - 1 - k, c});
        }
    }

    return {
        matches.begin(),
        matches.end()
    };
}

// ======================== УНИЧТОЖЕНИЕ ========================
void Board::destroyMatches(const std::vector<std::pair<int,int>>& matches)
{
    std::set<std::pair<int,int>> unique(matches.begin(), matches.end());

    for (auto [r, c] : unique)
    {
        Cell& cell = grid[r][c];
        if (cell.empty) continue;

        if (cell.hasBonus)
        {
            triggerBonus(r, c, cell.color);
        }

        cell.empty = true;
        cell.hasBonus = false;
        cell.bonusType = 0;
    }
}

// ======================== БОНУСЫ ========================
void Board::triggerBonus(int r, int c, GemColor originalColor)
{
    // Можно сделать рандомный выбор типа бонуса или использовать cell.bonusType
    if (rand() % 2 == 0)
        repaintBonus(r, c, originalColor);
    else
        bombBonus(r, c);
}

void Board::repaintBonus(int r, int c, GemColor color)
{
    std::vector<std::pair<int,int>> candidates;

    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            int nr = r + i;
            int nc = c + j;
            if (nr < 0 || nr >= SIZE || nc < 0 || nc >= SIZE) continue;
            if (nr == r && nc == c) continue;

            // Не сосед (манхэттенское расстояние > 1)
            if (std::abs(i) + std::abs(j) > 1)
                candidates.emplace_back(nr, nc);
        }
    }

    if (candidates.empty()) return;


    // Перекрашиваем до 2 случайных несоседних
    for (int i = 0; i < 2 && !candidates.empty(); ++i)
    {
        size_t idx = rand() % candidates.size();
        auto [nr, nc] = candidates[idx];

        if (!grid[nr][nc].empty)
            grid[nr][nc].color = color;

        candidates.erase(candidates.begin() + idx);
    }
}

void Board::bombBonus(int centerR, int centerC)
{
    std::set<std::pair<int,int>> cells;

    cells.insert({centerR, centerC});

    while (cells.size() < 5)
    {
        int r = rand() % SIZE;
        int c = rand() % SIZE;

        if (!grid[r][c].empty)
        {
            cells.insert({r, c});
        }
    }

    for (auto& p : cells)
    {
        grid[p.first][p.second].empty = true;
    }
}

// ======================== ГРАВИТАЦИЯ И СПАВН ========================
void Board::dropCells()
{
    for (int c = 0; c < SIZE; ++c)
    {
        int write = SIZE - 1;
        for (int r = SIZE - 1; r >= 0; --r)
        {
            if (!grid[r][c].empty)
            {
                if (r != write)
                {
                    std::swap(grid[write][c], grid[r][c]);
                    grid[write][c].targetY = write * CELL_SIZE;
                }
                --write;
            }
        }
        for (int r = write; r >= 0; --r)
            grid[r][c].empty = true;
    }
}

bool Board::swapCells(int r1, int c1, int r2, int c2)
{
    // Проверка на валидность и соседство
    if (r1 < 0 || r1 >= SIZE || c1 < 0 || c1 >= SIZE ||
        r2 < 0 || r2 >= SIZE || c2 < 0 || c2 >= SIZE)
        return false;

    if (std::abs(r1 - r2) + std::abs(c1 - c2) != 1)
        return false;

    // Сам свап
    std::swap(grid[r1][c1], grid[r2][c2]);

    // Обновляем позиции для анимации
    grid[r1][c1].targetX = c1 * CELL_SIZE;
    grid[r1][c1].targetY = r1 * CELL_SIZE;

    grid[r2][c2].targetX = c2 * CELL_SIZE;
    grid[r2][c2].targetY = r2 * CELL_SIZE;

    // Обновляем row/col
    grid[r1][c1].row = r1;
    grid[r1][c1].col = c1;
    grid[r2][c2].row = r2;
    grid[r2][c2].col = c2;

    return true;
}

bool Board::animationsFinished()
{
    for (const auto& row : grid)
    {
        for (const auto& cell : row)
        {
            if (std::fabs(cell.y - cell.targetY) > 0.5f ||
                std::fabs(cell.x - cell.targetX) > 0.5f)
                return false;
        }
    }
    return true;
}

void Board::update(float dt)
{
    for (auto& row : grid)
    {
        for (auto& cell : row)
        {
            cell.x += (cell.targetX - cell.x) * dt * 12.f;
            cell.y += (cell.targetY - cell.y) * dt * 12.f;
        }
    }
}

void Board::draw(sf::RenderWindow& window)
{
    for (auto& row : grid)
    {
        for (auto& cell : row)
        {
            if (cell.empty) continue;

            sf::Sprite sprite(textures[cell.color]);
            sprite.setPosition({cell.x, cell.y});
            window.draw(sprite);

            if (cell.hasBonus)
            {
                sf::CircleShape bonus(12.f);
                bonus.setFillColor(sf::Color::Yellow);
                bonus.setPosition({cell.x + 20, cell.y + 20});
                window.draw(bonus);
            }
        }
    }
}

void Board::spawnCells()
{
    for (int c = 0; c < SIZE; ++c)
    {
        for (int r = 0; r < SIZE; ++r)
        {
            if (grid[r][c].empty)
            {
                Cell& cell = grid[r][c];

                cell.empty = false;
                cell.color = static_cast<GemColor>(rand() % COLOR_COUNT);
                cell.hasBonus = (rand() % 100 < 10);  // 10% шанс выпадения бонуса
                cell.bonusType = 0;

                // Появляются сверху с небольшой задержкой
                cell.x = cell.targetX = c * CELL_SIZE;
                cell.y = -CELL_SIZE * (r + 2.0f);
                cell.targetY = r * CELL_SIZE;

                cell.row = r;
                cell.col = c;
            }
        }
    }
}
