#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");
#define NumberSize 9
#define MatrixSize (NumberSize * NumberSize)

class Number
{
	int number;
	BOOL fixed;
public:
	Number() { number = 0; fixed = FALSE; }
	BOOL IsHint() { return fixed; }
	operator int() { return number; }
	int operator=(int n) { number = n; return number; }
	void SetHint(int n) { number = n; fixed = TRUE; }
};

class NumberArray
{
	Number items[MatrixSize];
	void getTryNumber(int tryNum[], int n);
	BOOL tryPlace(int n);
public:
	NumberArray() {};
	BOOL GetAnswer();
	Number* operator[](int n) { return &items[n]; }
};

BOOL NumberArray::GetAnswer()
{
	for (int i = 0; i < MatrixSize; i++)
	{
		if (!items[i].IsHint()) items[i] = 0;
	}
	return tryPlace(0);
}

BOOL NumberArray::tryPlace(int n)
{
	int tryNumber[NumberSize + 1], *pTryNumber;
	if (n == MatrixSize) return TRUE;
	if (items[n].IsHint()) return tryPlace(n + 1);

	getTryNumber(tryNumber, n);
	for (pTryNumber = tryNumber; *pTryNumber; pTryNumber++)
	{
		items[n] = *pTryNumber;
		if (tryPlace(n + 1)) return TRUE;
		items[n] = 0;
	}
	return FALSE;
}

void NumberArray::getTryNumber(int tryNum[], int n)
{
	int tmpnum[NumberSize];
	int offset[] = {
		1, 1, NumberSize - 2,
		1, 1, NumberSize - 2,
		1, 1, NumberSize - 2
	};
	int i, j;
	int index;

	for (i = 0; i < NumberSize; i++) tmpnum[i] = i + 1;

	//横のチェック
	index = (n / NumberSize)*NumberSize;
	for (i = 0; i < NumberSize; i++)
	{
		int number = items[index];
		if (number) tmpnum[number - 1] = 0;
		index++;
	}

	//縦のチェック
	index = n % NumberSize;
	for (i = 0; i < NumberSize; i++)
	{
		int number = items[index];
		if (number) tmpnum[number - 1] = 0;
		index += NumberSize;
	}

	//ブロックのチェック
	index = (n / 3) / 9 * 27 + ((n / 3) % 3) * 3;
	for (i = 0; i < NumberSize; i++)
	{
		int number = items[index];
		if (number) tmpnum[number - 1] = 0;
		index += offset[i];
	}

	for (i = 0, j = 0; i < NumberSize; i++)
	{
		if (tmpnum[i]) tryNum[j++] = tmpnum[i];
	}
	tryNum[j] = 0;
}

static NumberArray numbers;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("解く"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		{
			const int table[] = {
				7,0,0,0,5,0,3,0,0,
				0,5,0,0,0,2,0,0,0,
				0,0,8,0,7,0,6,0,2,
				0,0,0,1,0,0,0,9,0,
				5,0,9,0,6,0,7,0,4,
				0,6,0,0,0,3,0,0,0,
				9,0,1,0,8,0,5,0,0,
				0,0,0,9,0,0,0,6,0,
				0,0,7,0,3,0,0,0,1,
			};
			for (int i = 0; i < MatrixSize; i++)
			{
				if (table[i])
					(numbers[i])->SetHint(table[i]);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			TCHAR szText[1024];
			for (int x = 0; x < NumberSize; x++)
			{
				for (int y = 0; y < NumberSize; y++)
				{
					const int nNumber = (int)(*numbers[y*NumberSize + x%NumberSize]);
					wsprintf(szText, TEXT("%d"), nNumber);
					TextOut(hdc, x * 32, y * 32 + 50, szText, lstrlen(szText));
				}
			}

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			const BOOL result = numbers.GetAnswer();
			InvalidateRect(hWnd, 0, 1);
			if (result)
				MessageBox(hWnd, TEXT("解が得られました"), TEXT("確認"), 0);
			else
				MessageBox(hWnd, TEXT("解が得られませんでした"), TEXT("確認"), 0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("数独を解く"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
