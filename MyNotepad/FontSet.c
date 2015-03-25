/*---------------------------
	记事本字体设置模块
		宋保明，2014
  ---------------------------*/
#include <Windows.h>
#include <CommDlg.h>

LOGFONT logfont;
static HFONT hfont;

//初始化字体对话框样式
BOOL PopChooseFont(HWND hwnd)
{
	CHOOSEFONT cf;

	cf.Flags			= CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS;
	cf.hDC				= NULL;
	cf.hInstance		= NULL;
	cf.hwndOwner		= hwnd;
	cf.iPointSize		= 0;
	cf.lCustData		= 0;
	cf.lpfnHook			= NULL;
	cf.lpLogFont		= &logfont;
	cf.lpszStyle		= NULL;
	cf.lpTemplateName	= NULL;
	cf.lStructSize		= sizeof(cf);
	cf.nFontType		= 0;
	cf.nSizeMax			= 0;
	cf.nSizeMin			= 0;
	cf.rgbColors		= 0;
	
	return ChooseFont(&cf);
}
//将用户选择字体初始化
void ChangeFont(void)
{
	DeleteObject(hfont);
	hfont = CreateFontIndirect(&logfont);
}
//设置编辑框字体
void SetFont(HWND hwndedit)
{
	SendMessage(hwndedit,WM_SETFONT,(WPARAM)hfont,0);
	UpdateWindow(hwndedit);
}
//初始化编辑框字体
void FontInitialize(HWND hwndedit)
{
	HDC hdc = GetDC(hwndedit);
	wcscpy_s(logfont.lfFaceName ,32, TEXT("宋体"));
	logfont.lfHeight = /*字号*/10.5 * GetDeviceCaps(hdc,LOGPIXELSX) / 72;
	ReleaseDC(hwndedit, hdc);

	hfont = CreateFontIndirect(&logfont);
	SendMessage(hwndedit,WM_SETFONT,(WPARAM)hfont,0);
}
//删除建立字体
void DeleteFont(void)
{
	DeleteObject(hfont);
}