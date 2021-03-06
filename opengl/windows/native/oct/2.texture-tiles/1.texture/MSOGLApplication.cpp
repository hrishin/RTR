#include <Windows.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#include "resource.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable decalration
HWND ghwnd = NULL;
HDC ghdc   = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint gbTextureSmily;
int gbDigitPressed;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	void initialize(void);
	void display(void);
	void uninitialize(void);

	// varible declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("MyOGL_FFP");
	bool bDone = false;

	// WNDCLASS initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OPENGL Fixed Function Pipeline Using Native Windowing : Basic Texture"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// message loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}

			display();
		}
	}

	uninitialize();

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void resize(int, int);
	void toggleFullScreen(void);
	void uninitialize(void);

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;

		case 0X46: // for 'F' or 'f'
			toggleFullScreen();
			gbFullscreen = !gbFullscreen;
			break;

		case '1':
			gbDigitPressed = 1;
			break;

		case '2':
			gbDigitPressed = 2;
			break;

		case '3':
			gbDigitPressed = 3;
			break;

		case '4':
			gbDigitPressed = 4;
			break;

		default:
			break;
		}
		break;
	
	case WM_MBUTTONDOWN:
		break;

	case WM_CLOSE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void toggleFullScreen()
{
	MONITORINFO mi;

	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = {sizeof(MONITORINFO)};
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	// function declarations
	void resize(int, int);
	int LoadTexture(GLuint *, TCHAR[]);
	
	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// PIXELFORMATDESCRIPTOR Initialization
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

										  
	glShadeModel(GL_SMOOTH);				// shade model
	// depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	LoadTexture(&gbTextureSmily, MAKEINTRESOURCE(IDB_BITMAP_SMILY));

	resize(WIN_WIDTH, WIN_HEIGHT);
}

int LoadTexture(GLuint *texture, TCHAR resource[])
{
	//variable declarations
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	glGenTextures(1, texture);
	hBitmap = (HBITMAP) LoadImage(GetModuleHandle(NULL), resource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		DeleteObject(hBitmap);
	}

	return(iStatus);
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -4.0f);

	glBindTexture(GL_TEXTURE_2D, gbTextureSmily);

	switch (gbDigitPressed)
	{
	case 1:
		glBegin(GL_QUADS);

		glTexCoord2f(0.5, 0.5);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 0.5);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glTexCoord2f(0.5, 0);
		glVertex3f(1.0f, -1.0f, 1.0f);

		glEnd();

		break;
	case 2:
		glBegin(GL_QUADS);

		glTexCoord2f(1, 1);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 1);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glTexCoord2f(1, 0);
		glVertex3f(1.0f, -1.0f, 1.0f);

		glEnd();

		break;
	case 3:
		glBegin(GL_QUADS);

		glTexCoord2f(2, 2);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 2);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glTexCoord2f(2, 0);
		glVertex3f(1.0f, -1.0f, 1.0f);

		glEnd();

		break;
	case 4:
		glBegin(GL_QUADS);

		glTexCoord2f(0.5, 0.5);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0.5, 0.5);
		glVertex3f(-1.0f, 1.0f, 1.0f);

		glTexCoord2f(0.5, 0.5);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glTexCoord2f(0.5, 0.5);
		glVertex3f(1.0f, -1.0f, 1.0f);

		glEnd();

		break;
	default:

		break;
	}

	SwapBuffers(ghdc);
}

void uninitialize(void)
{
	if (gbTextureSmily)
	{
		glDeleteTextures(1, &gbTextureSmily);
		gbTextureSmily = 0;
	}

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
}