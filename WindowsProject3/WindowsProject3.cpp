// WindowsProject3.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject3.h"
#include <vector>
#include "math.h"
#include <cstdlib>
#include <limits>
#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <string>  
HFONT hFont;

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

HPEN penRed;
HPEN penWhite;
HPEN penBlue;
HBRUSH brush;

int getRandomStringNumber() {
    // Генерация случайного числа от 1 до 20
    int randomNumber = (std::rand() % 20) + 1; // `rand()` возвращает числа от 0 до 19, добавляем 1
    return randomNumber; // Преобразуем число в строку и возвращаем
}

struct point
{
    float x;
    float y;
};
struct edge
{
    int i1;
    int i2;
    int length = 0;
};
std::vector<edge> edgelist;
std::vector<point> pointlist;

float mouseX;
float mouseY;

void DrawTextGDI(float x, float y, const char* str)
{
    SetTextColor(window.context, RGB(255, 255, 255));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    TextOutA(window.context, x * window.width, y * window.height, str, strlen(str));
}

void getMouse()
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(window.hWnd, &p);
    mouseX = p.x / (float)window.width;
    mouseY = p.y / (float)window.height;
}

void drawBack()
{
    RECT rect;
    GetClientRect(window.hWnd, &rect);
    auto blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(window.context, &rect, blackBrush);
    DeleteObject(blackBrush);
}

void drawPoint(float x, float y, float sz)
{
    Ellipse(window.context,
        x * window.width - sz * window.width,
        y * window.height - sz * window.width,
        x * window.width + sz * window.width,
        y * window.height + sz * window.width
    );

}
void drawRectangle(float x1, float y1, float x2, float y2)
{

    Rectangle(window.context, x1 * window.width, y1 * window.height, x2 * window.width, y2 * window.height);
}

void Line(float x, float y, float x1, float y1)
{
    MoveToEx(window.context, x * window.width, y * window.height, NULL);
    LineTo(window.context, x1 * window.width, y1 * window.height);
}

const int INF = 999999999999999; 

// Рекурсивная функция для алгоритма Дейкстры
void dijkstraUtil(int u, std::vector<int>& dist, const std::vector<edge>& edgelist, std::vector<bool>& visited) {
    visited[u] = true; // Помечаем узел как посещенный

    // Обходим все рёбра
    for (const auto& edge : edgelist) {
        int v = -1;
        if (edge.i1 == u) {
            v = edge.i2; // Соединение с ребром
        }
        else if (edge.i2 == u) {
            v = edge.i1; // Соединение с ребром
        }

        // Если найден более короткий путь
        if (v != -1 && !visited[v]) {
            if (dist[u] + edge.length < dist[v]) {
                dist[v] = dist[u] + edge.length; // Обновляем расстояние
            }
        }
    }

    // Ищем следующий узел с минимальным расстоянием
    int nextNode = -1;
    int minDist = INF; // Используем постоянное значение INF

    for (int i = 0; i < dist.size(); ++i) {
        if (!visited[i] && dist[i] < minDist) {
            minDist = dist[i];
            nextNode = i;
        }
    }

    // Запускаем рекурсию для следующего узла
    if (nextNode != -1) {
        dijkstraUtil(nextNode, dist, edgelist, visited);
    }
}

// Основная функция Дейкстры
void dijkstra(int start, std::vector<point>& pointlist, std::vector<edge>& edgelist) {
    int n = pointlist.size();
    std::vector<int> dist(n, INF); // Массив для расстояний
    std::vector<bool> visited(n, false); // Массив посещенных вершин
    dist[start] = 0; // Расстояние до стартовой вершины

    // Рекурсивный вызов
    dijkstraUtil(start, dist, edgelist, visited);
    
}


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

    MSG msg;

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);
    //поиграем шрифтами и цветами
    
    hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    penRed = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    penWhite = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    penBlue = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
    brush = CreateSolidBrush(RGB(128, 128, 128));

    bool tap = false;
    float pointsize = 14. / window.width;
    int ind1 = -1;
    int ind2 = -1;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        getMouse();

        drawBack();

        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if(tap == false)
            {
                point p = { mouseX, mouseY };
                pointlist.push_back(p);
                tap = true;
            }
        }
        else
        {
            tap = false;
        }
        
        if (GetAsyncKeyState(VK_RBUTTON))
        {

            if (ind1 == -1)
            {

                for (int i = 0; i < pointlist.size(); i++)
                {
                    float dx = pointlist[i].x - mouseX;
                    float dy = pointlist[i].y - mouseY;
                    float l = sqrt(dx * dx + dy * dy);
                    if (l < pointsize)
                    {
                        ind1 = i;
                        break;
                    }
                }
            }

            if (ind1 >= 0)
            {
                Line(pointlist[ind1].x, pointlist[ind1].y, mouseX, mouseY);

                if (ind2 == -1)
                {
                    for (int i = 0; i < pointlist.size(); i++)
                    {
                        float dx = pointlist[i].x - mouseX;
                        float dy = pointlist[i].y - mouseY;
                        float l = sqrt(dx * dx + dy * dy);
                        if (l < pointsize and ind1 != i)
                        {
                            ind2 = i;
                            break;
                        }
                    }
                }

            }
        }
        else
        {
            if (ind1 != -1 and ind2 != -1)
            {
                edgelist.push_back({ ind1, ind2 });
            }
            ind1 = -1;
            ind2 = -1;
        }

        SelectObject(window.context, penWhite);
        SelectObject(window.context, brush);
        
        drawRectangle(0.05, 0.05, 0.10, 0.1);
        DrawTextGDI(0.060, 0.06, "set edges");
        drawRectangle(0.150, 0.05, 0.210, 0.1);
        DrawTextGDI(0.160, 0.06, "set points");
        
        for (int i = 0; i < edgelist.size(); i++)
        {
            int i1 = edgelist[i].i1;
            int i2 = edgelist[i].i2;
            if (edgelist[i].length == 0)
            {
                edgelist[i].length = getRandomStringNumber();
            }
            float x = (pointlist[i2].x + pointlist[i1].x) / 2;
            float y = (pointlist[i2].y + pointlist[i1].y) / 2;
            std::string t = std::to_string(edgelist[i].length);
            DrawTextGDI(x+0.01, y + 0.02, t.c_str());
            Line(pointlist[i1].x, pointlist[i1].y, pointlist[i2].x, pointlist[i2].y);
        }

        for (int i = 0; i < pointlist.size(); i++)
        {
            drawPoint(pointlist[i].x, pointlist[i].y, pointsize);
            std::string x = std::to_string(i+1);
            DrawTextGDI(pointlist[i].x-0.002, pointlist[i].y - 0.01, x.c_str());
        }
        dijkstra(0, pointlist, edgelist);
        SelectObject(window.context, penRed);
        

                 
             

        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   window.hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
      0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

   if (!window.hWnd)
   {
      return FALSE;
   }

   ShowWindow(window.hWnd, SW_MAXIMIZE);
   UpdateWindow(window.hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
