#include <iostream>
#include <stack>

using namespace std;

typedef struct _OPER
{
    int prior;
    char op;
} OPER;

float calculate(stack<float> *num, stack<OPER> *oper);

int main(void)
{
    char input[14] = {'\0'};
    stack<float> num;
    stack<OPER> oper;

    cin >> input;

    float temp = 0;
    int i = 0;
    int prior = 0;

    while (input[i] != '\0')
    {
        if (input[i] > 47 && input[i] < 58)
        {
            temp = temp * 10 + (input[i] - 48);
            ++i;
        }
        else
        {
            num.push(temp);
            temp = 0;
            switch (input[i])
            {
            case '+':
            case '-':
                prior = 1;
                break;
            case '*':
            case '/':
                prior = 2;
                break;
            }
            while (!oper.empty() && prior <= oper.top().prior)
            {
                num.push(calculate(&num, &oper));
            }
            oper.push({prior, input[i]});
            ++i;
        }
    }

    num.push(temp);

    while (!oper.empty())
    {
        num.push(calculate(&num, &oper));
    }

    cout << input << " = " << num.top();

    return 0;
}

float calculate(stack<float> *num, stack<OPER> *oper)
{
    float n1 = num->top();
    num->pop();
    float n2 = num->top();
    num->pop();

    switch (oper->top().op)
    {
    case '+':
        oper->pop();
        return n2 + n1;
    case '-':
        oper->pop();
        return n2 - n1;
    case '*':
        oper->pop();
        return n2 * n1;
    case '/':
        oper->pop();
        return n2 / n1;
    }
}