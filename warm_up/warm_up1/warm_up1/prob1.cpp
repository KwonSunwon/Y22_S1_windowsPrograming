#include <iostream>

using namespace std;

int main()
{
    char input[60];
    char order = 0;

    cout << "Input sentenc : ";
    gets_s(input, sizeof(input));

    bool fFuncToggle = false;

    while (order != 'q')
    {
        cin >> order;
        switch (order)
        {
        case 'e':
        {
            int i = 0;

            while (input[i] != '.')
            {
                bool checkE = false;
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
                        if (input[i+1] == '.')
                            break;
                    }
                    if (checkE == true)
                    {
                        for (letterCounter; letterCounter > 0; --letterCounter)
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
                if(checkE == false)
                    ++i;
            }
            cout << input << "\n";
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
            cout << input << "\n";
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
            cout << "Result : " << wordsNumber << " words\n";
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
            cout << input << "\n";
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
            cout << input << "\n";
            break;
        }
        }
    }

    return 0;
}
