echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq Renoise.exe" |find /i "Renoise.exe" > nul
if errorlevel 1 START .\GN.renoise.bat
