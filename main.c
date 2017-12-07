#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

#include <windows.h>
#include <wchar.h>
#include <stdbool.h>
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>

#define ID_QUIT 1

VOID CALLBACK WinEventProc ( HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void ShowAndSetForeground(HWND hwnd)
{
    WINDOWPLACEMENT place;
    memset(&place, 0, sizeof(WINDOWPLACEMENT));
    place.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, &place);

    switch (place.showCmd)
    {
    case SW_SHOWMAXIMIZED:
        ShowWindow(hwnd, SW_SHOWMAXIMIZED);
        break;
    case SW_SHOWMINIMIZED:
        ShowWindow(hwnd, SW_RESTORE);
        break;
    default:
        ShowWindow(hwnd, SW_NORMAL);
        break;
    }

    SetForegroundWindow(hwnd);
}

// TRAY stars.

#define WM_TRAYMESSAGE WM_USER
static VOID ShowNotifyIcon(HWND hWnd, BOOL addOrHide)
{
  NOTIFYICONDATA nid;
  ZeroMemory(&nid,sizeof(nid));
  nid.cbSize=sizeof(NOTIFYICONDATA);
  nid.hWnd=hWnd;
  nid.uID=0;
  nid.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage=WM_TRAYMESSAGE;
  nid.hIcon=LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(MAIN_ICON));
  lstrcpy(nid.szTip,TEXT("Click to maxmize!"));

  if(addOrHide)
    Shell_NotifyIcon(NIM_ADD,&nid);
  else
    Shell_NotifyIcon(NIM_DELETE,&nid);
};

// TRAY ends.

HWND mainWindowHandle;
HWND consoleWindowHandle;
const int mainWindowWidth = 640;
const int mainWindowHeight = 270;

void HideToTray() {

  ShowWindow(consoleWindowHandle, SW_HIDE);
  ShowWindow(consoleWindowHandle, SW_HIDE);
  ShowNotifyIcon(mainWindowHandle, true);

}

void ShowFromTray() {

  ShowNotifyIcon(mainWindowHandle, false);
  ShowWindow(consoleWindowHandle, SW_SHOW | SW_MAXIMIZE);
  ShowWindow(consoleWindowHandle, SW_SHOW | SW_MAXIMIZE);
  // PostMessage(consoleWindowHandle, WM_SYSCOMMAND, SC_RESTORE, 0);
  SendMessage(consoleWindowHandle, WM_SYSCOMMAND, SC_RESTORE, 0);
  ShowAndSetForeground(consoleWindowHandle);
  SetActiveWindow(consoleWindowHandle);
  SetForegroundWindow(consoleWindowHandle);

}

int main(void) {

    MSG msg;
    WNDCLASSW wc = {0};
    wc.lpszClassName = L"Main window";
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.lpszMenuName  = NULL;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassW(&wc);

    mainWindowHandle = CreateWindowW(wc.lpszClassName, L"GoodbyeDPI",
                  WS_DISABLED
                  ,
                  100, 100, mainWindowWidth, mainWindowHeight, 0, 0, GetModuleHandle(0), 0);

    consoleWindowHandle = GetConsoleWindow();

    SetWinEventHook(
      EVENT_SYSTEM_MINIMIZESTART,
      EVENT_SYSTEM_MINIMIZEEND,
      GetModuleHandle(NULL),
      WinEventProc,
      0,
      0,
      WINEVENT_INCONTEXT
    );

    puts("Minimize me!");

    while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // return (int) msg.wParam;
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
  WPARAM wParam, LPARAM lParam) {

  static bool ifIconRemovalDefered = false;

  switch(msg) {

    case WM_CREATE: {

      CreateWindowW(L"Button", L"Exit",
        WS_VISIBLE | WS_CHILD,
        20, 50, 80, 25, hwnd, (HMENU) ID_QUIT, NULL, NULL);

      break;
    }
    case WM_COMMAND:

      if (LOWORD(wParam) == ID_QUIT) { // EXIT button.

        PostQuitMessage(0);

      }
      break;

    // TRAY starts.

    case WM_SYSCOMMAND:
      if ((wParam & 0xFFF0) == SC_MINIMIZE)
      {

          // HideToTray();
          // TODO: What is this below?
          // Return TRUE to tell DefDlgProc that we handled the message, and set
          // DWL_MSGRESULT to 0 to say that we handled WM_SYSCOMMAND
          SetWindowLong(mainWindowHandle, DWL_MSGRESULT, 0);

          return 0;
      }
      break;

    case WM_TRAYMESSAGE:
      switch(lParam) {

        case WM_LBUTTONUP:
        // The user has previously double-clicked the icon, remove it in
        // response to this second WM_LBUTTONUP
        ShowFromTray();
        return true;
      }
      break;

    // TRAY ends.

    case WM_DESTROY:

        PostQuitMessage(0);

      break;

  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);

};

void CALLBACK WinEventProc(
   HWINEVENTHOOK hWinEventHook,
   DWORD         event,
   HWND          hwnd,
   LONG          idObject,
   LONG          idChild,
   DWORD         dwEventThread,
   DWORD         dwmsEventTime
) {

  if (event == EVENT_SYSTEM_MINIMIZESTART) {

    if (hwnd != GetConsoleWindow()) {
      return;
    }
    HideToTray();
  }

}
