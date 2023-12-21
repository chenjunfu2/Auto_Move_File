// 自动拷贝壁纸.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "main.h"

#define MAX_LOADSTRING 100
#define WM_NEW_INSTANCE_OPEN (WM_USER + 1)//自定义消息

// 全局变量:
HINSTANCE hInst;                                //当前实例
WCHAR szTitle[MAX_LOADSTRING];                  //标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            //主窗口类名
WCHAR szSourceDir[MAX_PATH + 1];				//源路径
WCHAR szSourceFindDir[MAX_PATH + 1];			//查找用，带通配符的源路径
WCHAR szTargetDir[MAX_PATH + 1];				//目标路径
HANDLE hMutex;									//互斥量

/*
#define SOURCE_DIR L"C:\Users\chenj\AppData\Local\Packages\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\LocalState\Assets\"
#define TARGET_DIR L"D:\windows锁屏\"
*/

// 此代码模块中包含的函数的前向声明:
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

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_APP_CLASS, szWindowClass, MAX_LOADSTRING);

	// 创建或打开互斥体
	hMutex = CreateMutexW(NULL, TRUE, L"Auto_Move_File_Unique_Instance_Name");
	DWORD dwErrCode = GetLastError();

	// 检查互斥体是否已经存在，如果存在则表明应用程序已经运行
	if (hMutex != NULL && dwErrCode == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);//关闭互斥量引用
		BOOL b = PostMessageW(FindWindowW(szWindowClass, szTitle), WM_NEW_INSTANCE_OPEN, 0, 0);//给正在运行的实例发一条消息
		return (b == 0) ? 0 : -1;//提前返回
	}
	else if (hMutex == NULL)
	{
		MessageBoxW(NULL, L"错误: 无法创建互斥体!\n可能出现名称冲突或资源泄漏\n请检查名称空间和资源\n", szTitle, MB_OK);
		return -1;//失败返回
	}

	// 解析命令行参数字符串为参数数组
	int argc;
	LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);
	if (argv == NULL || argc != 2)
	{
		MessageBoxW(NULL, L"错误: 命令行参数不足或过多!\n正确命令为: [程序名] [源路径] [目标路径]\n路径后[不需要]以斜杠或反斜杠结尾", szTitle, MB_OK);
		return -1;//错误的命令行参数
	}

	//拷贝命令行参数
	if (lstrcpynW(szSourceDir, argv[0], MAX_PATH) == NULL||
		lstrcpynW(szSourceFindDir, szSourceDir, MAX_PATH - 1) == NULL||
		lstrcpynW(szTargetDir, argv[1], MAX_PATH) == NULL)
	{
		return -1;
	}

	//释放命令行
	LocalFree(argv);
	argv = NULL;

	//补充目录末尾反斜杠和查找路径的通配符*
	if (lstrcatW(szSourceDir, L"\\") == NULL ||
		lstrcatW(szSourceFindDir, L"\\*") == NULL ||
		lstrcatW(szTargetDir, L"\\") == NULL)
	{
		return -1;
	}

	// TODO: 在此处放置代码。
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDC_APP_CLASS));

	MSG msg;

	// 主消息循环:
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APP_CLASS);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);//始终隐藏自身

   return TRUE;
}

void CreateNewThread(BOOL bClear);

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NEW_INSTANCE_OPEN://用户正在打开一个新实例，说明需要检查一下，直接fall到下一个case块[[fallthrough]]
	case WM_CREATE://收到此消息，说明应用程序被手动、系统开机自动启动，检查一下Aeests文件夹
		{
			CreateNewThread(false);
		}
		break;
	case WM_POWERBROADCAST://收到此消息，说明应用程序有可能因为系统从休眠、睡眠模式中恢复，确认后检查一下Aeests文件夹
		{
			if (wParam == PBT_APMRESUMEAUTOMATIC)//成功确认，开始检查
			{
				CreateNewThread(false);
			}
			else//默认处理
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_DESTROY:
		CreateNewThread(true);
		CloseHandle(hMutex);//关闭互斥量引用
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


DWORD WINAPI MoveImage(LPVOID)
{
#ifndef _WIN64
	PVOID oldVal;
	Wow64DisableWow64FsRedirection(&oldVal);
#endif // _WIN64

	BOOL bSrc = PathIsDirectoryW(szSourceDir);
	BOOL bDst = PathIsDirectoryW(szTargetDir);

	if (!bSrc)
	{
		MessageBoxW(NULL, L"错误: 源目录不是有效目录!\n请检查目录路径\n", szTitle, MB_OK);
		MessageBoxW(NULL, szSourceDir, L"源目录路径", MB_OK);
	}

	if (!bDst)
	{
		MessageBoxW(NULL, L"错误: 目标目录不是有效目录!\n请检查目录路径\n", szTitle, MB_OK);
		MessageBoxW(NULL, szTargetDir, L"目标目录路径", MB_OK);
	}

	if (!bSrc || !bDst)
	{
		return -1;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFileW(szSourceFindDir, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//是目录
		{
			continue;
		}

		// 提取源文件的文件名
		//LPCWSTR fileName = PathFindFileNameW(findFileData.cFileName);

		// 组合目标文件夹路径和文件名
		WCHAR sourceFilePath[MAX_PATH + 1];
		WCHAR targetFilePath[MAX_PATH + 1];
		
		PathCombineW(sourceFilePath, szSourceDir, findFileData.cFileName);
		PathCombineW(targetFilePath, szTargetDir, findFileData.cFileName);

		MoveFileExW(sourceFilePath, targetFilePath, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);//忽略失败
	} while (FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);

#ifndef _WIN64
	Wow64RevertWow64FsRedirection(oldVal);
#endif // !_WIN64
	return 0;
}

void CreateNewThread(BOOL bClear)
{
	static HANDLE hThread = NULL;

	if (hThread != NULL)
	{
		// 等待线程结束
		WaitForSingleObject(hThread, INFINITE);
		// 关闭线程句柄
		CloseHandle(hThread);
	}

	if (!bClear)
	{
		//重新创建
		hThread = CreateThread(NULL, 0, MoveImage, NULL, 0, NULL);
	}
	else
	{
		hThread = NULL;
	}
}