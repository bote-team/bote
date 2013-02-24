echo off
echo *
echo For next SVN-Update it is neccessary to let original files stored and to make a copy of BotE anywhere else
echo *
echo *** This will overwrite all files in c:\BotESVNCOPY_EN - ok? (or STOP with STRG+C) ***
echo *
pause

xcopy game\*.* c:\BotESVNCOPY_EN\game\*.* /Y /S
xcopy *.* c:\BotESVNCOPY_EN\*.* /Y
xcopy batch\*.* c:\BotESVNCOPY_EN\*.* /Y

Echo *** now turns German into English ***
xcopy c:\BotESVNCOPY_EN\game\Data\Buildings\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Buildings\*.* /Y /S
xcopy c:\BotESVNCOPY_EN\game\Data\Names\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Names\*.* /Y /S
xcopy c:\BotESVNCOPY_EN\game\Data\Other\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Other\*.* /Y /S
xcopy c:\BotESVNCOPY_EN\game\Data\Races\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Races\*.* /Y /S
xcopy c:\BotESVNCOPY_EN\game\Data\Ships\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Ships\*.* /Y /S
xcopy c:\BotESVNCOPY_EN\game\Data\Strings\[en]\*.* c:\BotESVNCOPY_EN\game\Data\Strings\*.* /Y /S
echo on
pause
cd c:\BotESVNCOPY_EN
explorer .
