#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <commctrl.h>
#include <richedit.h>
#include "Scripts.h"


//Include AEC functions
#define AEC_FUNCTIONS
#include "AkelEdit.h"

//Include stack functions
#define StackInsertAfter
#define StackInsertBefore
#define StackInsertIndex
#define StackDelete
#define StackJoin
#define StackClear
#include "StackFunc.h"

//Include string functions
#define WideCharLower
#define xmemcpy
#define xmemcmp
#define xmemset
#define xstrcmpiW
#define xstrcmpinA
#define xstrcmpinW
#define xstrlenA
#define xstrlenW
#define xstrcpyW
#define xstrcpynW
#define xatoiW
#define xitoaW
#define xuitoaW
#define dec2hexW
#define hex2decW
#define xprintfW
#define xstrstrW
#include "StrFunc.h"

//Include wide functions
#define AppendMenuWide
#define CallWindowProcWide
#define CreateFileWide
#define CreateProcessWide
#define CreateWindowExWide
#define DefWindowProcWide
#define DialogBoxParamWide
#define DialogBoxWide
#define DispatchMessageWide
#define ExpandEnvironmentStringsWide
#define FileExistsAnsi
#define FileExistsWide
#define FindFirstFileWide
#define FindNextFileWide
#define GetFileAttributesWide
#define GetKeyNameTextWide
#define GetMessageWide
#define GetModuleFileNameWide
#define GetModuleHandleWide
#define GetWindowLongPtrWide
#define GetWindowTextWide
#define IsDialogMessageWide
#define ListView_GetItemWide
#define ListView_InsertColumnWide
#define ListView_InsertItemWide
#define ListView_SetItemWide
#define LoadLibraryWide
#define RegisterClassWide
#define SetDlgItemTextWide
#define SetFileAttributesWide
#define SetWindowLongPtrWide
#define SetWindowTextWide
#define ShellExecuteWide
#define TranslateAcceleratorWide
#define UnregisterClassWide
#include "WideFunc.h"

//Global variables
char szBuffer[BUFFER_SIZE];
wchar_t wszBuffer[BUFFER_SIZE];
wchar_t wszPluginName[MAX_PATH];
wchar_t wszPluginTitle[MAX_PATH];
HINSTANCE hInstanceEXE=NULL;
HINSTANCE hInstanceDLL=NULL;
HWND hMainWnd=NULL;
HACCEL hGlobalAccel;
BOOL bOldWindows;
BOOL bOldRichEdit;
BOOL bOldComctl32;
BOOL bAkelEdit;
int nMDI;
LANGID wLangModule;
HICON g_hPluginIcon=NULL;
BOOL g_MainMessageBox=FALSE;
BOOL bInitCommon=FALSE;
BOOL bInitMain=FALSE;
BOOL bMainOnFinish=FALSE;
DWORD dwSaveFlags=0;
HWND hWndMainDlg=NULL;
HWND hWndScriptsList=NULL;
WNDPROC lpOldFilterProc=NULL;
RECT rcMainMinMaxDialog={253, 337, 0, 0};
RECT rcMainCurrentDialog={0};
LISTVIEWCOLUMN lpColumns[]={{LVI_SCRIPT,      163, LVCOLF_VISIBLE},
                            {LVI_VERSION,     70,  LVCOLF_CONTENT},
                            {LVI_HOTKEY,      109, LVCOLF_VISIBLE},
                            {LVI_STATUS,      70,  LVCOLF_VISIBLE},
                            {LVI_AUTHOR,      70,  LVCOLF_CONTENT},
                            {LVI_DESCRIPTION, 300, LVCOLF_CONTENT},
                            {LVI_SITE,        70, LVCOLF_CONTENT},
                            {-1, 0, 0}};
int nSortColumn=LVI_SCRIPT;
BOOL bOpeningDlg=FALSE;
BOOL bContentFilterEnable=FALSE;
int nContentBuffer=512;
DWORD dwGlobalDebugJIT=JIT_FROMSTART;
BOOL bGlobalDebugEnable=FALSE;
DWORD dwGlobalDebugCode=0;
const char *pMutexInitName="AkelPad::Scripts::MutexInit";
const char *pMutexExecName="AkelPad::Scripts::MutexExec";
wchar_t wszScriptsDir[MAX_PATH];
wchar_t wszAkelPadDir[MAX_PATH];
wchar_t wszErrorMsg[BUFFER_SIZE]=L"";
wchar_t wszLastScript[MAX_PATH]=L"";
wchar_t wszFilter[MAX_PATH]=L"";
wchar_t wszContentFilter[MAX_PATH]=L"";
HTHREADSTACK hThreadStack={0};
SCRIPTTHREAD *lpScriptThreadActiveX=NULL;
HANDLE hExecThread=NULL;
DWORD dwExecThreadId=0;
UINT_PTR dwWaitScriptTimerId=0;
UINT_PTR dwWaitScriptTimerStart=0;
WNDPROCDATA *NewMainProcData=NULL;

//Identification
void __declspec(dllexport) DllAkelPadID(PLUGINVERSION *pv)
{
  pv->dwAkelDllVersion=AKELDLL;
  pv->dwExeMinVersion3x=MAKE_IDENTIFIER(-1, -1, -1, -1);
  pv->dwExeMinVersion4x=MAKE_IDENTIFIER(4, 8, 8, 0);
  pv->pPluginName="Scripts";
}

//Plugin extern function
void __declspec(dllexport) Main(PLUGINDATA *pd)
{
  pd->dwSupport|=PDS_SUPPORTALL;
  if (pd->dwSupport & PDS_GETSUPPORT)
    return;

  if (!bInitCommon) InitCommon(pd);
  if (!bInitMain) InitMain();

  if (pd->lParam)
  {
    INT_PTR nAction=GetExtCallParam(pd->lParam, 1);

    if (nAction == DLLA_SCRIPTS_EXEC ||
        nAction == DLLA_SCRIPTS_EXECWAIT ||
        nAction == DLLA_SCRIPTS_EDIT)
    {
      unsigned char *pScript=NULL;
      unsigned char *pArguments=NULL;
      wchar_t *wpScript=NULL;
      wchar_t *wpArguments=NULL;
      int nScriptLen=0;
      int nArgumentsLen=0;
      BOOL bWait=FALSE;

      if (nAction == DLLA_SCRIPTS_EXECWAIT)
        bWait=TRUE;

      if (IsExtCallParamValid(pd->lParam, 2))
        pScript=(unsigned char *)GetExtCallParam(pd->lParam, 2);
      if (IsExtCallParamValid(pd->lParam, 3))
        pArguments=(unsigned char *)GetExtCallParam(pd->lParam, 3);

      if (pScript)
      {
        if (pd->dwSupport & PDS_STRANSI)
        {
          if (nScriptLen=(int)xstrlenA((char *)pScript))
          {
            if (wpScript=(wchar_t *)GlobalAlloc(GPTR, (nScriptLen + 1) * sizeof(wchar_t)))
              MultiByteToWideChar(CP_ACP, 0, (char *)pScript, nScriptLen + 1, wpScript, nScriptLen + 1);
          }
          if (nArgumentsLen=(int)xstrlenA((char *)pArguments))
          {
            if (wpArguments=(wchar_t *)GlobalAlloc(GPTR, (nArgumentsLen + 1) * sizeof(wchar_t)))
              MultiByteToWideChar(CP_ACP, 0, (char *)pArguments, nArgumentsLen + 1, wpArguments, nArgumentsLen + 1);
          }
        }
        else
        {
          wpScript=(wchar_t *)pScript;
          wpArguments=(wchar_t *)pArguments;
        }
      }
      if ((!wpScript || !*wpScript) && *wszLastScript)
        wpScript=wszLastScript;

      if (wpScript && *wpScript)
      {
        if (nAction == DLLA_SCRIPTS_EDIT)
          EditScript(wpScript);
        else
          ExecScript(wpScript, wpArguments, bWait);

        //if (xstrcmpiW(wpScript, wszLastScript))
        //{
        //  xstrcpynW(wszLastScript, wpScript, MAX_PATH);
        //  SaveOptions(OF_LASTSCRIPT);
        //}
      }

      if (nScriptLen) GlobalFree((HGLOBAL)wpScript);
      if (nArgumentsLen) GlobalFree((HGLOBAL)wpArguments);
    }
  }
  else
  {
    if (!pd->bOnStart)
    {
      DialogBoxWide(hInstanceDLL, MAKEINTRESOURCEW(IDD_SCRIPTS), hMainWnd, (DLGPROC)MainDlgProc);
    }
  }

  //Stay in memory, and show as active
  pd->nUnload=UD_NONUNLOAD_ACTIVE;
}

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static HWND hWndScriptsFilter;
  static HWND hWndScriptsContentFilterEnable;
  static HWND hWndScriptsContentFilterButton;
  static HWND hWndChangeListGroup;
  static HWND hWndExecButton;
  static HWND hWndEditButton;
  static HWND hWndHotkey;
  static HWND hWndAssignButton;
  static HWND hWndDebugGroup;
  static HWND hWndDebugJITCheck;
  static HWND hWndDebugJITFromStartCheck;
  static HWND hWndDebugCodeCheck;
  static HWND hWndDebugCodeButton;
  static HWND hWndCloseButton;
  static HMENU hMenuList;
  static int nSelItem=-1;
  static BOOL bListChanged=FALSE;
  static DIALOGRESIZE drs[]={{&hWndScriptsList,                DRS_SIZE|DRS_X, 0},
                             {&hWndScriptsList,                DRS_SIZE|DRS_Y, 0},
                             {&hWndScriptsFilter,              DRS_SIZE|DRS_X, 0},
                             {&hWndScriptsFilter,              DRS_MOVE|DRS_Y, 0},
                             {&hWndScriptsContentFilterEnable, DRS_MOVE|DRS_X, 0},
                             {&hWndScriptsContentFilterEnable, DRS_MOVE|DRS_Y, 0},
                             {&hWndScriptsContentFilterButton, DRS_MOVE|DRS_X, 0},
                             {&hWndScriptsContentFilterButton, DRS_MOVE|DRS_Y, 0},
                             {&hWndChangeListGroup,            DRS_MOVE|DRS_X, 0},
                             {&hWndExecButton,                 DRS_MOVE|DRS_X, 0},
                             {&hWndEditButton,                 DRS_MOVE|DRS_X, 0},
                             {&hWndHotkey,                     DRS_MOVE|DRS_X, 0},
                             {&hWndAssignButton,               DRS_MOVE|DRS_X, 0},
                             {&hWndDebugGroup,                 DRS_MOVE|DRS_X, 0},
                             {&hWndDebugJITCheck,              DRS_MOVE|DRS_X, 0},
                             {&hWndDebugJITFromStartCheck,     DRS_MOVE|DRS_X, 0},
                             {&hWndDebugCodeCheck,             DRS_MOVE|DRS_X, 0},
                             {&hWndDebugCodeButton,            DRS_MOVE|DRS_X, 0},
                             {&hWndCloseButton,                DRS_MOVE|DRS_X, 0},
                             {&hWndCloseButton,                DRS_MOVE|DRS_Y, 0},
                             {0, 0, 0}};

  if (uMsg == WM_INITDIALOG)
  {
    bOpeningDlg=TRUE;
    hWndMainDlg=hDlg;

    SendMessage(hDlg, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)g_hPluginIcon);
    hWndScriptsList=GetDlgItem(hDlg, IDC_SCRIPTS_LIST);
    hWndScriptsFilter=GetDlgItem(hDlg, IDC_SCRIPTS_FILTER);
    hWndScriptsContentFilterEnable=GetDlgItem(hDlg, IDC_SCRIPTS_CONTENTFILTER_ENABLE);
    hWndScriptsContentFilterButton=GetDlgItem(hDlg, IDC_SCRIPTS_CONTENTFILTER_BUTTON);
    hWndChangeListGroup=GetDlgItem(hDlg, IDC_CHANGELIST_GROUP);
    hWndExecButton=GetDlgItem(hDlg, IDC_EXEC);
    hWndEditButton=GetDlgItem(hDlg, IDC_EDIT);
    hWndHotkey=GetDlgItem(hDlg, IDC_HOTKEY);
    hWndAssignButton=GetDlgItem(hDlg, IDC_ASSIGN);
    hWndDebugGroup=GetDlgItem(hDlg, IDC_DEBUG_GROUP);
    hWndDebugJITCheck=GetDlgItem(hDlg, IDC_DEBUG_JIT_CHECK);
    hWndDebugJITFromStartCheck=GetDlgItem(hDlg, IDC_DEBUG_JITFROMSTART_CHECK);
    hWndDebugCodeCheck=GetDlgItem(hDlg, IDC_DEBUG_CODE_CHECK);
    hWndDebugCodeButton=GetDlgItem(hDlg, IDC_DEBUG_CODE_BUTTON);
    hWndCloseButton=GetDlgItem(hDlg, IDC_CLOSE);

    SetWindowTextWide(hDlg, wszPluginTitle);
    SetDlgItemTextWide(hDlg, IDC_EXEC, GetLangStringW(wLangModule, STRID_EXEC));
    SetDlgItemTextWide(hDlg, IDC_EDIT, GetLangStringW(wLangModule, STRID_EDIT));
    SetDlgItemTextWide(hDlg, IDC_ASSIGN, GetLangStringW(wLangModule, STRID_ASSIGN));
    SetDlgItemTextWide(hDlg, IDC_DEBUG_GROUP, GetLangStringW(wLangModule, STRID_DEBUG));
    SetDlgItemTextWide(hDlg, IDC_DEBUG_JIT_CHECK, GetLangStringW(wLangModule, STRID_JIT));
    SetDlgItemTextWide(hDlg, IDC_DEBUG_JITFROMSTART_CHECK, GetLangStringW(wLangModule, STRID_FROMSTART));
    SetDlgItemTextWide(hDlg, IDC_CLOSE, GetLangStringW(wLangModule, STRID_CLOSE));

    EnableWindow(hWndExecButton, FALSE);
    EnableWindow(hWndEditButton, FALSE);
    EnableWindow(hWndHotkey, FALSE);
    EnableWindow(hWndAssignButton, FALSE);
    SendMessage(hWndScriptsList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);
    SendMessage(hMainWnd, AKD_SETHOTKEYINPUT, (WPARAM)hWndHotkey, 0);

    if (bContentFilterEnable)
      SendMessage(hWndScriptsContentFilterEnable, BM_SETCHECK, BST_CHECKED, 0);
    else
      EnableWindow(hWndScriptsContentFilterButton, FALSE);
    //Set button text
    xprintfW(wszBuffer, L"%s%s", GetLangStringW(wLangModule, STRID_CONTAIN), *wszContentFilter?L" *":L"");
    SetWindowTextWide(hWndScriptsContentFilterButton, wszBuffer);

    if (dwGlobalDebugJIT & JIT_DEBUG)
      SendMessage(hWndDebugJITCheck, BM_SETCHECK, BST_CHECKED, 0);
    else
      EnableWindow(hWndDebugJITFromStartCheck, FALSE);
    if (dwGlobalDebugJIT & JIT_FROMSTART)
      SendMessage(hWndDebugJITFromStartCheck, BM_SETCHECK, BST_CHECKED, 0);
    if (bGlobalDebugEnable)
      SendMessage(hWndDebugCodeCheck, BM_SETCHECK, BST_CHECKED, 0);
    else
      EnableWindow(hWndDebugCodeButton, FALSE);
    xprintfW(wszBuffer, L"0x%x", dwGlobalDebugCode);
    SetWindowTextWide(hWndDebugCodeButton, wszBuffer);

    if (hMenuList=CreatePopupMenu())
    {
      AppendMenuWide(hMenuList, MF_STRING, IDC_SCRIPTS_OPENSITE, GetLangStringW(wLangModule, STRID_MENU_OPENSITE));
    }

    CreateColumns(hWndScriptsList);

    SetWindowTextWide(hWndScriptsFilter, wszFilter);
    FillScriptList(hWndScriptsList, wszFilter, wszContentFilter);

    lpOldFilterProc=(WNDPROC)GetWindowLongPtrWide(hWndScriptsFilter, GWLP_WNDPROC);
    SetWindowLongPtrWide(hWndScriptsFilter, GWLP_WNDPROC, (UINT_PTR)NewFilterProc);

    bOpeningDlg=FALSE;
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    if ((HWND)wParam == hWndScriptsList)
    {
      LVHITTESTINFO lvhti;
      LVITEMW lvi;
      HWND hWndHeader;
      POINT ptScreen={0};
      POINT ptClient;
      RECT rcItem;
      BOOL bShowMenu=TRUE;

      if (lParam == -1)
      {
        if ((lvhti.iItem=nSelItem) != -1)
        {
          rcItem.left=LVIR_LABEL;
          SendMessage(hWndScriptsList, LVM_GETITEMRECT, (WPARAM)nSelItem, (LPARAM)&rcItem);
          ptScreen.x=rcItem.left;
          ptScreen.y=rcItem.bottom;
        }
        ClientToScreen(hWndScriptsList, &ptScreen);
      }
      else
      {
        GetCursorPos(&ptScreen);
        ptClient=ptScreen;
        ScreenToClient(hWndScriptsList, &ptClient);

        if ((hWndHeader=ChildWindowFromPoint(hWndScriptsList, ptClient)) && hWndHeader != hWndScriptsList)
        {
          if (DialogBoxWide(hInstanceDLL, MAKEINTRESOURCEW(IDD_COLUMNS), hDlg, (DLGPROC)ColumnsDlgProc))
          {
            CreateColumns(hWndScriptsList);
            FillScriptList(hWndScriptsList, wszFilter, wszContentFilter);
            dwSaveFlags|=OF_COLUMNS;
          }
          bShowMenu=FALSE;

          ////Get column index
          //HDHITTESTINFO hdhti;
          //
          //hdhti.pt=ptScreen;
          //ScreenToClient(hWndHeader, &hdhti.pt);
          //SendMessage(hWndHeader, HDM_HITTEST, 0, (LPARAM)&hdhti);
        }
        else
        {
          lvhti.pt=ptClient;
          SendMessage(hWndScriptsList, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvhti);

          lvi.stateMask=LVIS_SELECTED;
          lvi.state=LVIS_SELECTED;
          SendMessage(hWndScriptsList, LVM_SETITEMSTATE, (WPARAM)lvhti.iItem, (LPARAM)&lvi);
        }
      }

      if (bShowMenu)
      {
        LISTVIEWITEMPARAM *lpItemParam;

        lpItemParam=GetItemParam(hWndScriptsList, nSelItem);
        EnableMenuItem(hMenuList, IDC_SCRIPTS_OPENSITE, (lpItemParam && lpItemParam->wpSite)?MF_ENABLED:MF_GRAYED);
        TrackPopupMenu(hMenuList, TPM_LEFTBUTTON|TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, 0, hDlg, NULL);
      }
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (wParam == IDC_SCRIPTS_LIST)
    {
      NMLISTVIEW *pnmlv=(NMLISTVIEW *)lParam;

      if (pnmlv->hdr.code == LVN_ITEMCHANGED)
      {
        LISTVIEWITEMPARAM *lpItemParam=(LISTVIEWITEMPARAM *)pnmlv->lParam;

        if (pnmlv->uNewState & LVIS_SELECTED)
        {
          nSelItem=pnmlv->iItem;
          SendMessage(hWndHotkey, HKM_SETHOTKEY, lpItemParam->wHotkey, 0);

          EnableWindow(hWndExecButton, TRUE);
          EnableWindow(hWndEditButton, TRUE);
          EnableWindow(hWndHotkey, TRUE);
          EnableWindow(hWndAssignButton, TRUE);
        }
        if (pnmlv->uOldState & LVIS_SELECTED)
        {
          nSelItem=-1;
          SendMessage(hWndHotkey, HKM_SETHOTKEY, 0, 0);

          EnableWindow(hWndExecButton, FALSE);
          EnableWindow(hWndEditButton, FALSE);
          EnableWindow(hWndHotkey, FALSE);
          EnableWindow(hWndAssignButton, FALSE);
        }
      }
      else if (((NMHDR *)lParam)->code == LVN_COLUMNCLICK)
      {
        LISTVIEWCOLUMN *lpColumn;

        if (lpColumn=GetColumnByIndex(pnmlv->iSubItem))
        {
          nSortColumn=lpColumn->nID;
          SendMessage(hWndScriptsList, LVM_SORTITEMS, (LPARAM)lpColumn, (LPARAM)CompareFunc);
        }
      }
      else if (((NMHDR *)lParam)->code == (UINT)NM_DBLCLK)
      {
        PostMessage(hDlg, WM_COMMAND, IDC_EXEC, 0);
      }
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    if (LOWORD(wParam) == IDC_SCRIPTS_OPENSITE)
    {
      LISTVIEWITEMPARAM *lpItemParam;

      if (lpItemParam=GetItemParam(hWndScriptsList, nSelItem))
        ShellExecuteWide(hDlg, L"open", lpItemParam->wpSite, NULL, NULL, SW_MAXIMIZE);
    }
    else if (LOWORD(wParam) == IDC_SCRIPTS_FILTER)
    {
      if (HIWORD(wParam) == EN_CHANGE)
      {
        if (!bOpeningDlg)
        {
          SendMessage(hWndScriptsContentFilterEnable, BM_SETCHECK, BST_UNCHECKED, 0);
          bContentFilterEnable=FALSE;
          EnableWindow(hWndScriptsContentFilterButton, bContentFilterEnable);

          GetWindowTextWide(hWndScriptsFilter, wszFilter, MAX_PATH);
          FillScriptList(hWndScriptsList, wszFilter, NULL);
        }
      }
    }
    else if (LOWORD(wParam) == IDC_SCRIPTS_CONTENTFILTER_ENABLE)
    {
      bContentFilterEnable=(BOOL)SendMessage(hWndScriptsContentFilterEnable, BM_GETCHECK, 0, 0);
      EnableWindow(hWndScriptsContentFilterButton, bContentFilterEnable);
      if (*wszContentFilter) FillScriptList(hWndScriptsList, wszFilter, wszContentFilter);
    }
    else if (LOWORD(wParam) == IDC_SCRIPTS_CONTENTFILTER_BUTTON)
    {
      VARIANT vtResult;

      Document_InputBox(NULL, hDlg, (BSTR)GetLangStringW(wLangModule, STRID_CONTAIN), (BSTR)L"", wszContentFilter, &vtResult);

      if (vtResult.vt == VT_BSTR)
      {
        xstrcpynW(wszContentFilter, vtResult.bstrVal, MAX_PATH);
        SysFreeString(vtResult.bstrVal);
        FillScriptList(hWndScriptsList, wszFilter, wszContentFilter);

        //Set button text
        xprintfW(wszBuffer, L"%s%s", GetLangStringW(wLangModule, STRID_CONTAIN), *wszContentFilter?L" *":L"");
        SetWindowTextWide(hWndScriptsContentFilterButton, wszBuffer);
      }
    }
    else if (LOWORD(wParam) == IDC_ASSIGN)
    {
      LISTVIEWITEMPARAM *lpItemParam;
      WORD wHotkey;
      wchar_t wszHotkey[MAX_PATH];
      int nColumnIndex;

      lpItemParam=GetItemParam(hWndScriptsList, nSelItem);

      //Get script hotkey
      wHotkey=(WORD)SendMessage(hWndHotkey, HKM_GETHOTKEY, 0, 0);

      //Register hotkey
      if (RegisterHotkey(lpItemParam->wpScript, wHotkey))
      {
        if (GetColumnByID(LVI_HOTKEY, &nColumnIndex))
        {
          LVITEMW lvi;

          GetHotkeyString(wHotkey, wszHotkey);
          lpItemParam->wHotkey=wHotkey;
          CopyWideStr(wszHotkey, -1, &lpItemParam->wpHotkey);
          lvi.mask=LVIF_TEXT;
          lvi.pszText=lpItemParam->wpHotkey;
          lvi.iItem=nSelItem;
          lvi.iSubItem=nColumnIndex;
          ListView_SetItemWide(hWndScriptsList, &lvi);

          SendMessage(hWndHotkey, HKM_SETHOTKEY, wHotkey, 0);
          bListChanged=TRUE;
        }
      }
      else SetFocus(hWndHotkey);

      return TRUE;
    }
    else if (LOWORD(wParam) == IDC_DEBUG_JIT_CHECK)
    {
      BOOL bState;

      bState=(BOOL)SendMessage(hWndDebugJITCheck, BM_GETCHECK, 0, 0);
      EnableWindow(hWndDebugJITFromStartCheck, bState);
    }
    else if (LOWORD(wParam) == IDC_DEBUG_CODE_CHECK)
    {
      BOOL bState;

      bState=(BOOL)SendMessage(hWndDebugCodeCheck, BM_GETCHECK, 0, 0);
      EnableWindow(hWndDebugCodeButton, bState);
    }
    else if (LOWORD(wParam) == IDC_DEBUG_CODE_BUTTON)
    {
      DWORD dwDebugCode=dwGlobalDebugCode;

      if (DialogBoxParamWide(hInstanceDLL, MAKEINTRESOURCEW(IDD_CODE), hDlg, (DLGPROC)CodeDlgProc, (LPARAM)&dwDebugCode))
      {
        if (dwGlobalDebugCode != dwDebugCode)
        {
          dwGlobalDebugCode=dwDebugCode;
          dwSaveFlags|=OF_DEBUG;

          xprintfW(wszBuffer, L"0x%x", dwGlobalDebugCode);
          SetWindowTextWide(hWndDebugCodeButton, wszBuffer);
        }
      }
    }
    else if (LOWORD(wParam) == IDC_EXEC ||
             LOWORD(wParam) == IDC_EDIT ||
             LOWORD(wParam) == IDC_CLOSE ||
             LOWORD(wParam) == IDCANCEL)
    {
      LISTVIEWCOLUMN *lpColumnCount;
      LISTVIEWITEMPARAM *lpItemParam;
      BOOL bDebugEnable;
      DWORD dwDebugFlags;
      int nWidth;
      int nColumnIndex=0;

      //Get columns width
      for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
      {
        if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
        {
          nWidth=(int)SendMessage(hWndScriptsList, LVM_GETCOLUMNWIDTH, nColumnIndex++, 0);
          if (lpColumnCount->nWidth != nWidth)
          {
            lpColumnCount->nWidth=nWidth;
            dwSaveFlags|=OF_RECT;
          }
        }
      }

      dwDebugFlags=0;
      if (SendMessage(hWndDebugJITCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugFlags|=JIT_DEBUG;
      if (SendMessage(hWndDebugJITFromStartCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugFlags|=JIT_FROMSTART;
      if (dwGlobalDebugJIT != dwDebugFlags)
      {
        dwGlobalDebugJIT=dwDebugFlags;
        dwSaveFlags|=OF_DEBUG;
      }

      bDebugEnable=(BOOL)SendMessage(hWndDebugCodeCheck, BM_GETCHECK, 0, 0);
      if (bGlobalDebugEnable != bDebugEnable)
      {
        bGlobalDebugEnable=bDebugEnable;
        dwSaveFlags|=OF_DEBUG;
      }

      if (bListChanged || LOWORD(wParam) == IDC_CLOSE)
      {
        dwSaveFlags|=OF_LASTSCRIPT|OF_HOTKEYS;
      }
      if (LOWORD(wParam) == IDC_EXEC ||
          LOWORD(wParam) == IDC_EDIT)
      {
        if (lpItemParam=GetItemParam(hWndScriptsList, nSelItem))
        {
          xstrcpynW(wszLastScript, lpItemParam->wpScript, MAX_PATH);
          dwSaveFlags|=OF_LASTSCRIPT;
        }
      }
      if (dwSaveFlags)
      {
        SaveOptions(dwSaveFlags);
        dwSaveFlags=0;
      }

      //Free items data
      FreeScriptList(hWndScriptsList);

      //End dialog
      EndDialog(hDlg, 0);

      //Execute
      if (LOWORD(wParam) == IDC_EXEC)
        ExecScript(wszLastScript, NULL, FALSE);
      else if (LOWORD(wParam) == IDC_EDIT)
      {
        EditScript(wszLastScript);

        //Find text, if content filter exist
        if (bContentFilterEnable && *wszContentFilter)
        {
          INT_PTR nResult;
          BSTR wszContentFilterSys;

          if (wszContentFilterSys=SysAllocString(wszContentFilter))
          {
            Document_TextFind(NULL, NULL, wszContentFilterSys, FRF_BEGINNING|FRF_DOWN, &nResult);
            SysFreeString(wszContentFilterSys);
          }
        }
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_CLOSE)
  {
    PostMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
    return TRUE;
  }
  else if (uMsg == WM_DESTROY)
  {
    //Destroy resources
    DestroyMenu(hMenuList);
  }

  //Dialog resize messages
  {
    DIALOGRESIZEMSG drsm={&drs[0], &rcMainMinMaxDialog, &rcMainCurrentDialog, DRM_PAINTSIZEGRIP, hDlg, uMsg, wParam, lParam};

    if (SendMessage(hMainWnd, AKD_DIALOGRESIZE, 0, (LPARAM)&drsm))
      dwSaveFlags|=OF_RECT;
  }

  return FALSE;
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  LISTVIEWITEMPARAM *lpItemParam1=(LISTVIEWITEMPARAM *)lParam1;
  LISTVIEWITEMPARAM *lpItemParam2=(LISTVIEWITEMPARAM *)lParam2;
  LISTVIEWCOLUMN *lpColumn=(LISTVIEWCOLUMN *)lParamSort;

  if (lpColumn->nID == LVI_SCRIPT)
  {
    return xstrcmpiW(lpItemParam1->wpScript, lpItemParam2->wpScript);
  }
  if (lpColumn->nID == LVI_HOTKEY)
  {
    return xstrcmpiW(lpItemParam1->wpHotkey, lpItemParam2->wpHotkey);
  }
  if (lpColumn->nID == LVI_STATUS)
  {
    return xstrcmpiW(lpItemParam1->wpStatus, lpItemParam2->wpStatus);
  }
  if (lpColumn->nID == LVI_VERSION)
  {
    return xstrcmpiW(lpItemParam1->wpVersion, lpItemParam2->wpVersion);
  }
  if (lpColumn->nID == LVI_AUTHOR)
  {
    return xstrcmpiW(lpItemParam1->wpAuthor, lpItemParam2->wpAuthor);
  }
  if (lpColumn->nID == LVI_DESCRIPTION)
  {
    return xstrcmpiW(lpItemParam1->wpDescription, lpItemParam2->wpDescription);
  }
  if (lpColumn->nID == LVI_SITE)
  {
    return xstrcmpiW(lpItemParam1->wpSite, lpItemParam2->wpSite);
  }
  return 0;
}

BOOL CALLBACK ColumnsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static HWND hWndList;
  static HWND hWndBufferLabel;
  static HWND hWndBufferEdit;
  static HWND hWndBufferSpin;
  static HWND hWndOK;
  static HWND hWndCancel;
  static HMENU hMenuList;
  static int nSelItem;
  static LISTVIEWCOLUMN *lpColumnsDlg;

  if (uMsg == WM_INITDIALOG)
  {
    SendMessage(hDlg, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)g_hPluginIcon);
    hWndList=GetDlgItem(hDlg, IDC_COLUMNS_LIST);
    hWndBufferLabel=GetDlgItem(hDlg, IDC_COLUMNS_BUFFER_LABEL);
    hWndBufferEdit=GetDlgItem(hDlg, IDC_COLUMNS_BUFFER_EDIT);
    hWndBufferSpin=GetDlgItem(hDlg, IDC_COLUMNS_BUFFER_SPIN);
    hWndOK=GetDlgItem(hDlg, IDOK);
    hWndCancel=GetDlgItem(hDlg, IDCANCEL);

    SetWindowTextWide(hDlg, GetLangStringW(wLangModule, STRID_COLUMNS));
    SetDlgItemTextWide(hDlg, IDC_COLUMNS_BUFFER_LABEL, GetLangStringW(wLangModule, STRID_BUFFER));
    SetDlgItemTextWide(hDlg, IDOK, GetLangStringW(wLangModule, STRID_OK));
    SetDlgItemTextWide(hDlg, IDCANCEL, GetLangStringW(wLangModule, STRID_CANCEL));

    SendMessage(hWndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES);
    SendMessage(hWndBufferSpin, UDM_SETRANGE32, (WPARAM)-1, 99999);
    SendMessage(hWndBufferSpin, UDM_SETBUDDY, (WPARAM)hWndBufferEdit, 0);
    SetDlgItemInt(hDlg, IDC_COLUMNS_BUFFER_EDIT, nContentBuffer, TRUE);

    if (hMenuList=CreatePopupMenu())
    {
      AppendMenuWide(hMenuList, MF_STRING, IDC_COLUMNS_ITEMMOVEUP, GetLangStringW(wLangModule, STRID_MENU_ITEMMOVEUP));
      AppendMenuWide(hMenuList, MF_STRING, IDC_COLUMNS_ITEMMOVEDOWN, GetLangStringW(wLangModule, STRID_MENU_ITEMMOVEDOWN));
    }

    //Fill list
    {
      RECT rcList;
      LVCOLUMNW lvc;
      LISTVIEWCOLUMN *lpColumnCount;
      int nIndex=0;

      GetClientRect(hWndList, &rcList);
      lvc.mask=LVCF_TEXT|LVCF_WIDTH;
      lvc.pszText=(wchar_t *)GetLangStringW(wLangModule, STRID_COLUMNS);
      lvc.cx=rcList.right;
      ListView_InsertColumnWide(hWndList, 0, &lvc);

      if (lpColumnsDlg=GlobalAlloc(GPTR, sizeof(lpColumns)))
        xmemcpy(lpColumnsDlg, lpColumns, sizeof(lpColumns));
      for (lpColumnCount=lpColumnsDlg; lpColumnCount->nID >= 0; ++lpColumnCount)
      {
        SetColumnItem(hWndList, nIndex, lpColumnCount, (lpColumnCount->dwFlags & LVCOLF_VISIBLE));
        ++nIndex;
      }
    }
    nSelItem=-1;
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    if ((HWND)wParam == hWndList)
    {
      LVHITTESTINFO lvhti;
      LVITEMW lvi;
      POINT ptScreen={0};
      RECT rcItem;

      if (lParam == -1)
      {
        if ((lvhti.iItem=nSelItem) != -1)
        {
          rcItem.left=LVIR_LABEL;
          SendMessage(hWndList, LVM_GETITEMRECT, (WPARAM)nSelItem, (LPARAM)&rcItem);
          ptScreen.x=rcItem.left;
          ptScreen.y=rcItem.bottom;
        }
        ClientToScreen(hWndList, &ptScreen);
      }
      else
      {
        GetCursorPos(&ptScreen);
        lvhti.pt=ptScreen;
        ScreenToClient(hWndList, &lvhti.pt);
        SendMessage(hWndList, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvhti);

        lvi.stateMask=LVIS_SELECTED;
        lvi.state=LVIS_SELECTED;
        SendMessage(hWndList, LVM_SETITEMSTATE, (WPARAM)lvhti.iItem, (LPARAM)&lvi);
      }
      EnableMenuItem(hMenuList, IDC_COLUMNS_ITEMMOVEUP, nSelItem >= 0?MF_ENABLED:MF_GRAYED);
      EnableMenuItem(hMenuList, IDC_COLUMNS_ITEMMOVEDOWN, nSelItem >= 0?MF_ENABLED:MF_GRAYED);
      TrackPopupMenu(hMenuList, TPM_LEFTBUTTON|TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, 0, hDlg, NULL);
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (wParam == IDC_COLUMNS_LIST)
    {
      NMLISTVIEW *pnmlv=(NMLISTVIEW *)lParam;
      LISTVIEWCOLUMN *lpColumn=(LISTVIEWCOLUMN *)pnmlv->lParam;

      if (pnmlv->hdr.code == LVN_ITEMCHANGING)
      {
        if (pnmlv->uNewState & LVIS_STATEIMAGEMASK)
        {
          if (lpColumn->nID == LVI_SCRIPT)
          {
            SetWindowLongPtrWide(hDlg, DWLP_MSGRESULT, 1);
            return TRUE;
          }
        }
      }
      else if (((NMLISTVIEW *)lParam)->hdr.code == LVN_ITEMCHANGED)
      {
        LISTVIEWCOLUMN *lpColumnCount;
        BOOL bReadContent=FALSE;

        if (pnmlv->uNewState & LVIS_SELECTED)
        {
          nSelItem=pnmlv->iItem;
        }
        if (pnmlv->uOldState & LVIS_SELECTED)
        {
          nSelItem=-1;
        }
        if (pnmlv->uNewState & LVIS_STATEIMAGEMASK)
        {
          if (((pnmlv->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1)
            lpColumn->dwFlags|=LVCOLF_VISIBLE;
          else
            lpColumn->dwFlags&=~LVCOLF_VISIBLE;
        }

        //Enable content buffer
        for (lpColumnCount=lpColumnsDlg; lpColumnCount->nID >= 0; ++lpColumnCount)
        {
          if ((lpColumnCount->dwFlags & LVCOLF_VISIBLE) && (lpColumnCount->dwFlags & LVCOLF_CONTENT))
          {
            bReadContent=TRUE;
            break;
          }
        }
        EnableWindow(hWndBufferLabel, bReadContent);
        EnableWindow(hWndBufferEdit, bReadContent);
        EnableWindow(hWndBufferSpin, bReadContent);
      }
      else if (((NMHDR *)lParam)->code == (UINT)LVN_KEYDOWN)
      {
        NMLVKEYDOWN *pnkd=(NMLVKEYDOWN *)lParam;
        BOOL bAlt=FALSE;
        BOOL bShift=FALSE;
        BOOL bControl=FALSE;

        if (GetKeyState(VK_MENU) < 0)
          bAlt=TRUE;
        if (GetKeyState(VK_SHIFT) < 0)
          bShift=TRUE;
        if (GetKeyState(VK_CONTROL) < 0)
          bControl=TRUE;

        if (pnkd->wVKey == VK_UP)
        {
          if (bAlt && !bShift && !bControl)
          {
            PostMessage(hDlg, WM_COMMAND, IDC_COLUMNS_ITEMMOVEUP, 0);
            return TRUE;
          }
        }
        else if (pnkd->wVKey == VK_DOWN)
        {
          if (bAlt && !bShift && !bControl)
          {
            PostMessage(hDlg, WM_COMMAND, IDC_COLUMNS_ITEMMOVEDOWN, 0);
            return TRUE;
          }
        }
      }
      else if ((int)((NMHDR *)lParam)->code == NM_CUSTOMDRAW)
      {
        LPNMLVCUSTOMDRAW lplvcd=(LPNMLVCUSTOMDRAW)lParam;
        LRESULT lResult;

        if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
        {
          lResult=CDRF_NOTIFYITEMDRAW;
        }
        else if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
        {
          lResult=CDRF_NOTIFYSUBITEMDRAW;
        }
        else if (lplvcd->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT))
        {
          if (lplvcd->iSubItem == 0)
          {
            LISTVIEWCOLUMN *lpColumn=(LISTVIEWCOLUMN *)lplvcd->nmcd.lItemlParam;

            if (lpColumn && (lpColumn->dwFlags & LVCOLF_CONTENT))
            {
              lplvcd->clrText=RGB(0x00, 0x00, 0xFF);
              lplvcd->clrTextBk=RGB(0xFF, 0xFF, 0xFF);
            }
          }
          lResult=CDRF_DODEFAULT;
        }
        else lResult=CDRF_DODEFAULT;

        SetWindowLongPtrWide(hDlg, DWLP_MSGRESULT, lResult);
        return TRUE;
      }
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    if (LOWORD(wParam) == IDC_COLUMNS_ITEMMOVEUP ||
        LOWORD(wParam) == IDC_COLUMNS_ITEMMOVEDOWN)
    {
      if (nSelItem != -1)
      {
        LVITEMW lvi;
        int nOldIndex=nSelItem;
        int nNewIndex;

        //Get
        lvi.mask=LVIF_STATE|LVIF_PARAM;
        lvi.iItem=nOldIndex;
        lvi.iSubItem=0;
        lvi.stateMask=LVIS_STATEIMAGEMASK;
        ListView_GetItemWide(hWndList, &lvi);

        //Delete
        SendMessage(hWndList, LVM_DELETEITEM, nOldIndex, 0);

        //Add
        if (LOWORD(wParam) == IDC_COLUMNS_ITEMMOVEUP)
          nNewIndex=max(nOldIndex - 1, 0);
        else
          nNewIndex=nOldIndex + 1;
        nNewIndex=SetColumnItem(hWndList, nNewIndex, (LISTVIEWCOLUMN *)lvi.lParam, ((lvi.state & LVIS_STATEIMAGEMASK) >> 12) - 1);

        //Select
        SetFocus(hWndList);
        lvi.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
        lvi.state=LVIS_SELECTED|LVIS_FOCUSED;
        SendMessage(hWndList, LVM_SETITEMSTATE, nNewIndex, (LPARAM)&lvi);
      }
    }
    else if (LOWORD(wParam) == IDOK)
    {
      LVITEMW lvi;
      LISTVIEWCOLUMN *lpColumnCount;
      int nIndex=0;

      for (;;)
      {
        lvi.mask=LVIF_STATE|LVIF_PARAM;
        lvi.iItem=nIndex;
        lvi.iSubItem=0;
        lvi.stateMask=LVIS_STATEIMAGEMASK;
        if (!ListView_GetItemWide(hWndList, &lvi))
          break;

        if (lpColumnCount=(LISTVIEWCOLUMN *)lvi.lParam)
          xmemcpy(&lpColumns[nIndex], lpColumnCount, sizeof(LISTVIEWCOLUMN));
        ++nIndex;
      }
      //Last item ID must be -1
      lpColumns[nIndex].nID=-1;

      //Content buffer size
      nContentBuffer=GetDlgItemInt(hDlg, IDC_COLUMNS_BUFFER_EDIT, NULL, TRUE);

      EndDialog(hDlg, 1);
      return TRUE;
    }
    else if (LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, 0);
      return TRUE;
    }
  }
  else if (uMsg == WM_DESTROY)
  {
    //Destroy resources
    DestroyMenu(hMenuList);
    if (lpColumnsDlg)
      GlobalFree((HGLOBAL)lpColumnsDlg);
  }

  return FALSE;
}

int SetColumnItem(HWND hWnd, int nIndex, LISTVIEWCOLUMN *lpColumn, BOOL bCheck)
{
  LVITEMW lvi;

  lvi.mask=LVIF_TEXT|LVIF_PARAM;
  lvi.pszText=(wchar_t *)GetLangStringW(wLangModule, STRID_SCRIPT + lpColumn->nID);
  lvi.iItem=nIndex;
  lvi.iSubItem=0;
  lvi.lParam=(LPARAM)lpColumn;
  nIndex=ListView_InsertItemWide(hWnd, &lvi);

  lvi.mask=LVIF_STATE;
  lvi.iItem=nIndex;
  lvi.iSubItem=0;
  if (lpColumn->nID == LVI_SCRIPT)
    lvi.state=((0) << 12);
  else
    lvi.state=((bCheck + 1) << 12);
  lvi.stateMask=LVIS_STATEIMAGEMASK;
  ListView_SetItemWide(hWnd, &lvi);

  return nIndex;
}

BOOL CALLBACK CodeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static HWND hWndMemReadCheck;
  static HWND hWndMemWriteCheck;
  static HWND hWndMemFreeCheck;
  static HWND hWndMemLeakCheck;
  static HWND hWndSysCallCheck;
  static HWND hWndOK;
  static HWND hWndCancel;
  static DWORD *lpdwDebugCode;
  static DWORD dwDebugCode;

  if (uMsg == WM_INITDIALOG)
  {
    lpdwDebugCode=(DWORD *)lParam;
    dwDebugCode=*lpdwDebugCode;

    SendMessage(hDlg, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)g_hPluginIcon);
    hWndMemReadCheck=GetDlgItem(hDlg, IDC_CODE_MEMREAD_CHECK);
    hWndMemWriteCheck=GetDlgItem(hDlg, IDC_CODE_MEMWRITE_CHECK);
    hWndMemFreeCheck=GetDlgItem(hDlg, IDC_CODE_MEMFREE_CHECK);
    hWndMemLeakCheck=GetDlgItem(hDlg, IDC_CODE_MEMLEAK_CHECK);
    hWndSysCallCheck=GetDlgItem(hDlg, IDC_CODE_SYSCALL_CHECK);
    hWndOK=GetDlgItem(hDlg, IDOK);
    hWndCancel=GetDlgItem(hDlg, IDCANCEL);

    SetWindowTextWide(hDlg, GetLangStringW(wLangModule, STRID_CODE));
    SetDlgItemTextWide(hDlg, IDC_CODE_MEMREAD_CHECK, GetLangStringW(wLangModule, STRID_CODE_MEMREAD));
    SetDlgItemTextWide(hDlg, IDC_CODE_MEMWRITE_CHECK, GetLangStringW(wLangModule, STRID_CODE_MEMWRITE));
    SetDlgItemTextWide(hDlg, IDC_CODE_MEMFREE_CHECK, GetLangStringW(wLangModule, STRID_CODE_MEMFREE));
    SetDlgItemTextWide(hDlg, IDC_CODE_MEMLEAK_CHECK, GetLangStringW(wLangModule, STRID_CODE_MEMLEAK));
    SetDlgItemTextWide(hDlg, IDC_CODE_SYSCALL_CHECK, GetLangStringW(wLangModule, STRID_CODE_SYSCALL));
    SetDlgItemTextWide(hDlg, IDOK, GetLangStringW(wLangModule, STRID_OK));
    SetDlgItemTextWide(hDlg, IDCANCEL, GetLangStringW(wLangModule, STRID_CANCEL));

    if (dwDebugCode & DBG_MEMREAD)
      SendMessage(hWndMemReadCheck, BM_SETCHECK, BST_CHECKED, 0);
    if (dwDebugCode & DBG_MEMWRITE)
      SendMessage(hWndMemWriteCheck, BM_SETCHECK, BST_CHECKED, 0);
    if (dwDebugCode & DBG_MEMFREE)
      SendMessage(hWndMemFreeCheck, BM_SETCHECK, BST_CHECKED, 0);
    if (dwDebugCode & DBG_MEMLEAK)
      SendMessage(hWndMemLeakCheck, BM_SETCHECK, BST_CHECKED, 0);
    if (dwDebugCode & DBG_SYSCALL)
      SendMessage(hWndSysCallCheck, BM_SETCHECK, BST_CHECKED, 0);
  }
  else if (uMsg == WM_COMMAND)
  {
    if (LOWORD(wParam) == IDOK)
    {
      dwDebugCode=0;
      if (SendMessage(hWndMemReadCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugCode|=DBG_MEMREAD;
      if (SendMessage(hWndMemWriteCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugCode|=DBG_MEMWRITE;
      if (SendMessage(hWndMemFreeCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugCode|=DBG_MEMFREE;
      if (SendMessage(hWndMemLeakCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugCode|=DBG_MEMLEAK;
      if (SendMessage(hWndSysCallCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        dwDebugCode|=DBG_SYSCALL;
      *lpdwDebugCode=dwDebugCode;

      EndDialog(hDlg, 1);
      return TRUE;
    }
    else if (LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, 0);
      return TRUE;
    }
  }
  return FALSE;
}

LRESULT CALLBACK NewFilterProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_KEYDOWN)
  {
    if (wParam == VK_DOWN || wParam == VK_UP)
    {
      if (GetFocus() != hWndScriptsList)
        SetFocus(hWndScriptsList);
      return SendMessage(hWndScriptsList, uMsg, wParam, lParam);
    }
  }
  return CallWindowProcWide(lpOldFilterProc, hWnd, uMsg, wParam, lParam);
}

BOOL RegisterHotkey(wchar_t *wszScriptName, WORD wHotkey)
{
  PLUGINFUNCTION *pfElement;
  wchar_t wszPrefix[MAX_PATH];
  wchar_t wszFunction[MAX_PATH];
  wchar_t wszHotkeyOwner[MAX_PATH];
  int nOwner;

  xstrcpyW(wszPrefix, L"Scripts::Main::");
  xprintfW(wszFunction, L"%s%s", wszPrefix, wszScriptName);
  nOwner=(int)SendMessage(hMainWnd, AKD_CHECKHOTKEY, wHotkey, (LPARAM)wszHotkeyOwner);

  if (!nOwner)
  {
    if (pfElement=(PLUGINFUNCTION *)SendMessage(hMainWnd, AKD_DLLFINDW, (WPARAM)wszFunction, 0))
    {
      pfElement->wHotkey=wHotkey;

      if (!pfElement->wHotkey && !pfElement->bRunning && !pfElement->bAutoLoad)
        SendMessage(hMainWnd, AKD_DLLDELETE, 0, (LPARAM)pfElement);
    }
    else
    {
      if (wHotkey)
      {
        PLUGINADDW pa={0};

        pa.pFunction=wszFunction;
        pa.wHotkey=wHotkey;
        pa.PluginProc=HotkeyProc;

        if (pfElement=(PLUGINFUNCTION *)SendMessage(hMainWnd, AKD_DLLADDW, 0, (LPARAM)&pa))
        {
          pfElement->lpParameter=pfElement->wszFunction + xstrlenW(wszPrefix);
        }
      }
    }
    return TRUE;
  }
  else
  {
    xprintfW(wszBuffer, GetLangStringW(wLangModule, STRID_HOTKEY_EXISTS), wszFunction, wszHotkeyOwner);
    MessageBoxW(hWndMainDlg, wszBuffer, wszPluginTitle, MB_OK|MB_ICONEXCLAMATION);
    return FALSE;
  }
}

void CreateColumns(HWND hWnd)
{
  LVCOLUMNW lvc;
  LISTVIEWCOLUMN *lpColumnCount;
  int nColumnIndex=0;

  //Remove all columns
  while (SendMessage(hWnd, LVM_DELETECOLUMN, 0, 0));

  //Insert new columns
  for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
  {
    if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
    {
      lvc.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
      lvc.pszText=(wchar_t *)GetLangStringW(wLangModule, STRID_SCRIPT + lpColumnCount->nID);
      lvc.cx=lpColumnCount->nWidth;
      lvc.iSubItem=lpColumnCount->nID;
      ListView_InsertColumnWide(hWnd, nColumnIndex++, &lvc);
    }
  }
}

LISTVIEWCOLUMN* GetColumnByID(int nID, int *lpnColumnIndex)
{
  LISTVIEWCOLUMN *lpColumnCount;
  int nColumnIndex=0;

  for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
  {
    if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
    {
      if (lpColumnCount->nID == nID)
        break;
      ++nColumnIndex;
    }
  }
  if (lpnColumnIndex) *lpnColumnIndex=nColumnIndex;
  return lpColumnCount;
}

LISTVIEWCOLUMN* GetColumnByIndex(int nColumnIndex)
{
  LISTVIEWCOLUMN *lpColumnCount;
  int nCount=0;

  for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
  {
    if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
    {
      if (nCount == nColumnIndex)
        break;
      ++nCount;
    }
  }
  return lpColumnCount;
}

void FillScriptList(HWND hWnd, const wchar_t *wpFilter, const wchar_t *wpContentFilter)
{
  SCRIPTTHREAD *lpScriptThread;
  PLUGINFUNCTION *pfElement;
  LISTVIEWCOLUMN *lpColumnCount;
  LISTVIEWITEMPARAM *lpItemParam;
  LISTVIEWITEMPARAM *lpLastScriptItemParam=NULL;
  WIN32_FIND_DATAW wfd;
  HANDLE hFind;
  wchar_t wszFindFiles[MAX_PATH];
  wchar_t wszHotkey[MAX_PATH];
  const wchar_t *wpExt;
  wchar_t *wpCount;
  wchar_t *wpMaxCount;
  wchar_t *wpValue;
  wchar_t *wpValueEnd;
  wchar_t *wpContent;
  static wchar_t *wpVersion;
  static wchar_t *wpAuthor;
  static wchar_t *wpDescription;
  static wchar_t *wpSite;
  const wchar_t *wpComment;
  LVITEMW lvi;
  INT_PTR nContentLen;
  static INT_PTR nVersionLen;
  static INT_PTR nAuthorLen;
  static INT_PTR nDescriptionLen;
  static INT_PTR nSiteLen;
  DWORD dwContentInfo;
  int nCommentLen;
  LANGID wLang;
  LANGID wDescriptionLang;
  int nColumnIndex;
  int nIndexToSelect=-1;
  int nItemCount=0;
  int nIndex;
  BOOL bContentFilter;
  CONTENTKEY *ckCount;
  CONTENTKEY ck[]={{NULL, 0, &wpDescription, &nDescriptionLen},
                   {L"***", 3, &wpDescription, &nDescriptionLen},
                   {L"Description(", 12, &wpDescription, &nDescriptionLen},
                   {L"Version:", 8, &wpVersion, &nVersionLen},
                   {L"Author:", 7, &wpAuthor, &nAuthorLen},
                   {L"http://akelpad.sourceforge.net/", 31, &wpSite, &nSiteLen},
                   {0,0,0}};

  //Clear contents
  SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
  FreeScriptList(hWndScriptsList);
  SendMessage(hWnd, LVM_DELETEALLITEMS, 0, 0);

  //Find content columns
  dwContentInfo=0;

  for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
  {
    if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
    {
      if (lpColumnCount->dwFlags & LVCOLF_CONTENT)
      {
        if (lpColumnCount->nID == LVI_VERSION)
          dwContentInfo|=CCOLF_VERSION;
        else if (lpColumnCount->nID == LVI_AUTHOR)
          dwContentInfo|=CCOLF_AUTHOR;
        else if (lpColumnCount->nID == LVI_DESCRIPTION)
          dwContentInfo|=CCOLF_DESCRIPTION;
        else if (lpColumnCount->nID == LVI_SITE)
          dwContentInfo|=CCOLF_SITE;
      }
    }
  }

  //Content filter
  if (bContentFilterEnable && wpContentFilter && *wpContentFilter)
    bContentFilter=TRUE;
  else
    bContentFilter=FALSE;

  //Script files
  xprintfW(wszFindFiles, L"%s\\*.*", wszScriptsDir);

  if ((hFind=FindFirstFileWide(wszFindFiles, &wfd)) != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (wfd.cFileName[0] == '.' && (wfd.cFileName[1] == '\0' || (wfd.cFileName[1] == '.' && wfd.cFileName[2] == '\0'))) continue;

      if (!(wpExt=GetFileExt(wfd.cFileName, -1)))
        continue;
      if (!xstrcmpiW(wpExt, L"js"))
      {
        wpComment=L"//";
        nCommentLen=2;
      }
      else if (!xstrcmpiW(wpExt, L"vbs"))
      {
        wpComment=L"'";
        nCommentLen=1;
      }
      else continue;

      if (!dwContentInfo && wpFilter && *wpFilter)
      {
        if (!xstrstrW(wfd.cFileName, -1, wpFilter, -1, FALSE, NULL, NULL))
          continue;
      }

      //Script content
      ck[0].wpKey=wpComment;
      ck[0].nKeyLen=nCommentLen;
      wpContent=NULL;
      wpVersion=NULL;
      wpAuthor=NULL;
      wpDescription=NULL;
      wpSite=NULL;
      wDescriptionLang=0;

      if (dwContentInfo || bContentFilter)
      {
        if (bContentFilter)
          nContentLen=-1;
        else
          nContentLen=nContentBuffer;

        xprintfW(wszFindFiles, L"%s\\%s", wszScriptsDir, wfd.cFileName);
        if (nContentLen=ReadFileContent(wszFindFiles, ADT_BINARY_ERROR|ADT_DETECT_CODEPAGE|ADT_DETECT_BOM|ADT_NOMESSAGES, 0, 0, &wpContent, (UINT_PTR)nContentLen))
        {
          if (bContentFilter)
          {
            if (!xstrstrW(wpContent, nContentLen, wpContentFilter, -1, FALSE, NULL, NULL))
              nContentLen=0;
          }
          if (nContentLen && dwContentInfo)
          {
            wpCount=wpContent;
            wpMaxCount=wpContent + nContentLen;

            while (wpCount < wpMaxCount)
            {
              //Skip spaces at line start
              while (*wpCount == L' ' || *wpCount == L'\t') ++wpCount;

              //Line must start with comment
              if (xstrcmpinW(wpComment, wpCount, (UINT_PTR)-1))
                break;
              wpCount+=nCommentLen;

              //Skip spaces
              while (*wpCount == L' ' || *wpCount == L'\t') ++wpCount;

              for (ckCount=ck; ckCount->wpKey; ++ckCount)
              {
                wpValue=*ckCount->wppValue;

                if ((!wpValue || (ckCount->wppValue == &wpDescription && wDescriptionLang != wLangModule)) &&
                    !xstrcmpinW(ckCount->wpKey, wpCount, (UINT_PTR)-1))
                {
                  if (ckCount->wppValue != &wpSite)
                  {
                    wpCount+=ckCount->nKeyLen;
                    while (*wpCount == L' ' || *wpCount == L'\t') ++wpCount;
                  }
                  wpValue=wpCount;
                  while (*wpCount != L'\r' && *wpCount != L'\n' && *wpCount != L'\0') ++wpCount;
                  wpValueEnd=wpCount;

                  //Trim trailing spaces
                  while (wpValueEnd > wpValue && (*(wpValueEnd - 1) == L' ' || *(wpValueEnd - 1) == L'\t'))
                    --wpValueEnd;
                  if (ckCount->wppValue == &wpDescription)
                  {
                    if (ckCount->wpKey[ckCount->nKeyLen - 1] == L'(')
                    {
                      wLang=(WORD)xatoiW(wpValue, &wpValue);
                      if (LangMatchRate(wLang, wLangModule) <= LangMatchRate(wDescriptionLang, wLangModule))
                        break;
                      wDescriptionLang=wLang;

                      if (!xstrcmpinW(L"):", wpValue, (UINT_PTR)-1))
                        wpValue+=2;
                      while (*wpValue == L' ' || *wpValue == L'\t') ++wpValue;
                    }
                    else
                    {
                      if (wDescriptionLang) break;
                      wDescriptionLang=wLangModule;
                    }

                    //*** Description *** -> Description
                    if (wpValueEnd - ckCount->nKeyLen >= wpValue && !xstrcmpinW(ckCount->wpKey, wpValueEnd - ckCount->nKeyLen, (UINT_PTR)-1))
                      wpValueEnd-=ckCount->nKeyLen;
                    //Trim trailing spaces
                    while (wpValueEnd > wpValue && (*(wpValueEnd - 1) == L' ' || *(wpValueEnd - 1) == L'\t'))
                      --wpValueEnd;
                  }
                  *ckCount->wppValue=wpValue;
                  *ckCount->lpnValueLen=wpValueEnd - wpValue;
                  break;
                }
              }
              while (*wpCount != L'\r' && *wpCount != L'\n' && *wpCount != L'\0') ++wpCount;
              while (*wpCount == L'\r' || *wpCount == L'\n') ++wpCount;
            }
          }

          if (!wpVersion && !wpAuthor && !wpDescription && !wpSite)
          {
            SendMessage(hMainWnd, AKD_FREETEXT, 0, (LPARAM)wpContent);
            wpContent=NULL;
          }
        }
        if (!nContentLen) continue;
      }

      if (dwContentInfo && wpFilter && *wpFilter)
      {
        if (!xstrstrW(wfd.cFileName, -1, wpFilter, -1, FALSE, NULL, NULL) &&
            !xstrstrW(wpVersion, nVersionLen, wpFilter, -1, FALSE, NULL, NULL) &&
            !xstrstrW(wpAuthor, nAuthorLen, wpFilter, -1, FALSE, NULL, NULL) &&
            !xstrstrW(wpDescription, nDescriptionLen, wpFilter, -1, FALSE, NULL, NULL) &&
            !xstrstrW(wpSite, nSiteLen, wpFilter, -1, FALSE, NULL, NULL))
        {
          if (wpContent) SendMessage(hMainWnd, AKD_FREETEXT, 0, (LPARAM)wpContent);
          continue;
        }
      }

      nColumnIndex=0;
      nIndex=nItemCount;
      if (!(lpItemParam=(LISTVIEWITEMPARAM *)GlobalAlloc(GPTR, sizeof(LISTVIEWITEMPARAM))))
        continue;

      if (wpVersion)
        CopyWideStr(wpVersion, nVersionLen, &lpItemParam->wpVersion);
      if (wpAuthor)
        CopyWideStr(wpAuthor, nAuthorLen, &lpItemParam->wpAuthor);
      if (wpDescription)
        CopyWideStr(wpDescription, nDescriptionLen, &lpItemParam->wpDescription);
      if (wpSite)
        CopyWideStr(wpSite, nSiteLen, &lpItemParam->wpSite);

      for (lpColumnCount=lpColumns; lpColumnCount->nID >= 0; ++lpColumnCount)
      {
        if (lpColumnCount->dwFlags & LVCOLF_VISIBLE)
        {
          lvi.iItem=nIndex;

          if (lpColumnCount->nID == LVI_SCRIPT)
          {
            CopyWideStr(wfd.cFileName, -1, &lpItemParam->wpScript);
            lvi.mask=LVIF_TEXT;
            lvi.pszText=lpItemParam->wpScript;
            lvi.iSubItem=nColumnIndex;
          }
          else if (lpColumnCount->nID == LVI_HOTKEY)
          {
            //Find hotkey
            xprintfW(wszBuffer, L"Scripts::Main::%s", wfd.cFileName);
            pfElement=(PLUGINFUNCTION *)SendMessage(hMainWnd, AKD_DLLFINDW, (WPARAM)wszBuffer, 0);

            if (pfElement)
            {
              GetHotkeyString(pfElement->wHotkey, wszHotkey);
              lpItemParam->wHotkey=pfElement->wHotkey;
              CopyWideStr(wszHotkey, -1, &lpItemParam->wpHotkey);
              lvi.mask=LVIF_TEXT;
              lvi.pszText=lpItemParam->wpHotkey;
              lvi.iSubItem=nColumnIndex;
            }
          }
          else if (lpColumnCount->nID == LVI_STATUS)
          {
            if (lpScriptThread=StackGetScriptThreadByName(&hThreadStack, wfd.cFileName))
            {
              lvi.mask=LVIF_TEXT;
              lvi.pszText=(wchar_t *)(lpScriptThread->bWaiting?GetLangStringW(wLangModule, STRID_WAITING):GetLangStringW(wLangModule, STRID_RUNNING));
              lvi.iSubItem=LVI_STATUS;

              CopyWideStr(lvi.pszText, -1, &lpItemParam->wpStatus);
            }
          }
          else if (lpColumnCount->nID == LVI_VERSION)
          {
            if (lpItemParam->wpVersion)
            {
              lvi.mask=LVIF_TEXT;
              lvi.pszText=lpItemParam->wpVersion;
              lvi.iSubItem=nColumnIndex;
            }
          }
          else if (lpColumnCount->nID == LVI_AUTHOR)
          {
            if (lpItemParam->wpAuthor)
            {
              lvi.mask=LVIF_TEXT;
              lvi.pszText=lpItemParam->wpAuthor;
              lvi.iSubItem=nColumnIndex;
            }
          }
          else if (lpColumnCount->nID == LVI_DESCRIPTION)
          {
            if (lpItemParam->wpDescription)
            {
              lvi.mask=LVIF_TEXT;
              lvi.pszText=lpItemParam->wpDescription;
              lvi.iSubItem=nColumnIndex;
            }
          }
          else if (lpColumnCount->nID == LVI_SITE)
          {
            if (lpItemParam->wpSite)
            {
              lvi.mask=LVIF_TEXT;
              lvi.pszText=lpItemParam->wpSite;
              lvi.iSubItem=nColumnIndex;
            }
          }
          else
          {
            lvi.mask=LVIF_TEXT;
            lvi.pszText=(wchar_t *)L"<>";
            lvi.iSubItem=nColumnIndex;
          }
          if (!nColumnIndex++)
            nIndex=ListView_InsertItemWide(hWnd, &lvi);
          else
            ListView_SetItemWide(hWnd, &lvi);
        }
      }
      ++nItemCount;

      lvi.mask=LVIF_PARAM;
      lvi.iItem=nIndex;
      lvi.iSubItem=0;
      lvi.lParam=(LPARAM)lpItemParam;
      ListView_SetItemWide(hWnd, &lvi);

      if (!lpLastScriptItemParam)
      {
        if (!xstrcmpiW(wfd.cFileName, wszLastScript))
          lpLastScriptItemParam=lpItemParam;
      }
      if (wpContent) SendMessage(hMainWnd, AKD_FREETEXT, 0, (LPARAM)wpContent);
    }
    while (FindNextFileWide(hFind, &wfd));

    FindClose(hFind);
  }

  //Sort items
  if (lpColumnCount=GetColumnByID(nSortColumn, NULL))
    SendMessage(hWnd, LVM_SORTITEMS, (LPARAM)lpColumnCount, (LPARAM)CompareFunc);

  if (lpLastScriptItemParam)
  {
    LVFINDINFOA lvfi;

    lvfi.flags=LVFI_PARAM;
    lvfi.lParam=(LPARAM)lpLastScriptItemParam;
    nIndexToSelect=(int)SendMessage(hWnd, LVM_FINDITEMA, (WPARAM)-1, (LPARAM)&lvfi);
  }

  lvi.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
  lvi.state=LVIS_SELECTED|LVIS_FOCUSED;
  SendMessage(hWnd, LVM_SETITEMSTATE, (WPARAM)max(nIndexToSelect, 0), (LPARAM)&lvi);
  SendMessage(hWnd, LVM_ENSUREVISIBLE, (WPARAM)max(nIndexToSelect, 0), TRUE);

  SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(hWnd, NULL, TRUE);
}

void FreeScriptList(HWND hWnd)
{
  LISTVIEWITEMPARAM *lpItemParam;
  int nIndex;

  for (nIndex=0; lpItemParam=GetItemParam(hWnd, nIndex); ++nIndex)
  {
    FreeWideStr(lpItemParam->wpScript);
    FreeWideStr(lpItemParam->wpHotkey);
    FreeWideStr(lpItemParam->wpStatus);
    FreeWideStr(lpItemParam->wpVersion);
    FreeWideStr(lpItemParam->wpAuthor);
    FreeWideStr(lpItemParam->wpDescription);
    FreeWideStr(lpItemParam->wpSite);
    GlobalFree((HGLOBAL)lpItemParam);
  }
}

LISTVIEWITEMPARAM* GetItemParam(HWND hWnd, int nIndex)
{
  LVITEMW lvi;

  if (nIndex == -1)
    return NULL;
  lvi.mask=LVIF_PARAM;
  lvi.iItem=nIndex;
  lvi.iSubItem=0;
  lvi.lParam=0;
  ListView_GetItemWide(hWnd, &lvi);

  return (LISTVIEWITEMPARAM *)lvi.lParam;
}

int LangMatchRate(LANGID wCompareIt, LANGID wCompareWith)
{
  int nRate=0;

  if (wCompareIt == wCompareWith)
    nRate=3;
  else if (PRIMARYLANGID(wCompareIt) == PRIMARYLANGID(wCompareWith))
    nRate=2;
  else if (PRIMARYLANGID(wCompareIt) == LANG_ENGLISH)
    nRate=1;
  return nRate;
}

LRESULT CALLBACK NewMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CLOSE)
  {
    //Send WM_CLOSE to scripts dialogs or
    //post quit message to message loop and
    //wait for thread finished.
    if (IsAnyMessageLoop())
    {
      int nCloseOK;
      int nCloseERR;

      CloseScriptThreadAll(&nCloseOK, &nCloseERR);

      if (nCloseOK && !nCloseERR)
      {
        if (!dwWaitScriptTimerId)
        {
          dwWaitScriptTimerStart=GetTickCount();
          dwWaitScriptTimerId=SetTimer(NULL, 0, 100, (TIMERPROC)TimerProc);
        }
      }
      //Abort program closing
      return 0;
    }
  }
  else if (uMsg == AKDN_MESSAGEBOXBEGIN)
  {
    g_MainMessageBox=TRUE;
    ShowScriptWindowsAll(FALSE);
  }
  else if (uMsg == AKDN_MESSAGEBOXEND)
  {
    g_MainMessageBox=FALSE;
    ShowScriptWindowsAll(TRUE);
  }
  else if (uMsg == AKDN_MAIN_ONFINISH)
  {
    NewMainProcData->NextProc(hWnd, uMsg, wParam, lParam);

    StackFreeCallbacks(&g_hHookCallbackStack);
    StackFreeCallbacks(&g_hSysCallbackStack);
    StackFreeCallbacks(&g_hSubclassCallbackStack);
    if (g_WScriptTypeInfo) g_WScriptTypeInfo->lpVtbl->Release(g_WScriptTypeInfo);
    if (g_WArgumentsTypeInfo) g_WArgumentsTypeInfo->lpVtbl->Release(g_WArgumentsTypeInfo);
    if (g_DocumentTypeInfo) g_DocumentTypeInfo->lpVtbl->Release(g_DocumentTypeInfo);
    if (g_ScriptSettingsTypeInfo) g_ScriptSettingsTypeInfo->lpVtbl->Release(g_ScriptSettingsTypeInfo);
    if (g_SystemFunctionTypeInfo) g_SystemFunctionTypeInfo->lpVtbl->Release(g_SystemFunctionTypeInfo);
    if (g_ConstantsTypeInfo) g_ConstantsTypeInfo->lpVtbl->Release(g_ConstantsTypeInfo);
    if (bInitMain) UninitMain();
    return FALSE;
  }
  else if (uMsg == AKDN_MAIN_ONDESTROY)
  {
    bMainOnFinish=TRUE;
    //We already quit from message loops in WM_CLOSE.
    //PostQuitScriptAll();
  }

  //Call next procedure
  return NewMainProcData->NextProc(hWnd, uMsg, wParam, lParam);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  if (dwWaitScriptTimerId)
  {
    UINT_PTR dwElapsed=GetTickCount() - dwWaitScriptTimerStart;

    if (!IsAnyMessageLoop() || dwElapsed > 10000)
    {
      KillTimer(NULL, dwWaitScriptTimerId);
      dwWaitScriptTimerId=0;

      if (dwElapsed <= 10000)
        PostMessage(hMainWnd, WM_CLOSE, 0, 0);
    }
  }
}

BOOL CALLBACK HotkeyProc(void *lpParameter, LPARAM lParam, DWORD dwSupport)
{
  wchar_t *wpName=(wchar_t *)lpParameter;

  ExecScript(wpName, NULL, FALSE);
  return TRUE;
}

int EditScript(wchar_t *wpScript)
{
  OPENDOCUMENTW od;
  wchar_t wszFullName[MAX_PATH];

  xprintfW(wszFullName, L"%s\\%s", wszScriptsDir, wpScript);
  od.pFile=wszFullName;
  od.pWorkDir=NULL;
  od.dwFlags=OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE;
  od.nCodePage=0;
  od.bBOM=0;
  return (int)SendMessage(hMainWnd, AKD_OPENDOCUMENTW, (WPARAM)NULL, (LPARAM)&od);
}

void ExecScript(wchar_t *wpScript, wchar_t *wszArguments, BOOL bWaitExec)
{
  SCRIPTTHREAD *lpScriptThread;
  EXECSCRIPT es;
  HANDLE hInitMutex;

  if (lpScriptThread=StackGetScriptThreadByName(&hThreadStack, wpScript))
  {
    if (lpScriptThread->bSingleCopy)
      return;
  }

  if (hInitMutex=CreateEventA(NULL, FALSE, FALSE, pMutexInitName))
  {
    if (GetLastError() != ERROR_ALREADY_EXISTS)
    {
      es.wpScript=wpScript;
      es.wpArguments=wszArguments;
      es.nArgumentsLen=(int)xstrlenW(wszArguments);
      es.hInitMutex=hInitMutex;
      es.bWaitForScriptSignal=bWaitExec;

      if (hExecThread=CreateThread(NULL, 0, ExecThreadProc, &es, 0, &dwExecThreadId))
      {
        if (bWaitExec)
        {
          //Wait for hInitMutex and process messages.
          MSG msg;
          BOOL bExitLoop=FALSE;

          for (;;)
          {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
              if (msg.message == WM_QUIT)
                bExitLoop=TRUE;
              else
                SendMessage(hMainWnd, AKD_TRANSLATEMESSAGE, TMSG_ALL, (LPARAM)&msg);
            }
            if (bExitLoop)
              break;
            if (MsgWaitForMultipleObjects(1, &hInitMutex, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0)
              break;
          }
        }
        else WaitForSingleObject(hInitMutex, INFINITE);
      }
    }
    CloseHandle(hInitMutex);
  }
}

DWORD WINAPI ExecThreadProc(LPVOID lpParameter)
{
  EXECSCRIPT *es=(EXECSCRIPT *)lpParameter;
  SCRIPTTHREAD *lpScriptThread;
  HANDLE hThread=hExecThread;
  DWORD dwThreadID=GetCurrentThreadId();

  if (lpScriptThread=StackInsertScriptThread(&hThreadStack))
  {
    lpScriptThread->hThread=hThread;
    lpScriptThread->dwThreadID=dwThreadID;
    if (bGlobalDebugEnable)
      lpScriptThread->dwDebug=dwGlobalDebugCode;
    if (dwGlobalDebugJIT & JIT_DEBUG)
      lpScriptThread->dwDebugJIT=dwGlobalDebugJIT;

    if (es->wpScript)
    {
      const wchar_t *wpFileName;

      wpFileName=GetFileNameWide(es->wpScript, -1);
      xstrcpynW(lpScriptThread->wszScriptName, wpFileName, MAX_PATH);
      GetBaseName(lpScriptThread->wszScriptName, lpScriptThread->wszScriptBaseName, MAX_PATH);
      xprintfW(lpScriptThread->wszScriptFile, L"%s\\%s", wszScriptsDir, es->wpScript);
    }
    if (es->wpArguments)
    {
      SCRIPTARG *lpScriptArg;
      wchar_t *wpArgument;
      wchar_t *wpCount;
      int nArgLen;

      if (lpScriptThread->wszArguments=(wchar_t *)GlobalAlloc(GPTR, (es->nArgumentsLen + 1) * sizeof(wchar_t)))
        lpScriptThread->nArgumentsLen=(int)xstrcpynW(lpScriptThread->wszArguments, es->wpArguments, es->nArgumentsLen + 1);
      wpArgument=lpScriptThread->wszArguments;

      while (*wpArgument)
      {
        nArgLen=GetCommandLineArg(wpArgument, NULL, 0, NULL);

        if (lpScriptArg=StackInsertArgument(&lpScriptThread->hArgStack))
        {
          if (lpScriptArg->wpArgument=(wchar_t *)GlobalAlloc(GPTR, (nArgLen + 1) * sizeof(wchar_t)))
          {
            lpScriptArg->nArgumentLen=GetCommandLineArg(wpArgument, lpScriptArg->wpArgument, nArgLen + 1, &wpArgument);

            if (lpScriptArg->wpArgument[0] == L'-' || lpScriptArg->wpArgument[0] == L'/')
            {
              //Argument name
              lpScriptArg->wpArgName=lpScriptArg->wpArgument + 1;

              for (wpCount=lpScriptArg->wpArgName; *wpCount && *wpCount != L'=' && *wpCount != L' '; ++wpCount);
              lpScriptArg->nArgNameLen=(int)(wpCount - lpScriptArg->wpArgName);

              //Argument value
              if (*wpCount == L'=')
              {
                lpScriptArg->wpArgValue=wpCount + 1;
                lpScriptArg->nArgValueLen=lpScriptArg->nArgumentLen - (int)(lpScriptArg->wpArgValue - lpScriptArg->wpArgument);

                //"Value" -> Value
                if (lpScriptArg->nArgValueLen >= 2)
                {
                  if (lpScriptArg->wpArgValue[0] == L'\"' && lpScriptArg->wpArgValue[lpScriptArg->nArgValueLen - 1] == L'\"')
                  {
                    lpScriptArg->wpArgValue+=1;
                    lpScriptArg->nArgValueLen-=2;
                  }
                }
              }
              else
              {
                //If no value set it to "1".
                lpScriptArg->wpArgValue=L"1";
                lpScriptArg->nArgValueLen=1;
              }
            }
          }
          else break;
        }
        else break;
      }
    }
    if (es->hInitMutex)
    {
      //Protect from double execution
      lpScriptThread->hExecMutex=CreateEventA(NULL, FALSE, FALSE, pMutexExecName);

      if (!es->bWaitForScriptSignal)
      {
        //Thread is initialized now unlock main thread
        SetEvent(es->hInitMutex);
      }
      else
      {
        //Thread is initialized now remember hInitMutex to unlock main thread later - after script signal or termination
        lpScriptThread->hInitMutex=es->hInitMutex;
      }
    }

    if (*lpScriptThread->wszScriptFile)
    {
      if (lpScriptThread->hExecMutex)
      {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
          lpScriptThread->bWaiting=TRUE;
          WaitForSingleObject(lpScriptThread->hExecMutex, INFINITE);
          lpScriptThread->bWaiting=FALSE;
        }
      }

      //Run script
      if (!bMainOnFinish)
      {
        wchar_t *wpContent;
        const wchar_t *wpExt;
        char szExt[MAX_PATH];
        GUID guidEngine;
        INT_PTR nContentLen;

        if (wpExt=GetFileExt(lpScriptThread->wszScriptName, -1))
        {
          WideCharToMultiByte(CP_ACP, 0, --wpExt, -1, szExt, MAX_PATH, NULL, NULL);
          if (GetScriptEngineA(szExt, &guidEngine) == S_OK)
          {
            if (nContentLen=ReadFileContent(lpScriptThread->wszScriptFile, ADT_BINARY_ERROR|ADT_DETECT_CODEPAGE|ADT_DETECT_BOM|ADT_NOMESSAGES, 0, 0, &wpContent, (UINT_PTR)-1))
            {
              lpScriptThread->wpScriptText=wpContent;
              lpScriptThread->nScriptTextLen=nContentLen;

              if (ExecScriptText(lpScriptThread, &guidEngine) != S_OK)
              {
                //MessageBoxW(hMainWnd, GetLangStringW(wLangModule, STRID_EXECUTE_ERROR), wszPluginTitle, MB_OK|MB_ICONERROR);
              }
              StackFreeIncludes(&lpScriptThread->hIncludesStack);
              SendMessage(hMainWnd, AKD_FREETEXT, 0, (LPARAM)wpContent);
            }
            else
            {
              xprintfW(wszBuffer, GetLangStringW(wLangModule, STRID_READFILE_ERROR), lpScriptThread->wszScriptFile);
              MessageBoxW(hMainWnd, wszBuffer, wszPluginTitle, MB_OK|MB_ICONERROR);
            }
          }
          else
          {
            xprintfW(wszBuffer, GetLangStringW(wLangModule, STRID_ENGINE_ERROR), wpExt);
            MessageBoxW(hMainWnd, wszBuffer, wszPluginTitle, MB_OK|MB_ICONERROR);
          }
        }
      }
    }

    FreeScriptResources(lpScriptThread, (lpScriptThread->dwDebug & DBG_MEMLEAK));
  }
  else SetEvent(es->hInitMutex);

  //Free thread handle
  hExecThread=NULL;
  dwExecThreadId=0;

  if (hThread)
  {
    CloseHandle(hThread);
    hThread=NULL;
  }
  return 0;
}

SCRIPTTHREAD* StackInsertScriptThread(HTHREADSTACK *hStack)
{
  SCRIPTTHREAD *lpElement=NULL;

  if (!StackInsertIndex((stack **)&hStack->first, (stack **)&hStack->last, (stack **)&lpElement, -1, sizeof(SCRIPTTHREAD)))
  {
    lpElement->nRefCount=1;
    lpElement->lpStack=hStack;
    ++hStack->nElements;
  }
  return lpElement;
}

SCRIPTTHREAD* StackGetScriptThreadCurrent()
{
  return StackGetScriptThreadByID(&hThreadStack, GetCurrentThreadId());
}

SCRIPTTHREAD* StackGetScriptThreadByID(HTHREADSTACK *hStack, DWORD dwThreadID)
{
  SCRIPTTHREAD *lpElement;

  for (lpElement=hStack->first; lpElement; lpElement=lpElement->next)
  {
    if (lpElement->dwThreadID == dwThreadID)
      return lpElement;
  }
  return NULL;
}

SCRIPTTHREAD* StackGetScriptThreadByName(HTHREADSTACK *hStack, const wchar_t *wpScriptName)
{
  SCRIPTTHREAD *lpElement;

  for (lpElement=hStack->first; lpElement; lpElement=lpElement->next)
  {
    if (!xstrcmpiW(lpElement->wszScriptName, wpScriptName))
      return lpElement;
  }
  return NULL;
}

BOOL StackDeleteScriptThread(SCRIPTTHREAD *lpScriptThread)
{
  if (--lpScriptThread->nRefCount <= 0)
  {
    HTHREADSTACK *hStack=(HTHREADSTACK *)lpScriptThread->lpStack;

    if (lpScriptThread->wszArguments)
    {
      GlobalFree((HGLOBAL)lpScriptThread->wszArguments);
      lpScriptThread->wszArguments=NULL;
    }
    StackFreeArguments(&lpScriptThread->hArgStack);
    StackDelete((stack **)&hStack->first, (stack **)&hStack->last, (stack *)lpScriptThread);
    --hStack->nElements;
    return TRUE;
  }
  return FALSE;
}

void FreeScriptResources(SCRIPTTHREAD *lpScriptThread, BOOL bDebug)
{
  if (lpScriptThread->hPointersStack.nElements)
  {
    if (bDebug)
    {
      if (!lpScriptThread->bStopped)
      {
        xprintfW(wszBuffer, GetLangStringW(wLangModule, STRID_DEBUG_MEMLEAK), lpScriptThread->hPointersStack.nElements);
        MessageBoxW(hMainWnd, wszBuffer, wszPluginTitle, MB_OK|MB_ICONERROR);
      }
    }
    StackFreePointers(&lpScriptThread->hPointersStack);
  }
  if (lpScriptThread->hWndScriptsThreadDummy)
  {
    DestroyWindow(lpScriptThread->hWndScriptsThreadDummy);
    lpScriptThread->hWndScriptsThreadDummy=NULL;
  }
  if (lpScriptThread->hInitMutex)
  {
    SetEvent(lpScriptThread->hInitMutex);
    lpScriptThread->hInitMutex=NULL;
  }
  if (lpScriptThread->hExecMutex)
  {
    SetEvent(lpScriptThread->hExecMutex);
    CloseHandle(lpScriptThread->hExecMutex);
    lpScriptThread->hExecMutex=NULL;
  }
  StackDeleteScriptThread(lpScriptThread);
}

BOOL CloseScriptThread(SCRIPTTHREAD *lpScriptThread)
{
  CALLBACKITEM *lpCallback;
  CALLBACKITEM *lpNextCallback;
  BOOL bResult=TRUE;
  BOOL bTerminated=FALSE;
  BOOL bPostQuit=TRUE;

  //Close dialogs by sending WM_CLOSE. Script must call oSys.Call("user32::PostQuitMessage", 0) itself.
  for (lpCallback=lpScriptThread->hDialogCallbackStack.first; lpCallback; lpCallback=lpNextCallback)
  {
    lpNextCallback=lpCallback->next;

    if (lpCallback->hHandle)
    {
      if (IsWindow((HWND)lpCallback->hHandle))
      {
        SendMessage((HWND)lpCallback->hHandle, WM_CLOSE, 0, 0);
        if (IsWindow((HWND)lpCallback->hHandle))
          bResult=FALSE;
      }
      else
      {
        //Thread can be terminated outside of the program
        StackDeleteCallback(lpCallback);
        bTerminated=TRUE;
      }
      bPostQuit=FALSE;
    }
  }

  //Script has message loop - send quit mesage.
  if (bPostQuit)
  {
    if (lpScriptThread->hWndScriptsThreadDummy && lpScriptThread->dwMessageLoop)
      SendMessage(lpScriptThread->hWndScriptsThreadDummy, AKDLL_POSTQUIT, 0, 0);
  }

  if (bTerminated && !lpScriptThread->hDialogCallbackStack.nElements)
  {
    FreeScriptResources(lpScriptThread, FALSE);
  }
  return bResult;
}

void CloseScriptThreadAll(int *nCloseOK, int *nCloseERR)
{
  SCRIPTTHREAD *lpElement;
  SCRIPTTHREAD *lpNextElement;

  *nCloseOK=0;
  *nCloseERR=0;

  for (lpElement=hThreadStack.first; lpElement; lpElement=lpNextElement)
  {
    ++lpElement->nRefCount;
    if (CloseScriptThread(lpElement))
      ++*nCloseOK;
    else
      ++*nCloseERR;

    //Assign lpNextElement here cause after CloseScriptThread lpElement->next could be changed.
    lpNextElement=lpElement->next;
    StackDeleteScriptThread(lpElement);
  }
}

void PostQuitScriptAll()
{
  SCRIPTTHREAD *lpElement;
  SCRIPTTHREAD *lpNextElement;

  for (lpElement=hThreadStack.first; lpElement; lpElement=lpNextElement)
  {
    lpNextElement=lpElement->next;
    if (lpElement->hWndScriptsThreadDummy && lpElement->dwMessageLoop)
      SendMessage(lpElement->hWndScriptsThreadDummy, AKDLL_POSTQUIT, 0, 0);
  }
}

BOOL IsAnyMessageLoop()
{
  SCRIPTTHREAD *lpElement;

  for (lpElement=hThreadStack.first; lpElement; lpElement=lpElement->next)
  {
    if (lpElement->dwMessageLoop)
      return TRUE;
  }
  return FALSE;
}

void ShowScriptWindows(SCRIPTTHREAD *lpScriptThread, BOOL bShow)
{
  CALLBACKITEM *lpCallback;

  for (lpCallback=lpScriptThread->hDialogCallbackStack.first; lpCallback; lpCallback=lpCallback->next)
  {
    if (lpCallback->hHandle)
    {
      if (!bShow)
      {
        if (IsWindowVisible((HWND)lpCallback->hHandle))
        {
          ShowWindow((HWND)lpCallback->hHandle, SW_HIDE);
          lpCallback->bShow=TRUE;
        }
      }
      else
      {
        if (lpCallback->bShow)
          ShowWindow((HWND)lpCallback->hHandle, SW_SHOW);
      }
    }
  }
}

void ShowScriptWindowsAll(BOOL bShow)
{
  SCRIPTTHREAD *lpElement;

  for (lpElement=hThreadStack.first; lpElement; lpElement=lpElement->next)
  {
    //Hides window only if thread busy
    if (bShow || lpElement->bBusy)
      ShowScriptWindows(lpElement, bShow);
  }
}

int GetCommandLineArg(const wchar_t *wpCmdLine, wchar_t *wszArg, int nArgMax, const wchar_t **wpNextArg)
{
  const wchar_t *wpCount=wpCmdLine;
  wchar_t *wpArgSet=wszArg;
  wchar_t *wpArgSetMax=wszArg + nArgMax - 1;
  wchar_t wchInitStopChar;
  wchar_t wchCurStopChar;

  while (*wpCount == L' ') ++wpCount;

  if (*wpCount == L'\"' || *wpCount == L'\'' || *wpCount == L'`')
    wchInitStopChar=*wpCount++;
  else
    wchInitStopChar=L' ';
  wchCurStopChar=wchInitStopChar;

  for (; *wpCount; ++wpCount)
  {
    if (wchCurStopChar == L' ')
    {
      if (*wpCount == L' ')
        break;
      if (*wpCount == L'\"' || *wpCount == L'\'' || *wpCount == L'`')
        wchCurStopChar=*wpCount;
    }
    else if (wchCurStopChar == *wpCount)
    {
      if (wchCurStopChar == wchInitStopChar)
        break;
      wchCurStopChar=L' ';
    }

    if (wpArgSet < wpArgSetMax)
    {
      if (wszArg) *wpArgSet=*wpCount;
      ++wpArgSet;
    }
  }
  if (wszArg) *wpArgSet=L'\0';

  if (wpNextArg)
  {
    if (*wpCount) ++wpCount;
    for (*wpNextArg=wpCount; **wpNextArg == L' '; ++*wpNextArg);
  }
  return (int)(wpArgSet - wszArg);
}

SCRIPTARG* StackInsertArgument(HARGSTACK *hStack)
{
  SCRIPTARG *lpElement=NULL;

  if (!StackInsertIndex((stack **)&hStack->first, (stack **)&hStack->last, (stack **)&lpElement, -1, sizeof(SCRIPTARG)))
    ++hStack->nElements;

  return lpElement;
}

SCRIPTARG* StackGetArgumentByIndex(HARGSTACK *hStack, int nIndex)
{
  SCRIPTARG *lpElement;
  int nCount=0;

  for (lpElement=hStack->first; lpElement; lpElement=lpElement->next)
  {
    if (nIndex == nCount++)
      return lpElement;
  }
  return NULL;
}

SCRIPTARG* StackGetArgumentByName(HARGSTACK *hStack, const wchar_t *wpArgName, int nArgNameLen)
{
  SCRIPTARG *lpElement;

  for (lpElement=hStack->first; lpElement; lpElement=lpElement->next)
  {
    if (lpElement->wpArgName && nArgNameLen == lpElement->nArgNameLen)
    {
      if (!xstrcmpinW(wpArgName, lpElement->wpArgName, (UINT_PTR)-1))
        return lpElement;
    }
  }
  return NULL;
}

void StackFreeArguments(HARGSTACK *hStack)
{
  SCRIPTARG *lpElement;

  for (lpElement=hStack->first; lpElement; lpElement=lpElement->next)
  {
    GlobalFree((HGLOBAL)lpElement->wpArgument);
  }
  StackClear((stack **)&hStack->first, (stack **)&hStack->last);
  hStack->nElements=0;
}

UINT_PTR GetVariantInt(VARIANT *vt)
{
  VARIANT vtConverted;
  UINT_PTR dwResult=0;
  INT_PTR nResult=0;

  VariantInit(&vtConverted);
  VariantCopy(&vtConverted, vt);

  #ifdef _WIN64
    if (VariantChangeType(&vtConverted, &vtConverted, 0, VT_I8) == S_OK)
      nResult=vtConverted.llVal;
    if (!nResult && (VariantChangeType(&vtConverted, &vtConverted, 0, VT_UI8) == S_OK))
      dwResult=vtConverted.ullVal;
  #else
    if (VariantChangeType(&vtConverted, &vtConverted, 0, VT_I4) == S_OK)
      nResult=vtConverted.lVal;
    if (!nResult && (VariantChangeType(&vtConverted, &vtConverted, 0, VT_UI4) == S_OK))
      dwResult=vtConverted.ulVal;
  #endif
  return max(dwResult, (UINT_PTR)nResult);
}

UINT_PTR GetVariantValue(VARIANT *pvtParameter, BOOL bAnsi)
{
  CALLBACKITEM *lpSysCallback;
  UINT_PTR dwValue=0;
  int nUniLen;
  int nAnsiLen;

  if (pvtParameter->vt == VT_BSTR)
  {
    if (bAnsi)
    {
      nUniLen=SysStringLen(pvtParameter->bstrVal);
      nAnsiLen=WideCharToMultiByte(CP_ACP, 0, pvtParameter->bstrVal, nUniLen, NULL, 0, NULL, NULL);
      if (dwValue=(UINT_PTR)GlobalAlloc(GPTR, nAnsiLen + 1))
        WideCharToMultiByte(CP_ACP, 0, pvtParameter->bstrVal, nUniLen + 1, (char *)dwValue, nAnsiLen + 1, NULL, NULL);
    }
    else dwValue=(UINT_PTR)pvtParameter->bstrVal;
  }
  else if (pvtParameter->vt == VT_DISPATCH)
  {
    if (lpSysCallback=StackGetCallbackByObject(&g_hSysCallbackStack, pvtParameter->pdispVal))
      dwValue=(UINT_PTR)lpSysCallback->lpProc;
    else
      dwValue=(UINT_PTR)pvtParameter->pdispVal;
  }
  else
  {
    if (pvtParameter->vt == VT_BOOL)
      dwValue=pvtParameter->boolVal?TRUE:FALSE;
    else
      dwValue=GetVariantInt(pvtParameter);
  }
  return dwValue;
}

int GetHotkeyString(WORD wHotkey, wchar_t *wszString)
{
  wchar_t wszKeyText[100];
  LONG lScan;
  int nResult=0;

  wszString[0]=L'\0';
  wszKeyText[0]=L'\0';

  if (wHotkey)
  {
    if (bOldWindows)
      lScan=MapVirtualKeyA(LOBYTE(wHotkey), 0) << 16;
    else
      lScan=MapVirtualKeyW(LOBYTE(wHotkey), 0) << 16;
    if (HIBYTE(wHotkey) & HOTKEYF_EXT)
      lScan|=0x01000000L;
    GetKeyNameTextWide(lScan, wszKeyText, 100);

    nResult=(int)xprintfW(wszString, L"%s%s%s%s",
                                     (HIBYTE(wHotkey) & HOTKEYF_CONTROL)?L"Ctrl + ":L"",
                                     (HIBYTE(wHotkey) & HOTKEYF_SHIFT)?L"Shift + ":L"",
                                     (HIBYTE(wHotkey) & HOTKEYF_ALT)?L"Alt + ":L"",
                                     wszKeyText);
  }
  return nResult;
}

INT_PTR ReadFileContent(wchar_t *wpFile, DWORD dwFlags, int nCodePage, BOOL bBOM, wchar_t **wpContent, UINT_PTR dwBytesMax)
{
  DETECTFILEW df;
  FILECONTENT fc;
  INT_PTR nResult=0;

  *wpContent=NULL;

  //Detect codepage
  df.pFile=wpFile;
  df.dwBytesToCheck=1024;
  df.dwFlags=dwFlags;
  df.nCodePage=nCodePage;
  df.bBOM=bBOM;
  if (SendMessage(hMainWnd, AKD_DETECTFILEW, 0, (LPARAM)&df) == EDT_SUCCESS)
  {
    //Read contents
    if ((fc.hFile=CreateFileWide(df.pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)) != INVALID_HANDLE_VALUE)
    {
      fc.dwMax=dwBytesMax;
      fc.nCodePage=df.nCodePage;
      fc.bBOM=df.bBOM;
      if (nResult=SendMessage(hMainWnd, AKD_READFILECONTENT, 0, (LPARAM)&fc))
      {
        CloseHandle(fc.hFile);
        fc.hFile=NULL;

        *wpContent=fc.wpContent;
      }
      if (fc.hFile) CloseHandle(fc.hFile);
    }
  }
  return nResult;
}

int GetBaseName(const wchar_t *wpFile, wchar_t *wszBaseName, int nBaseNameMaxLen)
{
  int nFileLen=(int)xstrlenW(wpFile);
  int nEndOffset=-1;
  int i;

  for (i=nFileLen - 1; i >= 0; --i)
  {
    if (wpFile[i] == L'\\')
      break;

    if (nEndOffset == -1)
    {
      if (wpFile[i] == L'.')
        nEndOffset=i;
    }
  }
  ++i;
  if (nEndOffset == -1) nEndOffset=nFileLen;
  nBaseNameMaxLen=min(nEndOffset - i + 1, nBaseNameMaxLen);
  xstrcpynW(wszBaseName, wpFile + i, nBaseNameMaxLen);

  return nBaseNameMaxLen;
}

const wchar_t* GetFileExt(const wchar_t *wpFile, int nFileLen)
{
  const wchar_t *wpCount;

  if (nFileLen == -1) nFileLen=(int)xstrlenW(wpFile);

  for (wpCount=wpFile + nFileLen - 1; wpCount >= wpFile; --wpCount)
  {
    if (*wpCount == L'.') return wpCount + 1;
    if (*wpCount == L'\\') break;
  }
  return NULL;
}

const char* GetFileNameAnsi(const char *pFile, int nFileLen)
{
  const char *pCount;

  if (nFileLen == -1) nFileLen=(int)xstrlenA(pFile);

  for (pCount=pFile + nFileLen - 1; pCount >= pFile; --pCount)
  {
    if (*pCount == '\\')
      return pCount + 1;
  }
  return pFile;
}

const wchar_t* GetFileNameWide(const wchar_t *wpFile, int nFileLen)
{
  const wchar_t *wpCount;

  if (nFileLen == -1) nFileLen=(int)xstrlenW(wpFile);

  for (wpCount=wpFile + nFileLen - 1; wpCount >= wpFile; --wpCount)
  {
    if (*wpCount == L'\\')
      return wpCount + 1;
  }
  return wpFile;
}

int GetFileStreamOffset(const wchar_t *wpFile, int nFileLen)
{
  const wchar_t *wpCount;

  if (nFileLen == -1) nFileLen=(int)xstrlenW(wpFile);

  for (wpCount=wpFile + nFileLen - 1; wpCount > wpFile; --wpCount)
  {
    if (*wpCount == L':') return (int)(wpCount - wpFile);
    if (*wpCount == L'\\') break;
  }
  return 0;
}

DWORD ScrollCaret(HWND hWnd)
{
  AESCROLLTOPOINT stp;
  DWORD dwScrollResult;

  //Test scroll to caret
  stp.dwFlags=AESC_TEST|AESC_POINTCARET|AESC_OFFSETCHARX|AESC_OFFSETCHARY;
  stp.nOffsetX=1;
  stp.nOffsetY=0;
  dwScrollResult=(DWORD)SendMessage(hWnd, AEM_SCROLLTOPOINT, 0, (LPARAM)&stp);

  //Scroll to caret
  stp.dwFlags=AESC_POINTCARET;
  stp.nOffsetX=3;
  stp.nOffsetY=2;
  if (dwScrollResult & AECSE_SCROLLEDX)
    stp.dwFlags|=AESC_OFFSETRECTDIVX;
  if (dwScrollResult & AECSE_SCROLLEDY)
    stp.dwFlags|=AESC_OFFSETRECTDIVY;
  return (DWORD)SendMessage(hWnd, AEM_SCROLLTOPOINT, 0, (LPARAM)&stp);
}

int SaveLineScroll(HWND hWnd)
{
  SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
  return (int)SendMessage(hWnd, AEM_GETLINENUMBER, AEGL_FIRSTVISIBLELINE, 0);
}

void RestoreLineScroll(HWND hWnd, int nBeforeLine)
{
  if ((int)SendMessage(hWnd, AEM_GETLINENUMBER, AEGL_FIRSTVISIBLELINE, 0) != nBeforeLine)
  {
    POINT64 ptScrollPos;

    ptScrollPos.x=-1;
    ptScrollPos.y=SendMessage(hWnd, AEM_VPOSFROMLINE, AECT_GLOBAL, nBeforeLine);
    SendMessage(hWnd, AEM_SETSCROLLPOS, 0, (LPARAM)&ptScrollPos);
  }
  SendMessage(hWnd, AEM_UPDATECARET, 0, 0);
  SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(hWnd, NULL, TRUE);
}

BOOL GetWindowPos(HWND hWnd, HWND hWndOwner, RECT *rc)
{
  if (GetWindowRect(hWnd, rc))
  {
    rc->right-=rc->left;
    rc->bottom-=rc->top;

    if (hWndOwner)
    {
      if (!ScreenToClient(hWndOwner, (POINT *)&rc->left))
        return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

INT_PTR CopyWideStr(const wchar_t *wpSrc, INT_PTR nSrcLen, wchar_t **wppDst)
{
  wchar_t *wszDst=*wppDst;

  if (nSrcLen == -1)
    nSrcLen=xstrlenW(wpSrc);
  if (wszDst)
    FreeWideStr(wszDst);
  if (wszDst=(wchar_t *)GlobalAlloc(GMEM_FIXED, (nSrcLen + 1) * sizeof(wchar_t)))
    xstrcpynW(wszDst, wpSrc, nSrcLen + 1);
  *wppDst=wszDst;
  return nSrcLen;
}

BOOL FreeWideStr(wchar_t *wpWideStr)
{
  if (wpWideStr && GlobalFree((HGLOBAL)wpWideStr))
    return TRUE;
  return FALSE;
}


//// Options

INT_PTR WideOption(HANDLE hOptions, const wchar_t *pOptionName, DWORD dwType, BYTE *lpData, DWORD dwData)
{
  PLUGINOPTIONW po;

  if (!hOptions) return 0;
  po.pOptionName=pOptionName;
  po.dwType=dwType;
  po.lpData=lpData;
  po.dwData=dwData;
  return SendMessage(hMainWnd, AKD_OPTIONW, (WPARAM)hOptions, (LPARAM)&po);
}

void ReadOptions(DWORD dwFlags)
{
  HANDLE hOptions;

  if (hOptions=(HANDLE)SendMessage(hMainWnd, AKD_BEGINOPTIONSW, POB_READ, (LPARAM)wszPluginName))
  {
    WideOption(hOptions, L"/WindowRect", PO_BINARY, (LPBYTE)&rcMainCurrentDialog, sizeof(RECT));
    WideOption(hOptions, L"/Columns", PO_BINARY, (LPBYTE)&lpColumns, sizeof(lpColumns));
    WideOption(hOptions, L"/ContentBuffer", PO_BINARY, (LPBYTE)&nContentBuffer, sizeof(DWORD));
    WideOption(hOptions, L"/GlobalDebugJIT", PO_DWORD, (LPBYTE)&dwGlobalDebugJIT, sizeof(DWORD));
    WideOption(hOptions, L"/GlobalDebugEnable", PO_DWORD, (LPBYTE)&bGlobalDebugEnable, sizeof(DWORD));
    WideOption(hOptions, L"/GlobalDebugCode", PO_DWORD, (LPBYTE)&dwGlobalDebugCode, sizeof(DWORD));
    WideOption(hOptions, L"/LastScript", PO_STRING, (LPBYTE)wszLastScript, MAX_PATH * sizeof(wchar_t));

    //Scripts hotkeys
    {
      WIN32_FIND_DATAW wfd;
      HANDLE hFind;
      wchar_t wszFindFiles[MAX_PATH];
      const wchar_t *wpExt;

      xprintfW(wszFindFiles, L"%s\\*.*", wszScriptsDir);

      if ((hFind=FindFirstFileWide(wszFindFiles, &wfd)) != INVALID_HANDLE_VALUE)
      {
        do
        {
          if (wfd.cFileName[0] == '.' && (wfd.cFileName[1] == '\0' || (wfd.cFileName[1] == '.' && wfd.cFileName[2] == '\0'))) continue;

          if (wpExt=GetFileExt(wfd.cFileName, -1))
          {
            if (!xstrcmpiW(wpExt, L"js") ||
                !xstrcmpiW(wpExt, L"vbs"))
            {
              DWORD dwHotkey=0;

              if (hOptions)
                WideOption(hOptions, wfd.cFileName, PO_DWORD, (LPBYTE)&dwHotkey, sizeof(DWORD));
              if (dwHotkey)
                RegisterHotkey(wfd.cFileName, LOWORD(dwHotkey));
            }
          }
        }
        while (FindNextFileWide(hFind, &wfd));

        FindClose(hFind);
      }
    }

    SendMessage(hMainWnd, AKD_ENDOPTIONS, (WPARAM)hOptions, 0);
  }
}

void SaveOptions(DWORD dwFlags)
{
  HANDLE hOptions;

  if (hOptions=(HANDLE)SendMessage(hMainWnd, AKD_BEGINOPTIONSW, POB_SAVE|((dwFlags & OF_HOTKEYS)?POB_CLEAR:0), (LPARAM)wszPluginName))
  {
    if (dwFlags & OF_RECT)
    {
      WideOption(hOptions, L"/WindowRect", PO_BINARY, (LPBYTE)&rcMainCurrentDialog, sizeof(RECT));
    }
    if (dwFlags & OF_COLUMNS)
    {
      WideOption(hOptions, L"/Columns", PO_BINARY, (LPBYTE)&lpColumns, sizeof(lpColumns));
      WideOption(hOptions, L"/ContentBuffer", PO_BINARY, (LPBYTE)&nContentBuffer, sizeof(DWORD));
    }
    if (dwFlags & OF_DEBUG)
    {
      WideOption(hOptions, L"/GlobalDebugJIT", PO_DWORD, (LPBYTE)&dwGlobalDebugJIT, sizeof(DWORD));
      WideOption(hOptions, L"/GlobalDebugEnable", PO_DWORD, (LPBYTE)&bGlobalDebugEnable, sizeof(DWORD));
      WideOption(hOptions, L"/GlobalDebugCode", PO_DWORD, (LPBYTE)&dwGlobalDebugCode, sizeof(DWORD));
    }
    if (dwFlags & OF_LASTSCRIPT)
    {
      WideOption(hOptions, L"/LastScript", PO_STRING, (LPBYTE)wszLastScript, ((int)xstrlenW(wszLastScript) + 1) * sizeof(wchar_t));
    }
    if (dwFlags & OF_HOTKEYS)
    {
      WIN32_FIND_DATAW wfd;
      HANDLE hFind;
      wchar_t wszFindFiles[MAX_PATH];
      const wchar_t *wpExt;

      //Script files
      xprintfW(wszFindFiles, L"%s\\*.*", wszScriptsDir);

      if ((hFind=FindFirstFileWide(wszFindFiles, &wfd)) != INVALID_HANDLE_VALUE)
      {
        do
        {
          if (wfd.cFileName[0] == '.' && (wfd.cFileName[1] == '\0' || (wfd.cFileName[1] == '.' && wfd.cFileName[2] == '\0'))) continue;

          if (wpExt=GetFileExt(wfd.cFileName, -1))
          {
            if (!xstrcmpiW(wpExt, L"js") || !xstrcmpiW(wpExt, L"vbs"))
            {
              //Find hotkey
              PLUGINFUNCTION *pfElement=NULL;

              xprintfW(wszBuffer, L"Scripts::Main::%s", wfd.cFileName);
              pfElement=(PLUGINFUNCTION *)SendMessage(hMainWnd, AKD_DLLFINDW, (WPARAM)wszBuffer, 0);

              //Save hotkey
              if (pfElement)
              {
                WideOption(hOptions, wfd.cFileName, PO_DWORD, (LPBYTE)&pfElement->wHotkey, sizeof(WORD));
              }
            }
          }
        }
        while (FindNextFileWide(hFind, &wfd));

        FindClose(hFind);
      }
    }

    SendMessage(hMainWnd, AKD_ENDOPTIONS, (WPARAM)hOptions, 0);
  }
}

const char* GetLangStringA(LANGID wLangID, int nStringID)
{
  static char szStringBuf[MAX_PATH];

  WideCharToMultiByte(CP_ACP, 0, GetLangStringW(wLangID, nStringID), -1, szStringBuf, MAX_PATH, NULL, NULL);
  return szStringBuf;
}

const wchar_t* GetLangStringW(LANGID wLangID, int nStringID)
{
  wLangID=PRIMARYLANGID(wLangID);

  if (wLangID == LANG_RUSSIAN)
  {
    if (nStringID == STRID_ARCHITECTURE_MISMATCH)
      return L"\x0410\x0440\x0445\x0438\x0442\x0435\x043A\x0442\x0443\x0440\x0430\x0020\x043F\x043B\x0430\x0433\x0438\x043D\x0430\x0020\x0438\x0020\x0041\x006B\x0065\x006C\x0050\x0061\x0064\x0027\x0430\x0020\x043D\x0435\x0020\x0441\x043E\x0432\x043C\x0435\x0441\x0442\x0438\x043C\x044B\x002E";
    if (nStringID == STRID_HOTKEY_EXISTS)
      return L"%s\n\n\x0413\x043E\x0440\x044F\x0447\x0430\x044F\x0020\x043A\x043B\x0430\x0432\x0438\x0448\x0430\x0020\x0443\x0436\x0435\x0020\x043D\x0430\x0437\x043D\x0430\x0447\x0435\x043D\x0430\x0020\x0434\x043B\x044F \"%s\".";
    if (nStringID == STRID_EXECUTE_ERROR)
      return L"\x041E\x0448\x0438\x0431\x043A\x0430\x0020\x043F\x0440\x0438\x0020\x043F\x043E\x043F\x044B\x0442\x043A\x0435\x0020\x0437\x0430\x043F\x0443\x0441\x0442\x0438\x0442\x044C\x0020\x043A\x043E\x0434\x0020\x0441\x043A\x0440\x0438\x043F\x0442\x0430\x002E";
    if (nStringID == STRID_ENGINE_ERROR)
      return L"\x0421\x0435\x0440\x0432\x0435\x0440\x0020\x0441\x0446\x0435\x043D\x0430\x0440\x0438\x0435\x0432\x0020\x0434\x043B\x044F\x0020\x0442\x0438\x043F\x0430\x0020\x0444\x0430\x0439\x043B\x043E\x0432 \"%s\" \x043D\x0435\x0020\x043D\x0430\x0439\x0434\x0435\x043D\x002E";
    if (nStringID == STRID_READFILE_ERROR)
      return L"\x041D\x0435\x0020\x0443\x0434\x0430\x0435\x0442\x0441\x044F\x0020\x043F\x0440\x043E\x0447\x0438\x0442\x0430\x0442\x044C\x0020\x0444\x0430\x0439\x043B \"%s\".";
    if (nStringID == STRID_DEBUG_ERROR)
      return L"\x041D\x0435\x0020\x0443\x0434\x0430\x0435\x0442\x0441\x044F\x0020\x0438\x043D\x0438\x0446\x0438\x0430\x043B\x0438\x0437\x0438\x0440\x043E\x0432\x0430\x0442\x044C\x0020\x043F\x0440\x0438\x043B\x043E\x0436\x0435\x043D\x0438\x0435\x0020\x0434\x043B\x044F\x0020\x043E\x0442\x043B\x0430\x0434\x043A\x0438\x002E";
    if (nStringID == STRID_SCRIPTERROR)
      return L"\x0421\x043A\x0440\x0438\x043F\x0442:\t%s\n\x0421\x0442\x0440\x043E\x043A\x0430:\t%u\n\x0421\x0438\x043C\x0432\x043E\x043B\x003A\t%d\n\x041E\x0448\x0438\x0431\x043A\x0430:\t%s\n\x041A\x043E\x0434:\t%X\n\x0418\x0441\x0442\x043E\x0447\x043D\x0438\x043A:\t%s\n";
    if (nStringID == STRID_STOP)
      return L"\x041E\x0441\x0442\x0430\x043D\x043E\x0432\x0438\x0442\x044C";
    if (nStringID == STRID_CONTINUE)
      return L"\x041F\x0440\x043E\x0434\x043E\x043B\x0436\x0438\x0442\x044C";
    if (nStringID == STRID_DEBUG)
      return L"\x041E\x0442\x043B\x0430\x0434\x043A\x0430";
    if (nStringID == STRID_JIT)
      return L"JIT";
    if (nStringID == STRID_FROMSTART)
      return L"\x0421\x0020\x043D\x0430\x0447\x0430\x043B\x0430";
    if (nStringID == STRID_CODE)
      return L"\x041A\x043E\x0434";
    if (nStringID == STRID_CODE_MEMREAD)
      return L"\x041E\x0442\x043B\x0430\x0434\x0438\x0442\x044C\x0020\x0447\x0442\x0435\x043D\x0438\x0435\x0020\x043F\x0430\x043C\x044F\x0442\x0438";
    if (nStringID == STRID_CODE_MEMWRITE)
      return L"\x041E\x0442\x043B\x0430\x0434\x0438\x0442\x044C\x0020\x0437\x0430\x043F\x0438\x0441\x044C\x0020\x043F\x0430\x043C\x044F\x0442\x0438";
    if (nStringID == STRID_CODE_MEMFREE)
      return L"\x041E\x0442\x043B\x0430\x0434\x0438\x0442\x044C\x0020\x043E\x0441\x0432\x043E\x0431\x043E\x0436\x0434\x0435\x043D\x0438\x0435\x0020\x043F\x0430\x043C\x044F\x0442\x0438";
    if (nStringID == STRID_CODE_MEMLEAK)
      return L"\x041E\x0442\x043B\x0430\x0434\x0438\x0442\x044C\x0020\x0443\x0442\x0435\x0447\x043A\x0443\x0020\x043F\x0430\x043C\x044F\x0442\x0438";
    if (nStringID == STRID_CODE_SYSCALL)
      return L"\x041E\x0442\x043B\x0430\x0434\x0438\x0442\x044C\x0020\x0432\x044B\x0437\x043E\x0432\x0020\x0441\x0438\x0441\x0442\x0435\x043C\x043D\x044B\x0445\x0020\x0444\x0443\x043D\x043A\x0446\x0438\x0439";
    if (nStringID == STRID_DEBUG_MEMLOCATE)
      return L"\x0423\x043A\x0430\x0437\x0430\x0442\x0435\x043B\x044C (%d) \x043D\x0435\x0020\x043D\x0430\x0445\x043E\x0434\x0438\x0442\x0441\x044F\x0020\x0432\x0020\x043F\x0430\x043C\x044F\x0442\x0438\x002C\x0020\x0432\x044B\x0434\x0435\x043B\x0435\x043D\x043D\x043E\x0439\x0020\x0441\x0020\x043F\x043E\x043C\x043E\x0449\x044C\x044E AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMREAD)
      return L"\x0420\x0430\x0437\x043C\x0435\x0440\x0020\x0441\x0447\x0438\x0442\x044B\x0432\x0430\x0435\x043C\x044B\x0445\x0020\x0434\x0430\x043D\x043D\x044B\x0445\x0020\x0432\x044B\x0445\x043E\x0434\x0438\x0442\x0020\x0437\x0430\x0020\x043F\x0440\x0435\x0434\x0435\x043B\x044B\x0020\x043F\x0430\x043C\x044F\x0442\x0438\x002C\x0020\x0432\x044B\x0434\x0435\x043B\x0435\x043D\x043D\x043E\x0439\x0020\x0441\x0020\x043F\x043E\x043C\x043E\x0449\x044C\x044E AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMWRITE)
      return L"\x0420\x0430\x0437\x043C\x0435\x0440\x0020\x0437\x0430\x043F\x0438\x0441\x044B\x0432\x0430\x0435\x043C\x044B\x0445\x0020\x0434\x0430\x043D\x043D\x044B\x0445\x0020\x0432\x044B\x0445\x043E\x0434\x0438\x0442\x0020\x0437\x0430\x0020\x043F\x0440\x0435\x0434\x0435\x043B\x044B\x0020\x043F\x0430\x043C\x044F\x0442\x0438\x002C\x0020\x0432\x044B\x0434\x0435\x043B\x0435\x043D\x043D\x043E\x0439\x0020\x0441\x0020\x043F\x043E\x043C\x043E\x0449\x044C\x044E AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMFREE)
      return L"\x0423\x043A\x0430\x0437\x0430\x0442\x0435\x043B\x044C (%d) \x043D\x0435\x0020\x0431\x044B\x043B\x0020\x043F\x043E\x043B\x0443\x0447\x0435\x043D\x0020\x0441\x0020\x043F\x043E\x043C\x043E\x0449\x044C\x044E AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMLEAK)
      return L"\x041D\x0435\x0020\x0432\x044B\x0437\x0432\x0430\x043D\x043E AkelPad.MemFree \x0434\x043B\x044F (%d) \x043E\x0431\x044A\x0435\x043A\x0442\x043E\x0432\x002C\x0020\x043F\x043E\x043B\x0443\x0447\x0435\x043D\x043D\x044B\x0445\x0020\x0441\x0020\x043F\x043E\x043C\x043E\x0449\x044C\x044E\x0020\x0041\x006B\x0065\x006C\x0050\x0061\x0064\x002E\x004D\x0065\x006D\x0041\x006C\x006C\x006F\x0063\x002E";
    if (nStringID == STRID_DEBUG_SYSCALL)
      return L"\x041D\x0435\x0020\x043D\x0430\x0439\x0434\x0435\x043D\x0020\x0444\x0430\x0439\x043B \"%s\".";
    if (nStringID == STRID_DEBUG_SYSFUNCTION)
      return L"\x041D\x0435\x0020\x043D\x0430\x0439\x0434\x0435\x043D\x0430\x0020\x0444\x0443\x043D\x043A\x0446\x0438\x044F \"%s\" \x0432\x0020\x0444\x0430\x0439\x043B\x0435 \"%s\".";
    if (nStringID == STRID_COLUMNS)
      return L"\x041A\x043E\x043B\x043E\x043D\x043A\x0438";
    if (nStringID == STRID_BUFFER)
      return L"\x0411\x0443\x0444\x0435\x0440:";
    if (nStringID == STRID_MENU_OPENSITE)
      return L"\x041E\x0442\x043A\x0440\x044B\x0442\x044C\x0020\x0441\x0430\x0439\x0442";
    if (nStringID == STRID_MENU_ITEMMOVEUP)
      return L"\x041F\x0435\x0440\x0435\x043C\x0435\x0441\x0442\x0438\x0442\x044C\x0020\x0432\x0432\x0435\x0440\x0445\tAlt+Up";
    if (nStringID == STRID_MENU_ITEMMOVEDOWN)
      return L"\x041F\x0435\x0440\x0435\x043C\x0435\x0441\x0442\x0438\x0442\x044C\x0020\x0432\x043D\x0438\x0437\tAlt+Down";
    if (nStringID == STRID_SCRIPT)
      return L"\x0421\x043A\x0440\x0438\x043F\x0442";
    if (nStringID == STRID_HOTKEY)
      return L"\x0413\x043E\x0440\x044F\x0447\x0430\x044F\x0020\x043A\x043B\x0430\x0432\x0438\x0448\x0430";
    if (nStringID == STRID_STATUS)
      return L"\x0421\x0442\x0430\x0442\x0443\x0441";
    if (nStringID == STRID_VERSION)
      return L"\x0412\x0435\x0440\x0441\x0438\x044F";
    if (nStringID == STRID_AUTHOR)
      return L"\x0410\x0432\x0442\x043E\x0440";
    if (nStringID == STRID_DESCRIPTION)
      return L"\x041E\x043F\x0438\x0441\x0430\x043D\x0438\x0435";
    if (nStringID == STRID_SITE)
      return L"\x0421\x0430\x0439\x0442";
    if (nStringID == STRID_RUNNING)
      return L"\x0420\x0430\x0431\x043E\x0442\x0430\x0435\x0442";
    if (nStringID == STRID_WAITING)
      return L"\x041E\x0436\x0438\x0434\x0430\x0435\x0442";
    if (nStringID == STRID_CONTAIN)
      return L"\x0421\x043E\x0434\x0435\x0440\x0436\x0438\x0442";
    if (nStringID == STRID_EXEC)
      return L"\x0417\x0430\x043F\x0443\x0441\x0442\x0438\x0442\x044C";
    if (nStringID == STRID_EDIT)
      return L"\x0418\x0437\x043C\x0435\x043D\x0438\x0442\x044C";
    if (nStringID == STRID_ASSIGN)
      return L"\x041D\x0430\x0437\x043D\x0430\x0447\x0438\x0442\x044C";
    if (nStringID == STRID_PLUGIN)
      return L"%s \x043F\x043B\x0430\x0433\x0438\x043D";
    if (nStringID == STRID_OK)
      return L"\x004F\x004B";
    if (nStringID == STRID_CANCEL)
      return L"\x041E\x0442\x043C\x0435\x043D\x0430";
    if (nStringID == STRID_CLOSE)
      return L"\x0417\x0430\x043A\x0440\x044B\x0442\x044C";
  }
  else
  {
    if (nStringID == STRID_ARCHITECTURE_MISMATCH)
      return L"Architecture of the plugin and the AkelPad aren't compatible.";
    if (nStringID == STRID_HOTKEY_EXISTS)
      return L"%s\n\nHotkey already assigned to \"%s\".";
    if (nStringID == STRID_EXECUTE_ERROR)
      return L"Can't execute script code.";
    if (nStringID == STRID_ENGINE_ERROR)
      return L"Script engine for file type \"%s\" not found.";
    if (nStringID == STRID_READFILE_ERROR)
      return L"Can't read file \"%s\".";
    if (nStringID == STRID_DEBUG_ERROR)
      return L"Can't initialize debug IDE.";
    if (nStringID == STRID_SCRIPTERROR)
      return L"Script:\t%s\nLine:\t%u\nSymbol:\t%d\nError:\t%s\nCode:\t%X\nSource:\t%s\n";
    if (nStringID == STRID_STOP)
      return L"Stop";
    if (nStringID == STRID_CONTINUE)
      return L"Continue";
    if (nStringID == STRID_DEBUG)
      return L"Debug";
    if (nStringID == STRID_JIT)
      return L"JIT";
    if (nStringID == STRID_FROMSTART)
      return L"From start";
    if (nStringID == STRID_CODE)
      return L"Code";
    if (nStringID == STRID_CODE_MEMREAD)
      return L"Debug memory read";
    if (nStringID == STRID_CODE_MEMWRITE)
      return L"Debug memory write";
    if (nStringID == STRID_CODE_MEMFREE)
      return L"Debug memory free";
    if (nStringID == STRID_CODE_MEMLEAK)
      return L"Debug memory leak";
    if (nStringID == STRID_CODE_SYSCALL)
      return L"Debug system function call";
    if (nStringID == STRID_DEBUG_MEMLOCATE)
      return L"Pointer (%d) is not located in memory that was allocated with AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMREAD)
      return L"Data read size is out of the memory allocated with AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMWRITE)
      return L"Data write size is out of the memory allocated with AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMFREE)
      return L"Pointer (%d) has not been received with AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_MEMLEAK)
      return L"AkelPad.MemFree is not called for (%d) objects received with AkelPad.MemAlloc.";
    if (nStringID == STRID_DEBUG_SYSCALL)
      return L"Cannot find file \"%s\".";
    if (nStringID == STRID_DEBUG_SYSFUNCTION)
      return L"Cannot find function \"%s\" in file \"%s\".";
    if (nStringID == STRID_COLUMNS)
      return L"Columns";
    if (nStringID == STRID_BUFFER)
      return L"Buffer:";
    if (nStringID == STRID_MENU_OPENSITE)
      return L"Open site";
    if (nStringID == STRID_MENU_ITEMMOVEUP)
      return L"Move up\tAlt+Up";
    if (nStringID == STRID_MENU_ITEMMOVEDOWN)
      return L"Move down\tAlt+Down";
    if (nStringID == STRID_SCRIPT)
      return L"Script";
    if (nStringID == STRID_HOTKEY)
      return L"Hotkey";
    if (nStringID == STRID_STATUS)
      return L"State";
    if (nStringID == STRID_VERSION)
      return L"Version";
    if (nStringID == STRID_AUTHOR)
      return L"Author";
    if (nStringID == STRID_DESCRIPTION)
      return L"Description";
    if (nStringID == STRID_SITE)
      return L"Site";
    if (nStringID == STRID_RUNNING)
      return L"Running";
    if (nStringID == STRID_WAITING)
      return L"Waiting";
    if (nStringID == STRID_CONTAIN)
      return L"Contain";
    if (nStringID == STRID_EXEC)
      return L"Exec";
    if (nStringID == STRID_EDIT)
      return L"Edit";
    if (nStringID == STRID_ASSIGN)
      return L"Assign";
    if (nStringID == STRID_PLUGIN)
      return L"%s plugin";
    if (nStringID == STRID_OK)
      return L"OK";
    if (nStringID == STRID_CANCEL)
      return L"Cancel";
    if (nStringID == STRID_CLOSE)
      return L"Close";
  }
  return L"";
}

BOOL IsExtCallParamValid(LPARAM lParam, int nIndex)
{
  if (*((INT_PTR *)lParam) >= (INT_PTR)((nIndex + 1) * sizeof(INT_PTR)))
    return TRUE;
  return FALSE;
}

INT_PTR GetExtCallParam(LPARAM lParam, int nIndex)
{
  if (*((INT_PTR *)lParam) >= (INT_PTR)((nIndex + 1) * sizeof(INT_PTR)))
    return *(((INT_PTR *)lParam) + nIndex);
  return 0;
}

void InitCommon(PLUGINDATA *pd)
{
  bInitCommon=TRUE;
  hInstanceEXE=pd->hInstanceEXE;
  hInstanceDLL=pd->hInstanceDLL;
  hMainWnd=pd->hMainWnd;
  hGlobalAccel=pd->hGlobalAccel;
  bOldWindows=pd->bOldWindows;
  bOldRichEdit=pd->bOldRichEdit;
  bOldComctl32=pd->bOldComctl32;
  bAkelEdit=pd->bAkelEdit;
  nMDI=pd->nMDI;
  wLangModule=pd->wLangModule;

  //Initialize WideFunc.h header
  WideInitialize();

  //Plugin name
  {
    int i;

    for (i=0; pd->wszFunction[i] != L':'; ++i)
      wszPluginName[i]=pd->wszFunction[i];
    wszPluginName[i]=L'\0';
  }
  xprintfW(wszPluginTitle, GetLangStringW(wLangModule, STRID_PLUGIN), wszPluginName);
  xprintfW(wszScriptsDir, L"%s\\AkelFiles\\Plugs\\Scripts", pd->wszAkelDir);
  xstrcpynW(wszAkelPadDir, pd->wszAkelDir, MAX_PATH);
  ReadOptions(0);
}

void InitMain()
{
  bInitMain=TRUE;

  //Load plugin icon
  g_hPluginIcon=LoadIconA(hInstanceDLL, MAKEINTRESOURCEA(IDI_ICON_PLUGIN));

  //SubClass
  NewMainProcData=NULL;
  SendMessage(hMainWnd, AKD_SETMAINPROC, (WPARAM)NewMainProc, (LPARAM)&NewMainProcData);
}

void UninitMain()
{
  bInitMain=FALSE;

  //Destroy plugin icon
  if (g_hPluginIcon) DestroyIcon(g_hPluginIcon);

  if (NewMainProcData)
  {
    SendMessage(hMainWnd, AKD_SETMAINPROC, (WPARAM)NULL, (LPARAM)&NewMainProcData);
    NewMainProcData=NULL;
  }
}


//Entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
  {
    hInstanceDLL=hinstDLL;
    MyIClassFactoryObj.lpVtbl=NULL;
    DisableThreadLibraryCalls(hinstDLL);
  }
  else if (fdwReason == DLL_THREAD_ATTACH)
  {
  }
  else if (fdwReason == DLL_THREAD_DETACH)
  {
  }
  else if (fdwReason == DLL_PROCESS_DETACH)
  {
    //ActiveXObject
    if (MyIClassFactoryObj.lpVtbl)
    {
      if (lpScriptThreadActiveX)
      {
        FreeScriptResources(lpScriptThreadActiveX, (lpScriptThreadActiveX->dwDebug & DBG_MEMLEAK));
        lpScriptThreadActiveX=NULL;
      }
      StackFreeCallbacks(&g_hHookCallbackStack);
      StackFreeCallbacks(&g_hSysCallbackStack);
      StackFreeCallbacks(&g_hSubclassCallbackStack);
      if (g_WScriptTypeInfo) g_WScriptTypeInfo->lpVtbl->Release(g_WScriptTypeInfo);
      if (g_WArgumentsTypeInfo) g_WArgumentsTypeInfo->lpVtbl->Release(g_WArgumentsTypeInfo);
      if (g_DocumentTypeInfo) g_DocumentTypeInfo->lpVtbl->Release(g_DocumentTypeInfo);
      if (g_ScriptSettingsTypeInfo) g_ScriptSettingsTypeInfo->lpVtbl->Release(g_ScriptSettingsTypeInfo);
      if (g_SystemFunctionTypeInfo) g_SystemFunctionTypeInfo->lpVtbl->Release(g_SystemFunctionTypeInfo);
      if (g_ConstantsTypeInfo) g_ConstantsTypeInfo->lpVtbl->Release(g_ConstantsTypeInfo);
    }
  }
  return TRUE;
}
