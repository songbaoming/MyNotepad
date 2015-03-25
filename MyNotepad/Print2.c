/*-------------------------------------
	记事本打印输出模块
		宋保明，2014
 --------------------------------------
 改进方向：
	通过 GetTextExtentPoint32() ;
	计算需要打印的字符串的长度与打印机
	打印纸张宽度比较，决定打印字符数，
	再根据行内文本对齐方式进行打印
  -------------------------------------*/
#include <Windows.h>
#include <CommDlg.h>
#include <math.h>
#include <wchar.h>

void InitializePD(HINSTANCE hInst,HWND hwndedit ,LPPRINTDLG pd)
{
	pd->Flags				= PD_ALLPAGES | PD_COLLATE | PD_RETURNDC | PD_NOSELECTION;
	pd->hDC					= NULL;
	pd->hDevMode			= NULL;
	pd->hDevNames			= NULL;
	pd->hInstance			= hInst;
	pd->hPrintTemplate		= NULL;
	pd->hSetupTemplate		= NULL;
	pd->hwndOwner			= GetParent(hwndedit);
	pd->lCustData			= 0L;
	pd->lpfnPrintHook		= NULL;
	pd->lpfnSetupHook		= NULL;
	pd->lpPrintTemplateName	= NULL;
	pd->lpSetupTemplateName	= NULL;
	pd->lStructSize			= sizeof(PRINTDLG);
	pd->nCopies				= 1;
	pd->nFromPage			= 0;
	pd->nMaxPage			= 0;
	pd->nMinPage			= 0;
	pd->nToPage				= 0;
}

HFONT SetPrinterFont(HDC hdc)
{
	POINT pt;
	LOGFONT lf;

	pt.x = (int)((FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) * 105 / 72);
	pt.y = (int)((FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) * 105 / 72);
	DPtoLP(hdc,&pt,1);

	FillMemory(&lf,sizeof(LOGFONT),0);
	lf.lfHeight = -(int)(abs(pt.y)/10.0);
	lf.lfCharSet= DEFAULT_CHARSET;
	wcscpy(lf.lfFaceName,TEXT("宋体"));

	return CreateFontIndirect(&lf);
}

BOOL PrintFile(HINSTANCE hInst,HWND hwndedit ,PTSTR TitleName)
{
	static int cyChar;
	static PRINTDLG pd;
	static DOCINFO di = {sizeof(DOCINFO)};
	RECT rect;
	HFONT hfont;
	TEXTMETRIC txm;
	BOOL bSuccess = TRUE,bContinue;
	PTSTR pBuffer,pStart,pEnd,pPageStart;
	int iCharsPerLine,iLinesPerPage,iLength,iCopy,iRow,iNum,iSplitNum;

	InitializePD(hInst,hwndedit,&pd);
	if(!PrintDlg(&pd))
		return TRUE;

	iLength = GetWindowTextLength(hwndedit);
	pBuffer = (TCHAR *)malloc((iLength + 1) * sizeof(TCHAR));
	GetWindowText(hwndedit,pBuffer,iLength + 1);

	hfont = SetPrinterFont(pd.hDC);
	hfont = (HFONT)SelectObject(pd.hDC,hfont);

	GetTextMetrics(pd.hDC,&txm);
	cyChar = txm.tmHeight - txm.tmInternalLeading/*+ txm.tmExternalLeading*/;

	rect.left = rect.top = 0;
	rect.right = (int)(210.0 * GetDeviceCaps(pd.hDC,HORZRES) / GetDeviceCaps(pd.hDC,HORZSIZE));
	rect.bottom = (int)(297.0 * GetDeviceCaps(pd.hDC,VERTRES) / GetDeviceCaps(pd.hDC,VERTSIZE));
	iCharsPerLine = (rect.right) / txm.tmMaxCharWidth;
	iLinesPerPage = (rect.bottom) / cyChar;

	di.lpszDocName = TitleName[0] ? TitleName : TEXT("新文档");

	if(StartDoc(pd.hDC,&di) > 0)
		for(iCopy = 0;iCopy < (pd.Flags & PD_COLLATE ? pd.nCopies : 1);iCopy++) //逐份打印
		{
			for(bContinue = TRUE,pStart = pEnd = pBuffer;bContinue;)
			{
				pPageStart = pStart;
				for(iCopy = 0; iCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies);iCopy++) //逐页打印
				{
					if(StartPage(pd.hDC) < 0)
					{
						bSuccess = FALSE;
						break;
					}

					for(iRow = 0,bContinue = TRUE,pStart = pEnd = pPageStart;bContinue && iRow < iLinesPerPage;iRow += iNum)
					{
						while(*pEnd && *pEnd++ != TEXT('\n'));
						iLength = pEnd - pStart;
						
						iSplitNum = (int)ceil((double)iLength / iCharsPerLine);

						for(iNum = 0;iNum < iSplitNum;iNum++)
						{
							TextOut(pd.hDC,rect.left,(iRow + iNum) * cyChar + rect.top,
								pStart + iCharsPerLine * iNum,
								min(iCharsPerLine,iLength - iCharsPerLine * iNum));
						}
						if(!*(pStart =pEnd))
							bContinue = FALSE;
					}

					if(EndPage(pd.hDC) < 0)
					{
						bSuccess = FALSE;
						break;
					}
				}
				if(!bSuccess)
					break;
			}
			if(!bSuccess)
				break;
		}
	else
		bSuccess = FALSE;

	if(bSuccess)
			EndDoc(pd.hDC);

	free(pBuffer);
	DeleteObject(SelectObject(pd.hDC,hfont));
	DeleteDC(pd.hDC);
	return bSuccess;
}