#include <iostream>
#include <windows.h>
#include <random>

std::random_device re;
std::mt19937 gen(re());
std::uniform_int_distribution<int> loc(0, 9);
std::uniform_int_distribution<int> sta(0, 4);

typedef struct _STONE
{
    int row, column;
    char state;
} STONE;

void stoneInit(STONE *, STONE *);
void drawField(STONE *, STONE *);
void moveUp(STONE *);
void moveDown(STONE *);
void moveLeft(STONE *);
void moveRight(STONE *);
void changeState(STONE *);

int main()
{
    STONE player1;
    STONE player2;
    STONE *turnPlayer = nullptr;
    STONE *otherPlayer = nullptr;

    stoneInit(&player1, &player2);
    drawField(&player1, &player2);

    char order = 0;

    turnPlayer = &player1;
    otherPlayer = &player2;

    while (order != 'q')
    {
        std::cin >> order;
        switch (order)
        {
        case 'r':
        {
            stoneInit(&player1, &player2);
            turnPlayer = &player1;
            otherPlayer = &player2;
            drawField(&player1, &player2);
            break;
        }
        case 'w':
        {
            moveUp(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'a':
        {
            moveLeft(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 's':
        {
            moveDown(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'd':
        {
            moveRight(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'i':
        {
            moveUp(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'j':
        {
            moveLeft(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'k':
        {
            moveDown(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        case 'l':
        {
            moveRight(turnPlayer);
            drawField(&player1, &player2);
            break;
        }
        }
        if (turnPlayer == &player1)
            turnPlayer = &player2;
        else
            turnPlayer = &player1;
    }
}

void stoneInit(STONE *player1, STONE *player2)
{
    player1->state = 'o';
    player2->state = 'x';

    int randRow1, randRow2, randCol1, randCol2;
    randRow1 = loc(gen);
    randRow2 = loc(gen);
    while (randRow1 == randRow2)
    {
        randRow1 = loc(gen);
    }

    randCol1 = loc(gen);
    randCol2 = loc(gen);

    player1->row = randRow1;
    player1->column = randCol1;

    player2->row = randRow2;
    player2->column = randCol2;
}

void drawField(STONE *player1, STONE *player2)
{
    system("cls");
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "_____________________\n";
        for (int j = 0; j < 10; ++j)
        {
            std::cout << "|";
            if (player1->row == i && player1->column == j)
            {
                std::cout << player1->state;
            }
            if (player2->row == i && player2->column == j)
            {
                std::cout << player2->state;
            }
            std::cout << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "_____________________\n";
}

void moveUp(STONE *stone)
{
    stone->row -= 1;
    if (stone->row < 0)
    {
        stone->row = 9;
        changeState(stone);
    }
}

void moveDown(STONE *stone)
{
    stone->row += 1;
    if (stone->row > 9)
    {
        stone->row = 0;
        changeState(stone);
    }
}

void moveLeft(STONE *stone)
{
    stone->column -= 1;
    if (stone->column < 0)
    {
        stone->column = 9;
        changeState(stone);
    }
}

void moveRight(STONE *stone)
{
    stone->column += 1;
    if (stone->column > 9)
    {
        stone->column = 0;
        changeState(stone);
    }
}

void changeState(STONE *stone)
{
    int randState = sta(gen);
    switch (randState)
    {
    case 0:
        stone->state = 'o';
        break;
    case 1:
        stone->state = 'x';
        break;
    case 2:
        stone->state = '#';
        break;
    case 3:
        stone->state = '@';
        break;
    case 4:
        stone->state = '*';
        break;
    }
}
