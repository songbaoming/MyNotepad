/*-------------------------------------
	记事本打印输出模块
	宋保明，2014
	------------------------------------*/
#include <Windows.h>
#include <CommDlg.h>
#include <math.h>
#include <wchar.h>

extern LOGFONT logfont;
static RECT rectMargin = { 2000, 2500, 2000, 2500 };

void PageSetup(HINSTANCE hInst, HWND hwnd)
{
	PAGESETUPDLG psd = { sizeof(psd) };

	psd.hwndOwner = hwnd;
	psd.hInstance = hInst;
	psd.Flags = PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS;
	psd.rtMargin = rectMargin;

	if (PageSetupDlg(&psd))
		rectMargin = psd.rtMargin;
}

void InitializePD(HINSTANCE hInst, HWND hwndedit, LPPRINTDLG pd)
{
	pd->Flags = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC | PD_NOSELECTION;
	pd->hDC = NULL;
	pd->hDevMode = NULL;
	pd->hDevNames = NULL;
	pd->hInstance = hInst;
	pd->hPrintTemplate = NULL;
	pd->hSetupTemplate = NULL;
	pd->hwndOwner = GetParent(hwndedit);
	pd->lCustData = 0L;
	pd->lpfnPrintHook = NULL;
	pd->lpfnSetupHook = NULL;
	pd->lpPrintTemplateName = NULL;
	pd->lpSetupTemplateName = NULL;
	pd->lStructSize = sizeof(PRINTDLG);
	pd->nCopies = 1;
	pd->nFromPage = 0;
	pd->nMaxPage = 0;
	pd->nMinPage = 0;
	pd->nToPage = 0;
}

HFONT SetPrinterFont(HWND hwndedit, HDC hdcPrinter)
{
	LOGFONT lf;
	HDC hdc = GetDC(hwndedit);
	//pt.x = (int)((FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) * 105 / 72);
	//pt.y = (int)((FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) * 105 / 72);
	//DPtoLP(hdc,&pt,1);
	//FillMemory(&lf,sizeof(LOGFONT),0);
	//lf.lfHeight = -(int)(abs(pt.y)/10.0);
	//lf.lfCharSet= DEFAULT_CHARSET;
	//wcscpy_s(lf.lfFaceName,32, TEXT("宋体"));

	CopyMemory(&lf, &logfont, sizeof(LOGFONT));
	lf.lfHeight = lf.lfHeight * GetDeviceCaps(hdcPrinter, LOGPIXELSX) /
		GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(hwndedit, hdc);

	return CreateFontIndirect(&lf);
}

BOOL PrintFile(HINSTANCE hInst, HWND hwndedit, PTSTR TitleName)
{
	static int cyChar;
	static PRINTDLG pd;
	static DOCINFO di = { sizeof(DOCINFO) };
	SIZE size = { 0, 0 };
	RECT rect;
	HFONT hfont;
	BOOL bSuccess = TRUE, bContinue;
	PTSTR pBuffer, pStart, pEnd, pPageStart;
	int iLength, iCopy, iRow;
	double dPixelsPerMilli;

	InitializePD(hInst, hwndedit, &pd);
	if (!PrintDlg(&pd))
		return TRUE;

	iLength = GetWindowTextLength(hwndedit);
	pBuffer = (TCHAR *)malloc((iLength + 1) * sizeof(TCHAR));
	GetWindowText(hwndedit, pBuffer, iLength + 1);

	hfont = SetPrinterFont(hwndedit, pd.hDC);
	hfont = (HFONT)SelectObject(pd.hDC, hfont);

	dPixelsPerMilli = GetDeviceCaps(pd.hDC, LOGPIXELSX) / 25.4;
	rect.left = (LONG)(rectMargin.left * dPixelsPerMilli / 100);
	rect.top = (LONG)(rectMargin.top * dPixelsPerMilli / 100);
	rect.right = (LONG)(GetDeviceCaps(pd.hDC, HORZRES) - rectMargin.right * dPixelsPerMilli / 100);
	rect.bottom = (LONG)(GetDeviceCaps(pd.hDC, VERTRES) - rectMargin.bottom * dPixelsPerMilli / 100);
	//rect.right = (int)(210.0 * GetDeviceCaps(pd.hDC,HORZRES) / GetDeviceCaps(pd.hDC,HORZSIZE));
	//rect.bottom = (int)(297.0 * GetDeviceCaps(pd.hDC,VERTRES) / GetDeviceCaps(pd.hDC,VERTSIZE));

	di.lpszDocName = TitleName[0] ? TitleName : TEXT("新文档");

	if (StartDoc(pd.hDC, &di) > 0)
	for (iCopy = 0; iCopy < (pd.Flags & PD_COLLATE ? pd.nCopies : 1); iCopy++) //逐份打印
	{
		for (bContinue = TRUE, pStart = pEnd = pBuffer; bContinue;)
		{
			pPageStart = pStart;
			for (iCopy = 0; iCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies); iCopy++) //逐页打印
			{
				if (StartPage(pd.hDC) < 0)
				{
					bSuccess = FALSE;
					break;
				}

				for (iRow = rect.top, bContinue = TRUE, pStart = pEnd = pPageStart;
					bContinue && iRow + size.cy < rect.bottom; iRow += size.cy)
				{
					while (*pEnd && *pEnd++ != TEXT('\n'))
					{
						iLength = pEnd - pStart;
						GetTextExtentPoint32(pd.hDC, pStart, iLength, &size);
						if (size.cx >(rect.right - rect.left))
						{
							pEnd--;
							iLength = pEnd - pStart;
							break;
						}
					}
					TextOut(pd.hDC, rect.left, iRow, pStart, iLength);
					if (!*(pStart = pEnd))
						bContinue = FALSE;
				}

				if (EndPage(pd.hDC) < 0)
				{
					bSuccess = FALSE;
					break;
				}
			}
			if (!bSuccess)
				break;
		}
		if (!bSuccess)
			break;
	}
	else
		bSuccess = FALSE;

	if (bSuccess)
		EndDoc(pd.hDC);

	free(pBuffer);
	DeleteObject(SelectObject(pd.hDC, hfont));
	DeleteDC(pd.hDC);
	return bSuccess;
}