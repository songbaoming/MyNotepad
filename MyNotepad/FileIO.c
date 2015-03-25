/*--------------------------
	���±��ļ�����ģ��
		�α�����2014
  --------------------------*/
#include <Windows.h>
#include <CommDlg.h>

static OPENFILENAME ofn;
//��ʼ���ļ������Ի�������ֶ�
void OFNInitialize(HWND hwnd)
{
	static TCHAR Filter[] = TEXT("�ı��ĵ�(*.txt)\0*.txt\0") \
							TEXT("�����ļ�(*.*)\0*.*\0");

	ofn.dwReserved			= 0;
	ofn.Flags				= 0;
	ofn.FlagsEx				= 0;
	ofn.hInstance			= NULL;
	ofn.hwndOwner			= hwnd;
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.lpstrDefExt			= TEXT("txt");
	ofn.lpstrFile			= NULL;
	ofn.lpstrFileTitle		= NULL;
	ofn.lpstrFilter			= Filter;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.lStructSize			= sizeof(ofn);
	ofn.nFileExtension		= 0;
	ofn.nFileOffset			= 0;
	ofn.nFilterIndex		= 0;
	ofn.nMaxCustFilter		= 0;
	ofn.nMaxFile			= MAX_PATH;
	ofn.nMaxFileTitle		= MAX_PATH;
	ofn.pvReserved			= 0;
}
//��ʾ�ļ��򿪶Ի��򲢷���ѡ������
BOOL FileOpenDlg(HWND hwnd,PTSTR FileName,PTSTR TitleName)
{
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFile		= FileName;
	ofn.lpstrFileTitle	= TitleName;
	ofn.Flags			= OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return GetOpenFileName(&ofn);
}
//��ʾ�ļ�����Ի��򲢷���ѡ������
BOOL FileSaveDlg(HWND hwnd,PTSTR FileName,PTSTR TitleName)
{
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFile		= FileName;
	ofn.lpstrFileTitle	= TitleName;
	ofn.Flags			= OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}
//�ļ���ȡ����
BOOL FileRead(HWND hwndedit,PTSTR FileName)
{
	HANDLE hfile;
	BYTE Swap;
	DWORD dwByteRead;
	PBYTE pBuffer,pText,pConvert;
	int i,iFileLength,iUnitest;

	if(INVALID_HANDLE_VALUE == (hfile = CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ,
													NULL,OPEN_EXISTING,0,NULL)))
		return FALSE;
	iFileLength = GetFileSize(hfile,NULL);

	pBuffer = (PBYTE) malloc(iFileLength + 2);
	if(NULL==pBuffer)
	{
		CloseHandle(hfile);
		return FALSE;
	}

	ReadFile(hfile,pBuffer,iFileLength,&dwByteRead,NULL);
	CloseHandle(hfile);
	pBuffer[iFileLength] = '\0';
	pBuffer[iFileLength +1] = '\0';

	iUnitest = IS_TEXT_UNICODE_SIGNATURE |IS_TEXT_UNICODE_REVERSE_SIGNATURE;

	if(IsTextUnicode(pBuffer,iFileLength,&iUnitest))
	{
		pText = pBuffer + 2;
		iFileLength -= 2;
		if(iUnitest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
		{
			for(i=0;i<iFileLength / 2;i++)
			{
				Swap = pText[2*i];
				pText[2*i] = pText[2*i+1];
				pText[2*i+1] = Swap;
			}
		}
		pConvert = (PBYTE)malloc(iFileLength + 2);
		lstrcpy((LPWSTR)pConvert,(PTSTR)pText);
	}
	else
	{
		pText = pBuffer;

		pConvert = (PBYTE)malloc(2*iFileLength + 2);
		if(NULL == pConvert)
		{
			free(pBuffer);
			return FALSE;
		}

		MultiByteToWideChar(CP_ACP,0,(PSTR)pText,-1,(PTSTR)pConvert,iFileLength+1);
	}
	SetWindowText(hwndedit,(PTSTR)pConvert);
	free(pBuffer);
	free(pConvert);
	return TRUE;
}
//�ļ�д�����
BOOL FileWrite(HWND hwndedit,PTSTR FileName)
{
	HANDLE hfile;
	int iLength;
	PTSTR pBuffer;
	DWORD BytesWriten;
	WORD OrderMark = 0xFEFF;

	if(INVALID_HANDLE_VALUE == (hfile = CreateFile(FileName,GENERIC_WRITE,0,
													NULL,CREATE_ALWAYS,0,NULL)))
		return FALSE;
	iLength = GetWindowTextLength(hwndedit);
	pBuffer = (PTSTR)malloc((iLength + 1)*sizeof(TCHAR));
	if(NULL == pBuffer)
	{
		CloseHandle(hfile);
		return FALSE;
	}
	//����ΪUnicode��ʽ�ĵ�
	WriteFile(hfile,&OrderMark,2,&BytesWriten,NULL);
	GetWindowText(hwndedit,pBuffer,iLength + 1);
	WriteFile(hfile,pBuffer,iLength*sizeof(TCHAR),&BytesWriten,NULL);
	if((iLength*sizeof(TCHAR)) != (int)BytesWriten)
	{
		CloseHandle(hfile);
		free(pBuffer);
		return FALSE;
	}
	CloseHandle(hfile);
	free(pBuffer);
	return TRUE;
}