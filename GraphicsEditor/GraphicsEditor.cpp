// GraphicsEditor.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "GraphicsEditor.h"
#include <windows.h>
#include <CommDlg.h>
#include <stdio.h>
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
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

    // TODO: разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHICSEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICSEDITOR));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICSEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHICSEDITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HBITMAP bitmaps[5];
HBITMAP PrevBitmap;
int curBitmap = -1;
bool static flag = false, flag1 = false, flag2 = false, flag3 = true;

void CrBitmap(HDC hdc, RECT rect)
{
	int i;
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	HANDLE oldBitmap = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
	PrevBitmap = bitmaps[0];
	if (curBitmap<4)
	{
		curBitmap++;
		bitmaps[curBitmap] = hBitmap;

	}
	else
	{
		for (i = 0; i<4; i++)
		{
			bitmaps[i] = bitmaps[i + 1];

		}

		bitmaps[4] = hBitmap;
	}

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(oldBitmap);
	DeleteDC(hdcMem);

}
//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
BOOL fDraw = FALSE;
POINT ptPrevious = { 0 };

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU static MainMenu = CreateMenu();
	HMENU static SubMenuDraw = CreateMenu();
	HMENU static SubMenuAction = CreateMenu();
	HMENU static SubMenuFile = CreateMenu();
	HMENU static SubMenuWidth = CreateMenu();
	HMENU static SubMenuAbout = CreateMenu();
	HMENU static SubMenuColor = CreateMenu();

	static HBRUSH hBrush;
	static HPEN hPen;
	static COLORREF color;
	DWORD dColors[3] = { 255, 255, 255 };
	CHOOSECOLOR cc;
	
	HDC hdc;
	BOOL bRet = FALSE;
	BOOL bCmd = FALSE;
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);

	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(0x80, 0x80, 0x80);

    switch (message)
    {
	case WM_CREATE:
		AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)SubMenuFile, L"File");
		AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)SubMenuDraw, L"Draw");
		AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)SubMenuAction, L"Action");
		AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)SubMenuWidth, L"Width");
		AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)SubMenuAbout, L"About");
		AppendMenu(SubMenuFile, MF_STRING, 0, L"Open");
		AppendMenu(SubMenuFile, MF_STRING, 1, L"Save");
		AppendMenu(SubMenuFile, MF_STRING, 2, L"Print");
		AppendMenu(SubMenuFile, MF_STRING, IDM_EXIT, L"Exit");
		AppendMenu(SubMenuDraw, MF_STRING, 10, L"Pen");
		AppendMenu(SubMenuDraw, MF_STRING, 11, L"Line");
		AppendMenu(SubMenuDraw, MF_STRING, 12, L"Ellipse");
		AppendMenu(SubMenuDraw, MF_STRING, 13, L"Rectangle");
		AppendMenu(SubMenuDraw, MF_STRING, 14, L"Polyline");
		AppendMenu(SubMenuDraw, MF_STRING, 15, L"Text");
		AppendMenu(SubMenuAction, MF_STRING, 20, L"Eraser");
		AppendMenu(SubMenuAction, MF_STRING, 21, L"Pan");
		AppendMenu(SubMenuAction, MF_POPUP, (UINT_PTR)SubMenuColor, L"Color");
		AppendMenu(SubMenuWidth, MF_STRING, 40, L"1");
		AppendMenu(SubMenuWidth, MF_STRING, 41, L"2");
		AppendMenu(SubMenuWidth, MF_STRING, 42, L"3");
		AppendMenu(SubMenuWidth, MF_STRING, 43, L"4");
		AppendMenu(SubMenuWidth, MF_STRING, 44, L"5");
		AppendMenu(SubMenuWidth, MF_STRING, 45, L"6");
		AppendMenu(SubMenuAbout, MF_STRING, IDM_ABOUT, L"About");
		AppendMenu(SubMenuColor, MF_STRING, 61, L"Pen");
		AppendMenu(SubMenuColor, MF_STRING, 62, L"Brush");
		SetMenu(hWnd, MainMenu);
		break;
	case WM_INITDIALOG:
		hPen = CreatePen(PS_SOLID, 3, RGB(128, 0, 0));
		break;
    case WM_COMMAND:
        {            
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

	case WM_LBUTTONDOWN:		
		fDraw = TRUE;
		ptPrevious.x = LOWORD(lParam);
		ptPrevious.y = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		if (fDraw)
		{
			hdc = GetDC(hWnd);
			MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
			LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
			ReleaseDC(hWnd, hdc);
			fDraw = FALSE;
		}
		break;
	case WM_MOUSEMOVE:		
		if (fDraw)
		{
			hdc = GetDC(hWnd);
			MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
			LineTo(hdc, ptPrevious.x = LOWORD(lParam),ptPrevious.y = HIWORD(lParam));
			ReleaseDC(hWnd, hdc);
		}		
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);	
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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
