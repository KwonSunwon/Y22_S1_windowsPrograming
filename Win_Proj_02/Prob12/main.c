#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600,
                        NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    HFONT hFont, oldFont;

    static RECT answerBox;

    static const TCHAR problem[5][10] = {
        {L"apple"},
        {L"secure"},
        {L"program"},
        {L"window"},
        {L"hangman"},
    };

    static int wordLength;

    static int *correct;

    static TCHAR answer[10];
    static TCHAR input[1];

    static int gameWin;
    static int gameLife;

    static int noCorrect;

    static POINT hangman[5][2] = {
        {{625, 200}, {630, 380}},
        {{625, 240}, {600, 330}},
        {{625, 240}, {650, 330}},
        {{630, 380}, {610, 460}},
        {{630, 380}, {640, 450}},
    };
    static RECT hangmanBase = {400, 500, 750, 580};
    static RECT hangmanHead = {575, 100, 675, 200};
    static POINT hangmanStick[4] = {
        {500, 500},
        {500, 50},
        {625, 50},
        {625, 100},
    };

    switch (iMessage)
    {
    case WM_CREATE:
        // ���� �ܾ� ����, ���� �迭�� ����
        srand(time(NULL));
        int randWord = rand() % 5;
        wordLength = lstrlen(problem[randWord]);

        printf("length %d\nrand %d\n", wordLength, randWord);

        for (int i = 0; i < wordLength; ++i)
        {
            answer[i] = problem[randWord][i];
        }
        answer[wordLength] = 0x00;

        correct = (int *)malloc(sizeof(int) * wordLength);

        // TEST
        //  int debugTemp;
        //  scanf("%d", &debugTemp);
        //  for (int i = 0; i < wordLength; ++i)
        //      correct[i] = debugTemp;

        for (int i = 0; i < wordLength; ++i)
            correct[i] = 0;

        input[0] = 0x00;

        gameLife = 5;
        gameWin = 0;
        noCorrect = 1;

        break;

    case WM_CHAR:
        input[0] = wParam;

        // �Է� ���� �� ��
        for (int i = 0; i < wordLength; ++i)
        {
            if (wParam == answer[i])
            {
                correct[i] = 1;
                noCorrect = 1;
            }
        }

        // ���� ���� Ȯ��
        int check = 1;
        for (int i = 0; i < wordLength; ++i)
            if (correct[i] == 0)
                check = 0;
        if (check)
            gameWin = 1;
        else
            --gameLife;

        InvalidateRect(hWnd, NULL, TRUE);

        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        oldFont = (HFONT)SelectObject(hdc, hFont);

        //[x] ȸ�� ��� ����
        hPen = CreatePen(PS_SOLID, 1, BLACK_PEN);
        oldPen = SelectObject(hdc, hPen);
        if (noCorrect == 0)
        {
            printf("ȸ�� ���\n");
            hBrush = CreateSolidBrush(0xd3d3d3);
            oldBrush = SelectObject(hdc, hBrush);
            SetBkColor(hdc, 0xd3d3d3);
        }
        else
        {
            printf("��� ���\n");
            hBrush = CreateSolidBrush(0xffffff);
            oldBrush = SelectObject(hdc, hBrush);
        }

        RECT box = {50, 100, 90, 160};

        answerBox = box;
        for (int i = 0; i < wordLength; ++i)
        {
            Rectangle(hdc, answerBox.left, answerBox.top, answerBox.right, answerBox.bottom);
            OffsetRect(&answerBox, 50, 0);
        }
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        answerBox = box;
        for (int i = 0; i < wordLength; ++i)
        {
            if (correct[i] == 1)
            {
                printf("���� �Է�\n");
                TextOut(hdc, answerBox.left + 10, answerBox.top + 11, answer + i, 1);
                // DrawText(hdc, &answerBox, answer + i, 1, DT_CENTER);
            }
            answerBox.left += 50;
            answerBox.right += 50;
        }
        noCorrect = 0;

        SetBkColor(hdc, 0xffffff);

        if (gameWin == 1)
        {
            TextOut(hdc, 150, 300, L"GameWin", lstrlen(L"GameWin"));
        }
        else if (gameLife == 0)
        {
            TextOut(hdc, 150, 300, L"GameDefeat", lstrlen(L"GameDefeat"));
        }
        else
        {
            // �Է�â
            TextOut(hdc, 200, 300, input, lstrlen(input));
        }
        POINT temp[2] = {{100, 330}, {330, 330}};
        Polygon(hdc, temp, 2);
        SelectObject(hdc, oldFont);

        // ��� �׸�
        FillRect(hdc, &hangmanBase, BLACK_BRUSH);
        Polyline(hdc, &hangmanStick, 4);
        Ellipse(hdc, hangmanHead.left, hangmanHead.top, hangmanHead.right, hangmanHead.bottom);
        for (int i = 0; i < 5 - gameLife; ++i)
        {
            Polygon(hdc, &hangman[i], 2);
        }

        DeleteObject(hPen);
        DeleteObject(hBrush);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// 1. ������ �ܾ �����ϰ�, �� �ܾ��� ���� ��ŭ ĭ�� ����
//  a. ������ �ܾ� rand() ���� -> ������ �ܾ� answer�� ����
//  b. �ܾ��� ���̸� �˾Ƴ��� �� ����ŭ ��� �ݺ��� ����
// [x]  c. �ܾ� ĭ �׸���� ���̸�ŭ �ݺ��ϸ鼭 Ư�� ��ǥ��ŭ ������ �̵�
//
// 2. ����ڿ��� ���ĺ��� �Է� ����
//  a. ���õ� �ܾ�� ������� �Է��� ���ϱ� ���� �ܾ��� ���� ��ŭ �ݺ�
//  b. ���ĺ��� �ش� �ܾ �ִٸ� �� ��ġ�� ����
// 2-1.
// [x]  a. �ܾ ���ĺ��� ���ԵǾ����� �ʴٸ� ĭ�� ����� ��ο� ������ �ٲ�
// [x]  b. ����� �� ������ �Ŵ޸�
// 2-2. �ܾ ���ĺ��� ���ԵǾ� �ִٸ� �˸��� ĭ�� ä������
//  a. �˸��� ĭ�� ä�� �ִ� ���� �� ĭ�� �ܾ� ����(����ĭ)�� �׸� �Ŀ�
//    ������ ������ ��ġ�� ���� �׷�����
//    -> ���̸�ŭ int�� �迭�� ������ ���� bool Ÿ��ó�� 0, 1�� Ȯ���ؼ� �����ϸ� �� ��?

// 3-1. ����� ���� �Ŵ޸��� ���� �ܾ �� ����ٸ� ���ӿ��� �¸�
//  a. �ܾ� �Է��� �� ���� �� ���� ������ �迭�� �Ǵ�(���� 1�̸� �¸�)
//  b. �¸��ϸ� ���� �ܾ �� ����ϰ� �Է� ĭ�� ���� �¸� ��� - ���� �ٽ� ������ �� ���ص� �� ��?
// 3-2. 5���� ��ȸ�� �� �����ߴٸ� ���ӿ��� �й�
//  a. WM_CHAR �������� ���� ��ȸ Ƚ���� �Ǵ�

//�Է�ĭ �� ����ĭ �׸���
// [x] ���ǿ� ���� ��� �׸���
//  ��� �׸��� ���� �迭�� �����س��� �ϳ��� �׸��� �ɵ�?