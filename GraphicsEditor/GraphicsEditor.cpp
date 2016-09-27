#include "stdafx.h"
#include "GraphicsEditor.h"
#include <windows.h>
#include <CommDlg.h>
#include <stdio.h>
#include <string>

using namespace std;

#define MAX_LOADSTRING 100


HPEN hPen;
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int offsetX = 0, offsetY = 0;
double scale = 1;
LPACCEL TableAccel();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;
    
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHICSEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	hAccelTable = CreateAcceleratorTable(TableAccel(), 4);    

    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	DestroyAcceleratorTable(hAccelTable);
    return (int) msg.wParam;
}




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
	HWND hWnd;

	DWORD dColors[3] = { 255, 222, 222 };
	HWND hTest, hExit; // Elements of control windows
	CHOOSECOLOR cc;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(255, 0, 0);
}




BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

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
bool static flag = false, flag1 = false, flag2 = false, flag3 = true, bText;

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

HBITMAP hBitmap;

void LdBitmap(HDC hdc, HWND hWnd, RECT rect)
{

	HDC hdcMem = CreateCompatibleDC(hdc);
	if (flag1)
	{
		if (curBitmap>0)
		{
			curBitmap--;
		}
		else bitmaps[0] = PrevBitmap;
	}
	hBitmap = bitmaps[curBitmap];
	HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
	BITMAP bitmap;
	GetObject(hBitmap, sizeof(bitmap), &bitmap);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);

	DeleteDC(hdcMem);
}

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
	

	
	static COLORREF color;
	DWORD dColors[3] = { 255, 255, 255 };
	CHOOSECOLOR cc;
	
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(0x80, 0x80, 0x80);

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC static hdc, hdc1;

	static OPENFILENAME ofn;
	static char fullpath[256], filename[256], dir[256];
	HENHMETAFILE hEnhMtf;
	ENHMETAHEADER emh;

	TEXTMETRIC tm;
	static string text;
	static int cxChar, cyChar;
	static int xT, yT;

	PRINTDLG pd;
	static DOCINFO di;

	HBITMAP      hbmMem;
	HANDLE       hOld;
	static HDC hBitmapDC = 0;

	POINT static StartPoint, EndPoint, PrevPoint;
	RECT static rect, rect1;
	int static status = 10;
	int static width;
	HDC static hdcMem;
	HBITMAP static hBitmap;
	HGDIOBJ static oldBitmap;	
	int oldstatus;

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
		AppendMenu(SubMenuDraw, MF_STRING, 14, L"Triangle");
		AppendMenu(SubMenuDraw, MF_STRING, 16, L"Text");
		AppendMenu(SubMenuAction, MF_STRING, 20, L"Eraser");		
		AppendMenu(SubMenuAction, MF_POPUP, 30, L"Color");
		AppendMenu(SubMenuWidth, MF_STRING, 40, L"1");
		AppendMenu(SubMenuWidth, MF_STRING, 41, L"2");
		AppendMenu(SubMenuWidth, MF_STRING, 42, L"3");
		AppendMenu(SubMenuWidth, MF_STRING, 43, L"4");
		AppendMenu(SubMenuWidth, MF_STRING, 44, L"5");
		AppendMenu(SubMenuWidth, MF_STRING, 45, L"6");
		AppendMenu(SubMenuAbout, MF_STRING, IDM_ABOUT, L"About");		
		SetMenu(hWnd, MainMenu);
		hdc1 = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;
		break;
	
    case WM_COMMAND:
        {
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			bText = false;
			oldstatus = status;
			status = wParam;

            switch (wmId)
            {	
			case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case 0:
			{
			ofn.lStructSize=sizeof(OPENFILENAME);
			ofn.hwndOwner=hWnd;
			ofn.hInstance=hInst; // дескриптор копии приложения
			ofn.lpstrFilter = L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
			ofn.nFilterIndex=1;
			ofn.lpstrFile = (LPWSTR)fullpath;
    		ofn.nMaxFile=sizeof(fullpath);
			ofn.lpstrFileTitle=(LPWSTR)filename;
			ofn.nMaxFileTitle=sizeof(filename);
			ofn.lpstrInitialDir = (LPWSTR)dir;
			ofn.lpstrTitle  = L"Open file...";
			ofn.Flags=OFN_EXPLORER|OFN_CREATEPROMPT|OFN_ALLOWMULTISELECT;
			if(GetOpenFileName(&ofn))
			{
				scale=1;
				StartPoint.x=0;
				StartPoint.y=0;
                hdc1=CreateEnhMetaFile(NULL,NULL,NULL,NULL); 
			    hEnhMtf=GetEnhMetaFile((LPWSTR)fullpath);
				GetEnhMetaFileHeader(hEnhMtf,sizeof(ENHMETAHEADER),&emh);
				SetRect(&rect,emh.rclBounds.left,emh.rclBounds.top,emh.rclBounds.right,emh.rclBounds.bottom);
				DeleteObject(SelectObject(hdc1, hPen));
				PlayEnhMetaFile(hdc1,hEnhMtf,&rect);
				
				GetClientRect(hWnd,&rect1);
				DeleteDC(hdcMem);
				hdcMem =CreateCompatibleDC(hdc);
				hBitmap=CreateCompatibleBitmap(hdc,rect1.right,rect1.bottom);
				DeleteObject(SelectObject(hdcMem, hBitmap));
				PatBlt(hdcMem, 0, 0, rect1.right, rect1.bottom, PATCOPY);
				DeleteObject(SelectObject(hdcMem, hPen));
				PlayEnhMetaFile(hdcMem,hEnhMtf,&rect);

				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
				DeleteEnhMetaFile(hEnhMtf);
			}				
			}
				break;
			case 1:
			{
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400A;
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = (LPWSTR)filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = _T(".emf");
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetSaveFileName(&ofn)) {
					HDC hdc = GetDC(hWnd);
					HDC metafile;
					GetClientRect(hWnd, &rect);
					int nWidth = rect.right - rect.left;
					int nHeight = rect.bottom - rect.top;
					//HBITMAP hBitmap = CreateCompatibleBitmap(hdc, nWidth, nHeight);
					SelectObject(hdcMem, hBitmap);

					try {
						metafile = CreateEnhMetaFile(hdcMem, (LPCWSTR)ofn.lpstrFile, &rect, NULL);
					}

					catch (int e)
					{
						MessageBox(hWnd, NULL, (LPCWSTR)e, NULL);
					}


					BitBlt(metafile, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY);
					CloseEnhMetaFile(metafile);

				}
			}
				break;
			case 2:
			{
				ZeroMemory(&pd, sizeof(pd));
				pd.lStructSize = sizeof(pd);
				pd.hwndOwner = hWnd;
				pd.hDevMode = NULL; 
				pd.hDevNames = NULL; 
				pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
				pd.nCopies = 1;
				pd.nFromPage = 0xFFFF;
				pd.nToPage = 0xFFFF;
				pd.nMinPage = 1;
				pd.nMaxPage = 0xFFFF;


				if (PrintDlg(&pd) == TRUE)
				{
					int Rx = GetDeviceCaps(pd.hDC, LOGPIXELSX);
					int Ry = GetDeviceCaps(pd.hDC, LOGPIXELSY);
					di.cbSize = sizeof(DOCINFO);
					di.lpszDocName = L"Print Picture";
					di.fwType = NULL;
					di.lpszDatatype = NULL;
					di.lpszOutput = NULL;
					
					StartDoc(pd.hDC, &di);
					StartPage(pd.hDC);
					GetClientRect(hWnd, &rect);
					hdcMem = CreateCompatibleDC(hdc);
					hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
					int Rx1 = GetDeviceCaps(hdcMem, LOGPIXELSX);
					int Ry1 = GetDeviceCaps(hdcMem, LOGPIXELSY);
					hOld = SelectObject(hdcMem, hbmMem);
					FillRect(hdcMem, &rect, WHITE_BRUSH);
					StretchBlt(hdcMem, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale),
						hBitmapDC, StartPoint.x, StartPoint.y, rect.right, rect.bottom, SRCCOPY);
					SelectObject(hdcMem, (HBRUSH)GetStockObject(NULL_BRUSH));
					SelectObject(hdcMem, (HPEN)GetStockObject(BLACK_PEN));
					Rectangle(hdcMem, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale));
					//BitBlt(pd.hDC,0,0,rect.right,rect.bottom,hdcMem,0,0,SRCCOPY);
					StretchBlt(pd.hDC, 0, 0, (int)((float)(0.91*rect.right*((float)(Rx / Rx1)))), (int)((float)(0.91*rect.bottom*((float)(Ry / Ry1)))),
						hdcMem, 0, 0, rect.right, rect.bottom, SRCCOPY);
					SelectObject(hdcMem, hOld);
					DeleteObject(hbmMem);
					DeleteDC(hdcMem);
					EndPage(pd.hDC);
					EndDoc(pd.hDC);
					
					DeleteDC(pd.hDC);
				}
			}
				break;
			case 16:
				bText = true;
				break;
			case 30:
				if (ChooseColor(&cc)) {
					color = (COLORREF)cc.rgbResult;
				}
				status = oldstatus;
				break;			
			case 40:
			{
				width = 1;
				status = oldstatus;
			}
				break;
			case 41:
			{
				width = 2;
				status = oldstatus;
			}
			break;
			case 42:
			{
				width = 3;
				status = oldstatus;
			}
			break;
			case 43:
			{
				width = 4;
				status = oldstatus;
			}
			break;
			case 44:
			{
				width = 5;
				status = oldstatus;
			}
			break;
			case 45:
			{
				width = 6;
				status = oldstatus;
			}
			break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_LBUTTONDOWN:		
		if (flag3)
		{
			GetClientRect(hWnd, &rect);
			CrBitmap(hdc, rect);
			flag3 = false;
		}
		
		flag = true;
		StartPoint.x = LOWORD(lParam);
		StartPoint.y = HIWORD(lParam);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_LBUTTONUP:
		hdc = GetDC(hWnd);
		CrBitmap(hdc, rect);
		flag = false;
		if (bText)
		{
			xT = LOWORD(lParam);
			yT = HIWORD(lParam);
			text.clear();			
			break;
		}
		ReleaseDC(hWnd, hdc);
		break;
	case WM_MOUSEMOVE:		
		hdc = GetDC(hWnd);
		EndPoint.x = LOWORD(lParam);
		EndPoint.y = HIWORD(lParam);
		GetClientRect(hWnd, &rect);
		if (flag)
		{
			if (status == 10)
			{
				hPen = CreatePen(PS_SOLID, width, color);
				SelectObject(hdc, hPen);
				MoveToEx(hdc, PrevPoint.x, PrevPoint.y, NULL);
				LineTo(hdc, EndPoint.x, EndPoint.y);
			}

			if (status>10 && status<15)
			{
				hdcMem = CreateCompatibleDC(hdc);
				hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
				oldBitmap = SelectObject(hdcMem, hBitmap);
				FillRect(hdcMem, &rect, WHITE_BRUSH);
				LdBitmap(hdcMem, hWnd, rect);
			}
			switch (status)
			{

			case 11:
				hPen = CreatePen(PS_SOLID, width, color);
				SelectObject(hdcMem, hPen);
				MoveToEx(hdcMem, StartPoint.x, StartPoint.y, NULL);
				LineTo(hdcMem, EndPoint.x, EndPoint.y);
				break;

			case 14:
				hPen = CreatePen(PS_SOLID, width, color);
				SelectObject(hdcMem, hPen);
				MoveToEx(hdcMem, StartPoint.x, StartPoint.y, NULL);
				LineTo(hdcMem, StartPoint.x - (StartPoint.x - EndPoint.x), StartPoint.y);
				LineTo(hdcMem, StartPoint.x - (StartPoint.x - EndPoint.x) / 2, StartPoint.y - (StartPoint.y - EndPoint.y));
				LineTo(hdcMem, StartPoint.x, StartPoint.y);
				break;
			case 13:
				hPen = CreatePen(PS_SOLID, width, color);
				SelectObject(hdcMem, hPen);
				SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
				Rectangle(hdcMem, StartPoint.x, StartPoint.y, StartPoint.x - (StartPoint.x - EndPoint.x), StartPoint.y - (StartPoint.y - EndPoint.y));
				break;
			case 12:
				hPen = CreatePen(PS_SOLID, width, color);
				SelectObject(hdcMem, hPen);
				SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
				Ellipse(hdcMem, StartPoint.x, StartPoint.y, StartPoint.x - (StartPoint.x - EndPoint.x), StartPoint.y - (StartPoint.y - EndPoint.y));
				break;
			case 20:
				hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));
				SelectObject(hdc, hPen);
				MoveToEx(hdc, PrevPoint.x, PrevPoint.y, NULL);
				LineTo(hdc, EndPoint.x, EndPoint.y);
				break;

			default:
				break;
			}
			if (status>10 && status<15)
			{
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, oldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
			}
		}

		PrevPoint = EndPoint;
		GetFocus();
		ReleaseDC(hWnd, hdc);
		break;
	case WM_MOUSEWHEEL:
	{
		hdc = GetDC(hWnd);
						  hdcMem = CreateCompatibleDC(hdc);
						  hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
						  oldBitmap = SelectObject(hdcMem, hBitmap);
						  GetClientRect(hWnd, &rect);
						  LdBitmap(hdcMem, hWnd, rect);
						  if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
						  {
							  if (LOWORD(wParam) == MK_CONTROL)
							  {
								  scale += 0.04;
								  StretchBlt(hdc, 0, 0, rect.right * scale, rect.bottom * scale,
									  hdcMem, 0, 0, rect.right, rect.bottom, SRCCOPY);
							  }
								  else {
									  if (LOWORD(wParam) == MK_SHIFT)
										  {
										  offsetX += 5;
										  BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
										  }
									  else {
										  offsetY += 5;
										  BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
										  
									  }
								  }
						  }

						  if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
						  {
							if (LOWORD(wParam) == MK_CONTROL)
							  {
								  scale -= 0.04;
								  StretchBlt(hdc, 0, 0, rect.right * scale, rect.bottom * scale,
									  hdcMem, 0, 0, rect.right, rect.bottom, SRCCOPY);
							  }
								  else {
									 
									  if (LOWORD(wParam) == MK_SHIFT)
										   {
										  offsetX -= 5;
										  BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
										  }
									  else {
										  offsetY -= 5;
										  BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
										  
									  }
								  }
						  }

						  ReleaseDC(hWnd, hdc);
	}
	break;
	case  WM_CHAR:
		
		if (bText)
		{
			char c = (char)wParam;
			if (c == VK_RETURN)
				bText = false;
			else
				text += c;			
			TextOut(hdcMem, xT, yT, (LPWSTR)text.c_str(), strlen(text.c_str()));
			
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		break;
		HFONT hFont;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			LdBitmap(hdc, hWnd, rect);
			GetClientRect(hWnd, &rect);					

			flag1 = false;
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

LPACCEL TableAccel()
{
	ACCEL table[3];
	int i;
	for (i = 0; i < 3; i++)
	{
		table[i].fVirt = FSHIFT;
	}
	table[0].key = 0x53;
	table[0].cmd = 10;

	table[1].key = 0x50;
	table[1].cmd = 21;

	table[2].key = 0x51;
	table[2].cmd = IDM_EXIT;
	return table;
}