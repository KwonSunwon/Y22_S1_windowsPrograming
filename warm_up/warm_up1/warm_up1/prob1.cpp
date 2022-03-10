#include <iostream>

int main()
{
    char input[60];
    char order = 0;

    std::cout << "Input sentenc : ";
    gets_s(input, sizeof(input));

    bool fFuncToggle = false;

    while (order != 'q')
    {
        std::cin >> order;
        switch (order)
        {
        case 'e':
        {
            bool checkE = false;
            int i = 0;

            while (input[i] != '.')
            {
                int letterCounter = 0;
                if (input[i] == ' ')
                {
                    ++i;
                    while (input[i] != ' ')
                    {
                        if (input[i] == 'e' || input[i] == 'E')
                            checkE = true;
                        ++i;
                        ++letterCounter;
                    }
                    if (checkE == true)
                    {
                        for (letterCounter; letterCounter < 0; --letterCounter)
                        {
                            if (input[i - letterCounter] >= 65 && input[i - letterCounter] <= 90)
                            {
                                input[i - letterCounter] += 32;
                            }
                            else if (input[i - letterCounter] >= 97 && input[i - letterCounter] <= 122)
                            {
                                input[i - letterCounter] -= 32;
                            }
                        }
                    }
                }
                ++i;
            }
            std::cout << input << "\n";
            break;
        }
        case 'f':
        {
            int i = 0;
            if (fFuncToggle == false)
            {
                while (input[i] != '.')
                {
                    if (input[i] == ' ')
                    {
                        input[i] = '@';
                    }
                    ++i;
                }
            }
            else
            {
                while (input[i] != '.')
                {
                    if (input[i] == '@')
                    {
                        input[i] = ' ';
                    }
                    ++i;
                }
            }
            std::cout << input << "\n";
            fFuncToggle = !fFuncToggle;
            break;
        }
        case 'l':
        {
            int wordsNumber = 0;
            int i = 0;
            while (input[i] != '\0')
            {
                if ((input[i] >= 65 && input[i] <= 90) || (input[i] >= 97 && input[i] <= 122))
                {
                    if (!((input[i + 1] >= 65 && input[i + 1] <= 90) || (input[i + 1] >= 97 && input[i + 1] <= 122)))
                        ++wordsNumber;
                }
                ++i;
            }
            std::cout << "Result : " << wordsNumber << " words\n";
            break;
        }
        case 'c':
        {
            int i = 0;
            while (input[i] != '.')
            {
                if (input[i] >= 65 && input[i] <= 90)
                {
                    input[i] += 32;
                }
                else if (input[i] >= 97 && input[i] <= 122)
                {
                    input[i] -= 32;
                }
                ++i;
            }
            std::cout << input << "\n";
            break;
        }
        case 'a':
        {
            int i = 0;
            char temp = input[0];
            while (input[i + 1] != '.')
            {
                input[i] = input[i + 1];
                ++i;
            }
            input[i] = temp;
            std::cout << input << "\n";
            break;
        }
        }
    }

    return 0;
}
