/*----------------------
	记事本查找替换模块
		宋保明，2014
  ----------------------*/
#include <Windows.h>
#include <CommDlg.h>

#define MAX_STR_LEN 256

extern HWND hDlgModeless;
static TCHAR szFindText[MAX_STR_LEN],szReplaceText[MAX_STR_LEN];

HWND FindDlg(HWND hwnd)
{
	static FINDREPLACE fr;

	fr.Flags			= FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.hInstance		= NULL;
	fr.hwndOwner		= hwnd;
	fr.lCustData		= 0;
	fr.lpfnHook			= NULL;
	fr.lpstrFindWhat	= szFindText;
	fr.lpstrReplaceWith	= NULL;
	fr.lpTemplateName	= NULL;
	fr.lStructSize		= sizeof(fr);
	fr.wFindWhatLen		= MAX_STR_LEN;
	fr.wReplaceWithLen	= 0;

	return FindText(&fr);
}

HWND ReplaceDlg(HWND hwnd)
{
	static FINDREPLACE fr;

	fr.Flags			= FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.hInstance		= NULL;
	fr.hwndOwner		= hwnd;
	fr.lCustData		= 0;
	fr.lpfnHook			= NULL;
	fr.lpstrFindWhat	= szFindText;
	fr.lpstrReplaceWith	= szReplaceText;
	fr.lpTemplateName	= NULL;
	fr.lStructSize		= sizeof(fr);
	fr.wFindWhatLen		= MAX_STR_LEN;
	fr.wReplaceWithLen	= MAX_STR_LEN;

	return ReplaceText(&fr);
}

BOOL SearchText(HWND hwndedit,int *SearchMark,LPFINDREPLACE pfr,BOOL SearchBack)
{
	int iLength, iPos;
	PTSTR pDocument,pPosition;

	if(!(iLength = GetWindowTextLength(hwndedit)))
		return FALSE;

	if(NULL == (pDocument = (PTSTR)malloc((iLength + 1)*sizeof(TCHAR))))
		return FALSE;

	GetWindowText(hwndedit,pDocument,iLength+1);
	
	pPosition = wcsstr(pDocument + *SearchMark,pfr->lpstrFindWhat);
	if(pPosition == NULL)
	{
		pPosition = wcsstr(pDocument,pfr->lpstrFindWhat);
		if(pPosition == NULL || !SearchBack)									//以后添加检查与target的大小，判断是否到达搜索开头
		{
			free(pDocument);
			return FALSE;
		}
	}

	iPos = pPosition - pDocument;
	*SearchMark = iPos + wcslen(pfr->lpstrFindWhat);

	SendMessage(hwndedit,EM_SETSEL,iPos,*SearchMark);
	SendMessage(hwndedit,EM_SCROLLCARET,0,0);
	free(pDocument);
	return TRUE;
}

BOOL ReplaceDesignText(HWND hwndedit,int *SearchMark,LPFINDREPLACE pfr)
{
	int iStart, iEnd;
	PTSTR pDocument;

	SendMessage(hwndedit,EM_GETSEL,(WPARAM)&iStart,(LPARAM)&iEnd);
	if(iStart != iEnd)
	{
		if(NULL == (pDocument = (PTSTR)malloc( (iEnd+1)*sizeof(TCHAR) )))
			return FALSE;
		GetWindowText(hwndedit,pDocument,iEnd+1);

		if(!wcscmp(pDocument + iStart,pfr->lpstrFindWhat))
			SendMessage(hwndedit,EM_REPLACESEL,0,(LPARAM)pfr->lpstrReplaceWith);
	}

	SendMessage(hwndedit,EM_GETSEL,0,(LPARAM)SearchMark);
	if(!SearchText(hwndedit,SearchMark,pfr,TRUE))
		return FALSE;
	return TRUE;
}

BOOL ReplaceAllDesignText(HWND hwndedit, LPFINDREPLACE pfr)
{
	int iLength, iFindLen, iReplaceLen, iDstLen, sum = 0;
	TCHAR temp[MAX_PATH];
	PTSTR pBegin, pEnd, pDst,
		pBuffer = NULL,
		pDocument = NULL;

	iFindLen = wcslen(pfr->lpstrFindWhat);
	iReplaceLen = wcslen(pfr->lpstrReplaceWith);

	if (iLength = GetWindowTextLength(hwndedit))
	{	//内容不为空！
		iDstLen = iLength * (iReplaceLen > iFindLen ? (iFindLen + iReplaceLen - 1) / iFindLen : 1) + 1;

		if (NULL != (pBuffer = (PTSTR) malloc((iLength + 1)*sizeof(TCHAR))) &&
			NULL != (pDocument = (PTSTR) malloc(iDstLen * sizeof(TCHAR))))
		{
			GetWindowText(hwndedit, pBuffer, iLength + 1);
			pBegin = pEnd = pBuffer;
			pDst = pDocument;
			while (pEnd = wcsstr(pBegin, pfr->lpstrFindWhat))
			{
				for (; pBegin != pEnd; ++pBegin, ++pDst)
					*pDst = *pBegin;
				*pDst = TEXT('\0');
				wcscat_s(pDst,iDstLen-(pDst - pDocument), pfr->lpstrReplaceWith);
				pBegin += iFindLen;
				pDst += iReplaceLen;
				++sum;
			}
			while (*pDst++ = *pBegin++);

			SetWindowText(hwndedit, pDocument);
			free(pBuffer);
			free(pDocument);
		}
		else
		{
			if (pBuffer) free(pBuffer);
			if (pDocument) free(pDocument);
		}
	}

	if(!sum)
	{
		wsprintf(temp,TEXT("没找到“%s”！"),pfr->lpstrFindWhat);
		MessageBox(hDlgModeless,temp,TEXT("记事本"),MB_ICONWARNING);
		return FALSE;
	}
	else
	{
		wsprintf(temp,TEXT("共替换%d处！"),sum);
		MessageBox(hDlgModeless,temp,TEXT("记事本"),MB_ICONINFORMATION);
		return TRUE;
	}
}

//BOOL ReplaceAllDesignText(HWND hwndedit,LPFINDREPLACE pfr)
//{
//	int sum = 0,SearchMark = 0;
//	TCHAR temp[MAX_PATH];
//
//	for(;;)
//	{
//		if(SearchText(hwndedit,&SearchMark,pfr,FALSE))
//		{
//			SendMessage(hwndedit,EM_REPLACESEL,0,(LPARAM)pfr->lpstrReplaceWith);
//			sum++;
//		}
//		else
//			break;
//		SendMessage(hwndedit,EM_GETSEL,0,(LPARAM)&SearchMark);
//	}
//
//	if(!sum)
//	{
//		wsprintf(temp,TEXT("没找到“%s”！"),pfr->lpstrFindWhat);
//		MessageBox(hDlgModeless,temp,TEXT("记事本"),MB_ICONWARNING);
//		return FALSE;
//	}
//	else
//	{
//		wsprintf(temp,TEXT("共替换%d处！"),sum);
//		MessageBox(hDlgModeless,temp,TEXT("记事本"),MB_ICONINFORMATION);
//		return TRUE;
//	}
//}