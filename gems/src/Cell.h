//
//  Cell.h
//  gems
//
//  Created by Богдан on 08.05.2026.
//

#pragma once

//#include <SFML/Graphics.hpp>

enum GemColor
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    COLOR_COUNT
};

struct Cell
{
    GemColor color;

    bool empty = false;

    int row = 0;
    int col = 0;

    float x = 0.f;
    float y = 0.f;

    float targetX = 0.f;
    float targetY = 0.f;

    bool selected = false;

   
    bool hasBonus = false;
    int bonusType = 0;
};
