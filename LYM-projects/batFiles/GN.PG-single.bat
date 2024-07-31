echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq Renoise.exe" |find /i "Renoise.exe" > nul
if errorlevel 1 START .\GN.renoise.bat

PG-single.bat LYM_GN-scenario-v1.csv
