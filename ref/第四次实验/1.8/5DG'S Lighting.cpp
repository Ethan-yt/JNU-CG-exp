#define WM_TOGGLEFULLSCREEN (WM_USER+1)									// ����ȫ��/�����л���Ϣ
#define TIMER1		101													// ����ʱ�Ӵ���,����Ϊ101��
#define TITLE		"5DG'S Lighting"									// ���崰�ڱ���
#define CLASSNAME	"5DG_OPENGL"										// ���崰������
#define	WIDTH		640													// ���崰�ڿ��
#define HEIGHT		480													// ���崰�ڸ߶�
#define BPP			16													// ����ÿ���ص�λ��

// �ڴ˴����ó���Ҫ���ͷ�ļ�:
#include <windows.h>													// Windows��ͷ�ļ�
#include <gl\gl.h>														// OpenGL32���ͷ�ļ�
#include <gl\glu.h>														// Glu32���ͷ�ļ�

// �ڴ˴��������Ҫ��Ŀ⵽��������:
#pragma comment(lib, "opengl32.lib")									// ����ʱ����OpenGL32.lib
#pragma comment(lib, "glu32.lib")										// ����ʱ����glu32.lib

// �ڴ˴������봰����صĽṹ��:
typedef struct {														// ���崦����̵Ľṹ��
	BOOL keyDown [256];													// �洢���̰���״̬������
} Keys;

typedef struct {														// ����洢Ӧ�ó���ʵ���Ľṹ��
	HINSTANCE		hInstance;											// Ӧ�ó���ʵ��
	const char*		className;											// Ӧ�ó�������
} Application;

typedef struct {														// �����ʼ������������Ϣ
	Application*	application;										// ������Ӧ�ó���
	char*			title;												// ���ڱ���
	int				width;												// ���ڿ��
	int				height;												// ���ڸ߶�
	int				bitsPerPixel;										// ÿ���ص�λ��
	BOOL			isFullScreen;										// �Ƿ�ȫ��
} GL_WindowInit;

typedef struct {														// ���崰�ڽṹ��
	Keys*			keys;												// ����
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

// ����λ������Ļ���Ϸ��Ĺ�Դ
GLfloat light_position0[] = {1.0f, 1.0f, 1.0f, 0.0f};					// ���ù�Դ��λ��
GLfloat light_ambient0[]  = {0.7f, 0.7f, 0.7f, 1.0f};					// ���廷�������ɫ
GLfloat light_diffuse0[]  = {1.0f, 0.0f, 1.0f, 1.0f};				    // ��������������ɫ
GLfloat light_specular0[] = {1.0f, 1.0f, 1.0f, 1.0f};					// ���徵�淴������ɫ 
// ����λ������Ļ���·��Ĺ�Դ
GLfloat light_position1[] = {-1.0f, -1.0f, 1.0f, 0.0f};					// ���ù�Դ��λ��
GLfloat light_ambient1[]  = {0.7f, 0.7f, 0.7f, 1.0f};          			// ���廷�������ɫ
GLfloat light_diffuse1[]  = {1.0f, 0.0f, 1.0f, 1.0f};					// ��������������ɫ
GLfloat light_specular1[] = {1.0f, 1.0f, 1.0f, 1.0f};					// ���徵�淴������ɫ
 
BOOL lighting = FALSE;													// ���ƹ��յĿ���/�ر� 
BOOL light = FALSE;														// �����л�������Դ
BOOL pKeyL;
BOOL pKeyC;

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

	// ��һ����Դ  
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);					// ������Դλ��
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);					// ����������
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);					// �����������
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);					// �������淴���
	// �ڶ�����Դ
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);					// ������Դλ��
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);					// ����������
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);					// �����������
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);					// �������淴���

	return TRUE;														// ��ʼ���ɹ�����TRUE
}

//���Ƴ�����width��height��depth�ֱ�Ϊ������ĳ����ߺ����
void DrawCube(GLfloat width, GLfloat height, GLfloat depth)					
{
	GLfloat x = width/2, y = height/2, z = depth/2;
	glBegin(GL_QUADS);													// ���Ƴ�����ĸ���
		glNormal3f(0.0f, 0.0f, 1.0f);									// ���߷���Z������
		glVertex3f(-x,-y, z);											// ǰ���ı��ε����¶��㣨���ӣ�
		glVertex3f( x,-y, z);											// ǰ���ı��ε����¶���
		glVertex3f( x, y, z);											// ǰ���ı��ε����϶���
		glVertex3f(-x, y, z);											// ǰ���ı��ε����϶���
		glNormal3f(0.0f, 0.0f, -1.0f);									// ���߷���Z�Ḻ��
		glVertex3f( x,-y, -z);											// �����ı��ε����¶��㣨���ӣ�
		glVertex3f(-x,-y, -z);											// �����ı��ε����¶���
		glVertex3f(-x, y, -z);											// �����ı��ε����϶���
		glVertex3f( x, y, -z);											// �����ı��ε����϶���
		glNormal3f(1.0f, 0.0f, 0.0f);									// ���߷���X������
		glVertex3f( x,-y,  z);											// �Ҳ����ı��ε����¶��㣨���ӣ�
		glVertex3f( x,-y, -z);											// �Ҳ����ı��ε����¶���
		glVertex3f( x, y, -z);											// �Ҳ����ı��ε����϶���
		glVertex3f( x, y,  z);											// �Ҳ����ı��ε����϶���	
		glNormal3f(-1.0f,0.0f, 0.0f);									// ���߷���X�Ḻ��
		glVertex3f(-x,-y, -z);											// ������ı��ε����¶��㣨���ӣ�
		glVertex3f(-x,-y,  z);											// ������ı��ε����¶���
		glVertex3f(-x, y,  z);											// ������ı��ε����϶���
		glVertex3f(-x, y, -z);											// ���߷���Y������
		glNormal3f(0.0f, 1.0f, 0.0f);                 					// ������ı��ε����϶���
		glVertex3f(-x, y,  z);											// �����ı��ε����¶��㣨���ӣ�
		glVertex3f( x, y,  z);											// �����ı��ε����¶���
		glVertex3f( x, y, -z);											// �����ı��ε����϶���
		glVertex3f(-x, y, -z);											// �����ı��ε����϶���
		glNormal3f(0.0f, -1.0f, 0.0f);									// ���߷���Y�Ḻ��
		glVertex3f(-x, -y, -z);											// �����ı��ε����¶��㣨���ӣ�
		glVertex3f( x, -y, -z);											// �����ı��ε����¶���
		glVertex3f( x, -y,  z);											// �����ı��ε����϶���
		glVertex3f(-x, -y,  z);											// �����ı��ε����϶���
	glEnd();
}

void DrawSceneGL(void)													// ���Ƴ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// �����������Ȼ���
	glLoadIdentity();													// ���õ�ǰ����
	//�ڴ˴���Ӵ�����л���:
	if (lighting)														// ���û�й�Դ
	{
		glEnable(GL_LIGHTING);											// ���ù�Դ
	}
	else
	{
		glDisable(GL_LIGHTING);											// ���ù�Դ
	}

	if (light)															// ���lightΪTRUE
	{
		glEnable(GL_LIGHT0);											// ���ù�ԴLIGHT0
		glDisable(GL_LIGHT1);											// ���ù�ԴLIGHT1
	} 
	else
	{
		glDisable(GL_LIGHT0);											// ���ù�ԴLIGHT0
		glEnable(GL_LIGHT1);											// ���ù�ԴLIGHT1
	}

	glTranslatef(0.0f, 0.0f, -7.0f);                  					// ����Ļ����7.0f
	glColor3f(1.0f, 0.0f, 1.0f); 										// ���õ�ǰ��ɫ
	glRotatef(90.0f, 1.0f, 0.0f, 1.0f);									// ��XOZƽ����������޵Ľ�ƽ������ת90��
	DrawCube(1.5f, 1.5f, 1.5f);											// ����һ��������

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

	if (OGL_keys->keyDown['L'] == TRUE && !pKeyL)
	{
		pKeyL = TRUE;													// pKeyL��ΪTRUE
		lighting = !lighting;											// �л���Դ��TURE/FALSE
	}
	if (OGL_keys->keyDown['L'] == FALSE)								// L���Ƿ��ɿ�
	{
		pKeyL = FALSE;													// pKeyL��ΪFALSE
	}

	if (OGL_keys->keyDown['C'] == TRUE && !pKeyC)
	{
		pKeyC = TRUE;													// pKeyC��ΪTRUE
		light = !light;													// �л�������Դ
	}
	if (OGL_keys->keyDown['C'] == FALSE)								// C���Ƿ��ɿ�
	{
		pKeyC = FALSE;													// pKeyC��ΪFALSE
	}

}

void Deinitialize(void)													// �ڴ˴����˳�ǰɨβ����
{
	KillTimer(OGL_window->hWnd, TIMER1);								// �ͷ�ʱ��
}