#include <Windows.h>
#include "resource.h"

/////////////////使用xp风格控件 version 6.0////////////////

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifndef MY_NOTEPAD
#define MY_NOTEPAD
//编辑控件ID
#define EDITID1 1
#define EDITID2	2

extern HWND hDlgModeless;
extern TCHAR szAppName[];

//打印函数
void PageSetup(HINSTANCE hInst, HWND hwnd);
BOOL PrintFile(HINSTANCE hInst, HWND hwndedit, PTSTR TitleName);

//文件操作函数
void OFNInitialize(HWND hwnd);
BOOL FileRead(HWND hwndedit, PTSTR FileName);
BOOL FileWrite(HWND hwndedit, PTSTR FileName);
BOOL FileOpenDlg(HWND hwnd, PTSTR FileName, PTSTR TitleName);
BOOL FileSaveDlg(HWND hwnd, PTSTR FileName, PTSTR TitleName);

//字体设置函数
void FontInitialize(HWND hwndedit);
BOOL PopChooseFont(HWND hwnd);
void SetFont(HWND hwndedit);
void ChangeFont(void);
void DeleteFont(void);

//查找替换函数
HWND FindDlg(HWND hwnd);
HWND ReplaceDlg(HWND hwnd);
BOOL ReplaceAllDesignText(HWND hwndedit, LPFINDREPLACE pfr);
BOOL ReplaceDesignText(HWND hwndedit, int *SearchMark, LPFINDREPLACE pfr);
BOOL SearchText(HWND hwndedit, int *SearchMark, LPFINDREPLACE pfr, BOOL SearchBack);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);
void ChangeTitle(HWND hwnd, TCHAR * TitleName, BOOL NeedSave);
int AskAboutSave(HWND hwnd);
BOOL RestoreWindowState();
void SaveWindowState();
#endif