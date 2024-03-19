echo off

cd C:\home\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\home\LYM-projects\
REM copy C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Ada.exe
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/configuration/YN_Ada-head-single.csv ./Projects/scenarios/YN_Ada-scenario-v2.csv
