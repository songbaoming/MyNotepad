#include "MyNotepad.h"

HWND hDlgModeless;
TCHAR szAppName[] = TEXT("���±�");

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmd,int nShow)
{
	HWND hwnd;
	HACCEL haccel;
	MSG msg;
	WNDCLASSEX wcls;

	wcls.cbSize = sizeof(wcls);
	wcls.style = CS_HREDRAW|CS_VREDRAW;
	wcls.lpfnWndProc = WndProc;
	wcls.hInstance = hInst;
	wcls.cbWndExtra = 0;
	wcls.cbClsExtra = 0;
	wcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcls.hIcon = LoadIcon(hInst,szAppName);
	wcls.hIconSm = LoadIcon(hInst,szAppName);
	wcls.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcls.lpszClassName = szAppName;
	wcls.lpszMenuName = szAppName;

	if(!RegisterClassEx(&wcls)){
		MessageBox(NULL,TEXT("ע�������"),TEXT("����"),MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,NULL,
		hInst,NULL);

	ShowWindow(hwnd,nShow);
	UpdateWindow(hwnd);

	haccel = LoadAccelerators(hInst,szAppName);

	while(GetMessage(&msg,NULL,0,0))
	{
		if(!hDlgModeless || !IsDialogMessage(hDlgModeless,&msg))
		{
			if(!TranslateAccelerator(hwnd,haccel,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static int SearchMark;
	static BOOL NeedSave = FALSE, bOpen = FALSE;
	static HINSTANCE hInst;
	static HWND hwndedit, hedit1, hedit2;
	static HMENU hmenu;
	static TCHAR FileName[MAX_PATH],TitleName[MAX_PATH];
	static UINT MessageFR;
	int	   SelBegin, SelEnd, Enable, ChangeLineChecked, iLength, iNumArgs;
	TCHAR temp[MAX_PATH], *pBuffer;
	SYSTEMTIME stm;
	RECT rect;
	LPFINDREPLACE pfr;
	PTSTR *ppArgv;

	switch(message)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		//�Զ������ı���
		hwndedit = hedit1 = CreateWindow(TEXT("edit"),NULL,
			WS_CHILD|WS_VISIBLE| WS_VSCROLL|ES_LEFT|
			ES_MULTILINE|ES_AUTOVSCROLL| ES_NOHIDESEL,
			0,0,0,0,hwnd,(HMENU)EDITID1,hInst,NULL);
		//�ֶ������ı���
		hedit2 = CreateWindow(TEXT("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
			ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL |
			WS_HSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
			0,0,0,0,hwnd,(HMENU)EDITID2,hInst,NULL);
		ChangeTitle(hwnd,TitleName,NeedSave);

		FontInitialize(hedit1);
		FontInitialize(hedit2);
		OFNInitialize(hwnd);

		MessageFR = RegisterWindowMessage(FINDMSGSTRING);
		hmenu = GetMenu(hwnd);

		ppArgv = CommandLineToArgvW(GetCommandLine(), &iNumArgs);
		if (iNumArgs > 1)
		{
			wcscpy_s(FileName, MAX_PATH, ppArgv[1]);
			wcscpy_s(TitleName, MAX_PATH, wcsrchr(ppArgv[1], TEXT('\\'))+1);
			bOpen = TRUE;
			SendMessage(hwnd, WM_COMMAND, IDM_OPEN, 0);
		}
		return 0;
	case WM_SIZE:
		MoveWindow(hwndedit,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hwndedit);
		return 0;
	case WM_INITMENUPOPUP:
		switch(LOWORD(lParam))
		{
		case 1:
			EnableMenuItem(hmenu,IDM_UNDO,
				SendMessage(hwndedit,EM_CANUNDO,0,0) ? MF_ENABLED : MF_DISABLED);
			EnableMenuItem(hmenu,IDM_PASTE,
				IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_DISABLED);

			SendMessage(hwndedit,EM_GETSEL,(WPARAM) &SelBegin,(LPARAM) &SelEnd);
			Enable = SelBegin != SelEnd ? MF_ENABLED : MF_DISABLED ;

			EnableMenuItem(hmenu,IDM_CUT,Enable);
			EnableMenuItem(hmenu,IDM_COPY,Enable);
			EnableMenuItem(hmenu,IDM_DEL,Enable);

		}
		return 0;
	case WM_COMMAND:
		if (lParam && (LOWORD(wParam) == EDITID1 || LOWORD(wParam) == EDITID2))//�����ı�����Ϣ
		{
			switch(HIWORD(wParam))
			{
			case EN_UPDATE:
				GetWindowText(hwndedit,temp,10);
				if(TitleName[0] || temp[0]){
					NeedSave = TRUE;
				}
				else
					NeedSave = FALSE;
				ChangeTitle(hwnd,TitleName,NeedSave);
				return 0;
			case EN_ERRSPACE:
			case EN_MAXTEXT:
				MessageBox(hwnd,TEXT("���±��Ѵ����������"),szAppName,MB_ICONSTOP);
				return 0;
			}
			break;
		}
		//����˵���Ϣ
		switch(LOWORD(wParam))
		{
		case IDM_NEW:
			if(NeedSave && AskAboutSave(hwnd))
				return 0;
			NeedSave = FALSE;
			TitleName[0] = TEXT('\0');
			SetWindowText(hwndedit,TEXT(""));
			ChangeTitle(hwnd,TitleName,NeedSave);
			return 0;
		case IDM_OPEN:
			if(NeedSave && AskAboutSave(hwnd))
				return 0;
			if(bOpen || FileOpenDlg(hwnd,FileName,TitleName))
			{
				if(!FileRead(hwndedit,FileName))
				{
					MessageBox(hwnd,TEXT("���ļ�ʧ�ܣ�"),szAppName,MB_ICONEXCLAMATION);
					FileName[0] = TitleName[0] = '\0';
				}
			bOpen = NeedSave = FALSE;
			ChangeTitle(hwnd,TitleName,NeedSave);
			}
			return 0;
		case IDM_SAVE:
			if(TitleName[0])
			{
				if(FileWrite(hwndedit,FileName))
				{
					NeedSave = FALSE;
					ChangeTitle(hwnd,TitleName,NeedSave);
					return 1;
				}
				else
				{
					MessageBox(hwnd,TEXT("�����ļ�ʧ�ܣ�"),szAppName,MB_ICONERROR);
					return 0;
				}
			}
		case IDM_SAVEAS:
			if(FileSaveDlg(hwnd,FileName,TitleName))
			{
				if(FileWrite(hwndedit,FileName))
				{
					NeedSave = FALSE;
					ChangeTitle(hwnd,TitleName,NeedSave);
					return 1;
				}
				else
				{
					MessageBox(hwnd,TEXT("�����ļ�ʧ�ܣ�"),szAppName,MB_ICONERROR);
					return 0;
				}
			}
			return 0;
		case IDM_PAGE:
			PageSetup(hInst, hwnd);
			return 0;
		case IDM_PRINT:
			if(!PrintFile(hInst,hwndedit,TitleName))
				MessageBeep(0);//
			return 0;
		case IDM_UNDO:
			SendMessage(hwndedit,WM_UNDO,0,0);
			return 0;
		case IDM_CUT:
			SendMessage(hwndedit,WM_CUT,0,0);
			return 0;
		case IDM_COPY:
			SendMessage(hwndedit,WM_COPY,0,0);
			return 0;
		case IDM_PASTE:
			SendMessage(hwndedit,WM_PASTE,0,0);
			return 0;
		case IDM_DEL:
			SendMessage(hwndedit,WM_CLEAR,0,0);
			return 0;
		case IDM_FIND:
			if(!hDlgModeless){
				hDlgModeless = FindDlg(hwnd);
			}
			else
				SetFocus(hDlgModeless);
			return 0;
		case IDM_REPLACE:
			if(!hDlgModeless){
				hDlgModeless = ReplaceDlg(hwnd);
			}
			else
				SetFocus(hDlgModeless);
			return 0;
		case IDM_SELECTALL:
			SendMessage(hwndedit,EM_SETSEL,0,-1);
			return 0;
		case IDM_DATETIME:
			GetLocalTime(&stm);
			wsprintf(temp,TEXT("%d/%d/%d %d:%d"),stm.wYear,stm.wMonth,stm.wDay,stm.wHour,stm.wMinute);
			SendMessage(hwndedit,EM_REPLACESEL,0,(LPARAM)temp);
			return 0;
		case IDM_AUTOCHANGELINE:
			ChangeLineChecked = !(GetMenuState(hmenu,IDM_AUTOCHANGELINE,MF_BYCOMMAND) & MF_CHECKED);
			GetClientRect(hwnd, &rect);
			iLength = GetWindowTextLength(hwndedit);

			if (iLength)
			{
				if ((pBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR))) != NULL)
				{
					GetWindowText(hwndedit, pBuffer, iLength + 1);
					SetWindowText(ChangeLineChecked ? hedit1 : hedit2, pBuffer);
					free(pBuffer);
				}
				else
				{
					MessageBox(hwnd, TEXT("����ʧ�ܣ������ڴ����"), szAppName, MB_ICONERROR);
					return 0;
				}	
			}
			ShowWindow(hwndedit, SW_HIDE);
			hwndedit = ChangeLineChecked ? hedit1 : hedit2;
			ShowWindow(hwndedit, SW_SHOW);
			MoveWindow(hwndedit,0,0,rect.right,rect.bottom,TRUE);
			SetFocus(hwndedit);

			CheckMenuItem(hmenu, IDM_AUTOCHANGELINE, ChangeLineChecked ? MF_CHECKED : MF_UNCHECKED);
			EnableMenuItem(hmenu, IDM_STATE, ChangeLineChecked ? MF_DISABLED : MF_ENABLED);

			return 0;
		case IDM_FONT:
			if (PopChooseFont(hwnd))
			{
				ChangeFont();
				SetFont(hedit1);
				SetFont(hedit2);
			}
			return 0;
		case IDM_STATE:
			//������״̬��
			break;
		case IDM_EXIT:
			SendMessage(hwnd,WM_CLOSE,0,0);
			return 0;
		case IDM_HELP:
			MessageBox(hwnd,TEXT("�����ļ����ڴ����С���"),szAppName,MB_ICONINFORMATION);
			return 0;
		case IDM_ABOUT:
			DialogBox(hInst,szAppName,hwnd,DlgProc);
			return 0;
		}
		MessageBeep(MB_ICONINFORMATION);//������
		break;
	case WM_QUERYENDSESSION:
		if(NeedSave && AskAboutSave(hwnd))
			return 0;
		return 1;
	case WM_CLOSE:
		if(NeedSave && AskAboutSave(hwnd))
			return 0;
		break;
	case WM_DESTROY:
		DeleteFont();
		PostQuitMessage(0);
		return 0 ;
	default:
		if(message == MessageFR)
		{
			pfr = (LPFINDREPLACE)lParam;

			if(pfr->Flags & FR_DIALOGTERM)
				hDlgModeless = NULL;
			if(pfr->Flags & FR_FINDNEXT)
			{
				SendMessage(hwndedit,EM_GETSEL,0,(LPARAM)&SearchMark);
				if(!SearchText(hwndedit,&SearchMark,pfr,TRUE))
				{
					wsprintf(temp,TEXT("û�ҵ���%s����"),pfr->lpstrFindWhat);
					MessageBox(hDlgModeless,temp,szAppName,MB_ICONWARNING);
				}
			}
			if(pfr->Flags & FR_REPLACE )
			{
				if(!ReplaceDesignText(hwndedit,&SearchMark,pfr))
				{
					wsprintf(temp,TEXT("û�ҵ���%s����"),pfr->lpstrFindWhat);
					MessageBox(hDlgModeless,temp,szAppName,MB_ICONWARNING);
				}
			}
			if(pfr->Flags & FR_REPLACEALL)
				ReplaceAllDesignText(hwndedit,pfr);
			return 0;
		}
		break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

BOOL CALLBACK DlgProc(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hdlg,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void ChangeTitle(HWND hwnd,TCHAR * TitleName,BOOL NeedSave)
{
	TCHAR temp[MAX_PATH];

	wsprintf(temp,TEXT("%s%s - %s"),TitleName[0]?TitleName:TEXT("���ĵ�"),
		NeedSave?TEXT("*"):TEXT(""),szAppName);
	SetWindowText(hwnd,temp);
	return ;
}

int AskAboutSave(HWND hwnd)
{
	int back;

	back = MessageBox(hwnd,TEXT("�ĵ������Ѹ��ģ�\n�Ƿ񱣴棿"),szAppName,MB_YESNOCANCEL | MB_ICONQUESTION);
	switch(back)
	{
	case IDYES:
		if(!SendMessage(hwnd,WM_COMMAND,IDM_SAVE,0))
			return IDCANCEL;
	case IDNO:
		return 0;
	case IDCANCEL:
		break;
	}
	return IDCANCEL;
}

//BOOL RestoreWindowState()
//{
//
//}
//
//void SaveWindowState()
//{
//	WINDOWPLACEMENT wp;
//	TCHAR szVersion[] = TEXT("Version 1.0");
//
//	
//}