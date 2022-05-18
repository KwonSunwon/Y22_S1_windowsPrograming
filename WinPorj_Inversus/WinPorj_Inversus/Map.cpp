#pragma once

#include "Map.h"

Map::Map()
{
    level = NORMAL;
    _map.resize(ROW);
    for (int i = 0; i < ROW; ++i)
        for (int j = 0; j < N_COL; ++j)
            _map[i].push_back(Normal_Mode[i][j]);
}

void Map::change(int _level)
{
    level = _level;

    _map.resize(0);
    _map.resize(15);

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
}

void Map::draw(HDC mdc, int point)
{
    int size = MAP_SIZE;
    int row = ROW;
    int col;
    switch (level)
    {
    case 0:
        col = E_COL;
        break;
    case 1:
    case 2:
        col = N_COL;
        break;
    }

    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    for (int i = 0; i < row; ++i)
        for (int j = 0; j < col; ++j)
        {
            if (_map[i][j] == 0)
            {
                hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                oldPen = (HPEN)SelectObject(mdc, hPen);
                hBrush = CreateSolidBrush(RGB(255, 255, 255));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, point + size * j, point + size * i, point + size * (j + 1), point + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
                SelectObject(mdc, oldPen);
                DeleteObject(hPen);
            }
            else if (_map[i][j] == 1)
            {
                hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                oldPen = (HPEN)SelectObject(mdc, hPen);
                hBrush = CreateSolidBrush(RGB(0, 0, 0));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, point + size * j, point + size * i, point + size * (j + 1), point + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
                SelectObject(mdc, oldPen);
                DeleteObject(hPen);
            }
            else if (_map[i][j] == 2)
            {
                hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                oldPen = (HPEN)SelectObject(mdc, hPen);
                hBrush = CreateSolidBrush(RGB(180, 0, 0));
                oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                Rectangle(mdc, point + size * j, point + size * i, point + size * (j + 1), point + size * (i + 1));
                SelectObject(mdc, oldBrush);
                DeleteObject(hBrush);
                SelectObject(mdc, oldPen);
                DeleteObject(hPen);
            }
        }
}

int Map::object(int row, int col)
{
    return _map[row][col];
}