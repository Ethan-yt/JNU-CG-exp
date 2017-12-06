#define WM_TOGGLEFULLSCREEN (WM_USER + 1)								// ����ȫ��/�����л���Ϣ
#define TIMER1		101													// ����ʱ�Ӵ���,����Ϊ101��
#define TITLE		"5DG'S 3D Shadow!"									// ���崰�ڱ���
#define CLASSNAME	"5DG_OPENGL"										// ���崰������
#define	WIDTH		640													// ���崰�ڿ��
#define HEIGHT		480													// ���崰�ڸ߶�
#define BPP			16													// ����ÿ���ص�λ��

#define MAXTEXTURE	4					 								// ��������������Ŀ

// �ڴ˴����ó���Ҫ���ͷ�ļ�:
#include <windows.h>													// Windows��ͷ�ļ�
#include <gl\gl.h>														// OpenGL32���ͷ�ļ�
#include <gl\glu.h>														// Glu32���ͷ�ļ�
#include <gl\glaux.h>													// Glaux���ͷ�ļ�
#include <stdio.h>														// ��׼����/������ͷ�ļ�

// �ڴ˴��������Ҫ��Ŀ⵽��������:
#pragma comment(lib, "opengl32.lib")									// ����ʱ����OpenGL32.lib
#pragma comment(lib, "glu32.lib")										// ����ʱ����glu32.lib
#pragma comment(lib, "glaux.lib")										// ����ʱ����glaux.lib

// �ڴ˴������봰����صĽṹ��:
typedef struct {														// ���崦����̵Ľṹ��
	BOOL keyDown [256];													// �洢���̰���״̬������
} Keys;

typedef struct {														// ����洢Ӧ�ó���ʵ���Ľṹ��
	HINSTANCE		hInstance;											// Ӧ�ó���ʵ��
	const char*		className;											// Ӧ�ó�������
} Application;

typedef struct {														// �����ʼ����������Ϣ
	Application*	application;										// ������Ӧ�ó���
	char*			title;												// ���ڱ���
	int				width;												// ���ڿ��
	int				height;												// ���ڸ߶�
	int				bitsPerPixel;										// ÿ���ص�λ��
	BOOL			isFullScreen;										// �Ƿ�ȫ��
} GL_WindowInit;

typedef struct {														// ���崰�ڽṹ��
	Keys*			keys;												// ���ڵļ���
	HWND			hWnd;												// ���ھ��
	HDC				hDC;												// �豸������
	HGLRC			hRC;												// ����������
	GL_WindowInit	init;												// �ṩWindow��ʼ����Ϣ�Ľṹ��
} GL_Window;

// �˴���ģ���а����ĺ�����ǰ������:
BOOL Initialize(GL_Window* window, Keys* keys);							// ���������ǰ�ĳ�ʼ��ֵ
void DrawSceneGL(void);													// ��������ɳ����Ļ���
void Update(void);														// �ڴ˴����¶���Ϣ�Ķ���
void Deinitialize(void);												// �ڴ˴����˳�ǰɨβ����

// �ڴ˴�����ȫ�ֱ���:
GL_Window*	OGL_window;													// �洢������Ϣ
Keys*		OGL_keys;													// �洢������Ϣ
// �ڴ˴������û�����:
GLUquadricObj	*quadric;												// ��Ŷ������������
GLfloat shadow_matrix[4][4];											// �����Ӱ����
const GLfloat P[4] = {0.0f, 1.0f, 0.0f, 2.0f};							// ��Ӱƽ������

GLfloat planePos[]  = {0.0f,-1.2f, 0.0f};								// ֱ���ɻ���λ��

GLfloat lightPos[] = {0.0f, 4.0f,-3.0f, 0.0f};							// ������λ��
GLfloat lightAmb[] = {0.8f, 0.8f, 0.8f, 1.0f};							// ���廷�������ɫ
GLfloat lightDif[] = {1.0f, 0.7f, 0.8f, 1.0f};							// ��������������ɫ
GLfloat lightSpc[] = {0.8f, 1.0f, 1.0f, 1.0f};							// ���徵�淴������ɫ

GLfloat matAmb[] = {0.4f, 0.4f, 0.4f, 1.0f};							// ���ʵĻ�������ɫ
GLfloat matDif[] = {1.0f, 1.0f, 1.0f, 1.0f};							// ���ʵ����������ɫ
GLfloat matSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};							// ���ʵľ��淴�����ɫ
GLfloat matShn[] = {0.0f};								    			// ���ʵľ���ָ��

GLuint	texture[MAXTEXTURE];											// ��������
BOOL	pKey_space;														// �жϿո���Ƿ���
GLfloat	angle_X1;														// ����������X��ת��
GLfloat	angle_Y1;														// ����������Y��ת��
GLfloat	angle_X2;														// ����������X��ת��
GLfloat	angle_Y2;														// ����������Y��ת��
GLfloat	rot_X;															// ������X��ת�����ٶ�
GLfloat	rot_Y;															// ������Y��ת�����ٶ�
GLfloat	rot;															// ֱ���ɻ���������ת���ٶ�
GLfloat	z=-8.0f;														// ����ƽ������
GLint	change;															// ���Ƶ�ǰ�û��ƺ�������

void ResizeWindowGL(int width, int height)								// �������ô��ڴ�С
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));				// ���õ�ǰ�ӿڴ�С
	glMatrixMode(GL_PROJECTION);										// �л���ͶӰ����ģʽ
	glLoadIdentity();													// ����ͶӰ����
	gluPerspective(45, (float)width/(float)height, 0.1, 100);			// ����͸��ͶӰ
	glMatrixMode(GL_MODELVIEW);											// �л���ģ����ͼ����
	glLoadIdentity();													// ����ģ����ͼ����
}

BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel)	// �޸���Ļ�ֱ���
{
	DEVMODE dmScreenSettings;											// �豸����ģʽ 
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));						// ���
	dmScreenSettings.dmSize			= sizeof(DEVMODE);					// Devmode�ṹ�Ĵ�С
	dmScreenSettings.dmPelsWidth	= width;							// ����Ϊ��Ļ���
	dmScreenSettings.dmPelsHeight	= height;							// ����Ϊ��Ļ�߶�
	dmScreenSettings.dmBitsPerPel	= bitsPerPixel;						// ��Ϊָ��λ��;
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬��
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// �ֱ����޸�ʧ��,���� False
	}
	return TRUE;														// �ֱ����޸ĳɹ�,���� True;
}

BOOL CreateWindowGL(GL_Window* window)									// ����OpenGL����
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// ���ô�����ʽ
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// ���ô�����չ��ʽ

	PIXELFORMATDESCRIPTOR pfd =											// ���ظ�ʽ������(pfd)������
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// ���صĳߴ�
		1,																// �汾��
		PFD_DRAW_TO_WINDOW |											// pfd����֧�ִ��ڻ���
		PFD_SUPPORT_OPENGL |											// pfd����֧��OpenGL
		PFD_DOUBLEBUFFER,												// pfd����֧��˫����
		PFD_TYPE_RGBA,													// ���ظ�ʽΪRGBA
		window->init.bitsPerPixel,										// ������ɫ���
		0, 0, 0, 0, 0, 0,												// ������ɫλ��
		0,																// ��Alpha����
		0,																// ����ƫ��λ
		0,																// ���ۻ�����
		0, 0, 0, 0,														// �����ۻ�����λ 
		16,																// ��Ȼ���Ϊ16λ 
		0,																// ��ģ�建��
		0,																// �޸�������
		PFD_MAIN_PLANE,													// ��Ҫ���Ʋ�
		0,																// ����λ
		0, 0, 0															// ���Բ�����
	};

	RECT windowRect = {0, 0, window->init.width, window->init.height};	// ���崰�ڴ�С

	GLuint PixelFormat;													// �������ظ�ʽ

	if (window->init.isFullScreen == TRUE)								// �л�ȫ��
	{
		if (ChangeScreenResolution(window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			// ȫ���л�ʧ��
			MessageBox(HWND_DESKTOP, "�޷��л���ȫ��ģʽ,�ڴ���ģʽ������.\nMode Switch Failed,Running In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;							// ���� isFullscreen ΪFalse
		}
		else															// ȫ���л��ɹ�
		{
			ShowCursor (FALSE);											// �������
			windowStyle = WS_POPUP;										// ���ô�����ʽ
			windowExtendedStyle |= WS_EX_TOPMOST;						// ���ô�����չ��ʽ
		}
	}
	else															
	{
		// �������ڴ�С,�������ڱ߽�
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// ��ʼ���� OpenGL ����
	window->hWnd = CreateWindowEx(windowExtendedStyle,					// ������չ��ʽ
									window->init.application->className,// Ӧ�ó�������
									window->init.title,					// ���ڱ���
									windowStyle,						// ������ʽ
									0, 0,								// ���ڵ� X,Y ����λ��
									windowRect.right - windowRect.left,	// ���ڿ��
									windowRect.bottom - windowRect.top,	// ���ڸ߶�
									HWND_DESKTOP,						// ������Ϊ����
									0,									// �޲˵�
									window->init.application->hInstance,// ����Ӧ�ó���ʵ�� 
									window);

	if (window->hWnd == 0)												// �����Ƿ�ɹ�����
	{
		return FALSE;													// ��ʧ��,�򷵻�FALSE
	}

	window->hDC = GetDC(window->hWnd);									// ȡ�õ�ǰ���ڵ��豸������
	if (window->hDC == 0)												// ��δ�õ��豸������
	{
		DestroyWindow(window->hWnd);									// ���ٸô���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	PixelFormat = ChoosePixelFormat(window->hDC, &pfd);					// ѡ����ݵ����ظ�ʽ
	if (PixelFormat == 0)												// ��ѡ��ʧ��
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	if (SetPixelFormat(window->hDC, PixelFormat, &pfd) == FALSE)		// �������ظ�ʽ���ж��Ƿ�ʧ��
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	window->hRC = wglCreateContext(window->hDC);						// ȡ�û���������
	if (window->hRC == 0)												// ��δ�õ�����������
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������ 
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	if (wglMakeCurrent(window->hDC, window->hRC) == FALSE)				// ���û����������ж��Ƿ�ʧ��
	{
		wglDeleteContext(window->hRC);									// ɾ������������
		window->hRC = 0;												// ����������������
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	ShowWindow(window->hWnd, SW_NORMAL);								// ��ʾ����
	ResizeWindowGL(window->init.width, window->init.height);			// ���贰��
	ZeroMemory(window->keys, sizeof (Keys));							// ��ռ��̻�����

	return TRUE;														// ���ڴ����ɹ�
}

BOOL DestroyWindowGL(GL_Window* window)									// ���ٴ��ڲ��ͷų������õ���Դ
{
	if (window->hWnd != 0)												// �����ͷ��о��?
	{	
		if (window->hDC != 0)											// �����Ƿ��еõ�����������?
		{
			wglMakeCurrent(window->hDC, 0);								// ����ǰ������ָ����Ϊ0
			if (window->hRC != 0)										// �ô����Ƿ��л���������
			{
				wglDeleteContext(window->hRC);							// �ͷŻ���������
				window->hRC = 0;										// ����������������
			}
			ReleaseDC(window->hWnd, window->hDC);						// �ͷ��豸������
			window->hDC = 0;											// ���豸����������
		}
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// �����ھ������
	}

	if (window->init.isFullScreen)										// ��������ȫ��ģʽ��
	{
		ChangeDisplaySettings(NULL ,0);									// �л�Ϊ����ֱ���
		ShowCursor(TRUE);												// ��ʾ���
	}	
	return TRUE;														// ����TRUE
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)// ������Ϣ����
{

	GL_Window* window = (GL_Window*)(GetWindowLong(hWnd, GWL_USERDATA));// ȡ�ô�����Ϣ

	switch (uMsg)														// ȡ��Window����Ϣ
	{
	case WM_SYSCOMMAND:													// ��ȡϵͳ����
		{
			switch (wParam)												// ����ϵͳ����
			{
			case SC_SCREENSAVE:											// Ҫ��������?
			case SC_MONITORPOWER:										// ��ʾ������ڵ�ģʽ?
				return 0;												// ��ǰ����0,��ֹϵͳ����ִ��
			}
			break;														// �˳�
		}

	case WM_CREATE:														// ��������
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// ���洰�ڽṹָ��
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)(window));			// �ı䴰������
			return 0;													// ����
		}

	case WM_TIMER:														// TIMER�¼�
		{
			InvalidateRect(window->hWnd, NULL, FALSE);					// ʹ����ʧЧ
			break;
		}	

	case WM_PAINT:														// PAINT��Ϣ����,�ڴ˴���ͼ
		{	
			DrawSceneGL();												// �ڴ˴���ͼ
			SwapBuffers(window->hDC);									// ��������
			ValidateRect(window->hWnd, NULL);							// ʹ������Ч
			break;
		}	

	case WM_CLOSE:														// �رմ���
		{
			PostMessage(window->hWnd, WM_QUIT, 0, 0);					// ��������
			return 0;
		}

	case WM_SIZE:														// ���ڴ�С�仯
		{
			ResizeWindowGL(LOWORD(lParam), HIWORD(lParam));				// �޸Ĵ��ڴ�СΪ Width = LoWord, Height = HiWord
			return 0;													// ����
		}

	case WM_KEYDOWN:													// ��������ʱ���¼��̻���
		if ((wParam >= 0) && (wParam <= 255))							// �����Ƿ�Ϸ�?
		{
			window->keys->keyDown[wParam] = TRUE;						// ����Ӧ��Ϊ True
			return 0;													// ����
		}
		break;															// �˳�

	case WM_KEYUP:														// �����ɿ�ʱ���¼��̻���
		if ((wParam >= 0) && (wParam <= 255))							// �����Ƿ�Ϸ�?
		{
			window->keys->keyDown[wParam] = FALSE;						// ����Ӧ��ΪΪ FALSE
			return 0;													// ����
		}
		break;															// �˳�

	case WM_TOGGLEFULLSCREEN:											// ����ȫ��ģʽ
		{
			Deinitialize();												// ��ɨβ����
			DestroyWindowGL(window);									// ���ٴ���
			window->init.isFullScreen = !window->init.isFullScreen;		// �ı䴰��ģʽ
			CreateWindowGL(window);										// ���´�������
			Initialize(window, window->keys);							// ��ʼ��OpenGL���Ƴ���
			break;														// �˳�
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);					// �������򲻴������Ϣ���� DefWindowProc 
}

BOOL RegisterWindowClass(Application* application)						// Ϊ��Ӧ�ó���ע��һ����
{	
	WNDCLASSEX windowClass;												// ������
	ZeroMemory (&windowClass, sizeof(WNDCLASSEX));						// ����ڴ�
	windowClass.cbSize		= sizeof(WNDCLASSEX);						// ������Ĵ�С
	windowClass.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �ڴ����ƶ�,�ı��С��ʱ���ػ� 
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				// ��WindowProc����������Ϣ
	windowClass.hInstance		= application->hInstance;				// ����ʵ��
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// �౳���Ļ�ˢ��ɫ
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// �������ָ��
	windowClass.lpszClassName	= application->className;				// ����Ӧ�ó��������
	if (RegisterClassEx(&windowClass) == 0)								// ע����ʧ��?
	{
		MessageBox(HWND_DESKTOP, "Ӧ�ó�����ע��ʧ��!\nRegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// ����False (ע��ʧ��)
	}
	return TRUE;														// ����True (ע��ɹ�)
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application	application;											// Ӧ�ó���
	GL_Window	window;													// ����
	Keys		keys;													// ���̰���
	MSG			msg;													// Window��Ϣ
	BOOL		bRet;													// ������Ϣ����Ƿ�ɹ�

	// ��Ӧ�ó���ֵ
	application.className = CLASSNAME;									// ����������
	application.hInstance = hInstance;									// �������

	// ���������Ϣ����
	ZeroMemory(&window, sizeof(GL_Window));								// ���㴰�ڱ������ڴ�ռ�
	window.keys					= &keys;								// ���ð���
	window.init.application		= &application;							// ���ô��ڳ���
	window.init.title			= TITLE;								// ���ñ���
	window.init.width			= WIDTH;								// ���ô��ڿ��
	window.init.height			= HEIGHT;								// ���ô��ڸ߶�
	window.init.bitsPerPixel	= BPP;									// ����ÿ���ص�λ��
	window.init.isFullScreen	= FALSE;								// ���ó�ʼ�����Ƿ�ȫ����(FALSE)
	ZeroMemory(&keys, sizeof(Keys));									// ���̻�������

	if (RegisterWindowClass(&application) == FALSE)						// ע�����Ƿ�ʧ��
	{
		MessageBox(HWND_DESKTOP, "������ע��ʧ��!\nError Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// ��������
	}

	if (CreateWindowGL(&window) == TRUE)								// ���������Ƿ�ɹ�
	{	
		if (Initialize(&window, &keys) == FALSE)						// ��ʼ��OpenGL���Ƴ���
		{
			PostMessage(window.hWnd, WM_QUIT, 0, 0);					// �׳���ϢWM_QUIT
		}
	}
	else
	{
		return 0;														// �˳�����
	}

	while ((bRet = GetMessage(&msg, NULL, 0, 0) != 0))					// ѭ��ֱ���յ���ϢΪWM_QUIT
	{
		if (bRet != -1)
		{
			Update();													// ���´�����Ϣ�¼�
			TranslateMessage(&msg);										// ������Ϣ
			DispatchMessage(&msg);										// ������Ϣ
		}
		else
		{
			break;														// �˳�ѭ��
		}
	}
	// �˳���Ϣѭ��,׼���˳�����
	Deinitialize();														// ���˳�ǰ��ɨβ����
	DestroyWindowGL(&window);											// ���ٴ���	
	UnregisterClass(application.className, application.hInstance);		// ע��������
	return 0;															// �˳�����
}

AUX_RGBImageRec *LoadImage(char *Filename)								// ����һ��ͼƬ
{
	FILE *File = NULL;													// �ļ����

	if (!Filename)														// ȷ���ļ����Ѿ��ṩ
	{
		return NULL;													// ���û���򷵻�NULL
	}

	File = fopen(Filename,"r");											// ����ļ��Ƿ����

	if (File)															// �ļ�������
	{
		fclose(File);													// �ر�File�ļ����
		return auxDIBImageLoad(Filename);								// ����ͼƬ��������ָ��
	}

	return NULL;														// ������ش����򷵻�NULL
}

BOOL LoadTextureGL(void)												// ����ͼƬ��ת��Ϊ����
{
	BOOL State = FALSE;													// ״ָ̬ʾ

	AUX_RGBImageRec *TextureImage[MAXTEXTURE];							// Ϊ�����ٴ洢�ռ�

	memset(TextureImage, 0, sizeof(void *)*MAXTEXTURE);					// ���ͼ���¼��ȷ��������Ϊ�ղ�ʹָ��ָ��NULL
	// ����ͼƬ������Ƿ���� �����ͼƬ�������򷵻�
	if ((TextureImage[0] = LoadImage("Data/ground1.bmp"))
		&& (TextureImage[1] = LoadImage("Data/ground2.bmp"))
		&& (TextureImage[2] = LoadImage("Data/wood.bmp"))
		&& (TextureImage[3] = LoadImage("Data/plane.bmp")))
	{   
		State=TRUE;														// ����״̬����ΪTRUE
		glGenTextures(MAXTEXTURE, &texture[0]);							// ����Ψһ��������������ʶ����,������texture��

		// ��ͼƬ���ݲ�������
		for (int loop=0; loop<MAXTEXTURE; loop++)						// ѭ������MAXTEXTURE������
		{
			// ʹ������λͼ�������ɵĵ�������
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			// �������� 
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// �����˲�
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// �����˲�
		}
	}
	for (int loop=0; loop<MAXTEXTURE; loop++)
	{
		if (TextureImage[loop])											// �����Ƿ����	
		{
			if (TextureImage[loop]->data)								// ����ͼ���Ƿ����
			{
				free(TextureImage[loop]->data);							// �ͷ�����洢�ռ�
			}
			free(TextureImage[loop]);									// �ͷ�ͼ��ṹ
		}
	}
	return State;														// ����State
}

// ������Ӱ����
void generate_shadow_matrix(GLfloat matrix[4][4], const GLfloat ground[4], const GLfloat light[4])
{
	GLfloat dot = 0;
	int i, j;
	for(i = 0; i < 4; i++)
		dot += ground[i] * light[i];
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(i == j)
				matrix[i][j] = dot - ground[i] * light[j];
			else
				matrix[i][j] = - ground[i] * light[j];
}

// ����ֱ���ɻ�
void DrawPlane(void)
{
	// ����ֱ���ɻ���ͷ
	glPushMatrix();
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -1.5f);
		gluCylinder(quadric, 1.0f, 0.6f, 0.8f, 32, 32);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(1.9f, -2.3f, 0.0f);
		gluSphere(quadric, 0.3f, 32, 32);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-7.0f, -1.5f, 0.0f);
		gluSphere(quadric, 0.2f, 32, 32);
	glPopMatrix();

	glTranslatef(0.0f, -1.5f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.01f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.01f, 0.8f, 0.6f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 0.8f, 0.6f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.01f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.01f, 0.8f,-0.6f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 0.8f,-0.6f);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.3f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.0f, 0.8f, 0.6f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 0.8f,-0.6f);
		glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.3f,-1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f,-1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0f, 0.0f, 1.0f);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.3f,-1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f,-1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0f, 0.0f, -1.0f);
	    glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.3f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.3f,-1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.3f,0.0f,   1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.3f,-1.0f,  1.0f);
	glEnd();
    glPushMatrix();
	    glTranslatef(1.0f, -0.8f, 0.0f);
	    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	    gluCylinder(quadric, 1.0f, 0.2f, 1.1f, 32, 32);
    glPopMatrix();
    glPushMatrix();
	    glTranslatef(1.0f, -0.8f, 0.0f); 
	    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		gluCylinder(quadric, 1.0f, 1.0f, 3.0f, 32, 32);
	glPopMatrix();
	glPushMatrix();	
	    glTranslatef(-2.0f, -0.8f, 0.0f);
	    gluSphere(quadric, 1.0f, 32, 32);
	glPopMatrix();
	// ֱ������ͷ�������

	// ����ֱ������β
	glPushMatrix();
        glTranslatef(-2.8f, -0.5f, 0.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-7.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.3f, 0.2f, 4.3f, 32, 32);
    glPopMatrix();
	// ���ƻ�β���𲿷�
	glPushMatrix();
		glTranslatef(-2.8f, -0.5f, 0.0f);
     	glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 1.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.0f, 0.5f, 0.2f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-3.4f, 0.5f, 0.2f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-4.4f, 1.5f, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 1.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.0f, 0.5f,-0.2f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-3.4f, 0.5f,-0.2f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-4.4f, 1.5f, 0.0f);
		glEnd();
        glBegin(GL_TRIANGLES);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.0f, 0.5f, 0.2f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 1.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.0f, 0.5f,-0.2f);
		glEnd();
    glPopMatrix();
	// ֱ������β�������

	// ���Ʒɻ�������
	// ����������֧��
	glPushMatrix();
		glTranslatef(-0.5, 0.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.2f, 0.2f,1.5f, 32, 32);
	glPopMatrix();
	// ����������ҶƬ
	glPushMatrix();
		glTranslatef(-0.5f, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(angle_X2, 0.0f, 1.0f, 0.0f);//��ת�ɻ�������ҶƬ 
		glPushMatrix();
			glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
			glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, 1.5f, 0.3f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.0f, 1.5f, 0.3f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f, 1.5f,-0.3f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, 1.5f,-0.3f);
			glEnd();
		glPopMatrix();

		glPushMatrix();
			glRotatef(150.0f, 0.0f, 1.0f, 0.0f);
			glBegin(GL_QUADS);
				glNormal3f(0.0f,1.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, 1.5f, 0.3f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.0f, 1.5f, 0.3f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f, 1.5f,-0.3f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, 1.5f,-0.3f);
			glEnd();
		glPopMatrix();

		glPushMatrix();
			glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, 1.5f, 0.3f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.0f, 1.5f, 0.3f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f, 1.5f,-0.3f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, 1.5f,-0.3f);
			glEnd();
		glPopMatrix();
	glPopMatrix();
	// ֱ�����������������

	// ����ֱ��������
	// ���Ƶ���4��֧��
	glPushMatrix();
		glTranslatef(0.8f, 0.0f, -0.4f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.1f, 0.1f, 2.3f, 32, 32);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.8f, 0.0f, 0.4f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.1f, 0.1f, 2.3f, 32, 32);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-1.0f, 0.0f, -0.4f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.1f, 0.1f, 2.3f, 32, 32);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-1.0f, 0.0f, 0.4f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.1f, 0.1f, 2.3f, 32, 32);
	glPopMatrix();
	// ���Ƶ����ĵװ�
	glTranslatef(0.0f, -3.75f, 0.4f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 1.5f, 0.15f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.5f, 0.15f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.5f,-0.15f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 1.5f,-0.15f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 1.5f,-0.75f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.5f,-0.75f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.5f,-0.95f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 1.5f,-0.95f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, 1.7f, 0.15f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.0f, 1.5f, 0.15f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.0f, 1.5f,-0.15f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f, 1.7f,-0.15f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.5f, 1.7f, 0.15f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.5f, 1.7f,-0.15f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.5f,-0.15f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.5f, 0.15f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, 1.7f,-0.75f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.0f, 1.5f,-0.75f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.0f, 1.5f,-0.95f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f, 1.7f,-0.95f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.5f,  1.7f,-0.75f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.5f,  1.7f,-0.95f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f,  1.5f,-0.95f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f,  1.5f,-0.75f);
	glEnd();
	// ֱ���������������
}

//���Ƴ�����width��height��depth�ֱ�Ϊ������ĳ����ߺ����
void DrawCube(GLfloat width, GLfloat height, GLfloat depth)
{
	GLfloat x = width/2, y = height/2, z = depth/2;
	glBegin(GL_QUADS);													// ���Ƴ�����ĸ���
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y, z);					// ǰ���ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y, z);					// ǰ���ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y, z);					// ǰ���ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y, z);					// ǰ���ı��ε����϶���
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( x,-y,-z);					// �����ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,-y,-z);					// �����ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, y,-z);					// �����ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f( x, y,-z);					// �����ı��ε����϶���
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( x,-y, z);					// �Ҳ����ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y,-z);					// �Ҳ����ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y,-z);					// �Ҳ����ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f( x, y, z);					// �Ҳ����ı��ε����϶���
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y,-z);					// ������ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,-y, z);					// ������ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, y, z);					// ������ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y,-z);					// ������ı��ε����϶���
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, z);					// �����ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x, y, z);					// �����ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y,-z);					// �����ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y,-z);					// �����ı��ε����϶���
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y,-z);					// �����ı��ε����¶��㣨���ӣ�
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y,-z);					// �����ı��ε����¶���
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x,-y, z);					// �����ı��ε����϶���
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x,-y, z);					// �����ı��ε����϶���
	glEnd();
}

BOOL Initialize(GL_Window* window, Keys* keys)							// �û����Ƴ�����صĺ����ڴ˴�����	
{
	// ����ȫ�ֱ���
	OGL_window	= window;
	OGL_keys	= keys;

	// ����ʱ��
	SetTimer(window->hWnd, TIMER1, 33, NULL);

	// �ڴ˴���ʼ�����Ƴ���
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);								// ����Ϊ��ɫ
	glClearDepth(1.0f);													// ������Ȼ���
	glDepthFunc(GL_LEQUAL);												// ѡ����Ȳ��Է�ʽ
	glEnable(GL_DEPTH_TEST);											// ������Ȳ���
	glShadeModel(GL_SMOOTH);											// �����������ƽ����ʽ
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					// �ϸ��͸�Ӽ���	
	
	if (!LoadTextureGL())												// ����LoadTextureGL()����
	{
		return FALSE;													// ���������ش����򷵻�FALSE
	}
	glEnable(GL_TEXTURE_2D);											// ����2D����ӳ��
	
	quadric = gluNewQuadric();											// ����һ���������ָ��
    gluQuadricTexture(quadric, GLU_TRUE);								// ������������
    gluQuadricDrawStyle(quadric, GLU_FILL);								// �������
	
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);						// ���ƹ�Դ1
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);							// ���ù�Դ1�Ļ�����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);							// ���ù�Դ1���������
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);						// ���ù�Դ1�ľ��淴���
	glEnable(GL_LIGHT1);												// ���ù�Դ1

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);							// ���ò��ʵĻ�����
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);							// ���ò��ʵ��������
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpc);						// ���ò��ʵľ��淴���
	glMaterialfv(GL_FRONT, GL_SHININESS, matShn);						// ���ò��ʵľ���ָ��

	return TRUE;														// ��ʼ���ɹ�����TRUE
}

void DrawSceneGL(void)													// ���Ƴ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// �����ɫ����Ȼ���
	glLoadIdentity();													// ���õ�ǰ����
	//�ڴ˴���Ӵ�����л���:
	glTranslatef(0.0f, 0.0f, z);										// ������Ļz

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);						// ��Դ1��λ��
	glEnable(GL_LIGHTING);												// ���ù���
	glEnable(GL_TEXTURE_2D);											// ����������ͼ

	if (change == 0)													// ��change����0ѡ�񲢰󶨵�һ������
	{
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}
	else																// ��change����1ѡ�񲢰󶨵ڶ�������
	{
		glBindTexture(GL_TEXTURE_2D, texture[1]);
	}
	glBegin(GL_QUADS);													// ������Ӱƽ��	
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 8.0f, -2.0f, -16.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-8.0f, -2.0f, -16.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, -2.0f,  16.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 8.0f, -2.0f,  16.0f);
	glEnd();

	// change����0ʱ����ʮ�ּ�
	if (change == 0)
	{
		glPushMatrix();													// ����ǰ��������ջ��
			glBindTexture(GL_TEXTURE_2D, texture[2]);					// ѡ�񲢰󶨵���������
			glRotatef(angle_X1, 1.0f, 0.0f, 0.0f);						// ��ʮ�ּ���X��ת��
			glRotatef(angle_Y1, 0.0f, 1.0f, 0.0f);						// ��ʮ�ּ���Y��ת��
			// ����ʮ�ּ�
			DrawCube(2.0f, 0.2f, 0.2f);
			DrawCube(0.2f, 2.0f, 0.2f);
			DrawCube(0.2f, 0.2f, 2.0f);
		glPopMatrix();													// �ָ�����

		glPushMatrix();													// ����ǰ��������ջ��
			glDisable(GL_LIGHTING);										// �رչ�Դ
			glDisable(GL_TEXTURE_2D);									// ��������
			glColor3f(0.0f, 0.0f, 0.0f);								// ������Ӱ��ɫ
			glTranslatef(0.0f, 0.01f, 0.0f);
			// ����generate_shadow_matrix()����������Ӱ�����뵱ǰ������ˣ��õ�������Ӱ�ľ���
			generate_shadow_matrix(shadow_matrix, P, lightPos);
			glMatrixMode(GL_MODELVIEW);	
			glMultMatrixf((GLfloat*) shadow_matrix);
			// ����Ӱ����ʵ������һ���˶�
			glRotatef(angle_X1, 1.0f, 0.0f, 0.0f);
			glRotatef(angle_Y1, 0.0f, 1.0f, 0.0f);
			// ������Ӱʮ�ּ�
			DrawCube(2.0f, 0.2f, 0.2f);
			DrawCube(0.2f, 2.0f, 0.2f);
			DrawCube(0.2f, 0.2f, 2.0f);
		glPopMatrix();													// �ָ�����

		angle_X1 += rot_X;
		angle_Y1 += rot_Y;
	}
	// change����1ʱ����ֱ���ɻ�
	if (change == 1)
	{	
		glPushMatrix();													// ����ǰ��������ջ��
			glBindTexture(GL_TEXTURE_2D, texture[3]);					// ѡ�񲢰󶨵���������
			glRotatef(angle_Y2, 0.0f, 1.0f, 0.0f);						// �ı�ֱ���ɻ�ǰ���ķ���
			glTranslatef(planePos[0], planePos[1], planePos[2]);
			glScalef(0.2f, 0.2f, 0.2f);
			DrawPlane();												// ����ֱ���ɻ�
		glPopMatrix();													// �ָ�����

		glPushMatrix();													// ����ǰ��������ջ��
			glDisable(GL_LIGHTING);										// �رչ�Դ
			glDisable(GL_TEXTURE_2D);									// ��������
			glColor3f(0.0f, 0.0f, 0.0f);								// ������Ӱ��ɫ
			glTranslatef(0.0f, 0.01f, 0.0f);
			// ����generate_shadow_matrix()����������Ӱ�����뵱ǰ������ˣ��õ�������Ӱ�ľ���
			generate_shadow_matrix(shadow_matrix, P, lightPos);
			glMatrixMode(GL_MODELVIEW);
			glMultMatrixf((GLfloat*) shadow_matrix);
			// ����Ӱ����ʵ������һ���˶�
			glRotatef(angle_Y2, 0.0f, 1.0f, 0.0f);
			glTranslatef(planePos[0], planePos[1], planePos[2]);
			glScalef(0.2f, 0.2f, 0.2f);
			DrawPlane();												// ����ֱ���ɻ�
		glPopMatrix();													// �ָ�����

		// ��ֱ���ɻ��ѵ����棬����������ͣ��;�������
		if (planePos[1] <= -1.0 && rot != 0)
		{  
			rot -= 0.5f;
			angle_X2 += rot;
		}
		else
		{
			angle_X2 += rot;
		}
	}

	// ����һ��ģ���Դ����ʾ�������ķ���
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 0.0f);
		glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
		gluSphere(quadric, 0.1f, 32, 16);
	glPopMatrix();

	glFlush();															// ˢ��GL�������
}

void Update(void)														// �ڴ˴����¶���Ϣ�Ķ���
{
	if (OGL_keys->keyDown[VK_ESCAPE] == TRUE)							// �ж�ESC���Ƿ���
	{
		PostMessage(OGL_window->hWnd, WM_QUIT, 0, 0);					// ��������
	}

	if (OGL_keys->keyDown[VK_F1] == TRUE)								// �ж�F1�Ƿ���
	{
		PostMessage(OGL_window->hWnd, WM_TOGGLEFULLSCREEN, 0, 0);		// ��ȫ��/����ģʽ���л�
	}
	
	// �ı��Դ��λ��
	if (OGL_keys->keyDown['D'] == TRUE)									// D���Ƿ���
	{
		lightPos[0] += 0.05f;
	}
	if (OGL_keys->keyDown['A'] == TRUE)									// A���Ƿ���
	{
		lightPos[0] -= 0.05f;
	}
	if (OGL_keys->keyDown['W'] == TRUE)									// W���Ƿ���
	{	
		lightPos[1] += 0.05f;
	}
	if (OGL_keys->keyDown['S'] == TRUE)									// S���Ƿ���
	{	
		lightPos[1] -= 0.05f;
	}
	if (OGL_keys->keyDown['Q'] == TRUE)									// Q���Ƿ���
	{
		lightPos[2] += 0.05f;
	}
	if (OGL_keys->keyDown['E'] == TRUE)									// E���Ƿ���
	{
		lightPos[2] -= 0.05f;
	}

	// �ı�ɻ���λ��
	if (OGL_keys->keyDown[VK_RIGHT] == TRUE && planePos[1]>=-1.0f)		// RIGHT���Ƿ���
	{
		planePos[0] += 0.05f;
	}
	if (OGL_keys->keyDown[VK_LEFT] == TRUE && planePos[1]>=-1.0f)		// LEFT���Ƿ���
	{
		planePos[0] -= 0.05f;
	}
	if (OGL_keys->keyDown[VK_UP] == TRUE && rot>=30.0f)					// UP���Ƿ���
	{
		planePos[1] += 0.05f;
	}
	if (OGL_keys->keyDown[VK_DOWN] == TRUE && planePos[1]>-1.2f)		// DOWN���Ƿ���
	{
		planePos[1] -= 0.05f;
	}
	// �������������ٶ�
	if (OGL_keys->keyDown[VK_UP] == TRUE)								// UP���Ƿ���
	{
		rot += 0.5f;
	}
	// ��planePos[1]>=-1.0f����Ըı�ֱ���ɻ�ǰ���ķ���
	if (OGL_keys->keyDown[VK_NUMPAD4] == TRUE && planePos[1]>=-1.0f)	// ���ּ�4�Ƿ���
	{
		angle_Y2 -= 0.5;
	}
	if (OGL_keys->keyDown[VK_NUMPAD6] == TRUE && planePos[1]>=-1.0f)	// ���ּ�6�Ƿ���
	{
		angle_Y2 += 0.5;
	}

	// ����ʮ�ּܵ���ת
	if (OGL_keys->keyDown[VK_UP] == TRUE)								// UP���Ƿ���
	{
		rot_X -= 0.5;
	}
	if (OGL_keys->keyDown[VK_DOWN] == TRUE)								// DOWN���Ƿ���
	{
		rot_X += 0.5;
	}
	if (OGL_keys->keyDown[VK_LEFT] == TRUE)								// LEFT���Ƿ���
	{
		rot_Y -= 0.5;
	}
	if (OGL_keys->keyDown[VK_RIGHT] == TRUE)							// RIGHT���Ƿ���
	{
		rot_Y += 0.5;
	}

	if (OGL_keys->keyDown[VK_SPACE] == TRUE && !pKey_space)				// �ո���Ƿ���
	{
		// ���ո�����£���ı�change��ֵ����ʼ�����и�ֵ
		pKey_space = TRUE;
		z = -8.0f;
		change += 1;
		change = change % 2;
		angle_X1 = 0;
		angle_Y1 = 0;
		angle_X2 = 0;
		angle_Y2 = 0;
		rot = 0;
		rot_X = 0;
		rot_Y = 0;
		planePos[0] = 0.0f;
		planePos[1] = -1.2f;
		planePos[2] = 0.0f;
	}
	if (OGL_keys->keyDown[VK_SPACE] == FALSE)							// �ո���Ƿ�ſ�
	{
		pKey_space = FALSE;
	}

	// ���Ƴ��������
	if (OGL_keys->keyDown[VK_PRIOR] == TRUE)							// PageUp���Ƿ���
	{
		z += 0.5f;
	}
	if (OGL_keys->keyDown[VK_NEXT] == TRUE)								// PageDown���Ƿ���
	{
		z -= 0.5f;
	}
}

void Deinitialize(void)													// �ڴ˴����˳�ǰɨβ����
{
	KillTimer(OGL_window->hWnd, TIMER1);								// �ͷ�ʱ��
}