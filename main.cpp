#include <iostream>
#include <random>

std::random_device re;
std::mt19937 gen(re());
std::uniform_int_distribution<int> uid(1, 20);
std::uniform_int_distribution<int> shuffle(0, 19);

void makeRandomMatrix(int random[4][5]);
void showMatrix(int random[4][5]);

int main()
{
    int matrix[4][5];

    makeRandomMatrix(matrix);
    showMatrix(matrix);

    char order = 0;

    while (order != 'q')
    {
        std::cin >> order;

        switch (order)
        {
        case 'a':
        {
            for (int i = 0; i < 20 - 1; ++i) // int arr[5][5]; 에서 arr[1][0] == arr[0][5]
            {
                for (int j = 0; j < (20 - 1 - i); ++j)
                {
                    if (matrix[0][j] > matrix[0][j + 1])
                    {
                        int temp = matrix[0][j];
                        matrix[0][j] = matrix[0][j + 1];
                        matrix[0][j + 1] = temp;
                    }
                }
            }
            showMatrix(matrix);
            break;
        }
        case 'd':
        {
            int tempArr[20];

            int k = 0;

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    tempArr[k] = matrix[i][j];
                    ++k;
                }
            }

            for (int i = 0; i < 19; ++i)
            {
                for (int j = 0; j < 19 - i; ++j)
                {
                    if (tempArr[j] > tempArr[j + 1])
                    {
                        int temp = tempArr[j];
                        tempArr[j] = tempArr[j + 1];
                        tempArr[j + 1] = temp;
                    }
                }
            }

            k = 0;

            for (int i = 0; i < 5; ++i)
            {
                for (int j = 3; j > -1; --j)
                {
                    matrix[j][i] = tempArr[k];
                    k++;
                }
            }

            showMatrix(matrix);
            break;
        }
        case 'e':
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    if ((matrix[i][j] % 2) != 0)
                    {
                        matrix[i][j] = 0;
                    }
                }
            }
            showMatrix(matrix);
            break;
        }
        case 'o':
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    if ((matrix[i][j] % 2) == 0)
                    {
                        matrix[i][j] = 0;
                    }
                }
            }
            showMatrix(matrix);
            break;
        }
        case 'm':
        {
            int maximum = matrix[0][0];
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    if (matrix[i][j] >= maximum)
                    {
                        maximum = matrix[i][j];
                    }
                }
            }
            std::cout << maximum;
            break;
        }
        case 'n':
        {
            int minimum = matrix[0][0];
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    if (matrix[i][j] <= minimum)
                    {
                        minimum = matrix[i][j];
                    }
                }
            }
            std::cout << minimum;
            break;
        }
        case 'p':
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 1; j < 5; ++j)
                {
                    matrix[i][0] += matrix[i][j];
                }
            }
            showMatrix(matrix);
            break;
        }
        case 'r':
        {
            int numberShuffler[20];

            int k = 0;

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    numberShuffler[k] = matrix[i][j];
                    ++k;
                }
            }

            for (int i = 0; i < 50; ++i)
            {
                int ranNum = shuffle(gen);
                int ranNum2 = shuffle(gen);

                int temp = numberShuffler[ranNum];
                numberShuffler[ranNum] = numberShuffler[ranNum2];
                numberShuffler[ranNum2] = temp;
            }

            k = 0;

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    matrix[i][j] = numberShuffler[k];
                    ++k;
                }
            }
            showMatrix(matrix);
            break;
        }
        case 's':
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    matrix[i][j] = 0;
                }
            }
            makeRandomMatrix(matrix);
            showMatrix(matrix);
            break;
        }
        }
    }
}

void makeRandomMatrix(int random[4][5])
{
    // int randomNumber[20];
    // for (int i = 0; i < 20; ++i)
    // {
    //     int temp = uid(gen);
    //     randomNumber[i] = temp;
    //     for (int j = 0; j < i; ++j)
    //     {
    //         if (randomNumber[j] == temp)
    //         {
    //             if (i != j)
    //             {
    //                 --i;
    //             }
    //         }
    //     }
    // }

    // int k = 0;

    // for (int i = 0; i < 4; ++i)
    // {
    //     for (int j = 0; j < 5; ++j)
    //     {
    //         random[i][j] = randomNumber[k];
    //         ++k;
    //     }
    // }

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int temp = uid(gen);
            random[i][j] = temp;
            for (int k = 0; k < i + 1; ++k)
            {
                for (int l = 0; l < 5; ++l)
                {
                    if (random[k][l] == temp)
                    {
                        if (i != k || j != l)
                        {
                            if (j == 0)
                            {
                                --i;
                                j = 5;
                                break;
                            }
                            else
                            {
                                --j;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void showMatrix(int random[4][5])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 5; ++j)
            std::cout << random[i][j] << "\t";
        std::cout << "\n";
    }
}