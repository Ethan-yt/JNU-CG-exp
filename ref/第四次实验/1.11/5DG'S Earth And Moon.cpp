#define WM_TOGGLEFULLSCREEN (WM_USER+1)									// 定义全屏/窗口切换消息
#define TIMER1		101													// 定义时钟代号,设置为101号
#define TITLE		"5DG'S Earth And Moon"								// 定义窗口标题
#define CLASSNAME	"5DG_OPENGL"										// 定义窗口类名
#define	WIDTH		640													// 定义窗口宽度
#define HEIGHT		480													// 定义窗口高度
#define BPP			16													// 定义每象素的位数

#define MAXTEXTURE	4													// 定义最大的纹理数目

// 在此处引用程序要求的头文件:
#include <windows.h>													// Windows的头文件
#include <gl\gl.h>														// OpenGL32库的头文件
#include <gl\glu.h>														// Glu32库的头文件
#include <gl\glaux.h>													// Glaux库的头文件
#include <stdio.h>														// 标准输入/输出库的头文件

// 在此处加入程序要求的库到链接器中:
#pragma comment (lib, "opengl32.lib")									// 链接时查找OpenGL32.lib
#pragma comment (lib, "glu32.lib")										// 链接时查找glu32.lib
#pragma comment (lib, "glaux.lib")										// 链接时查找glaux.lib

// 在此处定义与窗口相关的结构体:
typedef struct {														// 定义处理键盘的结构体
	BOOL keyDown [256];													// 存储键盘按键状态的数组
} Keys;

typedef struct {														// 定义存储应用程序实例的结构体
	HINSTANCE		hInstance;											// 应用程序实例
	const char*		className;											// 应用程序类名
} Application;

typedef struct {														// 定义初始化窗口所需信息
	Application*	application;										// 所属的应用程序
	char*			title;												// 窗口标题
	int				width;												// 窗口宽度
	int				height;												// 窗口高度
	int				bitsPerPixel;										// 每像素的位数
	BOOL			isFullScreen;										// 是否全屏
} GL_WindowInit;

typedef struct {														// 定义窗口结构体
	Keys*			keys;												// 键盘
	HWND			hWnd;												// 窗口句柄
	HDC				hDC;												// 设备描述表
	HGLRC			hRC;												// 绘制描述表
	GL_WindowInit	init;												// 提供Window初始化信息的结构体
} GL_Window;

// 此代码模块中包含的函数的前向声明:
BOOL Initialize(GL_Window* window, Keys* keys);							// 设置你绘制前的初始化值
void DrawSceneGL(void);													// 在这里完成场景的绘制
void Update(void);														// 在此处更新对消息的动作
void Deinitialize(void);												// 在此处做退出前扫尾工作

// 在此处定义全局变量:
GL_Window*	OGL_window;													// 存储窗口信息
Keys*		OGL_keys;													// 存储按键信息
// 在此处定义用户变量:
GLfloat	ep_Angle;														// 地球公转的角度
GLfloat	es_Angle;														// 地球自转的角度
GLfloat	mp_Angle;														// 月亮公转的角度
GLfloat	ms_Angle;														// 月亮自转的角度
GLuint	texture[MAXTEXTURE];											// 纹理数组，保存纹理名字
GLUquadricObj *quadric;													// 建立二次曲面对象
GLfloat	angle_Z;														// 星空旋转角度

GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 0.0f };					// 定义环境光的颜色
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 0.0f };					// 定义漫反射光的颜色
GLfloat LightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };					// 定义光源的位置

GLfloat fogColor[4] = {0.5f, 0.0f, 0.5f, 1.0f};							// 雾的颜色为紫色

void ResizeWindowGL(int width, int height)								// 重新设置窗口大小
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));				// 重置当前视口大小
	glMatrixMode(GL_PROJECTION);										// 切换到投影矩阵模式
	glLoadIdentity();													// 重置投影矩阵
	gluPerspective(45, (float)width/(float)height, 0.1, 100);			// 设置透视投影
	glMatrixMode(GL_MODELVIEW);											// 切换到模型视图矩阵
	glLoadIdentity();													// 重置模型视图矩阵
}

BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel)	// 修改屏幕分辨率
{
	DEVMODE dmScreenSettings;											// 设备设置模式 
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));						// 清空
	dmScreenSettings.dmSize			= sizeof(DEVMODE);					// Devmode结构的大小
	dmScreenSettings.dmPelsWidth	= width;							// 设置为屏幕宽度
	dmScreenSettings.dmPelsHeight	= height;							// 设置为屏幕高度
	dmScreenSettings.dmBitsPerPel	= bitsPerPixel;						// 设为指定位长;
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	// 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态栏
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// 分辨率修改失败,返回 False
	}
	return TRUE;														// 分辨率修改成功,返回 True;
}

BOOL CreateWindowGL(GL_Window* window)									// 建立OpenGL窗口
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// 设置窗口样式
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// 设置窗口扩展样式

	PIXELFORMATDESCRIPTOR pfd =											// 像素格式描述符(pfd)的设置
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// 像素的尺寸
		1,																// 版本号
		PFD_DRAW_TO_WINDOW |											// pfd必须支持窗口绘制
		PFD_SUPPORT_OPENGL |											// pfd必须支持OpenGL
		PFD_DOUBLEBUFFER,												// pfd必须支持双缓存
		PFD_TYPE_RGBA,													// 像素格式为RGBA
		window->init.bitsPerPixel,										// 设置颜色深度
		0, 0, 0, 0, 0, 0,												// 忽略颜色位数
		0,																// 无Alpha缓存
		0,																// 忽略偏移位
		0,																// 无累积缓存
		0, 0, 0, 0,														// 忽略累积缓存位 
		16,																// 深度缓存为16位 
		0,																// 无模板缓存
		0,																// 无辅助缓存
		PFD_MAIN_PLANE,													// 主要绘制层
		0,																// 保留位
		0, 0, 0															// 忽略层遮罩
	};

	RECT windowRect = {0, 0, window->init.width, window->init.height};	// 定义窗口大小

	GLuint PixelFormat;													// 保存像素格式

	if (window->init.isFullScreen == TRUE)								// 切换全屏
	{
		if (ChangeScreenResolution(window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			// 全屏切换失败
			MessageBox(HWND_DESKTOP, "无法切换到全屏模式,在窗口模式下运行.\nMode Switch Failed,Running In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;							// 设置 isFullscreen 为False
		}
		else															// 全屏切换成功
		{
			ShowCursor (FALSE);											// 隐藏鼠标
			windowStyle = WS_POPUP;										// 设置窗口样式
			windowExtendedStyle |= WS_EX_TOPMOST;						// 设置窗口扩展样式
		}
	}
	else															
	{
		// 调整窗口大小,包括窗口边界
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// 开始创建 OpenGL 窗口
	window->hWnd = CreateWindowEx(windowExtendedStyle,					// 窗口扩展样式
									window->init.application->className,// 应用程序类名
									window->init.title,					// 窗口标题
									windowStyle,						// 窗口样式
									0, 0,								// 窗口的 X,Y 坐标位置
									windowRect.right - windowRect.left,	// 窗口宽度
									windowRect.bottom - windowRect.top,	// 窗口高度
									HWND_DESKTOP,						// 父窗口为桌面
									0,									// 无菜单
									window->init.application->hInstance,// 传入应用程序实例 
									window);

	if (window->hWnd == 0)												// 窗口是否成功创建
	{
		return FALSE;													// 若失败,则返回FALSE
	}

	window->hDC = GetDC(window->hWnd);									// 取得当前窗口的设备描述表
	if (window->hDC == 0)												// 若未得到设备描述表
	{
		DestroyWindow(window->hWnd);									// 销毁该窗口
		window->hWnd = 0;												// 窗口句柄清零
		return FALSE;													// 返回FALSE
	}

	PixelFormat = ChoosePixelFormat(window->hDC, &pfd);					// 选择兼容的像素格式
	if (PixelFormat == 0)												// 若选择失败
	{
		ReleaseDC(window->hWnd, window->hDC);							// 释放设备描述表
		window->hDC = 0;												// 将设备描述表清零
		DestroyWindow(window->hWnd);									// 销毁窗口
		window->hWnd = 0;												// 窗口句柄清零
		return FALSE;													// 返回FALSE
	}

	if (SetPixelFormat(window->hDC, PixelFormat, &pfd) == FALSE)		// 设置像素格式并判断是否失败
	{
		ReleaseDC(window->hWnd, window->hDC);							// 释放设备描述表
		window->hDC = 0;												// 将设备描述表清零
		DestroyWindow(window->hWnd);									// 销毁窗口
		window->hWnd = 0;												// 窗口句柄清零
		return FALSE;													// 返回FALSE
	}

	window->hRC = wglCreateContext(window->hDC);						// 取得绘制描述表
	if (window->hRC == 0)												// 若未得到绘制描述表
	{
		ReleaseDC(window->hWnd, window->hDC);							// 释放设备描述表 
		window->hDC = 0;												// 将设备描述表清零
		DestroyWindow(window->hWnd);									// 销毁窗口
		window->hWnd = 0;												// 窗口句柄清零
		return FALSE;													// 返回FALSE
	}

	if (wglMakeCurrent(window->hDC, window->hRC) == FALSE)				// 设置绘制描述表并判断是否失败
	{
		wglDeleteContext(window->hRC);									// 删除绘制描述表
		window->hRC = 0;												// 将绘制描述表清零
		ReleaseDC(window->hWnd, window->hDC);							// 释放设备描述表
		window->hDC = 0;												// 将设备描述表清零
		DestroyWindow(window->hWnd);									// 销毁窗口
		window->hWnd = 0;												// 窗口句柄清零
		return FALSE;													// 返回FALSE
	}

	ShowWindow(window->hWnd, SW_NORMAL);								// 显示窗口
	ResizeWindowGL(window->init.width, window->init.height);			// 重设窗口
	ZeroMemory(window->keys, sizeof (Keys));							// 清空键盘缓冲区

	return TRUE;														// 窗口创建成功
}

BOOL DestroyWindowGL(GL_Window* window)									// 销毁窗口并释放程序所用的资源
{
	if (window->hWnd != 0)												// 窗口释放有句柄?
	{	
		if (window->hDC != 0)											// 窗口是否有得到绘制描述表?
		{
			wglMakeCurrent(window->hDC, 0);								// 将当前描述表指针置为0
			if (window->hRC != 0)										// 该窗口是否有绘制描述表
			{
				wglDeleteContext(window->hRC);							// 释放绘制描述表
				window->hRC = 0;										// 将绘制描述表清零
			}
			ReleaseDC(window->hWnd, window->hDC);						// 释放设备描述表
			window->hDC = 0;											// 将设备描述表清零
		}
		DestroyWindow(window->hWnd);									// 销毁窗口
		window->hWnd = 0;												// 将窗口句柄清零
	}

	if (window->init.isFullScreen)										// 若窗口在全屏模式下
	{
		ChangeDisplaySettings(NULL ,0);									// 切换为桌面分辨率
		ShowCursor(TRUE);												// 显示鼠标
	}	
	return TRUE;														// 返回TRUE
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)// 窗口消息处理
{

	GL_Window* window = (GL_Window*)(GetWindowLong(hWnd, GWL_USERDATA));// 取得窗口信息

	switch (uMsg)														// 取得Window的消息
	{
	case WM_SYSCOMMAND:													// 截取系统命令
		{
			switch (wParam)												// 监听系统调用
			{
			case SC_SCREENSAVE:											// 要运行屏保?
			case SC_MONITORPOWER:										// 显示器进入节电模式?
				return 0;												// 提前返回0,防止系统调用执行
			}
			break;														// 退出
		}

	case WM_CREATE:														// 创建窗口
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// 保存窗口结构指针
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)(window));			// 改变窗口属性
			return 0;													// 返回
		}

	case WM_TIMER:														// TIMER事件
		{
			InvalidateRect(window->hWnd, NULL, FALSE);					// 使窗口失效
			break;
		}	

	case WM_PAINT:														// PAINT消息处理,在此处绘图
		{	
			DrawSceneGL();												// 在此处绘图
			SwapBuffers(window->hDC);									// 交换缓存
			ValidateRect(window->hWnd, NULL);							// 使窗口有效
			break;
		}	

	case WM_CLOSE:														// 关闭窗口
		{
			PostMessage(window->hWnd, WM_QUIT, 0, 0);					// 结束程序
			return 0;
		}

	case WM_SIZE:														// 窗口大小变化
		{
			ResizeWindowGL(LOWORD(lParam), HIWORD(lParam));				// 修改窗口大小为 Width = LoWord, Height = HiWord
			return 0;													// 返回
		}

	case WM_KEYDOWN:													// 按键按下时更新键盘缓冲
		if ((wParam >= 0) && (wParam <= 255))							// 按键是否合法?
		{
			window->keys->keyDown[wParam] = TRUE;						// 设相应键为 True
			return 0;													// 返回
		}
		break;															// 退出

	case WM_KEYUP:														// 按键松开时更新键盘缓冲
		if ((wParam >= 0) && (wParam <= 255))							// 按键是否合法?
		{
			window->keys->keyDown[wParam] = FALSE;						// 设相应键为为 FALSE
			return 0;													// 返回
		}
		break;															// 退出

	case WM_TOGGLEFULLSCREEN:											// 开关全屏模式
		{
			Deinitialize();												// 做扫尾工作
			DestroyWindowGL(window);									// 销毁窗口
			window->init.isFullScreen = !window->init.isFullScreen;		// 改变窗口模式
			CreateWindowGL(window);										// 重新创建窗口
			Initialize(window, window->keys);							// 初始化OpenGL绘制程序
			break;														// 退出
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);					// 将本程序不处理的消息传给 DefWindowProc 
}

BOOL RegisterWindowClass(Application* application)						// 为本应用程序注册一个类
{	
	WNDCLASSEX windowClass;												// 窗口类
	ZeroMemory (&windowClass, sizeof(WNDCLASSEX));						// 清空内存
	windowClass.cbSize		= sizeof(WNDCLASSEX);						// 窗口类的大小
	windowClass.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// 在窗口移动,改变大小的时候重绘 
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				// 用WindowProc函数处理消息
	windowClass.hInstance		= application->hInstance;				// 设置实例
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// 类背景的画刷颜色
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// 载入鼠标指针
	windowClass.lpszClassName	= application->className;				// 设置应用程序的类名
	if (RegisterClassEx(&windowClass) == 0)								// 注册类失败?
	{
		MessageBox(HWND_DESKTOP, "应用程序类注册失败!\nRegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// 返回False (注册失败)
	}
	return TRUE;														// 返回True (注册成功)
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application	application;											// 应用程序
	GL_Window	window;													// 窗口
	Keys		keys;													// 键盘按键
	MSG			msg;													// Window消息
	BOOL		bRet;													// 保存消息获得是否成功

	// 给应用程序赋值
	application.className = CLASSNAME;									// 程序类名字
	application.hInstance = hInstance;									// 程序入口

	// 窗口相关信息设置
	ZeroMemory(&window, sizeof(GL_Window));								// 清零窗口变量的内存空间
	window.keys					= &keys;								// 设置按键
	window.init.application		= &application;							// 设置窗口程序
	window.init.title			= TITLE;								// 设置标题
	window.init.width			= WIDTH;								// 设置窗口宽度
	window.init.height			= HEIGHT;								// 设置窗口高度
	window.init.bitsPerPixel	= BPP;									// 设置每像素的位数
	window.init.isFullScreen	= FALSE;								// 设置初始窗口是否全屏否(FALSE)
	ZeroMemory(&keys, sizeof(Keys));									// 键盘缓冲清零

	if (RegisterWindowClass(&application) == FALSE)						// 注册类是否失败
	{
		MessageBox(HWND_DESKTOP, "窗口类注册失败!\nError Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// 结束程序
	}

	if (CreateWindowGL(&window) == TRUE)								// 创建窗口是否成功
	{	
		if (Initialize(&window, &keys) == FALSE)						// 初始化OpenGL绘制程序
		{
			PostMessage(window.hWnd, WM_QUIT, 0, 0);					// 抛出消息WM_QUIT
		}
	}
	else
	{
		return 0;														// 退出程序
	}

	while ((bRet = GetMessage(&msg, NULL, 0, 0) != 0))					// 循环直到收到消息为WM_QUIT
	{
		if (bRet != -1)
		{
			Update();													// 更新处理消息事件
			TranslateMessage(&msg);										// 传递消息
			DispatchMessage(&msg);										// 分派消息
		}
		else
		{
			break;														// 退出循环
		}
	}
	// 退出消息循环,准备退出程序
	Deinitialize();														// 做退出前的扫尾工作
	DestroyWindowGL(&window);											// 销毁窗口	
	UnregisterClass(application.className, application.hInstance);		// 注销窗口类
	return 0;															// 退出程序
}

AUX_RGBImageRec *LoadImage(char *Filename)								// 加载一个位图
{
	FILE *File = NULL;													// 文件句柄

	if (!Filename)														// 确保文件名已经提供
	{
		return NULL;													// 如果没有则返回NULL
	}

	File = fopen(Filename,"r");											// 检查文件是否存在

	if (File)															// 文件存在吗？
	{
		fclose(File);													// 关闭File文件句柄
		return auxDIBImageLoad(Filename);								// 载入位图并返回其指针
	}

	return NULL;														// 如果加载错误则返回NULL
}

BOOL LoadTextureGL(void)												// 加载位图并转换为纹理
{
	int State = FALSE;													// 状态指示

	AUX_RGBImageRec *TextureImage[MAXTEXTURE];							// 为纹理开辟存储空间

	memset(TextureImage, 0, sizeof(void *)*MAXTEXTURE);           		// 创建一个指向NULL的指针
	//加载位图并检查是否出错，如果位图不存在则返回	
	if ((TextureImage[0] = LoadImage("Data/1.bmp")) 
		&&(TextureImage[1] = LoadImage("Data/2.bmp"))   
		&&(TextureImage[2] = LoadImage("Data/3.bmp")) 
		&&(TextureImage[3] = LoadImage("Data/4.bmp")))
	{   
		State = TRUE;													// 设置状态变量为TRUE
		glGenTextures(MAXTEXTURE, &texture[0]);							// 创建纹理

		// 用位图数据产生纹理
		for (int loop=0; loop<MAXTEXTURE; loop++)						// 循环处理MAXTEXTURE张纹理
		{
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	for (int loop=0; loop<MAXTEXTURE; loop++)
	{
		if (TextureImage[loop])											// 如果纹理存在
		{
			if (TextureImage[loop]->data)								// 如果纹理图片存在
			{
				free(TextureImage[loop]->data);							// 释放纹理图片存储空间
			}

			free(TextureImage[loop]);									// 释放纹理空间
		}
	}
	return State;														// 返回State
}

BOOL Initialize(GL_Window* window, Keys* keys)							// 用户绘制场景相关的函数在此处定义	
{
	// 设置全局变量
	OGL_window	= window;
	OGL_keys	= keys;

	// 建立时钟
	SetTimer(window->hWnd, TIMER1, 33, NULL);

	// 在此处初始化绘制场景
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);								// 清屏为黑色
	glClearDepth(1.0f);													// 设置深度缓存
	glDepthFunc(GL_LEQUAL);												// 选择深度测试方式
	glEnable(GL_DEPTH_TEST);											// 开启深度测试
	glShadeModel(GL_SMOOTH);											// 阴暗处理采用平滑方式
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					// 最精细的透视计算

	if (!LoadTextureGL())												// 载入纹理
	{
		return FALSE;
	}

	glEnable(GL_TEXTURE_2D);											// 开启2D纹理映射

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);						// 设置环境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);						// 设置漫反射光

	quadric = gluNewQuadric();											// 建立一个曲面对象指针
	gluQuadricTexture(quadric, GLU_TRUE);								// 建立纹理坐标
	gluQuadricDrawStyle(quadric, GLU_FILL);								// 用面填充
	glEnable(GL_LIGHTING);												// 打开光照
	glEnable(GL_LIGHT1);												// 打开光源1
	// 设置雾的各种参数
	glFogi(GL_FOG_MODE, GL_LINEAR);										// 雾的类型
	glFogfv(GL_FOG_COLOR, fogColor);                  					// 雾的颜色
	glFogf(GL_FOG_DENSITY, 0.6f);                 						// 雾的浓度
	glHint(GL_FOG_HINT, GL_DONT_CARE);                   				// 雾的渲染方式
	glFogf(GL_FOG_START, 1.0f);                          				// 雾的开始深度
	glFogf(GL_FOG_END, 30.0f);                            				// 雾的终止深度
	glEnable(GL_FOG);													// 启用雾

	return TRUE;														// 初始化成功返回TRUE
}

void DrawSceneGL(void)													// 绘制场景
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 清除颜色和深度缓存
	glLoadIdentity();													// 重置当前矩阵
	// 在此处添加代码进行绘制:

	glTranslatef(0.0f, 0.0f, -5.0f);									// 将坐标系移入屏幕8.0f
	glRotatef(10, 1.0f ,0.0f, 0.0f);									// 将坐标系绕x轴旋转10度
	glEnable(GL_LIGHT0);												// 打开光源0
/**********************************绘制背景星空*********************************************/

	glPushMatrix ();													// 当前模型矩阵入栈
	glTranslatef(-10.0f, 3.0f, 0.0f);
	glRotatef (angle_Z, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);							// 绑定星空纹理
	glBegin(GL_QUADS); 
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -20.0f, -5.0f);
		glTexCoord2f(6.0f, 0.0f); glVertex3f( 20.0f, -20.0f, -5.0f);
		glTexCoord2f(6.0f, 6.0f); glVertex3f( 20.0f,  20.0f, -5.0f);
		glTexCoord2f(0.0f, 6.0f); glVertex3f(-20.0f,  20.0f, -5.0f);
    glEnd();
	glPopMatrix ();														// 当前模型矩阵出栈

/**********************************绘制太阳*************************************************/
	
	glBindTexture(GL_TEXTURE_2D, texture[2]);							// 绑定纹理
	glEnable(GL_BLEND);													// 开启混合
	glDisable(GL_DEPTH_TEST);											// 关闭深度测试
	//绘制太阳光晕
	glDisable(GL_LIGHTING);												// 关闭光照
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);									// 基于源象素alpha通道值的半透明混合函数
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);									// 设置RGBA值
	glBegin(GL_QUADS);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
     glEnd();
	 glDisable(GL_BLEND);												// 关闭混合
	 glEnable(GL_DEPTH_TEST);
	 
	glDisable(GL_TEXTURE_2D);											// 关闭纹理
	
	glEnable(GL_LIGHTING);												// 开启光照
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);					// 设置光源1的当前位置
	
	gluSphere(quadric, 0.3f, 32, 32);									// 绘制太阳球体

/**********************************绘制地球*************************************************/

	glDisable(GL_LIGHT0);
	glRotatef(ep_Angle, 0.0f, 1.0f, 0.0f);								// 将坐标系绕Y轴旋转ep_Angle角度  控制地球公转
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);								// 将坐标系绕X轴旋转-90度
	
	glEnable(GL_TEXTURE_2D );											// 开启纹理
	
	glTranslatef(2.0f, 0.0f, 0.0f);										// 将坐标系右移2.0f
	glBindTexture(GL_TEXTURE_2D, texture[0]);							// 绑定纹理
	
	glPushMatrix ();													// 当前模型视图矩阵入栈
	glRotatef(es_Angle, 0.0f, 0.0f, 1.0f);								// 将坐标系绕Z轴旋转es_Angle角度  控制地球自转
	gluSphere(quadric, 0.2f, 32, 32);									// 地球球体
	glPopMatrix ();														// 当前模型视图矩阵出栈
	
/**********************************绘制月亮*************************************************/

	glRotatef(mp_Angle, 0.0f, 0.0f, 1.0f);								// 将坐标系绕Z轴旋转mp_Angle角度 控制月亮公转
	glBindTexture(GL_TEXTURE_2D, texture[3]);							// 绑定纹理
	glTranslatef(0.5f, 0.0f, 0.0f);										// 右移0.5f
	glRotatef(ms_Angle, 0.0f, 0.0f, 1.0f);								// 将坐标系绕Z轴旋转ms_Angle角度 控制月亮自转
	gluSphere(quadric, 0.05, 32, 32);									// 绘制月亮星体

	// 变量更新
	ep_Angle += 2.0f;
	mp_Angle += 5.0f;
	es_Angle += 5.0f;
	ms_Angle += 5.0f;
	
	angle_Z += 0.1f;

	glFlush();															// 刷新GL命令队列
}

void Update(void)														// 在此处更新对消息的动作
{
	if (OGL_keys->keyDown[VK_ESCAPE] == TRUE)							// 判断ESC键是否按下
	{
		PostMessage(OGL_window->hWnd, WM_QUIT, 0, 0);					// 结束程序
	}

	if (OGL_keys->keyDown[VK_F1] == TRUE)								// 判断F1是否按下
	{
		PostMessage(OGL_window->hWnd, WM_TOGGLEFULLSCREEN, 0, 0);		// 在全屏/窗口模式间切换
	}
}

void Deinitialize(void)													// 在此处做退出前扫尾工作
{
	KillTimer(OGL_window->hWnd, TIMER1);								// 释放时钟
}
