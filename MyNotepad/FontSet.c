/*---------------------------
	���±���������ģ��
		�α�����2014
  ---------------------------*/
#include <Windows.h>
#include <CommDlg.h>

LOGFONT logfont;
static HFONT hfont;

//��ʼ������Ի�����ʽ
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
//���û�ѡ�������ʼ��
void ChangeFont(void)
{
	DeleteObject(hfont);
	hfont = CreateFontIndirect(&logfont);
}
//���ñ༭������
void SetFont(HWND hwndedit)
{
	SendMessage(hwndedit,WM_SETFONT,(WPARAM)hfont,0);
	UpdateWindow(hwndedit);
}
//��ʼ���༭������
void FontInitialize(HWND hwndedit)
{
	HDC hdc = GetDC(hwndedit);
	wcscpy_s(logfont.lfFaceName ,32, TEXT("����"));
	logfont.lfHeight = /*�ֺ�*/10.5 * GetDeviceCaps(hdc,LOGPIXELSX) / 72;
	ReleaseDC(hwndedit, hdc);

	hfont = CreateFontIndirect(&logfont);
	SendMessage(hwndedit,WM_SETFONT,(WPARAM)hfont,0);
}
//ɾ����������
void DeleteFont(void)
{
	DeleteObject(hfont);
}