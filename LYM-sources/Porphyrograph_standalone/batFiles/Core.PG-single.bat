REM echo off

tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd ..\
.\Release\Porphyrograph_core_Core.exe ./Projects/configuration/YN_Core-head-single.csv ./Projects/scenarios/YN_Core-scenario-v1.csv
