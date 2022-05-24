#pragma once

#include "Map.h"

Map::Map()
{
    level = NORMAL;
    _map.resize(ROW);
    for (int i = 0; i < ROW; ++i)
        for (int j = 0; j < N_COL; ++j)
            _map[i].push_back(Normal_Mode[i][j]);
    mapSize = map_size();
}

void Map::change(int _level)
{
    level = _level;

    _map.resize(0);
    _map.resize(ROW);

    switch (level)
    {
    case 0:
        for (int i = 0; i < ROW; ++i)
            for (int j = 0; j < E_COL; ++j)
                _map[i].push_back(Easy_Mode[i][j]);
        break;
    case 1:
        for (int i = 0; i < ROW; ++i)
            for (int j = 0; j < N_COL; ++j)
                _map[i].push_back(Normal_Mode[i][j]);
        break;
    case 2:
        for (int i = 0; i < ROW; ++i)
            for (int j = 0; j < N_COL; ++j)
                _map[i].push_back(Hard_Mode[i][j]);
        break;
    }
    mapSize = map_size();
}

void Map::draw(HDC mdc)
{
    int size = MAP_SIZE;
    int row = get_row();
    int col = get_col();

    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;
    hPen = CreatePen(PS_SOLID, 1, RGB(175, 175, 175));
    oldPen = (HPEN)SelectObject(mdc, hPen);
    for (int i = 0; i < row; ++i)
        for (int j = 0; j < col; ++j)
        {
            if (_map[i][j] == 0)
            {
                hBrush = CreateSolidBrush(RGB(255, 255, 255));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, MAP_LOCATION + size * j, MAP_LOCATION + size * i, MAP_LOCATION + size * (j + 1), MAP_LOCATION + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
            }
            else if (_map[i][j] == 1)
            {
                hBrush = CreateSolidBrush(RGB(0, 0, 0));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, MAP_LOCATION + size * j, MAP_LOCATION + size * i, MAP_LOCATION + size * (j + 1), MAP_LOCATION + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
            }
            else if (_map[i][j] == 2)
            {
                hBrush = CreateSolidBrush(RGB(180, 0, 0));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, MAP_LOCATION + size * j, MAP_LOCATION + size * i, MAP_LOCATION + size * (j + 1), MAP_LOCATION + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
            }
        }
    SelectObject(mdc, oldPen);
    DeleteObject(hPen);
}

int Map::get_object(POINT pt)
{
    cout << _map[pt.y][pt.x] << "\n";
    return _map[pt.y][pt.x];
}

int Map::get_level()
{
    return level;
}

RECT Map::map_size()
{
    int row = get_row();
    int col = get_col();

    RECT temp;
    temp.left = MAP_LOCATION;
    temp.right = MAP_LOCATION + MAP_SIZE * col;
    temp.top = MAP_LOCATION;
    temp.bottom = MAP_LOCATION + MAP_SIZE * row;

    return temp;
}

void Map::tile_change_black(POINT pt)
{
    _map[pt.y][pt.x] = BLACK;
}
void Map::tile_change_white(POINT pt)
{
    _map[pt.y][pt.x] = WHITE;
}

RECT Map::get_map_size()
{
    return mapSize;
}

RECT Map::get_tile_rect(POINT pt)
{
    RECT temp = {
        MAP_LOCATION + pt.x * MAP_SIZE,
        MAP_LOCATION + pt.y * MAP_SIZE,
        MAP_LOCATION + (pt.x + 1) * MAP_SIZE,
        MAP_LOCATION + (pt.y + 1) * MAP_SIZE,
    };
    return temp;
}

int Map::get_col()
{
    switch (level)
    {
    case 0:
        return E_COL;
        break;
    case 1:
    case 2:
        return N_COL;
        break;
    }
}

int Map::get_row()
{
    return ROW;
}