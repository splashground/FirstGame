//
//  Cell.h
//  gems
//
//  Refactored: polymorphic gem hierarchy + GemFactory
//

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cstdlib>
#include <cmath>


enum GemColor
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    COLOR_COUNT
};


class Gem
{
public:
    GemColor color    = RED;
    bool     empty    = false;
    int      row      = 0;
    int      col      = 0;
    bool     selected = false;

    float x       = 0.f;
    float y       = 0.f;
    float targetX = 0.f;
    float targetY = 0.f;


    virtual bool hasBonus() const { return false; }


    virtual std::vector<std::pair<int,int>>
        onDestroy(std::vector<std::vector<std::unique_ptr<Gem>>>& /*grid*/,
                  int /*boardSize*/)
        { return {}; }

    virtual void draw(sf::RenderWindow& window, sf::Texture& texture) const
    {
        if (empty) return;
        sf::Sprite sprite(texture);
        sprite.setPosition({x, y});
        window.draw(sprite);
    }

    virtual ~Gem() = default;
};

using GemGrid = std::vector<std::vector<std::unique_ptr<Gem>>>;



class StandardGem : public Gem
{
};



class RepaintGem : public Gem
{
public:
    bool hasBonus() const override { return true; }

    std::vector<std::pair<int,int>>
        onDestroy(GemGrid& grid, int boardSize) override
    {
        std::vector<std::pair<int,int>> candidates;

        for (int i = -3; i <= 3; ++i)
        for (int j = -3; j <= 3; ++j)
        {
            int nr = row + i, nc = col + j;
            if (nr < 0 || nr >= boardSize || nc < 0 || nc >= boardSize) continue;
            if (nr == row && nc == col) continue;
            if (std::abs(i) + std::abs(j) > 1)
                candidates.emplace_back(nr, nc);
        }

        for (int i = 0; i < 2 && !candidates.empty(); ++i)
        {
            size_t idx = static_cast<size_t>(rand()) % candidates.size();
            auto [nr, nc] = candidates[idx];
            if (grid[nr][nc] && !grid[nr][nc]->empty)
                grid[nr][nc]->color = color;
            candidates.erase(candidates.begin() + idx);
        }

        return {};
    }

    void draw(sf::RenderWindow& window, sf::Texture& texture) const override
    {
        Gem::draw(window, texture);
        if (empty) return;

        sf::CircleShape badge(10.f);
        badge.setFillColor(sf::Color(255, 220, 0, 220));
        badge.setOutlineColor(sf::Color::White);
        badge.setOutlineThickness(1.5f);
        badge.setPosition({x + 4.f, y + 4.f});
        window.draw(badge);
    }
};



class BombGem : public Gem
{
public:
    bool hasBonus() const override { return true; }

    std::vector<std::pair<int,int>>
        onDestroy(GemGrid& grid, int boardSize) override
    {
        std::vector<std::pair<int,int>> extra;
        extra.push_back({row, col});

        int attempts = 0;
        while (extra.size() < 5 && attempts < 200)
        {
            ++attempts;
            int r = rand() % boardSize;
            int c = rand() % boardSize;

            if (!grid[r][c] || grid[r][c]->empty) continue;

            bool dup = false;
            for (auto& p : extra)
                if (p.first == r && p.second == c) { dup = true; break; }

            if (!dup) extra.emplace_back(r, c);
        }

        return extra;
    }

    void draw(sf::RenderWindow& window, sf::Texture& texture) const override
    {
        Gem::draw(window, texture);
        if (empty) return;

        sf::CircleShape badge(10.f);
        badge.setFillColor(sf::Color(220, 60, 60, 220));
        badge.setOutlineColor(sf::Color::White);
        badge.setOutlineThickness(1.5f);
        badge.setPosition({x + 4.f, y + 4.f});
        window.draw(badge);
    }
};



class GemFactory
{
public:
    static constexpr int BONUS_CHANCE   = 10;  // % of new gems that are bonus
    static constexpr int REPAINT_CHANCE = 50;  // % of bonus gems that repaint

    static std::unique_ptr<Gem> createRandom(GemColor color)
    {
        std::unique_ptr<Gem> gem;

        if (rand() % 100 < BONUS_CHANCE)
        {
            if (rand() % 100 < REPAINT_CHANCE)
                gem = std::make_unique<RepaintGem>();
            else
                gem = std::make_unique<BombGem>();
        }
        else
        {
            gem = std::make_unique<StandardGem>();
        }

        gem->color = color;
        return gem;
    }

    static std::unique_ptr<Gem> createStandard(GemColor color)
    {
        auto gem = std::make_unique<StandardGem>();
        gem->color = color;
        return gem;
    }
};
