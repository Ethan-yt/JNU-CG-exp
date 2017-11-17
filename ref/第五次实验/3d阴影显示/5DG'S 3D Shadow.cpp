#define WM_TOGGLEFULLSCREEN (WM_USER + 1)								// 定义全屏/窗口切换消息
#define TIMER1		101													// 定义时钟代号,设置为101号
#define TITLE		"5DG'S 3D Shadow!"									// 定义窗口标题
#define CLASSNAME	"5DG_OPENGL"										// 定义窗口类名
#define	WIDTH		640													// 定义窗口宽度
#define HEIGHT		480													// 定义窗口高度
#define BPP			16													// 定义每象素的位数

#define MAXTEXTURE	4					 								// 定义最大的纹理数目

// 在此处引用程序要求的头文件:
#include <windows.h>													// Windows的头文件
#include <gl\gl.h>														// OpenGL32库的头文件
#include <gl\glu.h>														// Glu32库的头文件
#include <gl\glaux.h>													// Glaux库的头文件
#include <stdio.h>														// 标准输入/输出库的头文件

// 在此处加入程序要求的库到链接器中:
#pragma comment(lib, "opengl32.lib")									// 链接时查找OpenGL32.lib
#pragma comment(lib, "glu32.lib")										// 链接时查找glu32.lib
#pragma comment(lib, "glaux.lib")										// 链接时查找glaux.lib

// 在此处定义与窗口相关的结构体:
typedef struct {														// 定义处理键盘的结构体
	BOOL keyDown [256];													// 存储键盘按键状态的数组
} Keys;

typedef struct {														// 定义存储应用程序实例的结构体
	HINSTANCE		hInstance;											// 应用程序实例
	const char*		className;											// 应用程序类名
} Application;

typedef struct {														// 定义初始窗口所需信息
	Application*	application;										// 所属的应用程序
	char*			title;												// 窗口标题
	int				width;												// 窗口宽度
	int				height;												// 窗口高度
	int				bitsPerPixel;										// 每像素的位数
	BOOL			isFullScreen;										// 是否全屏
} GL_WindowInit;

typedef struct {														// 定义窗口结构体
	Keys*			keys;												// 窗口的键盘
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
GLUquadricObj	*quadric;												// 存放二次曲面的物体
GLfloat shadow_matrix[4][4];											// 存放阴影矩阵
const GLfloat P[4] = {0.0f, 1.0f, 0.0f, 2.0f};							// 阴影平面向量

GLfloat planePos[]  = {0.0f,-1.2f, 0.0f};								// 直升飞机的位置

GLfloat lightPos[] = {0.0f, 4.0f,-3.0f, 0.0f};							// 定义光的位置
GLfloat lightAmb[] = {0.8f, 0.8f, 0.8f, 1.0f};							// 定义环境光的颜色
GLfloat lightDif[] = {1.0f, 0.7f, 0.8f, 1.0f};							// 定义漫反射光的颜色
GLfloat lightSpc[] = {0.8f, 1.0f, 1.0f, 1.0f};							// 定义镜面反射光的颜色

GLfloat matAmb[] = {0.4f, 0.4f, 0.4f, 1.0f};							// 材质的环境光颜色
GLfloat matDif[] = {1.0f, 1.0f, 1.0f, 1.0f};							// 材质的漫反射光颜色
GLfloat matSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};							// 材质的镜面反射光颜色
GLfloat matShn[] = {0.0f};								    			// 材质的镜面指数

GLuint	texture[MAXTEXTURE];											// 纹理数组
BOOL	pKey_space;														// 判断空格键是否按下
GLfloat	angle_X1;														// 控制物体绕X轴转动
GLfloat	angle_Y1;														// 控制物体绕Y轴转动
GLfloat	angle_X2;														// 控制物体绕X轴转动
GLfloat	angle_Y2;														// 控制物体绕Y轴转动
GLfloat	rot_X;															// 物体绕X轴转动的速度
GLfloat	rot_Y;															// 物体绕Y轴转动的速度
GLfloat	rot;															// 直升飞机螺旋桨的转动速度
GLfloat	z=-8.0f;														// 进入平面的深度
GLint	change;															// 控制当前该绘制何种物体

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

AUX_RGBImageRec *LoadImage(char *Filename)								// 加载一个图片
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
		return auxDIBImageLoad(Filename);								// 载入图片并返回其指针
	}

	return NULL;														// 如果加载错误则返回NULL
}

BOOL LoadTextureGL(void)												// 加载图片并转换为纹理
{
	BOOL State = FALSE;													// 状态指示

	AUX_RGBImageRec *TextureImage[MAXTEXTURE];							// 为纹理开辟存储空间

	memset(TextureImage, 0, sizeof(void *)*MAXTEXTURE);					// 清除图像记录，确保其内容为空并使指针指向NULL
	// 加载图片并检查是否出错 ，如果图片不存在则返回
	if ((TextureImage[0] = LoadImage("Data/ground1.bmp"))
		&& (TextureImage[1] = LoadImage("Data/ground2.bmp"))
		&& (TextureImage[2] = LoadImage("Data/wood.bmp"))
		&& (TextureImage[3] = LoadImage("Data/plane.bmp")))
	{   
		State=TRUE;														// 设置状态变量为TRUE
		glGenTextures(MAXTEXTURE, &texture[0]);							// 返回唯一的纹理名字来标识纹理,保存在texture中

		// 用图片数据产生纹理
		for (int loop=0; loop<MAXTEXTURE; loop++)						// 循环处理MAXTEXTURE张纹理
		{
			// 使用来自位图数据生成的典型纹理
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			// 生成纹理 
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// 线形滤波
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 线形滤波
		}
	}
	for (int loop=0; loop<MAXTEXTURE; loop++)
	{
		if (TextureImage[loop])											// 纹理是否存在	
		{
			if (TextureImage[loop]->data)								// 纹理图像是否存在
			{
				free(TextureImage[loop]->data);							// 释放纹理存储空间
			}
			free(TextureImage[loop]);									// 释放图像结构
		}
	}
	return State;														// 返回State
}

// 计算阴影矩阵
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

// 绘制直升飞机
void DrawPlane(void)
{
	// 绘制直升飞机机头
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
	// 直升机机头绘制完成

	// 绘制直升机机尾
	glPushMatrix();
        glTranslatef(-2.8f, -0.5f, 0.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-7.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.3f, 0.2f, 4.3f, 32, 32);
    glPopMatrix();
	// 绘制机尾翘起部分
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
	// 直升机机尾绘制完成

	// 绘制飞机螺旋桨
	// 绘制螺旋桨支柱
	glPushMatrix();
		glTranslatef(-0.5, 0.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.2f, 0.2f,1.5f, 32, 32);
	glPopMatrix();
	// 绘制螺旋桨叶片
	glPushMatrix();
		glTranslatef(-0.5f, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(angle_X2, 0.0f, 1.0f, 0.0f);//旋转飞机螺旋桨叶片 
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
	// 直升机螺旋桨绘制完成

	// 绘制直升机底座
	// 绘制底座4个支柱
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
	// 绘制底座的底板
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
	// 直升机底座绘制完成
}

//绘制长方体width，height，depth分别为长方体的长，高和深度
void DrawCube(GLfloat width, GLfloat height, GLfloat depth)
{
	GLfloat x = width/2, y = height/2, z = depth/2;
	glBegin(GL_QUADS);													// 绘制长方体的各面
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y, z);					// 前面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y, z);					// 前面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y, z);					// 前面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y, z);					// 前面四边形的左上顶点
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( x,-y,-z);					// 后面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,-y,-z);					// 后面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, y,-z);					// 后面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f( x, y,-z);					// 后面四边形的左上顶点
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( x,-y, z);					// 右侧面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y,-z);					// 右侧面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y,-z);					// 右侧面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f( x, y, z);					// 右侧面四边形的左上顶点
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y,-z);					// 左侧面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,-y, z);					// 左侧面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, y, z);					// 左侧面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y,-z);					// 左侧面四边形的左上顶点
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, z);					// 顶面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x, y, z);					// 顶面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y,-z);					// 顶面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, y,-z);					// 顶面四边形的左上顶点
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,-y,-z);					// 底面四边形的左下顶点（正视）
		glTexCoord2f(0.0f, 1.0f); glVertex3f( x,-y,-z);					// 底面四边形的右下顶点
		glTexCoord2f(1.0f, 1.0f); glVertex3f( x,-y, z);					// 底面四边形的右上顶点
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-x,-y, z);					// 底面四边形的左上顶点
	glEnd();
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
	
	if (!LoadTextureGL())												// 调用LoadTextureGL()函数
	{
		return FALSE;													// 如果纹理加载错误则返回FALSE
	}
	glEnable(GL_TEXTURE_2D);											// 开启2D纹理映射
	
	quadric = gluNewQuadric();											// 建立一个曲面对象指针
    gluQuadricTexture(quadric, GLU_TRUE);								// 建立纹理坐标
    gluQuadricDrawStyle(quadric, GLU_FILL);								// 用面填充
	
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);						// 绘制光源1
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);							// 设置光源1的环境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);							// 设置光源1的漫反射光
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);						// 设置光源1的镜面反射光
	glEnable(GL_LIGHT1);												// 启用光源1

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);							// 设置材质的环境光
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);							// 设置材质的漫反射光
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpc);						// 设置材质的镜面反射光
	glMaterialfv(GL_FRONT, GL_SHININESS, matShn);						// 设置材质的镜面指数

	return TRUE;														// 初始化成功返回TRUE
}

void DrawSceneGL(void)													// 绘制场景
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 清除颜色和深度缓存
	glLoadIdentity();													// 重置当前矩阵
	//在此处添加代码进行绘制:
	glTranslatef(0.0f, 0.0f, z);										// 移入屏幕z

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);						// 光源1的位置
	glEnable(GL_LIGHTING);												// 启用光照
	glEnable(GL_TEXTURE_2D);											// 启用纹理贴图

	if (change == 0)													// 若change等于0选择并绑定第一张纹理
	{
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}
	else																// 若change等于1选择并绑定第二张纹理
	{
		glBindTexture(GL_TEXTURE_2D, texture[1]);
	}
	glBegin(GL_QUADS);													// 绘制阴影平面	
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 8.0f, -2.0f, -16.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-8.0f, -2.0f, -16.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, -2.0f,  16.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 8.0f, -2.0f,  16.0f);
	glEnd();

	// change等于0时绘制十字架
	if (change == 0)
	{
		glPushMatrix();													// 将当前矩阵推入栈中
			glBindTexture(GL_TEXTURE_2D, texture[2]);					// 选择并绑定第三张纹理
			glRotatef(angle_X1, 1.0f, 0.0f, 0.0f);						// 让十字架绕X轴转动
			glRotatef(angle_Y1, 0.0f, 1.0f, 0.0f);						// 让十字架绕Y轴转动
			// 绘制十字架
			DrawCube(2.0f, 0.2f, 0.2f);
			DrawCube(0.2f, 2.0f, 0.2f);
			DrawCube(0.2f, 0.2f, 2.0f);
		glPopMatrix();													// 恢复矩阵

		glPushMatrix();													// 将当前矩阵推入栈中
			glDisable(GL_LIGHTING);										// 关闭光源
			glDisable(GL_TEXTURE_2D);									// 禁用纹理
			glColor3f(0.0f, 0.0f, 0.0f);								// 设置阴影颜色
			glTranslatef(0.0f, 0.01f, 0.0f);
			// 调用generate_shadow_matrix()函数计算阴影矩阵并与当前矩阵相乘，得到绘制阴影的矩阵
			generate_shadow_matrix(shadow_matrix, P, lightPos);
			glMatrixMode(GL_MODELVIEW);	
			glMultMatrixf((GLfloat*) shadow_matrix);
			// 让阴影跟随实际物体一起运动
			glRotatef(angle_X1, 1.0f, 0.0f, 0.0f);
			glRotatef(angle_Y1, 0.0f, 1.0f, 0.0f);
			// 绘制阴影十字架
			DrawCube(2.0f, 0.2f, 0.2f);
			DrawCube(0.2f, 2.0f, 0.2f);
			DrawCube(0.2f, 0.2f, 2.0f);
		glPopMatrix();													// 恢复矩阵

		angle_X1 += rot_X;
		angle_Y1 += rot_Y;
	}
	// change等于1时绘制直升飞机
	if (change == 1)
	{	
		glPushMatrix();													// 将当前矩阵推入栈中
			glBindTexture(GL_TEXTURE_2D, texture[3]);					// 选择并绑定第四张纹理
			glRotatef(angle_Y2, 0.0f, 1.0f, 0.0f);						// 改变直升飞机前进的方向
			glTranslatef(planePos[0], planePos[1], planePos[2]);
			glScalef(0.2f, 0.2f, 0.2f);
			DrawPlane();												// 绘制直升飞机
		glPopMatrix();													// 恢复矩阵

		glPushMatrix();													// 将当前矩阵推入栈中
			glDisable(GL_LIGHTING);										// 关闭光源
			glDisable(GL_TEXTURE_2D);									// 禁用纹理
			glColor3f(0.0f, 0.0f, 0.0f);								// 设置阴影颜色
			glTranslatef(0.0f, 0.01f, 0.0f);
			// 调用generate_shadow_matrix()函数计算阴影矩阵并与当前矩阵相乘，得到绘制阴影的矩阵
			generate_shadow_matrix(shadow_matrix, P, lightPos);
			glMatrixMode(GL_MODELVIEW);
			glMultMatrixf((GLfloat*) shadow_matrix);
			// 让阴影跟随实际物体一起运动
			glRotatef(angle_Y2, 0.0f, 1.0f, 0.0f);
			glTranslatef(planePos[0], planePos[1], planePos[2]);
			glScalef(0.2f, 0.2f, 0.2f);
			DrawPlane();												// 绘制直升飞机
		glPopMatrix();													// 恢复矩阵

		// 若直升飞机已到地面，控制螺旋桨停下;否则加速
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

	// 绘制一个模拟光源，表示光射来的方向
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 0.0f);
		glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
		gluSphere(quadric, 0.1f, 32, 16);
	glPopMatrix();

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
	
	// 改变光源的位置
	if (OGL_keys->keyDown['D'] == TRUE)									// D键是否按下
	{
		lightPos[0] += 0.05f;
	}
	if (OGL_keys->keyDown['A'] == TRUE)									// A键是否按下
	{
		lightPos[0] -= 0.05f;
	}
	if (OGL_keys->keyDown['W'] == TRUE)									// W键是否按下
	{	
		lightPos[1] += 0.05f;
	}
	if (OGL_keys->keyDown['S'] == TRUE)									// S键是否按下
	{	
		lightPos[1] -= 0.05f;
	}
	if (OGL_keys->keyDown['Q'] == TRUE)									// Q键是否按下
	{
		lightPos[2] += 0.05f;
	}
	if (OGL_keys->keyDown['E'] == TRUE)									// E键是否按下
	{
		lightPos[2] -= 0.05f;
	}

	// 改变飞机的位置
	if (OGL_keys->keyDown[VK_RIGHT] == TRUE && planePos[1]>=-1.0f)		// RIGHT键是否按下
	{
		planePos[0] += 0.05f;
	}
	if (OGL_keys->keyDown[VK_LEFT] == TRUE && planePos[1]>=-1.0f)		// LEFT键是否按下
	{
		planePos[0] -= 0.05f;
	}
	if (OGL_keys->keyDown[VK_UP] == TRUE && rot>=30.0f)					// UP键是否按下
	{
		planePos[1] += 0.05f;
	}
	if (OGL_keys->keyDown[VK_DOWN] == TRUE && planePos[1]>-1.2f)		// DOWN键是否按下
	{
		planePos[1] -= 0.05f;
	}
	// 控制螺旋桨的速度
	if (OGL_keys->keyDown[VK_UP] == TRUE)								// UP键是否按下
	{
		rot += 0.5f;
	}
	// 若planePos[1]>=-1.0f则可以改变直升飞机前进的方向
	if (OGL_keys->keyDown[VK_NUMPAD4] == TRUE && planePos[1]>=-1.0f)	// 数字键4是否按下
	{
		angle_Y2 -= 0.5;
	}
	if (OGL_keys->keyDown[VK_NUMPAD6] == TRUE && planePos[1]>=-1.0f)	// 数字键6是否按下
	{
		angle_Y2 += 0.5;
	}

	// 控制十字架的旋转
	if (OGL_keys->keyDown[VK_UP] == TRUE)								// UP键是否按下
	{
		rot_X -= 0.5;
	}
	if (OGL_keys->keyDown[VK_DOWN] == TRUE)								// DOWN键是否按下
	{
		rot_X += 0.5;
	}
	if (OGL_keys->keyDown[VK_LEFT] == TRUE)								// LEFT键是否按下
	{
		rot_Y -= 0.5;
	}
	if (OGL_keys->keyDown[VK_RIGHT] == TRUE)							// RIGHT键是否按下
	{
		rot_Y += 0.5;
	}

	if (OGL_keys->keyDown[VK_SPACE] == TRUE && !pKey_space)				// 空格键是否按下
	{
		// 若空格键按下，则改变change的值并初始化下列各值
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
	if (OGL_keys->keyDown[VK_SPACE] == FALSE)							// 空格键是否放开
	{
		pKey_space = FALSE;
	}

	// 控制场景的深度
	if (OGL_keys->keyDown[VK_PRIOR] == TRUE)							// PageUp键是否按下
	{
		z += 0.5f;
	}
	if (OGL_keys->keyDown[VK_NEXT] == TRUE)								// PageDown键是否按下
	{
		z -= 0.5f;
	}
}

void Deinitialize(void)													// 在此处做退出前扫尾工作
{
	KillTimer(OGL_window->hWnd, TIMER1);								// 释放时钟
}