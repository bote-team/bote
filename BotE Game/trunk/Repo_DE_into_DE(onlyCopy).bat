echo off
echo *
echo For next SVN-Update it is neccessary to let original files stored and to make a copy of BotE anywhere else
echo *
echo *** This will overwrite all files in c:\BotESVNCOPY_DE (deutsch) - ok? (or STOP with STRG+C) ***
echo *
pause

xcopy game\*.* c:\BotESVNCOPY_DE\game\*.* /Y /S
xcopy *.* c:\BotESVNCOPY_DE\*.* /Y
xcopy batch\*.* c:\BotESVNCOPY_EN\*.* /Y

echo on
pause
cd c:\BotESVNCOPY_DE
explorer .
